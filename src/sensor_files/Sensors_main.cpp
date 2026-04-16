#include "Sensors_main.hpp"
#include <cmath>


// Needed for ISR static access
static Sensors* g_instance = nullptr;

Sensors::Sensors(int echo, int trig)
: ultrasonic(echo, trig, LOG_LEVEL_WARNING)
{}

// ================= INIT =================
void Sensors::init() {
    Serial.begin(115200);

    Wire.begin(26, 27);
    Wire1.begin(23, 22);

    // ================= IMU =================
    if (!imu.begin(Wire1, 0x69)) {
        Serial.println("IMU begin failed");
    }

    imu.applyBasicDefaults();

    Serial.print("WHO_AM_I = 0x");
    Serial.println(imu.readWhoAmI(), HEX);

    calculateIMUBias();

    // ================= ULTRASONIC =================

    ultrasonic.begin(Serial);


    // ================= TOF =================
    if (!imager.begin(0x29, Wire))
        Serial.println("ToF Fail");
    else
        Serial.println("ToF PASS");

    imager.setResolution(4 * 4);
    imager.setRangingFrequency(60);
    imager.setIntegrationTime(15);
    imager.startRanging();

    // ================= IMU TASK =================
    xTaskCreatePinnedToCore(
        imuTaskWrapper,
        "IMU Task",
        4096,
        this,
        1,
        &imuTaskHandle,
        0
    );
}

// ================= IMU =================
void Sensors::IMUGetData(std::array<float, 9>& data) {
    float gx, gy, gz;
    float ax, ay, az;
    float mx, my, mz;

    imu.readGyro(gx, gy, gz);
    imu.readAccel(ax, ay, az);
    imu.readMag(mx, my, mz);

    data[0] = gx - gyro_bias_x;
    data[1] = gy - gyro_bias_y;
    data[2] = gz - gyro_bias_z;

    data[3] = (ax) * 10;
    data[4] = (ay) * 10;
    data[5] = (az) * 10;

    data[6] = mx;
    data[7] = my;
    data[8] = mz;
}

void Sensors::imuTaskWrapper(void* param) {
    static_cast<Sensors*>(param)->imuTask();
}

void Sensors::imuTask() {
    const TickType_t period = pdMS_TO_TICKS(1);
    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
        std::array<float, 9> imuData;
        IMUGetData(imuData);

        orientation.orientation(imuData);
        auto q = orientation.current_state();

        float newPitch =
            asin(2 * (q[0] * q[2] - q[3] * q[1])) * 180.0f / M_PI;

        portENTER_CRITICAL(&pitchMux);
        pitch = newPitch;
        portEXIT_CRITICAL(&pitchMux);

        vTaskDelayUntil(&lastWake, period);
    }
}

float Sensors::getPitch() {
    float p;
    portENTER_CRITICAL(&pitchMux);
    p = pitch;
    portEXIT_CRITICAL(&pitchMux);
    return p;
}

// ================= BIAS =================
void Sensors::calculateIMUBias() {
    const int N = 100;

    float gx, gy, gz;
    float ax, ay, az;

    for (int i = 0; i < N; i++) {
        imu.readGyro(gx, gy, gz);
        imu.readAccel(ax, ay, az);

        gyro_bias_x += gx;
        gyro_bias_y += gy;
        gyro_bias_z += gz;

        accel_bias_x += ax;
        accel_bias_y += ay;
        accel_bias_z += az;

        delay(10);
    }

    gyro_bias_x /= N;
    gyro_bias_y /= N;
    gyro_bias_z /= N;

    accel_bias_x /= N;
    accel_bias_y /= N;
    accel_bias_z = (accel_bias_z / N) - 0.981;
}

// ================= ULTRASONIC =================
int Sensors::getUltrasound() {
    static uint32_t last = 0;

    if (millis() - last < 60) {
        return x; // reuse last valid
    }

    last = millis();

    int d = ultrasonic.readDistance() * 10;

    if (d <= 0 || d > 6000) {
        glitchCount++;
        if (glitchCount == 1) {
            d = lastValidDistance;
        }
    } else {
        glitchCount = 0;
        lastValidDistance = d;
    }

    x = d;
    return x;
}

// ================= TOF =================
std::array<float,3> Sensors::getToF(float pitch_deg, int ultrasound_distance) {
    if (imager.isDataReady()) {
        if (imager.getRangingData(&measurementData)) {
            
            // VIEWER FOR TESTING 
            // Output JSON with distance and quaternion data
            Serial.print("{\"distances\":[");

            for (int i = 0; i < 16; i++) {
                // Distance in mm
                Serial.print(measurementData.distance_mm[i]);
                if (i < 15) Serial.print(",");
            }

            Serial.print("],\"status\":[");

            for (int i = 0; i < 16; i++) {
                // Target status (5 = valid, others = various error states)
                Serial.print(measurementData.target_status[i]);
                if (i < 15) Serial.print(",");
            }

            // Add quaternion (wxyz format) with 6 decimal places for accuracy
            Serial.print("],\"quat\":[");
            Serial.print(1.0, 6); Serial.print(",");
            Serial.print(0.0, 6); Serial.print(",");
            Serial.print(0.0, 6); Serial.print(",");
            Serial.print(0.0, 6);
            Serial.print("],\"v\":\"");
            Serial.print("0.1.0");
            Serial.println("\"}");
            // END VIEWING FOR TESTING

            float theta = pitch_deg * DEG2RAD_TOF;
            float cos_t = cosf(theta);
            float sin_t = sinf(theta);

            float Z[16];

            for (int y = 0; y < 16; y += 4) {
                int row = y / 4;
                float tan_v = tan_zone_v[row];

                for (int x = 0; x < 4; x++) {
                    int idx = x + y;

                    if (measurementData.target_status[idx] == 5) {
                        float dist = measurementData.distance_mm[idx];
                        float X = dist * (cos_t - tan_v * sin_t);

                        Z[idx] = (X > Max_Upward_mm)
                            ? DUMMY_Z
                            : dist * (sin_t + tan_v * cos_t);
                    } else {
                        Z[idx] = DUMMY_Z;
                    }
                }
            }

            float min_left = DUMMY_Z;
            float min_mid = DUMMY_Z;
            float min_right = DUMMY_Z;

            for (int y = 0; y < 16; y += 4) {
                if (Z[3+y] < min_left)  min_left = Z[3+y];
                if (Z[2+y] < min_mid)   min_mid = Z[2+y];
                if (Z[1+y] < min_mid)   min_mid = Z[1+y];
                if (Z[0+y] < min_right) min_right = Z[0+y];
            }

            min_left = (min_left == 100000) ? ultrasound_distance : min_left;
            min_mid = (min_mid == 100000) ? ultrasound_distance : min_mid;
            min_right = (min_right == 100000) ? ultrasound_distance : min_right;

            ToF_out = {min_left, min_mid, min_right};
        }
    }

    return ToF_out;
}
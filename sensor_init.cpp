#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include "sensor_init.hpp"
#include <SparkFun_VL53L5CX_Library.h>
#include <jsnsr04t.h>
#include <Wire.h>

#define SDA_1 17
#define SCL_1 16
#define SDA_2 21
#define SCL_2 22
#define ECHO_PIN 2
#define TRIGGER_PIN 4

//const uint8_t TOF_ADDR = 0x52;


Adafruit_BNO055 IMU = Adafruit_BNO055(55);
SparkFun_VL53L5CX Imager;
JsnSr04T ultrasonicSensor(ECHO_PIN, TRIGGER_PIN, LOG_LEVEL_VERBOSE);
AsyncDelay measureDelay;
VL53L5CX_ResultsData measurementData;


void sensors::init(){
    Serial.println("TEST HERE");
    ultrasonicSensor.begin(Serial);
    measureDelay.start(1000, AsyncDelay::MILLIS);
    delay(1000);
    // Initialize Wire1 and Wire2 for I2C Connection
    Wire.begin(SDA_1, SCL_1);
    Wire1.begin(SDA_2, SCL_2);
    // IMU Start
    if(!IMU.begin())Serial.print("BNO FAIL");
    else Serial.println("IMU PASS");

    // ToF Imager Start
    if(!Imager.begin(0x29, Wire1))Serial.print("ToF Fail");
    else Serial.println("ToF PASS");

    // ToF Operating Setup
    Imager.setResolution(4*4);
    Imager.setRangingFrequency(1);
    Imager.setIntegrationTime(15);
    Imager.startRanging();

    // Gyroscope and Accelerometer Calibration
    gyro_calibration();
    Serial.println("Gyroscope calibrated");
    accel_calibration();
    Serial.println("Accelerometer calibrated");
}

const std::array<float, 3>& sensors::getGyr(){
    imu::Vector<3> gyr_vec = IMU.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    for (int i=0; i<3; i++) gyro[i] = gyr_vec[i] - gyro_bias[i];
    return gyro;
}

const std::array<float, 3>& sensors::getAcc(){
    imu::Vector<3> acc_vec = IMU.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    std::array<float,3> accel_new;
    for (int i=0; i<3; i++) accel[i] = (acc_vec[i]-accel_bias[i]);
    return accel;
}

const std::array<float, 3>& sensors::getMag(){
    imu::Vector<3> mag_vec = IMU.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
    for (int i=0; i<3; i++) mag[i] = mag_vec[i] - mag_bias[i];
    return mag;
}

const std::array<float, 9>& sensors::getIMU(){
    gyro = getGyr();
    accel = getAcc();
    mag = getMag();

    for (int i=0; i<3; i++){
        IMUSensor[i] = gyro[i];
        IMUSensor[i+3] = accel[i];
        IMUSensor[i+6] = mag[i];
    };
    return IMUSensor;
}

void sensors::gyro_calibration(){
    for (int i=0; i<1000; i++){
        auto calibration_gyr = IMU.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

        for (int i=0; i<3; i++) gyro_bias[i] += calibration_gyr[i];
        delay(1);
    }
    for (int i=0; i<3; i++) gyro_bias[i] /= 1000;
}

void sensors::accel_calibration(){
    std::array<float,3> sum{0,0,0};

    for(int k=0;k<500;k++){
        auto a = IMU.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        for(int i=0;i<3;i++) sum[i] += a[i];
        delay(2);
    }

    for(int i=0;i<3;i++)
        accel_bias[i] = sum[i] / 500.0;
    accel_bias[2] -= 9.81;
}

int lastValidDistance = 0;
int glitchCount = 0; // Counts how many "bad" samples we've seen in a row

int sensors::ultrasound() {
    if (measureDelay.isExpired()) {
        measureDelay.repeat();

       x  = ultrasonicSensor.readDistance() * 10;

        // Check if the reading is "bad"
        if (x >= 500 || x <= 0) {
            glitchCount++; // We found a potential glitch

            if (glitchCount == 1) {
                // STRIKE ONE: Use the previous value instead of the 505
                x = lastValidDistance;
            }
            // If glitchCount is 2 or more, we stop filtering and let the 505 through
        } else {
            // If the reading is GOOD, reset the glitch counter
            glitchCount = 0;
            lastValidDistance = x;
        }
    }
    return x;
}
// Precomputed zone constants — fixed by sensor geometry, never recalculated
// 4x4 resolution, 45° per-axis FoV: angle_step = 45°/4 = 11.25°
// Zone center offsets: ±1.5 and ±0.5 steps → 16.875°, 5.625°, -5.625°, -16.875°
static constexpr float DEG2RAD_TOF    = 3.14159265f / 180.0f;
static constexpr float zone_v_deg[4]  = { 16.875f,  5.625f, -5.625f, -16.875f };
static constexpr float tan_zone_v[4]  = { 0.30335f, 0.09849f, -0.09849f, -0.30335f };
static constexpr float Max_Upward_mm = 1200.0f;
static constexpr float DUMMY_Z        = 100000.0f;

std::array<float,3> sensors::time_of_flight(float pitch_deg){
    if (Imager.isDataReady()){
        if (Imager.getRangingData(&measurementData)){
            float theta_rad = pitch_deg * DEG2RAD_TOF;
            float cos_t = cosf(theta_rad);
            float sin_t = sinf(theta_rad);

            // Fill Z array: 16 zones, dummy value if invalid or X too large
            float Z[16];
            for (int y = 0; y < 16; y += 4){
                int row = y / 4;
                float tan_v = tan_zone_v[row];
                for (int x = 0; x < 4; x++){
                    int idx = x + y;
                    if (measurementData.target_status[idx] == 5){
                        float dist = measurementData.distance_mm[idx];
                        float X = dist * (cos_t - tan_v * sin_t);
                        Z[idx] = (X > Max_Upward_mm) ? DUMMY_Z : dist * (sin_t + tan_v * cos_t);
                    } else {
                        Z[idx] = DUMMY_Z;
                    }
                }
            }

            // Compute min Z per column group: left=col3, mid=col2+col1, right=col0
            float min_left  = DUMMY_Z;
            float min_mid   = DUMMY_Z;
            float min_right = DUMMY_Z;
            for (int y = 0; y < 16; y += 4){
                if (Z[3 + y] < min_left)  min_left  = Z[3 + y];
                if (Z[2 + y] < min_mid)   min_mid   = Z[2 + y];
                if (Z[1 + y] < min_mid)   min_mid   = Z[1 + y];
                if (Z[0 + y] < min_right) min_right = Z[0 + y];
            }

            ToF_out[0] = min_left;
            ToF_out[1] = min_mid;
            ToF_out[2] = min_right;
            Serial.print("L:");
            if (min_left  < DUMMY_Z) Serial.print(min_left,  0); else Serial.print("----");
            Serial.print("mm  M:");
            if (min_mid   < DUMMY_Z) Serial.print(min_mid,   0); else Serial.print("----");
            Serial.print("mm  R:");
            if (min_right < DUMMY_Z) Serial.print(min_right, 0); else Serial.print("----");
            Serial.println("mm");
        }
    }
    
    return ToF_out;
}



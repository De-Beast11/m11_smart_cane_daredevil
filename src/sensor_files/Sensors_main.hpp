#pragma once

#include <Wire.h>
#include <array>
#include <SparkFun_VL53L5CX_Library.h>
#include <AsyncDelay.h>
#include <7Semi_ICM20948.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <jsnsr04t.h>

#include "OrientationEKF.hpp"

class Sensors {
private:
    // ================= IMU =================
    ICM20948_7Semi imu;
    OrientationEKF orientation;

    float accel_bias_x = 0;
    float accel_bias_y = 0;
    float accel_bias_z = 0;

    float gyro_bias_x = 0;
    float gyro_bias_y = 0;
    float gyro_bias_z = 0;

    void calculateIMUBias();
    void IMUGetData(std::array<float, 9>& data);

    // ================= TOF =================
    SparkFun_VL53L5CX imager;
    VL53L5CX_ResultsData measurementData;
    std::array<float, 3> ToF_out;

    static constexpr float DEG2RAD_TOF = 3.14159265f / 180.0f;
    static constexpr float tan_zone_v[4]  = { 0.30335f, 0.09849f, -0.09849f, -0.30335f };
    static constexpr float Max_Upward_mm = 1200.0f;
    static constexpr float DUMMY_Z = 100000.0f;

    // ================= ULTRASONIC =================
    JsnSr04T ultrasonic;

    int lastValidDistance = 0;
    int glitchCount = 0;
    int x = 0;

    // ================= IMU TASK =================
    TaskHandle_t imuTaskHandle = nullptr;

    float pitch = 0.0f;
    portMUX_TYPE pitchMux = portMUX_INITIALIZER_UNLOCKED;

    static void imuTaskWrapper(void* param);
    void imuTask();

public:
    Sensors(int echo = 33, int trig = 25);

    void init();

    float getPitch();
    int getUltrasound();
    std::array<float,3> getToF(float pitch_deg, int ultrasound_distance);
};
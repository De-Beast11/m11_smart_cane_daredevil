#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <array>

class sensors{
    private:
    std::array<float, 9> IMUSensor;
    std::array<float, 3> gyro;
    std::array<float, 3> gyro_bias;
    std::array<float, 3> accel;
    std::array<float, 3> accel_bias;
    std::array<float, 3> mag;
    std::array<float, 3> mag_bias;
    std::array<float, 3> ToF_out;
    int x;

    public:
        sensors(){
            gyro_bias.fill(0);
            accel_bias.fill(0);
            mag_bias.fill(0);
        };
    
    void init();
    void gyro_calibration();
    void accel_calibration();
    int ultrasound();
    std::array<float, 3> time_of_flight(float pitch_deg);
    const std::array<float, 9>& getIMU();
    const std::array<float, 3>& getGyr();
    const std::array<float, 3>& getAcc();
    const std::array<float, 3>& getMag();
    
};
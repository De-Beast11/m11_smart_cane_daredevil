#include "OrientationEKF.hpp"
#include "sensor_init.hpp"
#include <SparkFun_VL53L5CX_Library.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <jsnsr04t.h>
#include <AsyncDelay.h>
#define ECHO_PIN 2
#define TRIGGER_PIN 4


OrientationEKF Orientation;
sensors Sensor;

Adafruit_BNO055 IMUtest = Adafruit_BNO055(55);


std::array<float,9> Sensor_IMU;
std::array<float,7> quaternion_out;
int imageWidth = 4;


float roll;
float pitch;
float yaw;


void setup(){
    Serial.begin(115200);
    Sensor.init();
}

void loop(){
    Orientation.orientation(Sensor.getIMU());
    quaternion_out = Orientation.current_state();
    std::array<float, 3> euler;
    Orientation.quat2euler(quaternion_out, euler);                      
    Serial.print("Pitch: ");                
    Serial.println(euler[1]);

   /*
   Serial.print(quaternion_out[0]); Serial.print(",");
    Serial.print(quaternion_out[1]); Serial.print(",");
    Serial.print(quaternion_out[2]); Serial.print(",");
    Serial.println(quaternion_out[3]);*/ 

    Sensor.ultrasound();
    Sensor.time_of_flight(euler[1]);

    delay(10);
}


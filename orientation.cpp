#include <vector>
#include <cmath>
#include <Arduino.h>
#include "OrientationEKF.hpp"
#include "matrix.hpp"
#include "EKF_MATH.hpp"


constexpr float Ts = 0.01;                // IMU Timestep
constexpr float  G = 9.81;                  // Gravititational acceleration
constexpr float  Mag_dec = 67.483;          // Local magnetic declanation
constexpr float  test_PI = 3.141592653589793;  // PI
constexpr float RAD2DEG = 180/test_PI;
constexpr float DEG2RAD = test_PI/180.0;

// Earth Magnetic Interference Calculation
static constexpr float magrad = Mag_dec*DEG2RAD;
float mx = cosf(magrad);
float my = 0;
float mz = sinf(magrad);
float magnorm = 1/sqrt(mx*mx + mz*mz);
float m_x = mx*magnorm;
float m_z = mz*magnorm;

// Environmental Matrices
std::array<float, 3> gravity = {0,0, -G};
std::array<float, 3> mag_matrix = {m_x, 0, m_z};


void OrientationEKF::orientation(const std::array<float, SENSOR>& sensor){
    // Initialize matrix (B is not used, too uncontrollable)

    std::array<float, 3> gyr {sensor[0]*DEG2RAD, sensor[1]*DEG2RAD, sensor[2]*DEG2RAD};
    std::array<float, 3> acc {sensor[3], sensor[4], sensor[5]};
    std::array<float, 3> mag {sensor[6], sensor[7], sensor[8]};
    std::array<float, 4> q{};
    std::array<float, 3> bias{};
    std::array<float, 3> w{};
    std::array<float, 4> qtemp{};
    std::array<float, STATES*STATES> A{};
    //std::array<float, MEASUREMENT*STATES> B{};
    std::array<float, MEASUREMENT*STATES> C{};
    std::array<float, MEASUREMENT> prediction{};
    std::array<float, MEASUREMENT> error_state{};
    std::array<float, MEASUREMENT> measurement_state{};
    std::array<float, STATES*STATES> P_now{};

    float accnorm = matrix::norm<3>(acc);
    float magnorm = matrix::norm<3>(mag);




    if (accnorm > 1e-6f && magnorm > 1e-6)
        for (size_t i=0; i<3; i++){
            acc[i] /= accnorm;
            mag[i] /= magnorm;
        }
        acc[2] *= -1;

    q = {x_state[0], x_state[1], x_state[2], x_state[3]};
    bias = {x_state[4], x_state[5], x_state[6]};
    w = {gyr[0]-bias[0], gyr[1]-bias[1], gyr[2]-bias[2]};
    std::array<float, 16> omega = {
        0  , -w[0] , -w[1], -w[2],
        w[0],   0  , w[2] , -w[1],
        w[1], -w[2],   0  ,  w[0],
        w[2],  w[1], -w[0],   0
    };

    matrix::identity<STATES, STATES>(A);
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++) 
            A[i*STATES+j] += omega[i*4+j]*Ts*0.5; 
    A[0*STATES+4] =  q[1]*Ts/2; A[0*STATES+5] =  q[2]*Ts/2; A[0*STATES+6] =  q[3]*Ts/2; 
    A[1*STATES+4] = -q[0]*Ts/2; A[1*STATES+5] =  q[3]*Ts/2; A[1*STATES+6] = -q[2]*Ts/2; 
    A[2*STATES+4] = -q[3]*Ts/2; A[2*STATES+5] = -q[0]*Ts/2; A[2*STATES+6] =  q[1]*Ts/2; 
    A[3*STATES+4] =  q[2]*Ts/2; A[3*STATES+5] = -q[1]*Ts/2; A[3*STATES+6] = -q[0]*Ts/2; 
    
    C = {
        q[2]*2,  -q[3]*2, q[0]*2, -q[1]*2, 0,0,0,
         -q[1]*2,  -q[0]*2,  -q[3]*2, -q[2]*2, 0,0,0,
         -q[0]*2, q[1]*2, q[2]*2, -q[3]*2, 0,0,0,
         2*(m_x*q[0]-m_z*q[2]), 2*(m_x*q[1]+m_z*q[3]), 2*(-m_x*q[2]-m_z*q[0]), 2*(m_z*q[1]-m_x*q[3]),  0, 0, 0,
         2*(m_z*q[1]-m_x*q[3]), 2*(m_x*q[2]+m_z*q[0]), 2*(m_x*q[1]+m_z*q[3]), 2*(m_z*q[2]-m_x*q[0]),  0, 0, 0,
         2*(m_x*q[2]+m_z*q[0]), 2*(m_x*q[3]-m_z*q[1]), 2* (m_x*q[0]-m_z*q[2]), 2*(m_x*q[1]+m_z*q[3]),  0, 0, 0,
    };

    for (int i=0; i<3; i++) {
        measurement_state[i] = acc[i];
        measurement_state[i+3] = mag[i];
        //Serial.println(mag[i]);
    }

    //for (int i=3; i<6; i++){
    //    Serial.print(measurement_state[i]);
    //    Serial.print(" ");
    //    Serial.println(prediction[i]);}

    std::array<float, STATES> predicted_states;
    matrix::mult<STATES, 1, STATES>(A, x_state, predicted_states);
    orientation::y_estimate<STATES, MEASUREMENT>(x_state, gravity, mag_matrix, prediction);
    matrix::sub<MEASUREMENT>(measurement_state, prediction, error_state);
    EKF::P<STATES>(A, P, Q, P_now);
    EKF::K<STATES, MEASUREMENT>(A, P_now, C, R, K);
    EKF::update<STATES, MEASUREMENT>(predicted_states, K, error_state, x_state);
    EKF::Pnew<STATES, MEASUREMENT>(C, P_now, K, R, P);
    qtemp = {x_state[0], x_state[1], x_state[2], x_state[3]};
    float normquat = matrix::norm<4>(qtemp);
    for (int i=0; i<4; i++)
        x_state[i] /= normquat;
}

void OrientationEKF::quat2euler(const std::array<float,STATES> q,
                           std::array<float, 3>& euler){
    float roll;
    float pitch;
    float yaw;
    float roll_deg;
    float pitch_deg;
    float yaw_deg;

    roll = atan2(2.0f*(q[0]*q[1]+q[2]*q[3]), 1-2.0f*(q[1]*q[1]+q[2]*q[2]));
    pitch = asin(2.0f*(q[0]*q[2]-q[3]*q[1]));
    yaw = atan2(2.0f*(q[0]*q[3]+q[1]*q[2]), 1-2.0f*(q[2]*q[2]+q[3]*q[3]));
    roll_deg = roll * (-180/test_PI);
    pitch_deg = pitch * (-180/test_PI);
    yaw_deg = yaw * (-180/test_PI);
    euler = {roll_deg, pitch_deg, yaw_deg};
}

const std::array<float, 7> OrientationEKF::current_state(){
    return x_state;
}

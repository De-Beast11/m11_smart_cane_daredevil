#include <array>
#include "matrix.hpp"
#include <Arduino.h>



class OrientationEKF{
    private: 
    static constexpr int STATES = 7;
    static constexpr int MEASUREMENT = 6;
    static constexpr int SENSOR = 9;
    const float MAG_BASE_R = 120.0;
    const float ACC_BASE_R = 6.0;
    public:
        OrientationEKF():
        Q{0.1, 0, 0, 0, 0, 0, 0,
          0, 0.1, 0, 0, 0, 0, 0,
          0, 0, 0.1, 0, 0, 0, 0,
          0, 0, 0, 0.1, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0},
        R{ACC_BASE_R, 0, 0, 0, 0, 0,
          0, ACC_BASE_R, 0, 0, 0, 0,
          0, 0, ACC_BASE_R, 0, 0, 0,
          0, 0, 0, MAG_BASE_R, 0, 0,
          0, 0, 0, 0, MAG_BASE_R, 0,
          0, 0, 0, 0, 0, MAG_BASE_R,
          }
        {
            x_state.fill(0);
            x_state[0] = 1.0f;
            x_state[4] = 0.0056f;
            x_state[5] = 0.0056f;
            x_state[6] = 0.0056f;
        }

        void orientation(const std::array<float, SENSOR>& sensor);
        void quat2euler(const std::array<float, STATES> x_state, 
                        std::array<float,3>& euler);
        const std::array<float, STATES> current_state ();

    private:
    std::array<float, STATES> x_state{};
    std::array<float, STATES*STATES> P{};
    std::array<float, STATES*MEASUREMENT> K{};
    std::array<float, STATES*STATES> Q{};
    std::array<float, MEASUREMENT*MEASUREMENT> R{};

};

namespace orientation{
    inline std::array<float,9> Quat2Rot(const std::array<float, 4>& quaternion){
        float q0 = quaternion[0];
        float q1 = quaternion[1];
        float q2 = quaternion[2];
        float q3 = quaternion[3];

 
        return{
            q0*q0+q1*q1-q2*q2-q3*q3,
            2*(q1*q2+q0*q3),
            2*(q1*q3-q0*q2),
            2*(q1*q2-q0*q3),
            q0*q0-q1*q1+q2*q2-q3*q3,
            2*(q2*q3+q0*q1),
            2*(q1*q3+q0*q2),
            2*(q2*q3-q0*q1),
            q0*q0-q1*q1-q2*q2+q3*q3
        };
    }

    template<size_t STATE, size_t MEASUREMENTS>
    inline void y_estimate(const std::array<float, STATE>& x_state,
                           const std::array<float, 3>& gravity, 
                           const std::array<float, 3>& mag_matrix, 
                           std::array<float, MEASUREMENTS>& output){
        std::array<float, 4> quaternion{};        
        for (int i=0; i<4; i++)
            quaternion[i] = x_state[i];


        std::array<float, 3> predicted_acc{};
        std::array<float, 3> predicted_mag{};
        auto quatmatrix = orientation::Quat2Rot(quaternion);
        //for(int i=0; i<4; i++) Serial.println(quaternion[i]);

        matrix::mult<3,1,3>(quatmatrix, gravity, predicted_acc);
        float accnorm = matrix::norm<3>(predicted_acc);
        
        for (int i=0; i<3; i++){
            predicted_acc[i] /= accnorm;
            output[i] = predicted_acc[i];    
        }
        matrix::mult<3,1,3>(quatmatrix, mag_matrix, predicted_mag);
        float magnorm = matrix::norm<3>(predicted_mag);
        for (int i=0; i<3; i++){
            predicted_mag[i] /= magnorm;
            output[i+3] = predicted_mag[i];
            //Serial.println(predicted_mag[i]);
        }
        output[5] *= -1;
        //float magmult = matrix::dot_product<3>(predicted_mag, predicted_acc);
        //for (int i=0; i<3; i++){
        //    predicted_mag[i] -= magmult*predicted_acc[i];
        //    output[i+3] = predicted_mag[i];
        //}
    }

    template<size_t MEASUREMENT>
    inline void magnetometer_trust(const std::array<float, 3> magnet,
                                   const float ACC_BASE_R,
                                   std::array<float, MEASUREMENT*MEASUREMENT> R){
        float magnorm = matrix::norm<3>(magnet);
        float diff = fabs(magnorm - 1);
        float scaling = 1 + 10*diff;
        //Serial.println(magnorm);
        //Serial.println(diff);
        if (diff > 0.2) {
            for (size_t i=3; i< MEASUREMENT; i++) R[i*MEASUREMENT+i] = ACC_BASE_R*scaling;
        }
    }

    inline void quat_mult(const std::array<float, 4> qi,
                          const std::array<float, 4> qo,
                          std::array<float, 4> output){
        
        output[0] = qi[0]*qo[0] - qi[1]*qo[1] - qi[2]*qo[2] - qi[3]*qo[3];
        output[1] = qi[0]*qo[1] + qi[1]*qo[0] + qi[2]*qo[3] - qi[3]*qo[2];
        output[2] = qi[0]*qo[2] - qi[1]*qo[3] + qi[2]*qo[0] + qi[3]*qo[1];
        output[3] = qi[0]*qo[3] + qi[1]*qo[2] - qi[2]*qo[1] + qi[3]*qo[0];

        //std::array<float, 4> qdot;
        //qdot[0] = -0.5f*( w[0]*q[1] + w[1]*q[2] + w[2]*q[3] );
        //qdot[1] =  0.5f*( w[0]*q[0] + w[1]*q[3] - w[2]*q[2] );
        //qdot[2] =  0.5f*( -w[0]*q[3] + w[1]*q[0] + w[2]*q[1] );
        //qdot[3] =  0.5f*( w[0]*q[2] - w[1]*q[1] + w[2]*q[0] ); 

        //for (int i=0;i<4;i++) output[i] = q[i] + qdot[i]*0.005;
        float outnorm = matrix::norm<4>(output);
        //for (int i=0;i<4;i++) output[i] /= outnorm;


    }


}
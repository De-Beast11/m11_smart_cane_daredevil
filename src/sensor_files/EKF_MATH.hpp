#pragma once
#include "matrix.hpp"
#include <array>


namespace EKF{
    template <size_t STATES>
    inline void P(const std::array<float, STATES*STATES>& A, 
                  const std::array<float, STATES*STATES>& P_prev, 
                  const std::array<float, STATES*STATES>& Q, 
                  std::array<float, STATES*STATES>& P_next){
        
        std::array<float, STATES*STATES> AP{};
        std::array<float, STATES*STATES> AT{};
        std::array<float, STATES*STATES> APAT{};

        matrix::transpose<STATES, STATES>(A, AT);
        matrix::mult<STATES, STATES, STATES>(A, P_prev, AP);
        matrix::mult<STATES, STATES, STATES>(AP, AT, APAT);
        matrix::add<STATES*STATES>(APAT,Q, P_next);
    }

    template<size_t STATES, size_t MEASUREMENT>
    inline void   K(const std::array<float, STATES*STATES>& A,
                    const std::array<float, STATES*STATES>& P,
                    const std::array<float, MEASUREMENT*STATES>& C,
                    const std::array<float, MEASUREMENT*MEASUREMENT>& R,
                    std::array<float, STATES*MEASUREMENT>&K){
        // Creating K
        std::array<float, STATES*STATES> AP{};
        std::array<float, STATES*MEASUREMENT> CT{};
        std::array<float, STATES*MEASUREMENT> APCT{};
        std::array<float, MEASUREMENT*STATES> CP{};
        std::array<float, MEASUREMENT*MEASUREMENT> CPCT{};
        std::array<float, MEASUREMENT*MEASUREMENT> CPCTR{};

        // Inverting Matrix
        std::array<float, MEASUREMENT*MEASUREMENT> cholesky{};
        std::array<float, MEASUREMENT*MEASUREMENT> cholesky_T{};
        std::array<float, MEASUREMENT*STATES> APCT_T{};
        std::array<float, MEASUREMENT*STATES> inter_stage{};
        std::array<float, MEASUREMENT*STATES> inv_K{};
        // Calculating TOP side
        matrix::transpose<MEASUREMENT, STATES>(C, CT);
        matrix::mult<STATES, STATES, STATES>(A, P, AP);
        matrix::mult<STATES, MEASUREMENT, STATES>(AP, CT, APCT);
        // Calculating BOTTOM side
        matrix::mult<MEASUREMENT, STATES, STATES>(C, P, CP);
        matrix::mult<MEASUREMENT, MEASUREMENT, STATES>(CP, CT, CPCT);
        matrix::add<MEASUREMENT*MEASUREMENT>(CPCT, R, CPCTR);
        // Calculating BOTTOM INVERSE and K
        matrix::cholesky_decomp<MEASUREMENT>(CPCTR, cholesky);
        matrix::transpose<MEASUREMENT, MEASUREMENT>(cholesky, cholesky_T);
        matrix::transpose<STATES, MEASUREMENT>(APCT, APCT_T);
        matrix::forward_sub<MEASUREMENT, STATES>(cholesky, APCT_T, inter_stage);
        matrix::backward_sub<MEASUREMENT, STATES>(cholesky_T, inter_stage, inv_K);
        matrix::transpose<MEASUREMENT, STATES>(inv_K, K);
    }

    template<size_t STATES, size_t MEASUREMENT>
    inline void update(const std::array<float, STATES> predicted_states,
                       const std::array<float, STATES*MEASUREMENT>& K, 
                       const std::array<float, MEASUREMENT>& ERROR_STATES, 
                       std::array<float, STATES>& UPDATE){
        
        std::array<float, STATES> correction_states{};
        matrix::mult<STATES, 1, MEASUREMENT>(K, ERROR_STATES, correction_states);
        matrix::add<STATES>(predicted_states, correction_states, UPDATE);
    }

    template<size_t STATES, size_t MEASUREMENT>
    inline void Pnew(const std::array<float, STATES*MEASUREMENT>& C, 
              const std::array<float, STATES*STATES>& P, 
              const std::array<float, STATES*MEASUREMENT>& K,
              const std::array<float, MEASUREMENT*MEASUREMENT>& R, 
              std::array<float, STATES*STATES>& newP){
        
        std::array<float, STATES*STATES> identity{};        
        std::array<float, STATES*STATES> IKC{};
        std::array<float, STATES*STATES> IKCP{};
        std::array<float, STATES*STATES> KC{};
        std::array<float, STATES*STATES> IKC_T{};
        std::array<float, MEASUREMENT*STATES> K_T{};
        std::array<float, STATES*MEASUREMENT> KR{};
        std::array<float, STATES*STATES> LHS{};
        std::array<float, STATES*STATES> RHS{};       

        for (size_t i=0; i<STATES; i++)
            for (size_t j=0; j<STATES; j++)
                identity[i*STATES+j] = (i == j);
        
        matrix::mult<STATES,STATES,MEASUREMENT>(K, C, KC);           // K*C
        matrix::sub <STATES*STATES>(identity, KC, IKC);             // I - K*C
        matrix::transpose<STATES, STATES>(IKC, IKC_T);              // (I-K*C)_T
        matrix::transpose<STATES, MEASUREMENT>(K, K_T);             // K_T
        matrix::mult<STATES, STATES, STATES>(IKC, P, IKCP);         // (I-K*C)*P
        matrix::mult<STATES, STATES, STATES>(IKCP, IKC_T, LHS);     // (I*K*C)*P*((I-K*C)_T)
        matrix::mult<STATES, MEASUREMENT, MEASUREMENT>(K, R, KR);   // K*R
        matrix::mult<STATES, STATES, MEASUREMENT>(KR, K_T, RHS);         // K*R*K_T
        matrix::add<STATES*STATES>(LHS, RHS, newP);                 // (I*K*C)*P*((I-K*C)_T) + (K*R*K_T)
    }
}
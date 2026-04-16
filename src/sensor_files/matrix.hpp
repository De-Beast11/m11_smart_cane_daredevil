#pragma once
#include <vector>
#include <array>
#include <cmath>

constexpr float eps = 1e-6f;

namespace matrix {
    template<size_t rowA, size_t colB, size_t common>
    inline void mult(const std::array<float, rowA*common>& a, 
                     const std::array<float, common*colB>& b, 
                     std::array<float, rowA*colB>& output){
        for (size_t i=0; i<rowA; i++){
            for (size_t j=0; j<colB; j++){
                float temp = 0.0;
                for (size_t k=0; k<common; k++){
                    temp += a[i*common + k]*b[k*colB+j];}
                output[i*colB+j] = temp;
            }
        }
    }

    template<size_t row, size_t column>
    inline void transpose(const std::array<float, row*column>& input, 
                          std::array<float, column*row>& output){
        for (size_t i=0; i<row; i++)
            for(size_t j=0; j<column; j++){
                output[j*row+i] = input[i*column+j];
            }
    }
    
    template<size_t size>
    inline void sub(const std::array<float, size>& a, 
                    const std::array<float, size>& b,
                    std::array<float, size>& out){
    for (size_t i=0; i<size; i++) out[i] = a[i] - b[i];
    }

    template<size_t size>
    inline void add(const std::array<float, size>& a,
                    const std::array<float, size>& b,
                    std::array<float, size>& out){
        for (size_t i=0; i<size; i++) out[i] = a[i] + b[i];
    }

    template<size_t size>
    inline float norm(const std::array<float, size>& input){
        float sum = 0.0;
        for (size_t i=0; i<size; i++)
            sum += input[i]*input[i];
        return sqrt(sum);
    }

    template<size_t size>
    inline float dot_product(const std::array<float, size>& a,
                              const std::array<float, size>& b){
        float result = 0.0;
        for (size_t i=0; i<size; i++)
            result += a[i]*b[i];
        return result;
    }

    template<size_t row, size_t col>
    inline void zeros(std::array<float, row*col>& A){
        for (size_t i=0; i<row*col; i++) A[i] = 0.0;
    }

    template<size_t row, size_t col>
    inline void identity(std::array<float, row*col>& A){
        matrix::zeros<row,col>(A);
        for (size_t i=0; i<row; i++){
            for (size_t j=0; j<col; j++)
                if (i == j) A[i*row + j] = 1.0;
                
            }        
    }

    template<size_t rowA, size_t colA, size_t rowB, size_t colB>
    inline void insert(int row_offset, int col_offset,
                           std::array<float, rowA*colA>& A, 
                           std::array<float, rowB*colB>& B, 
                           float mult){
        
        for (size_t i=0; i<rowB; i++)
            for (size_t j=0; j<colB; j++){
                A[(i+row_offset)*colA + (j+col_offset)] = mult*B[i*colB+j];
            }
    }

    template<size_t size>
    inline void cholesky_decomp(const std::array<float, size*size>& input, 
                                std::array<float, size*size>& lower){
            for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j <= i; j++) {
                float sum = 0;
                float temp = 0;
                float denomi = 0;

                // summation for diagonals
                if (j == i) {
                    for (size_t k = 0; k < j; k++)
                        sum += (lower[j*size+k]*lower[j*size+k]);

                    temp = input[j*size+j] - sum;
                    if (temp < 0.0f) temp = eps;        // Non positive definite safe cases 
                    lower[j*size+j] = sqrt(temp);
                } else {

                    // Evaluating L(i, j) using L(j, j)
                    for (size_t k = 0; k < j; k++) 
                        sum += (lower[i*size+k] * lower[j*size+k]);

                    denomi = lower[j*size+j];
                    if (denomi < eps) denomi = eps;    // Zero denomminator safe cases
                    lower[i*size+j] = (input[i*size+j] - sum) / denomi;
                }
            }
            for(size_t j=i+1;j<size;j++)
                lower[i*size+j] = 0.0f;
        }
    }

    template<size_t row, size_t column>
    inline void forward_sub(const std::array<float, row*row >& cholesky, 
                            const std::array<float, row*column>& APCT, 
                            std::array<float, row*column>& out){
        float denom;
        for (size_t i=0; i<column; i++)
            for (size_t j=0; j<row; j++){
                float sum = APCT[j*column+i];
                for (size_t k=0; k<j; k++)
                    sum -= cholesky[j*row+k] * out[k*column+i];
                denom = cholesky[j*row+j];
                if (denom < eps) denom = eps;
                out[j*column+i] = sum/denom;
            }
    }

    template<size_t row, size_t column>
    inline void backward_sub(const std::array<float, row*row>& choleskyT, 
                             const std::array<float, row*column>& Y, 
                             std::array<float, row*column>& out){

        int rowint = row;
        int colint = column;
        float denom;
        for (int i=0; i<colint; i++)
            for (int j = rowint-1; j >= 0; j--){
                float sum = Y[j*column+i];
                for (int k= j+1; k<rowint; k++)
                    sum -= choleskyT[j*row+k] * out[k*column+i];
                denom = choleskyT[j*row+j];
                if (denom < eps) denom = eps;
                out[j*column+i] = sum/denom;
            }
    }
}
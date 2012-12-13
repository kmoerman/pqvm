//#include <iostream>
#include <complex>
#include <cstdlib>
#include <cstddef>
#include <ctime>
#include <cmath>
#include <tbb/tbb.h>
#include "../measure/measure.h"

using namespace std;
using namespace tbb;

/* Number types */
typedef double Real;
typedef complex<Real> Complex;

Real random_real () {
    return ((Real) rand())/RAND_MAX;
}

template <class Number>
void fill_vector (Number vector[], size_t size, Number (*g)()) {
    for (size_t i = 0; i < size; ++i)
        vector[i] = g();
}

/* Algorithms */
//Data
Real* A;
Real* B;
size_t p_size;
size_t m_size;


//Naive transpose
void transpose_naive_seq () {
    for (size_t i = 0; i < p_size; ++i)
        for (size_t j = 0; j < p_size; ++j)
            B[j*p_size + i] = A[i*p_size + j];
}

void transpose_naive_pll (const blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i!=r.end(); ++i)
        for (size_t j = 0; j < p_size; ++j)
            B[j*p_size + i] = A[i*p_size + j];
}

void transpose_naive_par () {
    parallel_for(blocked_range<size_t>(0, p_size), &transpose_naive_pll);
}

//Prokopp transpose
uint32_t deinterleave_util(uint32_t x) {
    x = x & 0x55555555;
    x = (x | (x >> 1)) & 0x33333333;
    x = (x | (x >> 2)) & 0x0F0F0F0F;
    x = (x | (x >> 4)) & 0x00FF00FF;
    x = (x | (x >> 8)) & 0x0000FFFF;
    return x;
}

void deinterleave(uint32_t source, uint32_t& even, uint32_t& odd) {
    even = deinterleave_util(source);
    odd  = deinterleave_util(source >> 1);
}

void transpose_prokopp_seq () {
    uint32_t i, j;
    for (size_t k = 0; k < m_size; ++k) {
        deinterleave (k, j, i);
        B[j * p_size + i] = A[i * p_size + j];
    }
}

void transpose_prokopp_pll (const blocked_range<size_t>& r) {
    uint32_t i, j;
    for (size_t k = r.begin(); k != r.end(); ++k) {
        deinterleave(k, j, i);
        B[j * p_size + i] = A[i * p_size + j];
    }
}

void transpose_prokopp_par () {
    parallel_for(blocked_range<size_t>(0, m_size), &transpose_prokopp_pll);
}

/* Setup Experiment */
size_t algo_n = 2;
void (*algorithms[])() = {
    transpose_naive_seq,
    transpose_naive_par,
    transpose_prokopp_seq,
    transpose_prokopp_par
};

string names[] = {
    "transpose-naive",
    "transpose-prokopp"
};

//transpose <problem size> <iterations>
int main (int argc, const char * argv[]) {
    
    srand(time(0));
    
    p_size = atoi(argv[1]);
    m_size = p_size * p_size;
    size_t iterations = atoi(argv[2]);
    
    A = new Real [m_size];
    B = new Real [m_size];
    
    fill_vector(A, m_size, random_real);
    
    /*
     //Check if transposed, dirty will do for now.
     for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
            cout << A[i*p_size + j] << " ";
        cout << endl;
    }
    cout << endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
            cout << B[i*p_size + j] << " ";
        cout << endl;
    }*/
    
    /*uint32_t k, l;
    for (int i = 0; i < 16; ++i) {
        deinterleave(i, k, l);
        cout << "["<< l << " " << k << "], ";
    }*/
    
    
    for (size_t i = 0; i < algo_n; ++i) {
        string name (names[i] + '-' + string(argv[1]));
        measure(name, iterations, algorithms[i * 2], algorithms[i * 2 + 1]);
    }
    
    delete A;
    delete B;
    return EXIT_SUCCESS;
}
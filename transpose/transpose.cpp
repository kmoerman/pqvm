#include <iostream>
#include <fstream>
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
void transpose_naive_fn (const blocked_range<size_t>& r) {
    size_t i, j, k, l;
    for (i = r.begin(), k = i*p_size; i!=r.end(); ++i, k+=p_size)
        for (j = 0, l = 0; j < p_size; ++j, l+=p_size)
            B[l + i] = A[k + j];
}

void transpose_naive () {
    parallel_for(blocked_range<size_t>(0, p_size), &transpose_naive_fn);
}

size_t block_size;
size_t num_blocks;

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

void transpose_prokopp_fn (const blocked_range<size_t>& r) {
    uint32_t i, j;
    for (size_t k = r.begin(); k != r.end(); ++k) {
        deinterleave(k, j, i);
        B[p_size * j + i] = A[p_size * i + j];
    }
}

void transpose_prokopp () {
    parallel_for(blocked_range<size_t>(0, m_size), &transpose_prokopp_fn);
}

/* Setup Experiment */
void (*algorithms[])() = {
    transpose_naive,
    transpose_prokopp
};

string names[] = {
    "naive",
    "prokopp"
};

int algo_n = 2;

int main (int argc, const char * argv[]) {
    
    srand(time(0));
    
    string problem_name = "transpose-";
    
    p_size = atoi(argv[1]);
    m_size = p_size * p_size;
    size_t iterations = atoi(argv[2]);
    
    A = new Real [m_size];
    B = new Real [m_size];
    
    fill_vector(A, m_size, random_real);
    
    transpose_prokopp();
    
    /*for (int i = 0; i < 8; i++) {
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
    
    
    for (int i = 0; i < algo_n; ++i) {
        string f_name = problem_name;
        f_name.append(names[i]);
        f_name.append("-");
        f_name.append(argv[1]);
        f_name.append(".dat");
        
        ofstream data_file (f_name.c_str());
        
        measure(data_file, iterations, transpose_naive);
        
        data_file.close();
    }
    
    return EXIT_SUCCESS;
}
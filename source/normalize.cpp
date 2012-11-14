#include <iostream>
#include <complex>
#include <cstdlib>
#include <ctime>
#include "measure.h"

using namespace std;

//Number types
typedef double Real;
typedef complex<Real> Complex;

Real random_real () {
  return ((Real) rand())/RAND_MAX;
}

Complex random_complex () {
  Complex z (random_real(), random_real());
  return z;
}

ostream& operator<< (ostream& os, const Complex& z) {
  os << z.real() << "+" << z.imag() << "i";
}

template <class Number, class Generator>
void fill_vector (Number vector[], int size, Generator g) {
  for (--size; size >= 0; --size) vector[size] = g();
}

//Measure the time [seconds] of n consecutive executions
//of of functional object F.
template <class Function>
double measure(int n, Function F) {
    clock_t zero = clock();
    for (; n > 0; --n) F();
    return ((double)(clock() - zero)) / CLOCKS_PER_SEC;
}


//Test data
#define TEST_SIZE 1000000
#define TEST_ITER 100

Real x[TEST_SIZE];
Complex z[TEST_SIZE];

void fill_real () {
    fill_vector(x, TEST_SIZE, random_real);
}

void fill_complex () {
    fill_vector(z, TEST_SIZE, random_complex);
}

int main (int argc, const char * argv[]) {
  srand(time(0));

  cout << measure(TEST_ITER, fill_real) << endl;
  cout << measure(TEST_ITER, fill_complex) << endl;

  return EXIT_SUCCESS;
}

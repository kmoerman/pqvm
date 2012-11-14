#include <iostream>
#include <complex>
#include <cstdlib>
#include <cstddef>
#include <ctime>
#include <cmath>
#include <tbb/tbb.h>
//#include "measure.h"

using namespace std;
using namespace tbb;

//Number types
typedef double Real;
typedef complex<Real> Complex;

Real random_real () {
  return ((Real) rand())/RAND_MAX;
}

Real inner_product_term(Real a, Real b) {
  return a * b;
}

Complex random_complex () {
  Complex z (random_real(), random_real());
  return z;
}

Complex inner_product_term(const Complex& a, const Complex& b) {
  return a * conj(b);
}

ostream& operator<< (ostream& os, const Complex& z) {
  os << real(z) << "+" << imag(z) << "i";
}

template <class Number, class Generator>
void fill_vector (Number vector[], size_t size, Generator g) {
  while (size > 0) vector[--size] = g();
}

//Measure the time [milliseconds] of n consecutive executions
//of of functional object F.
//TODO: put in separate .cpp file (strange linking errors)
template <class Function>
double measure(size_t n, Function F) {
  static double factor = 1000.0 / CLOCKS_PER_SEC;
  clock_t zero = clock();
  for (; n > 0; --n) F();
  return factor * ((clock() - zero));
}

//Normalization
//Reduce to find the norm of the vector
template <class Number>
class InnerProduct {
  Number* vector;
public:
  Number result;
  void operator() (const blocked_range<size_t>& r) {
    Number* v = vector;
    Number res = result;
    size_t end = r.end();
    for (size_t i = r.begin(); i != end; ++i)
      res += inner_product_term(v[i], v[i]);
    result = res;
  }
  
  InnerProduct (InnerProduct& ip, split) : vector(ip.vector), result(0) {}
  
  InnerProduct (Number v[]) : vector(v), result(0) {}
  
  void join (const InnerProduct& ip) {
    result += ip.result;
  }
};

template <class Number>
Number norm (Number vector[], size_t n) {
  InnerProduct<Number> ip(vector);
  parallel_reduce(blocked_range<size_t>(0,n), ip);
  return sqrt(ip.result);
}

//Map to divide by the norm
template <class Number>
class Normalizer {
  Number * const source;
  Number norm;
public:
  Number * const destination;
  void operator() (const blocked_range<size_t>& r) const {
    Number * s = source;
    Number * d = destination;
    for (size_t i=r.begin(); i!=r.end(); ++i)
      d[i] = s[i]/norm;
  }
  Normalizer (Number s[], Number d[], Number n) : source(s), destination(d), norm(n) {}
};

template <class Number>
void normalize (Number source[], Number destination[], size_t n) {
  parallel_for(blocked_range<size_t>(0,n),
	       Normalizer<Number> (source, destination, norm(source, n)));
}

//Test data
#define TEST_SIZE 1000000
#define TEST_ITER 10

Real xs[TEST_SIZE];
Real xd[TEST_SIZE];
Complex zs[TEST_SIZE];
Complex zd[TEST_SIZE];

//Measure the results
void normalize_real () {
  normalize(xs, xd, TEST_SIZE);
}
void normalize_complex () {
  normalize(zs, zd, TEST_SIZE);
}

int main (int argc, const char * argv[]) {
    
  srand(time(0));
  
  fill_vector(xs, TEST_SIZE, random_real);
  fill_vector(zs, TEST_SIZE, random_complex);
  
  cout << measure(TEST_ITER, normalize_real) << endl;
  cout << measure(TEST_ITER, normalize_complex) << endl;
  
  return EXIT_SUCCESS;
}

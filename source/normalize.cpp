#include <iostream>
#include <complex>
#include <cstdlib>
#include "measure.h"

using namespace std;

#define TEST_SIZE 10//00000

typedef double Real;
typedef complex<Real> Complex;


//#define UPPER 1000
//#define LOWER -UPPER
Real real_random() {
	//static default_random_engine generator;
	//static uniform_real_distribution<Real> distribution (LOWER, UPPER);
	//static auto make = bind(distribution, generator);
	//return make();
	return ((Real) rand())/RAND_MAX;
}

Complex complex_random() {
    Complex z (real_random(), real_random());
    return z;
}

template <class Number, class Generator>
void fill_vector(Number vector[], int size, Generator g) {
	for(--size; size >= 0; --size) vector[size] = g();
}

int main (int argc, const char * argv[]) {
	Real x[TEST_SIZE];
	Complex z[TEST_SIZE];
	
	srand(time(0));
	
	fill_vector(x, TEST_SIZE, real_random);
	fill_vector(z, TEST_SIZE, complex_random);
	
	for(int k = TEST_SIZE - 1; k >= 0; --k)
		cout << x[k] << endl;
	cout << endl;
	for (int k = TEST_SIZE - 1; k >= 0; --k)
		cout << z[k] << endl;
    
	return EXIT_SUCCESS;
}
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
    return conj(a) * b;
}

template <class Number, class Generator>
void fill_vector (Number vector[], size_t size, Generator g) {
    for (int i = 0; i < size; ++i)
        vector[i] = g();
}

//Test data
Real* xs;
Real* xd;
Complex* zs;
Complex* zd;
size_t p_size;

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

//Caclulate norm
template <class Number>
Number norm (Number vector[], size_t n) {
    InnerProduct<Number> ip(vector);
    parallel_reduce(blocked_range<size_t>(0,n), ip);
    return sqrt(ip.result);
}

//Map to divide by the norm
template <class Number>
class Normalize {
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
    Normalize (Number s[], Number d[], Number n) : source(s), destination(d), norm(n) {}
};


void normalize_real_par () {
    parallel_for(blocked_range<size_t>(0, p_size),
                 Normalize<Real>(xs, xd, norm(xs, p_size)));
}
void normalize_complex_par () {
    parallel_for(blocked_range<size_t>(0, p_size),
                 Normalize<Complex>(zs, zd, norm(zs, p_size)));
}

void normalize_real_seq () {
    Real norm;
    for (size_t i = 0; i < p_size; ++i)
        norm += inner_product_term(xs[i], xs[i]);
    norm = sqrt(norm);
    for (size_t i = 0; i < p_size; ++i)
        xd[i] = xs[i] / norm;    
}

void normalize_complex_seq () {
    Complex norm;
    for (size_t i = 0; i < p_size; ++i)
        norm += inner_product_term(zs[i], zs[i]);
    norm = sqrt(norm);
    for (size_t i = 0; i < p_size; ++i)
        zd[i] = zs[i] / norm;
}

int main (int argc, const char * argv[]) {
    srand(time(0));
    
    p_size = atoi(argv[1]);
    size_t iterations = atoi(argv[2]);
    string name;
    
    //test real
    xs = new Real [p_size];
    xd = new Real [p_size];
    
    fill_vector(xs, p_size, random_real);
    
    name = "normalize-real" + string(argv[1]);
    measure(name, iterations, normalize_real_seq, normalize_real_par);
    
    delete xs;
    delete xd;
    
    //test complex
    zs = new Complex [p_size];
    zd = new Complex [p_size];
    fill_vector(zs, p_size, random_complex);
    
    name = "normalize-complex" + string(argv[1]);
    measure(name, iterations, normalize_complex_seq,normalize_complex_par);
    delete zs;
    delete zd;
    
    return EXIT_SUCCESS;
    
}


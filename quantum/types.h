#ifndef pqvm_quantum_types_h
#define pqvm_quantum_types_h

#include <complex>
#include "../vector.h"

namespace quantum {
    
    typedef double real;
    typedef std::complex<real> complex;
    typedef vector<complex> quregister;
    typedef quregister::iterator iterator;
    typedef quregister::size_type size_type;
    
}

#endif
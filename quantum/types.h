#ifndef pqvm_quantum_types_h
#define pqvm_quantum_types_h

#include <complex>
#include <iostream>

#include "../vector.h"

namespace quantum {
    
    typedef double real;
    typedef std::complex<real> complex;
    typedef vector<complex> quregister;
    typedef quregister::iterator iterator;
    typedef quregister::size_type size_type;
    
    std::ostream& operator << (std::ostream& out, const quregister& reg) {
        out << "(";
        for (quregister::const_iterator i (reg.begin()), n (reg.end()); i != n; ++i) {
            out << std::real(*i) << "+" << std::imag(*i) << "i ";
        }
        out << ")" << std::endl;
        return out;
    }
}

#endif
#ifndef pqvm_quantum_h
#define pqvm_quantum_h

#include <iostream>

#include "types.h"
#include "openmp.h"
#include "sequential.h"
#include "tbb.h"

#define QUANTUM_IMPLEMENTATION(namespace)     \
    sigma_x      = &namespace::sigma_x,       \
    sigma_z      = &namespace::sigma_z,       \
    controlled_z = &namespace::controlled_z,  \
    kronecker    = &namespace::kronecker,     \
    measure      = &namespace::measure,       \
    normalize    = &namespace::normalize,     \
    phase_kick   = &namespace::phase_kick,    \
    copy         = &namespace::copy,          \
    namespace::initialize()

namespace quantum {
    
    void (*sigma_x)      (const size_type, quregister&, quregister&);
    void (*sigma_z)      (const size_type, quregister&, quregister&);
    void (*controlled_z) (const size_type, const size_type, quregister&, quregister&);
    void (*kronecker)    (quregister&, quregister&, quregister&);
    int  (*measure)      (const size_type, const real, quregister&, quregister&);
    void (*normalize)    (quregister&, quregister&);
    void (*phase_kick)   (const size_type, const real, quregister&, quregister&);
    void (*copy)         (quregister& input, quregister& output);
    
    void implementation (std::string imp) {
        if (imp == "omp")
            QUANTUM_IMPLEMENTATION (openmp);
        if (imp == "seq")
            QUANTUM_IMPLEMENTATION (sequential);
        if (imp == "tbb")
            QUANTUM_IMPLEMENTATION (itbb);
    }
    
    std::ostream& operator << (std::ostream& out, const quregister& reg) {
        for (size_type i (0), n (reg.size()); i != n; ++i) {
            out << std::real(reg[i]) << " + " << std::imag(reg[i]) << "i |" << i << ">" << std::endl;
            }
        return out;
    }
    
    void print (const quregister& reg) {
        std::cout << reg;
    }

}

#endif
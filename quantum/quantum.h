#ifndef pqvm_quantum_h
#define pqvm_quantum_h

#include <iostream>

#include "types.h"

#include "openmp.h"
#include "sequential.h"
#include "tbb.h"

#define QUANTUM_IMPLEMENTATION(namespace)    \
    sigma_x      = &namespace::sigma_x,      \
    sigma_z      = &namespace::sigma_z,      \
    controlled_z = &namespace::controlled_z, \
    kronecker    = &namespace::kronecker,    \
    measure      = &namespace::measure       \

namespace quantum {
    
    void (*sigma_x)      (const size_type, quregister&, quregister&);
    void (*sigma_z)      (const size_type, quregister&, quregister&);
    void (*controlled_z) (const size_type, const size_type, quregister&, quregister&);
    void (*kronecker)    (quregister&, quregister&, quregister&);
    void (*measure)      (const size_type, const real, quregister&, quregister&);
    
    void implementation (std::string imp) {
        if (imp == "openmp")
            QUANTUM_IMPLEMENTATION (openmp);
        if (imp == "sequential")
            QUANTUM_IMPLEMENTATION (sequential);
        if (imp == "tbb")
            QUANTUM_IMPLEMENTATION (itbb);
    }
    
}

#endif
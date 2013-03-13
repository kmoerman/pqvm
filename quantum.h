#ifndef pqvm_quantum_h
#define pqvm_quantum_h

#include <iostream>
#include <complex>
#include <tbb/tbb.h>

#include "vector.h"

namespace quantum {
    
    namespace details {
        typedef float axis_type;
    }
    
    typedef std::complex<details::axis_type> amplitude;
    typedef vector<amplitude> reg;
    
    std::ostream& operator << (std::ostream& s, reg& r) {
        s << "( ";
        for (reg::const_iterator i (r.begin()), n (r.end()); i != n; ++i)
            s << *i << " ";
        s << ")" << std::endl;
        return s;
    }
        
    std::ostream& operator << (std::ostream& s, reg* r) {
        s << "( ";
        for (reg::const_iterator i (r->begin()), n (r->end()); i != n; ++i)
            s << (*i) << " ";
        s << ")" << std::endl;
        return s;
    }
    
    namespace details {
        
        struct kronecker {
            typedef typename reg::size_type size;
            const size m;
            const reg& a, b;
            reg* c;
            kronecker (const size m_, const reg& a_, const reg& b_, reg* c_) :
            m(m_), a(a_), b(b_), c(c_) {}
            void operator() (const tbb::blocked_range<size>& r) const {
                for (size i (r.begin()), k (i * m); i < r.end(); ++i)
                    for (size j (0); j < m; ++j, ++k)
                        (*c)[k] = a[i] * b[j];
            }
        };
        
    }
    
    reg* kronecker(reg& left, reg& right) {
        typedef typename reg::size_type size;
        typedef tbb::blocked_range<size> range;
        size    n (left.size()),
                m (right.size());
        
        reg* result = new reg(n * m);
        
        tbb::parallel_for(range(0, n),  details::kronecker {m, left, right, result});
        return result;
        
    }
    
}


#endif
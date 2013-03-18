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
    
    reg* kronecker (reg& left, reg& right) {
        typedef typename reg::size_type size;
        typedef tbb::blocked_range<size> range;
        size    n (left.size()),
                m (right.size());
        
        reg* result = new reg(n * m);
        
        tbb::parallel_for(range(0, n),  details::kronecker (m, left, right, result));
        return result;
    }
    
    /**
     * Finish single gate operators.
     * ...
     */
    namespace details {
        struct single_gate {
            typedef typename reg::size_type size;
            const reg* a;
            const axis_type* m;
            single_gate (axis_type* m_, reg* a_) :
            m(m_), a(a_) {}
            void operator() (const tbb::blocked_range<size>& r) const {
                for (size i (r.begin()); i < r.end(); ++i) ;
            }
        };
    }
    
    void hadamard (reg::size_type target, reg& r) {
        typedef typename reg::size_type size;
        typedef tbb::blocked_range<size> range;
        size    n (r.size());
        details::axis_type half_sqrt (std::sqrt(0.5));
        details::axis_type m[4] = {half_sqrt, half_sqrt, half_sqrt, -half_sqrt};
        
        tbb::parallel_for(range(0, n), details::single_gate (m, &r));
    }
    
    /**
     * Measurement. We measure in the |+alpha> |-alpha> basis (on the equator
     * of the Bloch sphere). For unitary opertors, it suffices to assume
     * the outcome signal zero and transform only the register.
     */
    namespace details {
        
        typedef reg::size_type size;
        
        template <typename F>
        void stride_operator (reg in, reg out, size stride, F f) {
            size period = 2 * stride;
            
            
            
        }
        
    }
    
    reg* measure (reg::size_type target, double angle, reg& r) {
        reg::size_type N = r.size();
        reg* outcome = new reg(N / 2);
        
        
        
        return outcome;
        
    }
    
}


#endif
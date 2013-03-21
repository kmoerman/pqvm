#ifndef pqvm_quantum_h
#define pqvm_quantum_h

#include <iostream>
#include <complex>
#include <tbb/tbb.h>

#include "vector.h"

namespace quantum {
    
    typedef float real;
    typedef std::complex<real> complex;
    typedef vector<complex> quregister;
    typedef quregister::iterator iterator;
    typedef quregister::size_type size_type;
    typedef const tbb::blocked_range<size_type> range;
        
    std::ostream& operator << (std::ostream& out, quregister& reg) {
        out << "( ";
        for (iterator i (reg.begin()), n (reg.end()); i != n; ++i)
            out << *i << " ";
        out << ")" << std::endl;
        return out;
    }
    
    
    
    /*
     * Pauli-X gate.
     * In place operation.
     * @TODO This jumps around in the input vector: turn into strided pattern.
     */
    
    namespace details {
        struct pauli_x {
            const size_type mask;
            const iterator input;
            
            pauli_x (size_type target_, quregister& input_) :
            input (input_.begin()), mask (1 << target_)) {}
            
            void operator() (range& r) {
                for (size_type i (r.begin()); i != r.end(); ++i)
                    if (i & bitmask == bitmask)
                        input[i] = input[i^mask];
            }
        };
    }
    
    
    /*
     * Pauli-Z gate.
     * In place operation.
     * @TODO Maybe we can do better than iterating the whole register?
     */
    
    namespace details {
        struct pauli_z {
            const size_type mask;
            const iterator input;
            
            pauli_z (size_type target_, quregister& input_) :
            input (input_.begin()), mask (1 << target_)) {}
            
            void operator() (range& r) {
                for (size_type i (r.begin()); i < r.end(); ++i)
                    if (i & bitmask == bitmask)
                        input[i] *= -1;
            }
        };
    }
    
    void pauli_z (size_type target, quregister& input) {
        tbb::parallel_for (range (0, input.size()), details::pauli_z (target, input));
    }
    
    /*
     * Controlled-Z gate.
     * In place operation.
     * @TODO Maybe we can do better than iterating the whole register?
     */
    
    namespace details {
        struct controlled_z {
            const size_type mask;
            const iterator input;
            
            controlled_z (const size_type control_, const size_type target_, const quregister& input_) :
            input (input_.begin()), mask ((1 << control_) | (1 << target_)) {}
            
            void operator() (range& r) {
                for (size i (r.begin()); i < r.end(); ++i)
                    if((i & bitmask) == bitmask)
                        input[i] *= -1;
            }
        };
    }
    
    void controlled_z (const size_type control, const size_type target, const quregister& input) {
        tbb::parallel_for (range (0, input.size()), details::controlled_z (control, target, input));
    };
    
    /*
     * Kronecker product.
     * Calculate the kronecker product of two vectors (size n and m).
     * Fills a vector of size n x m in parallel.
     * Straightforward implementation: spread threads accross the result vector
     * and then perform a double loop to calculate the results.
     */
    
    namespace details {
        struct kronecker {
            const iterator left, right, result;
            const size_type m;
            
            kronecker (quregister& left_, quregister& right_, quregister& result_) :
                left (left_.begin()), right (right_.begin()), result (result_.begin()), m (right_.size()) {}
            
            void operator() (range& r) const {
                for (size_type i (r.begin()), k (i * m); i != r.end(); ++i)
                    for (size_type j (0); j < m; ++j, ++k)
                        right[k] = left[i] * right[j];
            }
        };
    }
    
    void kronecker (quregister& left, quregister& right, quregister& result) {
        result.reserve(left.size() * right.size());
        tbb::parallel_for(range (0, left.size()),  details::kronecker (left, right, result));
    }
    
    /*
     * Measurement.
     * We measure in the (|+alpha>, |-alpha>) basis (on the equator
     * of the Bloch sphere). For unitary operators, it suffices to assume
     * the outcome signal zero (|+alpha>) and transform only the register.
     * 
     * Measurement is performed on a target qubit t, relative to an angle a. This
     * results in a strided access pattern on the input vector. FOr a given target t
     * (one-based), the stride s is given by s = 2^(t-1). We call p = 2s the stride
     * period.
     *
     * Example, measurement of a 3 qubit register A, for target bit 2.
     *
     *     s = 2, p = 4:
     *         E1  E2  O1  O2  E3  E4  O3  O4
     *        +---+---+---+---+---+---+---+---+
     *     A: | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | ...
     *        +---+---+---+---+---+---+---+---+
     *         |   |   .   .   /   /    .   .
     *         |   | .   .   /   / .   .
     *         |   |   .   /  ./  .
     *         | . | .   /   /
     *        +---+---+---+---+
     *     B: | 0 | 1 | 2 | 3 | ...
     *        +---+---+---+---+
     *
     *     B[k]  = A[Ek] + exp(-a*i) * A[Ok]
     *
     * The strided access pattern is resolved by iterating the result vector twice,
     * first copying the even amplitudes, then adding the odd amplitudes.
     *
     *     B[k]  = A[Ek]
     *     B[k] += exp(-a*i) * A[Ok]
     *
     * This should work well for large strides; for small strides, a single (parallel)
     * iteration might be faster. I assume the minimal stride period should equal the
     * cache line size, but this needs experimental validation.
     * The threads should be spread accross the result vector, aligned with the cache
     * and with the stride periods; this needs experimental validation.
     * @TODO Test effects of software prefetching.
     */
    
    namespace details {
        struct measure {
            const size_type target;
            const real angle;
            const iterator input, output;
            
            measure (size_type t_, real a_, quregister& i_, quregister& o_) :
                target (t_), angle (a_), input (i_.begin()), output (o_.begin()) {}
        };
        
        struct even : public measure {
            void operator() (range& r) const {
                size_type stride (1 << target - 1),
                          period (stride << 1),
                          i (r.begin()),
                          j ((k / stride) * period + (i % stride));
                
                for (; i != r.end(); ++i, i % stride ? ++j : j += period)
                    output[k] = input[i];
            }
        };
        
        struct odd : public measure {
            void operator() (range& r) const {
                size_type stride (1 << target - 1),
                          period (stride << 1),
                          i (r.begin()),
                          j ((i / stride) * period + (i % stride) + stride);
                complex factor (exp(complex (0, -angle)));
                
                for (; i != r.end(); ++i, i % stride ? ++j : j += period)
                    output[k] += factor * input[i];
            }
        };
    }
    
    void measure (size_type target, real angle, quregister& input, quregister& output) {
        size_type n (source.size() / 2);
        destination.reserve(n);
        details::measure m (target, angle, input, output);
        
        tbb::parallel_for(range (0, n), (details::even) m);
        tbb::parallel_for(range (0, n), (details::odd) m);
    }
    
}

#endif
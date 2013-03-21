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
     * Some quantum operators are implemented as strided access pattern. The vectors
     * are accessed twice, once for the even and once for the odd elements.
     *
     * For an operator on a target qubit t (zero based) in quregister A, the stride s
     * is given by s = 2^t; we call p = 2s the stride period. Example, for a 3-qubit
     * register A, on target qubit 1:
     *
     *     t = 1, s = 2, p = 4
     *
     *         E1  E2  O1  O2  E3  E4  O3  O4
     *        +---+---+---+---+---+---+---+---+
     *     A: | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
     *        +---+---+---+---+---+---+---+---+
     *        ' \___|___/   | ' \___|_'_/   |
     *        '     \_______/ '     \_'_____/
     *        '               '       '
     *        '               '       '
     *        '<----- p ----->'<- s ->'
     *
     * The odd/even access pattern permutes (E1 01 E2 O2 E3 O3 E4 O4) into
     * (E1 E2 E3 E4 O1 O1 O3 O4) without the need to copy a permutation vector first.
     *
     * This should work well for large strides; for small strides, a single (parallel)
     * iteration might be faster. I assume the minimal stride period should equal the
     * cache line size, but this needs experimental validation.
     * The threads should be spread accross the destination vector, aligned with the cache
     * and with the stride periods; this needs experimental validation.
     *
     * @TODO Apply software prefetching (and test its performace).
     *
     */
    
    /*
     * Sigma-X gate.
     *
     *     t = 1
     *
     *         E1  E2  O1  O2  E3  E4  O3  O4
     *        +---+---+---+---+---+---+---+---+
     *     A: | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
     *        +---+---+---+---+---+---+---+---+
     *        ' \   \  .   .  ' \   \  .   .  '
     *        '   \  .\  .    '   \  .\  .    '
     *        '    .\  .\     '    .\  .\     '
     *        '  .  . \   \   '  .  . \   \   '
     *        +---+---+---+---+---+---+---+---+
     *     B: | 2 | 3 | 0 | 1 | 6 | 7 | 4 | 5 |
     *        +---+---+---+---+---+---+---+---+
     *
     */
    
    namespace details {
        
        struct sigma_x {
            const size_type target;
            const iterator input, output;
            
            sigma_x (size_type target_, quregister& input_, quregister& output_) :
            input (input_.begin()), output (output_.begin()), target (target_) {}
        };
        
        struct sigma_x_even : public sigma_x {
            void operator () (range& r) {
                size_type stride (1 << target),
                          period (stride << 1),
                          i      (r.begin()),
                          offset (i % period);
                if (offset >= stride)
                    i += period - offset;
                
                while (i < r.end()) {
                    output[i + stride] = input[i];
                    i++;
                    if (i % stride) i+= stride;
                }
            }
        };
        
        struct sigma_x_odd : public sigma_x {
            void operator () (range& r) {
                size_type stride (1 << target),
                          period (stride << 1),
                          i      (r.begin()),
                          offset (i % period);
                if (offset < stride)
                    i += stride - offset;
                
                while (i < r.end()) {
                    output[i - stride] = input[i];
                    i++;
                    if (i % stride) i+= stride;
                }
            }
        };
    }
    
    void sigma_x (size_type target, quregister& input, quregister& output) {
        size_type n (input.size());
        output.reserve(n);
        details::sigma_x x (target, input, output);
        
        tbb::parallel_for (range (0, n), *(static_cast<details::sigma_x_even*>(&x)));
        tbb::parallel_for (range (0, n), *(static_cast<details::sigma_x_odd*>(&x)));
    }
    
    
    /*
     * Sigma-Z gate.
     *     
     *     t = 1
     *
     *        +---+---+---+---+---+---+---+---+
     *     A: | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
     *        +---+---+---+---+---+---+---+---+
     *          |   |   |   |   |   |   |   |
     *        +---+---+---+---+---+---+---+---+
     *     B: | 0 | 1 |-2 |-3 | 4 | 5 |-6 |-7 |
     *        +---+---+---+---+---+---+---+---+
     *
     * @TODO What happens with state |0...0> ?
     * @TODO Make in-place (easy, but depends on execution model of the pqvm).
     *
     */
    
    namespace details {
        struct sigma_z {
            const size_type mask;
            const iterator input, output;
            
            sigma_z (size_type target_, quregister& input_, quregister& output_) :
            input (input_.begin()), output (output_.begin()), mask (1 << target_) {}
            
            void operator() (range& r) {
                for (size_type i (r.begin()); i < r.end(); ++i)
                    if (i & mask) output[i] = -input[i];
                    else output[i] = input[i];
            }
        };
    }
    
    void sigma_z (size_type target, quregister& input, quregister& output) {
        size_type n (input.size());
        output.reserve(n);
        tbb::parallel_for (range (0, n), details::sigma_z (target, input, output));
    }
    
    /*
     * Controlled-Z gate.
     * 
     *     c = 0, t = 1
     *
     *        +---+---+---+---+---+---+---+---+
     *     A: | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
     *        +---+---+---+---+---+---+---+---+
     *          |   |   |   |   |   |   |   |
     *        +---+---+---+---+---+---+---+---+
     *     B: | 0 | 1 | 2 |-3 | 4 | 5 | 6 |-7 |
     *        +---+---+---+---+---+---+---+---+
     *
     * @TODO What happens with state |0...0> ?
     * @TODO Make in-place (easy, but depends on execution model of the pqvm).
     *
     */
    
    namespace details {
        struct controlled_z {
            const size_type mask;
            const iterator input, output;
            
            controlled_z (const size_type control_, const size_type target_, quregister& input_, quregister& output_) :
            input (input_.begin()), output(output_.begin()), mask ((1 << control_) | (1 << target_)) {}
            
            void operator() (range& r) {
                for (size_type i (r.begin()); i < r.end(); ++i)
                    if((i & mask) == mask) output[i] = -input[i];
                    else output[i] = input[i];
            }
        };
    }
    
    void controlled_z (const size_type control, const size_type target, quregister& input, quregister& output) {
        size_type n (input.size());
        output.reserve(n);
        tbb::parallel_for (range (0, n), details::controlled_z (control, target, input, output));
    };
    
    /*
     * Kronecker product.
     * Calculate the kronecker product of two vectors (size n and m).
     * Fills a vector of size n x m in parallel.
     * Straightforward implementation: spread threads accross the result vector
     * and then perform a double loop to calculate the results.
     *
     */
    
    namespace details {
        struct kronecker {
            const iterator left, right, result;
            const size_type m;
            
            kronecker (quregister& left_, quregister& right_, quregister& result_) :
                left (left_.begin()), right (right_.begin()), result (result_.begin()), m (right_.size()) {}
            
            void operator() (range& r) const {
                for (size_type i (r.begin()), k (i * m); i < r.end(); ++i)
                    for (size_type j (0); j < m; ++j, ++k)
                        right[k] = left[i] * right[j];
            }
        };
    }
    
    inline void kronecker (quregister& left, quregister& right, quregister& result) {
        result.reserve(left.size() * right.size());
        tbb::parallel_for(range (0, left.size()),  details::kronecker (left, right, result));
    }
    
    /*
     * Measurement.
     * We measure in the (|+alpha>, |-alpha>) basis (on the equator of the
     * Bloch sphere). For unitary operators, it suffices to assume the
     * outcome signal zero (|+alpha>) and transform only the register.
     * 
     * Measurement is performed on a target qubit t, relative to an angle a.
     *
     *     t = 1, s = 2, p = 4:
     *
     *         E1  E2  O1  O2  E3  E4  O3  O4
     *        +---+---+---+---+---+---+---+---+
     *     A: | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
     *        +---+---+---+---+---+---+---+---+
     *          |   |  .   .   /   /   .   .
     *          |   |.   .   /   /.   .
     *          |  .|  .   / . / .
     *          |.  |.  ./  ./
     *        +---+---+---+---+
     *     B: | 0 | 1 | 2 | 3 |
     *        +---+---+---+---+
     *     
     *     even: B[j]  = A[Ej]
     *     odd:  B[j] += exp(-a*i) * A[Oj]
     *
     */
    
    namespace details {
        
        
        void test () {}
        
        struct measure {
            const size_type target;
            const real angle;
            const iterator input, output;
            
            measure (size_type target_, real angle_, quregister& input_, quregister& output_) :
                target (target_), angle (angle_), input (input_.begin()), output (output_.begin()) {}
        };
        
        struct measure_even : public measure {
            void operator() (range& r) const {
                size_type stride (1 << target - 1),
                          period (stride << 1),
                          i (r.begin()),
                          j ((i / stride) * period + (i % stride));
                
                for (; i < r.end(); ++i, i % stride ? ++j : j += period)
                    output[j] = input[i];
            }
        };
        
        struct measure_odd : public measure {
            void operator() (range& r) const {
                size_type stride (1 << target - 1),
                          period (stride << 1),
                          i (r.begin()),
                          j ((i / stride) * period + (i % stride) + stride);
                complex factor (exp(complex (0, -angle)));
                
                for (; i < r.end(); ++i, i % stride ? ++j : j += period)
                    output[j] += factor * input[i];
            }
        };
    }
    
    void measure (size_type target, real angle, quregister& input, quregister& output) {
        size_type n (input.size() / 2);
        output.reserve(n);
        details::measure m (target, angle, input, output);
        
        tbb::parallel_for(range (0, n), *(static_cast<details::measure_even*>(&m)));
        tbb::parallel_for(range (0, n), *(static_cast<details::measure_odd*>(&m)));
    }
    
}

#endif
#ifndef pqvm_quantum_h
#define pqvm_quantum_h

#include <iostream>
#include <complex>
#include <tbb/tbb.h>

#include "../vector.h"

namespace quantum {
    
    typedef float real;
    typedef std::complex<real> complex;
    typedef vector<complex> quregister;
    typedef quregister::iterator iterator;
    typedef quregister::size_type size_type;
    typedef tbb::blocked_range<size_type> range;
    
    std::ostream& operator << (std::ostream& out, const quregister& reg) {
        out << "(";
        for (quregister::const_iterator i (reg.begin()), n (reg.end()); i != n; ++i) {
            out << std::real(*i) << "+" << std::imag(*i) << "i ";
        }
        out << ")" << std::endl;
        return out;
    }
    
    /*
     * Some quantum operators are implemented as strided access pattern. The vectors
     * are accessed twice, once for the even and once for the odd elements.
     *
     * For an operator on a target qubit t (zero based) in quregister Q, the stride s
     * is given by s = 2^t; we call p = 2s the stride period. Example: operator on
     * 3-qubit register, on target qubit 1:
     *
     *     t = 1, s = 2, p = 4
     *
     *         E1  E2  O1  O2  E3  E4  O3  O4
     *        +---+---+---+---+---+---+---+---+
     *     Q: | A | B | C | D | E | F | G | H |
     *        +---+---+---+---+---+---+---+---+
     *        ' \___|___/   | ' \___|_'_/   |
     *        '     \_______/ '     \_'_____/
     *        '               '       '
     *        '               '       '
     *        '<----- p ----->'<- s ->'
     *
     * The odd/even access pattern permutes (E1 E1 E2 O2 E3 O3 E4 O4) into
     * (E1 E2 E3 E4 O1 O1 O3 O4) without the need to copy a permutation vector first.
     *
     * This should work well for large strides; for small strides, a single iteration
     * might be faster. I assume the minimal stride period should equal the cache
     * line size, but this needs experimental validation.
     * The threads should be spread accross the destination vector, aligned with the
     * cache and with the stride periods. This can be achieved by setting the grainsize
     * parameter or set a partioner in tbb.
     *
     * @TODO Apply software prefetching (and test its performace).
     *
     */
    
    /*
     * Sigma-X gate.
     *
     *     t = 1
     *
     *         000 001 010 011 100 101 110 111
     *        +---+---+---+---+---+---+---+---+
     *     S: | A | B | C | D | E | F | G | H |
     *        +---+---+---+---+---+---+---+---+
     *        ' \   \  .   .  ' \   \  .   .  '
     *        '   \  .\  .    '   \  .\  .    '
     *        '    .\  .\     '    .\  .\     '
     *        '  .  . \   \   '  .  . \   \   '
     *        +---+---+---+---+---+---+---+---+
     *     D: | C | D | A | B | G | H | E | F |
     *        +---+---+---+---+---+---+---+---+
     *
     */
    
    namespace details {
        
        struct sigma_x_even {
            const size_type target;
            const iterator input, output;
            
            sigma_x_even (size_type target_, quregister& input_, quregister& output_) :
            input (input_.begin()), output (output_.begin()), target (target_) {}

            void operator () (range& r) const {
                size_type stride (1 << target),
                          period (stride << 1),
                          i      (r.begin()),
                          offset (i % period);
                if (offset >= stride)
                    i += period - offset;
                
                while (i < r.end()) {
                    output[i + stride] = input[i];
                    i++;
                    if (i % stride) continue;
                    else i+= stride;
                }
            }
        };
        
        struct sigma_x_odd {
            const size_type target;
            const iterator input, output;
            
            sigma_x_odd (size_type target_, quregister& input_, quregister& output_) :
            input (input_.begin()), output (output_.begin()), target (target_) {}

            void operator () (range& r) const {
                size_type stride (1 << target),
                          period (stride << 1),
                          i      (r.begin()),
                          offset (i % period);
                if (offset < stride)
                    i += stride - offset;
                
                while (i < r.end()) {
                    output[i - stride] = input[i];
                    i++;
                    if (i % stride) continue;
                    else i+= stride;
                }
            }
        };
    }
    
    void sigma_x (size_type target, quregister& input, quregister& output) {
        size_type n (input.size());
        output.reserve(n);
        details::sigma_x_even even (target, input, output);
        details::sigma_x_odd  odd  (target, input, output);

        tbb::parallel_for (range (0, n, 1024), even);
        tbb::parallel_for (range (0, n, 1024), odd);
    }
    
    
    /*
     * Sigma-Z gate.
     *     
     *     t = 1
     *
     *         000 001 010 011 100 101 110 111
     *        +---+---+---+---+---+---+---+---+
     *     S: | A | B | C | D | E | F | G | H |
     *        +---+---+---+---+---+---+---+---+
     *          |   |   |   |   |   |   |   |
     *        +---+---+---+---+---+---+---+---+
     *     D: | A | B |-C |-D | E | F |-G |-H |
     *        +---+---+---+---+---+---+---+---+
     *
     * @TODO What happens with state |0...0> ?
     *
     */
    
    namespace details {
        struct sigma_z {
            const size_type mask;
            const iterator input, output;
            
            sigma_z (size_type target_, quregister& input_, quregister& output_) :
            input (input_.begin()), output (output_.begin()), mask (1 << target_) {}
            
            void operator() (const range& r) const {
                for (size_type i (r.begin()); i < r.end(); ++i) {
                    if (i & mask) output[i] = -input[i];
                    else output[i] = input[i];
                    //tertiary...
                }
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
     *         000 001 010 011 100 101 110 111
     *        +---+---+---+---+---+---+---+---+
     *     S: | A | B | C | D | E | F | G | H |
     *        +---+---+---+---+---+---+---+---+
     *          |   |   |   |   |   |   |   |
     *        +---+---+---+---+---+---+---+---+
     *     D: | A | B | C |-D | E | F | G |-H |
     *        +---+---+---+---+---+---+---+---+
     *
     * @TODO What happens with state |0...0> ?
     *
     */
    
    namespace details {
        struct controlled_z {
            const size_type mask;
            const iterator input, output;
            
            controlled_z (const size_type control_, const size_type target_, quregister& input_, quregister& output_) :
            input (input_.begin()), output(output_.begin()), mask ((1 << control_) | (1 << target_)) {}
            
            void operator() (const range& r) const {
                for (size_type i (r.begin()); i < r.end(); ++i) {
                    if ((i & mask) == mask) output[i] = -input[i];
                    else output[i] = input[i];
                }
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
     */
    
    namespace details {
        struct kronecker {
            const iterator left, right, result;
            const size_type m;
            
            kronecker (quregister& left_, quregister& right_, quregister& result_) :
                left (left_.begin()), right (right_.begin()), result (result_.begin()), m (right_.size()) {}
            
            void operator() (const range& r) const {
                for (size_type i (r.begin()), k (i * m); i < r.end(); ++i)
                    for (size_type j (0); j < m; ++j, ++k)
                        result[k] = left[i] * right[j];
            }
        };
    }
    
    void kronecker (quregister& left, quregister& right, quregister& result) {
        result.reserve(left.size() * right.size());
        details::kronecker k (left, right, result);
        
        tbb::parallel_for(range (0, left.size()),  k);
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
     *          E   E   O   O   E   E   O   O
     *         000 001 010 011 100 101 110 111
     *        +---+---+---+---+---+---+---+---+
     *     S: | A | B | C | D | E | F | G | H |
     *        +---+---+---+---+---+---+---+---+
     *          |   |  .   .   /   /   .   .
     *          |   |.   .   /   /.   .
     *          |  .|  .   / . / .
     *          |.  |.  ./  ./
     *        +---+---+---+---+
     *     D: | I | J | K | L |
     *        +---+---+---+---+
     *         00  01  10  11
     *     
     *     even: D[Ej]  = A[j]
     *     odd:  D[Oj] += exp(-a*i) * A[j]
     *
     */
    
    namespace details {
        
        
        void test () {}
        
        struct measure_even {
            const size_type target;
            const real angle;
            const iterator input, output;
            
            measure_even (size_type target_, real angle_, quregister& input_, quregister& output_) :
            target (target_), angle (angle_), input (input_.begin()), output (output_.begin()) {}
            
            void operator() (const range& r) const {
                size_type stride (1 << target),
                          period (stride << 1),
                          i      (r.begin()),
                          j      (period * (i / stride) + stride + i % stride);
                
                while (i < r.end()) {
                    output[i] = input[j];
                    ++i;
                    ++j;
                    if (i % stride) continue;
                    else j+= stride;
                }
            }
        };
        
        struct measure_odd {
            const size_type target;
            const real angle;
            const iterator input, output;
            
            measure_odd (size_type target_, real angle_, quregister& input_, quregister& output_) :
            target (target_), angle (angle_), input (input_.begin()), output (output_.begin()) {}
            
            void operator() (const range& r) const {
                size_type stride (1 << target),
                          period (stride << 1),
                          i      (r.begin()),
                          j      (period * (i / stride) + i % stride);
                complex factor (exp(complex (0, -angle)));
                
                while (i < r.end()) {
                    output[i] += factor * input[j];
                    ++i;
                    ++j;
                    if (i % stride) continue;
                    else j+= stride;
                }
            }
        };
    }
    
    void measure (size_type target, real angle, quregister& input, quregister& output) {
        size_type n (input.size() / 2);
        output.reserve(n);
        
        details::measure_even even (target, angle, input, output);
        details::measure_odd  odd  (target, angle, input, output);
        
        tbb::parallel_for(range (0, n), even);
        
        tbb::parallel_for(range (0, n), odd);
    }
    
}

#endif
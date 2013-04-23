#ifndef pqvm_quantum_tbb_range_h
#define pqvm_quantum_tbb_range_h

#include "types.h"
#include <tbb/tbb.h>

namespace quantum { namespace itbb_range {
    
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
            target (target_), input (input_.begin()), output (output_.begin()) {}
            
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
            target (target_), input (input_.begin()), output (output_.begin()) {}
            
            void operator () (range& r) const {
                size_type stride (1 << target),
                period (2 * stride),
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
    
    void sigma_x (const size_type target, quregister& input, quregister& output) {
        size_type n (input.size());
        output.reserve(n);
        details::sigma_x_even even (target, input, output);
        details::sigma_x_odd  odd  (target, input, output);
        
        tbb::parallel_for (range (0, n, grainsize), even);
        tbb::parallel_for (range (0, n, grainsize), odd);
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
     */
    
    namespace details {
        struct sigma_z {
            const size_type mask;
            const iterator input, output;
            
            sigma_z (size_type target_, quregister& input_, quregister& output_) :
            mask (1 << target_), input (input_.begin()), output (output_.begin()) {}
            
            void operator() (const range& r) const {
                for (size_type i (r.begin()); i < r.end(); ++i) {
                    if (i & mask) output[i] = -input[i];
                    else output[i] = input[i];
                    //tertiary...
                }
            }
        };
    }
    
    void sigma_z (const size_type target, quregister& input, quregister& output) {
        size_type n (input.size());
        output.reserve(n);
        tbb::parallel_for (range (0, n, grainsize), details::sigma_z (target, input, output));
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
     */
    
    namespace details {
        struct controlled_z {
            const size_type mask;
            const iterator input, output;
            
            controlled_z (const size_type control_, const size_type target_, quregister& input_, quregister& output_) :
            mask ((1 << control_) | (1 << target_)), input (input_.begin()), output(output_.begin()) {}
            
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
        tbb::parallel_for (range (0, n, grainsize), details::controlled_z (control, target, input, output));
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
            const size_type m;
            const iterator left, right, result;
            
            kronecker (quregister& left_, quregister& right_, quregister& result_) :
            m (right_.size()), left (left_.begin()), right (right_.begin()), result (result_.begin()) {}
            
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
        
        tbb::parallel_for(range (0, left.size(), grainsize),  k);
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
     *     even: D[j]  = A[Ej]
     *     odd:  D[j] += exp(-a*i) * A[Oj]
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
                j      ((i / stride) * period + (i % stride));
                
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
                j      ((i / stride) * period + (i % stride) + stride);
                
                complex   factor (std::exp(complex (0, -angle)));
                
                while (i < r.end()) {
                    output[i] -= input[j] * factor;
                    ++i;
                    ++j;
                    if (i % stride) continue;
                    else j+= stride;
                }
            }
        };
    }
    
    int measure (const size_type target, const real angle, quregister& input, quregister& output) {
        size_type n (input.size() / 2);
        output.reserve(n);
        
        details::measure_even even (target, angle, input, output);
        details::measure_odd  odd  (target, angle, input, output);
        
        tbb::parallel_for(range (0, n, grainsize), even);
        tbb::parallel_for(range (0, n, grainsize), odd);
        
        return 1;
    }
    
    /*
     * Copy.
     */
    namespace details {
        
        struct copy {
            iterator input, output;
            
            copy (quregister& input_, quregister& output_) :
            input(input_.begin()), output(output_.begin()) {}
            
            void operator () (const range& r) const {
                for (size_type i (r.begin()); i < r.end(); ++i)
                    output[i] = input[i];
            }
            
        };
        
    }
    
    void copy (quregister& input, quregister& output) {
        size_type n (input.size());
        
        output.reserve(n);
        
        tbb::parallel_for(range (0, n, grainsize), details::copy (input, output));
    }
    
    void initialize () {
        tbb::task_scheduler_init i;
    }
    
    /*
     * Normalize.
     * The sum of the amplitudes should equal 1.
     */
    
    namespace details {
        struct norm {
            real total;
            iterator input;
            norm (quregister& input_) :
            total (0), input(input_.begin()) {}
            
            norm (norm& origin, tbb::split) :
            total (0), input (origin.input) {}
            
            void operator () (const range& r) {
                for (size_type i (r.begin()); i < r.end(); ++i)
                    total += std::norm(input[i]);
            }
            
            void join (norm& rhs) {
                total += rhs.total;
            }
            
        };
        
        struct normalize {
            real norm;
            iterator input, output;
            
            normalize (const real n, quregister& input_, quregister& output_) :
            norm(n), input(input_.begin()), output(output_.begin()) {}
            
            void operator () (const range& r) const {
                for (size_type i (r.begin()); i < r.end(); ++i)
                    output[i] = input[i] / norm;
            }
        };
    }
    
    void normalize (quregister& input, quregister& output) {
        size_type n (input.size());
        output.reserve(n);
        real limit = 1.0e-8;
        
        details::norm norm (input);
        
        tbb::parallel_reduce(range (0, n, grainsize), norm);
        if (std::abs(1 - norm.total) > limit)
            tbb::parallel_for(range (0, n, grainsoze), details::normalize (norm.total, input, output));
        else copy(input, output);
    }
    
    /*
     * Phase-kick.
     */
    
    namespace details {
        struct phase_kick {
            size_type target;
            real gamma;
            iterator input, output;
            
            phase_kick (size_type target_, real gamma_, quregister& input_, quregister& output_) :
            target (target_), gamma(gamma_), input (input_.begin()), output (output_.begin()) {}
            
            void operator () (const range& r) const {
                size_type   mask    (1 << target);
                complex     factor  (std::conj(std::exp(complex(0, gamma))));
                
                for (size_type i (r.begin()); i <r.end(); ++i)
                    output[i] = (i & mask) ? factor * input[i] : input[i];
            }
            
        };
        
    }
    
    void phase_kick (size_type target, real gamma, quregister& input, quregister& output) {
        size_type n (input.size());
        
        output.reserve(n);
        
        tbb::parallel_for(range (0, n, grainsize), details::phase_kick (target, gamma, input, output));
    }
    
} }

#endif
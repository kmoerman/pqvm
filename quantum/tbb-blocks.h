#ifndef pqvm_quantum_tbb_blk_h
#define pqvm_quantum_tbb_blk_h

#include "types.h"
#include <tbb/tbb.h>
#include <cstring>
#include <iostream>

#define qb_min(a, b) (a < b ? a : b)
#define qb_max(a, b) (a > b ? a : b)

/*
 * A quantum back end based on block movement of data
 * Elemenitas all unnecesary data movement.
 */

namespace quantum { namespace itbb_blk {
    
    typedef tbb::blocked_range<size_type> range;
    
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
     *          E1  E2  O1  O2  E3  E4  O3  O4
     *         000 001 010 011 100 101 110 111
     *        +---+---+---+---+---+---+---+---+
     *     Q: | A | B | C | D | E | F | G | H |
     *        +---+---+---+---+---+---+---+---+
     *        ' \___|___/   | ' \___|_'_/   |
     *        '     \_______/ '     \_'_____/
     *        '               '       '
     *        '               '       '
     *        '<----- p ----->'<- s ->'
     *
     * possible range divisions:
     *        |<----->|<->|<->|<->|<->|<----->|
     *        |<------------->|<------------->|
     *
     * impossible range division:
     *        |<----->|<------------->|<----->|
     *
     *
     * As the state vectors are always sized in powers of 2 and TBB always halves the
     * range, we can safely assume the range either fully lies within a single stride,
     * or contains only full periods.
     *
     * The odd/even access pattern permutes (E1 E1 E2 O2 E3 O3 E4 O4) into
     * (E1 E2 E3 E4 O1 O1 O3 O4) without the need to copy a permutation vector first.
     * This should work well for large strides; I assume the minimal stride period
     * should equal the cache line size, but this needs experimental validation.
     * 
     * The threads should be spread accross the destination vector, aligned with the
     * cache and with the stride periods. This can be achieved by setting the grainsize
     * parameter or set a partioner in tbb.
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
            
            sigma_x_even (size_type t_, quregister& i_, quregister& o_) :
            target (t_), input (i_.begin()), output (o_.begin()) {}
            
            void operator () (range& r) const {
                size_type stride = 1 << target,
                          period = stride << 1,
                          i      = r.begin(),
                          n      = r.size();
                
                //When range is fully within a stride, copy the entire range
                //to the right, but only when i is at an even position.
                if (n < period) {
                    if (!(i & stride))
                        memcpy(output + i + stride, input + i, n * sizeof(complex));
                    return;
                }
                
                //When the range contains multiple periods, copy the even
                //amplitudes in each period to the right.
                size_type block  = stride * sizeof(complex),
                          blocks = n / period;
                iterator  ipt    = input  + i,
                          opt    = output + i + stride;
                
                while (blocks > 0) {
                    memcpy(opt, ipt, block);
                    opt += period;
                    ipt += period;
                    --blocks;
                }
            }
        };
        
        struct sigma_x_odd {
            const size_type target;
            const iterator input, output;
            
            sigma_x_odd (size_type t_, quregister& i_, quregister& o_) :
            target (t_), input (i_.begin()), output (o_.begin()) {}
            
            void operator () (range& r) const {
                size_type stride = 1 << target,
                          period = stride << 1,
                          i      = r.begin(),
                          n      = r.size();
                
                //When range is fully within a stride, copy the entire range
                //to the left, but only when i is at an odd position.
                if (n < period) {
                    if (i & stride)
                        memcpy(output + i - stride, input + i, n * sizeof(complex));
                    return;
                }
                
                //When the range contains multiple periods, copy the odd
                //amplitudes in each period to the left.
                size_type block  = stride * sizeof(complex),
                          blocks = n / period;
                iterator  ipt    = input  + i + stride,
                          opt    = output + i;
                
                while (blocks) {
                    memcpy(opt, ipt, block);
                    opt+=period;
                    ipt+=period;
                    --blocks;
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
            const size_type target;
            const iterator input;
            
            sigma_z (size_type target_, quregister& input_) :
            target (target_), input (input_.begin()) {}
            
            void operator() (const range& r) const {
                size_type i      = r.begin(),
                          n      = r.end(),
                          size   = n - i,
                          block  = 1 << target,
                          period = 2 * block;
                
                //range contains at least one period
                if (size >= period)
                    //skip even target blocks
                    for (i += block; i < n; i += block)
                    //loop amplitudes in odd target block
                    for (size_type j = 0; j < block; ++j, ++i)
                        input[i] *= -1;
                            
                    //range contained in a period
                    else
                        //skip if in even target block
                        if (i & block)
                            //loop ampliutdes in add target block
                            for (; i < n; ++i)
                                input[i] *= -1;

            }
        };
    }
    
    void sigma_z (const size_type target, quregister& input, quregister& output) {
        size_type n (input.size());
        
        tbb::parallel_for (range (0, n, grainsize), details::sigma_z (target, input));
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
            const size_type control, target;
            const iterator input;
            
            controlled_z (const size_type control_, const size_type target_, quregister& input_) :
            control (qb_max(control_, target_)), target (qb_min(control_, target_)), input (input_.begin()) {}
            
            void operator() (const range& r) const {
                size_type i        = r.begin(),
                          n        = r.end(),
                          size     = n - i,
                          c_block  = 1 << control,
                          c_period = 2 * c_block,
                          t_block  = 1 << target,
                          t_period = 2 * t_block;
                
                //range contains at least one control period
                if (size >= c_period)
                    //skip even control blocks
                    for (i += c_block + t_block; i < n; i += c_block)
                    //skip even target blocks
                    for (size_type j = 0; j < c_block; j += t_period, i += t_block)
                    //loop amplitudes in odd target block
                    for (size_type k = 0; k < t_block; ++k, ++i)
                        input[i] *= -1;
                        
                //range contained in a control period
                else
                    //skip if in even control block
                    //if odd block: same as normal z
                    if (i & c_block) {
                        if (size >= t_period)
                            for (i += t_block; i < n; i += t_block)
                            for (size_type j = 0; j < t_block; ++j, ++i)
                                input[i] *= -1;
                        else
                            if (i & t_block)
                                for (; i < n; ++i)
                                    input[i] *= -1;
                    }

            }
        };
    }
    
    void controlled_z (const size_type control, const size_type target, quregister& input, quregister& output) {
        size_type n (input.size());
        tbb::parallel_for (range (0, n, grainsize), details::controlled_z (control, target, input));
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
     * Bloch sphere). We only measure to the + state.
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
            tbb::parallel_for(range (0, n, grainsize), details::normalize (norm.total, input, output));
        else copy(input, output);
    }
    
    /*
     * Phase-kick.
     * Dead code.
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
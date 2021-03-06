#ifndef pqvm_quantum_openmp_h
#define pqvm_quantum_openmp_h

#include "types.h"

/*
 * A quantum backend based on OpenMP
 */

namespace quantum { namespace openmp {
    
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
    
    void sigma_x (const size_type target, quregister& input, quregister& output) {
        size_type   stride  (1 << target),
                    n       (input.size()),
                    period  (stride << 1);
        
        output.reserve(n);
        
        //even
        #pragma omp parallel for
        for (size_type i = 0; i < n; i += period)
            for (size_type j = 0; j < stride; ++j)
                output[i + j + stride] = input[i + j];
        
        //odd
        #pragma omp parallel for
        for (size_type i = 0; i < n; i += period)
            for (size_type j = 0; j < stride; ++j)
                output[i + j] = input[i + j + stride];
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
    
    void sigma_z (size_type target, quregister& input, quregister& output) {
        size_type   n       (input.size()),
                    mask    (1 << target);
        
        output.reserve(n);
        
        #pragma omp parallel for
        for (size_type i = 0; i < n; ++i)
            output[i] = (i & mask) ? -input[i] : input[i];
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
    
    
    void controlled_z (const size_type control, const size_type target, quregister& input, quregister& output) {
        size_type   n       (input.size()),
                    mask    ((1 << control) | (1 << target));
        
        output.reserve(n);
        
        #pragma omp parallel for
        for (size_type i = 0; i < n; ++i)
            output[i] = ((i & mask) == mask) ? -input[i] : input[i];
        
    };
    
    /*
     * Kronecker product.
     * Calculate the kronecker product of two vectors (size n and m).
     * Fills a vector of size n x m in parallel.
     * Straightforward implementation: spread threads accross the result vector
     * and then perform a double loop to calculate the results.
     */
    
    void kronecker (quregister& left, quregister& right, quregister& result) {
        size_type   n   (left.size()),
                    m   (right.size());
        
        result.reserve(n * m);
        
        size_type k = 0;
        #pragma omp parallel for
        for (size_type i = 0; i < n; ++i)
            for (size_type j = 0; j < m; ++j, ++k)
                result[k] = left[i] * right[j];
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
     *     even: D[j]  = A[Oj]
     *     odd:  D[j] += exp(-a*i) * A[Oj]
     *
     */
    
    int measure (const size_type target, const real angle, quregister& input, quregister& output) {
        size_type   n       (input.size()),
                    stride  (1 << target),
                    period  (stride << 1);
        complex     factor  (std::exp(complex (0, -angle)));
        
        output.reserve(n/2);
        
        //even
        size_type k = 0;
        #pragma omp parallel for
        for (size_type i = 0; i < n; i += period)
            for (size_type j = 0; j < stride; ++j, ++k)
                output[k] = input[i + j];
        
        
        //odd
        k = 0;
        #pragma omp parallel for
        for (size_type i = 0; i < n; i += period)
            for (size_type j = stride; j < period; ++j, ++k)
                output[k] -= input[i + j] * factor;
        
        return 1;
    }
    
    /*
     * Copy.
     */
    void copy (quregister& input, quregister& output) {
        size_type n (input.size());
        
        output.reserve(n);
        
        #pragma omp parallel for
        for (size_type i = 0; i < n; ++i)
            output[i] = input[i];
    }
    
    /*
     * Normalize.
     * The sum of the amplitudes should equal 1.
     */
    
    void normalize (quregister& input, quregister& output) {
        size_type n (input.size());
        output.reserve(n);
        
        real norm = 0, limit = 1.0e-8;
        #pragma omp parallel for reduction(+:norm)
        for (size_type i = 0; i < n; ++i)
            norm += std::norm(input[i]);
        

        if (std::abs(1 - norm) > limit) {
            #pragma omp parallel for
            for (size_type i = 0; i < n; ++i)
                output[i] = input[i] / norm;
        }
        else copy(input, output);
    }
    
    /*
     * Phase-kick.
     */
    void phase_kick (size_type target, real gamma, quregister& input, quregister& output) {
        size_type   n       (input.size()),
                    mask    (1 << target);
        complex     factor  (std::conj(std::exp(complex(0, gamma))));
        
        output.reserve(n);
        
        #pragma omp parallel for
        for (size_type i = 0; i < n; ++i)
            output[i] = (i & mask) ? factor * input[i] : input[i];
        
    }
    
    void initialize () {}
    
} }

#endif


#include "qops.h"

/*in quantum.h: sparse version
struct quantum_reg_node_struct
{
    COMPLEX_FLOAT amplitude; // alpha_j
    MAX_UNSIGNED state;  // j
};

typedef struct quantum_reg_node_struct quantum_reg_node;


struct quantum_reg_struct
{
    int width; //numbers of qubits in qureg
    int size; // number of non-zero vectors
    int hashw;  // width of hash array
    quantum_reg_node *node; //sparse node array
    int *hash; //hash array
};

typedef struct quantum_reg_struct quantum_reg;

*/

// replace by dense version:
struct quantum_reg_struct {
    int size;
    COPMLEX_FLOAT* amplitudes
}

typedef quantum_reg_struct quantum_reg;

//basic opeations:
struct qop_cz_pll {
    quantum_reg* reg;
    MAX_UNSIGNED bitmask;
    
    qop_cz_pll (quantum_reg* r, MAX_UNSIGNED b):
    reg(r), bitmask(b) {}
    
    void operator() (const tbb::blocked_range<int>& r) {
        for (int i = r.begin(); i < r.end(); ++i)
            if((reg->node[i].state & bitmask) == bitmask)
                reg->node[i].amplitude *= (COMPLEX_FLOAT)-1;
    }
    
}

void qop_cz (const qubit_t qubit_1, const qubit_t qubit_2 ) {
    const int tar1 = get_target(qubit_1);
    const int tar2 = get_target(qubit_2);
    assert( !(invalid(qubit_1) || invalid(qubit_2)) );
    assert( qubit_1.tangle == qubit_2.tangle );
    
    quantum_reg* reg = get_qureg( qubit_1 );
    MAX_UNSIGNED bitmask = 
    ((MAX_UNSIGNED) 1 << tar1) | ((MAX_UNSIGNED) 1 << tar2);
    
    parallel_for (tbb::blocked_range<int>(0, reg->size), qop_cz_pll(reg, bitmask));
};

struct qop_z_pll {
    qunatum_reg* reg;
    MAX_UNSIGNED bitmask;
    
    qop_z_pll (quantum_reg* r, MAX_UNSIGNED b):
    reg(r), bitmask(b) {}
    
    void operator() (const tbb::blocked_range<int>& r) {
        for (int i = r.begin(); i < r.end(); ++i)
            if (i & bitmask == bitmask)
                reg->node[i] *= -1;
    }
    
};

void qop_z (const qubit_t qubit ) {
    const int tar1 = get_target(qubit);
    assert( !(invalid(qubit_1) );
    
    quantum_reg* reg = get_qureg( qubit_1 );
    MAX_UNSIGNED bitmask = (MAX_UNSIGNED) 1 << tar1);
    
    parallel_for (tbb::blocked_range<int>(0, reg->size), qop_z_pll(reg, bitmask));

}

struct qop_x_pll {
    qunatum_reg* reg;
    MAX_UNSIGNED bitmask;
    
    qop_x_pll (quantum_reg* r, MAX_UNSIGNED b):
    reg(r), bitmask(b) {}
    
    void operator() (const tbb::blocked_range<int>& r) {
        for (int i = r.begin(); i < r.end(); ++i)
            if (i & bitmask == bitmask)
                reg->node[i] = reg->node[i^bitmask];
    }
    
};

void qop_x (const qubit_t qubit ) {
    const int tar1 = get_target(qubit);
    assert( !(invalid(qubit_1) );
    
    quantum_reg* reg = get_qureg( qubit_1 );
    MAX_UNSIGNED bitmask = (MAX_UNSIGNED) 1 << tar1);
    
    parallel_for (tbb::blocked_range<int>(0, reg->size), qop_x_pll(reg, bitmask));
    
}
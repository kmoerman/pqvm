#include "../../../local/include/quantum.h"

int main (int argc, char** argv) {
    
    quantum_reg _proto_diag_qubit_;
    quantum_reg _proto_dual_diag_qubit_;
    quantum_matrix _cz_gate_ = 
    { 4,4, (COMPLEX_FLOAT[16]){1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,-1} };
    
    _proto_diag_qubit_ = quantum_new_qureg(0, 1);
    _proto_dual_diag_qubit_ = quantum_new_qureg(0, 2);
    quantum_hadamard(0, &_proto_diag_qubit_);
    quantum_hadamard(0, &_proto_dual_diag_qubit_);
    quantum_hadamard(1, &_proto_dual_diag_qubit_);
    quantum_gate2(0, 1, _cz_gate_, &_proto_dual_diag_qubit_);
    
    quantum_print_qureg(_proto_diag_qubit_);
    quantum_print_qureg(_proto_dual_diag_qubit_);
    
    return 0;

}
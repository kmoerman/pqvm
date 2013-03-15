
#include <iostream>
#include "quantum.h"
#include "measure.h"

using namespace quantum;

#define SIZE 1024

reg A (SIZE), B (SIZE);
reg* C;

int main (int argc, char** argv) {
    
    //fill
    for (int i = 0; i < SIZE; ++i) {
        A[i] = amplitude(i, 2);
        B[i] = amplitude(i, -2);
    }
    
    //measure
    measure_parallel ("kronecker-test.data", 10)
        C = kronecker(A, B);
    
    //bye bye
    return EXIT_SUCCESS;
    
}
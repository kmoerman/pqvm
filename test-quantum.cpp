#include <iostream>
#include <cstdlib>
#include <ctime>

#include "measure.h"
#include "quantum.h"


using namespace quantum;

int main (int argc, char** argv) {

    srand(time(NULL));
    quregister a (4096);
    quregister b (4096);
    quregister c (4096*4096);
    quregister d (2048);
    
    for (iterator i (a.begin()), j (b.begin()); i < a.end(); ++i, ++j) {
        *i = complex (rand()/RAND_MAX, rand()/RAND_MAX);
        *j = complex (rand()/RAND_MAX, rand()/RAND_MAX);
    }
    
    measure_parallel ("kronecker.data", 5) {
        quantum::kronecker(a, b, c);
    }
    
    measure_parallel ("measurement.data", 5) {
        quantum::measure (10, M_PI_2, a, d);
    }
    
}
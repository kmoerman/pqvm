#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../performance.h"
#include "../quantum/openmp.h"

using namespace quantum;

int main (int argc, char** argv) {
    
    
    if (argc < 2 || argc > 3) {
        std::cout << "Usage: sigma-x problem-size target [iterations=5]" << std::endl;
        return EXIT_FAILURE;
    }
    
    size_type size = atoi(argv[1]),
            target = atoi(argv[2]);
    int iterations = (argc == 4) ? atoi(argv[3]) : 5;
    
    srand(time(NULL));
    
    quregister a (size),
               b;
    
    for (iterator i (a.begin()); i < a.end(); ++i) {
        *i = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
    }
    
    //std::cout << a;
    
    //measure_sequential ("sigma-x-omp.data", iterations) {
        sigma_x(target, a, b);
    //}
    
    //std::cout << b;
    
    return 0;
    
}

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../performance.h"
#include "../quantum/sequential.h"

using namespace quantum;

int main (int argc, char** argv) {
    
    
    if (argc < 2 || argc > 3) {
        std::cout << "Usage: sigma-z problem-size [iterations=5]" << std::endl;
        return EXIT_FAILURE;
    }
    
    size_type size = atoi(argv[1]);
    int iterations = (argc == 3) ? atoi(argv[2]) : 5;
    
    srand(time(NULL));
    
    quregister a (size),
               b (size),
               c;
    
    for (iterator i (a.begin()), j (b.begin()); i < a.end(); ++i, ++j) {
        *i = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
        *j = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
    }
    
    std::cout << a;
    std::cout << b;
    
    //measure_sequential ("kronecker-omp.data", iterations) {
        kronecker(a, b, c);
    //}
    
    std::cout << c;
        
    return 0;
    
}
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../performance.h"
#include "../quantum.h"


using namespace quantum;

int main (int argc, char** argv) {
    
    if (argc < 2 || argc > 3) {
        std::cout << "Usage: quantum problem-size [iterations=5]" << std::endl;
        return EXIT_FAILURE;
    }
    
    size_type size = atoi(argv[1]);
    int iterations = (argc == 3) ? atoi(argv[2]) : 5;
    
    srand(time(NULL));
    
    quregister a (size),
               b (size);
    
    for (iterator i (a.begin()), j (b.begin()); i < a.end(); ++i, ++j) {
        *i = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
        //*j = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
    }
    
    measure_parallel("sigma_x.data", 5) {
        sigma_x(iterations, a, b);
    }
    
    return 0;
    
}
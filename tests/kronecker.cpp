#include <string>
#include <iostream>

#include "../performance.h"
#include "../quantum/quantum.h"

using namespace quantum;

int main (int argc, char** argv) {
    
    if (argc < 2 || argc > 4) {
        std::cout << "Usage: kronecker problem-size [implementation=tbb [iterations=3]]" << std::endl;
        return EXIT_FAILURE;
    }
    
    size_type  size = atoi(argv[1]);
    int        iter = (argc > 3) ? atoi(argv[3]) : 3;
    std::string imp = (argc > 2) ? argv[2] : "tbb";
    
    std::string file = "kronecker-";
    file += imp + ".data";
    
    srand(time(NULL));
    implementation(imp);
    
    quregister a (size),
               b (size),
               c;
    
    for (iterator i (a.begin()), j (b.begin()); i < a.end(); ++i, ++j) {
        *i = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
        *j = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
    }
    
    measure_parallel (file, iter) {
        kronecker(a, b, c);
    }
    
    return 0;
    
}
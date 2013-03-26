#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../performance.h"
#include "../quantum/quantum.h"

using namespace quantum;

int main (int argc, char** argv) {
    
    if (argc < 2 || argc > 4) {
        std::cout << "Usage: kronecker problem-size [implementation=tbb [iterations=5]]" << std::endl;
        return EXIT_FAILURE;
    }
    
    size_type  size = atoi(argv[1]),
    char       iter = (argc > 3) ? atoi(argv[3]) : 5;
    std::string imp = (argc > 2) ? argv[2] : "tbb";
    
    srand(time(NULL));
    implementation(imp);
    
    quregister a (size),
               b (size),
               c;
    
    for (iterator i (a.begin()), j (b.begin()); i < a.end(); ++i, ++j) {
        *i = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
        *j = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
    }
    
    //measure_sequential ("kronecker-"+imp+".data", iterations) {
        kronecker(a, b, c);
    //}
        
    return 0;
    
}
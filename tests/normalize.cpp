#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../performance.h"
#include "../quantum/quantum.h"

using namespace quantum;

int main (int argc, char** argv) {
    
    if (argc < 2 || argc > 5) {
        std::cout << "Usage: normalize problem-size [implementation=tbb [iterations=10]]" << std::endl;
        return EXIT_FAILURE;
    }
    
    size_type  size = atoi(argv[1]);
    char       iter = (argc > 3) ? atoi(argv[3]) : 10;
    std::string imp = (argc > 2) ? argv[2] : "tbb";
    
    std::ostringstream filestr;
    filestr << "data/normalize-" << size << "-" << imp << ".data";
    std::string file = filestr.str();
    
    srand(time(NULL));
    implementation(imp);
    
    quregister a (size),
               b;
    
    for (iterator i (a.begin()); i < a.end(); ++i) {
        *i = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
    }
    
    performance::init();
    
    if (imp == "tbb" || imp == "tbb_ran")
        measure_parallel (file, iter, false)
            normalize(a, b);
    else
        measure_sequential (file, iter, false)
            normalize(a, b);
    
    return 0;
    
}
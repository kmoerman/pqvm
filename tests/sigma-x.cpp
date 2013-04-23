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
        std::cout << "Usage: sigma-x problem-size target [implementation=tbb [iterations=10]]" << std::endl;
        return EXIT_FAILURE;
    }
    
    size_type  size = atoi(argv[1]),
             target = atoi(argv[2]);
    char       iter = (argc > 4) ? atoi(argv[4]) : 10;
    std::string imp = (argc > 3) ? argv[3] : "tbb";
    
    std::ostringstream filestr;
    filestr << "data/sigma-x-" << size << "-" << target << "-" << imp << ".data";
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
            sigma_x(target, a, b);
    else
        measure_sequential (file, iter, false)
            sigma_x(target, a, b);
    
    return 0;
    
}
#include <string>
#include <iostream>

#include "../performance.h"
#include "../quantum/quantum.h"

using namespace quantum;

int main (int argc, char** argv) {
    
    if (argc < 2 || argc > 4) {
        std::cout << "Usage: grainsize problem-size target [start-grainsize=1000000]" << std::endl;
        return EXIT_FAILURE;
    }
    
    size_type  size  = atoi(argv[1]),
               target = atoi(argv[2]);
    int        gsize = (argc > 3) ? atoi(argv[3]) : 1000000;
    
    std::ostringstream filestr;
    filestr << "data/grainsize-" << size << ".data";
    std::string file = filestr.str();
    
    srand(time(NULL));
    
    implementation("tbb_ran");
    
    quregister a (size),
               b;
    
    for (iterator i (a.begin()); i < a.end(); ++i) {
        *i = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
    }
    
    performance::init();
    
    decrease_grainsize(file, gsize)
        sigma_x(target, a, b);
    
    return 0;
    
}
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../performance.h"
#include "../quantum/quantum.h"
#include "../options.h"

using namespace quantum;

int main (int argc, char** argv) {
    
    //default options
    int num_qubits = 10; //q
    int num_repeat = 1;  //r
    std::string imp = "tbb"; //i
    std::string file = "kronecker-speedup.data"; //f
    bool measure = false; //f
    bool verbose = false; //v
    set_grainsize (128); //g
    
    //get options
    int option;
    while ((option = getopt (argc, argv, "q:r:i:f:p:vg:")) != -1) {
        switch (option) {
            case 'q':
                num_qubits = parseopt<int>();
                break;
            case 'r':
                num_repeat = parseopt<int>();
                break;
            case 'i':
                imp = parseopt<std::string>();
                break;
            case 'f':
                measure = true;
                file = parseopt<std::string>();
                break;
            case 'p':
                performance::set_threads(parseopt<int>());
                break;
            case 'v':
                verbose = true;
                break;
            case 'g':
                set_grainsize(parseopt<size_type>());
                break;
        }
    }
    
    //initialize random state
    srand(time(NULL));
    implementation(imp);
    
    size_type size = 1 << num_qubits;
    
    quregister a (size),
               b (size),
               c;
    
    for (iterator i (a.begin()), j (b.begin()); i < a.end(); ++i, ++j) {
        *i = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
        *j = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
    }
    
    performance::init();
    
    if (verbose) {
        std::cout
            << "Running kronecker on "
            << num_qubits << " qubits" << std::endl
            << "Input state vectors each contain "
            << size << " amplitudes, "
            << ((double)(size * sizeof(complex))/(1024*1024))
            << "MiB" << std::endl
            << "Result state contains "
            << size * size << " amplitudes, "
            << ((double)(size * size * sizeof(complex))/(1024*1024))
            << "MiB" << std::endl;
    }
    
    //measure speedup
    if (measure) {
        if (imp == "tbb")
            measure_parallel (file, num_repeat, verbose)
                kronecker(a, b, c);
        
        else
            measure_sequential (file, num_repeat, verbose)
                kronecker(a, b, c);
    }
    
    //or just execute operation
    else
        for (int i = 1; num_repeat > 0; --num_repeat) {
            if (verbose) std::cout << "iteration " << i++ << std::endl;
            kronecker(a, b, c);
        }
    
    return 0;
    
}
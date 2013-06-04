#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../performance.h"
#include "../quantum/quantum.h"
#include "../options.h"

using namespace quantum;

/*
 * The the performance of the Z operator
 * options:
 *   q  number of qubits
 *   r  number of iterations (set high to overcome init times)
 *   i  select  quantum backend implementation
 *   f  output filename
 *   t  target qubit number
 *   v  verbose output
 *   g  grainsize
 *   s  random seed, to obtain same results twice
 *   o  display the statevector (before and after) output on screen
 *   p  explicitly set the number of threads
 */

int main (int argc, char** argv) {
    
    //default options
    int num_qubits = 20; //q
    int num_repeat = 1;  //r
    std::string imp = "tbb"; //i
    std::string file = "sigma-z-speedup.data"; //f
    bool measure = false; //f
    size_type target = 10; //t
    bool verbose = false; //v
    set_grainsize (512); //g
    uint seed = (uint)time(NULL); //s
    bool output = false; //o
    
    //get options
    int option;
    while ((option = getopt (argc, argv, "q:r:i:f:p:t:vg:s:o")) != -1) {
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
            case 't':
                target = parseopt<size_type>();
                break;
            case 'v':
                verbose = true;
                break;
            case 'g':
                set_grainsize(parseopt<size_type>());
                break;
            case 's':
                seed = parseopt<uint>();
                break;
            case 'o':
                output = true;
                break;
        }
    }
    
    /*
     * Initialize random state
     * use a seed for repeatable results.
     */
    srand(seed);
    
    implementation(imp);
    
    size_type size = 1 << num_qubits;
    
    quregister a (1 << num_qubits),
    b;
    
    for (iterator i (a.begin()); i < a.end(); ++i) {
        *i = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
    }
    /*
     * initilaize the performance counters
     */
    performance::init();
    
    if (verbose) {
        std::cout
        << "Running sigma-z on "
        << num_qubits << " qubits, target qubit "
        << target << std::endl
        << "State vector contains "
        << size << " amplitudes, "
        << ((double)(size* sizeof(complex)) / (1024*1024))
        << "MiB" << std::endl;
    }
    
    /*
     * Either measure the speedup (if output file is give)
     * Of just execute th operator (for external measurements with PERF
     * or correctness testing.
     */
    if (measure) {
        if (imp != "seq" && imp != "omp")
            measure_parallel (file, num_repeat, verbose)
            sigma_z(target, a, b);
        
        else
            measure_sequential (file, num_repeat, verbose)
            sigma_z(target, a, b);
    }
    
    else {
        if (output) print(a);
        for (int i = 1;num_repeat > 0; --num_repeat) {
            if (verbose) std::cout << "iteration " << i++ << std::endl;
            sigma_z(target, a, b);
        }
        if (output) {if (imp == "tbb_blk") print(a); else print(b);}
    }
    return 0;
    
}
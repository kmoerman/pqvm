#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../performance.h"
#include "../quantum.h"


using namespace quantum;

int main (int argc, char** argv) {

    srand(time(NULL));
    
    quregister a (4096*2);
    quregister b (4096*2);
    
    for (iterator i (a.begin()), j (b.begin()); i < a.end(); ++i, ++j) {
        *i = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
        *j = complex ((rand() % 100) / 100.0, (rand() % 100) / 100.0);
    }
    
    measure_parallel ("kronecker.data", 10) {
        quregister c;
        kronecker(a, b, c);
    }
    
    return 0;
    
}
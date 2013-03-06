#include <tbb/tbb.h>
#include "measure.h"

double x, y, z;
void test_pll (tbb::blocked_range<unsigned int>& r) {
    for (unsigned int i (r.begin()); i < r.end(); ++i)
        z = x * y;
}

int main (int argc, char** args) {
    x = 1.234, y = 5.678;
    measure_parallel ("test.data", 10) {
        parallel_for (tbb::blocked_range<unsigned int>(0,10), &test_pll);
    }
    
    return 0;
}
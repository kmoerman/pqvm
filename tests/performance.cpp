#include <tbb/tbb.h>
#include "../performance.h"

double x, y, z;
#define TEST_RANGE 5000000

void test_pll (const tbb::blocked_range<unsigned int>& r) {
    for (unsigned int i (r.begin()); i < r.end(); ++i)
        z = x + y * i;
}

int main (int argc, char** args) {
    x = 0.1234, y = 5.6789;
    measure_parallel ("performace-pll.data", 10, true) {
        parallel_for (tbb::blocked_range<unsigned int> (0, TEST_RANGE, 1024), &test_pll);
    }
    
    measure_sequential ("performace-seq.data", 10, true) {
        for (unsigned int i = 0; i < TEST_RANGE; ++i)
            z = x + y * i;
    }
    
    return 0;
}
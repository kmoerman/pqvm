#include <tbb/tbb.h>
#include <cstddef>

using namespace tbb;
using namespace std;

void Foo(int& d) {
    d *= 10;
}

class ApplyFoo {
    int* const a;
public:
    void operator() (const blocked_range<size_t>& r) const {
        for (size_t i = r.begin(); i != r.end(); ++i)
            Foo(a[i]);
    }
    ApplyFoo (int _a[]): a(_a) {}
};


int main (int argc, char* argv[]) {
    #define TEST_SIZE 20    
    int a[TEST_SIZE] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    parallel_for(blocked_range<size_t>(0, TEST_SIZE), ApplyFoo(a));
    
    
}
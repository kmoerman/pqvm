#include <iostream>
#include "vector.h"
#include <vector>
#include <complex>

vector<double> a (50);
double b [50];
class empty {
    char t;
    void in() {
    }
    
    void out () {
    }
};

int main (int argc, char** args) {
    
    typedef std::complex<double> complex;
    
    std::cout << sizeof(vector<complex>) << std::endl;
    std::cout << sizeof(std::vector<complex>) << std::endl;
    std::cout << sizeof(complex) << std::endl;
    std::cout << sizeof(const std::allocator<void>) << std::endl;
    
    std::cout << sizeof(empty) << std::endl;
    
}
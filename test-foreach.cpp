
#include <iostream>
#include "foreach.h"

int main (int argc, char** argv) {
    
    foreach (95, i)
        std::cout << (char) (i+32) << " ";
    
    return 0;
    
}
#include <iostream>
#include <string>

#include "../options.h"

/* test the options header */

int main (int argc, char** argv) {
    
    std::string confession = "Mama, I killed a man.";
    bool truth = false;
    int fee = 0;
    
    int option;
    while ((option = getopt(argc, argv, "tc:f:")) != -1) {
        switch (option) {
        case 't':
            truth = true;
            break;
        case 'c':
            confession = parseopt<std::string>();
            break;
        case 'f':
            fee = parseopt<int>();
            break;
        }
    }
    
    std::cout << confession << std::endl;
    std::cout << (truth ? "true" : "false") << std::endl;
    std::cout << fee << std::endl;
    
    return 0;
    
}
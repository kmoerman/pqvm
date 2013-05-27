#ifndef pqvm_options_h
#define pqvm_options_h

#include <getopt.h>
#include <string>
#include <sstream>

template <typename T>
T parseopt () {
    T t;
    std::stringstream parser (optarg);
    parser >> t;
    return t;
}

template <>
inline std::string parseopt () {
    return std::string(optarg);
}

template <>
inline char* parseopt () {
    return optarg;
}

#endif
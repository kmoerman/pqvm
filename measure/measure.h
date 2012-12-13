#ifndef MEASURE_H
#define MEASURE_H

#include <cstddef>
#include <string>

void measure (std::string& name, size_t iterations, void (*S)(), void (*P)());

#endif
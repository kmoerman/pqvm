#ifndef MEASURE_H
#define MEASURE_H

#include <ostream>
#include "tbb/tick_count.h"

// Timing functions are based on TBB to always obtain wall-clock time
typedef tbb::tick_count tbb_time_t;

tbb_time_t gettime();

double time_diff(tbb_time_t start, tbb_time_t end);

void measure(std::ostream& result, int iterations, void (*F)());

#endif
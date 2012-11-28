#include <ostream>
#include <string>
#include "tbb/task_scheduler_init.h"
#include "tbb/extra/utility.h" //to be found in tbb/examples/common/utility
#include "measure.h"

// Timing functions are based on TBB to always obtain wall-clock time
tbb_time_t time() {
    return tbb::tick_count::now();
}

double diff(tbb_time_t start, tbb_time_t end) {
    return (end - start).seconds();
}

utility::thread_number_range threads(tbb::task_scheduler_init::default_num_threads);

std::string separator ("\t");

// Measure the time [seconds] of n consecutive executions
// of a function F.
void measure(std::ostream& result, int iterations, void (*F)()) {
    
    result << "# Results for " << iterations << " iterations on " << threads.first << " - " << threads.last << " threads." << std::endl;
    result << "# threads" << separator << "time per iteration (s)" << std::endl;
    
    for (int thread_n = threads.first; thread_n <= threads.last; ++thread_n) {
        tbb::task_scheduler_init init(thread_n);
        
        result << thread_n;
        for (int i = 0; i < iterations; ++i) {
            tbb_time_t start = time();
            F();
            tbb_time_t stop = time();
            result << separator << diff(start, stop);
        }
        result << std::endl;
    }
}
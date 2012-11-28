#include <ostream>
#include <string>
#include "tbb/task_scheduler_init.h"
#include "utility.h"
#include "measure.h"

tbb_time_t time() {
    return tbb::tick_count::now();
}

double diff(tbb_time_t start, tbb_time_t end) {
    return (end - start).seconds();
}

utility::thread_number_range threads(tbb::task_scheduler_init::default_num_threads);

std::string tab ("\t");

//Measure the time [seconds] of n consecutive executions
//of a function F.
void measure(std::ostream& result, int iterations, void (*F)()) {
    
    result << "# Results for " << iterations << " iterations on " << threads.first << " - " << threads.last << " threads." << std::endl;
    result << "# threads" << tab << "time [s]" << std::endl;
    
    for (int thread_n = threads.first; thread_n <= threads.last; thread_n = threads.step(thread_n)) {
        tbb::task_scheduler_init init(thread_n);
        
        tbb_time_t start = time();
        for (int i = 0; i < iterations; ++i) F();
        tbb_time_t stop = time();
        
        result << thread_n << tab << diff(start, stop) << std::endl;
    }
}
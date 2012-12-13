#include <cstddef>
#include <string>
#include <fstream>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#include <tbb/extra/utility.h> //to be found in tbb/examples/common/utility

// Timing functions are based on TBB to always obtain wall-clock time
typedef tbb::tick_count tbb_time_t;

tbb_time_t time() {
    return tbb::tick_count::now();
}

double diff(tbb_time_t start, tbb_time_t end) {
    return (end - start).seconds();
}

utility::thread_number_range threads (tbb::task_scheduler_init::default_num_threads);

const std::string separator ("\t");

// Measure the time [seconds] of n consecutive executions of a algorithm P,
// executed in parallel, compared to its equivalent sequential algorithm S.
void measure (std::string& name, size_t iterations, void (*S)(), void (*P)()) {
    
    //Log the results
    std::ostream& log = std::cout;
    
    //Open the data files
    std::ofstream time_data ((name + ".times.data").c_str());
    std::ofstream speedup_data ((name + ".speedup.data").c_str());
    speedup_data << "# Average speedup for " << iterations << " iterations on " << threads.first << " - " << threads.last << " threads." << std::endl;
    time_data << "# Execution time for " << iterations << " iterations on " << threads.first << " - " << threads.last << " threads." << std::endl;
    
    //Measure sequential average time as speedup reference
    log << "# SEQUENTIAL" << std::endl << "Iterations: " << iterations << std::endl;
    double s_time = 0;
    tbb_time_t start;
    for (int i = 0; i < iterations; ++i) {
        start = time();
        S();
        s_time += diff(start, time());
    }
    speedup_data << "# Average sequential time: " << s_time/iterations << " s." << std::endl;
    log << "Sequential time: " << s_time/iterations << std::endl;
    
    //Measure parallel execution times
    log << std::endl << "# PARALLEL" << std::endl;
    time_data << "# threads" << separator << separator << "execution time [s]" << std::endl;
    speedup_data << "# threads" << separator << separator << "average speedup" << separator << "average execution time [s]"<<std::endl;
    double p_time = 0;
    double t_time = 0;
    for (int thread_n = threads.first; thread_n <= threads.last; ++thread_n) {
        log << "Threads: " << thread_n << std::endl << "Iterations: " << iterations << std::endl;
        tbb::task_scheduler_init init(thread_n);

        for (int i = 0; i < iterations; ++i) {
            tbb_time_t start = time();
            P();
            p_time = diff(start, time());
            t_time += p_time;
            time_data << thread_n << separator << p_time << separator << std::endl;
        }
        speedup_data << thread_n << separator << s_time/t_time << separator << t_time/iterations << std::endl;
        log << "Speedup: " << s_time/t_time << std::endl;
    }
    
    speedup_data.close();
    time_data.close();
}
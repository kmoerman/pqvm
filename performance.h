#ifndef pqvm_performance_h
#define pqvm_performance_h

#include <iostream>
#include <fstream>

#include <string>

#include "quantum/quantum.h"
#include "thread-control.h"

#include <stdlib.h>
#include <stdio.h>
#include <papi.h>

namespace performance {
    
    using namespace thread_control;
    
    //PAPI
    const int num_papi_events = 2;
    int num_hwd_counters;
    int papi_events[num_papi_events] = {
        PAPI_L1_TCM,
        PAPI_L2_TCM
    };
    
    void output_papi_headers (std::ostream& o, std::string sep) {
        o   << "PAPI_L1_TCM" << sep
            << "PAPI_L2_TCM";
    }
    
    void output_papi_counters (std::ostream& o, long_long values[]) {
        for (int i = 0; i < num_papi_events; ++i)
            o << values[i] << "\t";
    }
    
    void handle_papi_error (int retval) {
        std::cout << "PAPI error " << retval << PAPI_strerror(retval);
        exit(1);
    }
    
    void init () {
        if ((num_hwd_counters = PAPI_num_counters()) <= PAPI_OK)
            handle_papi_error(1);
        
        if (num_hwd_counters > num_papi_events)
            num_hwd_counters = num_papi_events;
    }
    
    //PAPI WALL-CLOCK TIMER
    typedef long_long time;
    typedef double interval;
    
    inline time now () {
        return PAPI_get_real_usec();
    }
    
    inline interval diff (time start, time end) {
        return ((double) (end - start)) / 1000000;
    }
    
    namespace details {
        
        /**
         * Some machinery to perform sequential and parallel measurements in wall-
         * clock time, with a custom control structures
         * With a simple struct, we can maintain all of the state and use a
         * basic for loop. Syntactic sugar is provided in the macros below.
         * See <http://www.chiark.greenend.org.uk/~sgtatham/mp/> for interesting
         * approaches to creating custom control structures.
         */
        
        struct parallel {
            time begin;
            int iteration;
            int iterations;
            std::fstream output;
            int threads;
            bool verbose;
            
            //calculate and print the interval
            //increment the iteration counter
            inline void after () {
                double interval = diff (begin, now());
                long_long values[num_papi_events];
                if (PAPI_stop_counters(values, num_papi_events) != PAPI_OK)
                    handle_papi_error(1);
                output << threads << "\t" << interval << "\t";
                output_papi_counters(output, values);
                output << std::endl;
                if (verbose)
                    std::cout << interval << "s"<< std::endl;
                
                ++iteration;
            }
            
            //increment the thread counter when necessary
            //halt when we run out of threads
            //store the starting time for the next iteration
            inline bool before () {
                if (iteration == iterations) {
                    ++threads;
                    if (threads > max_threads()) {
                        output.close();
                        return false;
                    }
                    iteration = 0;
                }
                set_threads (threads);
                if (verbose)
                    std::cout << "threads: " << threads << std::endl;
                PAPI_start_counters(papi_events, num_hwd_counters);
                begin = now();
                return true;
            }
            
            //initialize the output file and set the initial number of threads
            parallel (std::string filename, int iters, bool v = true):
            iteration (0), iterations(iters), output(filename.c_str(), std::ios_base::out),  threads(1), verbose(v) {
                output << "#PAPI events for " << iterations
                       << " iterations on 1" << " to " << max_threads() << " threads." << std::endl
                       << "#PROCESSORS\tTIME\t";
                output_papi_headers(output, "\t");
                output << std::endl;
            }
            
            ~parallel () {
                output.close();
            }
        };
        
        struct sequential {
            time begin;
            int iteration;
            int iterations;
            std::fstream output;
            bool verbose;
            
            //calculate and print the interval and
            //increment the iteration counter
            inline void after () {
                double interval = diff (begin, now());
                long_long values[num_papi_events];
                if (PAPI_stop_counters(values, num_papi_events) != PAPI_OK)
                    handle_papi_error(1);
                output << interval << "\t";
                output_papi_counters(output, values);
                output << std::endl;
                if (verbose)
                    std::cout << interval << "s"<< std::endl;
                ++iteration;
            }
            
            //decide wether to continue or not
            //store the starting time for the subsequent measurement
            inline bool before () {
                if (iteration == iterations) {
                    output.close();
                    return false;
                }
                PAPI_start_counters(papi_events, num_hwd_counters);
                begin = now();
                return true;
            }
            
            //initialize the output file
            sequential (std::string filename, int iters, bool v = true):
            begin (now()), iteration (0), iterations (iters), output (filename.c_str(), std::ios_base::out), verbose(v) {
                output  << "#PAPI events for " << iterations
                        << " iterations on 1 thread." << std::endl
                        << "#TIME\t";
                output_papi_headers(output, "\t");
                output << std::endl;
            }   
            
            ~sequential () {
                output.close();
            }
        };
        
        struct decrease_grainsize {
            size_t grainsize;
            time begin;
            std::fstream output;
            
            inline void after () {
                double interval = diff (begin, now());
                output << grainsize << "\t" << interval << std::endl;
                grainsize -= 1000;
                quantum::set_grainsize(grainsize);
            }
            
            inline bool before () {
                if (grainsize < 1)
                    return false;
                quantum::set_grainsize(grainsize);
                begin = now();
                return true;
                
            }
            
            decrease_grainsize (std::string f, size_t g): grainsize (g), output(f.c_str(), std::ios_base::out) {
                set_threads(1);
                output  << "#TIME for 1 iteration on 1 thread." << std::endl
                << "#GRAINSIZE\tTIME" << std::endl;
            }
            
            ~decrease_grainsize () {
                output.close();
            }
        };
        
        #define PERF_CONCAT_(x, y) x ## y
        #define PERF_CONCAT(x, y) PERF_CONCAT_(x, y)
        #define PERF_EXPERIMENT PERF_CONCAT(experiment_, __LINE__)
                
        #define measure_parallel(...) \
            for (performance::details::parallel PERF_EXPERIMENT (__VA_ARGS__); \
                 PERF_EXPERIMENT.before(); PERF_EXPERIMENT.after())
                
        #define measure_sequential(...) \
            for (performance::details::sequential PERF_EXPERIMENT (__VA_ARGS__); \
                 PERF_EXPERIMENT.before(); PERF_EXPERIMENT.after())
        
        #define decrease_grainsize(...) \
            for (performance::details::decrease_grainsize PERF_EXPERIMENT (__VA_ARGS__); \
                PERF_EXPERIMENT.before(); PERF_EXPERIMENT.after())
        
    }
}

#endif

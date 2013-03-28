#ifndef MEASURE_H
#define MEASURE_H

#include <iostream>
#include <fstream>
#include <string>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>

/*
 * The measure namespace exposes the tbb::tick_count functionality,
 * and allows us to control the number of threads during measurements.
 */
namespace performance {
    
    typedef tbb::tick_count time;
    typedef double interval;
    
    inline time now () {
        return time::now();
    }
    
    inline interval diff (time start, time end) {
        return (end - start).seconds();
    }
    
    inline int max_threads () {
        static const int n = tbb::task_scheduler_init::default_num_threads();
        return n;
    }
    
    inline void set_threads (int n) {
        static tbb::task_scheduler_init init;
        init.terminate();
        init.initialize(n);
    }
    
    namespace details {
        
        /**
         * Some machinery to perform sequential and parallel measurements in wall-
         * clock time.
         * A custom control structure to perform measurements can be made using the
         * technique explained at <http://www.chiark.greenend.org.uk/~sgtatham/mp/>
         * However, with a simple struct, we can maintain all of the state and use a
         * basic for loop instead. Syntactic sugar is provided in the macros below.
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
                double interval = diff(begin, now());
                output << threads << "\t" << interval << std::endl;
                if (verbose)
                    std::cout << threads << " threads, iteration: " << iteration + 1 << ": " << interval << "s"<< std::endl;
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
                    set_threads (threads);
                }
                begin = now();
                return true;
            }
            
            //initialize the output file and set the initial number of threads
            parallel (std::string filename, int iters, bool v = true):
            iteration (0), iterations(iters), output(filename.c_str(), std::ios_base::out),  threads(1), verbose(v) {
                output << "# Wall-clock time (seconds) for " << iterations
                       << " iterations on 1" << " to " << max_threads() << " threads."
                       << std::endl;
                set_threads (threads);
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
                double interval = diff(begin, now());
                output << interval << std::endl;
                if (verbose)
                    std::cout << "iteration " << iteration + 1 << ": " << interval << "s"<< std::endl;
                ++iteration;
            }
            
            //decide wether to continue or not
            //store the starting time for the subsequent measurement
            inline bool before () {
                if (iteration == iterations) {
                    output.close();
                    return false;
                }
                begin = now();
                return true;
            }
            
            //initialize the output file
            sequential (std::string filename, int iters, bool v = true):
            begin (now()), iteration (0), iterations (iters), output (filename.c_str(), std::ios_base::out), verbose(v) {
                output  << "# Wall-clock time (seconds) for " << iterations
                        << " iterations on 1 thread." << std::endl;
            }   
            
            ~sequential () {
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
        
    }
}

#endif

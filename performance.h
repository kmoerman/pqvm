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
        tbb::task_scheduler_init init(n);
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
            std::fstream output;
            time begin;
            int threads;
            int iterations;
            int iteration;
            double interval;
            bool verbose;
            
            //calculate and print the interval
            //increment the iteration counter
            inline void after () {
                interval = diff(begin, now());
                output << threads << "\t" << interval << std::endl;
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
                    tbb::task_scheduler_init init(threads);
                }
                if (verbose)
                    std::cout << threads << " threads, iteration: " << iteration + 1 << ": " << interval << "s"<< std::endl;
                begin = now();
                return true;
            }
            
            //initialize the output file and set the initial number of threads
            parallel (std::string filename, int iters, bool v_ = true):
            iterations(iters), iteration (0), threads(1), verbose(v_),
            output(filename.c_str(), std::ios_base::out) {
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
            std::fstream output;
            time begin;
            int iterations;
            int iteration;
            double interval;
            bool verbose;
            
            //calculate and print the interval and
            //increment the iteration counter
            inline void after () {
                interval = diff(begin, now());
                output << interval << std::endl;
                ++iteration;
            }
            
            //decide wether to continue or not
            //store the starting time for the subsequent measurement
            inline bool before () {
                if (iteration < iterations) {
                    output.close();
                    return false;
                }
                if (verbose)
                    std::cout << "iteration " << iteration + 1 << ": " << interval << "s"<< std::endl;
                begin = now();
                return true;
            }
            
            //initialize the output file
            sequential (std::string filename, int iters, bool v_ = true):
            iterations (iters), iteration (0), begin (now()),
            output (filename.c_str(), std::ios_base::out), verbose(v_) {
                output  << "# Wall-clock time (seconds) for " << iterations
                        << " iterations on 1 thread." << std::endl;
            }   
            
            ~sequential () {
                output.close();
            }
        };
        
        #define MS_CONCAT_(x, y) x ## y
        #define MS_CONCAT(x, y) MS_CONCAT_(x, y)
        #define MS_EXPERIMENT MS_CONCAT(experiment_, __LINE__)
                
        #define measure_parallel(...) \
            for (performance::details::parallel MS_EXPERIMENT (__VA_ARGS__); \
                 MS_EXPERIMENT.before(); MS_EXPERIMENT.after())
                
        #define measure_sequential(...) \
            for (performance::details::sequential MS_EXPERIMENT (__VA_ARGS__); \
                 MS_EXPERIMENT.before(); MS_EXPERIMENT.after())
        
    }
}

#endif

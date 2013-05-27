#ifndef THREAD_CONTROL_H
#define THREAD_CONTROL_H

//TBB THREADS

#include <tbb/task_scheduler_init.h>

namespace thread_control {

    inline int max_threads () {
        static const int n = tbb::task_scheduler_init::default_num_threads();
        return n;
    }

    inline void set_threads (int n) {
        static tbb::task_scheduler_init init;
        init.terminate();
        if (n == 0)
            init.initialize();
        else
            init.initialize(n);
    }
    
}

#endif
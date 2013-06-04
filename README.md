This repository contains the code for my [bachelor’s paper project](https://caliweb.cumulus.vub.ac.be/caliweb/?page=course-offer&id=006306&language=en) at the [Computer Science Department](http://dinf.vub.ac.be) of the [Vrije Universiteit Brussel](http://www.vub.ac.be/en).

The main program is `pqvm.cpp`, adapted form the original `qvm.c` to work with a parallel backend. The code has mostly stayed the same, with some minor changes in the `qop_` functions and the tangle objects.

Our main contribution is in the `quantum` folder, containing the parallel quantum operations.

## Header files
+ `bitmask.h`        simple macros to define and manipulate bitfields
+ `qvm.h`            headers for the original QVM
+ `options.h`        parser for getopt.h option arguments
+ `vector.h`         custom STL-style vector class
+ `thread-control.h` explicitly set the number of threads
+ `performnace.h`    wraps time and hardware counters

## Code files
+ `qvm.c`    original sequential machine
+ `pqvm.cpp` adapted version of `qvm`, to work with the parallel backend

## Folders
+ `quantum` The implementation fo the quantum backend
+ `tests`   Testfiles for th quantum backend (correctness and performnance)
+ `mc`      Some MC programs, we used `mc/qft/qft16.mc` as a benchmark program
+ `report`  The final report tex sources
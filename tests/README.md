The folder contains the testfiles for the quantum backends.
Each c++ file compiles to a program testing a single operator, the actual backend (tbb, seq ...) can be set as a command line parameter (option -i).

Run `make` to compile all, or `make <name>` to compile a single program (e.g. `make sigma-x`).
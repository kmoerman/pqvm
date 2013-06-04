This folder contains the quantum backends. Each backend exports  a fixed set of functions, defined by the quantum.h header. A program need only include the quantum.h header, and call the `implementation` function with one of the names below to select a specific backend.

The `types.h` header defines the basic types we use (quregisters, iterators).

## backends
+ `seq` the sequential implementation, in `sequential.h`
+ `tbb` the basis TBB parallel implementation, in `tbb.h`
+ `omp` a parallel version with OMP, in `openmp.h`
+ `tbb_rng` an adaptation of the basis TBB to set grainsize, in `tbb-range.h`
+ `tbb_mcp` a better version with TBB, in `tbb-mcp.h`
+ `tbb_blk` the final version with TBB, in `tbb-blocks.h`
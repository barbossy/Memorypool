# Memory pool 

This project implement the memory pool in C++.

The memory pool should pre-allocate a continuous range of memory over which the memory pool will operate and hold ownership of that memory throughout its life-cycle. The memory pool should be constructed of multiple, user-defined sub-pools with fixed memory chunk sizes. The request to allocate a memory within a memory pool should result in an allocation of the chunk within the sub-pool that has the smallest possible chunk that will fit the requested allocation size.


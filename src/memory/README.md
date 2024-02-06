# Quick Pool Audio Bus (Pre-allocated Shared Memory Pool)

A pre-allocated memory pool.

## Architecture and Life Cycle Overview

Quick Pool allocates a certain amount of memory from the OS on its initiation. When QP is asked for memory by users via `QP::Alloc()`, QP slices a chunk out of its OS-allocated memory and marks it with two metadata structures: `ChunkH` (header of a chunk) and `ChunkF` (footer of a chunk). The metadata structure is located adjacent to the user-required memory chunk. Upon creating new memory chunks, a member variable `mPoolBuoy` is incremented by the size of that chunk (including metadata size) to record the used pool. The start of a memory chunk is always at that moving buoy. The buoy doesn't drop down when one chunk is freed. `mPoolBuoy` is also responsible for verifying the remaining memory of the Pool.

### One Chunk's Header and Footer

`ChunkH` contains the size of this memory chunk, a truth value for marking allocation, and pointers to another two `ChunkH` structures. When one chunk is freed, it can be added to a doubly linked list that records all the freed chunk slots. `ChunkF` contains nothing but the size of this memory chunk.

### Chunk Coalescence

When one allocated chunk is freed, it first checks whether its adjacent left chunk is also freed. The method of checking the left chunk is optimized to O(1) complexity by simply accessing the left chunk's footer and tracing back the memory location of its header via the footer's size and memory arithmetic. If true, it will try to merge with the left chunk by simply setting the left chunk's size in its `ChunkH` to be the sum of the two chunks (including the metadata's size), along with the tail of the newly freed chunk.

After passing the left chunk merging procedure, it will try to merge with the right chunk. When merging with the right chunk, if it has not been merged with its left chunk, it simply rewrites the chunk header footer size and lets the newly freed chunk take the right free chunk's place. If it has been merged with the left chunk, it rewrites the size data in chunk header and footer, removes both chunks (merged left chunk and right chunk) from the list, and appends the new merged list at the start of the list.

### Doubly Linked Free Chunk List

The freed chunks are grouped into a doubly linked list, where every list element is a `ChunkH` that is not allocated. `ChunkH`'s two pointers are used as the list's linker. The list is used for faster search time for a free chunk and to make a garbage collector (not implemented) do its job easier.

### Implemented Optimizations

The Boolean value that stands for allocation is synthesized to only one bit inside the `size_t` value in `ChunkH`, which stands for chunk size, for reducing the metadata footprint. Free chunk list is used for reducing free chunk search time.

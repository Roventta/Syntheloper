# Quick Pool audio Bus (pre-allocated shared memory pool)

A pre-allocated memory pool

## Architecture and Life Cycle Overview

Quick Pool allocate a certain amount of memory from OS on it's initiation. When QP is asked for memory by users via QP::Alloc(),
QP slice a chunk out of its OS allocated memory, and marked it with two meta data structures, ChunkH (header of a chunk) and ChunkF (footer of a chunk). The meta data structure is located adjacent of the user required memory chunk.
On creating new memory chunks, a member variable "mPoolBuoy" is raised by the size of that chunk (include meta data size) for recording the used pool, the start of an memory chunk is always that moving buoy. Buoy doesn't drop down when one chunk is freed.
mPoolBuoy is also responsible for verifying the remaining memory of the Pool. 

### one Chunk's header and footer

ChunkH contains the size of this memory chunk, a truth value for marking allocation and pointers to another two ChunkH, that when one Chunk is freed, it can be added to a doubly linked list that record all the freed chunk slot.
ChunkF contains nothing but the size of this memory chunk.

### Chunk Coalescence

When one allocated Chunk is freed. It will first check whether its adjacent left chunk is also freed.
The method of checking left chunk is optimized to O(1) complexity by simply access the left chunk's footer, and trace back the memory location of its header via footer::size and memory arithmetic.
In the case of true, it will try to merge with the left chunk by simply setting left chunk's size in its ChunkH to be the sum of two chunks (include the meta data's size), along with the tail of newly freed chunk.

After passing the left chunk merging procedure, it will try to merge with the right chunk. 
When merging with the right chunk, in the case of it has not been merged with its left chunk, it simply rewrite the chunk header footer size, and let the newly free chunk to take the right free chunk's place in t
In the case of it has been merged with the left chunk, it rewrites the size data in chunk header and footer, and remove both chunks(merged left chunk and right chunk) from the list, and append the new merged list at the start of the list.

### Doubly linked Free Chunk list

The Freed chunks are grouped into a doubly linked list, where every list element is a ChunkH that is not allocated. ChunkH's two pointer is used as List's linker.
The list is used for faster search time for a free chunk, and making a garbage collector (not implemented) do their job easier.

### Implemented Optimizations

The Bool value that stands for allocation is synthesized to only one bit inside the size_t value in ChunkH which stands for chunk size, for reducing the meta data foot print.
Free chunk list used for reducing free chunk search time.

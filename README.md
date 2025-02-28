# Allocator (Buddy System)
## Overview
Allocator is a header-only library implementing a basic Buddy Allocation scheme. It manages a contiguous memory pool by splitting (for allocation) or merging (for deallocation) blocks in logarithmic time.
## Dependencies
No dependency
## Features
### Buddy System-Based Memory Management
+ **Power-of-two block splitting & merging**: Allocates the smallest power-of-two block that fits the requested size and automatically merges buddy blocks upon deallocation to minimize fragmentation.
+ **Efficient Free List Management**: Maintains a per-level free list for quick allocation and deallocation, ensuring O(log N) time complexity for memory operations.
### Memory Visualization Support
+ The dump() function provides a visual representation of memory usage:
	+ Allocated blocks → 'A'
	+ Free blocks → 'E'
	+ Displays memory structure in a tree format for easy analysis.
### Robust Pointer Validation
+ Prevents invalid pointer deallocation (nullptr checks, offset verification).
+ Double free protection: Ensures that already freed blocks are not freed again.

### Template-Based Customization
+ leaf_size, height are template parameters, allowing flexible memory configurations.
+ Generic design makes it adaptable to various environments.

## Usage
```C++
#include <iostream>
#include "Allocator.h"

int main(void) {
    char pool[120]; // 30 * 2^2 = 120
    Allocator<30, 2> a(pool);

    a.dump();
    void *ptr1 = a.allocate(30);
    a.dump();
    void *ptr2 = a.allocate(30);
    a.dump();
    a.free(ptr1);
    a.dump();
    a.free(ptr2);
    a.dump();
    void *ptr3 = a.allocate(64);
    a.dump();

    std::cout << ptr1 << std::endl;
    std::cout << ptr2 << std::endl;
    std::cout << ptr3 << std::endl;
}
```

## Notes
+ **No Thread Safety**: **Allocator** is not thread-safe. If you need concurrency, you must provide your own synchronization.
+ **Internal Fragmentation**: The buddy system can still suffer from internal fragmentation, especially when the allocated block is significantly larger than the requested size due to the power-of-two splitting rule.

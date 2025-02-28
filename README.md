# Allocator (Buddy System)
## Overview
Allocator is a header-only library that implements a basic Buddy Allocation scheme. It manages a contiguous memory pool by splitting blocks for allocation and merging them for deallocation, both in logarithmic time.
## Dependencies
No dependency
## Features

### Buddy System-Based Memory Management

+ **Power-of-two block splitting & merging**: Allocates the smallest block of size leaf_size * 2^n that can accommodate the requested size and automatically merges buddy blocks upon deallocation to minimize fragmentation.
+ **Efficient free list management**: Maintains a per-level free list for quick allocation and deallocation, ensuring O(log N) time complexity for memory operations.

### Template-Based Customization

+ **Configurable block size and hierarchy**: Uses leaf_size and height as template parameters, allowing flexible memory configurations based on application needs.
+ **Generic and adaptable design**: Supports various environments by providing a template-based approach without being tied to a specific memory model.

## Usage Example

The following example demonstrates how to use **Allocator** for memory allocation and deallocation. It also shows how to visualize the memory layout using the dump() function.

```C++
#include <iostream>
#include "Allocator.h"

int main(void) {
    // Create a memory pool of size 120 bytes (leaf_size = 30, height = 2)
    char pool[120]; // 30 * 2^2 = 120
    Allocator<30, 2> a(pool);

    std::cout << "Initial memory state:" << std::endl;
    a.dump(); // Print initial memory layout

    // Allocate a 30-byte block
    std::cout << "\nAllocating 30 bytes..." << std::endl;
    void *ptr1 = a.allocate(30);
    a.dump();

    // Allocate another 30-byte block
    std::cout << "\nAllocating another 30 bytes..." << std::endl;
    void *ptr2 = a.allocate(30);
    a.dump();

    // Free the first allocated block
    std::cout << "\nFreeing the first 30-byte block..." << std::endl;
    a.free(ptr1);
    a.dump();

    // Free the second allocated block
    std::cout << "\nFreeing the second 30-byte block..." << std::endl;
    a.free(ptr2);
    a.dump();

    // Allocate a 64-byte block (requires merging)
    std::cout << "\nAllocating 64 bytes..." << std::endl;
    void *ptr3 = a.allocate(64);
    a.dump();

    // Print allocated pointer addresses
    std::cout << "\nAllocated addresses:" << std::endl;
    std::cout << "ptr1: " << ptr1 << std::endl;
    std::cout << "ptr2: " << ptr2 << std::endl;
    std::cout << "ptr3: " << ptr3 << std::endl;

    return 0;
}
```

## Notes
+ **No Thread Safety**: **Allocator** is not thread-safe. If you require concurrency, you must implement your own synchronization mechanisms.
+ **Internal Fragmentation**: The buddy system may still suffer from internal fragmentation, particularly when the allocated block is significantly larger than the requested size due to the power-of-two splitting rule.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
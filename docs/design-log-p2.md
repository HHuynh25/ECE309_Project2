# Design Log — Project 2

(500–800 words total. See spec §5 for what each section must cover.)

## Growth factor and amortized cost
For the Conversation dynamic array implementation, I chose a growth factor of 2. This means that every time the underlying array reaches its current capacity, a new array exactly double the size is allocated on the heap, the existing Message objects are sequentially copied over to the new memory space, and the old array is cleanly deallocated.
To prove that the amortized insertion cost is O(1), we can evaluate the absolute total cost of inserting N elements into the data structure. Each of the N elements requires exactly 1 basic operation to insert into an available slot. However, expensive reallocations occur when the array size reaches powers of two. The computational cost to copy elements during these specific reallocation events forms a geometric series: 1 + 2 + 4 + ... + (N/2). The mathematical sum of this geometric series is always strictly less than N. Therefore, the total combined cost for N insertions is the baseline insertion cost (N) plus the occasional reallocation copying cost (< N), yielding an upper bound of 2N operations. When we divide this aggregate total cost (2N) by the number of inserted elements (N), the average mathematical cost per individual insertion simplifies to a constant O(1). This guarantee ensures the application's chat history can scale gracefully without UI lag or performance degradation.


## Rule of Five evidence
Because the Conversation class manages a raw heap-allocated array of Message objects, relying on the compiler's default shallow copies would lead to catastrophic double-free errors. Implementing the complete Rule of Five was mandatory to maintain memory safety.

Destructor: Iterates through the lifecycle and safely calls delete[] on the Message buffer when a Conversation goes out of scope, guaranteeing no memory is leaked.

Copy Constructor: Allocates a completely independent memory buffer of the exact same capacity and performs a deep copy of each Message. The unit tests verify this by asserting that the memory addresses of the elements in the copy do not match the original.

Copy Assignment Operator: Critically checks for self-assignment first (if (this == &other)). Without this check, assigning an object to itself would delete the very buffer it was attempting to copy. After passing the check, it deallocates the current buffer, allocates a new one, and deep-copies the source elements.

Move Constructor: Steals the underlying pointer from the temporary source object and forcefully sets the source's pointer to nullptr and its size to 0. This bypasses expensive data copying entirely when returning conversations by value from functions.

Move Assignment Operator: Checks for self-assignment, deletes the current buffer to prevent leaks, and then transfers ownership of the source's pointer and state, fully nullifying the source object to leave it in a valid but empty state.


## Sentinel scanner: bounded pending_ proof
The SentinelScanner is designed to process virtually infinite text streams without ever running out of system memory. To guarantee this bounded memory footprint, the internal pending_ buffer only ever stores the exact characters that form a potential matching prefix of the target sentinel string. 
If our targeted sentinel string has a total length of S, the absolute maximum number of characters that can structurally match a prefix without triggering a complete sentinel match is exactly S - 1. Whenever a new text chunk is fed into the scanner, it is temporarily appended to pending_. The algorithm then scans pending_ for the complete sentinel. If no full match is found, the system calculates the longest suffix of pending_ that matches a prefix of the sentinel. Any characters residing before this overlap suffix are mathematically guaranteed to be safe and are flushed directly to safe_text. The pending_ buffer is then rigorously truncated to only hold that potential prefix. Because this truncated suffix can never exceed S - 1 characters, the memory footprint of pending_ is strictly bounded to an upper limit of O(S), ensuring it remains consistently flat whether the incoming LLM stream is ten bytes or ten gigabytes.


## What I would change differently
If I were to approach this project a second time, I would implement my unit tests incrementally alongside the core logic rather than waiting until the end. Trying to hunt down elusive segmentation faults in the Conversation move assignment operator would have been exponentially faster if I had a test suite verifying each Rule of Five component in complete isolation from day one. Additionally, configuring my VS Code and WSL Ubuntu environment to work seamlessly with CMake took quite a bit of trial and error early in the development cycle. Next time, I would invest much more time up front ensuring my Linux compilation toolchain and debugger were fully configured before drafting the C++ headers, which would have made stepping through the scanner's buffering edge cases much more intuitive.

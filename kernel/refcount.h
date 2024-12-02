#ifndef XV6_KERNEL_REFCOUNT_H
#define XV6_KERNEL_REFCOUNT_H

typedef int refcount_t;

// Create a new shared page handle with its counter initialized to 1,
// indicating that this shared page is used by 1 process.
//
// Returns -1 is the maximum amount of shared page handles is exceeded.
refcount_t refcount_new();

// Increment the counter for the given shared page handle 
// (e.g. when it is used by an additional process).
//
// Returns the new value of the counter after incrementing.
int refcount_increment(refcount_t index);

// Decrement the counter for the given shared page handle.
// Should be called when process using the shared page exits.
//
// Returns the new value of the counter after decrementing.
// If the returned value is 0, there are no processes left using this page,
// and you should free the physical memory used for this page.
int refcount_decrement(refcount_t index);

// Used for testing/debugging
void refcountdump(void);

// Initializes the locks of the datastructures, called by main
void refcount_init(void);

#endif

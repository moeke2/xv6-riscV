#ifndef XV6_KERNEL_BITMAP_H
#define XV6_KERNEL_BITMAP_H

#include "types.h"

// Abstract data type for a bitmap
// NOTE: *never* modify the undelying integer directly and *always* use the functions provided below
typedef uint64 bitmap_t;

// Return a newly initialized bitmap with all bits set
bitmap_t init_bitmap(void);

// Return a new bitmap derived from the provided @param(bitmap) with @param(bit) additionally cleared
bitmap_t clear_bit(bitmap_t bitmap, uint64 bit);

// Return 1 (true) if @param(bit) is set in the @param(bitmap), 0 (false) otherwise
uint is_bit_set(bitmap_t bitmap, uint64 bit);

#endif

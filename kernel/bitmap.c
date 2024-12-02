#include "types.h"
#include "bitmap.h"
#include "proc.h"
#include "defs.h"
#include "syscall.h"
#include "bookkeeping.h"

bitmap_t init_bitmap(void) {
  log_init_syscall_filter();
  return ~((bitmap_t) 0x0); // All bits set to 1
}

uint64 get_mask(uint64 bit) {
  return 1ULL << bit;
}

bitmap_t clear_bit(bitmap_t bitmap, uint64 bit) {
  return bitmap & ~get_mask(bit); // Clear the specified bit
}

uint is_bit_set(bitmap_t bitmap, uint64 bit) {
  return (bitmap & get_mask(bit)) != 0; // Check if the specified bit is set
}
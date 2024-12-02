#include "user/user.h"
#include "kernel/memlayout.h"

int mmap() __attribute__((weak));

// Check if mmap syscall is implemented
int main(int argc, char **argv) {
  if (!mmap) {
    printf("Error: mmap system call not implemented yet.\n");
    return -1;
  }
  printf("SUCCESS\n");
  return 0;
}

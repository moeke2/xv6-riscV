#include "user/user.h"
#include "kernel/memlayout.h"

#define MEM 0x0000000baddad000
#define PERM (PTE_R | PTE_W);

int mmap() __attribute__((weak));

// Initialize counter
int main(int argc, char **argv) {
  int perm = PERM;
  if (mmap(MEM, perm, 1) < 0) {
    printf("Error: mmap() failed\n");
    exit(1);
  }

  printf("Counters\n");
  refcountdump();
  printf("SUCCESS\n");
  
  return 0;
}

#include "user/user.h"
#include "kernel/memlayout.h"

#define MEM 0x0000000baddad000
#define MEM2 0x0000000bddddd000
#define PERM (PTE_R | PTE_W);

int mmap() __attribute__((weak));

// Check that `vaddr` is not already mapped
int main(int argc, char **argv) {
  int perm = PERM;
  if (mmap(MEM, perm, 0) != 0) {
    printf("Error: mmap should have succedded\n");
    return -1;
  }

  if (mmap(MEM, perm, 0) == 0) {
    printf("Error: mmap should have failed\n");
    return -1;
  }

  if (mmap(MEM, perm, 1) == 0) {
    printf("Error: mmap should have failed\n");
    return -1;
  }

  if (mmap(MEM2, perm, 1) != 0) {
    printf("Error: mmap should have succedded\n");
    return -1;
  }

  if (mmap(MEM2, perm, 0) == 0) {
    printf("Error: mmap should have failed\n");
    return -1;
  }

  if (mmap(MEM2, perm, 1) == 0) {
    printf("Error: mmap should have failed\n");
    return -1;
  }

  printf("Mappings\n");
  vmprintmappings();
  printf("SUCCESS\n");
  return 0;
}

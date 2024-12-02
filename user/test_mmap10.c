#include "user/user.h"
#include "kernel/memlayout.h"

#define MEM 0x0000000baddad000
#define MEM2 0x0000000bdddad000
#define PERM (PTE_R | PTE_W);

int mmap() __attribute__((weak));

// Increment counter
int main(int argc, char **argv) {
  int perm = PERM;
  if (mmap(MEM, perm, 1) < 0) {
    printf("Error: mmap() failed\n");
    exit(1);
  }

  if (mmap(MEM2, perm, 1) < 0) {
    printf("Error: mmap() failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid == 0) {
    printf("Counters\n");
    refcountdump();
    printf("SUCCESS\n");
  } else {
    wait(0);
  }
  
  return 0;
}

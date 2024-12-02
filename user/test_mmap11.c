#include "user/user.h"
#include "kernel/memlayout.h"

#define MEM 0x0000000baddad000
#define PERM (PTE_R | PTE_W);

int mmap() __attribute__((weak));

// Increment counter
int main(int argc, char **argv) {
  int perm = PERM;
  if (mmap(MEM, perm, 1) < 0) {
    printf("Error: mmap() failed\n");
    exit(1);
  }

  uint64 *mem = (uint64 *)MEM;
  *mem = 42;

  int pid = fork();
  if (pid == 0) {
    printf("In child\n");
    refcountdump();
  } else {
    wait(0);
    printf("In parent\n");
    refcountdump();
    printf("SUCCESS\n");

    if (*mem != 42) {
      printf("Error: expected 42 on shared page after child exit");
      exit(1);
    }
  }
  
  return 0;
}

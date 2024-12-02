#include "user/user.h"
#include "kernel/memlayout.h"

#define MEM 0x0000000baddad000
#define PERM (PTE_R | PTE_W);

int mmap() __attribute__((weak));

// Properly checks that pages are private
int main(int argc, char **argv) {
  int perm = PERM;
  if (mmap(MEM, perm, 0) < 0) {
    printf("Error: mmap() failed\n");
    exit(1);
  }

  int pid = fork();

  if (pid < 0) {
    printf("Error: fork() failed\n");
    exit(1);
  }

  int *base = (int *) MEM;
  *base = 42;

  if (pid == 0) {
    *base = *base + 1;
    if (*base != 43) {
      printf("Error: in child process, 43 expected, got %d\n", *base);
      exit(1);
    }
  } else {
    wait(0);
    if (*base != 42) {
      if (*base == 43) {
        printf("Error: page should not be shared between child and parent\n");
      } else {
        printf("Error: in parent process, 43 expected, got %d\n", *base);
      }
      exit(1);
    }

    printf("SUCCESS\n");
  }
  
  return 0;
}

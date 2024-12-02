#include "user/user.h"
#include "kernel/memlayout.h"

#define MEM 0x0000000baddad000
#define MEM2 0x0000000bddddd000
#define MAXMAPPAGES 10
#define PERM (PTE_R | PTE_W);

int mmap() __attribute__((weak));

// Mappings copied to child
int main(int argc, char **argv) {
  int perm = PERM;
  if (mmap(MEM, perm, 0) != 0) {
    printf("Error: mmap should have succedded\n");
    return -1;
  }
  if (mmap(MEM2, perm, 1) != 0) {
    printf("Error: mmap should have succedded\n");
    return -1;
  }
  int pid = fork();
  if (pid == 0) {
    printf("Mappings\n");
    vmprintmappings();
    printf("SUCCESS\n");
  } else {
    wait(0);
  }
  return 0;
}

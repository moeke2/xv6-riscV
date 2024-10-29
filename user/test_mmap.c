#include "user/user.h"
#include "kernel/memlayout.h"
#include "kernel/param.h"

#define BASE  0x0000000baddad000
#define PERMS (PTE_R | PTE_W)

int main(int argc, char **argv) 
{
  printf("Before mmap()\n");
  vmprintmappings();

  // Call mmap() MAXMMAP times, in PGSIZE steps, starting at address BASE
  // All calls should succeed
  for (int i=0; i < MAXMMAP; i++) {
    uint64 vaddr = BASE + (i * PGSIZE);
    if (mmap(vaddr, PERMS) < 0) {
      fprintf(2, "mmap() failed\n");
      exit(1);
    }
  }

  // 2. mmap an already mapped address
  //   This should fail
  if (mmap(BASE, PERMS) == 0) {
    fprintf(2, "mmap() succeeded\n");
    exit(1);
  }

  printf("After mmap()\n");
  vmprintmappings();

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork() failed\n");
    exit(1);
  }

  // Write the value of 42 at the base address (which is mmapped by now)
  int *base = (int *) BASE;
  *base = 42;

  if (pid == 0) {

    printf("In child\n");
    vmprintmappings();

    // The child should have inherited MAXMMAP mappings from the parent
    // Calling mmap() again should fail.
    uint64 vaddr = BASE + (MAXMMAP * PGSIZE);
    if (mmap(vaddr, PERMS) >= 0) {
      fprintf(2, "mmap() succeeded\n");
      exit(1);
    }

    // The value at the base address should be 42 in the child's address space
    //  as well. Incrementing it with one should not affect the parent process.
    *base = *base + 1;
    if (*base != 43) {
      fprintf(2, "43 expected, got %d\n", *base);
      exit(1);
    }

    exit(0);
  }

  wait(0);
  // The value at the base address should still be 42 in the parent's address
  //  space.
  printf("In parent\n");
  if (*base != 42) {
    fprintf(2, "42 expected, got %d\n", *base);
    exit(1);
  }

  printf("All tests passed!\n");

  return 0;
}

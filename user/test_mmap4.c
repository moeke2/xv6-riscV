#include "user/user.h"
#include "kernel/memlayout.h"

#define MEM 0x0000000baddad000
#define MAXMAPPAGES 10
#define PERMRW (PTE_R | PTE_W)
#define PERMR  (PTE_R)
#define PERMW  (PTE_W)
#define PERMRX  (PTE_X | PTE_R)

int perms[] = {PERMRW, PERMR, PERMW, PERMRX};

int mmap() __attribute__((weak));

// Check that permissions are working
int main(int argc, char **argv) {
  int N = sizeof(perms) / sizeof(*perms);
  for(int i = 0; i < N; ++i) {
    if (mmap(MEM+(i*PGSIZE), perms[i], 0) != 0) {
      printf("Error: mmap(0x%lx, %d) should have succedded %d\n", MEM+(i*PGSIZE),  perms[i], i);
      return -1;
    }
  }
  printf("Mappings\n");
  vmprintmappings();
  printf("SUCCESS\n");
  return 0;
}

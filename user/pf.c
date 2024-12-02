#include "user/user.h"
#include "kernel/riscv.h"

int main(void)
{
  int *p = (int *) MAXVA;

  printf("%d", *p);

  return 0;
} 

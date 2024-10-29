#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
  printf("Hello, World!\n");

  /* Task - Part 1 */
  printf("Task - Part 1\n");
  vmprintmappings();

  /* Task - Part 2 */
  void *p = sbrk(1);
  if (p == 0)
  {
    fprintf(2, "sbrk() failed\n");
    exit(1);
  }
  printf("Task - Part 2\n");
  int poep = 100;
  int test = 20;
  test = poep + test;
  vmprintmappings();

  return 0;
} 

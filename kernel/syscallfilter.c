#include "types.h"
#include "bitmap.h"
#include "proc.h"
#include "defs.h"
#include "syscall.h"
#include "syscallfilter.h"

// Blocks the system call corresponding to the number provided in the first system-call argument.
// Returns 0 on success or -1 if the system call number argument is invalid.
uint64 sys_syscallfilter(void)
{
  int num;

  argint(0, &num);

  struct proc* p = myproc();

  if( num < 1 || num > 28){
    return -1;
  }

  p->bmap = clear_bit(p->bmap,num);
  return 0;

}
#include "trace.h"

#include "defs.h"
#include "proc.h"

uint64
sys_traceme(void)
{
  int traceme;

  argint(0, &traceme);

  myproc()->traceme = traceme;
  return 0;
}

void
trace_syscall()
{
  struct proc* p = myproc();

  if (p->traceme) { //if traceme is not equal to 0
    int syscall = p->trapframe->a7;
    printf("[%d] syscall %d\n", p->pid, syscall);
  }
}

#include "trace.h"
#include "defs.h"
#include "proc.h"
#include "file.h"
#include "param.h"


uint64
sys_traceme(void)
{
  int traceme;

  argint(0, &traceme);

  myproc()->traceme = traceme;
  return 0;
}

uint64
sys_tracemefd(void)
{
  int tracefd;

  argint(0, &tracefd);

  struct proc* p = myproc();

  if (tracefd < 0) {
    p->tracefd = -1;
    return 0;
  }

  if (tracefd >= NOFILE ||
      !p->ofile[tracefd] ||
      p->ofile[tracefd]->type != FD_PIPE ||
      !p->ofile[tracefd]->writable) {
    p->tracefd = -1;
    return -1;
  }

  p->tracefd = tracefd;
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
  if (p->tracefd >= 0) {
    struct tracemsg msg;
    msg.pid = p->pid;
    msg.syscall = p->trapframe->a7;

    struct pipe* pipe = p->ofile[p->tracefd]->pipe;
    pipewrite(pipe, /*user_src=*/0, (uint64)&msg, sizeof(msg));
  }

}
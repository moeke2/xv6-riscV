#ifndef XV6_KERNEL_TRACE_H
#define XV6_KERNEL_TRACE_H
struct tracemsg {
  int pid;
  int syscall;
};
void trace_syscall(void);
#endif
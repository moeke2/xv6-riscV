#include "bookkeeping.h"
#include "proc.h"
#include "defs.h"

bookkeeping_t global_bookkeeping = 0;

void init_bookkeeping(bookkeeping_t *bk) {
  *bk = 0;
}

void log_init_syscall_filter() {
  global_bookkeeping++;
}

uint64 sys_printbookkeeping() {
  //struct proc *p = myproc();
  printf("[bookkeeping] init_syscall_filter = %ld\n", global_bookkeeping);
  return 0;
}

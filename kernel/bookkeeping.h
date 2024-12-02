#ifndef BOOKKEEPING_H_
#define BOOKKEEPING_H_
#include "types.h"

typedef uint64 bookkeeping_t;

void init_bookkeeping(bookkeeping_t *bk);

void log_init_syscall_filter();

uint64 sys_printbookkeeping();

#endif // BOOKKEEPING_H_

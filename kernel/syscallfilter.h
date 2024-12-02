#ifndef XV6_KERNEL_SYSCALLFILTER_H
#define XV6_KERNEL_SYSCALLFILTER_H

#include "types.h"

// Blocks the system call corresponding to the number provided in the first system-call argument.
// Returns 0 on success or -1 if the system call number argument is invalid.
uint64 sys_syscallfilter(void);

#endif

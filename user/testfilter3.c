#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/syscall.h"
#include "user/user.h"

int syscallfilter(int) __attribute__((weak));

int main()
{
    if (!syscallfilter) {
      printf("Error: syscallfilter system call not implemented yet.\n");
      return -1;
    }

    int i = syscallfilter(0);

    printf("[testfilter] syscallfilter(0) = %d\n", i);

    i = syscallfilter(45);
    printf("[testfilter] syscallfilter(45) = %d\n", i);

    i = syscallfilter(SYS_fork);
    printf("[testfilter] syscallfilter(%d) = %d\n", SYS_fork, i);

    return 0;
}

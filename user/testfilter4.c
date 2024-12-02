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

    syscallfilter(SYS_mkdir);

    printbookkeeping();

    return 0;
}

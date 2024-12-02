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

    printf("[testfilter] Allowed\n");

    syscallfilter(SYS_write);

    if(fork() > 0) {
        wait(0);
        printf("[testfilter] Not allowed - parent\n");
    } else {
        printf("[testfilter] Not allowed - child\n");
    } 

    return 0;
}

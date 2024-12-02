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

    printf("[filter] Allowed\n");

    for(int i = 0; i < 20; ++i) {
      if(fork() == 0) {
        syscallfilter(SYS_write);
        printf("[filter] Not allowed - child\n");
        return -1;
      } else {
        wait(0);
      }
    }
    for(int i = 0; i < 20; ++i) {
      if(fork() == 0) {
        printf("[filter] Allowed - child\n");
        return 0;
      } else {
        wait(0);
      }
    }


    return 0;
}

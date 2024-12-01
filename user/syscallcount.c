#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    printf("System calls executed by this process: %d\n", getnumsyscalls());
    
    // Perform some system calls
    getpid();
    getpid();
    
    printf("System calls executed by this process after calling getpid twice: %d\n", getnumsyscalls());
    exit(0);
}
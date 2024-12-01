#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Needs to be invoked with a command line argument (1-3)\n");
        return 1;
    }

    char arg = argv[1][0];
    int *ptr = 0;
    switch (arg) {
        case '1':
            printf("Testing illegal instruction:\n");
            asm(".word 0x0");
            break;

        case '2':
            printf("Testing null pointer dereference (store):\n");
            *ptr = 42;
            printf("Value: %d\n", *ptr);
            break;

        case '3':
            printf("Testing instruction page fault:\n");
            int (*fptr)(void) = (void*)0x31337;
            fptr();
            break;

        default:
            printf("Unknown test case\n");
            break;
    }
}

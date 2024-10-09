#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    char str[] = "poep\n";
    if (write(1, str, strlen(str)) != strlen(str)) {
        fprintf(2, "cat: write error\n");
        exit(1);
    }
}
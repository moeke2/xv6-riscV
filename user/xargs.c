#include "kernel/types.h"
#include "user/user.h"

#define BUF_SIZE    512
#define ARGS_SIZE   50

char buf[BUF_SIZE];
char *args[ARGS_SIZE];

int readline(int fd, char *buf, int size)
{
    char c;
    int i = 0;

    while (i < size && read(fd, &c, 1) > 0)
    {
        if (c == '\n')
        {
            buf[i] = '\0';
            return i + 1;
        }
        else
            buf[i] = c;
        i++;
    }

    buf[i] = '\0';
    return i;
}

int main(int argc, char *argv[])
{
    int n;
    if (argc < 2)
    {
        printf("usage: %s cmd arg1 arg2 ...\n", argv[0]);
        exit(1);
    }
    if (argc >= ARGS_SIZE)
    {
        printf("%s: too many arguments\n", argv[0]);
    }

    /* First copy cmdline args (to be followed by stdin) */
    for (int i = 0; i < argc-1; i++)
        args[i] = argv[i+1];

    /* Now read from stdin line per line */
    while((n = readline(0, buf, sizeof(buf))) > 0)
    {
        /* Fork: see xv6 book section 1.1 */
        int pid = fork();
        if (pid > 0)
        {
            /* parent */
            wait((int *) 0);
        }
        else if(pid == 0)
        {
            /* child */
            args[argc-1] = buf;
            args[argc] = 0;
            exec(argv[1], args);
            printf("%s: exec error for '%s'\n", argv[0], argv[1]);
            exit(1);
        }
        else
        {
            printf("%s: fork error\n", argv[0]);
            exit(1);
        }
    }
    exit(0);
}

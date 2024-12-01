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
            return i+1;
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
    if (argc < 2) {
        printf("wrong usage .......");
        exit(1);
    }
    if (argc >= ARGS_SIZE){
        printf("too many arguments");
    }

    //prepare a new arguments array
    for (int i = 0 ; i < argc -1; i++){
        args[i] = argv[i+1];
    }
    while((n = readline(0,buf,sizeof(buf))) > 0){
        int pid = fork();
        if (pid > 0){
            wait(0);
        }
        else if(pid ==0) {
            args[argc-1] = buf;
            args[argc] = 0;
            exec(argv[1], args);
            printf("exec fucking failed");
            exit(1);
        }
        else{
            printf("fork failed");
            exit(1);
        }
    }
    exit(0);
}

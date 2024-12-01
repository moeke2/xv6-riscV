#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/trace.h"
#include "user/user.h"
static void usage(const char* name)
{
  printf("Usage: %s [-o tracefile] executable [args]...\n", name);
}
int main(int argc, char *argv[])
{
  if (argc < 2) {
    usage(argv[0]);
    return 1;
  }
  int has_tracefile = strcmp(argv[1], "-o") == 0;
  if (has_tracefile && argc < 4) {
    usage(argv[0]);
    return 1;
  }
  int tracefd = 1;
  char** cmd_argv = argv + 1;
  if (has_tracefile) {
    const char* tracefile = argv[2];
    tracefd = open(tracefile, O_WRONLY | O_CREATE | O_TRUNC);
    if (tracefd < 0) {
      printf("Error opening %s\n", tracefile);
      return 1;
    }
    cmd_argv = argv + 3;
  }
  int pipefds[2];
  if (pipe(pipefds) < 0) {
    puts("pipe() error");
    return 1;
  }
  int fork_result = fork();
  if (fork_result < 0) {
    puts("fork() error");
    return 1;
  }
  if (fork_result == 0) {
    if (has_tracefile) {
      close(tracefd);
    }
    close(pipefds[0]);
    tracemefd(pipefds[1]);
    exec(cmd_argv[0], cmd_argv);
    // exec() only returns if there was an error
    tracemefd(-1);
    printf("Error executing %s\n", argv[1]);
    return 1;
  } else {
    close(pipefds[1]);
    struct tracemsg msg;
    while (read(pipefds[0], &msg, sizeof(msg)) != 0) {
      fprintf(tracefd, "[%d] syscall %d\n", msg.pid, msg.syscall);
    }
    return 0;
  }
}
#include "user/user.h"

int main(void)
{
  void *p = sbrk(1);
  if (p == 0)
  {
    fprintf(2, "sbrk() failed\n");
    exit(1);
  }

  /* Revisiting fork and exec */
  int pid = fork();
  if (pid < 0)
  {
    fprintf(2, "fork() failed\n");
    exit(1);
  }

  /* Child */
  if (pid == 0)
  {
    printf("Child\n");
    vmprintmappings();
    exit(0);
  }

  /* Parent */
  wait(0);
  printf("Parent\n");
  vmprintmappings();

  /* Execute vmprint */
  char *args[2];
  args[0] = "vmprint";
  args[1] = 0;
  printf("Exec\n");
  exec(args[0], args);

  fprintf(2, "exec() failed");
  exit(1);
} 

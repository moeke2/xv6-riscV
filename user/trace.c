#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: %s executable [args]...\n", argv[0]);
    return 1;
  }

  traceme(1);
  exec(argv[1], argv + 1);

  // exec() only returns if there was an error
  traceme(0);
  printf("Error executing %s\n", argv[1]);
  return 1;
}

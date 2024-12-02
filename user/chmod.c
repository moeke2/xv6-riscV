#include "user/user.h"
#include "kernel/stat.h"

int main(int argc, char* argv[])
{
  if (argc != 3) {
    fprintf(2, "Usage: %s MODE PATH\n", argv[0]);
    return 1;
  }

  int mode = atoi(argv[1]);

  if ((mode & ~M_ALL) != 0) {
    fprintf(2, "Illegal mode: %s\n", argv[1]);
    return 1;
  }

  const char* path = argv[2];

  if (chmod(path, mode) < 0) {
    fprintf(2, "chmod failed\n");
    return 1;
  }

  return 0;
}

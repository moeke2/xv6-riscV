#include "kernel/types.h"
#include "user/user.h"

int puts(const char* str)
{
  int rv = write(1, str, strlen(str));
  if (rv > 0)
      rv = write(1, "\n", 1);
  return rv;
}

#include "user/user.h"

int main(void)
{
  int t1 = uptime(); 
  int t2 = fastuptime(); 

  printf("t1=%d t2=%d\n", t1, t2);

  return 0;
}

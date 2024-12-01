#include "user/user.h"
char script[] = "hello.sh";
char *args[2] = { 0 };
int main(int argc, char *argv[]) {
  args[0] = script;
  exec(args[0], args);
  printf("Error exec\n");
  exit(1);
}
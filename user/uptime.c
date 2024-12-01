#include "user/user.h"

int main(int argc, char const *argv[])
{
    int t1 = uptime();
    int t2 = fastuptime();

    printf("t1=%d t2=%d\n", t1, t2);
    return 0;
}

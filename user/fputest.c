#include "user.h"

double get_sum() {
  double sum = 0;

  for (double d = 1.23; d < 100; d *= 1.9) {
    sum += d;
  }

  return sum;
}

int main() {
  double sum = get_sum();
  printf("Sum: %d\n", (int)sum);
  // converting a double to an int is not good practice,
  // but our printf does not support floating point numbers
  return 0;
}

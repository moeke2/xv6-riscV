#include "user/user.h"

struct memlayout {
  void* text;
  int* data;
  int* stack;
  int* heap;
};

struct memvalues {
  int data;
  int stack;
  int heap;
};

// who should be either "parent" or "child"
void print_mem(const char* who, struct memlayout* layout, struct memvalues* values) {
  printf("%s:stack:%p:%d\n", who, layout->stack, values->stack);
  printf("%s:heap:%p:%d\n", who, layout->heap, values->heap);
  printf("%s:data:%p:%d\n", who, layout->data, values->data);
  printf("%s:text:%p\n", who, layout->text);
}

int data_var = 0;

int main() {
  // 1. and 2.
  struct memlayout layout;
  int stack_var = 0;
  layout.stack = &stack_var;
  int* heap_var = sbrk(sizeof(int));
  *heap_var = 0;
  layout.heap = heap_var;
  layout.text = (void*)main;
  layout.data = &data_var;

  // 3. pipe
  int pipefd[2];

  if (pipe(pipefd) == -1) {
    printf("pipe() failed\n");
    exit(1);
  }

  // 3. fork
  int fork_result = fork();

  if (fork_result == -1) {
    printf("fork() failed\n");
    exit(1);
  } else if (fork_result == 0) {
    // Child
    // Close the pipe's read-end, we will only write
    close(pipefd[0]);

    // 4.i en 4.ii
    struct memlayout layout;
    stack_var = 1;
    layout.stack = &stack_var;
    int* heap_var = sbrk(sizeof(int));
    *heap_var = 1;
    layout.heap = heap_var;
    layout.text = (void*)main;
    data_var = 1;
    layout.data = &data_var;

    // 4.iii
    struct memvalues values;
    values.stack = stack_var;
    values.heap = *heap_var;
    values.data = data_var;

    // 4.iv
    write(pipefd[1], &layout, sizeof(layout));
    write(pipefd[1], &values, sizeof(values));
    close(pipefd[1]);
  } else {
    // Parent
    // Close the pipe's write-end, we will only read
    close(pipefd[1]);

    // 5.i
    struct memlayout child_layout;
    struct memvalues child_values;
    read(pipefd[0], &child_layout, sizeof(child_layout));
    read(pipefd[0], &child_values, sizeof(child_values));

    // 5.ii
    print_mem("child", &child_layout, &child_values);

    // 5.iii
    struct memvalues values;
    values.stack = stack_var;
    values.heap = *heap_var;
    values.data = data_var;

    // 5.iv
    print_mem("parent", &layout, &values);
  }

  exit(0);
}

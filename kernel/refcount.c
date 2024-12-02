#include "defs.h"
#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "refcount.h"

struct {
  struct spinlock lock;
  uint count[MAXREFCOUNT];
} refcounts;

void refcountdump(void) 
{
  printf("Dumping shared page counters\n");
  acquire(&refcounts.lock);
  for (int i = 0; i < MAXREFCOUNT; i++) {
    printf("%d = %d\n", i, refcounts.count[i]);
  }
  release(&refcounts.lock);
}

void refcount_init()
{
  initlock(&refcounts.lock, "refcounts");
}

refcount_t refcount_new()
{
  acquire(&refcounts.lock);

  uint i;
  int result = -1;
  for(i = 0; i < MAXREFCOUNT; ++i) {
    if (refcounts.count[i] == 0) {
      refcounts.count[i]++;
      result = i;
      break;
    }
  }

  release(&refcounts.lock);

  return result;
}

int refcount_increment(refcount_t index)
{
  acquire(&refcounts.lock);

  int result = ++refcounts.count[index];
  
  release(&refcounts.lock);

  return result;
}

int refcount_decrement(refcount_t index)
{
  acquire(&refcounts.lock);

  int result = 0;
  if (refcounts.count[index] > 0)
    result = --refcounts.count[index];
  
  release(&refcounts.lock);

  return result;
}


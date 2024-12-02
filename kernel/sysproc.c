#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "refcount.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_vmprintmappings(void)
{
  vmprintmappings(myproc()->pagetable);
  return 0;
}

uint64
sys_refcountdump(void)
{
  refcountdump();
  return 0;
}

uint64
sys_mmap(void)
{
  struct proc *p = myproc();

  // Feth the system call arguments from the trap frame
  uint64 vaddr;
  int    perms;
  argaddr(0, &vaddr);
  argint(1, &perms);
  vaddr = PGROUNDDOWN(vaddr);

  // Verify that vaddr is not already mapped
  if (walkaddr(p->pagetable, vaddr) != 0)
    return -1;

  // Sanitize perms
  perms &= (PTE_R | PTE_W | PTE_X);
  if (perms == 0)
    return -1;
  perms |= PTE_U;

  // Get index to free element in mmapped
  int idx = 0;
  while ( (idx < MAXMMAP) && (p->mmapped[idx].vaddr != 0) ) {
    idx++;
  }

  // Verify that the process will not exceed MAXMMAP number of mmapped pages.
  if (idx >= MAXMMAP)
    return -1;

  // Allocate a new phyisical page and map vaddr
  char * paddr = kalloc();
  if (paddr == 0)
    return -1;
  memset(paddr, 0, PGSIZE);
  if (mappages(p->pagetable, vaddr, PGSIZE, (uint64) paddr, perms) != 0)
  {
    kfree(paddr);
    return -1;
  }

  // Record the mapped page
  p->mmapped[idx].vaddr = vaddr;

  return 0;
}

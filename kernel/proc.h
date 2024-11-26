#ifndef XV6_KERNEL_PROC_H
#define XV6_KERNEL_PROC_H

#include "types.h"
#include "riscv.h"
#include "param.h"
#include "spinlock.h"

// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context context;     // swtch() here to enter scheduler().
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// usertrapret() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
// the trapframe includes callee-saved user registers like s0-s11 because the
// return-to-user path via usertrapret() doesn't return through
// the entire kernel call stack.
struct trapframe {
  /*   0 */ uint64 kernel_satp;   // kernel page table
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64 kernel_trap;   // usertrap()
  /*  24 */ uint64 epc;           // saved user program counter
  /*  32 */ uint64 kernel_hartid; // saved kernel tp
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;

  /* 288 */ uint64 f0;    
  /* 296 */ uint64 f1; 
  /* 304 */ uint64 f2;  
  /* 312 */ uint64 f3;  
  /* 320 */ uint64 f4;  
  /* 328 */ uint64 f5;  
  /* 336 */ uint64 f6;   
  /* 344 */ uint64 f7;  
  /* 352 */ uint64 f8;  
  /* 360 */ uint64 f9;   
  /* 368 */ uint64 f10; 
  /* 376 */ uint64 f11; 
  /* 384 */ uint64 f12;   
  /* 392 */ uint64 f13;   
  /* 400 */ uint64 f14;  
  /* 408 */ uint64 f15;  
  /* 416 */ uint64 f16;   
  /* 424 */ uint64 f17;   
  /* 432 */ uint64 f18;  
  /* 440 */ uint64 f19;  
  /* 448 */ uint64 f20; 
  /* 456 */ uint64 f21;  
  /* 464 */ uint64 f22;  
  /* 472 */ uint64 f23;  
  /* 480 */ uint64 f24;  
  /* 488 */ uint64 f25;  
  /* 496 */ uint64 f26; 
  /* 504 */ uint64 f27;  
  /* 512 */ uint64 f28; 
  /* 520 */ uint64 f29;  
  /* 528 */ uint64 f30;   
  /* 536 */ uint64 f31;   
  /* 544 */ uint64 fcsr; 
};

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID

  // wait_lock must be held when using this:
  struct proc *parent;         // Parent process

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack
  uint64 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // User page table
  struct trapframe *trapframe; // data page for trampoline.S
  struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  uint64 numsyscalls;          // track the number of system calls
  int traceme;                 // enable or disable traceme
  int tracefd;                 // fd this process is writing traces to
  uint64 mmapped[MAXMMAP];     // to keep track of mmapped pages

};

#endif

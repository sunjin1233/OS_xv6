#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

extern void forkret(void);
extern void trapret(void);


extern struct _ptable {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

int thread_create(void *(*function)(void *), int priority, void *arg, void *stack){
	struct proc *p;
	char *sp;

	  //cli();

	  acquire(&ptable.lock);
	  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
	    if(p->state == UNUSED)
	      goto found;
	  release(&ptable.lock);
	  return -1;

	found:
	  p->state = RUNNABLE;
	  release(&ptable.lock);

	  if((priority <0) || (priority>40)){//priority boundary check
	    return -1;
	  }

	  //setting priority
	  p->priority = priority;

	  // Allocate kernel stack.
	  if((p->kstack = kalloc()) == 0){
	    p->state = UNUSED;
	    return -1;
	  }
	  sp = p->kstack + KSTACKSIZE;
	  
	  // Leave room for trap frame.
	  sp -= sizeof *p->tf;
	  p->tf = (struct trapframe*)sp;
	  
	  // Set up new context to start executing at forkret,
	  // which returns to trapret.
	  sp -= 4;
	  *(uint*)sp = (uint)trapret;

	  sp -= sizeof *p->context;
	  p->context = (struct context*)sp;
	  memset(p->context, 0, sizeof *p->context);
	  p->context->eip = (uint)forkret;

	  //referencing main thread
	  p->tref = proc;

	  //setting tid
	  p->tid = proc->tidcounter++;

	  //set register by argument
      stack = arg;
	  p->tf->eip = *(uint *)function;
	  p->tf->esp = *((uint*)stack+4);

	  //sti();

	  return p->tid;
}

void thread_exit(void *retval){
}

int thread_join(int tid, void **retval){
	return -1;
}

int gettid(void){
	return -1;
}

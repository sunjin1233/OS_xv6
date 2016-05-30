#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

int thread_create(void *(*function)(void *), int priority, void *arg, void *stack){
	int i, valid=0;
	char *sp;

	for(i=0; i<8; i++){
		if(proc->tp[i].state == UNUSED){//if thread
			valid++;
			break;
		}
	}

	if(valid != 0){
		//fill thread state
		proc->tp[i].ta = 1;
		proc->tp[i].state = RUNNABLE;
		if((proc->tp[i].priority <0) || (proc->tp[i].priority>40){
		    proc->tp[i].state = UNUSED;
		    return -1;
		}
		proc->tp[i].priority = priority;
		proc->tp[i].tid = proc->threadNumber;
		proc->threadNumber++;

		  // Allocate kernel stack.
		  if((proc->tp[i].kstack = kalloc()) == 0){
		    proc->tp[i].state = UNUSED;
		    return -1;
		  }

		  sp = proc->tp[i].kstack + KSTACKSIZE;
		  
		  // Leave room for trap frame.
		  sp -= sizeof *(proc->tp[i].tf);
		  proc->tp[i].tf = (struct trapframe*)sp;
		  
		  // Set up new context to start executing at forkret,
		  // which returns to trapret.
		  sp -= 4;
		  *(uint*)sp = (uint)trapret;

		  sp -= sizeof *(proc->tp[i].context);
		  proc->tp[i].context = (struct context*)sp;
		  memset(proc->tp[i].context, 0, sizeof *(proc->tp[i].context));
		  proc->tp[i].context->eip = (uint)forkret;

		  //set register by argument
		  stack = arg;
		  proc->tp[i].tf->eip = function;
		  proc->tp[i].tf->esp = stack;

		  return proc->tp[i].tid
 	}

	return -1;//error
}

void thread_exit(void *retval){
}

int thread_join(int tid, void **retval){
	return -1;
}

int gettid(void){
	return -1;
}

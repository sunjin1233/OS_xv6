#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

extern struct proc* allocproc(void);

extern struct _ptable {
	struct spinlock lock;
	struct proc proc[NPROC];
} ptable;

int thread_create(void *(*function)(void *), int priority, void *arg, void *stack){
	struct proc *p;
	void *sRet, *sArg;
	int tid;

	if((p = allocproc()) == 0){
		return -1;
	}

	if(proc->refcounter >= 7){
		return -1;
	}

	p->pid = proc->pid;
	p->tref = proc;
	proc->refcounter++;

	p->pgdir = proc->pgdir;
	p->sz = proc->sz;
	p->parent = proc->parent;
	*p->tf = *proc->tf;
	p->stack = stack;

	sRet = stack + 4096 - 2 * sizeof(void*);
	*(uint *)sRet = 0xFFFFFFFF;

	sArg = stack + 4096 - sizeof(void*);
	*(uint *)sArg = (uint)arg;

	p->tf->esp = (int)stack;
	memmove((void *)p->tf->esp, stack, PGSIZE);
	p->tf->esp += PGSIZE - 2*sizeof(void *);
	p->tf->ebp = p->tf->esp;
	p->tf->eip = (uint)function;

	//just copying ofile like fork
	/*for(i=0; i < NOFILE; i++){
		if(proc->ofile[i])
			p->ofile[i] = filedup(proc->ofile[i]);
	}*/

	p->cwd = idup(proc->cwd);
	p->tf->eax = 0;

	acquire(&ptable.lock);
	p->state = RUNNABLE;
	release(&ptable.lock);

	safestrcpy(p->name, proc->name, sizeof(proc->name));
	p->tid = proc->tid + proc->tidcounter++;
	p->priority = priority;
	tid = p->tid;

	return tid;
}

void thread_exit(void *retval){
	int fd;
	struct proc *p;

	if(proc->tref != 0){//not main thread
	    kfree(proc->kstack);
        proc->kstack = 0;
        proc->state = UNUSED;
        proc->pid = 0;
        proc->parent = 0;
        proc->name[0] = 0;
        proc->killed = 0;

        *(int*)retval= proc->tid;
	} else{//close file table

		for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
			if ( p->pgdir != proc->pgdir)//if not in same process 
	    		continue;

	        kfree(p->kstack);
	        p->kstack = 0;
	        p->state = UNUSED;
	        p->pid = 0;
	        p->parent = 0;
	        p->name[0] = 0;
	        p->killed = 0;
		}

	    // Close all open files.
        for(fd = 0; fd < NOFILE; fd++){
          if(proc->ofile[fd]){
            fileclose(proc->ofile[fd]);
            proc->ofile[fd] = 0;
          }
        }

		*(int*)retval = proc->tid;
	}
}

int thread_join(int tid, void **retval){
  struct proc *p;
  int pid;

  acquire(&ptable.lock);
  for(;;) {

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
      if ( p->pgdir != proc->pgdir || proc->tid == p->tid)//if me or not in same process 
        continue;

      if(p->tid == tid){
      	 if(p->state == UNUSED){
      	 	return -1;
      	 }
      }

      if ( ( p->state == ZOMBIE ) && ( p->tid == tid ) ) {
        // get pid of zombie child to return
        pid = p->pid;
        int *tmp = (int*) 0x1FD8;

        void *stackAddr = (void *)p->parent->tf->esp + 7*sizeof(void *);
        *(uint *)stackAddr = p->tf->ebp;
        *(uint *)stackAddr += 3 * sizeof(void *) - PGSIZE;

        kfree(p->kstack);
        p->kstack = 0;
        p->state = UNUSED;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        // Get stack of the zombie child thread to return
        *retval = p->stack;
        *tmp = pid;
        release(&ptable.lock);
        return 0;
      }
    }

    // killed
    if (proc->killed) {
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(proc, &ptable.lock);

  }
  return -1;
}

int gettid(void){
	return proc->tid;
}

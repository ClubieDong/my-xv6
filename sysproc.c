#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_clone(void)
{
  // int clone(void *(*fn)(void *), void *stack, void *arg);
  void *(*fn)(void *);
  void *stack, *arg;

  // Idk how big is the function, so `size` is set to 0
  if(argptr(0, (char**)&fn, 0) < 0)
    return -1;
  if(argptr(1, (char**)&stack, PGSIZE) < 0)
    return -1;
  // Idk how big is the argument neither, so `size` is set to 0,
  // what if arg == NULL ?
  if(argptr(2, (char**)&arg, PGSIZE) < 0)
    return -1;
  return clone(fn, stack, arg);
}

int
sys_join(void)
{
  // void join(int tid, void **ret_p, void **stack)
  int tid;
  void **ret_p, **stack;

  if(argint(0, &tid) < 0)
    return -1;
  if(argptr(1, (char**)&ret_p, sizeof(ret_p)) < 0)
    return -1;
  if(argptr(2, (char**)&stack, sizeof(stack)) < 0)
    return -1;
  join(tid, ret_p, stack);
  return 0;
}

int
sys_thread_exit(void)
{
  // void thread_exit(void *ret)
  void *ret;

  // Idk how big is the argument, so `size` is set to 0 (same as `sys_clone`)
  if(argptr(0, (char**)&ret, 0) < 0)
    return -1;
  thread_exit(ret);
  return 0;  // actually never return
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_shutdown(void){
  outw(0x604, 0x2000);
  return 0;
}

extern struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;
int
sys_set_priority(void){
  int pid, prior;

  if (argint(0, &pid) < 0)
    return -1;
  if (argint(1, &prior) < 0)
    return -1;
  if (prior < 0 || prior >= PRIORITY_LEVEL)
    return -1;
  acquire(&ptable.lock);
  for (struct proc *p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if (p->pid == pid) {
      p->prior = prior;
      release(&ptable.lock);
      return 0;
    }
  release(&ptable.lock);
  return -1;
}

extern int display_enabled;
int
sys_enable_sched_display(void){
  int n;

  if(argint(0, &n) < 0)
    return -1;
  if (n != 0 && n != 1)
    return -1;
  display_enabled = n;
  return 0;
}

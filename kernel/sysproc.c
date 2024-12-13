#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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
sys_sysinfo(void)
{
  int n;
  argint(0, &n);

  switch (n) {
    case 0:
      return process_count();

    case 1:
      return system_calls_count();

    case 2:
      return free_memory_pages_count();
    
    default:
      break;
  }

  return -1;
}

uint64
sys_procinfo(void)
{
  struct pinfo {
    int ppid;
    int syscall_count;
    int page_usage;
  } process_info;

  uint64 n;
  argaddr(0, (void *)&n);
  struct proc *process = myproc();

  process->syscall_count++;

  process_info.ppid = (process->parent) ? process->parent->pid : process->pid;
  process_info.syscall_count = process->syscall_count;
  process_info.page_usage = process->sz / PGSIZE;

  if (copyout(process->pagetable, n, (char *) &process_info, sizeof(struct pinfo)) < 0)
    return -1; // NULL input pointer, fail to copy results back to the caller process, etc.
  
  return 0;
}

uint64
sys_sched_statistics(void)
{
  print_sched_statistics();
  return 0;
}

uint64
sys_sched_tickets(void)
{
  int n;
  argint(0, &n);
  if (n > 10000) n = 10000;

  struct proc *p = myproc();
  p->tickets_alloted = n;
  p->stride = 10000/n;
  p->pass = p->stride;
  
  return 0;
}

uint64
sys_clone(void)
{
  uint64 stack;
  argaddr(0, &stack);

  if(stack == 0)
    return -1;

  return clone((void *) stack);
}

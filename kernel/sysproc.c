#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
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

int
sys_pgaccess(void)
{
  uint64 startaddr;
  int npage;
  uint64 useraddr;
  argaddr(0, &startaddr);
  argint(1, &npage);
  argaddr(2, &useraddr);

  uint64 bitmask = 0;
  uint64 complement = ~PTE_A;
  struct proc *p = myproc();
  for (int i=0; i< npage; ++i){
    pte_t *pte = walk(p->pagetable, startaddr+i*PGSIZE, 0);
    if (*pte & PTE_A){
      bitmask |= (1 <<i);
      *pte &=complement;
    }
  }
  copyout(p->pagetable, useraddr, (char *)&bitmask, sizeof(bitmask));
  return 0;
}


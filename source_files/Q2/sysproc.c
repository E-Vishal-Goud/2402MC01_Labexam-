#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
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

static pte_t*
lookup_pte(pde_t *pgdir, const void *va)
{
  pde_t *pde;
  pte_t *pgtab;

  if(pgdir ==0|| va == 0)
    return 0;
  pde = &pgdir[PDX(va)];
  if((*pde & PTE_P) == 0)
    return 0;
  pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
  return &pgtab[PTX(va)];
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_freepages(void)
{
  return get_freepages();
}

int
sys_va2pa(void)
{
  int va;

  if(argint(0, &va) < 0)
    return -1;
  return va2pa(myproc()->pgdir, (uint)va);
}

int
sys_sem_init(void)
{
  int id, value;

  if(argint(0, &id) < 0 || argint(1, &value) < 0)
    return -1;
  return sem_init(id, value);
}

int
sys_sem_wait(void)
{
  int id;

  if(argint(0, &id) < 0)
    return -1;
  return sem_wait(id);
}

int
sys_sem_post(void)
{
  int id;

  if(argint(0, &id) < 0)
    return -1;
  return sem_post(id);
}

int
sys_memstats(void)
{
  uint total_allocated;
  uint total_freed;
  uint currently_allocated;
  uint available_pages;

  get_memstats(&total_allocated, &total_freed,
               &currently_allocated, &available_pages);
  cprintf("Total allocated pages: %d\n", total_allocated);
  cprintf("Total freed pages: %d\n", total_freed);
  cprintf("Currently allocated pages: %d\n", currently_allocated);
  cprintf("Available physical memory pages: %d\n", available_pages);
  return 0;
}

int
sys_vtop(void)
{
  int va;
  uint pa;
  pte_t *pte;

  if(argint(0, &va) < 0)
    return -1;
  if(myproc() == 0 || myproc()->pgdir == 0)
    return -1;
  pte = lookup_pte(myproc()->pgdir, (void*)va);
  if(pte == 0 || (*pte & PTE_P) == 0)
    return -1;
  pa = PTE_ADDR(*pte) + ((uint)va & (PGSIZE - 1));
  return (int)pa;
}

int
sys_pageinfo(void)
{
  int va;
  pte_t *pte;
  uint frame;

  if(argint(0, &va) < 0)
    return -1;
  pte = lookup_pte(myproc()->pgdir, (void*)va);
  if(pte == 0 || (*pte & PTE_P) == 0){
    cprintf("pageinfo: virtual address 0x%x is unmapped\n", va);
    return -1;
  }
  frame = PTE_ADDR(*pte) >> PTXSHIFT;
  cprintf("VA 0x%x -> PFN %d, flags: P=%d W=%d U=%d\n",
          va, frame, (*pte & PTE_P) ? 1 : 0,
          (*pte & PTE_W) ? 1 : 0, (*pte & PTE_U) ? 1 : 0);
  return 0;
}

int
sys_setreadonly(void)
{
  int va;
  pte_t *pte;

  if(argint(0, &va) < 0)
    return -1;
  pte = lookup_pte(myproc()->pgdir, (void*)va);
  if(pte == 0 || (*pte & PTE_P) == 0)
    return -1;
  *pte &= ~PTE_W;
  return 0;
}

static int
sys_change_page_permissions(int writable)
{
  int addr, len;
  struct proc *p = myproc();

  if(argint(0, &addr) < 0 || argint(1, &len) < 0 || p == 0)
    return -1;
  if(change_page_permissions(p->pgdir, p->sz, (uint)addr, len, writable) < 0)
    return -1;
  lcr3(V2P(p->pgdir));
  return 0;
}

int
sys_mprotect(void)
{
  return sys_change_page_permissions(0);
}

int
sys_munprotect(void)
{
  return sys_change_page_permissions(1);
}

int
sys_set_priority(void)
{
  int pid, priority;

  if(argint(0, &pid) < 0 || argint(1, &priority) < 0)
    return -1;
  return set_priority(pid, priority);
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

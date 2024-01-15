#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"  // 包含struct的头文件

extern int mem_freesz(void);
extern int procnum(void);


uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
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

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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

// lab2
uint64
sys_trace(void) {
  // struct proc *p = myproc();
  int msk;
  if(argint(0, &msk) < 0)    // 调用argraw()，其会通过struct proc *p = myproc(); 和 p->trapframe->寄存器  来获取系统调用参数
    return -1;
  myproc()->tracemask |= msk;  // 使用 |=
  return 0;
}

// 类似于trace的实验过程。
// 没有实现sysinfo()函数，而是通过usys.pl生成系统调用存根来调用sys_info()函数。所以要写sys_info()函数。
// 要同步修改：syscall.h中的系统调用宏，syscall.c中的syscalls数组（记得extern函数）
int
sys_info(void) {
  uint64 addr;
    // 从寄存器中获取相应用户空间的地址
  if (argaddr(0, &addr) < 0)  // 注意n的设置，是0，不要直接复制成1
    return -1;
  // 往结构体填充对应数据
  struct sysinfo info;  // 不能直接写成指针，因为没初始化
  info.freemem = mem_freesz();
  info.nproc = procnum();
  // 内核把结构体中数据写回用户空间
  struct proc *p = myproc();
  if (copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;
  return 0;
};

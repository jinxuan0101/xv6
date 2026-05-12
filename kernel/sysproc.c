#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
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
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

// sys_pause: sleep the current process for n clock ticks.
//
// 用户态调用 pause(n) 时，内核会将当前进程挂起，直到至少经过
// n 个时钟滴答（ticks）或进程被杀死。
//
// 动作流程：
// 1. 取出用户传入的第一个参数 n。
// 2. 如果 n 为负值，则将其归零，避免无限等待或未定义行为。
// 3. 通过 tickslock 保护对全局 ticks 变量的读取和比较。
// 4. 记录当前 ticks 计数为 ticks0。
// 5. 在 ticks - ticks0 < n 的条件下循环睡眠：
//      a. 如果当前进程被 kill，则释放锁并返回 -1。
//      b. 调用 sleep(&ticks, &tickslock) 让进程睡眠，等待 wakeup(&ticks) 唤醒。
//      c. sleep() 内部会释放 tickslock，然后在被唤醒后重新获取该锁。
// 6. 循环退出后释放 tickslock，并返回 0 表示成功睡眠指定的时钟滴答。
uint64
sys_pause(void)
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
  return kkill(pid);
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

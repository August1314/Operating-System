#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

int shared_variable;
// 使用我们自定义的BTS实现的SpinLock
BTSSpinLock btsLock;

int cheese_burger;

// 使用BTS自旋锁版本的线程函数
void bts_mother(void *arg)
{
    btsLock.lock();
    int delay = 0;

    printf("BTS mother: start to make cheese burger, there are %d cheese burger now\n", cheese_burger);
    // make 10 cheese_burger
    cheese_burger += 10;

    printf("BTS mother: oh, I have to hang clothes out.\n");
    // hanging clothes out
    delay = 0xfffffff;
    while (delay)
        --delay;
    // done

    printf("BTS mother: Oh, Jesus! There are %d cheese burgers\n", cheese_burger);
    btsLock.unlock();
}

void bts_naughty_boy(void *arg)
{
    btsLock.lock();
    printf("BTS boy: Look what I found!\n");
    // eat all cheese_burgers out secretly
    cheese_burger -= 10;
    // run away as fast as possible
    btsLock.unlock();
}

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    cheese_burger = 0;
    btsLock.initialize();
    
    // 打印提示信息，表明使用的是BTSSpinLock
    printf("Using BTSSpinLock with bts instruction and lock prefix\n\n");

    programManager.executeThread(bts_mother, nullptr, "bts mother thread", 1);
    programManager.executeThread(bts_naughty_boy, nullptr, "bts boy thread", 1);

    asm_halt();
}

extern "C" void setup_kernel()
{
    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}

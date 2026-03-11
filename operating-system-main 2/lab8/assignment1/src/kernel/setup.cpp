#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"
#include "syscall.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;
// 系统调用
SystemService systemService;

int syscall_0(int first, int second, int third, int forth, int fifth)
{
    printf("systerm call 0: %d, %d, %d, %d, %d\n",
           first, second, third, forth, fifth);
    return first + second + third + forth + fifth;
}

int syscall_multiply(int first, int second, int third, int forth, int fifth){
    printf("systerm call 1 - multiply: %d, %d\n",
           first, second);
    return first * second;
}

int syscall_divide(int first, int second, int third, int forth, int fifth){
    printf("systerm call 2 - divide: %d, %d\n",
           first, second);
    return first / second;
}

int syscall_mod(int first, int second, int third, int forth, int fifth){
    printf("systerm call 3 - mod: %d, %d\n",
           first, second);
    return first % second;
}

void first_thread(void *arg)
{
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

    // 内存管理器
    memoryManager.openPageMechanism();
    memoryManager.initialize();

    // 初始化系统调用
    systemService.initialize();
    // 设置0号系统调用
    systemService.setSystemCall(0, (int)syscall_0);

    int ret;

    ret = asm_system_call(0, 123, 324, 9248, 7, 123);
    printf("return value: %d\n", ret);

    systemService.setSystemCall(1, (int)syscall_multiply);

    ret = asm_system_call(1, 123, 3, 0, 0);
    printf("return value: %d\n", ret);

    systemService.setSystemCall(2, (int)syscall_divide);

    ret = asm_system_call(2, 123, 3, 0, 0);
    printf("return value: %d\n", ret);

    systemService.setSystemCall(3, (int)syscall_mod);

    ret = asm_system_call(3, 123, 3, 0, 0);
    printf("return value: %d\n", ret);


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

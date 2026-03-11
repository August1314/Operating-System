#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"
#include "bitmap.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;

// 简单的延时函数
void delay(int count) {
    int i = 0;
    while (i < count) {
        ++i;
    }
}

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    printf("Memory Allocation Algorithm Test\n");
    printf("================================\n\n");

    // 1. 首先测试First-Fit算法
    printf("1. Testing First-Fit Algorithm\n");
    printf("-----------------------------\n");
    setAllocateAlgorithm(FIRST_FIT);
    
    // 分配内存
    char *pages_0 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 128);
    printf("Allocated 128 pages for pages_0, starting at 0x%x\n", pages_0);
    delay(0x3ffffff);

    char *pages_1 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 64);
    printf("Allocated 64 pages for pages_1, starting at 0x%x\n", pages_1);
    delay(0x3ffffff);

    char *pages_2 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 16);
    printf("Allocated 16 pages for pages_2, starting at 0x%x\n", pages_2);
    delay(0x3ffffff);

    char *pages_3 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 8);
    printf("Allocated 8 pages for pages_3, starting at 0x%x\n", pages_3);
    delay(0x3ffffff);

    // 释放部分内存
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_0), 128);
    printf("Released 128 pages for pages_0\n");
    delay(0x3ffffff);

    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_2), 16);
    printf("Released 16 pages for pages_2\n");
    delay(0x3ffffff);

    // 再次分配，First-Fit会从头开始找，应该会重用pages_0的位置
    char *pages_4 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 16);
    printf("Allocated 16 pages for pages_4, starting at 0x%x\n", pages_4);
    delay(0x3ffffff);

    char *pages_5 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 100);
    printf("Allocated 100 pages for pages_5, starting at 0x%x\n", pages_5);
    delay(0x3ffffff);

    // 释放所有分配的内存
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_1), 64);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_3), 8);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_4), 16);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_5), 100);

    printf("\nFirst-Fit Test Completed! Pausing before next test...\n");
    delay(0x8ffffff);

    // 2. 测试Best-Fit算法
    printf("2. Testing Best-Fit Algorithm\n");
    printf("---------------------------\n");
    setAllocateAlgorithm(BEST_FIT);
    
    // 创建不同大小的碎片
    pages_0 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 100);
    printf("Allocated 100 pages for pages_0, starting at 0x%x\n", pages_0);
    delay(0x3ffffff);
    
    pages_1 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 50);
    printf("Allocated 50 pages for pages_1, starting at 0x%x\n", pages_1);
    delay(0x3ffffff);
    
    pages_2 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 80);
    printf("Allocated 80 pages for pages_2, starting at 0x%x\n", pages_2);
    delay(0x3ffffff);
    
    // 释放中间的块，创建不同大小的空闲区域
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_0), 100);
    printf("Released 100 pages for pages_0\n");
    delay(0x3ffffff);
    
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_2), 80);
    printf("Released 80 pages for pages_2\n");
    delay(0x3ffffff);
    
    // 现在有两个空闲区域：100页和80页
    // 分配30页，Best-Fit应该选择较小的80页区域
    pages_3 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 30);
    printf("Allocated 30 pages for pages_3, starting at 0x%x\n", pages_3);
    delay(0x3ffffff);
    
    // 分配60页，只能放在100页的空闲区域中
    pages_4 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 60);
    printf("Allocated 60 pages for pages_4, starting at 0x%x\n", pages_4);
    delay(0x3ffffff);
    
    // 释放所有内存
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_1), 50);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_3), 30);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_4), 60);
    
    printf("\nBest-Fit Test Completed! Pausing before next test...\n");
    delay(0x8ffffff);
    
    // 3. 测试Worst-Fit算法
    printf("3. Testing Worst-Fit Algorithm\n");
    printf("----------------------------\n");
    setAllocateAlgorithm(WORST_FIT);
    
    // 创建不同大小的碎片
    pages_0 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 40);
    printf("Allocated 40 pages for pages_0, starting at 0x%x\n", pages_0);
    delay(0x3ffffff);
    
    pages_1 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 60);
    printf("Allocated 60 pages for pages_1, starting at 0x%x\n", pages_1);
    delay(0x3ffffff);
    
    pages_2 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 80);
    printf("Allocated 80 pages for pages_2, starting at 0x%x\n", pages_2);
    delay(0x3ffffff);
    
    // 释放不同大小的内存块
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_0), 40);
    printf("Released 40 pages for pages_0\n");
    delay(0x3ffffff);
    
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_2), 80);
    printf("Released 80 pages for pages_2\n");
    delay(0x3ffffff);
    
    // 现在有两个空闲区域：40页和80页
    // 分配30页，Worst-Fit应该选择较大的80页区域
    pages_3 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 30);
    printf("Allocated 30 pages for pages_3, starting at 0x%x\n", pages_3);
    delay(0x3ffffff);
    
    // 再分配25页，应该选择剩下的50页空间（80-30=50页）
    pages_4 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 25);
    printf("Allocated 25 pages for pages_4, starting at 0x%x\n", pages_4);
    delay(0x3ffffff);
    
    // 清理所有内存
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_1), 60);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_3), 30);
    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_4), 25);
    
    printf("\nWorst-Fit Test Completed!\n");
    delay(0x3ffffff);
    
    printf("\nMemory allocation algorithm tests completed!\n");
    delay(0x8ffffff);

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
    memoryManager.initialize();

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

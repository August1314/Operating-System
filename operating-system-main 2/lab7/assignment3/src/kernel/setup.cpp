#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

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
    printf("Virtual Memory Management Test\n");
    printf("===============================\n\n");

    // 测试1：基本的内存分配和释放
    printf("Test 1: Basic memory allocation and release\n");
    printf("------------------------------------------\n");
    char *p1 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 5);
    printf("p1 = 0x%x (5 pages)\n", p1);
    delay(0x1FFFFFF);
    
    char *p2 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 10);
    printf("p2 = 0x%x (10 pages)\n", p2);
    delay(0x1FFFFFF);
    
    char *p3 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 15);
    printf("p3 = 0x%x (15 pages)\n", p3);
    delay(0x1FFFFFF);
    
    // 释放内存
    printf("\nReleasing memory...\n");
    memoryManager.releasePages(AddressPoolType::KERNEL, (int)p2, 10);
    printf("Released p2 (10 pages)\n");
    delay(0x1FFFFFF);
    
    // 重新分配，应该分配在刚刚释放的p2位置
    char *p4 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 10);
    printf("p4 = 0x%x (10 pages, should be same as p2)\n", p4);
    delay(0x1FFFFFF);
    
    printf("Test 1 completed\n\n");
    delay(0x5FFFFFF);
    
    // 测试2：虚拟地址到物理地址的转换
    printf("Test 2: Virtual to physical address translation\n");
    printf("---------------------------------------------\n");
    
    if (p1) {
        int physical_addr = memoryManager.vaddr2paddr((int)p1);
        printf("Virtual address: 0x%x, Physical address: 0x%x\n", p1, physical_addr);
        delay(0x1FFFFFF);
        
        // 测试偏移地址
        int offset = 100;
        int v_addr_offset = (int)p1 + offset;
        int p_addr_offset = memoryManager.vaddr2paddr(v_addr_offset);
        printf("Virtual address+%d: 0x%x, Physical address: 0x%x\n", 
               offset, v_addr_offset, p_addr_offset);
        delay(0x1FFFFFF);
        
        printf("Physical address difference: %d (should be %d)\n", 
               p_addr_offset - physical_addr, offset);
        delay(0x1FFFFFF);
    } else {
        printf("Memory allocation failed, can't test address translation\n");
    }
    
    printf("Test 2 completed\n\n");
    delay(0x5FFFFFF);
    
    // 测试3：页表清理测试 - 重点验证页表释放功能
    printf("Test 3: Page table cleanup after memory release\n");
    printf("--------------------------------------------\n");
    
    // 先分配一个完整的页表范围内的页面（1024个页表项对应1024个页面）
    int pagesPerTable = 1024;
    printf("Allocating a single page at beginning of a new page table...\n");
    char *p5 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    printf("p5 = 0x%x (1 page)\n", p5);
    delay(0x1FFFFFF);
    
    // 获取页目录项和页表项
    int *pde = (int *)memoryManager.toPDE((int)p5);
    int *pte = (int *)memoryManager.toPTE((int)p5);
    int pageTablePhysAddr = (*pde) & 0xfffff000;
    
    printf("Before release - PDE at 0x%x = 0x%x (Page table physical address: 0x%x)\n", 
           pde, *pde, pageTablePhysAddr);
    printf("Before release - PTE at 0x%x = 0x%x\n", pte, *pte);
    delay(0x1FFFFFF);
    
    // 释放该页面，此时页表应该完全为空，且应当被释放
    printf("Releasing p5...\n");
    memoryManager.releasePages(AddressPoolType::KERNEL, (int)p5, 1);
    printf("Released p5\n");
    delay(0x1FFFFFF);
    
    // 检查页表项和页目录项的变化
    printf("After release - PTE at 0x%x = 0x%x (should be 0)\n", pte, *pte);
    printf("After release - PDE at 0x%x = 0x%x (should be 0 if page table was released)\n", 
           pde, *pde);
    delay(0x1FFFFFF);
    
    // 再次分配页面，验证页表是否被正确释放和重新分配
    printf("Allocating new page at same location...\n");
    char *p6 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    printf("p6 = 0x%x (1 page, should be same as p5=0x%x)\n", p6, p5);
    delay(0x1FFFFFF);
    
    // 检查新分配页面的页目录项和页表项
    pde = (int *)memoryManager.toPDE((int)p6);
    pte = (int *)memoryManager.toPTE((int)p6);
    int newPageTablePhysAddr = (*pde) & 0xfffff000;
    
    printf("After reallocation - PDE at 0x%x = 0x%x (Page table physical address: 0x%x)\n", 
           pde, *pde, newPageTablePhysAddr);
    printf("After reallocation - PTE at 0x%x = 0x%x\n", pte, *pte);
    printf("Page table physical address changed: %s (old=0x%x, new=0x%x)\n", 
           pageTablePhysAddr != newPageTablePhysAddr ? "YES" : "NO", 
           pageTablePhysAddr, newPageTablePhysAddr);
    delay(0x1FFFFFF);
    
    // 释放新分配的页面
    memoryManager.releasePages(AddressPoolType::KERNEL, (int)p6, 1);
    printf("Released p6\n");
    delay(0x1FFFFFF);
    
    printf("Test 3 completed\n\n");
    delay(0x5FFFFFF);
    
    // 测试4：边界条件 - 分配0页内存
    printf("Test 4: Allocating 0 pages\n");
    printf("-------------------------\n");
    
    char *zero_pages = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 0);
    printf("zero_pages = 0x%x (0 pages, should be 0)\n", zero_pages);
    delay(0x1FFFFFF);
    
    printf("Test 4 completed\n\n");
    delay(0x5FFFFFF);
    
    // 清理剩余的分配内存
    printf("Cleaning up allocated memory...\n");
    if (p1) memoryManager.releasePages(AddressPoolType::KERNEL, (int)p1, 5);
    if (p3) memoryManager.releasePages(AddressPoolType::KERNEL, (int)p3, 15);
    if (p4) memoryManager.releasePages(AddressPoolType::KERNEL, (int)p4, 10);
    
    printf("All tests completed\n");
    delay(0x5FFFFFF);

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

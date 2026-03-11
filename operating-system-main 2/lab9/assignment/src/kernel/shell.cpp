#include "shell.h"
#include "asm_utils.h"
#include "syscall.h"
#include "stdio.h"

Shell::Shell()
{
    initialize();
}

void Shell::initialize()
{
}

void Shell::run()
{
    move_cursor(0, 0);
    for(int i = 0; i < 25; ++i ) {
        for(int j = 0; j < 80; ++j ) {
            printf(" ");
        }
    }
    move_cursor(0, 0);

    printLogo();

    move_cursor(7, 26);
    printf("Welcome to YatSenOS Project!\n");
    printf("             https://github.com/YatSenOS/YatSenOS-Tutorial-Volume-1\n\n");
    printf("  YatSenOS is an OS course project.\n\n"
           "  Proposed and led by\n"
           "           Prof. PengFei Chen.\n\n"
           "  Developed by\n"
           "           Prof. PengFei Chen,\n"
           "           HPC and AI students in grade 2019,\n"
           "           HongYang Chen,\n"
           "           WenXin Xie,\n"
           "           YuZe Fu,\n"
           "           Nelson Cheung.\n\n"
           );
    
    // 测试malloc和free
    testMemoryAllocation();

    asm_halt();
}

void Shell::printLogo()
{
    move_cursor(0, 17);
    printf("__   __    _   ____              ___  ____ \n");
    move_cursor(1, 17);
    printf("\\ \\ / /_ _| |_/ ___|  ___ _ __  / _ \\/ ___| \n");
    move_cursor(2, 17);
    printf(" \\ V / _` | __\\___ \\ / _ \\ '_ \\| | | \\___ \\ \n");
    move_cursor(3, 17);
    printf("  | | (_| | |_ ___) |  __/ | | | |_| |___) |\n");
    move_cursor(4, 17);
    printf("  |_|\\__,_|\\__|____/ \\___|_| |_|\\___/|____/ \n");
}

void Shell::testMemoryAllocation()
{
    printf("\n\n=== Testing Malloc/Free Implementation ===\n");
    
    // 测试分配小内存
    printf("Testing small memory allocation...\n");
    void *ptr1 = malloc(10);
    void *ptr2 = malloc(20);
    void *ptr3 = malloc(30);
    void *ptr4 = malloc(100);
    
    printf("Allocated memory addresses: %x, %x, %x, %x\n", ptr1, ptr2, ptr3, ptr4);
    
    // 测试释放内存
    printf("Testing memory release...\n");
    free(ptr2);
    free(ptr4);
    
    // 测试再分配内存
    printf("Testing memory reallocation...\n");
    void *ptr5 = malloc(15);
    void *ptr6 = malloc(50);
    
    printf("Reallocated memory addresses: %x, %x\n", ptr5, ptr6);
    
    // 测试分配大内存
    printf("Testing large memory allocation...\n");
    void *bigPtr1 = malloc(5000);
    void *bigPtr2 = malloc(8000);
    
    printf("Large memory addresses: %x, %x\n", bigPtr1, bigPtr2);
    
    // 释放所有内存
    printf("Releasing all memory...\n");
    free(ptr1);
    free(ptr3);
    free(ptr5);
    free(ptr6);
    free(bigPtr1);
    free(bigPtr2);
    
    printf("Memory test completed!\n");
}
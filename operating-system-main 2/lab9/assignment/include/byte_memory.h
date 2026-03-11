#ifndef BYTE_MEMORY_H
#define BYTE_MEMORY_H

#include "memory.h"
#include "sync.h"
#include "os_modules.h"

// 前向声明
class MemoryManager;
class ProgramManager;
extern MemoryManager memoryManager;
extern ProgramManager programManager;

// Arena类型枚举
enum ArenaType
{
    ARENA_16,
    ARENA_32,
    ARENA_64,
    ARENA_128,
    ARENA_256,
    ARENA_512,
    ARENA_1024,
    ARENA_MORE
};

// Arena元信息结构
struct Arena
{
    ArenaType type; // Arena的类型
    int counter;    // 如果是ARENA_MORE，则counter表明页的数量，
                    // 否则counter表明该页中的可分配arena的数量
};

// 内存块链表项
struct MemoryBlockListItem
{
    MemoryBlockListItem *previous, *next;
};

// 字节级内存管理器
class ByteMemoryManager
{
private:
    // 16, 32, 64, 128, 256, 512, 1024
    static const int MEM_BLOCK_TYPES = 7;       // 内存块的类型数目
    static const int minSize = 16;              // 内存块的最小大小
    int arenaSize[MEM_BLOCK_TYPES];             // 每种类型对应的内存块大小
    MemoryBlockListItem *arenas[MEM_BLOCK_TYPES]; // 每种类型的arena内存块的指针
    
    // 同步互斥锁，确保线程安全
    int mutex; // 使用简单的整数代替Mutex类型

public:
    ByteMemoryManager();
    void initialize();
    void *allocate(int size);  // 分配一块地址
    void release(void *address); // 释放一块地址

private:
    bool getNewArena(AddressPoolType type, int index);
};

// 全局内核字节级内存管理器
extern ByteMemoryManager kernelByteMemoryManager;

// 系统调用函数声明
void *syscall_malloc(int size);
void syscall_free(void *address);

#endif 
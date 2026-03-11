#include "byte_memory.h"
#include "program.h"
#include "stdio.h"
#include "os_modules.h"

// 全局内核字节级内存管理器
ByteMemoryManager kernelByteMemoryManager;

// 实现构造函数
ByteMemoryManager::ByteMemoryManager()
{
    initialize();
}

// 初始化函数
void ByteMemoryManager::initialize()
{
    int size = minSize;
    for (int i = 0; i < MEM_BLOCK_TYPES; ++i)
    {
        arenas[i] = nullptr;
        arenaSize[i] = size;
        size = size << 1;
    }
    
    // 初始化互斥锁
    mutex = 0;
}

// 简单的加锁和解锁实现
void lock(int *mutex)
{
    while(__sync_lock_test_and_set(mutex, 1)) {
        // 自旋等待锁释放
    }
}

void unlock(int *mutex)
{
    __sync_lock_release(mutex);
}

// 内存分配函数
void *ByteMemoryManager::allocate(int size)
{
    // 加锁，确保线程安全
    lock(&mutex);
    
    int index = 0;
    while (index < MEM_BLOCK_TYPES && arenaSize[index] < size)
        ++index;

    PCB *pcb = programManager.running;
    AddressPoolType poolType = (pcb->pageDirectoryAddress) ? AddressPoolType::USER : AddressPoolType::KERNEL;
    void *ans = nullptr;

    if (index == MEM_BLOCK_TYPES)
    {
        // 大内存分配，按页分配
        // 上取整
        int pageAmount = (size + sizeof(Arena) + PAGE_SIZE - 1) / PAGE_SIZE;

        // allocatePages返回整数地址，需转换为void*
        int addr = memoryManager.allocatePages(poolType, pageAmount);
        if (addr)
        {
            ans = (void*)addr;
            Arena *arena = (Arena *)ans;
            arena->type = ArenaType::ARENA_MORE;
            arena->counter = pageAmount;
        }
    }
    else
    {
        // 从现有arena中分配
        if (arenas[index] == nullptr)
        {
            if (!getNewArena(poolType, index))
            {
                unlock(&mutex);
                return nullptr;
            }
        }

        // 每次取出内存块链表中的第一个内存块
        ans = arenas[index];
        arenas[index] = ((MemoryBlockListItem *)ans)->next;

        if (arenas[index])
        {
            (arenas[index])->previous = nullptr;
        }

        Arena *arena = (Arena *)((int)ans & 0xfffff000);
        --(arena->counter);
    }

    // 解锁
    unlock(&mutex);
    return ans;
}

// 获取新的arena
bool ByteMemoryManager::getNewArena(AddressPoolType type, int index)
{
    // allocatePages返回整数地址，需转换为void*
    int addr = memoryManager.allocatePages(type, 1);
    void *ptr = (addr ? (void*)addr : nullptr);

    if (ptr == nullptr)
        return false;

    // 内存块的数量
    int times = (PAGE_SIZE - sizeof(Arena)) / arenaSize[index];
    // 内存块的起始地址
    int address = (int)ptr + sizeof(Arena);

    // 记录下内存块的数据
    Arena *arena = (Arena *)ptr;
    arena->type = (ArenaType)index;
    arena->counter = times;

    MemoryBlockListItem *prevPtr = (MemoryBlockListItem *)address;
    MemoryBlockListItem *curPtr = nullptr;
    arenas[index] = prevPtr;
    prevPtr->previous = nullptr;
    prevPtr->next = nullptr;
    --times;

    // 创建空闲内存块链表
    while (times)
    {
        address += arenaSize[index];
        curPtr = (MemoryBlockListItem *)address;
        prevPtr->next = curPtr;
        curPtr->previous = prevPtr;
        curPtr->next = nullptr;
        prevPtr = curPtr;
        --times;
    }

    return true;
}

// 内存释放函数
void ByteMemoryManager::release(void *address)
{
    // 加锁，确保线程安全
    lock(&mutex);
    
    // 由于Arena是按页分配的，所以其首地址的低12位必定0，
    // 其中划分的内存块的高20位也必定与其所在的Arena首地址相同
    Arena *arena = (Arena *)((int)address & 0xfffff000);

    if (arena->type == ARENA_MORE)
    {
        // 大内存直接释放页
        int pageAddr = (int)arena;
        memoryManager.releasePages(AddressPoolType::USER, pageAddr, arena->counter);
    }
    else
    {
        // 将内存块放回链表
        MemoryBlockListItem *itemPtr = (MemoryBlockListItem *)address;
        itemPtr->next = arenas[arena->type];
        itemPtr->previous = nullptr;

        if (itemPtr->next)
        {
            itemPtr->next->previous = itemPtr;
        }

        arenas[arena->type] = itemPtr;
        ++(arena->counter);

        // 若整个Arena被归还，则清空分配给Arena的页
        int amount = (PAGE_SIZE - sizeof(Arena)) / arenaSize[arena->type];

        if (arena->counter == amount)
        {
            // 将属于Arena的内存块从链上删除
            itemPtr = arenas[arena->type];
            while (itemPtr)
            {
                // 使用符号整数比较，避免有符号/无符号比较警告
                int itemAddr = (int)itemPtr & 0xfffff000;
                int arenaAddr = (int)arena;
                if (arenaAddr != itemAddr)
                {
                    itemPtr = itemPtr->next;
                    continue;
                }

                if (itemPtr->previous == nullptr) // 链表中的第一个节点
                {
                    arenas[arena->type] = itemPtr->next;
                    if (itemPtr->next)
                    {
                        itemPtr->next->previous = nullptr;
                    }
                }
                else
                {
                    itemPtr->previous->next = itemPtr->next;
                }

                if (itemPtr->next)
                {
                    itemPtr->next->previous = itemPtr->previous;
                }

                itemPtr = itemPtr->next;
            }

            // 释放页
            memoryManager.releasePages(AddressPoolType::USER, (int)arena, 1);
        }
    }
    
    // 解锁
    unlock(&mutex);
}

// 为每个进程创建一个ByteMemoryManager实例
ByteMemoryManager *createProcessMemoryManager() {
    // 分配一页作为ByteMemoryManager
    int addr = memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    if (!addr) {
        return nullptr;
    }
    
    ByteMemoryManager *manager = (ByteMemoryManager*)addr;
    // 调用构造函数
    manager->initialize();
    
    return manager;
}

// 系统调用函数实现 - malloc
void *syscall_malloc(int size)
{
    PCB *pcb = programManager.running;
    if (pcb->pageDirectoryAddress)
    {
        // 每一个进程有自己的ByteMemoryManager
        if (pcb->byteMemoryManager == nullptr)
        {
            // 为进程分配内存管理器，避免使用new
            pcb->byteMemoryManager = createProcessMemoryManager();
            if (!pcb->byteMemoryManager) {
                return nullptr;
            }
        }
        return pcb->byteMemoryManager->allocate(size);
    }
    else
    {
        // 所有内核线程共享一个ByteMemoryManager
        return kernelByteMemoryManager.allocate(size);
    }
}

// 系统调用函数实现 - free
void syscall_free(void *address)
{
    PCB *pcb = programManager.running;
    if (pcb->pageDirectoryAddress)
    {
        if (pcb->byteMemoryManager)
        {
            pcb->byteMemoryManager->release(address);
        }
    }
    else
    {
        kernelByteMemoryManager.release(address);
    }
} 
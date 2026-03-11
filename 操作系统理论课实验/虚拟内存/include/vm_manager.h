#ifndef VM_MANAGER_H
#define VM_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// 常量定义
#define PAGE_TABLE_SIZE 256    // 页表大小，256条目
#define PAGE_SIZE 256          // 页面大小，256字节
#define TLB_SIZE 16            // TLB条目数量
#define FRAME_SIZE 256         // 帧大小，256字节
#define FRAME_COUNT 256        // 帧数量，256个
#define MEMORY_SIZE (FRAME_SIZE * FRAME_COUNT)  // 物理内存大小，65536字节
#define BACKING_STORE_FILE "BACKING_STORE.bin"  // 后备存储文件名

// TLB条目结构
typedef struct {
    uint8_t page_number;       // 页码
    uint8_t frame_number;      // 帧码
    int valid;                 // 有效位，1表示有效，0表示无效
    int timestamp;             // 时间戳，用于LRU策略
} TLBEntry;

// 页表条目结构
typedef struct {
    uint8_t frame_number;      // 帧码
    int valid;                 // 有效位，1表示有效，0表示无效
} PageTableEntry;

// 虚拟内存管理器结构
typedef struct {
    uint8_t physical_memory[MEMORY_SIZE];   // 物理内存
    PageTableEntry page_table[PAGE_TABLE_SIZE];  // 页表
    TLBEntry tlb[TLB_SIZE];                 // TLB
    int tlb_index;                          // TLB索引，用于FIFO替换策略
    int tlb_timer;                          // TLB时间计数器，用于LRU策略
    
    uint8_t next_frame;                     // 下一个可用帧号
    uint8_t frame_used[FRAME_COUNT];        // 帧使用情况，1表示已使用，0表示可用
    
    // 统计数据
    int total_addresses;                    // 处理的总地址数
    int page_faults;                        // 缺页错误数
    int tlb_hits;                           // TLB命中数
    
    FILE* backing_store;                    // 后备存储文件
} VMManager;

// 函数声明
VMManager* vm_init();                     // 初始化虚拟内存管理器
void vm_free(VMManager* vm);              // 释放虚拟内存管理器资源

// 核心功能函数
int vm_translate_address(VMManager* vm, uint32_t logical_address, uint32_t* physical_address, char* value);
int vm_lookup_tlb(VMManager* vm, uint8_t page_number, uint8_t* frame_number);
int vm_lookup_page_table(VMManager* vm, uint8_t page_number, uint8_t* frame_number);
int vm_handle_page_fault(VMManager* vm, uint8_t page_number, uint8_t* frame_number);
void vm_update_tlb(VMManager* vm, uint8_t page_number, uint8_t frame_number);
uint8_t vm_allocate_frame(VMManager* vm);  // 分配一个可用帧

// 辅助函数
void vm_print_statistics(VMManager* vm);   // 打印统计信息
void vm_extract_page_offset(uint32_t logical_address, uint8_t* page_number, uint8_t* offset);

#endif /* VM_MANAGER_H */ 
#include "../include/vm_manager.h"

// 初始化虚拟内存管理器
VMManager* vm_init() {
    VMManager* vm = (VMManager*)malloc(sizeof(VMManager));
    if (vm == NULL) {
        fprintf(stderr, "内存分配失败！\n");
        exit(EXIT_FAILURE);
    }

    // 初始化物理内存为0
    memset(vm->physical_memory, 0, MEMORY_SIZE);
    
    // 初始化页表为无效
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        vm->page_table[i].valid = 0;
    }
    
    // 初始化TLB为无效
    for (int i = 0; i < TLB_SIZE; i++) {
        vm->tlb[i].valid = 0;
        vm->tlb[i].timestamp = 0;
    }
    
    // 初始化帧管理
    vm->next_frame = 0;
    memset(vm->frame_used, 0, FRAME_COUNT);
    
    // 初始化统计数据
    vm->total_addresses = 0;
    vm->page_faults = 0;
    vm->tlb_hits = 0;
    vm->tlb_index = 0;
    vm->tlb_timer = 0;
    
    // 打开后备存储文件
    vm->backing_store = fopen(BACKING_STORE_FILE, "rb");
    if (vm->backing_store == NULL) {
        fprintf(stderr, "无法打开后备存储文件 %s\n", BACKING_STORE_FILE);
        free(vm);
        exit(EXIT_FAILURE);
    }
    
    return vm;
}

// 释放虚拟内存管理器资源
void vm_free(VMManager* vm) {
    if (vm) {
        if (vm->backing_store) {
            fclose(vm->backing_store);
        }
        free(vm);
    }
}

// 分配一个可用帧
uint8_t vm_allocate_frame(VMManager* vm) {
    // 循环分配策略：按顺序分配帧
    uint8_t frame = vm->next_frame;
    vm->frame_used[frame] = 1;
    
    // 更新下一个可用帧
    vm->next_frame = (vm->next_frame + 1) % FRAME_COUNT;
    
    return frame;
}

// 从逻辑地址提取页码和偏移量
void vm_extract_page_offset(uint32_t logical_address, uint8_t* page_number, uint8_t* offset) {
    // 屏蔽高16位，仅保留低16位
    uint16_t address = logical_address & 0xFFFF;
    
    // 提取页码（高8位）
    *page_number = (address >> 8) & 0xFF;
    
    // 提取偏移量（低8位）
    *offset = address & 0xFF;
}

// 在TLB中查找页码对应的帧码
int vm_lookup_tlb(VMManager* vm, uint8_t page_number, uint8_t* frame_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (vm->tlb[i].valid && vm->tlb[i].page_number == page_number) {
            *frame_number = vm->tlb[i].frame_number;
            vm->tlb[i].timestamp = vm->tlb_timer++;  // 更新时间戳，用于LRU策略
            return 1; // TLB命中
        }
    }
    return 0; // TLB未命中
}

// 在页表中查找页码对应的帧码
int vm_lookup_page_table(VMManager* vm, uint8_t page_number, uint8_t* frame_number) {
    if (vm->page_table[page_number].valid) {
        *frame_number = vm->page_table[page_number].frame_number;
        return 1; // 页表命中
    }
    return 0; // 页表未命中，需要处理缺页错误
}

// 处理缺页错误
int vm_handle_page_fault(VMManager* vm, uint8_t page_number, uint8_t* frame_number) {
    uint8_t buffer[PAGE_SIZE];
    
    // 在后备存储中定位到对应页面
    long offset = page_number * PAGE_SIZE;
    if (fseek(vm->backing_store, offset, SEEK_SET) != 0) {
        fprintf(stderr, "后备存储文件寻址失败！\n");
        return 0;
    }
    
    // 从后备存储读取页面数据
    if (fread(buffer, sizeof(uint8_t), PAGE_SIZE, vm->backing_store) != PAGE_SIZE) {
        fprintf(stderr, "从后备存储读取数据失败！\n");
        return 0;
    }
    
    // 分配一个可用帧
    *frame_number = vm_allocate_frame(vm);
    
    // 将页面数据复制到物理内存
    int frame_offset = (*frame_number) * FRAME_SIZE;
    memcpy(&(vm->physical_memory[frame_offset]), buffer, PAGE_SIZE);
    
    // 更新页表
    vm->page_table[page_number].frame_number = *frame_number;
    vm->page_table[page_number].valid = 1;
    
    return 1; // 缺页处理成功
}

// 更新TLB
void vm_update_tlb(VMManager* vm, uint8_t page_number, uint8_t frame_number) {
    int replace_index;
    int found_invalid = 0;
    
    // 查找无效的TLB条目
    for (int i = 0; i < TLB_SIZE; i++) {
        if (!vm->tlb[i].valid) {
            replace_index = i;
            found_invalid = 1;
            break;
        }
    }
    
    // 如果没有无效条目，使用FIFO置换策略
    if (!found_invalid) {
        replace_index = vm->tlb_index;
        vm->tlb_index = (vm->tlb_index + 1) % TLB_SIZE;
        
        // 或者使用LRU策略，查找最早使用的条目
        /*
        int min_timestamp = vm->tlb[0].timestamp;
        replace_index = 0;
        
        for (int i = 1; i < TLB_SIZE; i++) {
            if (vm->tlb[i].timestamp < min_timestamp) {
                min_timestamp = vm->tlb[i].timestamp;
                replace_index = i;
            }
        }
        */
    }
    
    // 更新TLB条目
    vm->tlb[replace_index].page_number = page_number;
    vm->tlb[replace_index].frame_number = frame_number;
    vm->tlb[replace_index].valid = 1;
    vm->tlb[replace_index].timestamp = vm->tlb_timer++;
}

// 将逻辑地址转换为物理地址
int vm_translate_address(VMManager* vm, uint32_t logical_address, uint32_t* physical_address, char* value) {
    uint8_t page_number, offset;
    uint8_t frame_number;
    
    // 递增总地址计数
    vm->total_addresses++;
    
    // 从逻辑地址提取页码和偏移量
    vm_extract_page_offset(logical_address, &page_number, &offset);
    
    // 先查询TLB
    if (vm_lookup_tlb(vm, page_number, &frame_number)) {
        // TLB命中
        vm->tlb_hits++;
    } else {
        // TLB未命中，查询页表
        if (!vm_lookup_page_table(vm, page_number, &frame_number)) {
            // 页表未命中，处理缺页错误
            vm->page_faults++;
            if (!vm_handle_page_fault(vm, page_number, &frame_number)) {
                return 0; // 缺页处理失败
            }
        }
        // 更新TLB
        vm_update_tlb(vm, page_number, frame_number);
    }
    
    // 计算物理地址
    *physical_address = (frame_number * FRAME_SIZE) + offset;
    
    // 读取物理地址中的值
    *value = vm->physical_memory[*physical_address];
    
    return 1; // 地址转换成功
}

// 打印统计信息
void vm_print_statistics(VMManager* vm) {
    double page_fault_rate = 0.0;
    double tlb_hit_rate = 0.0;
    
    if (vm->total_addresses > 0) {
        page_fault_rate = (double)vm->page_faults / vm->total_addresses * 100.0;
        tlb_hit_rate = (double)vm->tlb_hits / vm->total_addresses * 100.0;
    }
    
    printf("\n统计信息：\n");
    printf("总共处理的地址数: %d\n", vm->total_addresses);
    printf("缺页错误数: %d\n", vm->page_faults);
    printf("缺页错误率: %.2f%%\n", page_fault_rate);
    printf("TLB命中数: %d\n", vm->tlb_hits);
    printf("TLB命中率: %.2f%%\n", tlb_hit_rate);
} 
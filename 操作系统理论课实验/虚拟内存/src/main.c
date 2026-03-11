#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/vm_manager.h"

#define MAX_LINE_LENGTH 256

int main(int argc, char *argv[]) {
    // 检查命令行参数
    if (argc != 2) {
        fprintf(stderr, "使用方法：%s [地址文件]\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    // 打开地址文件
    FILE *address_file = fopen(argv[1], "r");
    if (!address_file) {
        fprintf(stderr, "无法打开地址文件：%s\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    // 初始化虚拟内存管理器
    VMManager *vm = vm_init();
    if (!vm) {
        fprintf(stderr, "初始化虚拟内存管理器失败！\n");
        fclose(address_file);
        return EXIT_FAILURE;
    }
    
    char line[MAX_LINE_LENGTH];
    uint32_t logical_address;
    uint32_t physical_address;
    char value;
    
    // 每行读取一个逻辑地址并进行转换
    while (fgets(line, MAX_LINE_LENGTH, address_file)) {
        // 解析逻辑地址
        logical_address = (uint32_t)strtoul(line, NULL, 10);
        
        // 将逻辑地址转换为物理地址
        if (vm_translate_address(vm, logical_address, &physical_address, &value)) {
            printf("逻辑地址：%u, 物理地址：%u, 值：%d\n", 
                   logical_address, physical_address, (signed char)value);
        } else {
            fprintf(stderr, "转换逻辑地址 %u 失败！\n", logical_address);
        }
    }
    
    // 打印统计信息
    vm_print_statistics(vm);
    
    // 释放资源
    fclose(address_file);
    vm_free(vm);
    
    return EXIT_SUCCESS;
} 
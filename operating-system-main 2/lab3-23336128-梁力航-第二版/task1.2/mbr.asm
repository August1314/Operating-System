%include "boot.inc"

[bits 16]
xor ax, ax ; eax = 0
; 初始化段寄存器
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; 初始化栈指针
mov sp, 0x7c00    

; 显示MBR加载中的信息
mov ax, 0xb800
mov gs, ax
mov ah, 0x02 ; 绿色
mov al, 'M'
mov [gs:0], ax
mov al, 'B'
mov [gs:2], ax
mov al, 'R'
mov [gs:4], ax

mov ax, LOADER_START_SECTOR  ; 起始逻辑扇区号
mov cx, LOADER_SECTOR_COUNT  ; 要读取的扇区数
mov bx, LOADER_START_ADDRESS ; 加载地址

load_bootloader:
    push ax
    push bx
    push cx
    
    ; 显示当前加载的扇区号
    mov dx, ax
    add dl, '0'  ; 转换为ASCII
    mov ax, 0xb800
    mov gs, ax
    mov ah, 0x06 ; 棕色
    mov al, dl
    mov [gs:6], ax  ; 在'MBR'后面显示扇区号
    
    ; 恢复参数
    pop cx
    push cx
    pop bx
    push bx
    pop ax
    push ax
    
    ; 调用CHS方式读取硬盘
    call asm_read_hard_disk_chs
    
    ; 显示成功读取标记
    mov ax, 0xb800
    mov gs, ax
    mov ah, 0x0A ; 亮绿色
    mov al, 'S'
    mov [gs:8], ax  ; 显示成功标记
    
    pop cx
    pop bx
    pop ax
    
    inc ax         ; 下一个扇区
    add bx, 512    ; 下一个内存位置
    loop load_bootloader
    
    ; 显示准备跳转标记
    mov ax, 0xb800
    mov gs, ax
    mov ah, 0x0E ; 黄色
    mov al, 'J'
    mov [gs:10], ax
    mov al, 'M'
    mov [gs:12], ax
    mov al, 'P'
    mov [gs:14], ax
    
    ; 直接在此处验证bootloader的前几个字节
    mov si, 0x7e00
    mov al, [si]
    mov ah, 0x0C
    mov [gs:20], ax
    inc si
    mov al, [si]
    mov ah, 0x0C
    mov [gs:22], ax
    inc si
    mov al, [si]
    mov ah, 0x0C
    mov [gs:24], ax
    
    jmp 0x0000:0x7e00  ; 跳转到bootloader

; CHS方式读取硬盘
; 参数: ax=逻辑扇区号, bx=目标内存地址
asm_read_hard_disk_chs:
    push bp
    mov bp, sp
    
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    
    ; LBA到CHS的转换
    mov ax, [bp + 2 * 3]  ; 逻辑扇区号
    
    ; 计算扇区号: S = (LBA % 63) + 1
    mov dx, 0
    mov cx, 63
    div cx        ; ax = LBA / 63, dx = LBA % 63
    inc dx        ; S = (LBA % 63) + 1
    mov cl, dl    ; CL低6位存储扇区号
    
    ; 计算磁头号和柱面号
    mov dx, 0
    mov cx, 18
    div cx        ; ax = (LBA / 63) / 18, dx = (LBA / 63) % 18
    
    ; DH = 磁头号 = (LBA / 63) % 18
    mov dh, dl
    
    ; 柱面号 = (LBA / 63) / 18
    mov ch, al    ; CH = 柱面号低8位
    
    ; 柱面号高2位放入CL的高2位
    shl ah, 6
    or cl, ah
    
    ; 准备读取参数
    mov dl, 0x80   ; 驱动器号 = 80h (第一个硬盘)
    mov ax, 0x0201 ; AH=02h(读扇区), AL=01(读1个扇区)
    
    ; ES:BX = 目标地址
    xor ax, ax     ; 确保ES为0
    mov es, ax
    mov bx, [bp + 2 * 2]  ; 目标内存地址
    
    mov ax, 0x0201 ; 由于前面修改了AX，这里需要重新设置读扇区命令
    
    int 0x13      ; 调用BIOS中断读取扇区
    jc .error     ; 如果出错(CF=1)，跳转到错误处理
    
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    pop bp
    ret
    
.error:
    ; 读取错误处理 - 显示错误代码
    mov cx, ax    ; 保存错误代码
    
    mov ax, 0xb800
    mov gs, ax
    mov ah, 0x4F  ; 白底红字
    mov al, 'E'
    mov [gs:160], ax  ; 显示在第二行开始位置
    mov al, 'R'
    mov [gs:162], ax
    mov al, 'R'
    mov [gs:164], ax
    
    ; 显示错误代码
    mov ax, cx
    mov ah, 0x4F
    mov [gs:166], ax
    
    jmp $           ; 循环

times 510 - ($ - $$) db 0
db 0x55, 0xaa 
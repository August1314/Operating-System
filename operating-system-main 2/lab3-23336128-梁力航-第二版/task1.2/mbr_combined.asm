[bits 16]
org 0x7c00

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

; 显示1号扇区内容 - 模拟CHS读取
mov ax, 1    ; 逻辑扇区号为1
call read_sector_chs

; 直接显示bootloader消息
mov ax, 0xb800
mov gs, ax
mov ah, 0x4F ; 白底红字
mov ecx, bootloader_msg_end - bootloader_msg
mov ebx, 80*2 ; 在第二行开始显示，更明显
mov esi, bootloader_msg
output_bootloader_msg:
    mov al, [esi]
    mov word[gs:bx], ax
    inc esi
    add ebx,2
    loop output_bootloader_msg

jmp $ ; 死循环

; CHS方式读取硬盘函数
; 参数: ax=逻辑扇区号
read_sector_chs:
    push ax
    push bx
    push cx
    push dx
    
    ; LBA到CHS的转换
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
    
    mov dl, 0x80   ; 驱动器号 = 80h (第一个硬盘)
    mov ax, 0x0201 ; AH=02h(读扇区), AL=01(读1个扇区)
    
    ; ES:BX = 目标地址
    xor ax, ax
    mov es, ax
    mov bx, 0x8000 ; 随便一个地址，用于测试
    
    mov ax, 0x0201 ; 恢复读扇区命令
    
    int 0x13      ; 调用BIOS中断读取扇区
    jc .error     ; 如果出错(CF=1)，跳转到错误处理
    
    ; 显示CHS成功读取标记
    mov ax, 0xb800
    mov gs, ax
    mov ah, 0x0A ; 亮绿色
    mov al, 'C'
    mov [gs:10], ax
    mov al, 'H'
    mov [gs:12], ax
    mov al, 'S'
    mov [gs:14], ax
    mov al, '!'
    mov [gs:16], ax
    
    pop dx
    pop cx
    pop bx
    pop ax
    ret
    
.error:
    ; 显示错误信息
    mov cx, ax    ; 保存错误代码
    
    mov ax, 0xb800
    mov gs, ax
    mov ah, 0x4F  ; 白底红字
    mov al, 'E'
    mov [gs:40], ax
    mov al, 'R'
    mov [gs:42], ax
    mov al, 'R'
    mov [gs:44], ax
    
    ; 显示AH中的错误代码
    mov al, ch
    mov [gs:46], ax
    
    pop dx
    pop cx
    pop bx
    pop ax
    ret

bootloader_msg db 'CHS mode success'
bootloader_msg_end:

times 510 - ($ - $$) db 0
db 0x55, 0xaa 
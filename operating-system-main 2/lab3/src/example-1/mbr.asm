org 0x7c00
[bits 16]
xor ax, ax ; eax = 0
; 初始化段寄存器, 段地址全部设为0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; 初始化栈指针
mov sp, 0x7c00
mov ax, 1                ; 逻辑扇区号第0~15位
mov cx, 0                ; 逻辑扇区号第16~31位
mov bx, 0x7e00           ; bootloader的加载地址
load_bootloader:
    call asm_read_hard_disk  ; 读取硬盘
    inc ax
    cmp ax, 5
    jle load_bootloader
jmp 0x0000:0x7e00        ; 跳转到bootloader

jmp $ ; 死循环

asm_read_hard_disk:
    pusha                   ; 保护所有寄存器

    ;--- LBA转CHS参数（适配BIOS规范） ---
    mov  si, ax             ; 保存LBA到SI
    xor  dx, dx             ; 清空DX用于除法
    mov  cx, 63             ; 每磁道扇区数（SPT=63）
    div  cx                 ; AX=LBA/63（柱面/磁头商），DX=余数（扇区号-1）
    inc  dx                 ; DX=扇区号（1~63）
    mov  cl, dl             ; CL=扇区号（低6位）

    xor  dx, dx             ; 再次清空DX
    mov  cx, 16             ; 每柱面磁头数（HPC=16）
    div  cx                 ; AX=柱面号，DX=磁头号（0~15）
    mov  dh, dl             ; DH=磁头号（0~15）
    mov  ch, al             ; CH=柱面号低8位
    shr  ax, 8              ; 柱面号高2位（存到CL高2位）
    and  ax, 0x03          ; 仅保留柱面号高2位
    or   cl, al             ; CL高2位存储柱面号高2位

    ;--- 调用BIOS中断INT 13h AH=02h ---
    mov  ax, 0x0201         ; AH=02h（读扇区），AL=1（读1个扇区）
    mov  dl, 0x80           ; DL=0x80（主硬盘）
    int  0x13               ; 调用BIOS中断
    jc   .error             ; 失败处理（CF=1）

    add  bx, 512            ; BX+=512（移动到下一个缓冲区地址）
    popa                    ; 恢复寄存器
    clc                     ; 清除进位标志（成功）
    ret

.error:
    popa
    stc                     ; 设置进位标志（失败）
    ret

times 510 - ($ - $$) db 0
db 0x55, 0xaa
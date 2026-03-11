[bits 16]
org 0x7c00

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

; 初始化视频模式
mov ax, 3
int 10h
mov ax, 0xB800
mov es, ax

; 初始参数
mov dl, 2      ; X=2 (0-79)
xor dh, dh     ; Y=0 (0-24)
mov bl, 1      ; 水平速度
mov bh, 1      ; 垂直速度
mov cl, '0'    ; 当前字符
mov ch, 0x0A   ; 颜色属性

main:
; 检测按键
mov ah, 1
int 16h
jnz exit

; 计算显存地址
mov al, dh
mov ah, 80
mul ah
add al, dl
adc ah, 0
shl ax, 1
mov di, ax

; 写入字符
mov [es:di], cx
add ch, 0x73
and ch, 0xFF    ; 保持bit7为0（无闪烁）

; 字符递增
inc cl
cmp cl, '9'
jbe .no_reset
mov cl, '0'

.no_reset:
; 移动坐标
add dl, bl
add dh, bh

; 水平边界检测
cmp dl, 79
jg .right
cmp dl, 0
jl .left
.x_ok:

; 垂直边界检测
cmp dh, 24
jg .down
cmp dh, 0
jl .up
.y_ok:

; 延时循环
mov bp, 0xFFFF
.delay:
dec bp
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jmp $+2
jnz .delay

jmp main

; 边界处理
.right:
mov dl, 78
neg bl
jmp .x_ok

.left:
mov dl, 1
neg bl
jmp .x_ok

.down:
mov dh, 23
neg bh
jmp .y_ok

.up:
mov dh, 1
neg bh
jmp .y_ok

exit:
mov ax, 3      ; 恢复文本模式
int 10h
mov ax, 0x4C00 ; 返回DOS
int 21h

times 510 - ($ - $$) db 0
dw 0xaa55
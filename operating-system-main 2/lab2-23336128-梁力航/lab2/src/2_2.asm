[bits 16]
xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov sp, 0x7c00

; 设置光标到(8,8)
mov ah, 0x02
mov bh, 0
mov dh, 8 ; 行号8 (从0开始)
mov dl, 8 ; 列号8 (从0开始)
int 10h

mov ah, 0x09
mov al, '2'
mov bh, 0
mov bl, 0x52
mov cx, 1
int 10h

; 设置光标到(8,9)
mov ah, 0x02
mov dh, 8 ; 行号8 (从0开始)
mov dl, 9 ; 列号8 (从0开始)
int 10h

mov ah, 0x09
mov al, '3'
mov bh, 0
mov bl, 0x52
mov cx, 3
int 10h

; 设置光标到(8,9)
mov ah, 0x02
mov dh, 8 ; 行号8 (从0开始)
mov dl, 12 ; 列号8 (从0开始)
int 10h

mov ah, 0x09
mov al, '6'
mov bh, 0
mov bl, 0x52
mov cx, 1
int 10h

; 设置光标到(8,9)
mov ah, 0x02
mov dh, 8 ; 行号8 (从0开始)
mov dl, 13 ; 列号8 (从0开始)
int 10h

mov ah, 0x09
mov al, '1'
mov bh, 0
mov bl, 0x52
mov cx, 1
int 10h

; 设置光标到(8,9)
mov ah, 0x02
mov dh, 8 ; 行号8 (从0开始)
mov dl, 14 ; 列号8 (从0开始)
int 10h

mov ah, 0x09
mov al, '2'
mov bh, 0
mov bl, 0x52
mov cx, 1
int 10h

; 设置光标到(8,9)
mov ah, 0x02
mov dh, 8 ; 行号8 (从0开始)
mov dl, 15 ; 列号8 (从0开始)
int 10h

mov ah, 0x09
mov al, '8'
mov bh, 0
mov bl, 0x52
mov cx, 1
int 10h

    jmp $

times 510 - ($ - $$) db 0
db 0x55, 0xaa
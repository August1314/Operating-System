[bits 16]
xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov sp, 0x7c00

; 初始化光标位置 (8,8)
mov ah, 0x02
mov bh, 0
mov dh, 8    ; 行号8
mov dl, 0    ; 列号8
int 0x10

input_loop:
    ; 阻塞读取键盘输入 (AH=0x00)
    mov ah, 0x00
    int 0x16      ; 返回: AL=ASCII码, AH=扫描码

    ; 判断是否为回车键（ASCII 0x0D）
    cmp al, 0x0D
    je exit       ; 是回车则退出

    ; 判断是否为退格键（ASCII 0x08）
    cmp al, 0x08
    je handle_backspace

    ; 正常字符回显（使用电传打字功能）
    mov ah, 0x0E    ; 功能号：显示字符并移动光标
    mov bh, 0       ; 页码0
    mov bl, 0x04    ; 红色前景
    int 0x10

    jmp input_loop   ; 继续循环

handle_backspace:
    ; 获取当前光标位置
    mov ah, 0x03
    int 0x10        ; 返回 DH=行, DL=列

    ; 如果光标已在起始位置，不做处理
    cmp dl, 8
    jbe input_loop

    ; 光标左移一格
    dec dl
    mov ah, 0x02
    int 0x10

    ; 输出空格覆盖原字符
    mov al, ' '
    mov ah, 0x0E
    int 0x10

    ; 光标再次左移
    mov ah, 0x03
    int 0x10
    dec dl
    mov ah, 0x02
    int 0x10

    jmp input_loop

exit:
    jmp $           ; 无限循环

times 510 - ($ - $$) db 0
db 0x55, 0xaa
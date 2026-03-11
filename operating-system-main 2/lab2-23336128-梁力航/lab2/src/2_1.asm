    [bits 16]
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov sp, 0x7c00
    mov ax, 0xb800
    mov gs, ax

    ; 设置光标到(8,8)
    mov ah, 0x02
    mov bh, 0
    mov dh, 8
    mov dl, 8
    int 0x10

    ; 获取光标位置
    mov ah, 0x03 ; 功能号：获取光标位置
    mov bh, 0 ; 页码（第0页）
    int 0x10 ; 返回: DH=行, DL=列

    ; 将行列号转换为ASCII字符并显示
    mov al, 'R'
    mov ah, 0x01 ; 蓝色背景
    mov [gs:2*80*12+2*12], ax ; 显示"R:"

    mov al, 'C'
    mov [gs:2*80*12+2*15], ax ; 显示"C:"

    ; 显示行号（DH的值）
    mov al, dh
    add al, '0' ; 数字转ASCII
    mov ah, 0x02 ; 绿色背景
    mov [gs:2*80*12+2*13], ax ; 显示行号在"R:"后面

    ; 显示列号（DL的值）
    mov al, dl
    add al, '0' ; 数字转ASCII
    mov ah, 0x04 ; 红色背景
    mov [gs:2*80*12+2*16], ax ; 显示列号在"C:"后面

    jmp $

    times 510 - ($ - $$) db 0
    db 0x55, 0xaa
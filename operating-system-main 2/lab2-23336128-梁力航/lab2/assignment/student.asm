%include "head.include"      ; 包含头文件，初始化系统环境


; ==================== IF 分支逻辑 ====================
your_if:
    push    ebx             ; 保存通用寄存器ebx的值到栈，eax 是32位寄存器，ax 是低16位，al 是低8位
    mov     eax, [a1]       ; 将内存变量a1的值加载到eax寄存器
    cmp     eax, 12         ; 比较eax与数值12
    jl      .less_than_12   ; 若eax<12跳转到较小分支处理
    cmp     eax, 24         ; 再次比较eax与数值24
    jl      .less_than_24   ; 若12<=eax<24跳转到中等分支处理
    
    ; 处理 eax >=24 的情况
    shl     eax, 4          ; 左移4位(相当于eax*16)
    mov     [if_flag], eax  ; 结果存入内存变量if_flag
    jmp     .end_if         ; 跳转到函数结尾

.less_than_12:              ; 处理 eax<12 的情况
    mov     ebx, eax        ; 备份eax值到ebx
    shr     ebx, 1          ; 右移1位(相当于ebx/2)
    inc     ebx             ; 结果加1 (ebx = eax//2 + 1)
    mov     [if_flag], ebx  ; 结果存入内存变量if_flag
    jmp     .end_if         ; 跳转到函数结尾

.less_than_24:              ; 处理 12<=eax<24 的情况
    mov     ebx, 24         ; 加载数值24到ebx
    sub     ebx, eax        ; 计算24-eax
    imul    ebx, eax        ; 乘法运算 ebx = (24-eax)*eax
    mov     [if_flag], ebx  ; 结果存入内存变量if_flag
    jmp      .end_if

.end_if:
    pop     ebx             ; 恢复ebx寄存器原始值
                        ; 函数返回


; ==================== WHILE 循环逻辑 ====================
your_while:
    push    ecx             ; 保存通用寄存器ecx的值到栈，ecx传统上用作循环计数器，有时用于存储临时数据
    mov     ecx, [a2]       ; 将内存变量a2的值加载到ecx寄存器
    jmp     .loop

.loop:                      ; 循环主体开始
    cmp     ecx, 12         ; 比较ecx与数值12
    jl      .end_while      ; 若ecx<12直接结束循环 jump if less

    call    my_random       ; 调用外部随机函数，结果保存在al寄存器
    mov     edx,  ecx       
    sub     edx, 12         ; 计算偏移量 edx=ecx-12（数组索引）
    mov     ebx, [while_flag]  ; 加载char数组基地址到ebx
    add     ebx, edx        ; 计算数组元素地址（char类型按字节寻址）
    mov     [ebx], al       ; 将随机数存入数组对应位置（使用al寄存器存储字节）这里会不会存在错误，随机数可能是eax整个

    dec     ecx             ; 循环计数器减1
   jmp      .loop

.end_while:
    mov [a2],ecx
    pop     ecx             ; 恢复ecx寄存器原始值
    jmp end                     ; 函数返回


; ==================== 字符串打印函数 ====================
your_function:
    xor     esi, esi        ; 使用异或操作清零esi寄存器，用作字符索引计数器
    mov     ebx, [your_string]  ; 加载字符串基地址到ebx寄存器
    jmp   .loop_char

.loop_char:                 ; 字符遍历循环
    mov     al, [ebx + esi] ; 读取当前字符到al寄存器
    test    al, al          ; 检测字符是否为终止符'\0'
    jz      .end_function   ; 若遇到终止符则结束循环

    ; 保护现场后调用C函数（防止寄存器被修改）
    pushad                  ; 保存所有通用寄存器到栈
    push    eax             ; 将字符参数压栈（print_a_char函数约定）
    call    print_a_char    ; 调用外部字符打印函数
    pop     eax
    popad                   ; 恢复所有通用寄存器

    inc     esi             ; 递增字符索引
    jmp     .loop_char      ; 继续循环处理下一个字符

.end_function:
    jmp end                     ; 函数返回


end: 

%include "end.include"      ; 包含尾文件，处理系统环境清理
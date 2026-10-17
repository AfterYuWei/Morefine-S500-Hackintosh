.text
.globl _replacement
_replacement:
    pushq %rbx
    movq %rdi, %rbx
    movq 0x12d8(%rbx), %rax
    cmpw $0x15e7, 0x0c(%rax)
    pushq $1
    popq %rcx
    je 1f
    xorl %ecx, %ecx
    testb $4, 0x30ec(%rbx)
    je 1f
    movl 0xa0(%rax), %ecx
    shrl $1, %ecx
    andl $1, %ecx
1:
    movl 0x1398(%rbx), %eax
    btrl $0, %eax
    orl %ecx, %eax
    movl %eax, 0x1398(%rbx)
    movq 0x0d68(%rbx), %rcx
    leaq 12(%rcx), %rdx
    cmpq 0x0d60(%rbx), %rdx
    jb 2f
    movq %rbx, %rdi
    pushq $2
    popq %rsi
    .byte 0xe8
    .long (0x5f5ae - 0x819e) - (. - _replacement) - 4
    movq 0x0d68(%rbx), %rcx
    movl 0x1398(%rbx), %eax
2:
    movabsq $0x00000202c0016900, %rdx
    movq %rdx, (%rcx)
    movl %eax, 8(%rcx)
    addq $12, %rcx
    movq %rcx, 0x30f8(%rbx)
    movq %rcx, 0x0d68(%rbx)
    popq %rbx
    retq

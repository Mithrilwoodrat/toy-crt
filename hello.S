.data
hello:
    .string "Hello World!\n"

.text
.globl _start
_start:
    movq $1, %rax
    movq $1, %rdi
    movq $hello, %rsi
    movq $13, %rdx
    syscall

    movq $60, %rax
    movq $0, %rdi
    syscall

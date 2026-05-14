.section .text
.global _start

_start:

addi x1, x0, 7 # x1 = 7
addi x2, x0, 8

add x3, x1, x2

addi x17, x0, 1
addi x11, x3, 0
ecall

addi x17, x0, 93
addi x10, x0, 0
ecall

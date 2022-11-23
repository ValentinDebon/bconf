.section ".note.GNU-stack","",@progbits
.text

.global _start
.type _start,@function
_start:
	mov $1, %rax
	mov $1, %rdi
	lea _start.hello(%rip), %rsi
	mov $14, %rdx
	syscall
	mov $60, %rax
	mov $0, %rdi
	syscall

.data
_start.hello:
	.asciz "Hello, world!\n"

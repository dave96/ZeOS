# 1 "handlers.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "handlers.S"
# 1 "include/asm.h" 1
# 2 "handlers.S" 2
# 1 "include/segment.h" 1
# 3 "handlers.S" 2
# 41 "handlers.S"
.section .text

.globl keyboard_handler; .type keyboard_handler, @function; .align 0; keyboard_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 movb $0x20, %al; outb %al, $0x20;
 call keyboard_routine
 popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
 iret

.globl clock_handler; .type clock_handler, @function; .align 0; clock_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 movb $0x20, %al; outb %al, $0x20;
 call clock_routine
 popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
 iret

.globl system_call_handler; .type system_call_handler, @function; .align 0; system_call_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 cmpl $0, %eax
 jl error
 cmp $MAX_SYSCALL, %eax
 jg error
 call *sys_call_table(, %eax, 0x04)
 jmp fin
error:
 movl $-38, %eax
fin:
 movl %eax, 24(%esp)
 popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
 iret

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
 pushl %ebp
 movl %esp, %ebp
 pushl %esi
 pushl %edi
 pushl %ebx
 movl 8(%ebp), %esi
 pushl %esi
 call inner_task_switch
 addl $4, %esp
 popl %ebx
 popl %edi
 popl %esi
 movl %ebp, %esp
 popl %ebp
 ret

.globl ret_from_fork; .type ret_from_fork, @function; .align 0; ret_from_fork:
 movl $0, %eax
 ret
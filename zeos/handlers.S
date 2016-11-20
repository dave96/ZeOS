#include <asm.h>
#include <segment.h>

#define	ENOSYS		38

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %edx; \
      pushl %ecx; \
      pushl %ebx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es

#define RESTORE_ALL \
      popl %ebx; \
      popl %ecx; \
      popl %edx; \
      popl %esi; \
      popl %edi; \
      popl %ebp; \
      popl %eax; \
      popl %ds; \
      popl %es; \
      popl %fs; \
      popl %gs; \

#ifndef EOI
#define EOI \
		movb $0x20, %al; \
		outb %al, $0x20;
#endif

.section .text

ENTRY(keyboard_handler)
	SAVE_ALL
	EOI
	call	stats_enter_kernel
	call 	keyboard_routine
	call	stats_exit_kernel
	RESTORE_ALL
	iret
	
ENTRY(clock_handler)
	SAVE_ALL
	EOI
	call	stats_enter_kernel
	call	clock_routine
	call	stats_exit_kernel
	RESTORE_ALL
	iret
	
ENTRY(system_call_handler)
	SAVE_ALL
	pushl	%eax
	call 	stats_enter_kernel
	popl	%eax
	cmpl	$0, %eax
	jl		error
	cmp		$MAX_SYSCALL, %eax
	jg		error
	call	*sys_call_table(, %eax, 0x04)
	jmp		fin
error:
	movl $-ENOSYS,	%eax
fin:
	movl %eax, 24(%esp)
	call 	stats_exit_kernel
	RESTORE_ALL
	iret

ENTRY(task_switch)
	pushl	%ebp
	movl 	%esp, %ebp
	pushl	%esi
	pushl	%edi
	pushl	%ebx
	movl 	8(%ebp), %esi
	pushl	%esi
	call	inner_task_switch
	addl	$4, %esp
	popl	%ebx
	popl	%edi
	popl	%esi
	movl	%ebp, %esp
	popl 	%ebp
	ret

ENTRY(ret_from_fork)
	movl $0, %eax
	ret

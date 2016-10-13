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
	call 	keyboard_routine
	RESTORE_ALL
	iret
	
ENTRY(clock_handler)
	SAVE_ALL
	EOI
	call	clock_routine
	RESTORE_ALL
	iret
	
ENTRY(system_call_handler)
	SAVE_ALL
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
	RESTORE_ALL
	iret

ENTRY(task_switch)
	pushl	%esi
	pushl	%edi
	pushl	%ebx
	call	inner_task_switch
	popl	%ebx
	popl	%edi
	popl	%esi
	ret

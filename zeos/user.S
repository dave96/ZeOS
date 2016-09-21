add:
	pushl 	%ebp
	movl	%esp, %ebp
	movl	$0, %eax
	addl	8(%ebp), %eax
	addl	12(%ebp), %eax
	movl	%ebp, %esp
	popl	%ebp
	ret

.text
.globl main
main:
	pushl %ebp
	movl %esp, %ebp
	
	movl $0xffff2222, %eax
	movl $0xaaaaaaaa, %ecx
	
	subl %eax,%ecx
	
	push %ecx
	movl %ebp, %esp
	popl %ebp
	
	ret

	pushl %ebp
	movl %esp, %ebp

	movl $0xabcdef,-4(%ebp);
	movl $0xabcdef,-8(%ebp);
	movl $0xabcdef,-12(%ebp);
	movl $0xabcdef,-16(%ebp);
	movl $0xabcdef,-20(%ebp);
	subl $20,%esp;
	movl $0xab,%eax
	
	movl %ebp, %esp
	popl %ebp
	
	ret



	movl -4(%ebp),%eax;
	movl -8(%ebp),%eax;
	movl -12(%ebp),%eax;
	movl -16(%ebp),%eax;
	movl -20(%ebp),%eax;

	movl $0xabcdef,%eax;

	addl $0xabcdef,%eax;

	movl %eax,-4(%ebp);
	movl %eax,-8(%ebp);
	movl %eax,-12(%ebp);
	movl %eax,-16(%ebp);
	movl %eax,-20(%ebp);

	subl $0xabcdef,%eax;
	imull $0xabcdef,%eax;




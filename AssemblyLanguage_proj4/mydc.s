### --------------------------------------------------------------------
### mydc.s
### Name -Reda 
### --------------------------------------------------------------------

	.equ   ARRAYSIZE, 20
	.equ   EOF, -1
	.equ   RAND_MAX, 1024
	.equ   EOFFSET, 4
	
.section ".rodata"

scanfFormat:
	.asciz "%s"
not_enogh_operands_error:
	.asciz "dc: stack empty\n"
divide_by_zero_error:
	.asciz "dc: division by zero\n"
modulo_of_zero_error:
	.asciz "dc: error of modulo by zero\n"
the_resulting_integer:
	.asciz "%d\n"

### --------------------------------------------------------------------

        .section ".data"
 #index_pointer:
 			#.skip 4

### --------------------------------------------------------------------

        .section ".bss"
buffer:
        .skip  ARRAYSIZE

### --------------------------------------------------------------------

	.section ".text"

	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

	.globl  main
	.type   main,@function

main:

	pushl   %ebp
	movl    %esp, %ebp

input:

	## dc number stack initialized. %esp = %ebp
	
	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call    scanf
	addl    $8, %esp

	## check if user input EOF
	cmp	$EOF, %eax        
	je	quit_operator

	##CHECK IF THE USER INPUT IS
	cmpb $'+' , buffer
		je  addition_operator
	cmpb $'-' , buffer
		je subtraction_operator
	cmpb $'*' , buffer
		je multiplication_operator
	cmpb $'/' , buffer
		je division_operator
	cmpb $'%' , buffer
		je modulo_operator
	cmpb $'^' , buffer
		je power_operator
	cmpb $'p' , buffer
		je top_operator
	cmpb $'q' , buffer
		je quit_operator
	cmpb $'_' , buffer
	 	je negative
	cmpb $'f' ,buffer
		je print_operator
	cmpb $'c' , buffer
		je clear_stack
	cmpb $'d' , buffer
		je duplicate_operator
	cmpb $'r' , buffer
		je swap_operator
	cmpb $'x' , buffer
		je random_generator
	cmpb $'y' , buffer
		je largest_prime
	## if buffer is non of the above

    movl    $buffer, %eax
    movsbl  (%eax), %ebx
    pushl   %ebx
    call    isdigit
    addl    $4, %esp
    cmpl    $0, %eax
    	je      input
    #else  if digit
    pushl $buffer
    call atoi
    addl $4, %esp
    pushl %eax
    jmp input
	
	
##performs addition and pushes 
##the resulting number to the stack.
##if there is no enough operands
##it prints empty stack
addition_operator:
	cmpl %esp, %ebp
	 je not_enogh_operands_error_message
	movl %esp, %eax
	addl $4, %eax
	cmpl %eax, %ebp
	 je not_enogh_operands_error_message
	movl (%eax), %ebx
	addl (%esp), %ebx
	addl  $8, %esp
	pushl %ebx
	jmp input 
##performs subtraction and pushes 
##the resulting number to the stack.
##if there is no enough operands
##it prints empty stack	
subtraction_operator:
	cmpl %esp, %ebp
	je not_enogh_operands_error_message
	movl %esp, %eax
	addl $4 , %eax
	cmpl %eax, %ebp
	je not_enogh_operands_error_message
	movl (%eax) , %ebx
	subl (%esp) , %ebx
	addl $8, %esp
	pushl %ebx
	jmp input
##performs multiplication and pushes 
##the resulting number to the stack.
##if there is no enough operands
##it prints empty stack	
multiplication_operator:
	cmpl %esp, %ebp
	je not_enogh_operands_error_message
	movl %esp, %ebx
	addl $4, %ebx
	cmpl %ebx, %ebp
	je not_enogh_operands_error_message
	movl (%esp), %eax
	imull (%ebx)
	addl $8, %esp
	pushl %eax
	jmp input

##performs subtraction and pushes 
##the resulting number to the stack.
##if there is no enough operands
##it prints empty stack	
division_operator:
	cmpl %esp, %ebp
	je not_enogh_operands_error_message
	movl %esp, %ebx
	addl $4, %ebx
	cmpl %ebx, %ebp
	je not_enogh_operands_error_message
	cmpl $0,(%esp)
	je divide_by_zero_error_message
	movl (%ebx), %eax
	cltd
	idivl (%esp)
	addl $8, %esp
	pushl %eax
	jmp input
##divides the two numbers in the stack 
##and push the remainder to stack if 
##they exist or prints emptystack
modulo_operator:
	cmpl %esp, %ebp
	je not_enogh_operands_error_message
	movl %esp, %ebx
	addl $4, %ebx
	cmpl %ebx, %ebp
	je not_enogh_operands_error_message
	cmpl $0, (%esp)
	je modulo_of_zero_error_message
	movl (%ebx), %eax
	cltd
	idivl (%esp)
	addl $8, %esp
	pushl %edx
	jmp input
	 
##to perform power operation
##which calculates base to the 
##power of exponenet





##prints empty stack if there is no
##an operand else it prints the operand 
##at the top of the stack
top_operator:
	cmpl %esp, %ebp
	je not_enogh_operands_error_message
	movl (%esp), %ebx
	pushl %ebx
	pushl $the_resulting_integer
	call printf
	addl $8, %esp
	jmp input
##to quit the main and 
##return zero
quit_operator:
	movl $0, %eax
	movl %ebp, %esp
	popl %ebp
	ret
negative:
	movl $buffer, %ebx
	addl $1, %ebx
	##movl %ebx, %ecx
	pushl %ebx
	call atoi
	addl $EOFFSET, %esp

	imull $-1, %eax
	pushl %eax
	jmp input





	
##it iterates through the elements of 
##the stack and prints them all. if there 
##is no enough operand, it jumps to input
print_operator:
	cmpl %esp, %ebp
	je input
	movl $0, %ebx

	jmp loop
	loop:
		movl (%esp,%ebx), %eax
		pushl %eax
		pushl $the_resulting_integer
		call printf
		addl $8, %esp

		addl $4, %ebx

		movl %esp,%edx
		addl %ebx,%edx
		cmpl %edx, %ebp
		je loop_end
		#movl %esp, %ebx
		jmp loop
	loop_end:
		jmp input
## it clears
clear_stack:
	loop_for_stack:
		cmpl %esp, %ebp
		je loop_end_for_stack
		popl (%esp)    
		jmp loop_for_stack
	loop_end_for_stack:
		jmp input
## it duplicates the operand at the
##top of the stack and pushes it.
##if there is no enough operand, it 
##prints empty stack.
duplicate_operator:
	cmpl %esp, %ebp
	je not_enogh_operands_error_message          
	movl (%esp), %ebx
	pushl %ebx
	jmp input
##it swaps the top two operands from 
##the stack. if there is no enough operands
##it prints empty stack
swap_operator:
	cmpl %esp, %ebx
	je not_enogh_operands_error_message
	movl %esp, %eax
	addl $4, %eax
	cmpl %eax, %ebp
	je not_enogh_operands_error_message
	movl (%eax), %ebx
	movl (%esp), %ecx
	addl $8, %esp
	pushl %ecx
	pushl %ebx
	jmp input
## it generates a random number less than 1024
random_generator:
	call rand 
	movl $RAND_MAX, %ebx
	cltd
	idivl %ebx
	pushl %eax
	jmp input
##Finds the biggest one of the prime numbers 
##that are lesser or equal to the top-most 
##entry in the stack, and pushes it in the 
##stack if it exists.
largest_prime:
	cmpl %esp, %ebp
	je input
	movl (%esp), %ecx
	loop_to_initialize:
		movl $2, %ebx
		jmp loop_for_prime1
	loop_for_prime1:
		cmpl $2, %ecx
		jl input
		jmp loop_for_prime2
	loop_for_prime2:
		cmpl %ebx, %ecx
		je finish_loop
		movl %ecx, %eax
		cltd
		idivl %ebx
		cmpl $0, %edx
		je next_element
		incl %ebx
		jmp loop_for_prime2
	next_element:
		decl %ecx
		jmp loop_to_initialize
	finish_loop:
		pushl %ecx
		jmp input
 

##it prints the string empty stack
##when there is no enogh operand
##found in the stack, while trying 
##to do +,-,*,/,%,p,d,r
not_enogh_operands_error_message:
	pushl $not_enogh_operands_error
	call printf
	addl $4, %esp
	jmp input
## it prints error by zero division 
## if we try to divide by zero.
divide_by_zero_error_message:
	pushl $divide_by_zero_error
	call printf
	addl $4, %esp
	jmp input
## it prints error modulo by zero 
##if we try to modulo by zero
modulo_of_zero_error_message:
	pushl $modulo_of_zero_error
	call printf
	addl $4, %esp
	jmp input

power_operator:
	cmpl %esp,%ebp
	je not_enogh_operands_error_message

	popl %ebx

	cmpl %esp, %ebp
	je not_enogh_operands_error_message

	popl %eax

	pushl %eax
	pushl %ebx

	call power
	addl $8,%esp
	pushl %eax
	jmp input





	
	



   
    ## Formal parameter offsets:
    .equ    IBASE,  12
    .equ    IEXP,   8
    ## Local variable offsets:
    .equ    IPOWER, -4
    .equ    IINDEX, -8
    .type power,@function

power:
    pushl   %ebp
    movl    %esp, %ebp
    ## int iPower = 1; int iIndex;
    pushl   $1           
    subl    $4, %esp
    movl    $1, IINDEX(%ebp) ## iIndex = 1
loop1:
    ## if (iIndex > iExp) goto loopend1
    movl    IINDEX(%ebp), %eax
    cmpl    IEXP(%ebp), %eax
    jg      loopend1
    ## iPower *= iBase
    movl    IPOWER(%ebp), %eax
    imull   IBASE(%ebp), %eax
    movl    %eax, IPOWER(%ebp)
    #iIndex++
    incl    IINDEX(%ebp)
    ## goto loop1
    jmp     loop1
loopend1:
	
	movl IPOWER(%ebp),%eax
	movl %ebp,%esp
	popl %ebp
	ret








# CSIS 3740 — Project 4 (No Loops Edition)
# Your first MIPS assembly programs in MARS!
# Complete any 3 of the 4 small tasks below.
# programmed by: Ethan Woycehoski

.data
msg_hello:    .asciiz "Hello, MIPS World!"
msg_sum:      .asciiz "The sum is: "
msg_a:        .asciiz "\nEnter first number: "
msg_b:        .asciiz "Enter second number: "
msg_bigger:   .asciiz "The larger number is: "
msg_pos:      .asciiz "Positive\n"
msg_neg:      .asciiz "Negative\n"
msg_zero:     .asciiz "Zero\n"

.text
.globl main
main:
    # Uncomment one line at a time to test a task
    jal task1_hello
    jal task2_sum_two
    jal task3_compare_two
    jal task4_sign_check

    li $v0, 10      # Exit
    syscall

#############################
# Task 1 — Hello World
#############################
task1_hello:
    # TODO: Print "Hello, MIPS World!"
    # loads hello message string address into $a0 
    la $a0, msg_hello
    
    # syscall code 4, prints string
    li $v0, 4
    syscall 
    
    jr $ra

#############################
# Task 2 — Sum of Two Numbers
#############################
task2_sum_two:
    # TODO: Read two ints and print their sum.
    # Hints: syscall 5 to read, syscall 1 to print int, syscall 4 for text.
    # loads enter first number string into $a0
    la $a0, msg_a
    
    # syscall code 4, prints string
    li $v0, 4
    syscall
    
    # syscall code 5 reads input
    li $v0, 5
    syscall
    
    # moves the input into a temporary register to be used later
    move $t0, $v0
    
    # loads enter second number string into $a0
    la $a0, msg_b
    
    # syscall code 4 prints string
    li $v0, 4
    syscall
    
    # syscall code 5 reads input
    li $v0, 5
    syscall
    
    # moves the input into temporary register 
    move $t1, $v0
    
    # adds the two inputs together and stores the sum into $t2
    add $t2, $t0, $t1
    
    # loads sum string into $a0
    la $a0, msg_sum
    
    # syscall code 4 prints string
    li $v0, 4
    syscall
    
    # moves the sum into $a0
    move $a0, $t2
    
    # syscall code 1 prints integer
    li $v0, 1
    syscall
    
    jr $ra

#############################
# Task 3 — Compare Two Numbers
#############################
task3_compare_two:
    # TODO: Read two ints and print the larger value.
    # Use bgt / blt and move instructions.
    # loads enter first number string into $a0
    la $a0, msg_a
    
    # syscall code 4, prints string
    li $v0, 4
    syscall
    
    # syscall code 5, reads input
    li $v0, 5
    syscall
    
    # moves the input into temporary register 3
    move $t3, $v0
    
    # loads enter second number into register $a0
    la $a0, msg_b
    
    # syscall code 4, prints string
    li $v0, 4
    syscall
    
    # syscall code 5, reads input
    li $v0, 5
    syscall
    
    # moves the input into temporary register 4
    move $t4, $v0
    
    # if $t3 is greater than $t4, jumps to t3_is_bigger label
    bgt $t3, $t4, t3_is_bigger
    
    # loads message bigger string
    la $a0, msg_bigger
    
    # syscall code 4 prints string
    li $v0, 4
    syscall
    
    # moves the temporary register 4 integer into $a0
    move $a0, $t4
    
    # syscall code 1 prints integer
    li $v0, 1
    syscall
    
    jr $ra
    
    # label t3_is_bigger
    t3_is_bigger:
    
    	# loads message bigger into $a0
    	la $a0, msg_bigger
    
    	# syscall code 4, prints string
    	li $v0, 4
    	syscall
    
    	# moves the temporary register 3 integer into $a0
    	move $a0, $t3
    
    	# syscall code 1 prints integer
    	li $v0, 1
    	syscall
    	
    	jr $ra

#############################
# Task 4 — Sign Check (Optional)
#############################
task4_sign_check:
    # TODO: Read one int. Print Positive, Negative, or Zero.
    # Use bgtz, bltz, and beqz.
    
    # loads message enter first number into $a0
    la $a0, msg_a
    
    # syscall code 4, print string
    li $v0, 4
    syscall
    
    # syscall code 5, read integer input
    li $v0, 5
    syscall
    
    # move contents of $v0 to temporary register 5
    move $t5, $v0
    
    # branch if $t5 is equal to zero
    beqz $t5, t5_is_zero
    
    # branch if $t5 is greater than zero
    bgtz $t5, t5_positive
    
    # branch if $t5 is less than zero
    bltz $t5, t5_negative
    
    # t5_is_zero label marking start of branch if zero
    t5_is_zero: 
    	# load zero message into $a0
    	la $a0, msg_zero
    	
    	# syscall code 4, print string
    	li $v0, 4
    	syscall
    	jr $ra
    
    # t5_positive label marking start of branch if positive
    t5_positive:
    	# load positive message into $a0
    	la $a0, msg_pos
    	
    	# syscall code 4, print string
    	li $v0, 4
    	syscall
    	jr $ra
    
    # t5_negative label marking start of branch if negative
    t5_negative:
    	# load negative message into $a0
    	la $a0, msg_neg
    	
    	# syscall code 4, print string
    	li $v0, 4
    	syscall
    	jr $ra
    
    jr $ra

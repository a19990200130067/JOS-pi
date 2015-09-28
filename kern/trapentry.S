/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
	
#define PUSHFRAME							   \
	sub	sp, sp, #4	;	/* Align the stack */		   \
	str	lr, [sp, #-4]!	;	/* Push the return address */	   \
	sub	sp, sp, #(4*17)	;	/* Adjust the stack pointer */	   \
	stmia	sp, {r0-r12}	;	/* Push the user mode registers */ \
	add	r0, sp, #(4*13)	;	/* Adjust the stack pointer */	   \
	stmia	r0, {r13-r14}^	;	/* Push the user mode registers */ \
	mov	r0, r0		;	/* NOP for previous instruction */ \
	mrs	r0, spsr	;	/* Put the SPSR on the stack */	   \
	str	r0, [sp, #-4]!	;

#define PULLFRAME							   \
	ldr	r0, [sp], #4	;	/* Get the SPSR from stack */	   \
	msr	spsr_fsxc, r0	;					   \
	clrex			;					   \
	ldmia   sp, {r0-r14}^	;	/* Restore registers (usr mode) */ \
	mov	r0, r0		;	/* NOP for previous instruction */ \
	add	sp, sp, #(4*17)	;	/* Adjust the stack pointer */	   \
	ldr	lr, [sp], #4	;	/* Pull the return address */	   \
	add	sp, sp, #4	;	/* Align the stack */

	#stmfd	sp!, {r0-r12, lr};				\
	#ldr	sp, =stack_start;/* first set stack to KSTACKTOP */     \
	
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
name:			        /* function starts here */		\
	PUSHFRAME               					\
	movs	r0, sp		;					\
	ldr	r1, [pc]	;					\
	ldr	pc, _alltraps	;					\
	.word num;

#define VECENTRY(label, name)                                           \
label:  .word name
	
.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */

	.p2align PGSHIFT
	.globl int_vector
int_vector:
	ldr	pc, _reset_h
	ldr	pc, _undefined_instruction_vector_h
	ldr	pc, _software_interrupt_vector_h
	ldr	pc, _prefetch_abort_vector_h
	ldr	pc, _data_abort_vector_h
	ldr	pc, _unused_handler_h
	ldr	pc, _interrupt_vector_h
	ldr	pc, _fast_interrupt_vector_h

	VECENTRY(_reset_h, reset_handler)
	VECENTRY(_undefined_instruction_vector_h, undefined_instruction_handler)
	VECENTRY(_software_interrupt_vector_h, sw_i_handler)
	VECENTRY(_prefetch_abort_vector_h, prefetch_abort_handler)
	VECENTRY(_data_abort_vector_h, data_abort_handler)
	VECENTRY(_unused_handler_h, unused_handler_handler)
	VECENTRY(_interrupt_vector_h, interrupt_handler)
	VECENTRY(_fast_interrupt_vector_h, fiq_handler)	
	
	VECENTRY(_alltraps, alltraps)

	TRAPHANDLER(reset_handler, 0)
	TRAPHANDLER(undefined_instruction_handler, 1)
	TRAPHANDLER(sw_i_handler, 2)
	TRAPHANDLER(prefetch_abort_handler, 3)
	TRAPHANDLER(data_abort_handler, 4)
	TRAPHANDLER(unused_handler_handler, 5)
	TRAPHANDLER(interrupt_handler, 6)
	TRAPHANDLER(fiq_handler, 7)

	.globl led_on_addr
led_on_addr:
	.word 0x3f200020
	
	.globl led_off_addr
led_off_addr:
	.word 0x3f20002c
	
	.globl led_set_val	
led_set_val:
	.word 0x8000

	.globl stack_start
stack_start:
	.word KSTACKTOP
	
	

/*
 * Lab 3: Your code here for alltraps
 */

alltraps:
	bl	trap_handler
	#bl	trap_handler
	#ldr	r0, led_set_val 
	#ldr	r1, led_off_addr 
	#str	r0, [r1]
	#ldmfd  	sp!, {r0-r12, sp, pc}^
	PULLFRAME
	movs	pc, lr


/*
.data
.global vectors
vectors:

.data
.global irq_vectors
irq_vectors:
*/

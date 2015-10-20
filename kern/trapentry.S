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
	stmia	r0, {sp, lr}^	;	/* Push the user mode registers */ \
	mov	r0, r0		;	/* NOP for previous instruction */ \
	mrs	r0, spsr	;	/* Put the SPSR on the stack */	   \
	str	r0, [sp, #-4]!	;

#define PUSHFRAMEINSVC							   \
	stmdb	sp, {r0-r3}		  ;		/* Save 4 registers */		   \
	mov	r0, lr			  ;		/* Save xxx32 r14 */		   \
	mov	r1, sp			  ;		/* Save xxx32 sp */		   \
	mrs	r3, spsr		  ;		/* Save xxx32 spsr */		   \
	mrs	r2, cpsr		  ;		/* Get the CPSR */		   \
	bic	r2, r2, #(PSR_MODE) 	  ;		/* Fix for SVC mode */		   \
	orr	r2, r2, #(PSR_SVC32_MODE) ;						   \
	msr	cpsr_c, r2		  ;		/* Punch into SVC mode */	   \
	mov	r2, sp			  ;		/* Save	SVC sp */		   \
	bic	sp, sp, #7		  ;		/* Align sp to an 8-byte addrress */  \
	sub	sp, sp, #(4 * 17)	  ;	/* Pad trapframe to keep alignment */ \
	/* and for dtrace to emulate push/pop */  \
	str	r0, [sp, #-4]!	;	/* Push return address */	   \
	str	lr, [sp, #-4]!	;	/* Push SVC lr */		   \
	str	r2, [sp, #-4]!	;	/* Push SVC sp */		   \
	msr	spsr_fsxc, r3	;	/* Restore correct spsr */	   \
	ldmdb	r1, {r0-r3}	;	/* Restore 4 regs from xxx mode */ \
	sub	sp, sp, #(4*15)	;	/* Adjust the stack pointer */	   \
	stmia	sp, {r0-r12}	;	/* Push the user mode registers */ \
	add	r0, sp, #(4*13)	;	/* Adjust the stack pointer */	   \
	stmia	r0, {r13-r14}^	;	/* Push the user mode registers */ \
	mov	r0, r0		;	/* NOP for previous instruction */ \
	mrs	r0, spsr	;	/* Put the SPSR on the stack */	   \
	str	r0, [sp, #-4]!	;

#define PULLFRAME								   \
	ldr	r0, [sp], #4		;	/* Get the SPSR from stack */	   \
	msr	spsr_fsxc, r0		;					   \
	clrex				;					   \
	ldmia   sp, {r0-r12, sp, lr}^	;	/* Restore registers (usr mode) */ \
	mov	r0, r0			;	/* NOP for previous instruction */ \
	add	sp, sp, #(4*17)		;	/* Adjust the stack pointer */	   \
	ldr	lr, [sp], #4		;	/* Pull the return address */	   \
	add	sp, sp, #4		;	/* Align the stack */

#define PULLFRAMEFROMSVCANDEXIT						   \
	ldr	r0, [sp], #4	;	/* Get the SPSR from stack */	   \
	msr	spsr_fsxc, r0	;	/* restore SPSR */		   \
	clrex			;					   \
	ldmia	sp, {r0-r14}^	;	/* Restore registers (usr mode) */ \
	mov	r0, r0		;	/* NOP for previous instruction */ \
	add	sp, sp, #(4*15)	;	/* Adjust the stack pointer */	   \
	ldmia	sp, {sp, lr, pc}^	/* Restore lr and exit */
	
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
name:			        /* function starts here */		\
	PUSHFRAME               					\
	movs	r0, sp		;					\
	ldr	r1, (name##n)	;					\
	ldr	pc, _alltraps	;					\
name##n:	                                                        \
	.word num;

#define TRAPHANDLERINSVC(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
name:			        /* function starts here */		\
	PUSHFRAMEINSVC               					\
	movs	r0, sp		;					\
	ldr	r1, name##nn	;					\
	ldr	pc, name	;					\
name##nn:	                                                        \
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

	TRAPHANDLERINSVC(reset_handler, 0)
	TRAPHANDLERINSVC(undefined_instruction_handler, 1)
	TRAPHANDLER(sw_i_handler, 2)
	TRAPHANDLERINSVC(prefetch_abort_handler, 3)
	TRAPHANDLERINSVC(data_abort_handler, 4)
	TRAPHANDLERINSVC(unused_handler_handler, 5)
	TRAPHANDLERINSVC(interrupt_handler, 6)
	TRAPHANDLERINSVC(fiq_handler, 7)

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
	bl	trap
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

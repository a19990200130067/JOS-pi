#include <inc/mmu.h>
#include <inc/memlayout.h>

.text

	.globl _start
	.globl entry
	.globl _get_stack_pointer

_start:
entry:
	// First enable the VFP
	// r1 = Access Control Register
	mrc p15, 0, r1, c1, c0, 2
	// enable full access for p10,11
	orr r1, r1, #(0xf << 20)
	// ccess Control Register = r1
	mcr p15, 0, r1, c1, c0, 2
	mov r1, #0
	// flush prefetch buffer because of FMXR below
	mcr p15, 0, r1, c7, c5, 4
	// and CP 10 & 11 were only just enabled
	// Enable VFP itself
	mov r0,#0x40000000
	// FPEXC = r0
	fmxr FPEXC, r0
	
	// Turn on paging with the entry_pgdir
	ldr	r2, =entry_pgdir
	mov	r3, #0
	// r4 == entry_pgdir - KERNBASE to calculate the physical addr of entry_pgdir
	mov	r4, #0x0FFFFFFF
	and	r2, r2, r4
	mcr	p15, 0, r3, c2, c0, 2
	mcr	p15, 0, r2, c2, c0, 0  //write TTBR0
	mcr	p15, 0, r2, c2, c0, 1  //write TTBR1
	mrc	p15, 0, r1, c1, c0, 0
	orr	r4, r1, #C1_MMU
	mcr	p15, 0, r4, c1, c0, 0
	mcr	p15, 0, r3, c8, c7, 0  //invalidate all TLB
	ldr 	r0, =_relocated	
	bx	r0
	
_relocated:	
	// Run the c startup function - should not return and will call kernel_main
	mov	fp, #0
	//mcr	p15, 0, r1, c1, c0, 0
	ldr	sp, =bootstacktop
	bl	arm_init	

_inf_loop:
	b       _inf_loop


_get_stack_pointer:
	// Return the stack pointer value
	str     sp, [sp]
	ldr     r0, [sp]

	// Return from the function
	mov     pc, lr


.data	
	.p2align PGSHIFT
	.globl bootstack
bootstack:
	.space KSTKSIZE
	.globl bootstacktop
bootstacktop:	

#ifndef JOS_INC_TRAP_H
#define JOS_INC_TRAP_H

#define INSN_SIZE       4
#define INSN_COND_MASK  0xf0000000      /* Condition mask */
#define PSR_MODE        0x0000001f      /* mode mask */
#define PSR_USR32_MODE  0x00000010
#define PSR_FIQ32_MODE  0x00000011
#define PSR_IRQ32_MODE  0x00000012
#define PSR_SVC32_MODE  0x00000013
#define PSR_MON32_MODE  0x00000016
#define PSR_ABT32_MODE  0x00000017
#define PSR_HYP32_MODE  0x0000001a
#define PSR_UND32_MODE  0x0000001b
#define PSR_SYS32_MODE  0x0000001f
#define PSR_32_MODE     0x00000010
#define PSR_T           0x00000020      /* Instruction set bit */
#define PSR_F           0x00000040      /* FIQ disable bit */
#define PSR_I           0x00000080      /* IRQ disable bit */
#define PSR_A           0x00000100      /* Imprecise abort bit */
#define PSR_E           0x00000200      /* Data endianess bit */
#define PSR_GE          0x000f0000      /* Greater than or equal to bits */
#define PSR_J           0x01000000      /* Java bit */
#define PSR_Q           0x08000000      /* Sticky overflow bit */
#define PSR_V           0x10000000      /* Overflow bit */
#define PSR_C           0x20000000      /* Carry bit */
#define PSR_Z           0x40000000      /* Zero bit */
#define PSR_N           0x80000000      /* Negative bit */
#define PSR_FLAGS       0xf0000000      /* Flags mask. */

// Trap numbers
// These are processor defined:
#define T_RESET         0x00		// reset
#define T_UDEF_INSTR    0x04            // undefined instruction
#define T_SWI           0x08            // software interrrupt
#define T_PREFETCH      0x0C            // prefetch abort
#define T_DATA          0x10            // data abort

#define T_IRQ           0x18            // IRQ
#define T_FIQ           0x1C            // fast interrupt

#define IRQ_OFFSET	32	// IRQ 0 corresponds to int IRQ_OFFSET

// Hardware IRQ numbers. We receive these as (IRQ_OFFSET+IRQ_WHATEVER)
#define IRQ_TIMER        0
#define IRQ_KBD          1
#define IRQ_SERIAL       4
#define IRQ_SPURIOUS     7
#define IRQ_IDE         14
#define IRQ_ERROR       19

#ifndef __ASSEMBLER__

#include <inc/types.h>

struct PushRegs {
    /* registers as pushed by stmia */
    uint32_t reg_r0;
    uint32_t reg_r1;
    uint32_t reg_r2;
    uint32_t reg_r3;
    uint32_t reg_r4;
    uint32_t reg_r5;
    uint32_t reg_r6;
    uint32_t reg_r7;
    uint32_t reg_r8;
    uint32_t reg_r9;
    uint32_t reg_sl;
    uint32_t reg_fp;
    uint32_t reg_ip;
} __attribute__((packed));

struct Trapframe {
    uint32_t spsr;
    struct PushRegs tf_regs;
    uint32_t tf_sp_user;
    uint32_t tf_lr_user;
    uint32_t tf_sp_svc;
    uint32_t tf_lr_svc;
    uint32_t tf_ret_addr;
    uint32_t tf_padding;
} __attribute__((packed));

struct UTrapframe {
    /* information about the fault */
    uint32_t utf_fault_va;	/* va for T_PGFLT, 0 otherwise */
    uint32_t utf_err;
    /* trap-time return state */
    struct PushRegs utf_regs;
    uintptr_t utf_eip;
    uint32_t utf_eflags;
    /* the trap-time stack to return to */
    uintptr_t utf_esp;
} __attribute__((packed));

#endif /* !__ASSEMBLER__ */

#endif /* !JOS_INC_TRAP_H */

#ifndef JOS_INC_TRAP_H
#define JOS_INC_TRAP_H

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

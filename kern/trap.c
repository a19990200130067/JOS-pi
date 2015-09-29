#include <inc/mmu.h>
#include <inc/arm.h>
#include <inc/assert.h>
#include <inc/string.h>

#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/console.h>
#include <kern/monitor.h>
//#include <kern/env.h>

static struct Trapframe *last_tf;

void
trap_init(void)
{
    extern char int_vector[];
    cprintf("int_vec addr: 0x%x\n", (uint32_t)int_vector);
    page_insert(kern_pgdir, pa2page(PADDR(int_vector)), 0, PTE_P);
}

void
trap_init_percpu(void)
{
}

void
print_regs(struct PushRegs *regs)
{
    cprintf("\tr0\t0x%08x\n", regs->reg_r0);
    cprintf("\tr1\t0x%08x\n", regs->reg_r1);
    cprintf("\tr2\t0x%08x\n", regs->reg_r2);
    cprintf("\tr3\t0x%08x\n", regs->reg_r3);
    cprintf("\tr4\t0x%08x\n", regs->reg_r4);
    cprintf("\tr5\t0x%08x\n", regs->reg_r5);
    cprintf("\tr6\t0x%08x\n", regs->reg_r6);
    cprintf("\tr7\t0x%08x\n", regs->reg_r7);
    cprintf("\tr8\t0x%08x\n", regs->reg_r8);
    cprintf("\tr9\t0x%08x\n", regs->reg_r9);
    cprintf("\tsl\t0x%08x\n", regs->reg_sl);
    cprintf("\tfp\t0x%08x\n", regs->reg_fp);
    cprintf("\tip\t0x%08x\n", regs->reg_ip);
}

void
print_trapframe(struct Trapframe *tf)
{
    cprintf("TRAP frame at %p from CPU %d\n", tf, 0);
    cprintf("\tspsr\t0x%08x\n", tf->spsr);    
    print_regs(&tf->tf_regs);
    cprintf("\tsp\t0x%08x\n", tf->tf_sp_user);
    cprintf("\tlr\t0x%08x\n", tf->tf_lr_user);
    cprintf("\tsp\t0x%08x\n", tf->tf_sp_svc);
    cprintf("\tlr\t0x%08x\n", tf->tf_lr_svc);
    cprintf("\tret_addr\t0x%08x\n", tf->tf_ret_addr);
    cprintf("end of TRAP frame\n");
}

void 
trap(struct Trapframe *tf, uint32_t trap_num)
{
    uint32_t lr;
    asm volatile("mov %[val], lr" : [val] "=r" (lr):);
    cprintf("in trap!!! cpsr is: 0x%x, spsr is 0x%x, r0 is 0x%x\n", rcpsr(), rspsr(), trap_num);
    cprintf("in trap2!!! cpsr is: 0x%x, lr is 0x%x\n", rcpsr(), lr);
    print_trapframe(tf);
    LED_OFF();
    spinsleep(1000000);
    LED_ON();
    tf->tf_sp_svc = 0;
    tf->tf_lr_svc = 0;
    //asm volatile("swi #10":::);
    cprintf("why swi not working?\n");
}

void
page_fault_handler(struct Trapframe * tf)
{
}

void
backtrace(struct Trapframe *tf)
{
}

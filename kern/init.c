#include <inc/rpi-gpio.h>
#include <kern/console.h>
#include <inc/stdio.h>

#include <inc/arm.h>
#include <inc/trap.h>
#include <kern/pmap.h>


void spinsleep(int ctr) {
    volatile int tim = 0;
    for(tim = 0; tim < ctr; tim++);
}

void print_trapframe(struct Trapframe *tf) {
    int i = 0;
    cprintf("start printing trapframe: \n");
    for (i = 0; i < (sizeof(struct Trapframe) / sizeof(uint32_t)) + 5; i++) {
	cprintf("reg: 0x%x\n", ((uint32_t *)tf)[i]);
    }
    cprintf("end printing tf\n");
}

void trap_handler(struct Trapframe *tf, uint32_t r0) {
    uint32_t lr;
    asm volatile("mov %[val], lr" : [val] "=r" (lr):);
    cprintf("in trap!!! cpsr is: 0x%x, spsr is 0x%x, r0 is 0x%x\n", rcpsr(), rspsr(), r0);
    cprintf("in trap2!!! cpsr is: 0x%x, lr is 0x%x\n", rcpsr(), lr);
    print_trapframe(tf);
    LED_OFF();
    spinsleep(1000000);
    LED_ON();
    //asm volatile("swi #10":::);
    cprintf("why swi not working?\n");
}

int arm_init() {
    extern char edata[], end[];

    // Before doing anything else, complete the ELF loading process.
    // Clear the uninitialized global data (BSS) section of our program.
    // This ensures that all static/global variables start out zero.
    memset(edata, 0, end - edata);
    
    // Only after cons_init can we invoke the cprintf function call
    cons_init();
    cprintf("Welcome to the JOS kernel\n");
    cprintf("6828 decimal is %o octal!\n", 6828);

    mem_init();
    cprintf("memory init complete!\n");

    extern char int_vector[];
    cprintf("int_vec addr: 0x%x\n", (uint32_t)int_vector);
    page_insert(kern_pgdir, pa2page(PADDR(int_vector)), 0, PTE_P);
    RPI_GetGpio()->LED_GPFSEL |= (1 << LED_GPFBIT);
    //LED_OFF();
    //*((int *)0x3f200020) = 0x8000;
    int cpsr = rcpsr();
    cprintf("cpsr is: 0x%x, spsr is: 0x%x\n", cpsr, rspsr());
    //cpsr = 0x60000003;
    //lcpsr(cpsr);
    
    cprintf("cpsr is: 0x%x\n", rcpsr());
    int pc_before = read_pc();
    asm volatile("swi #10":::);
    cprintf("pc is: 0x%x\n", pc_before);

    cprintf("swi over, and again. cpsr: 0x%x, spsr: 0x%x\n", rcpsr(), rspsr());
    //asm volatile("swi #20":::);
    
    /* Assign the address of the GPIO peripheral (Using ARM Physical Address) */
    volatile unsigned int *gpio = (unsigned int*)GPIO_BASE;

    /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
    //RPI_GetGpio()->LED_GPFSEL |= (1 << LED_GPFBIT);
    cprintf("led on addr: 0x%x, val: 0x%x\n", &(RPI_GetGpio()->LED_GPCLR), (1 << LED_GPIO_BIT));
    cprintf("led off addr: 0x%x, val: 0x%x\n", &(RPI_GetGpio()->LED_GPSET), (1 << LED_GPIO_BIT));
    while(1) {
	spinsleep(100000);
	/* Set the LED GPIO pin low ( Turn OK LED on for original Pi, and off
	   for plus models )*/
	//gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);
	//LED_ON();

	spinsleep(100000);

	/* Set the LED GPIO pin high ( Turn OK LED off for original Pi, and on
	   for plus models )*/
	//gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
	//LED_OFF();
    }
}

/*
 * Variable panicstr contains argument to first call to panic; used as flag
 * to indicate that the kernel has already called panic.
 */
const char *panicstr;

/*
 * Panic is called on unresolvable fatal errors.
 * It prints "panic: mesg", and then enters the kernel monitor.
 */
void
_panic(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	if (panicstr)
		goto dead;
	panicstr = fmt;

	// Be extra sure that the machine is in as reasonable state
	//__asm __volatile("cli; cld");

	va_start(ap, fmt);
	cprintf("kernel panic on at %s:%d: ", file, line);
	vcprintf(fmt, ap);
	cprintf("\n");
	va_end(ap);

dead:
	/* break into the kernel monitor */
	while (1)
		monitor(NULL);
}

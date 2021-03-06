/* See COPYRIGHT for copyright information. */

#include <inc/memlayout.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/kbdreg.h>
#include <inc/rpi-base.h>
#include <inc/rpi-gpio.h>
#include <inc/rpi-aux.h>

#include <kern/console.h>

static void cons_intr(int (*proc)(void));
static void cons_putc(int c);

// Stupid I/O delay routine necessitated by historical PC design flaws
static void
delay(void)
{
}

static bool serial_exists;

static int
serial_proc_data(void)
{
}

void
serial_intr(void)
{
    if (serial_exists)
	cons_intr(serial_proc_data);
}

static void
serial_putc(int c)
{
    if ((char)c != '\n')
	RPI_AuxMiniUartWrite((char)c);
    else {
	RPI_AuxMiniUartWrite('\r');
	RPI_AuxMiniUartWrite('\n');
    }
}

static void
serial_init(void)
{
    RPI_AuxMiniUartInit(115200, 8);
}



/***** Parallel port output code *****/
// For information on PC parallel port programming, see the class References
// page.

static void
lpt_putc(int c)
{
}




/***** Text-mode CGA/VGA display output *****/

static unsigned addr_6845;
static uint16_t *crt_buf;
static uint16_t crt_pos;

static void
cga_init(void)
{
}



static void
cga_putc(int c)
{
}


/***** Keyboard input code *****/

#define NO		0

#define SHIFT		(1<<0)
#define CTL		(1<<1)
#define ALT		(1<<2)

#define CAPSLOCK	(1<<3)
#define NUMLOCK		(1<<4)
#define SCROLLLOCK	(1<<5)

#define E0ESC		(1<<6)

static uint8_t shiftcode[256] =
{
    [0x1D] = CTL,
    [0x2A] = SHIFT,
    [0x36] = SHIFT,
    [0x38] = ALT,
    [0x9D] = CTL,
    [0xB8] = ALT
};

static uint8_t togglecode[256] =
{
    [0x3A] = CAPSLOCK,
    [0x45] = NUMLOCK,
    [0x46] = SCROLLLOCK
};

static uint8_t normalmap[256] =
{
    NO,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',	// 0x00
    '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',	// 0x10
    'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',	// 0x20
    '\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',  '.',  '/',  NO,   '*',	// 0x30
    NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',	// 0x40
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,	// 0x50
    [0xC7] = KEY_HOME,	      [0x9C] = '\n' /*KP_Enter*/,
    [0xB5] = '/' /*KP_Div*/,      [0xC8] = KEY_UP,
    [0xC9] = KEY_PGUP,	      [0xCB] = KEY_LF,
    [0xCD] = KEY_RT,	      [0xCF] = KEY_END,
    [0xD0] = KEY_DN,	      [0xD1] = KEY_PGDN,
    [0xD2] = KEY_INS,	      [0xD3] = KEY_DEL
};

static uint8_t shiftmap[256] =
{
    NO,   033,  '!',  '@',  '#',  '$',  '%',  '^',	// 0x00
    '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
    'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',	// 0x10
    'O',  'P',  '{',  '}',  '\n', NO,   'A',  'S',
    'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',	// 0x20
    '"',  '~',  NO,   '|',  'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  '<',  '>',  '?',  NO,   '*',	// 0x30
    NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',	// 0x40
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,	// 0x50
    [0xC7] = KEY_HOME,	      [0x9C] = '\n' /*KP_Enter*/,
    [0xB5] = '/' /*KP_Div*/,      [0xC8] = KEY_UP,
    [0xC9] = KEY_PGUP,	      [0xCB] = KEY_LF,
    [0xCD] = KEY_RT,	      [0xCF] = KEY_END,
    [0xD0] = KEY_DN,	      [0xD1] = KEY_PGDN,
    [0xD2] = KEY_INS,	      [0xD3] = KEY_DEL
};

#define C(x) (x - '@')

static uint8_t ctlmap[256] =
{
    NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
    NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
    C('Q'),  C('W'),  C('E'),  C('R'),  C('T'),  C('Y'),  C('U'),  C('I'),
    C('O'),  C('P'),  NO,      NO,      '\r',    NO,      C('A'),  C('S'),
    C('D'),  C('F'),  C('G'),  C('H'),  C('J'),  C('K'),  C('L'),  NO,
    NO,      NO,      NO,      C('\\'), C('Z'),  C('X'),  C('C'),  C('V'),
    C('B'),  C('N'),  C('M'),  NO,      NO,      C('/'),  NO,      NO,
    [0x97] = KEY_HOME,
    [0xB5] = C('/'),		[0xC8] = KEY_UP,
    [0xC9] = KEY_PGUP,		[0xCB] = KEY_LF,
    [0xCD] = KEY_RT,		[0xCF] = KEY_END,
    [0xD0] = KEY_DN,		[0xD1] = KEY_PGDN,
    [0xD2] = KEY_INS,		[0xD3] = KEY_DEL
};

static uint8_t *charcode[4] = {
    normalmap,
    shiftmap,
    ctlmap,
    ctlmap
};

/*
 * Get data from the keyboard.  If we finish a character, return it.  Else 0.
 * Return -1 if no data.
 */
static int
kbd_proc_data(void)
{
    return 0;
}

void
kbd_intr(void)
{
    //cons_intr(kbd_proc_data);
}

static void
kbd_init(void)
{
    // Drain the kbd buffer so that QEMU generates interrupts.
    //kbd_intr();
    //irq_setmask_8259A(irq_mask_8259A & ~(1<<1));
}



/***** General device-independent console code *****/
// Here we manage the console input buffer,
// where we stash characters received from the keyboard or serial port
// whenever the corresponding interrupt occurs.

#define CONSBUFSIZE 512

static struct {
    uint8_t buf[CONSBUFSIZE];
    uint32_t rpos;
    uint32_t wpos;
} cons;

// called by device interrupt routines to feed input characters
// into the circular console input buffer.
static void
cons_intr(int (*proc)(void))
{
    int c;
    
    while ((c = (*proc)()) != -1) {
	if (c == 0)
	    continue;
	cons.buf[cons.wpos++] = c;
	if (cons.wpos == CONSBUFSIZE)
	    cons.wpos = 0;
    }
}

// return the next input character from the console, or 0 if none waiting
int
cons_getc(void)
{
    /*
    int c;

    // poll for any pending input characters,
    // so that this function works even when interrupts are disabled
    // (e.g., when called from the kernel monitor).
    serial_intr();
    kbd_intr();

    // grab the next character from the input buffer.
    if (cons.rpos != cons.wpos) {
	c = cons.buf[cons.rpos++];
	if (cons.rpos == CONSBUFSIZE)
	    cons.rpos = 0;
	return c;
    }
    */
    return 0;
}

// output a character to the console
static void
cons_putc(int c)
{
    serial_putc(c);
    //lpt_putc(c);
    //cga_putc(c);
}

// initialize the console devices
void
cons_init(void)
{
    //cga_init();
    //kbd_init();
    serial_init();
    /*
    if (!serial_exists)
	cprintf("Serial port does not exist!\n");
    */
}


// `High'-level console I/O.  Used by readline and cprintf.

void
cputchar(int c)
{
    cons_putc(c);
}

int
getchar(void)
{
    int c;

    while ((c = cons_getc()) == 0)
	/* do nothing */;
    return c;
}

int
iscons(int fdnum)
{
    // used by readline
    return 1;
}

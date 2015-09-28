/* The base address of the GPIO peripheral (ARM Physical Address) */
#include <inc/rpi-gpio.h>

volatile unsigned int* gpio;

/** Simple loop variable */
volatile unsigned int tim;
volatile unsigned int counter;
volatile int sp;

int timeUnit = 500000;

void spinsleep(int ctr) {
    for(tim = 0; tim < ctr; tim++)
	;
}

void ledBlink(int time) {
    gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);
    spinsleep(time);
    gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
    spinsleep(time);
}

void ledOutput() {
    spinsleep(timeUnit);
    gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
    spinsleep(timeUnit);
    gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);
    spinsleep(timeUnit);
    gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
    spinsleep(timeUnit);
    gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);
    spinsleep(timeUnit);
    for (tim = 0; tim < 32; tim++) {
	if (sp % 2 != 0) {
	    ledBlink(250000);
	    ledBlink(250000);
	} else {
	    ledBlink(500000);
	}
	sp = sp / 2;
    }
}

int readSp() {
    asm volatile("mov %[result], sp" : [result] "=r" (sp)::);
    return sp;
}

/** Main function - we'll never return from here */
int main(void) __attribute__((naked));
int main(void) {
    /* Assign the address of the GPIO peripheral (Using ARM Physical Address) */
    gpio = (unsigned int*)GPIO_BASE;

    /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
    gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);

    /* Never exit as there is no OS to exit to! */
    
    sp = readSp();
    spinsleep(timeUnit);
    int i = 0;
    for (i = 0; i < 32; i++) {
	if (sp % 2 != 0) {
	    ledBlink(250000);
	    ledBlink(250000);
	} else {
	    ledBlink(500000);
	}
	sp = sp / 2;
    }
    
    while(1) {
	spinsleep(1000000);
	/* Set the LED GPIO pin low ( Turn OK LED on for original Pi, and off
	   for plus models )*/
	gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);

	spinsleep(1000000);

	/* Set the LED GPIO pin high ( Turn OK LED off for original Pi, and on
	   for plus models )*/
	gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
    }
}

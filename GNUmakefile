
TOP = .
OBJDIR := obj

CC := arm-none-eabi-gcc
LD := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump
# for raspberry pi 2
CFLAGS := -nostdinc -O2 -I$(TOP) -DRPI2 -mfpu=vfp -mfloat-abi=hard -march=armv7-a -mtune=cortex-a7

KERN_CFLAGS := $(CFLAGS) -DJOS_KERNEL

all: kernel7.img

-include kern/Makefrag

clean:
	rm -rf $(OBJDIR) *.img *.elf


# Run 'make V=1' to turn on verbose commands, or 'make V=0' to turn them off.
ifeq ($(V),1)
override V =
endif
ifeq ($(V),0)
override V = @
endif

TOP = .
OBJDIR := obj
OBJDIRS :=

TOOL_CHAIN_PREFIX := arm-none-eabi-

CC := $(TOOL_CHAIN_PREFIX)gcc
LD := $(TOOL_CHAIN_PREFIX)ld
AS := $(TOOL_CHAIN_PREFIX)as
AR := $(TOOL_CHAIN_PREFIX)ar
OBJCOPY := $(TOOL_CHAIN_PREFIX)objcopy
OBJDUMP := $(TOOL_CHAIN_PREFIX)objdump
NM := $(TOOL_CHAIN_PREFIX)nm
# for raspberry pi 2
CFLAGS := -nostdinc -O2 -fno-builtin -I$(TOP) -DRPI2 -mfpu=vfp -mfloat-abi=hard -march=armv7-a -mtune=cortex-a7

KERN_CFLAGS := $(CFLAGS) -DJOS_KERNEL
USER_CFLAGS := $(CFLAGS) -DJOS_USER -gstabs
ULDFLAGS := -T user/user.ld

all: kernel7.img

include kern/Makefrag
include user/Makefrag
include lib/Makefrag

clean:
	rm -rf $(OBJDIR) *.img *.elf

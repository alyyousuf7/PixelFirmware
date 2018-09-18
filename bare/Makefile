# If the GCC ARM tools are already on the path, use them. Otherwise, use
# the local version in gcc-arm/bin
GCC_ARM_VERSION := $(shell arm-none-eabi-gcc --version 2>/dev/null)
ifdef GCC_ARM_VERSION
	GCCDIR=
else
	GCCDIR=gcc-arm/bin/
endif

CC = $(GCCDIR)arm-none-eabi-gcc
AR = $(GCCDIR)arm-none-eabi-ar
OBJCOPY = $(GCCDIR)arm-none-eabi-objcopy
OBJDUMP = $(GCCDIR)arm-none-eabi-objdump

DEBUG_OPTS = -g3 -gdwarf-2 -gstrict-dwarf
OPTS = -Os
TARGET = -mcpu=cortex-m0
CFLAGS = -ffunction-sections -fdata-sections -Wall -Wa,-adhlns="$@.lst" \
		-fmessage-length=0 $(TARGET) -mthumb -mfloat-abi=soft \
		$(DEBUG_OPTS) $(OPTS) -I .

LIBOBJS = _startup.o syscalls.o uart.o spi.o delay.o ring.o

.PHONY:	clean

# -----------------------------------------------------------------------------

all: libbare.a

libbare.a: $(LIBOBJS)
	$(AR) -rv libbare.a $(LIBOBJS)

clean:
	rm -f *.o *.lst *.map *.out libbare.a

%.o: %.c
	$(CC) $(CFLAGS) -c $<

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

BARE_DIR = bare
BARE_PATH = $(BARE_DIR)/libbare.a
PROGRAM = PixelDisplay

DEBUG_OPTS = -g3 -gdwarf-2 -gstrict-dwarf
OPTS = -Os
TARGET = -mcpu=cortex-m0
CFLAGS = -ffunction-sections -fdata-sections -Wall -Wa,-adhlns="$@.lst" \
		-fmessage-length=0 $(TARGET) -mthumb -mfloat-abi=soft \
		$(DEBUG_OPTS) $(OPTS) -I . -I $(BARE_DIR)

OBJECTS += main.o
OBJECTS += display.o
OBJECTS += gamma.o

.PHONY: clean deploy

# -----------------------------------------------------------------------------

all: $(PROGRAM).srec $(PROGRAM).bin $(PROGRAM).dump

clean:
	rm -f *.o *.lst *.map *.out *.srec *.bin *.dump
	make -C $(BARE_DIR) clean

$(BARE_PATH):
	make -C $(BARE_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(PROGRAM).dump: $(PROGRAM).out
	$(OBJDUMP) --disassemble $< >$@

$(PROGRAM).srec: $(PROGRAM).out
	$(OBJCOPY) -O srec $< $@

$(PROGRAM).bin: $(PROGRAM).out
	$(OBJCOPY) -O binary $< $@

$(PROGRAM).out: $(OBJECTS) mkl25z4.ld $(BARE_PATH)
	$(CC) $(CFLAGS) -T $(filter %.ld, $^) -Wl,-Map="$(PROGRAM).map" -o $@ $(filter %.o, $^) $(BARE_PATH)

# -----------------------------------------------------------------------------
# Burn/deploy by copying to the development board filesystem
#  Hack:  we identify the board by the volume name "MBED"
DEPLOY_VOLUME = $(shell df -h 2>/dev/null | fgrep "MBED" | awk '{print $$6}')
deploy: $(PROGRAM).bin
	dd conv=fsync bs=64k if=$< of=$(DEPLOY_VOLUME)/$<

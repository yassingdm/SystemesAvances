#===============================================================
# FOLDERS
#===============================================================
CWD        = $(shell pwd)
# Base folder for include paths
INCLUDEDIR = $(dir $(CWD))
# All library sources are currently there
LIBDIR     = $(INCLUDEDIR)/librpi3

#####################################################
# SOURCES
#####################################################

ASMFILES = \
	$(wildcard $(LIBDIR)/*.S)

CFILES = \
	$(wildcard $(LIBDIR)/*.c) \
	main.c

HEADERS = \
	$(wildcard $(LIBDIR)/*.h)

LDCFG = $(CWD)/app-specific.ld

LDSCRIPT = $(LIBDIR)/rpi64.ld

#####################################################
# OUTPUT
#####################################################

# Mandatory output
IMGFILE = kernel8.img

# Optional outputs
LIST = kernel.list
MAP = kernel.map


#####################################################
# COMPILER AND LINKER
#####################################################

ARMGNU = aarch64-none-elf

CFLAGS = \
	-I $(INCLUDEDIR) \
	-march=armv8-a+simd -mtune=cortex-a53 \
	-Wall -O3 -mstrict-align -fno-tree-loop-vectorize \
	-fno-tree-slp-vectorize \
	-nostdlib -nodefaultlibs -nostartfiles -ffreestanding \
	-fno-asynchronous-unwind-tables -fomit-frame-pointer \

LDFLAGS=-Wl,-gc-sections -Wl,--build-id=none -Wl,-Bdynamic
LIBS = 

#####################################################
# COMPILER AND LINKER
#####################################################

CC = $(ARMGNU)-gcc $(CFLAGS)
AS = $(ARMGNU)-gcc $(CFLAGS)

OBJECTS = $(CFILES:.c=.o) $(ASMFILES:.S=.o)


# Rule to remake everything. Does not include clean.
all: kernel.elf
	$(ARMGNU)-objcopy kernel.elf -O binary $(IMGFILE)
	$(ARMGNU)-objdump -d kernel.elf > $(LIST)
	$(ARMGNU)-nm -n kernel.elf > $(MAP)

# Rule to make the elf file.
kernel.elf : $(OBJECTS) $(LDSCRIPT) $(LDCFG)
	$(ARMGNU)-gcc $(CFLAGS) $(LDFLAGS) \
		-Wl,-T,$(LDSCRIPT) $(OBJECTS) -o $@ $(LIBS)

depend :
	makedepend -I../ $(CFILES)

clean:
	$(RM) kernel8.img
	$(RM) $(MAP)
	$(RM) kernel.elf
	$(RM) $(LIST)
	$(RM) $(OBJECTS)

# DO NOT DELETE THIS LINE -- make depend depends on it.

/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/arm-config.o: ../librpi3/arm.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/arm-config.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/arm-config.o: ../librpi3/arm-config.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/arm-config.o: ../librpi3/required.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/arm-config.o: ../librpi3/uart-mini.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/arm-config.o: ../librpi3/stdio.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/arm-config.o: ../librpi3/bcm2837.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/ascii-print.o: ../librpi3/stdio.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/ascii-print.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/ascii-print.o: ../librpi3/ascii-print.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/crc16.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/crc16.o: ../librpi3/crc16.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/gpio.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/gpio.o: ../librpi3/assert.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/gpio.o: ../librpi3/bcm2837.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/gpio.o: ../librpi3/gpio.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mailbox.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mailbox.o: ../librpi3/assert.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mailbox.o: ../librpi3/bcm2837.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mailbox.o: ../librpi3/mailbox.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mmu-pagetable.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mmu-pagetable.o: ../librpi3/mmu-private.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mmu-pagetable.o: ../librpi3/assert.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mmu-pagetable.o: ../librpi3/arm.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mmu.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mmu.o: ../librpi3/mmu.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mmu.o: ../librpi3/assert.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mmu.o: ../librpi3/mmu-private.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/mmu.o: ../librpi3/arm.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/speed.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/speed.o: ../librpi3/mailbox.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/speed.o: ../librpi3/speed.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/stdio.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/stdio.o: ../librpi3/string.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/stdio.o: ../librpi3/stdio.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/string.o: ../librpi3/string.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/svc.o: ../librpi3/svc.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/svc.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/svc.o: ../librpi3/arm.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/svc.o: ../librpi3/vector-handlers.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/svc.o: ../librpi3/semaphore.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/svc.o: ../librpi3/uart-mini.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/uart-mini.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/uart-mini.o: ../librpi3/assert.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/uart-mini.o: ../librpi3/arm.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/uart-mini.o: ../librpi3/bcm2837.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/uart-mini.o: ../librpi3/gpio.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/uart-mini.o: ../librpi3/mailbox.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/uart-mini.o: ../librpi3/uart-mini.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/vector-handlers.o: ../librpi3/arm.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/vector-handlers.o: /usr/include/stdint.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/vector-handlers.o: ../librpi3/uart-mini.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/vector-handlers.o: ../librpi3/stdio.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/vector-handlers.o: ../librpi3/vector-handlers.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/vector-handlers.o: ../librpi3/svc.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/vector-handlers.o: ../librpi3/semaphore.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/vector-handlers.o: ../librpi3/bcm2837.h
/home/yassi/SysAv/TP5/demo-tp5/aarch64-cortexA53-rpi3-runtime//librpi3/vector-handlers.o: ../librpi3/arm-config.h
main.o: ../librpi3/semaphore.h /usr/include/stdint.h ../librpi3/uart-mini.h
main.o: ../librpi3/mmu.h ../librpi3/assert.h ../librpi3/mmu-private.h
main.o: ../librpi3/arm.h ../librpi3/arm-config.h ../librpi3/stdio.h
main.o: ../librpi3/required.h ../librpi3/vector-table.h ../librpi3/svc.h
main.o: ../librpi3/vector-handlers.h ../librpi3/bcm2837.h ../librpi3/boot.h

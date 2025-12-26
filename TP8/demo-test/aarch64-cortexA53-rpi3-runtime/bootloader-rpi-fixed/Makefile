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
	loader.c \
	main.c

HEADERS = \
	$(wildcard $(LIBDIR)/*.h) \
	loader.h

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

/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/arm-config.o: ../librpi3/arm.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/arm-config.o: ../librpi3/arm-config.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/arm-config.o: ../librpi3/required.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/arm-config.o: ../librpi3/uart-mini.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/arm-config.o: ../librpi3/stdio.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/arm-config.o: ../librpi3/bcm2837.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/ascii-print.o: ../librpi3/stdio.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/ascii-print.o: ../librpi3/ascii-print.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/crc16.o: ../librpi3/crc16.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/gpio.o: ../librpi3/assert.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/gpio.o: ../librpi3/bcm2837.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/gpio.o: ../librpi3/gpio.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/mailbox.o: ../librpi3/assert.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/mailbox.o: ../librpi3/bcm2837.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/mailbox.o: ../librpi3/mailbox.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/mmu-pagetable.o: ../librpi3/mmu-private.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/mmu-pagetable.o: ../librpi3/assert.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/mmu-pagetable.o: ../librpi3/arm.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/mmu.o: ../librpi3/mmu.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/mmu.o: ../librpi3/assert.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/mmu.o: ../librpi3/mmu-private.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/mmu.o: ../librpi3/arm.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/speed.o: ../librpi3/mailbox.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/speed.o: ../librpi3/speed.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/stdio.o: ../librpi3/string.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/stdio.o: ../librpi3/stdio.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/string.o: ../librpi3/string.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/svc.o: ../librpi3/svc.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/svc.o: ../librpi3/arm.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/svc.o: ../librpi3/vector-handlers.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/svc.o: ../librpi3/semaphore.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/svc.o: ../librpi3/uart-mini.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/uart-mini.o: ../librpi3/assert.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/uart-mini.o: ../librpi3/arm.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/uart-mini.o: ../librpi3/bcm2837.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/uart-mini.o: ../librpi3/gpio.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/uart-mini.o: ../librpi3/mailbox.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/uart-mini.o: ../librpi3/uart-mini.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/vector-handlers.o: ../librpi3/arm.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/vector-handlers.o: ../librpi3/uart-mini.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/vector-handlers.o: ../librpi3/stdio.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/vector-handlers.o: ../librpi3/vector-handlers.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/vector-handlers.o: ../librpi3/svc.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/vector-handlers.o: ../librpi3/semaphore.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/vector-handlers.o: ../librpi3/bcm2837.h
/Users/dpotop/svn/dpotop/soft/RaspberryPi/RPi3B/demo-safran/RPi3B//librpi3/vector-handlers.o: ../librpi3/arm-config.h
loader.o: ../librpi3/uart-mini.h ../librpi3/string.h ../librpi3/stdio.h
loader.o: ../librpi3/crc16.h magic.h loader.h
main.o: ../librpi3/uart-mini.h ../librpi3/arm.h ../librpi3/arm-config.h
main.o: ../librpi3/stdio.h ../librpi3/speed.h ../librpi3/required.h loader.h

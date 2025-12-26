#===============================================================
# FOLDERS
#===============================================================
CWD        = $(shell pwd)
# Base folder for include paths
INCLUDEDIR = $(dir $(CWD))
# All library sources are currently there
LIBDIR     = $(INCLUDEDIR)/librpi3
# All (generated) thread code is here
GENERATED  = $(INCLUDEDIR)/application/gen-t1042
# Provided nodes are here
NODES      = $(INCLUDEDIR)/application/nodes

#####################################################
# SOURCES
#####################################################

ASMFILES = \
	$(LIBDIR)/bcm2837.S \
	$(LIBDIR)/semaphore.S 

CFILES = \
	$(wildcard $(LIBDIR)/*.c) \
	$(wildcard $(GENERATED)/*.c) \
	$(wildcard $(GENERATED)/threads/*.c) \
	$(wildcard $(GENERATED)/wrappers/*.c) \
	$(NODES)/nodes.c \
	main.c \

HEADERS = \
	$(wildcard $(LIBDIR)/*.h)

# Used to specify entry point function name, load
# address, and some other parameters that should not
# change often, but can change.
LDCFG = $(CWD)/app-specific.ld

# Application-independent ldscript
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
	-I $(shell heptc -where)/c \
	-I $(INCLUDEDIR) -I $(GENERATED) -I $(CWD)\
	-march=armv8-a+simd -mtune=cortex-a53 \
	-Wall -O3 -mstrict-align -fno-tree-loop-vectorize \
	-fno-tree-slp-vectorize \
	-nostdlib -nodefaultlibs -nostartfiles -ffreestanding \
	-fno-asynchronous-unwind-tables -fomit-frame-pointer \

LDFLAGS=-Wl,-gc-sections -Wl,--build-id=none -Wl,-Bdynamic
LIBS = \
#	/Users/dpotop/svn/SENSITIVE/ASSUME-confidential/safran/Livraison_code_compiles/full.a \
#	/Users/dpotop/svn/SENSITIVE/ASSUME-confidential/safran/Livraison_code_compiles/lookup.a \


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
		-Wl,-T,$(LDSCRIPT) $(OBJECTS) -o $@ $(LIBS) $(LIBS)

depend :
	makedepend -I $(INCLUDEDIR) -I $(GENERATED) -I $(CWD) $(CFILES)

clean:
	$(RM) kernel8.img
	$(RM) $(MAP)
	$(RM) kernel.elf
	$(RM) $(LIST)
	$(RM) $(OBJECTS)

# DO NOT DELETE THIS LINE -- make depend depends on it.


########################################
# Toolchain et options
########################################

CROSS_COMPILE ?= aarch64-none-elf-

CC      := $(CROSS_COMPILE)gcc
LD      := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy

CFLAGS  := -Wall -Wextra -O2 -std=c11 \
           -ffreestanding -nostdlib -nostartfiles \
           -mgeneral-regs-only

# Adapté à ton arborescence :
# ce Makefile est dans application/gen-t1042
RUNTIME_DIR := ../../..
LIBRPI3_DIR := $(RUNTIME_DIR)/librpi3

# Includes
CFLAGS  += -I. \
           -I$(RUNTIME_DIR)/application/gen-t1042 \
           -I$(RUNTIME_DIR)/application \
           -I$(LIBRPI3_DIR)

########################################
# Fichiers de l'application
########################################

APP_SRCS := \
    externc.c \
    variables.c \
    scheduler.c \
    scheduler_data.c \
    scheduler_data_types.c \
    scheduler_types.c \
    threads/thread_cpu0.c \
    threads/thread_cpu1.c

APP_OBJS := $(APP_SRCS:.c=.o)

########################################
# Fichiers du runtime (auto)
########################################
# On prend tous les .c et .S de librpi3 sans les nommer un par un.

RUNTIME_C_SRCS  := $(wildcard $(LIBRPI3_DIR)/*.c)
RUNTIME_ASM_SRCS:= $(wildcard $(LIBRPI3_DIR)/*.S)

RUNTIME_OBJS := $(RUNTIME_C_SRCS:.c=.o)
RUNTIME_OBJS += $(RUNTIME_ASM_SRCS:.S=.o)

########################################
# Script de link
########################################
# Adapte ce nom si ton script s’appelle autrement (link.ld, kernel.ld, etc.)
LDSCRIPT := $(LIBRPI3_DIR)/rpi64.ld

########################################
# Cibles principales
########################################

all: kernel8.img

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $< $@

kernel8.elf: $(RUNTIME_OBJS) $(APP_OBJS)
	$(LD) -T $(LDSCRIPT) -o $@ $^

########################################
# Règles de compilation
########################################

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

########################################
# Nettoyage
########################################

clean:
	rm -f $(APP_OBJS) $(RUNTIME_OBJS) kernel8.elf kernel8.img

.PHONY: all clean

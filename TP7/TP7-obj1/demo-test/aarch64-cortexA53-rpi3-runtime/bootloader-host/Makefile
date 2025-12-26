#####################################################
# SOURCES
#####################################################

CFILES = \
	main.c \
	tty.c \

HEADERS = \
	tty.h \

RESULT = boot

#####################################################
# COMPILER AND LINKER
#####################################################

CC = gcc

all: $(RESULT)

$(RESULT): $(CFILES) $(HEADERS) Makefile
	$(CC) -I../ $(CFILES) -lpthread -o $(RESULT)

clean:
	$(RM) $(RESULT)

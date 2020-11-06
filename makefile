# makefile to compile required applicationand clean up all object files
# Author: Pavan Shiralagi
# Date: 3rd September 2020

# The compile used is gcc, compiler for cross-compilation is arm-unknown-linux-gnueabi-gcc

ifeq ($(CC),)
	CC = $(CROSS_COMPILE)gcc
endif
ifeq ($(CFLAGS),)
	# Compilet flags -Wall turns on most compiler warnings
	CFLAGS = -g -Wall -Werror
endif

ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif

ACCC = adxl335/adxl335
ACC = adxl335

all: $(ACC)

$(ACC): $(ACCC).c
	$(CC) $(CFLAGS) $(ACCC).c -o $(ACC) $(LDFLAGS)


clean:
	$(RM) $(ACC) *.o

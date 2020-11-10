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

all: subsystems

subsystems:
	cd adxl335 && $(MAKE)
	cd socket && $(MAKE)


clean:
	(cd adxl335; make clean)
	(cd server; make clean)

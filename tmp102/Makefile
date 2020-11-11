# makefile to compile tmp102.c and clean up object files
# Author: Pavan Shiralagi
# Date: 11th November 2020
ifeq ($(CC),)
	  CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	 CFLAGS = -g -Wall -Werror
endif

all: tmp102
	
tmp102: tmp102.c
	$(CC) $(CFLAGS) $(INCLUDES) tmp102.c -o tmp102 -pthread -lrt
clean:	
	rm -rf *o tmp102

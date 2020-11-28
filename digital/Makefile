# makefile to compile adxl335.c and clean up object files
# Author: Pavan Shiralagi
# Date: 9th November 2020
ifeq ($(CC),)
	  CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	 CFLAGS = -g -Wall -Werror
endif

all: digital
	
digital: digital.c
	$(CC) $(CFLAGS) $(INCLUDES) digital.c -o digital -lwiringPi -pthread -lrt
clean:	
	rm -rf *.o digital

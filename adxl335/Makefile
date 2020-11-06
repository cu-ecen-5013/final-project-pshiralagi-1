# makefile to compile adxl335.c and clean up object files
# Author: Pavan Shiralagi
# Date: 9th November 2020
ifeq ($(CC),)
	  CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	 CFLAGS = -g -Wall -Werror
endif

all: adxl335
	
adxl335: adxl335.c
	$(CC) $(CFLAGS) $(INCLUDES) adxl335.c -o adxl335 -lwiringPi
clean:	
	rm -rf *o adxl335

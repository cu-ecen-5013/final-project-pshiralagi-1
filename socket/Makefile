# makefile to compile adxl335.c and clean up object files
# Author: Pavan Shiralagi
# Date: 9th November 2020
ifeq ($(CC),)
	  CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	 CFLAGS = -g -Wall -Werror
endif

all: server client
	
server: server.c
	$(CC) $(CFLAGS) $(INCLUDES) server.c -o server -pthread -lrt

client: client.c
	$(CC) $(CFLAGS) $(INCLUDES) client.c -o client

clean:	
	rm -rf *o client server

/*
 *  @filename: adxl335.c
 *  @author: Pavan Shiralagi
 *  @description: Reads accelerometer values through analog pin
 *  @reference: http://wiringpi.com/reference/core-functions/
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>
#include <pthread.h>
#include <unistd.h>

#define Z_AXIS 0
#define READ_DELAY 5000

void read_adxl335(void);

int main(int argc, char* argv[])
{
    int i, status;
    for(i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			printf("Daemon\n\r");
			daemon(1, 1);
		}	
	} 
	status=wiringPiSetup();
	
	if(status != -1)
	{
		printf("Connected to accelerometer\n\r");
	}
	else
	{
		exit(1);
	}
    while(1)
    {
	//pinMode (0, OUTPUT) ;
    //digitalWrite (0, HIGH) ; delay (500) ;
    //digitalWrite (0,  LOW) ; delay (500) ;
    read_adxl335();
	delay(1000);
    }
}

void read_adxl335(void)
{
    volatile int z_axis_read;
    pinMode(Z_AXIS, INPUT);
    z_axis_read = analogRead(Z_AXIS);
    printf("Value read from accelerometer is - %d\n\r", z_axis_read);
}

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
#define LED_PIN 1
#define READ_DELAY 5000

void blink_led(void);
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
	pinMode(Z_AXIS, INPUT);
	pinMode (LED_PIN, OUTPUT) ;
    while(1)
    {
		blink_led();
    	read_adxl335();
		delay(1000);
    }
}

void read_adxl335(void)
{
    static int z_axis_read;    
    z_axis_read = digitalRead(Z_AXIS);
    printf("Accelerometer - %d\n\r", z_axis_read);
}

void blink_led(void)
{
    digitalWrite (LED_PIN, HIGH) ; delay (500) ;
    digitalWrite (LED_PIN,  LOW) ; delay (500) ;
}

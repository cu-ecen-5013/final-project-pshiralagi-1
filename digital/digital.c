/*
 *  @filename: digital.c
 *  @author: Pavan Shiralagi
 *  @description: Reads PIR values through digital pin and blinks LED
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

#define PIR 0
#define LED_PIN 1
#define READ_DELAY 5000

bool led_value;

void blink_led(void);
void read_digital(void);

int main(int argc, char* argv[])
{
    int i, status;
	led_value = 1;
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
	pinMode(PIR, INPUT);
	pinMode (LED_PIN, OUTPUT) ;
    while(1)
    {
		blink_led();
    	read_digital();
		delay(1000);
    }
}

void read_digital(void)
{
    static int PIR_read;    
    PIR_read = digitalRead(PIR);
    //printf("Accelerometer - %d\n\r", PIR_read);
}

void blink_led(void)
{
    digitalWrite (LED_PIN, led_value);
	led_value = !led_value;
}

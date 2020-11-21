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
int read_digital(void);

int main(int argc, char* argv[])
{
    int i, status, digital_read, count;
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
		printf("Connected to digital applications\n\r");
	}
	else
	{
		exit(1);
	}
	pinMode(PIR, INPUT);
	pinMode (LED_PIN, OUTPUT) ;
	count = 0;
    while(1)
    {
    	digital_read = read_digital();
		if (digital_read == 1 || count != 0)
 		{
			count++;
			count = count % 100;
			blink_led();
		}
		else
			led_value = 0;
		delay(1000);
    }
}

int read_digital(void)
{
    static int PIR_read;    
    PIR_read = digitalRead(PIR);
    printf("Accelerometer - %d\n\r", PIR_read);
	return (PIR_read);
}

void blink_led(void)
{
    digitalWrite (LED_PIN, led_value);
	led_value = !led_value;
}

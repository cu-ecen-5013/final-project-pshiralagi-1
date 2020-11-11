/*
 *  @filename: tmp102.c
 *  @author: Pavan Shiralagi
 *  @description: Reads temperature value from tmp102 using i2c
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define I2C_ADDR 0x48
#define I2C_BUS "/dev/i2c-1"

int poll_tmp(int fd);

int main(int argc, char* argv[])
{
	int fd, i, tmp;
	if ((fd = open(I2C_BUS, O_RDWR)) < 0)
	{
		perror("open");
		exit(1);
	}

	if (ioctl(fd,I2C_SLAVE, I2C_ADDR) < 0)
	{
		perror("ioctl");
		exit(1);
	}

	write(fd, 0x00, 1);
    for(i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			printf("Daemon - i2c\n\r");
			daemon(1, 1);
		}	
	} 
	
	while(1)
	{
		tmp = poll_tmp(fd);
		printf("Temperature is : %d", tmp);
		sleep(5);
	}
	return 0;
}

int poll_tmp(int fd)
{
	int len, tmp, tmp_c;
	char i2c_read[1];
	if ((len = read(fd, i2c_read, 2) != 2))
		perror("read");
	else
	{
		tmp = (((i2c_read[0]) << 4) | ((i2c_read[1]) >> 4));
		if(tmp > 0x7FF)
		{
			tmp |= 0xF000;
		}
		tmp_c = tmp * 0.0625;
		return(tmp_c);
	}
	return(0);
	
}


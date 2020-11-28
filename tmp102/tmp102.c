/*
 *  @filename: tmp102.c
 *  @author: Pavan Shiralagi
 *  @description: Reads temperature value from tmp102 using i2c
 *	@reference: https://opensource.com/article/19/4/interprocess-communication-linux-storage
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
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define I2C_ADDR 0x48
#define I2C_BUS "/dev/i2c-1"

int poll_tmp(int fd);
void ipc_init(void);
void ipc_tmp(int tmp);

typedef struct {
	int unique_id;
	int tmp;
} data;

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
	printf("Entering while loop\n\r");
	ipc_init();
	while(1)
	{
		tmp = poll_tmp(fd);
		printf("Temperature is : %d\n\r", tmp);
		ipc_tmp(tmp);
		sleep(5);
	}
	return 0;
}

/*	Function to read and format temperature	*/
int poll_tmp(int fd)
{
	printf("Polling temp\n\r");
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
/*	Function to initialize shared memory	*/
void ipc_init(void)
{
	int fd_shared;
	fd_shared = shm_open("shareTmp",  O_CREAT | O_RDWR, 0666);
	if (fd_shared < 0)
	{
		perror("open");
	}
	ftruncate(fd_shared, 500);
	if (close(fd_shared) < 0)
	{
		perror("close");
	}
}

/*	Function to store temperature in shared memory	*/
void ipc_tmp(int tmp)
{
	data temperature = {1, tmp};
	data *temperature_ptr = &temperature;
	data *temp_ptr = NULL;
	int fd_shared;
	fd_shared = shm_open("shareTmp", O_RDWR, 0666);
	if (fd_shared < 0)
	{
		perror("open");
	}
	temp_ptr = (data *)mmap(NULL, sizeof(data), PROT_WRITE, MAP_SHARED, fd_shared, 0);
	if (close(fd_shared) < 0)
	{
		perror("close");
	}
	memcpy((void *)(&temp_ptr[0]),(void*)temperature_ptr,sizeof(data));
	munmap(temp_ptr,sizeof(data));
}

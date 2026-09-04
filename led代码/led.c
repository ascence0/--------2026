#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/ioctl.h"
#include "stdlib.h"
#include "termios.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "sys/time.h"

#define LED_BRIGHTNESS    "/sys/class/leds/led1/brightness"
#define LED1_ON       "echo 0 > /sys/class/leds/led1/brightness"
#define LED2_ON       "echo 0 > /sys/class/leds/led2/brightness"
#define LED1_OFF      "echo 1 > /sys/class/leds/led1/brightness"
#define LED2_OFF       "echo 1 > /sys/class/leds/led2/brightness"
int main()
{
		
	int on=1;
	int led;
	int fd;
	fd=open(LED_BRIGHTNESS, O_WRONLY);
	if(fd<0)
	{
	   perror("open device leds");
		exit(1);
	}
	printf("leds test show,press ctrl+c to exit \n");
	while(1)
	{
			system(LED1_ON);
			system(LED2_OFF);
			usleep(500000);
			system(LED1_OFF);
			system(LED2_ON);
			usleep(500000);
	}
		close(fd);
	return 0;

}


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h> /*PPSIX 终端控制定义*/
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "i2c.h"

char ver[20] = {"ver01.002"};

int i2c_fd;
char dev[20];

struct_i2c_param i2c_param; /*外设参数初始化*/
char write_mode = 0;
char read_mode = 0;

/*
 * @description : 自定义打印函数
 * @param - buff: 打印数据缓冲区
 * @param - lens: 打印数据长度
 * @param - mode: 打印格式
 * @return		: 无
 */
void func_my_print(unsigned char *buff, unsigned int lens, unsigned char mode)
{
    int i = 0;

    switch (mode)
    {
    case 'H': //按照16进制打印数据
        for (i = 0; i < lens; i++)
        {
            printf("0X%02X  ", buff[i]);
        }
        break;
    case 'h': //按照16进制打印数据
        for (i = 0; i < lens; i++)
        {
            printf("0x%02x  ", buff[i]);
        }
        break;
    case 'd': //按照10进制打印数据
        for (i = 0; i < lens; i++)
        {
            printf("%02d  ", buff[i]);
        }
        break;
    case 'c': //按照字符打印数据
        for (i = 0; i < lens; i++)
        {
            printf("%c", buff[i]);
        }
        break;
    default:
        break;
    }
    printf("\n");
}

/*
 * @description  : 打印参数设置格式
 * @param - pname: 函数名
 * @return		 : 无
 */
static void print_usage(const char *pname)
{
    printf("Usage: %s device [-d dev_addr ] [-w w_addr data] [-r r_addr numb] [-v]"
           "\n\t'-d dev_addr' for device addr"
           "\n\t'-w w_addr data' for write mode, write addr and The characters required to write to"
           "\n\t'-r r_addr numb' for read mode, read addr and The required to read numbers"
           "\n\t'-v' show version"
           "\n\texample :  --> ./elf1_cmd_i2c i2c-1 -d 0x29 -w 0x80 0x03"
           "\n\texample :  --> ./elf1_cmd_i2c i2c-1 -d 0x29 -r 0x81 1\n",
           pname);
}

/*
 * @description   : 解析函数带入参数
 * @param - numb  : 参数个数
 * @param - *param: 带入参数数组指针
 * @param - *i2c_param: i2c应用参数
 * @return		  : 无
 */
void get_param(int numb, char *param[], struct_i2c_param *i2cparam)
{
    int i = 0;
    int temp = 0;

    if (numb <= 2)
        return;

    for (i = 2; i < numb; i++)
    {
        if (!strcmp(param[i], "-d"))
        {
            i++;

            temp = strtol(param[i], NULL, 16); 
            if (temp > 0)
            {
                i2cparam->dev_addr = (unsigned char)temp;
            }
            continue;
        }
        if (!strcmp(param[i], "-w"))
        {
            write_mode = 1;

            i++;

            temp = strtol(param[i], NULL, 16); 
            if (temp > 0)
            {
                i2cparam->w_addr = (unsigned char)temp;
            }

            i++;
            memset(i2cparam->w_data, 0, MAX_LEN);
            i2cparam->w_data[0] = (unsigned char)strtol(param[i], NULL, 16); 
	    i2cparam->w_len= 1;
            continue;
        }
        if (!strcmp(param[i], "-r"))
        {
            read_mode = 1;
            memset(i2cparam->r_data, 0, MAX_LEN);

            i++;
            temp= strtol(param[i], NULL, 16); 
            if (temp > 0)
            {
                i2cparam->r_addr = (unsigned char)temp;
            }

            i++;
            temp= strtol(param[i], NULL, 16); 
            if (temp > 0)
            {
                i2cparam->r_len = temp;
            }
            continue;
        }
        if (!strcmp(param[i], "-v"))
        {
            printf("tty_demo ver:  %s\n", ver);
            continue;
        }
    }
}

/*
 * @description  : 主函数
 * @param - argc : 参数个数
 * @param - *argv: 带入参数数组指针
 * @return		 : 执行结果
 */
int main(int argc, char *argv[])
{
    int result = 0, i = 0;

    //检测是否有参数
    if (argc < 2 || strncmp(argv[1], "i2c", 3))
    {
        print_usage(argv[0]);
        exit(1);
    }

    //检测是否有--h或--help
    if ((!strcmp(argv[1], "--h")) || (!strcmp(argv[1], "--help")))
    {
        print_usage(argv[0]);
        exit(1);
    }
    //print_usage(argv[0])
    strcpy(dev, "/dev/");
    strcat(dev, argv[1]);

    //从main函数带入的参数解析为i2c应用参数
    get_param(argc, argv, &i2c_param);

    //当知道设备名称时可以直接赋值dev，例strcpy(dev, "/dev/i2c-0");
    //打开i2c 外设
    i2c_fd = open(dev, O_RDWR);
    if (i2c_fd < 0)
    {
        perror(dev);
        printf("Can't Open i2c device %s \n", dev);
        exit(0);
    }
    else
    {
        //打印参数
        printf("dev_addr=0x%x, write first addr=0x%x, write numb=%d, read first addr=0x%x, read numb=%d\n", i2c_param.dev_addr, i2c_param.w_addr, i2c_param.w_len, i2c_param.r_addr, i2c_param.r_len);
        //设置i2c参数
        if ((result = func_set_opt(i2c_fd)) < 0)
        {
            perror("set_opt error");
            exit(0);
        }

        //设置i2c为非阻塞方式
        if (fcntl(i2c_fd, F_SETFL, FNDELAY) < 0)
        {
            printf("fcntl failed!\n");
        }
    }

    while (1)
    {
        if (1 == write_mode)
        {
            printf("[%s nwrite=%d] ", dev, i2c_param.w_len);
            func_my_print(i2c_param.w_data, i2c_param.w_len, 'c');

            for (i = 0; i < i2c_param.w_len; i++)
            {
                func_write_regs(i2c_fd, i2c_param.dev_addr, i2c_param.w_addr++, &i2c_param.w_data[i], 1);
                usleep(3000);
            }
            write_mode = 0;
            usleep(1000);
        }

	else if (1 == read_mode)
        {
            for (i = 0; i < i2c_param.r_len; i++)
            {
                func_read_regs(i2c_fd, i2c_param.dev_addr, i2c_param.r_addr++, &i2c_param.r_data[i], 1);
                usleep(1000);
            }
            read_mode = 0;
            printf("[%s nread=%d ] ", dev, i2c_param.r_len);
            func_my_print(i2c_param.r_data, i2c_param.r_len, 'h');
            usleep(1000);
        }

	else break;
	
    }
    exit(0);
}

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h> /*PPSIX 终端控制定义*/
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "spi.h"
#include "file.h"

char ver[20] = {"ver01.001"};

int spi_fd;
struct_spi_param spi_param = {1000000, 0, 8, 0, 0, 0, 0, 0, 0}; /*外设参数初始化*/

unsigned char send_buff[100], receive_buff[100];
unsigned int send_num = 0, receive_num = 0, real_send_num = 0;
char dev[20];

//接收的数据存入文件
long syn_rev_len = 0; /*缓存数据长度，待同步到文件 */
char file_path[50];   /*文件路径 */
FILE *pFileStream = NULL;
int save_file = 0; /*是否需要存文件 */

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
 * @description : 打印参数设置格式
 * @param - pname: 函数名
 * @return		: 无
 */
static void print_usage(const char *pname)
{
    printf("Usage: %s device [-s speed] [-d delay] [-b bpm] [-m mode] [-l] [-nc] [-f path] [-v]"
           "\n\t'-s speed' for max speed (kHz)"
           "\n\t'-d delay' for delay (usec)"
           "\n\t'-b bpm' for bits per word 8/16"
           "\n\t'-m mode' for 0:CPOL=0,CPHA=0  1:CPOL=0,CPHA=1 2:CPOL=1,CPHA=0 3:CPOL=1,CPHA=1 "
           "\n\t'-l' for lsb:least significant bit first"
           "\n\t'-nc' for 1 dev/bus, no chipselect"
           "\n\t'-f path' save the data received by the serial port into the file, path for file to save"
           "\n\t'-v' show version"
           "\n\texample :  --> ./elf1_cmd_spi spidev0.0 "
           "\n\texample :  --> ./elf1_cmd_spi spidev0.0 -s 1000 -m 1 -f /home/root\n ",
           pname);
}

/*
 * @description : 解析函数带入参数
 * @param - numb: 参数个数
 * @param - *param: 带入参数数组指针
 * @param - *spiparam: spi应用参数
 * @return		: 无
 */
void get_param(int numb, char *param[], struct_spi_param *spiparam)
{
    int i = 0;
    unsigned char data = 0;
    unsigned int speed = 0;

    if (numb <= 2)
        return;

    for (i = 2; i < numb; i++)
    {
        if (!strcmp(param[i], "-s"))
        {
            i++;
            speed = atoi(param[i]);
            spiparam->speed = speed * 1000;
            continue;
        }
        if (!strcmp(param[i], "-d"))
        {
            i++;
            data = atoi(param[i]);
            spiparam->delay = data;
            continue;
        }
        if (!strcmp(param[i], "-b"))
        {
            i++;
            data = atoi(param[i]);
            if (16 == data)
            {
                spiparam->data_bit = data;
            }
            else
            {
                spiparam->data_bit = 8;
            }
            continue;
        }
        if (!strcmp(param[i], "-m"))
        {
            i++;
            data = atoi(param[i]);
            switch (data)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                spiparam->mode = data;
                break;
            default:
                spiparam->mode = 0;
                break;
            }
            continue;
        }
        if (!strcmp(param[i], "-l"))
        {
            spiparam->lsb_first = 1;
            continue;
        }
        if (!strcmp(param[i], "-nc"))
        {
            spiparam->no_cs = 1;
            continue;
        }
        if (!strcmp(param[i], "-f"))
        {
            i++;
            strcpy(file_path, param[i]);
            save_file = 1;
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
    int result = 0;
    unsigned char transfer_tx[38] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
        0xF0, 0x0D,
    };
    unsigned char transfer_rx[38] = {
        0,
    };

    //检测是否有参数
    if (argc < 2 || strncmp(argv[1], "spi", 3))
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

    strcpy(dev, "/dev/");
    strcat(dev, argv[1]);

    //从main函数带入的参数解析为SPI应用参数
    get_param(argc, argv, &spi_param);

    //当知道设备名称时可以直接赋值dev，例strcpy(dev, "/dev/spidev1.1");
    //打开SPI 外设
    spi_fd = open(dev, O_RDWR);
    if (spi_fd < 0)
    {
        perror(dev);
        printf("Can't Open spi device %s \n", dev);
        exit(0);
    }
    else
    {
        //打印参数
        printf("speed=%ldhz,delay=%d,data_bit=%d,mode=%d,lsb=%d,no_cs='%d,\n", spi_param.speed, spi_param.delay, spi_param.data_bit,
               spi_param.mode, spi_param.lsb_first, spi_param.no_cs);
        //设置SPI参数
        if ((result = func_set_opt(spi_fd, spi_param)) < 0)
        {
            perror("set_opt error");
            exit(0);
        }

        //设置SPI为非阻塞方式
        if (fcntl(spi_fd, F_SETFL, FNDELAY) < 0)
        {
            printf("fcntl failed!\n");
        }
    }

    //将接收的数据存入receive_data 文件中
    if (save_file == 1)
    {
        pFileStream = file_open(file_path);
    }

    while (1)
    {
        

            send_num = 38;
            printf("[%s nwrite=%d] ", dev, send_num);
            func_my_print(transfer_tx, send_num, 'h');

            func_transfer(spi_fd, transfer_tx, transfer_rx, send_num, spi_param);
            usleep(1000);

            receive_num = send_num;
            printf("[%s nread=%d ] ", dev, receive_num);
            func_my_print(transfer_rx, receive_num, 'h');

            if ((save_file == 1) && (pFileStream != NULL))
            {
                file_write(pFileStream, transfer_rx, receive_num, &syn_rev_len);
            }
        
        usleep(1000000);
    }
    file_close(pFileStream);
    exit(0);
}

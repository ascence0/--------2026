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

/*
 * @description : 参数设置函数
 * @param - fd  : 文件描述符
 * @param - param: SPI应用参数
 * @return		: 执行结果
 */
int func_set_opt(int fd, struct_spi_param param)
{
    int ret = 0;
    unsigned int set_mode = 0;

    //设置CPOL和CPHA
    set_mode |= param.mode;
    //设置SPI读写从低位开始
    if (1 == param.lsb_first)
    {
        set_mode |= SPI_LSB_FIRST;
    }
    //设置SPI片选高有效
    if (1 == param.cs_high)
    {
        set_mode |= SPI_CS_HIGH;
    }
    //设置SPI无片选
    if (1 == param.no_cs)
    {
        set_mode |= SPI_NO_CS;
    }

    //设置2线，或4线
    if (1 == param.dual_spi)
    {
        set_mode = set_mode | SPI_TX_DUAL | SPI_RX_DUAL;
    }
    else if (1 == param.quad_spi)
    {
        set_mode = set_mode | SPI_TX_QUAD | SPI_RX_QUAD;
    }

    // 设置SPI读/写模式
    ret = ioctl(fd, SPI_IOC_WR_MODE, &set_mode);
    if (ret == -1)
        printf("can't set spi mode");

    ret = ioctl(fd, SPI_IOC_RD_MODE, &set_mode);
    if (ret == -1)
        printf("can't get spi mode");

    //设置SPI读/写最大速率 单位为hz
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &param.speed);
    if (ret == -1)
        printf("can't set max speed hz");

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &param.speed);
    if (ret == -1)
        printf("can't get max speed hz");

    // 设置SPI读/写每字多少位
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &param.data_bit);
    if (ret == -1)
        printf("can't set bits per word");

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &param.data_bit);
    if (ret == -1)
        printf("can't get bits per word");

    printf("set done!\n");
    return 0;
}

/*
 * @description : 写多个寄存器
 * @param - fd  : 文件描述符
 * @param - reg : 要写入寄存器首地址
 * @param - *buf: 要写入数据buff
 * @param - lens: 要写入数据长度
 * @param - param: SPI应用参数
 * @return		: 执行结果
 */
int func_write_regs(int fd, unsigned char reg, unsigned char *buf, unsigned char lens, struct_spi_param param)
{
    int ret;
    unsigned char tx_data[lens + 1];

    tx_data[0] = reg & ~0x80; /* 写数据的时候寄存器地址bit8要清零 */
    memcpy(&tx_data[1], buf, lens);
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_data,
        .rx_buf = 0,
        .len = lens + 1,
        .delay_usecs = param.delay,
        .speed_hz = param.speed,
        .bits_per_word = param.data_bit,
    };

    //写2线或4线设置
    if (1 == param.dual_spi)
    {
        tr.tx_nbits = 2;
        tr.rx_nbits = 2;
    }
    else if (1 == param.quad_spi)
    {
        tr.tx_nbits = 4;
        tr.rx_nbits = 4;
    }
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        printf("can't send spi message");
    return ret;
}

/*
 * @description : 读多个寄存器
 * @param - fd  : 文件描述符
 * @param - reg : 要读入寄存器首地址
 * @param - *buf: 要读入数据buff
 * @param - lens: 要读入数据长度
 * @param - param: SPI参数
 * @return		: 执行结果
 */
int func_read_regs(int fd, unsigned char reg, unsigned char *buf, unsigned char lens, struct_spi_param param)
{
    int ret;
    unsigned char tx_data[lens + 1];
    memset(&tx_data[1], 0xff, lens);
    tx_data[0] = reg | 0x80; /* 读数据的时候寄存器地址bit8要置1 */
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_data,
        .rx_buf = (unsigned long)buf,
        .len = lens + 1,
        .delay_usecs = param.delay,
        .speed_hz = param.speed,
        .bits_per_word = param.data_bit,
    };

    //写2线或4线设置
    if (1 == param.dual_spi)
    {
        tr.tx_nbits = 2;
        tr.rx_nbits = 2;
    }
    else if (1 == param.quad_spi)
    {
        tr.tx_nbits = 4;
        tr.rx_nbits = 4;
    }
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        printf("can't send spi message");
    return ret;
}

/*
 * @description : 寄存器收发
 * @param - fd  : 文件描述符
 * @param - *tx : 发送缓冲区
 * @param - *rx : 接收缓冲区
 * @param - lens: 发送长度
 * @param - param: SPI参数
 * @return		: 无
 */
void func_transfer(int fd, unsigned char *tx, unsigned char *rx, unsigned int lens, struct_spi_param param)
{
    int ret;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = lens,
        .delay_usecs = param.delay,
        .speed_hz = param.speed,
        .bits_per_word = param.data_bit,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        printf("can't send spi message");
}

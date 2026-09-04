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

/*
 * @description : 参数设置函数
 * @param - fd  : 文件描述符
 * @return		: 执行结果
 */
int func_set_opt(int fd)
{
    int ret = 0;

    // 设置IIC超时时间
    ret = ioctl(fd, I2C_TIMEOUT, 2);
    if (ret == -1)
        printf("can't set timeout");

    ret = ioctl(fd, I2C_RETRIES, 1);
    if (ret == -1)
        printf("can't set retries");

    printf("set done!\n");
    return 0;
}

/*
 * @description     : 写一个寄存器
 * nmsgs决定了有多少start信号，一个msgs对应一个start信号，在nmsg个信号结束后总线会产生一个stop
 * @param - fd      : 文件描述符
 * @param - devaddr : 设备地址
 * @param - reg     : 要写入寄存器地址
 * @param - *buf    : 要写入数据buff
 * @param - data_len: 要写入数据长度
 * @return		    : 执行结果
 */
int func_write_regs(int fd, unsigned char devaddr, unsigned char reg, unsigned char *buf, int data_len)
{
    int ret;
    struct i2c_rdwr_ioctl_data work_queue;

    work_queue.nmsgs = 1;
    work_queue.msgs = (struct i2c_msg *)malloc(work_queue.nmsgs * sizeof(work_queue.msgs));
    if (!work_queue.msgs)
    {
        printf("memory alloc failed");
        ret = -1;
        return ret;
    }

    //往i2c里面写数据
    // printf("began to write:\n");
    work_queue.nmsgs = 1;
    (work_queue.msgs[0]).len = data_len + 1; // buf的长度，要写的字节数+1字节地址
    (work_queue.msgs[0]).flags = 0;          // write
    (work_queue.msgs[0]).addr = devaddr;     //设备地址
    (work_queue.msgs[0]).buf = (unsigned char *)malloc(data_len + 1);
    (work_queue.msgs[0]).buf[0] = reg;                 //写的地址
    memcpy(&work_queue.msgs[0].buf[1], buf, data_len); //你要写的数据

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&work_queue);
    if (ret < 0)
        printf("error during I2C_RDWR ioctl with error code %d\n", ret);
    free((work_queue.msgs[0]).buf);
    free(work_queue.msgs);
    return ret;
}

/*
 * @description     : 读一个寄存器
 * @param - fd      : 文件描述符
 * @param - devaddr : 设备地址
 * @param - reg     : 要读入寄存器地址
 * @param - *buf    : 要读入数据buff
 * @param - data_len: 要读入数据长度
 * @return		    : 执行结果
 */
int func_read_regs(int fd, unsigned char devaddr, unsigned char reg, unsigned char *buf, int data_len)
{
    int ret;
    struct i2c_rdwr_ioctl_data work_queue;

    work_queue.nmsgs = 1;
    work_queue.msgs = (struct i2c_msg *)malloc(work_queue.nmsgs * sizeof(work_queue.msgs));
    if (!work_queue.msgs)
    {
        printf("memory alloc failed");
        ret = -1;
        return ret;
    }

    //从i2c里面读出数据
    // printf("\nbegan to read:\n");
    work_queue.nmsgs = 1;
    //先设定一下地址
    (work_queue.msgs[0]).len = 1;   // buf的长度
    (work_queue.msgs[0]).flags = 0; // write
    (work_queue.msgs[0]).addr = devaddr;
    (work_queue.msgs[0]).buf = (unsigned char *)malloc(1);
    (work_queue.msgs[0]).buf[0] = reg;
    ret = ioctl(fd, I2C_RDWR, (unsigned long)&work_queue);
    if (ret < 0)
    {
        printf("error during I2C_RDWR ioctl with error code %d\n", ret);
        return ret;
    }
    //然后从刚才设定的地址处读
    work_queue.nmsgs = 1;
    (work_queue.msgs[0]).len = data_len;
    (work_queue.msgs[0]).flags = 1;
    (work_queue.msgs[0]).addr = devaddr;
    (work_queue.msgs[0]).buf = (unsigned char *)malloc(data_len);
    memset((work_queue.msgs[0]).buf, 0, data_len); //初始化读缓冲

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&work_queue);
    if (ret < 0)
    {
        printf("error during I2C_RDWR ioctl with error code %d\n", ret);
    }
    else
    {
        memcpy(buf, (work_queue.msgs[0]).buf, data_len);
    }
    free((work_queue.msgs[0]).buf);
    free(work_queue.msgs);
    return ret;
}

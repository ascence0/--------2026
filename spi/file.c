#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h> /*PPSIX 终端控制定义*/
#include "file.h"

/*
 * @description : 打开文件
 * @param - path: 文件路径
 * @return		: 文件结构指针
 */
FILE *file_open(char *path)
{
    FILE *pfile = NULL;

    strcat(path, "receive_data.txt");
    pfile = fopen(path, "wt+");
    if (pfile == NULL)
    {
        printf("File Open Fail!\n");
    }
    else
        printf("File Open %s OK!\n", path);
    return pfile;
}

/*
 * @description    : 写文件
 * @param - pfile  : 文件结构指针
 * @param - scr_ata: 要写入文件的数据
 * @param - len    : 要写入文件数据长度
 * @param - syn_len: 需要同步的数据长度
 * @return		   : 无
 */
void file_write(FILE *pfile, unsigned char *scr_ata, int len, long *syn_len)
{
    fwrite(scr_ata, sizeof(char), len, pfile); /*写入数据*/

    *syn_len += len;
    if (*syn_len > MAX_SYN_LEN)
    {
        fflush(pfile); /*将数据同步至ROM*/
        *syn_len = 0;
    }
}

/*
 * @description    : 关闭文件
 * @return		   : 无
 */
void file_close(FILE *pfile)
{
    fclose(pfile);
}
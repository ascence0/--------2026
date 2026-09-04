

#define MAX_LEN 265

/*I2c参数结构体 */
typedef struct
{
    unsigned char dev_addr; /*设备地址*/

    unsigned char w_addr;          /*写首地址 */
    unsigned int w_len;            /*写首地址 */
    unsigned char w_data[MAX_LEN]; /*写数据缓冲区 */

    unsigned char r_addr;          /*读首地址 */
    unsigned int r_len;            /*读首地址 */
    unsigned char r_data[MAX_LEN]; /*读数据缓冲区 */
} struct_i2c_param;

int func_set_opt(int fd);
int func_read_regs(int fd, unsigned char devaddr, unsigned char reg, unsigned char *buf, int data_len);
int func_write_regs(int fd, unsigned char devaddr, unsigned char reg, unsigned char *buf, int data_len);

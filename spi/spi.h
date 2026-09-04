
/*spi参数结构体 */
typedef struct
{
    unsigned long speed;     /*最大速率 */
    unsigned char mode;      /*模式 0 : CPOL=0,CPHA=0  1 : CPOL=0,CPHA=1 2 : CPOL=1,CPHA=0 3 : CPOL=1,CPHA=1 */
    unsigned char data_bit;  /*8位、16位 */
    unsigned char lsb_first; /*低位先发 msb是高位先发*/

    unsigned char cs_high; /*片选高有效*/
    unsigned char no_cs;   /*不使用片选*/

    unsigned char delay; /*延时*/

    unsigned char dual_spi; /*半双工 两线制 */
    unsigned char quad_spi; /*半双工 四线制*/
} struct_spi_param;

int func_set_opt(int fd, struct_spi_param param);
int func_read_regs(int fd, unsigned char reg, unsigned char *buf, unsigned char len, struct_spi_param param);
int func_write_regs(int fd, unsigned char reg, unsigned char *buf, unsigned char len, struct_spi_param param);
void func_transfer(int fd, unsigned char *tx, unsigned char *rx, unsigned int lens, struct_spi_param param);

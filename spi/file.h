

#define MAX_SYN_LEN 500

FILE *file_open(char *path);
void file_write(FILE *pfile, unsigned char *scr_ata, int len, long *syn_len);
void file_close(FILE *pfile);

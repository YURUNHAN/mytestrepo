#include "../level1/commands.h"

int cp_file(char *src, char *dest)
{
    int n = 0;
    char buf[BLKSIZE];
    //以读方式打开源文件，以写方式打开目标文件
    int fd = open_file(src, "0");
    int gd = open_file(dest, "1");

    buf[BLKSIZE]=0; // terminate it
    //如果目标文件不存在就创建一个
    if(gd < 0)
    {
        printf("Creating File\n");
        creat_file(dest);
        gd = open_file(dest, "1");;
    }
    //Need read to complete
    //循环读取源文件BLKSIZE大小（1024bytes）的内容，返回读取长度
    //并写入目标文件对应长度的地方
    while((n = myread(fd, buf, BLKSIZE, 0)))
    {
        mywrite(gd, buf, n);
    }
    //写完后关闭两文件
    close_file(fd);
    close_file(gd);
    
    return 0;
}

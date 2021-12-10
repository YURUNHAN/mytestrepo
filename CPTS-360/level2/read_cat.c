#include "../level1/type.h"

int read_file(char *fd, char *bytes){
    int i_fd = atoi(fd), i_bytes = atoi(bytes);
    char buf[BLKSIZE];
    printf("[read_file]: Reading fd=%d\n", i_fd);

    return myread(i_fd, buf, i_bytes, 0);
}

int min3(int a, int b, int c){
    if (a <= b && a <= c)
        return a;
    else if (b <= c && b <= a)
        return b;
    else
        return c;
}

int myread(int fd, char *buf, int nbytes, int supress_msg){
    char readbuf[BLKSIZE];
    int min, count=0, blk, lblk, dblk, start, remain, avail, ibuf[256], dbuf[256];

    if (running->fd[fd] == NULL){ // make sure fd exists
        printf("[myread]: fd is NULL!");
        return -1;
    }
    OFT *oftp = running->fd[fd];
    MINODE *mip = oftp->mptr;

    if (!mip || !oftp) return -1;

    avail = mip->inode.i_size - oftp->offset;

    if (!supress_msg)
        printf("[myread]: fd=%d offset=%d bytes=%d\n", fd, oftp->offset, nbytes);

    while (nbytes && avail){ // read loop
        lblk = oftp->offset / BLKSIZE;
        start = oftp->offset % BLKSIZE;
        //如果长度小于12，则为直接访问，直接将指针对应数据块的内容取出来放入buf
        if (lblk < 12){ // direct blocks
            if (!supress_msg)
                printf("[myread]: direct block\n");
            blk = mip->inode.i_block[lblk];
        }
        //如果长度在12到268之间，则为1级间接访问，将指针指向的数据指针取出来后，
        //再将数据指针指向的数据取出来
        else if (lblk >= 12 && lblk < 256 + 12){ // indirect blocks
            if (!supress_msg)
                printf("[myread]: indirect block\n");

            get_block(mip->dev, mip->inode.i_block[12], (char*)ibuf); // from book
            blk = ibuf[lblk-12];
        }
        //如果长度大于268，则为2级间接访问，将指针指向的指针指向的指针指向的数据取出来
        else{
            if (!supress_msg)
                printf("[myread]: double indirect block\n");
            
            lblk -= 268;
            int buf13[256];
            get_block(mip->dev, mip->inode.i_block[13], (char*)buf13);
            dblk = buf13[lblk/256];
            get_block(mip->dev, dblk, (char*)dbuf);
            blk = dbuf[lblk % 256];
        }

        get_block(mip->dev, blk, readbuf);
        char *cp = readbuf + start; // takes care of offset
        remain = BLKSIZE - start;

        // read optimization
        min = min3(nbytes, avail, remain);
        if (!supress_msg)
            printf("[myread]: offset=%d min=%d blk=%d\n", oftp->offset, min, blk);
        strncpy(buf, cp, min);
        //char *cq = buf;

        oftp->offset += min;
        count += min;
        avail -= min;
        nbytes -= min;
        remain -= min;

        //while (remain > 0){
        //    *cq++ = *cp++; // cpy byte into buf
        //    oftp->offset++;
        //    count++; // inc offset and count
        //    avail--; nbytes--; remain--; // dec avail, nbytes and remain
        //    if (nbytes <= 0 || avail <= 0)
        //        break;
        //}
        if (!supress_msg){
            printf("[myread]: nbytes=%d len(buf)=%d", count, (int)strlen(buf));
            printf(" text=%s\n", buf);
        }
    }

    return count;
}

int cat_file(char *filename){
    char mybuf[BLKSIZE];
    //根据输入文件路径，以mode0，读模式打开文件，将文件指针保存在fd中
    int len=0, n, i, fd = open_file(filename, "0");
    //文件不存在返回-1
    if (fd < 0) return -1;
    //初始化缓存buf
    mybuf[BLKSIZE]=0; // terminate mybuf

    printf("[cat_file]:\n\n");
    //读文件并将BLKSIZE大小（1024bytes）的数据放入mybuf中，返回读到的字节数n
    while ((n = myread(fd, mybuf, BLKSIZE, 1))){
        //在mybuf末尾加上终止符号
        mybuf[n]=0;
        //将读到的\n翻译成换行符
        for (i=0; i<n; i++){
            if (mybuf[i] == '\\' && mybuf[i++] == 'n'){
                putchar('\n');
                continue;
            }
            //在屏幕上打印数据
            putchar(mybuf[i]);
        }
        //打印的字符数
        len += n;
    }
    printf("[cat_file]: Read %d bytes.\n\n", len);
    //关闭文件
    close_file(fd);

    return 0;
}

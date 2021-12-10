/*********** util.c file ****************/
#include "type.h"

extern PROC *running;
extern MINODE *root, minode[NMINODE];
extern MNTENTRY mtable[NMNT];
extern char gpath[128], *name[32];
extern int n, inode_start, dev, imap, bmap, ninodes, nblocks;
int r;

/**
 * 通过块号找到在磁盘中的数据，并读出1024个字节
 * @param dev
 * @param blk
 * @param buf
 * @return
 */
int get_block(int dev, int blk, char *buf){
    lseek(dev, (long)blk*BLKSIZE, 0);
    r = read(dev, buf, BLKSIZE);

    return r;
}   

/**
 * 通过块号找到在磁盘中的数据，并最多写入1024个字节
 * @param dev
 * @param blk
 * @param buf
 * @return
 */

int put_block(int dev, int blk, char *buf){
    lseek(dev, (long)blk*BLKSIZE, 0);
    r = write(dev, buf, BLKSIZE);

    return r;
}   

/**
 * 根据 / 来切分路径，把切分后的结果存到了全局数组name中
 * @param pathname
 * @return
 */
int tokenize(char *pathname){
    int i;
    char *s;
    printf("[tokenize]: %s\n", pathname);

    strcpy(gpath, pathname);   // tokens are in global gpath[ ]
    n = 0;

    s = strtok(gpath, "/");
    while(s){
        name[n] = s;
        n++;
        s = strtok(0, "/");
    }

    for (i= 0; i<n; i++)
        printf("%s  ", name[i]);
    printf("\n");

    return 0;
}
    
// return minode pointer to loaded INODE
/**
 * 通过设备号和节点号，找到一个inode块
 * 如果没有对应的节点存在，就分配一个空间
 * @param dev
 * @param ino
 * @return
 */
MINODE *iget(int dev, int ino){
    int i;
    MINODE *mip;
    char buf[BLKSIZE];
    int blk, offset;
    INODE *ip;

    for (i=0; i<NMINODE; i++){
        mip = &minode[i];
        // 此种情况是inode已经被某个节点分配的情况下
        // 找到了需要的inode， 初始化的时候所有inode的ino都为0
        if (mip->dev == dev && mip->ino == ino){
            //printf("\nmip=%s refCount++ now=%d\n", mip->ino, mip->refCount);
            // 引用数加1
            mip->refCount++;
            //printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
            // 返回这个inode节点信息
            return mip;
        }
    }
    
    for (i=0; i<NMINODE; i++){
        mip = &minode[i];
        // inode节点没有被分配，从0号inode开始分配，根目录拿到了第0号inode
        // 通过refCount来确定是不是有inode没有被占用
        if (mip->refCount == 0){
            //printf("allocating NEW minode[%d] for [%d %d]\n", i, dev, ino);
            mip->refCount = 1;
            mip->dev = dev;
            mip->ino = ino;

            // get INODE of ino into buf[ ]
            // 获得节点表指针的偏移，每个inode节点大小128 Bytes
            blk    = (ino-1)/8 + inode_start;
            offset = (ino-1) % 8;

            //printf("iget: ino=%d blk=%d offset=%d\n", ino, blk, offset);
            // 找到inode节点所在的块
            get_block(dev, blk, buf);
            // 找到块中偏移
            ip = (INODE *)buf + offset;
            // copy INODE to mp->INODE
            mip->inode = *ip;
            return mip;
        }
    }   
    printf("PANIC: no more free minodes\n");
    return 0;
}


/**
 * 通过检查m_inode是不是refcount == 0以及 dirty位
 * 来决定是不是有数据需要写入到磁盘中，主要通过write()系统调用写入
 * @param mip
 */
void iput(MINODE *mip){
    int block, offset;
    char buf[BLKSIZE];
    INODE *ip;

    if (mip==0) return;

    //printf("\nmip=%d refCount-- now=%d\n", mip->ino, mip->refCount);
    mip->refCount--;
    if (mip->refCount > 0)  // minode is still in use
        return;
    if (!mip->dirty)        // INODE has not changed; no need to write back
        return;

    /* write INODE back to disk */
    /***** NOTE *******************************************
        For mountroot, we never MODIFY any loaded INODE
        so no need to write it back
        FOR LATER WROK: MUST write INODE back to disk if refCount==0 && DIRTY
        Write YOUR code here to write INODE back to disk
    ********************************************************/

    printf("[iput]: inode_start=%d\n", inode_start);

    block = (mip->ino - 1) / 8 + inode_start;
    offset = (mip->ino -1) % 8;

    printf("[iput]: block=%d, offset=%d\n", block, offset);

    get_block(mip->dev, block, buf);
    ip = (INODE *)buf + offset;
    *ip = mip->inode;
    put_block(mip->dev, block, buf);
    mip->refCount = 0;
} 

/**
 * 通过m_inode来找name
 * 先读出传入的m_inode绑定的inode，拿到这个inode中的第一个数据块的信息
 * 然后转换成目录类型，然后读出每个目录项的name属性与给定的name进行对比，
 * 如果对比成功了，然后这个name所对应的inode号,
 * 如果没有成功，接着查找直到结束，如果还没找到，返回0号
 * @param mip
 * @param name
 * @return
 */
int search(MINODE *mip, char *name){
    char *cp, sbuf[BLKSIZE], temp[256];
    DIR *dp;
    INODE *ip;

    printf("[search]: for %s in MINODE = [%d, %d]\n", name, mip->dev, mip->ino);
    ip = &(mip->inode);

    /*** search for name in mip's data blocks: ASSUME i_block[0] ONLY ***/

    get_block(dev, ip->i_block[0], sbuf);
    dp = (DIR *)sbuf;
    cp = sbuf;
    printf("[search]:  ino   rlen  nlen  name\n");

    while (cp < sbuf + BLKSIZE){
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        printf("[search]: %4d  %4d  %4d    %s\n", 
            dp->inode, dp->rec_len, dp->name_len, temp);
        if (strcmp(temp, name)==0){
            printf("[search]: found %s : ino = %d\n", temp, dp->inode);
            return dp->inode;
        }
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    return 0;
}


/**
 * 通过路径名，找到文件所对应的inode
 * @param pathname
 * @return
 */
int getino(char *pathname){
    int i, ino;// blk, disp;
    //char buf[BLKSIZE];
    //INODE *ip;
    MINODE *mip, *newmip;
    MNTENTRY *mntptr;

    printf("[getino]: pathname=%s\n", pathname);
    if (strcmp(pathname, "/")==0)
        return 2;

    // starting mip = root OR CWD
    if (pathname[0]=='/')
        mip = root;
    else
        mip = running->cwd;

    mip->refCount++;         // because we iput(mip) later

    tokenize(pathname);
    for (i=0; i<n; i++){
        if(!maccess(mip, 'x'))
        {
            printf("[getino]: Access denied\n");
            iput(mip);
            return -1;
        }
        printf("[getino]: ===========================================\n");
        printf("[getino]: i=%d name[%d]=%s ", i, i, name[i]);

        printf("mip->dev=%d, mip->ino=%d\n", mip->dev, mip->ino);
        printf("mounted=%d\n", mip->mounted);

        if (strncmp(name[i], "..", 2)==0 && mip->dev != root->dev && mip->ino == 2){
            printf("[getino]: crossing up from mount point\n");
            
            // get mount point mip
            for (i=0; i<NMNT; i++)
                if ((mntptr=&mtable[i])->dev == mip->dev)
                    break;

            newmip = mntptr->mptr;
            iput(mip);

            // get parent of mount pount mip and update dev
            u32 ino; 
            int pino = findino(newmip, &ino);
            printf("[getino]: ino=%d pino=%d\n", (int)ino, pino);
            mip = iget(newmip->dev, pino);
            dev = newmip->dev;
            printf("[getino]: new dev=%d\n", dev);

            continue;
        }

        ino = search(mip, name[i]);
        newmip = iget(dev, ino);
        iput(mip);
        mip = newmip;

        if (mip->mounted){
            printf("[getino]: crossing down to mount point\n");
            MNTENTRY *mntptr = mip->mntptr;
            iput(mip);
            dev = mntptr->dev;
            printf("[getino]: new dev=%d\n", dev);
            mip = iget(dev, 2);
        } else if (ino==0){
            iput(mip);
            printf("[getino]: name %s does not exist\n", name[i]);
            return 0;
        }
    }

    iput(mip);                   // release mip  
    return mip->ino;
}

/**
 * 给定父节点的m_inode和 my_inode
 * 找到 parent中的m_inode中 inode == my_inode的name，
 * @param parent
 * @param myino
 * @param myname
 * @return
 */
int findmyname(MINODE *parent, u32 myino, char *myname){
    DIR *dp;
    char buf[BLKSIZE], temp[256], *cp;

    get_block(dev, parent->inode.i_block[0], buf);
    dp = (DIR *)buf;
    cp = buf;

    while (cp < buf + BLKSIZE){
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;

        if (dp->inode == myino)
            strcpy(myname, temp);

        cp += dp->rec_len;
        dp = (DIR *)cp;
    }

    return 0;
}

/**
 * 根据传入的 minode, 返回inode号
 * @param mip
 * @param myino
 * @return
 */
int findino(MINODE *mip, u32 *myino){ // myino = ino of . return ino of ..
    char buf[BLKSIZE], *cp;   
    DIR *dp;

    get_block(mip->dev, mip->inode.i_block[0], buf);
    cp = buf; 
    dp = (DIR *)buf;
    *myino = dp->inode;
    cp += dp->rec_len;
    dp = (DIR *)cp;
    return dp->inode;
}

int show(MINODE *mip){
    DIR *dp;
    char buf[BLKSIZE], name[256], *cp;
    INODE *ip = &(mip->inode);

    get_block(mip->dev, ip->i_block[0], buf);
    cp = buf;
    dp = (DIR*)cp;

    printf("\ninode\trec_len\tname_len\tname\n========================================\n");
    while (cp < buf + BLKSIZE){
        strncpy(name, dp->name, dp->name_len);
        name[dp->name_len]=0;

        printf("%1d\t%1d\t%1d\t%s\n", dp->inode, dp->rec_len, dp->name_len, name);

        cp += dp->rec_len;
        dp = (DIR*)cp;
    }
    putchar('\n');

    return 0;
}

// LEVEL 1
int abs_path(char *path){
    if (path[0] == '/')
        return 0;
    else
        return -1;
}

/**
 * 测试给定buf中的某个bit是不是1
 * @param buf
 * @param bit
 * @return
 */
int tst_bit(char *buf, int bit){
    int i = bit / 8, j = bit % 8;

    if (buf[i] & (1 << j))
        return 1;

    return 0;
}

int set_bit(char *buf, int bit){
    int i = bit / 8, j = bit % 8;

    buf[i] |= (1 << j);

    return 0;
}

int clr_bit(char *buf, int bit){
    int i = bit / 8, j = bit % 8;

    buf[i] &= ~(1 << j);

    return 0;
}

int dec_free_inodes(int dev){
    char buf[BLKSIZE];

    get_block(dev, 1, buf); // dec the super table
    sp = (SUPER *)buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf; // dec the GD table
    gp->bg_free_inodes_count--;
    put_block(dev, 2, buf);

    return 0;
}

int dec_free_blocks(int dev){
    char buf[BLKSIZE];

    get_block(dev, 1, buf); // dec the super table
    sp = (SUPER *)buf;
    sp->s_free_blocks_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf; // dec the GD table
    gp->bg_free_blocks_count--;
    put_block(dev, 2, buf);

    return 0;
}

int inc_free_inodes(int dev){
    char buf[BLKSIZE];

    get_block(dev, 1, buf); // get the super table
    sp = (SUPER*)buf;
    sp->s_free_inodes_count++; // inc free inodes
    put_block(dev, 1, buf); // put it back

    get_block(dev, 2, buf); // get the gd table
    gp = (GD*)buf;
    gp->bg_free_inodes_count++; // inc free inodes
    put_block(dev, 2, buf); // put it back

    return 0;
}

int inc_free_blocks(int dev){
    char buf[BLKSIZE];

    get_block(dev, 1, buf); 
    sp = (SUPER *)buf;
    sp->s_free_blocks_count++; // dec the super table
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf; // dec the GD table
    gp->bg_free_blocks_count++;
    put_block(dev, 2, buf);

    return 0;
}

/**
 * 标记bitmap中inode，返回找到的inode号
 * @param dev
 * @return
 */
int ialloc(int dev){
    int i;
    char buf[BLKSIZE];

    get_block(dev, imap, buf);

    for (i=0; i < ninodes; i++){
        if (tst_bit(buf, i) == 0){
            set_bit(buf, i);
            put_block(dev, imap, buf);
            dec_free_inodes(dev);
            printf("[ialloc]: allocated ino = %d\n", i+1); // bits 0..n, ino 1..n+1
            return i+1;
        }
    }
    return 0;
}

/**
 * 释放bitmap中标记的inode信息
 * @param dev
 * @param ino
 * @return
 */
int idalloc(int dev, int ino){
    char buf[BLKSIZE];

    if (ino > ninodes){
        printf("[idalloc]: inumber=%d out of range.\n", ino);
        return 0;
    }

    get_block(dev, imap, buf);
    clr_bit(buf, ino-1);
    put_block(dev, imap, buf);

    inc_free_inodes(dev);
    printf("[idalloc]: deallocated ino=%d\n", ino);

    return 0;
}

/**
 * 分配block位，返回找到的block号
 * @param dev
 * @return
 */
int balloc(int dev){
    int i;
    char buf[BLKSIZE];

    get_block(dev, bmap, buf);

    for (i=0; i < nblocks; i++){
        if (tst_bit(buf, i) == 0){
            set_bit(buf, i);
            put_block(dev, bmap, buf);
            printf("[balloc]: allocated block = %d\n", i+1); // bits 0..n, ino 1..n+1
            dec_free_blocks(dev);
            return i+1;
        }
    }
    return 0;
}

// potential issue here, deallocating to wrong blk?
int bdalloc(int dev, int blk){
    char buf[BLKSIZE];

    if (blk > nblocks){
        printf("[bdalloc]: block=%d out of range.\n", blk);
        return 0;
    }

    get_block(dev, bmap, buf);
    clr_bit(buf, blk-1);
    put_block(dev, bmap, buf);

    inc_free_blocks(dev);
    printf("[bdalloc]: deallocated block=%d\n", blk-1);

    return 0;
}

// LEVEL 2
int pfd(){
    int i;
    printf("\n[pfd]:  fd   mode   offset   INODE\n");
    printf("[pfd]: ---- ------ -------- -------\n");
    for (i=0; i<NFD; i++){
        if (running->fd[i]){
            OFT *cur = running->fd[i];
            char mode[8];

            switch(cur->mode){
                case 0:
                    strcpy(mode, "READ"); break;
                case 1:
                    strcpy(mode, "WRITE"); break;
                case 2:
                    strcpy(mode, "R/W"); break;
                case 3:
                    strcpy(mode, "APPEND"); break;
            }

            printf("[pfd]:   %d %6s  %4d     [%d, %d]\n", i, mode, cur->offset,
                    cur->mptr->dev, cur->mptr->ino);
        } else
            break; // no more open fds
    }
    putchar('\n');

    return 0;
}

int faccess(char *pathname, char mode)
{
    char t1[9] = "xwrxwrxwr", t2[9] = "---------";
    char permi[9];
    int offset = 0;
    int ino = getino(pathname);
    MINODE *mip = iget(dev,ino);
    INODE *ip = &mip->inode;

    for (int i=8; i >= 0; i--) // permissions
        if (ip->i_mode & (1 << i))
            permi[i]=t1[i];
        else
            permi[i]='-';
    
    //printf("\npermission %s\n", permi);
    if(mode == 'w')
        offset = 1;
    if(mode == 'x')
        offset = 2;

    // Super User
    if(running->uid ==0)
        return 1;

    // Owner
    if(ip->i_uid == running->uid)
        if(mode == permi[offset])
            return 1;
    // Same group
    else if(ip->i_gid == running->gid)
        if(mode == permi[offset + 3])
            return 1;
    // Other
    else
        if(mode == permi[offset + 6])
            return 1;

    return 0;
}

int maccess(MINODE *mip, char mode)
{
    char t1[9] = "xwrxwrxwr", t2[9] = "---------";
    char permi[9];
    int offset = 0;
    INODE *ip = &mip->inode;

    for (int i=8; i >= 0; i--) // permissions
        if (ip->i_mode & (1 << i))
            permi[i]=t1[i];
        else
            permi[i]='-';
    
    //printf("\npermission %s\n", permi);
    if(mode == 'w')
        offset = 1;
    if(mode == 'x')
        offset = 2;

     // Super User
    if(running->uid ==0)
        return 1;    

    // Owner
    if(ip->i_uid == running->uid)
        if(mode == permi[offset])
            return 1;
    // Group
    else if(ip->i_gid == running->gid)
        if(mode == permi[offset + 3])
            return 1;
    // Other
    else
        if(mode == permi[offset + 6])
            return 1;

    return 0;
}

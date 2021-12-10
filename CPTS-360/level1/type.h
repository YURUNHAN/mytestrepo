/*************** type.h file ************************/
#ifndef __360PROJ_TYPES__
#define __360PROJ_TYPES__

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>

typedef unsigned char  u8;

typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   

#define FREE        0
#define READY       1

#define BLKSIZE     1024
#define NMINODE      128
#define NFD           16
#define NPROC          2
#define NOFT          40
#define NMNT          16

typedef struct minode{
    INODE inode;
    int dev, ino;
    int refCount;
    int dirty;
    int mounted;
    struct mt *mntptr;
}MINODE;

typedef struct oft{
    int  mode;
    int  refCount;
    MINODE *mptr;
    int  offset;
}OFT;

typedef struct proc{
    struct proc *next;
    int          pid;
    int          status;
    int          uid, gid;
    MINODE      *cwd;
    OFT         *fd[NFD];
}PROC;

typedef struct mt{
    MINODE *mptr;
    char name[64];
    int dev;
}MNTENTRY;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <unistd.h>
#include <fcntl.h>

int get_block(int dev, int blk, char *buf);
int put_block(int dev, int blk, char *buf);
int tokenize(char *pathname);
MINODE *iget(int dev, int ino);
void iput(MINODE *mip);
int search(MINODE *mip, char *name);
int getino(char *pathname);
int findmyname(MINODE *parent, u32 myino, char *myname);
int show(MINODE *mip);
int findino(MINODE *mip, u32 *myino);

// our added util functions
int abs_path(char *path);
int tst_bit(char *buf, int bit);
int set_bit(char *buf, int bit);
int clr_bit(char *buf, int bit);
int dec_free_inodes(int dev);
int dec_free_blocks(int dev);
int inc_free_inodes(int dev);
int inc_free_blocks(int dev);
int ialloc(int dev);
int idalloc(int dev, int ino);
int balloc(int dev);
int bdalloc(int dev, int blk);

int pfd();

int faccess(char *pathname, char mode);
int maccess(MINODE *mip, char mode);
extern MINODE minode[NMINODE], *root;
extern PROC proc[NPROC], *running;
extern MNTENTRY mtable[NMNT];

extern int dev, imap, bmap, ninodes, nblocks;

// mountroot.c
int init();
int mount_root();
int quit();

// cd_ls_pwd.c
int cd(char *pathname);
int ls_file(MINODE *mip, char *name);
int ls_dir(MINODE *mip);
int ls(char *pathname);
char *rpwd(MINODE *wd);
char *pwd(MINODE *wd);

//mkdir_creat.c
int make_dir(char *pathname);
int mymkdir(MINODE *pip, char *name);
int enter_name(MINODE *pip, int myino, char *myname);
int creat_file(char *pathname);
int mycreat(MINODE *pip, char *name);

// rmdir.c
int rm_dir(char *pathname);
int rm_name(MINODE *pmip, char *name);

// symlink.c
int sym_link(char *src, char *dest);

// link.c
int link_file(char *pathname, char *linkname);

// unlink.c
int unlink_file(char *filename);

// open_close_lseek.c
int open_file(char *pathname, char *mode);
int truncate_file(MINODE *mip);
int close_file(int fd);


#endif

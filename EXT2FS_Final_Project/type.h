/*************** type.h file ************************/
#ifndef TYPE
#define TYPE

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

#define BLKSIZE  1024
#define NMINODE    64
#define NFD         8
#define NPROC       2
#define NOFT       40
#define NMTABLE    10

#define SUPERBLOCK  1
#define GDBLOCK     2
#define MAGIC       0xEF53
#define DIR_MODE    0x41ED

#define RD 0
#define WR 1
#define RW 2
#define AP 3


typedef struct minode{
  INODE INODE;
  int dev, ino;
  int refCount;
  int dirty;
  // for level-3
  int mounted; //
  struct mount *mptr;
}MINODE;


typedef struct mount{
  int dev;
  int ninodes;
  int nblocks;
  int free_blocks;
  int free_inodes;
  int bmap;
  int imap;
  int iblock;
  MINODE *mntDirPtr;
  char devName[256];
  char mntName[64];
}MOUNT;


typedef struct oft{ // for level-2
  int  mode;
  int  refCount;
  MINODE *mptr;
  int  offset;
}OFT;

typedef struct proc{
  struct proc *next;
  int          pid;
  int          uid;
  int          status;
  MINODE      *cwd;
  OFT         *fd[NFD];
}PROC;

#endif
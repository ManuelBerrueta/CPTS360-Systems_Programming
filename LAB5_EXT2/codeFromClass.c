#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>

//! Define shorter types, to save fingers =)
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

GD      *gp;
SUPER   *sp;


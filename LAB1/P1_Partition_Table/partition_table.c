#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

struct partition {
	u8 drive;             /* drive number FD=0, HD=0x80, etc. */

	u8  head;             /* starting head */
	u8  sector;           /* starting sector */
	u8  cylinder;         /* starting cylinder */

	u8  sys_type;         /* partition type: NTFS, LINUX, etc. */

	u8  end_head;         /* end head */
	u8  end_sector;       /* end sector */
	u8  end_cylinder;     /* end cylinder */

	u32 start_sector;     /* starting sector counting from 0 */
	u32 nr_sectors;       /* number of of sectors in partition */
};

struct partition *p;


int main()
{
    int fd;
    char buf[512];
	int i=0;
	int partition_number = 0;

    fd = open("vdisk", O_RDONLY);  // check fd value: -1 means open() failed
    read(fd, buf, 512);            // read sector 0 into buf[ ]

	//TODO: Swap this for myprintf
	
	//! Print out first sector
/* 	while(i++ <= 512)
	{
		printf("%x\n", buf[i]);
	} */

	printf("TEST SPACE\n");
	//! 0x1BE is the start of the First Partition

	p = &(buf[0x1BE]); //*0x1BE = 446
	printf("==== Partion %x ====\n", partition_number);
	printf("start_sector: %x   |  ", p->start_sector);
	printf("end_sector: %x   |  ", p->end_sector);
	printf("nr_sectors: %x   |  ", p->nr_sectors);
	printf("sys_type: %x\n", p->sys_type); //* Print system type

	//! Increment partition number
	partition_number++;

    lseek(fd, (long)123*512, SEEK_SET);  // lseek to byte 123*512 OR sector 123
    read(fd, buf, 512);                  // read sector 123 into buf[ ]

	i=0;
/* 	while(i++ <= 512)
	{
		printf("%x\n", buf[i]);
	} */


}
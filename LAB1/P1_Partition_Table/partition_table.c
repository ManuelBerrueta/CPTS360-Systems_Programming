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
	int end_of_sector = 0;

    fd = open("vdisk", O_RDONLY);  // check fd value: -1 means open() failed
    read(fd, buf, 512);            // read sector 0 into buf[ ]

	//TODO: Swap this for myprintf
	
	//! Print out first sector
/* 	while(i++ <= 512)
	{
		printf("%x\n", buf[i]);
	} */

	//! 0x1BE is the start of the First Partition
	p = &(buf[0x1BE]); //*0x1BE = 446

	//! Traverse through the first 4 partitions
	while(partition_number < 4)
	{
		printf("==== Partion %d ====\n", partition_number);
		printf("start_sector: %d   |  ", p->start_sector);
		end_of_sector = (p->start_sector + p->nr_sectors) - 1;
		printf("end_sector: %d   |  ", end_of_sector);
		//printf("end_sector: %d   |  ", p->end_sector); //TODO: Why isn't this printing out the right value??
		printf("nr_sectors: %d   |  ", p->nr_sectors);
		printf("sys_type: %x\n", p->sys_type); 

		p++; //! Move to the next partition
		partition_number++;
	}

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
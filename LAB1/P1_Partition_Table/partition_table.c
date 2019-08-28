/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Read the virtual disk "vdisk" as the Linux utility fdisk would          *
 *                                                                           *
 *                                                                           *
 * 																			 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
	int partition_number = 1;
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
	while(partition_number <= 4)
	{
		printf("==== Partion %d ====\n", partition_number);
		printf("start_sector: %d   |  ", p->start_sector);
		end_of_sector = (p->start_sector + p->nr_sectors) - 1;
		printf("end_sector: %d   |  ", end_of_sector);
		//printf("end_sector: %d   |  ", p->end_sector); //TODO: Why isn't this printing out the right value??
		printf("nr_sectors: %d   |  ", p->nr_sectors);
		printf("sys_type: %x\n", p->sys_type); 

		p++; //! Move to the next partition
		partition_number++; // Increment partition number
	}

	printf("********** Look for Extend Partition ************\n");
	//! p=*p? EXTENDED Partition
	//? p=&p; this assignment works... TODO:remove?

	// lseek to byte (p4s start sector)*512 OR in this case sector 1440
	//lseek(fd, (long)(p->start_sector)*512, SEEK_SET);
	
	//! Partition 5
	lseek(fd, (long)1440*512, SEEK_SET);  
    read(fd, buf, 512);                  // read sector 123 into buf[ ]
	p = &(buf[0x1BE]); // This should be the start of the first sector..
	printf("%d   |  ", p->start_sector); //? 18
	printf("%d \n", p->nr_sectors);		//? 342
	p++;
	printf("%d   |  ", p->start_sector); //? 377
	printf("%d \n", p->nr_sectors); //? 523
	

	//! Partition 6
	//int tempSector = p->start_sector + p->nr_sectors;
	//tempSector = tempSector+1440;

	//? 1440 + 523 = 1963 Xnot work
	//?
	lseek(fd, (long)1458*512, SEEK_SET);  //!this works but skipped a part
    read(fd, buf, 512);                  // read sector 123 into buf[ ]
	p = &(buf[0x1BE]); // This should be the start of the first sector..
	printf("%d  |  ", p->start_sector);
	printf("%d  |  \n", p->nr_sectors);


	lseek(fd, (long)1817*512, SEEK_SET);  //!this works but skipped a part
    read(fd, buf, 512);                  // read sector 123 into buf[ ]
	p = &(buf[0x1BE]); // This should be the start of the first sector..
	printf("%d  |  ", p->start_sector);
	printf("%d  |  \n", p->nr_sectors);

	//!Partition 7
	//int tempSector = p->start_sector + p->nr_sectors;
	//tempSector = tempSector+1440;

	lseek(fd, (long)1452*512, SEEK_SET);  
    read(fd, buf, 512);                  // read sector 123 into buf[ ]
	p = &(buf[0x1BE]); // This should be the start of the first sector..
	printf("%d  |  ", p->start_sector);
	printf("%d  |  ", p->nr_sectors);
	getchar();
	
	//! NOTES: The idea is that the P4's begin sector is a local MBR
	//!        Thus, we should be able to access it like we had before?
	//? Also if they are relative to P4's sector can't we just seek from there?
	//? may be multiply the size of the partition *512 (size of sector)
	//? To move on to the next partition...

/* 	while(p != 0)
	{
		printf("%x", p->start_sector);
		p+1440;
		partition_number++; // Increment partition number
		
		//! FOR TESTING ONLY -REMOVE AFTER DEBUG
 		if (partition_number == 7)
		{
			break;
		}
	} */


    //lseek(fd, (long)123*512, SEEK_SET);  // lseek to byte 123*512 OR sector 123
    //read(fd, buf, 512);                  // read sector 123 into buf[ ]

	i=0;
/* 	while(i++ <= 512)
	{
		printf("%x\n", buf[i]);
	} */

	fclose(fd);

	return 0;
}
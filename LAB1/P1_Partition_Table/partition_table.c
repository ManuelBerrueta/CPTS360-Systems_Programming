/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Read the virtual disk "vdisk" as the Linux utility fdisk would          *
 *                                                                           *
 *                                                                           *
 * 																			 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct partition
{
	u8 drive; /* drive number FD=0, HD=0x80, etc. */

	u8 head;	 /* starting head */
	u8 sector;   /* starting sector */
	u8 cylinder; /* starting cylinder */

	u8 sys_type; /* partition type: NTFS, LINUX, etc. */

	u8 end_head;	 /* end head */
	u8 end_sector;   /* end sector */
	u8 end_cylinder; /* end cylinder */

	u32 start_sector; /* starting sector counting from 0 */
	u32 nr_sectors;   /* number of of sectors in partition */
};

struct partition *p;

int main()
{
	int fd;
	char buf[512];
	int i = 0;
	int partition_number = 1;
	int end_of_sector = 0;
	int maintemp = 0;
	int sectoroffset = 0;

	fd = open("vdisk", O_RDONLY); // check fd value: -1 means open() failed
	read(fd, buf, 512);			  // read sector 0 into buf[ ]

	//TODO: Swap this for myprintf

	//! Print out first sector
	/* 	while(i++ <= 512)
	{
		myprintf("%x\n", buf[i]);
	} */

	//! 0x1BE is the start of the First Partition
	p = &(buf[0x1BE]); //*0x1BE = 446

	//! Traverse through the first 4 partitions
	while (partition_number <= 4)
	{
		myprintf("==== Partion %d ====\n", partition_number);
		myprintf("start_sector: %d   |  ", p->start_sector);
		end_of_sector = (p->start_sector + p->nr_sectors) - 1;
		myprintf("end_sector: %d   |  ", end_of_sector);
		//myprintf("end_sector: %d   |  ", p->end_sector); //TODO: Why isn't this printing out the right value??
		myprintf("nr_sectors: %d   |  ", p->nr_sectors);
		myprintf("sys_type: %x\n", p->sys_type);

		p++;
		partition_number++; // Increment partition number
	}

	p--;
	myprintf("************ Look for Extend Partition ************\n");
	long p4offset = p->start_sector;
	long tempOffset = 0;

	while( p->nr_sectors != 0)
	{
		lseek(fd, (long)(p4offset+tempOffset) * 512, SEEK_SET);
		read(fd, buf, 512);								   // read sector 123 into buf[ ]
		p = &(buf[0x1BE]);								   // This should be the start of the first sector..
		myprintf("start_sector: %d   |  ", p->start_sector); //? 18
		//! Testing
		//myprintf("p->end_sector: %d   |  ", p->end_sector); //? TESTING
		end_of_sector = (p->start_sector + p->nr_sectors) - 1;
		myprintf("end_sector: %d  |  ", end_of_sector);
		myprintf("nr sector: %d  |  ", p->nr_sectors);	   //? 342"
		myprintf("sys_type: %x  \n", p->sys_type);

		++p;
		myprintf("start_sector: %d   |  ", p->start_sector); //? 377
		//! Testing
		//myprintf("p->end_sector: %d   |  ", p->end_sector); //? TESTING
		end_of_sector = (p->start_sector + p->nr_sectors) - 1;
		myprintf("end_sector: %d  |  ", end_of_sector);
		myprintf("nr_sector: %d  |  ", p->nr_sectors);		   //? 523
		myprintf("sys_type: %x\n", p->sys_type);

		tempOffset= p->start_sector;
	}


	//TODO: The code below is the non-looped version of the second loop above.
/* 	//! Partition 5
	lseek(fd, (long)1440 * 512, SEEK_SET);
	read(fd, buf, 512);								   // read sector 123 into buf[ ]
	p = &(buf[0x1BE]);								   // This should be the start of the first sector..
	myprintf("start_sector: %d   |  ", p->start_sector); //? 18
	//! Testing
	//myprintf("p->end_sector: %d   |  ", p->end_sector); //? TESTING
	end_of_sector = (p->start_sector + p->nr_sectors) - 1;
	myprintf("end_sector: %d  |  ", end_of_sector);
	myprintf("nr sector: %d  |  ", p->nr_sectors);	   //? 342"
	myprintf("sys_type: %x  \n", p->sys_type);

	p++;
	myprintf("start_sector: %d   |  ", p->start_sector); //? 377
	//! Testing
	//myprintf("p->end_sector: %d   |  ", p->end_sector); //? TESTING
	end_of_sector = (p->start_sector + p->nr_sectors) - 1;
	myprintf("end_sector: %d  |  ", end_of_sector);
	myprintf("nr_sector: %d  |  ", p->nr_sectors);		   //? 523
	myprintf("sys_type: %x\n", p->sys_type);


	//! Partition 6
	//? Here 377 is the start of the last sector

	lseek(fd, (long)(1440 + 377) * 512, SEEK_SET); 
	read(fd, buf, 512);							  
	p = &(buf[0x1BE]);							  
	myprintf("start_sector: %d   |  ", p->start_sector); 
	//! Testing
	//myprintf("p->end_sector: %d   |  ", p->end_sector);
	end_of_sector = (p->start_sector + p->nr_sectors) - 1;
	myprintf("end_sector: %d  |  ", end_of_sector);
	myprintf("nr_sector: %d  |  ", p->nr_sectors);		  
	myprintf("sys_type: %x\n", p->sys_type);
	//? EXPERIMENTAL
	p++;
	myprintf("start_sector: %d   |  ", p->start_sector); 
	//! Testing
	//myprintf("p->end_sector: %d   |  ", p->end_sector); 
	end_of_sector = (p->start_sector + p->nr_sectors) - 1;
	myprintf("end_sector: %d  |  ", end_of_sector);
	myprintf("nr_sector: %d  |  ", p->nr_sectors);		   
	myprintf("sys_type: %x\n", p->sys_type);

	//!Partition 7
	//int tempSector = p->start_sector + p->nr_sectors;
	//tempSector = tempSector+1440;

	//? Here 917 is the start of the last sector

	lseek(fd, (long)(1440+917) * 512, SEEK_SET);
	read(fd, buf, 512); // read sector 123 into buf[ ]
	p = &(buf[0x1BE]);  // This should be the start of the first sector..
	myprintf("start_sector: %d   |  ", p->start_sector);
	//myprintf("p->end_sector: %d   |  ", p->end_sector);
	end_of_sector = (p->start_sector + p->nr_sectors) - 1;
	myprintf("end_sector: %d  |  ", end_of_sector);
	myprintf("nr_sector: %d  |  ", p->nr_sectors);		   
	myprintf("sys_type: %x\n", p->sys_type);
	p++;
	myprintf("start_sector: %d   |  ", p->start_sector);
	//! Testing
	//myprintf("p->end_sector: %d   |  ", p->end_sector);
	end_of_sector = (p->start_sector + p->nr_sectors) - 1;
	myprintf("end_sector: %d  |  ", end_of_sector);
	myprintf("nr_sector: %d  |  ", p->nr_sectors);	
	myprintf("sys_type: %x\n", p->sys_type); */

	return 0;
}
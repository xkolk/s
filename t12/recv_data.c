#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MEM_BASE 0x20002000
#define REG_BASE  0x20001000
#define MEM_SIZE	(1024)
#define REG_SIZE	(16)

#define PLAT_WR_FLAG_REG		(8) /*Offset of flag register*/
#define PLAT_WR_SIZE_REG		(12) /*Offset of size register*/
#define PLAT_WR_START	(1) /*IO data ready flag */


extern int errno;

int main(int argc, char **argv)
{
	volatile unsigned int *reg_addr = NULL, *count_addr, *flag_addr;
	volatile unsigned char *mem_addr = NULL;
	unsigned int i, num = 0, val;

	int fd = open("/dev/mem",O_RDWR|O_SYNC);
	if(fd < 0)
	{
		printf("Can't open /dev/mem\n");
		return -1;
	}
	mem_addr = (unsigned char *) mmap(0, MEM_SIZE, PROT_READ, MAP_SHARED, fd, MEM_BASE);
	if(mem_addr == NULL)
	{
		printf("Can't mmap\n");
		return -1;
	}

	reg_addr = (unsigned int *) mmap(0, REG_SIZE, PROT_WRITE |PROT_READ, MAP_SHARED, fd, REG_BASE);

	if(reg_addr == NULL)
	{
		printf("Can't mmap\n");
		return -1;
	}

	flag_addr = reg_addr + 2; /* skip read registers */
	count_addr = reg_addr + 3;

	if (*flag_addr & PLAT_WR_START) {
		num = *count_addr;
		if (num > MEM_SIZE)
			num = MEM_SIZE;
		fwrite((void *)mem_addr, 1, num, stdout);
		puts("");
		*flag_addr = 0;
	}

	return 0;
}

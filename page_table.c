#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "page_table_driver.h"

int fd;

// Obtain my cr3 value (a.k.a. PML4 table physical address)
uint64_t get_cr3_value()
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_CR3;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
	return cmd.ret;
}

// Given a physical address, return the value
uint64_t read_physical_address(uint64_t physical_address)
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_READ;
	cmd.request[1] = physical_address;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
	return cmd.ret;
}

// Write value to a physical address
void write_physical_address(uint64_t physical_address, uint64_t value)
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_WRITE;
	cmd.request[1] = physical_address;
	cmd.request[2] = value;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
}

int main()
{
	char *x = (char*)aligned_alloc(4096, 4096) + 0x123;
	char *y = (char*)aligned_alloc(4096, 4096) + 0x123;
	strcpy(x, "This is a simple HW.");
	strcpy(y, "You have to modify my page table.");

	fd = open("/dev/os", O_RDONLY);
	if(fd < 0) 
	{
		printf("Cannot open device!\n");
		return 0;
	}

	printf("Before\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	uint64_t x_addr = &x;
	uint64_t y_addr = &y;
	uint64_t cr3 = get_cr3_value();

	uint64_t x_pml4 = (x_addr >> 39)*8;
	x_addr -= x_addr >> 39 << 39;

	uint64_t x_pdpt_addr = read_physical_address(cr3 + x_pml4);
	x_pdpt_addr = x_pdpt_addr >> 12;
	x_pdpt_addr = x_pdpt_addr << 12;
	x_pdpt_addr = x_pdpt_addr << 28;
	x_pdpt_addr = x_pdpt_addr >> 28;
	uint64_t x_pdpt = (x_addr >> 30)*8;
	x_addr -= x_addr >> 30 << 30;

	uint64_t x_pd_addr = read_physical_address(x_pdpt_addr + x_pdpt);
	x_pd_addr = x_pd_addr >> 12;
	x_pd_addr = x_pd_addr << 12;
	uint64_t x_pd = (x_addr >> 21)*8;
	x_addr -= x_addr >> 21 << 21;

	uint64_t x_pt_addr = read_physical_address(x_pd_addr + x_pd);
	x_pt_addr = x_pt_addr >> 12;
	x_pt_addr = x_pt_addr << 12;
	uint64_t x_pt = (x_addr >> 12)*8;
	x_addr -= x_addr >> 12 << 12;

	uint64_t x_offset_addr = read_physical_address(x_pt_addr + x_pt);
	x_offset_addr = x_offset_addr >> 12;
	x_offset_addr = x_offset_addr << 12;
	x_offset_addr = x_offset_addr << 28;
	x_offset_addr = x_offset_addr >> 28;
	uint64_t x_offset = (x_addr);

	uint64_t x_physical_addr = read_physical_address(x_offset_addr + x_offset);

	//-------------------------------------------------------------------------------
	uint64_t y_pml4 = (y_addr >> 39)*8;
	y_addr -= y_addr >> 39 << 39;

	uint64_t y_pdpt_addr = read_physical_address(cr3 + y_pml4);
	y_pdpt_addr = y_pdpt_addr >> 12;
	y_pdpt_addr = y_pdpt_addr << 12;
	y_pdpt_addr = y_pdpt_addr << 28;
	y_pdpt_addr = y_pdpt_addr >> 28;
	uint64_t y_pdpt = (y_addr >> 30)*8;
	y_addr -= y_addr >> 30 << 30;

	uint64_t y_pd_addr = read_physical_address(y_pdpt_addr + y_pdpt);
	y_pd_addr = y_pd_addr >> 12;
	y_pd_addr = y_pd_addr << 12;
	uint64_t y_pd = (y_addr >> 21)*8;
	y_addr -= y_addr >> 21 << 21;

	uint64_t y_pt_addr = read_physical_address(y_pd_addr + y_pd);
	y_pt_addr = y_pt_addr >> 12;
	y_pt_addr = y_pt_addr << 12;
	uint64_t y_pt = (y_addr >> 12)*8;
	y_addr -= y_addr >> 12 << 12;

	uint64_t y_offset_addr = read_physical_address(y_pt_addr + y_pt);
	y_offset_addr = y_offset_addr >> 12;
	y_offset_addr = y_offset_addr << 12;
	y_offset_addr = y_offset_addr << 28;
	y_offset_addr = y_offset_addr >> 28;
	uint64_t y_offset = (y_addr);

	uint64_t y_physical_addr = read_physical_address(y_offset_addr + y_offset);
	
	write_physical_address(y_offset_addr + y_offset, x_physical_addr);

	getchar();

	printf("After modifying page table\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	getchar();

	strcpy(y, "When you modify y, x is modified actually.");
	printf("After modifying string y\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	write_physical_address(y_offset_addr + y_offset, y_physical_addr);

	getchar();

	printf("After recovering page table of y\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	close(fd);
}

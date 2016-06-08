/*
 * dump_pm_regs.c -- Dump Raspberry Pi's PM_* registers.
 *
 * Copyright (c) 2016 Sugizaki Yukimasa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <bcm_host.h>

#define HW_REGISTER_RW(addr) (((addr)) & ~PM_BASE)
#define HW_REGISTER_RO(addr) (((addr)) & ~PM_BASE)
#include "cpr_powman.h"

#define DEV_MEM "/dev/mem"

#define BUS_TO_PHYS(addr) ((((addr)) & ~0xc0000000))

/* Note: base should be pagesize-aligned. */
static void* mapmem_cpu(const off_t base, const size_t size)
{
	int fd = -1;
	unsigned *mem = NULL;
	int reti;

	fd = open(DEV_MEM, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "error: open: %s: %s\n", DEV_MEM, strerror(errno));
		exit(EXIT_FAILURE);
	}

	mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base);
	if (mem == MAP_FAILED) {
		fprintf(stderr, "error: mmap: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	reti = close(fd);
	if (reti == -1) {
		fprintf(stderr, "error: close: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	return mem;
}

static void unmapmem_cpu(void *addr, const size_t size)
{
	int reti;

	reti = munmap(addr, size);

	if (reti == -1) {
		fprintf(stderr, "error: munmap: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

#define print_reg(addr, offset) print_reg_core(((addr)), ((offset)), #offset)

static void print_reg_core(unsigned *addr, const off_t offset, const char *name)
{
	printf("%s: 0x%08x\n", name, addr[offset / sizeof(*addr)]);
}

int main()
{
	unsigned *addr = NULL;
	uint32_t peri_addr;

	bcm_host_init();
	peri_addr = bcm_host_get_peripheral_address();
	bcm_host_deinit();

	addr = mapmem_cpu(BUS_TO_PHYS(peri_addr + (PM_BASE & ~0xff000000)), 0x1000);

	print_reg(addr, PM_GNRIC);
	print_reg(addr, PM_AUDIO);
	print_reg(addr, PM_STATUS);
	print_reg(addr, PM_IMAGE);
	print_reg(addr, PM_GRAFX);
	print_reg(addr, PM_PROC);
	print_reg(addr, PM_RSTC);
	print_reg(addr, PM_RSTS);
	print_reg(addr, PM_WDOG);
	print_reg(addr, PM_PADS0);
	print_reg(addr, PM_PADS2);
	print_reg(addr, PM_PADS3);
	print_reg(addr, PM_PADS4);
	print_reg(addr, PM_PADS5);
	print_reg(addr, PM_PADS6);
	print_reg(addr, PM_CAM0);
	print_reg(addr, PM_CAM1);
	print_reg(addr, PM_CCP2TX);
	print_reg(addr, PM_DSI0);
	print_reg(addr, PM_DSI1);
	print_reg(addr, PM_HDMI);
	print_reg(addr, PM_USB);
	print_reg(addr, PM_PXLDO);
	print_reg(addr, PM_PXBG);
	print_reg(addr, PM_DFT);
	print_reg(addr, PM_SMPS);
	print_reg(addr, PM_XOSC);
	print_reg(addr, PM_SPAREW);
	print_reg(addr, PM_SPARER);
	print_reg(addr, PM_AVS_RSTDR);
	print_reg(addr, PM_AVS_STAT);
	print_reg(addr, PM_AVS_EVENT);
	print_reg(addr, PM_AVS_INTEN);
	print_reg(addr, PM_DUMMY);

	unmapmem_cpu(addr, 0x1000);
	addr = NULL;

	return 0;
}

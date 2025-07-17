// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2017 Tuomas Tynkkynen
 */

#include <common.h>
#include <dm.h>
#include <fdtdec.h>
#include <virtio_types.h>
#include <virtio.h>

#ifdef CONFIG_ARM64
#include <asm/armv8/mmu.h>

static struct mm_region pi4_mem_map[] = {
	{
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x40000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	},
	{
		0,
	}
};

struct mm_region *mem_map = pi4_mem_map;
#endif

int board_init(void)
{

	return 0;
}

int dram_init(void)
{
	gd->ram_size = 0x40000000UL; /* 1 GB */

	return 0;
}

int dram_init_banksize(void)
{
	return 0;
}
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

static struct mm_region qemu_arm64_mem_map[] = {
};

struct mm_region *mem_map = qemu_arm64_mem_map;
#endif

int board_init(void)
{

	return 0;
}

int dram_init(void)
{
	return 0;
}

int dram_init_banksize(void)
{
	return 0;
}
#include <common.h>
#include <dm.h>
#include <errno.h>
#include <watchdog.h>
#include <asm/io.h>
#include <serial.h>
#include <linux/compiler.h>
#include "serial_hl_pl_internal.h"

DECLARE_GLOBAL_DATA_PTR;

static int pl_putc(struct pl_regs *regs, char c)
{
	if (readl(&regs->fr) & (1 << 5))
		return -EAGAIN;

	/* Send the character */
	writel(c, &regs->dr);

	return 0;
}

static int pl_getc(struct pl_regs *regs)
{
	unsigned int data;

	/* Wait until there is data in the FIFO */
	if (readl(&regs->fr) & (1 << 4))
		return -EAGAIN;

	data = readl(&regs->dr);
    
	return (int) data;
}

#if defined(CONFIG_DEBUG_UART_HL_PL)

#include <debug_uart.h>

static void pl_generic_serial_init(struct pl_regs *regs)
{
    u32 selector = 0;
    // 1. set gpio functions for UART
    selector = readl(GPFSEL1);
    selector &= ~(7 << 12); // Clear bits for GPIO 14
    selector |= (4 << 12);  // Set GPIO 14 to ALT0 (UART TX)
    selector &= ~(7 << 15); // Clear bits for GPIO 15
    selector |= (4 << 15);  // Set GPIO 15 to ALT0 (UART RX)
    writel(selector, GPFSEL1);

    // 2. set pull-up/down for UART pins
    writel(0, GPIO_PUP_PDN_CNTRL_REG0); // Disable pull-up/down
    selector |= (0x0 << 30) | (0x3 << 28);
    writel(selector, GPIO_PUP_PDN_CNTRL_REG0);

    /* Initialize the serial port */
    writel(0, &regs->cr);  /* Disable the UART */
    writel(26, &regs->ibrd);  /* 8 bits, no parity, 1 stop bit */
    writel(0x3, &regs->fbrd);  /* Set integer baud rate divisor */
    writel((1 << 4) | (3 << 5), &regs->lcrh);  /* Set fractional baud rate divisor */
    writel(0, &regs->imsc);  /* Enable the UART */
    writel(1 | (1 << 8) | (1 << 9), &regs->cr);  /* Enable RX, TX, and the UART */
}

static void _debug_uart_init(void)
{
	struct pl_regs *regs = (struct pl_regs *)0xFE201000; // Base address for HL PL UART

	pl_generic_serial_init(regs);
}

static inline void _debug_uart_putc(int ch)
{
	struct pl_regs *regs = (struct pl_regs *)0xFE201000;

	pl_putc(regs, ch);
}

DEBUG_UART_FUNCS

#endif

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

static const struct udevice_id pl_serial_id[] ={
	{.compatible = "hl,pl-uart", .data = 0},
	{}
};

int pl_serial_ofdata_to_platdata(struct udevice *dev)
{
    struct pl_serial_platdata *plat = dev_get_platdata(dev);
    fdt_addr_t addr;

    addr = devfdt_get_addr(dev);

    plat->base = addr;

    return 0;
}

int pl_serial_putc(struct udevice *dev, const char ch)
{
	struct pl_priv *priv = dev_get_priv(dev);

    return pl_putc(priv->regs, ch);
}

int pl_serial_pending(struct udevice *dev, bool input)
{
    return 1;
}

int pl_serial_getc(struct udevice *dev)
{
	struct pl_priv *priv = dev_get_priv(dev);

    return pl_getc(priv->regs);
}

int pl_serial_setbrg(struct udevice *dev, int baudrate)
{
    return 0;
}

static const struct dm_serial_ops pl_serial_ops = {
	.putc = pl_serial_putc,
	.pending = pl_serial_pending,
	.getc = pl_serial_getc,
	.setbrg = pl_serial_setbrg,
};

static void pl_init_uart(struct pl_regs *regs)
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

int pl_serial_probe(struct udevice *dev)
{
    struct pl_serial_platdata *plat = dev_get_platdata(dev);
    struct pl_priv *priv = dev_get_priv(dev);

    priv->regs = (struct pl_regs *)plat->base;
    pl_init_uart(priv->regs);

    return 0;
}

U_BOOT_DRIVER(serial_pl01x) = {
	.name	= "serial_pl",
	.id	= UCLASS_SERIAL,
	.of_match = of_match_ptr(pl_serial_id),
	.ofdata_to_platdata = of_match_ptr(pl_serial_ofdata_to_platdata),
	.platdata_auto_alloc_size = sizeof(struct pl_serial_platdata),
	.probe = pl_serial_probe,
	.ops	= &pl_serial_ops,
	.flags = DM_FLAG_PRE_RELOC,
	.priv_auto_alloc_size = sizeof(struct pl_priv),
};

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

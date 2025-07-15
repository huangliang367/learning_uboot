#ifndef __ASSEMBLY__
struct pl_regs {
    u32 dr;
    u32 resv[5];
    u32 fr;
    u32 resv1[2];
    u32 ibrd;
    u32 fbrd;
    u32 lcrh;
    u32 cr;
    u32 resv2;
    u32 imsc;
};

struct pl_priv {
	struct pl_regs *regs;
};

#define PBASE       0xFE000000
#define GPFSEL1     (PBASE + 0x00200004)
#define GPSET0      (PBASE + 0x0020001C)
#define GPCLR0      (PBASE + 0x00200028)
#define GPPUD       (PBASE + 0x00200094)
#define GPPUDCLK0   (PBASE + 0x00200098)
#define GPIO_PUP_PDN_CNTRL_REG0 (PBASE + 0x002000E4)

#endif /* !__ASSEMBLY__ */
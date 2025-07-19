#include <common.h>
#include <command.h>
#include <hash.h>
#include <linux/ctype.h>

static int do_hello(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    if (argc != 2)
        return CMD_RET_USAGE;

    printf("Hello, %s!\n", argv[1]);

    return 0;
}

U_BOOT_CMD(
	hello,	2,	1,	do_hello,
    "Say hello to the world\n",
    "xxx"
);
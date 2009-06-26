/*
 * ddrdrive.c - scsi driver for DDRdrive PCI Express ramdisk
 * Copyright (c) 2009 Greg Kroah-Hartman <gregkh@suse.de>
 * Copyright (c) 2009 Novell, Inc.
 *
 * This file is released under the GPLv2 only.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static int __init ddrdrive_init(void)
{
	return 0;
}

static void __exit ddrdrive_exit(void)
{
}

module_init(ddrdrive_init);
module_exit(ddrdrive_exit);

MODULE_AUTHOR("Greg Kroah-Hartman <gregkh@suse.de>");
MODULE_DESCRIPTION("DDRdrive scsi driver");
MODULE_LICENSE("GPL");

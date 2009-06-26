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
#include <linux/pci.h>

static struct pci_device_id ids[] = {
	{ PCI_DEVICE(0x19e3, 0xdd52) },
	{ },
};
MODULE_DEVICE_TABLE(pci, ids);

static int __devinit ddr_probe(struct pci_dev *pdev,
			       const struct pci_device_id *id)
{
	return 0;
}

static void __devexit ddr_remove(struct pci_dev *pdev)
{
}

static int ddr_suspend(struct pci_dev *pdev, pm_message_t state)
{
	return 0;
}

static int ddr_resume(struct pci_dev *pdev)
{
	return 0;
}

static struct pci_driver ddr_driver = {
	.name	= KBUILD_MODNAME,
	.id_table	= ids,
	.probe		= ddr_probe,
	.remove		= ddr_remove,
	.suspend	= ddr_suspend,
	.resume		= ddr_resume,
};

static int __init ddrdrive_init(void)
{
	return pci_register_driver(&ddr_driver);
}

static void __exit ddrdrive_exit(void)
{
	pci_unregister_driver(&ddr_driver);
}

module_init(ddrdrive_init);
module_exit(ddrdrive_exit);

MODULE_AUTHOR("Greg Kroah-Hartman <gregkh@suse.de>");
MODULE_DESCRIPTION("DDRdrive scsi driver");
MODULE_LICENSE("GPL");

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

#define PATHID			0x0
#define TARGETID		0x0
#define LUN			0x0

#define SECTOR_SIZE		0x00000200
#define SECTOR_SIZE_BIG_ENDIAN	0x00020000

#define SGL_HOST_ADDRESS_REG	0x00
#define SGL_CARD_READ_REG	0x04
#define SGL_CARD_WRITE_REG	0x08
#define INT_STATUS_REG		0x0C
#define EXT_LED_CONTROL_REG	0x20
#define INT_CONTROL_REG		0x130
#define AC_ADAPTER_REG		0x134
#define LED_YELLOW_CONTROL	0x158
#define LED_RED_CONTROL		0x15C
#define BACKUP_RESTORE_CONTROL	0x210
#define BACKUP_RESTORE_ADDRESS	0x214
#define BACKUP_RESTORE_STATUS	0x218
#define NAND0_ECC_ERROR_TOTAL	0x23C
#define NAND1_ECC_ERROR_TOTAL	0x24C

#define LED_TURN_ON		0xC0000000
#define LED_TURN_OFF		0xA0000000

#define SCSIOP_DDRDRIVE_UNIQUE	0xFF
#define SUBCODE_READ_REGISTER	0x02
#define SUBCODE_WRITE_REGISTER	0x03
#define SUBCODE_BRICK_STATE	0x04
#define SUBCODE_DRIVER_VERSION	0x07
#define SUBCODE_FPGA_REVISION	0x0C
#define SUBCODE_DRIVE_BACKUP	0x11
#define SUBCODE_DRIVE_RESTORE	0x12

#define ANY_INTERRUPT_MASK	0x5500003F
#define BRICK_PWR_ON_MASK	0x40000000
#define BRICK_PWR_OFF_MASK	0x10000000
#define BACKUP_COMPLETE_MASK	0x04000000
#define RESTORE_COMPLETE_MASK	0x01000000
#define SGL_COMPLETION_MASK	0x0000003F

#define	MAX_SGL_QUEUE		0x20		/* 32 SGL's */
#define	SGL_SIZE		0x600		/* 16 Byte Aligned */
#define	MAX_DMA_SIZE		0x10000		/* 64KB */
#define	MAX_DMA_TRANSFERS	0x80000000	/* 256 DMA's */
#define	PHYSICAL_BREAKS		0xFF		/* 255 */
#define	UNCACHED_EXT_SIZE	0xC000		/* MAX_SGL_QUEUE * SGL_SIZE */
#define MAX_TRANSFER_SIZE	0xFF000		/* PHYSICAL_BREAKS * 0x1000 */

/*
 * DMA Group Data Structure - See DDRdrive DMA Engine V8.0 Specification
 */
struct dma_group {
	u16	byte_count1;
	u16	byte_count2;
	u32	host_addr1;
	u32	host_addr2;
};

/*
 * DDRdrive X1 Device Extension
 */
struct ddr {
	void __iomem			*bar0;
	int				pm_cap;
	struct pci_dev			*pdev;
	u8				*virt_sg_list;
	u32				phys_sg_list;
	u32				queue_head;
	u32				queue_tail;
	u32				queue_count;
/*
	PSCSI_REQUEST_BLOCK pSrbQueue[MAX_SGL_QUEUE];
	PSCSI_REQUEST_BLOCK pSrbBackup;
	PSCSI_REQUEST_BLOCK pSrbRestore;
*/
	u32				hba_state;
	u32				brick_state;
	u32				last_sector;
};


static struct pci_device_id ids[] = {
	{ PCI_DEVICE(0x19e3, 0xdd52) },
	{ },
};
MODULE_DEVICE_TABLE(pci, ids);

static void ddr_write32(struct ddr *ddr, u32 off, u32 val)
{
	writel(val, ddr->bar0 + off);
}

static u32 ddr_read32(struct ddr *ddr, u32 off)
{
        return (readl(ddr->bar0 + off));
}

static void ddr_init(struct ddr *ddr)
{
	u32 value;

	value = ddr_read32(ddr, BACKUP_RESTORE_STATUS);
	if (((value & 0x80000000) != 0x0) || ((value & 0x40000000) != 0x0))
		ddr->hba_state = 0x01;
	else
		ddr->hba_state = 0x00;

	ddr->queue_head = 0x00;
	ddr->queue_tail = 0x00;
	ddr->queue_count = 0x00;

	ddr->brick_state = ddr_read32(ddr, AC_ADAPTER_REG) >> 0x1f;
	ddr->last_sector = 0x007BFFFF;

	ddr_write32(ddr, EXT_LED_CONTROL_REG, 0x20000001);
	ddr_write32(ddr, INT_CONTROL_REG, 0xF0000000);
}

static int __devinit ddr_probe(struct pci_dev *pdev,
			       const struct pci_device_id *id)
{
	struct ddr *ddr;
	int pm_cap;
	int error;

	error = pci_enable_device(pdev);
	if (error) {
		dev_err(&pdev->dev, "Can not enable device\n");
		return error;
	}

	error = pci_request_regions(pdev, KBUILD_MODNAME);
	if (error) {
		dev_err(&pdev->dev, "Can not obtain PCI resources\n");
		goto error_disable;
	}

	pci_set_master(pdev);

	pm_cap = pci_find_capability(pdev, PCI_CAP_ID_PM);
	if (pm_cap == 0) {
		dev_err(&pdev->dev, "Can not find power capability\n");
		error = -EIO;
		goto error_free_resource;
	}

	ddr = kzalloc(sizeof(*ddr), GFP_KERNEL);
	if (!ddr) {
		dev_err(&pdev->dev, "Out of memory\n");
		error = -ENOMEM;
		goto error_free_resource;
	}

	ddr->pdev = pdev;
	ddr->pm_cap = pm_cap;

	ddr->bar0 = pci_ioremap_bar(pdev, 0);
	if (!ddr->bar0) {
		dev_err(&pdev->dev, "Can not map registers\n");
		error = -ENOMEM;
		goto error_free_ddr;
	}

	pci_set_drvdata(pdev, ddr);

	ddr_init(ddr);

	return 0;

error_free_ddr:
	kfree(ddr);

error_free_resource:
	pci_release_regions(pdev);

error_disable:
	pci_disable_device(pdev);
	return error;
}

static void __devexit ddr_remove(struct pci_dev *pdev)
{
	struct ddr *ddr = pci_get_drvdata(pdev);

	iounmap(ddr->bar0);
	pci_release_regions(pdev);
	pci_disable_device(pdev);
	pci_set_drvdata(pdev, NULL);
	kfree(ddr);
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

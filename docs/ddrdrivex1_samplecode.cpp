/****************************************************************************
**
** Copyright 2009 DDRdrive, LLC - All rights reserved.
**
** Code Snippets from a Windows SCSI miniport device driver
**
****************************************************************************/

//#define INFOPRINT
//#define SCSIPRINT
//#define ERRORPRINT
//#define QUEUEPRINT
//#define BUSYPRINT
//#define SGLPRINT
//#define BRICKPRINT
//#define SPURIOUSPRINT

#define DRIVER_VERSION	0x07010171

#define PATHID		0x0
#define	TARGETID	0x0
#define	LUN			0x0

#define SECTOR_SIZE				0x00000200
#define SECTOR_SIZE_BIG_ENDIAN	0x00020000

#define SGL_HOST_ADDRESS_REG	0x00
#define SGL_CARD_READ_REG		0x04
#define SGL_CARD_WRITE_REG		0x08
#define INT_STATUS_REG			0x0C
#define EXT_LED_CONTROL_REG		0x20
#define INT_CONTROL_REG			0x130
#define AC_ADAPTER_REG			0x134
#define LED_YELLOW_CONTROL		0x158
#define LED_RED_CONTROL			0x15C
#define BACKUP_RESTORE_CONTROL	0x210
#define BACKUP_RESTORE_ADDRESS	0x214
#define BACKUP_RESTORE_STATUS	0x218
#define NAND0_ECC_ERROR_TOTAL	0x23C
#define NAND1_ECC_ERROR_TOTAL	0x24C

#define LED_TURN_ON		0xC0000000
#define LED_TURN_OFF	0xA0000000

#define SCSIOP_DDRDRIVE_UNIQUE		0xFF
#define SUBCODE_READ_REGISTER		0x02
#define SUBCODE_WRITE_REGISTER		0x03
#define SUBCODE_BRICK_STATE			0x04
#define SUBCODE_DRIVER_VERSION		0x07
#define SUBCODE_FPGA_REVISION		0x0C
#define SUBCODE_DRIVE_BACKUP		0x11
#define SUBCODE_DRIVE_RESTORE		0x12

#define ANY_INTERRUPT_MASK		0x5500003F
#define BRICK_PWR_ON_MASK		0x40000000
#define BRICK_PWR_OFF_MASK		0x10000000
#define BACKUP_COMPLETE_MASK	0x04000000
#define RESTORE_COMPLETE_MASK	0x01000000
#define SGL_COMPLETION_MASK		0x0000003F

#define	MAX_SGL_QUEUE		0x20		// 32 SGL's
#define	SGL_SIZE			0x600		// 16 Byte Aligned
#define	MAX_DMA_SIZE		0x10000		// 64KB
#define	MAX_DMA_TRANSFERS	0x80000000	// 256 DMA's - Windows Limit
#define	PHYSICAL_BREAKS		0xFF		// 255 - Windows Limit
#define	UNCACHED_EXT_SIZE	0xC000		// MAX_SGL_QUEUE * SGL_SIZE
#define MAX_TRANSFER_SIZE	0xFF000		// PHYSICAL_BREAKS * 0x1000 - Windows Limit

//
// DMA Group Data Structure - See DDRdrive DMA Engine V8.0 Specification
//
typedef struct
	{
	USHORT	ByteCount1;
	USHORT	ByteCount2;
	ULONG	HostAdd1;
	ULONG	HostAdd2;
	} *PDMA_GROUP;

//
// DDRdrive X1 Device Extension
//
typedef struct
	{
	PUCHAR				pBar0;
	PUCHAR				pVirtSGList;
	ULONG				PhysSGList;
	ULONG				QueueHead;
	ULONG				QueueTail;
	ULONG				QueueCount;
	PSCSI_REQUEST_BLOCK pSrbQueue[MAX_SGL_QUEUE];
	PSCSI_REQUEST_BLOCK pSrbBackup;
	PSCSI_REQUEST_BLOCK pSrbRestore;
	ULONG				HbaState;
	ULONG				BrickState;
	ULONG				LastSector;
	} DDR_EXT,*PDDR_EXT;


/****************************************************************************
**
**  FUNCTION: DDRdriveInitialize()
**
*****************************************************************************/
BOOLEAN DDRdriveInitialize(IN PVOID DevExt)
	{
	ULONG		status;
	PDDR_EXT	pDevExt;
	ULONG		lastsector;

	pDevExt = (PDDR_EXT)DevExt;

	status = KernelReadRegisterUlong((PULONG)(pDevExt->pBar0 + BACKUP_RESTORE_STATUS));
	if (((status & 0x80000000) != 0x0) || ((status & 0x40000000) != 0x0))
		{
		pDevExt->HbaState = 0x1;
		}
	else { pDevExt->HbaState = 0x0; }

	pDevExt->QueueHead = 0x0;
	pDevExt->QueueTail = 0x0;
	pDevExt->QueueCount = 0x0;

	pDevExt->pSrbQueue[0x00] = 0x0;
	pDevExt->pSrbQueue[0x01] = 0x0;
	pDevExt->pSrbQueue[0x02] = 0x0;
	pDevExt->pSrbQueue[0x03] = 0x0;
	pDevExt->pSrbQueue[0x04] = 0x0;
	pDevExt->pSrbQueue[0x05] = 0x0;
	pDevExt->pSrbQueue[0x06] = 0x0;
	pDevExt->pSrbQueue[0x07] = 0x0;
	pDevExt->pSrbQueue[0x08] = 0x0;
	pDevExt->pSrbQueue[0x09] = 0x0;
	pDevExt->pSrbQueue[0x0A] = 0x0;
	pDevExt->pSrbQueue[0x0B] = 0x0;
	pDevExt->pSrbQueue[0x0C] = 0x0;
	pDevExt->pSrbQueue[0x0D] = 0x0;
	pDevExt->pSrbQueue[0x0E] = 0x0;
	pDevExt->pSrbQueue[0x0F] = 0x0;
	pDevExt->pSrbQueue[0x10] = 0x0;
	pDevExt->pSrbQueue[0x11] = 0x0;
	pDevExt->pSrbQueue[0x12] = 0x0;
	pDevExt->pSrbQueue[0x13] = 0x0;
	pDevExt->pSrbQueue[0x14] = 0x0;
	pDevExt->pSrbQueue[0x15] = 0x0;
	pDevExt->pSrbQueue[0x16] = 0x0;
	pDevExt->pSrbQueue[0x17] = 0x0;
	pDevExt->pSrbQueue[0x18] = 0x0;
	pDevExt->pSrbQueue[0x19] = 0x0;
	pDevExt->pSrbQueue[0x1A] = 0x0;
	pDevExt->pSrbQueue[0x1B] = 0x0;
	pDevExt->pSrbQueue[0x1C] = 0x0;
	pDevExt->pSrbQueue[0x1D] = 0x0;
	pDevExt->pSrbQueue[0x1E] = 0x0;
	pDevExt->pSrbQueue[0x1F] = 0x0;

	pDevExt->pSrbBackup = 0x0;
	pDevExt->pSrbRestore = 0x0;

	pDevExt->BrickState = KernelReadRegisterUlong((PULONG)(pDevExt->pBar0 + AC_ADAPTER_REG)) >> 0x1F;

	pDevExt->LastSector = 0x007BFFFF;

	KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + EXT_LED_CONTROL_REG),0x20000001);

	KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + INT_CONTROL_REG),0xF0000000);

	return TRUE;
	}

/****************************************************************************
**
**  FUNCTION: DDRdriveStartIo()
**
*****************************************************************************/
BOOLEAN DDRdriveStartIo(IN PVOID DevExt,IN PSCSI_REQUEST_BLOCK pSrb)
	{
	ULONG					eax;
	ULONG					edx;
	ULONG					sgl;
	PCDB					pCdb;
	ULONG					status;
	ULONG					dmalba;
	ULONG					subcode;
	ULONG					command;
	ULONG					physadd;
	PDDR_EXT				pDevExt;
	ULONG					regvalue;
	PDMA_GROUP				pDmaGroup;
	ULONG					brt_entry;
	ULONG					regoffset;
	ULONG					seglength;
	ULONG					lastsector;
	ULONG					tranlength;
	PULONG					validator0;
	PULONG					validator1;
	PUCHAR					pByteBuffer;
	PUCHAR					pTranBuffer;
	SCSI_PHYSICAL_ADDRESS	ScsiPhysAdd;
	PULONG					pDwordBuffer;
	ULONG					firstaddress;
	ULONG					prom_address;
	ULONG					bar_register;
	ULONG					allocation_length;
	ULONG					cdb_transfer_blocks;
	ULONG					cdb_transfer_length;
	ULONG					lastsector_big_endian;

	pDevExt = (PDDR_EXT)DevExt;

	if (pSrb->Function == SRB_FUNCTION_EXECUTE_SCSI)
		{
		command = pSrb->Cdb[0x0];
		if ((pSrb->PathId == PATHID) && (pSrb->TargetId == TARGETID))
			{
			if (command == SCSIOP_READ || command == SCSIOP_WRITE)
				{
				if (pSrb->Lun == LUN)
					{
					if (pDevExt->HbaState == 0x0) 
						{
						#ifdef ERRORPRINT
						if (pDevExt->QueueCount > 0x1F) { DbgPrint("\nDDRdriveStartIo: MAX_SGL_QUEUE Error! QueueCount=0x%x\n\n",pDevExt->QueueCount); }
						#endif
						#ifdef SCSIPRINT
						if (pSrb->Cdb[0x9] != 0x00) { DbgPrint("SCSIOP_READ/WRITE: Control NOT Zero! 0x%x\n",pSrb->Cdb[0x9]); }
						#endif

						cdb_transfer_blocks = pSrb->Cdb[0x8] | (pSrb->Cdb[0x7] << 0x8);

						if (cdb_transfer_blocks > 0x0)
							{
							dmalba = pSrb->Cdb[0x5] | (pSrb->Cdb[0x4] << 0x8) | (pSrb->Cdb[0x3] << 0x10) | (pSrb->Cdb[0x2] << 0x18);

							if ((dmalba + cdb_transfer_blocks - 0x1) > pDevExt->LastSector)
								{
								#ifdef ERRORPRINT
								DbgPrint("DDRdriveStartIo: Transfer beyond lastsector! Beginning LBA=0x%x,Transfer Blocks=0x%x\n",dmalba,cdb_transfer_blocks);
								#endif
								FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_ILLEGAL_BLOCK,0x0);
								goto KernelNotification;
								}

							cdb_transfer_length = cdb_transfer_blocks << 0x9;

							if (cdb_transfer_length > pSrb->DataTransferLength) 
								{
								#ifdef ERRORPRINT
								DbgPrint("DDRdriveStartIo: Data Overrun! DataTransferLength=0x%x,cdb_transfer_length=0x%x\n",pSrb->DataTransferLength,cdb_transfer_length);
								#endif
								pSrb->SrbStatus = SRB_STATUS_DATA_OVERRUN;
								goto KernelNotification;
								}

							#ifdef SGLPRINT
							if (command == SCSIOP_READ) { DbgPrint("SCSIOP_READ: pSrb->DataTransferLength=0x%x\n",pSrb->DataTransferLength); }
							else { DbgPrint("SCSIOP_WRITE: pSrb->DataTransferLength=0x%x\n",pSrb->DataTransferLength); }
							#endif

							firstaddress = 0x1;
							pTranBuffer = pSrb->DataBuffer;
							tranlength = cdb_transfer_length;
							pDmaGroup = (PDMA_GROUP)(pDevExt->pVirtSGList + (pDevExt->QueueTail * SGL_SIZE));

							do
								{
								ScsiPhysAdd = KernelGetPhysicalAddress(pDevExt,pSrb,pTranBuffer,&seglength);

								if (ScsiPhysAdd.HighPart != 0x0)
									{
									#ifdef ERRORPRINT
									DbgPrint("DDRdriveStartIo: Physical address is greater than 4GB! HighPart=0x%x/LowPart=0x%x\n",ScsiPhysAdd.u.HighPart,ScsiPhysAdd.u.LowPart);
									#endif
									pSrb->SrbStatus = SRB_STATUS_INVALID_REQUEST;
									goto KernelNotification;
									}
								if (ScsiPhysAdd.LowPart == 0x0)
									{
									#ifdef ERRORPRINT
									DbgPrint("DDRdriveStartIo: Failed to convert virtual address to physical address!\n");
									#endif
									pSrb->SrbStatus = SRB_STATUS_INVALID_REQUEST;
									goto KernelNotification;
									}

								physadd = ScsiPhysAdd.LowPart;

								if (seglength > 0x0)
									{
									if (seglength > tranlength)
										{
										seglength = tranlength;
										tranlength = 0x0;
										}
									else
										{
										tranlength = tranlength - seglength;
										pTranBuffer = pTranBuffer + seglength;
										}

									do 
										{
										if (dmalba < MAX_DMA_TRANSFERS)
											{
											if (seglength >= MAX_DMA_SIZE)
												{
												if (firstaddress == 0x1)
													{
													pDmaGroup->ByteCount1 = 0x0;
													pDmaGroup->HostAdd1 = physadd;

													#ifdef SGLPRINT
													DbgPrint("DDRdriveStartIo: ByteCount1=0x10000/HostAdd1=0x%x/DmaTransfers=0x%x\n",pDmaGroup->HostAdd1,dmalba >> 0x17);
													#endif

													firstaddress = 0x0;
													}
												else
													{
													pDmaGroup->ByteCount2 = 0x0;
													pDmaGroup->HostAdd2 = physadd;

													#ifdef SGLPRINT
													DbgPrint("DDRdriveStartIo: ByteCount2=0x10000/HostAdd2=0x%x/DmaTransfers=0x%x\n",pDmaGroup->HostAdd2,dmalba >> 0x17);
													#endif

													pDmaGroup = pDmaGroup + 0x1;
													firstaddress = 0x1;
													}

												physadd = physadd + MAX_DMA_SIZE;
												seglength = seglength - MAX_DMA_SIZE;
												}
											else
												{
												if (firstaddress == 0x1)
													{
													pDmaGroup->ByteCount1 = (USHORT) seglength;
													pDmaGroup->HostAdd1 = physadd;

													#ifdef SGLPRINT
													DbgPrint("DDRdriveStartIo: ByteCount1=0x%x/HostAdd1=0x%x/DmaTransfers=0x%x\n",pDmaGroup->ByteCount1,pDmaGroup->HostAdd1,dmalba >> 0x17);
													#endif

													firstaddress = 0x0;
													}
												else
													{
													pDmaGroup->ByteCount2 = (USHORT) seglength;
													pDmaGroup->HostAdd2 = physadd;

													#ifdef SGLPRINT
													DbgPrint("DDRdriveStartIo: ByteCount2=0x%x/HostAdd2=0x%x/DmaTransfers=0x%x\n",pDmaGroup->ByteCount2,pDmaGroup->HostAdd2,dmalba >> 0x17);
													#endif

													pDmaGroup = pDmaGroup + 0x1;
													firstaddress = 0x1;
													}

												seglength = 0x0;
												}

											dmalba = dmalba + 0x00800000;
											}
										else
											{
											#ifdef ERRORPRINT
											DbgPrint("DDRdriveStartIo: DmaTransfers > MAX_DMA_TRANSFERS! 0x%x\n",dmalba >> 0x17);
											#endif
											FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_CDB,0x0);
											goto KernelNotification;
											}
										}
									while (seglength > 0x0);
									}
								}
							while (tranlength > 0x0);

							#ifdef ERRORPRINT
							if ((dmalba >> 0x17) == 0x0) { DbgPrint("DDRdriveStartIo: Dma Transfers = 0x0!\n"); }
							#endif

							sgl = pDevExt->PhysSGList + (pDevExt->QueueTail * SGL_SIZE);
							KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + SGL_HOST_ADDRESS_REG),sgl);
							if (command == SCSIOP_READ)
								{
								KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + SGL_CARD_READ_REG),dmalba);
								}
							else
								{
								KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + SGL_CARD_WRITE_REG),dmalba);
								}

							pDevExt->pSrbQueue[pDevExt->QueueTail] = pSrb;
							pDevExt->QueueTail = (pDevExt->QueueTail + 0x1) & 0x1F;
							pDevExt->QueueCount = pDevExt->QueueCount + 0x1;

							#ifdef SGLPRINT
							DbgPrint("DDRdriveStartIo: Start SRB! pSRB=0x%x/SrbFlags=0x%x/QueueTag=0x%x/QueueAction=0x%x/QueueCount=0x%x\n",pSrb,pSrb->SrbFlags,pSrb->QueueTag,pSrb->QueueAction,pDevExt->QueueCount);
							#endif

							if (cdb_transfer_length < pSrb->DataTransferLength)
								{
								pSrb->DataTransferLength = cdb_transfer_length;
								pSrb->SrbStatus = SRB_STATUS_DATA_OVERRUN;
								}
							else { pSrb->SrbStatus = SRB_STATUS_SUCCESS; }

							pSrb->SenseInfoBufferLength = 0x0;
							pSrb->ScsiStatus = SCSISTAT_GOOD;

							if (pDevExt->QueueCount < 0x20) { KernelNotification(NextLuRequest,pDevExt,PATHID,TARGETID,LUN); }

							return TRUE;
							}
						else
							{
							#ifdef SCSIPRINT
							DbgPrint("DDRdriveStartIo: cdb_transfer_blocks = 0x0!\n");
							#endif
							pSrb->DataTransferLength = 0x0;
							pSrb->SenseInfoBufferLength = 0x0;
							pSrb->SrbStatus = SRB_STATUS_SUCCESS;
							pSrb->ScsiStatus = SCSISTAT_GOOD;
							}
						}
					else
						{
						#ifdef BUSYPRINT
						DbgPrint("\nDDRdriveStartIo: SRB_STATUS_BUSY - Scsi Command=0x%x\n\n",command);
						#endif
						pSrb->SrbStatus = SRB_STATUS_BUSY;
						KernelNotification(RequestComplete,pDevExt,pSrb);
						return TRUE;
						}
					}
				else { //FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_LUN,0x0); }
				}
			else if (command == SCSIOP_DDRDRIVE_UNIQUE)
				{
				if (pSrb->Lun == LUN)
					{
					validator0 = (PULONG) &pSrb->Cdb[0x8];
					validator1 = (PULONG) &pSrb->Cdb[0xC];

					subcode = pSrb->Cdb[0x1];

					if ((*validator0 != 0x44445244) || (*validator1 != 0x00495645))
						{
						#ifdef ERRORPRINT
						DbgPrint("SCSIOP_DDRDRIVE_UNIQUE: Validator Failed! V0=0x%x/V1=0x%x/Subcode=0x%x\n",*validator0,*validator1,subcode);
						#endif
						goto UnsupportedVU;
						}

					if (subcode == SUBCODE_BRICK_STATE)
						{
						#ifdef BRICKPRINT
						DbgPrint("SCSIOP_DDRDRIVE_UNIQUE: SUBCODE_BRICK_STATE State=0x%x\n",pDevExt->BrickState);
						#endif

						pDwordBuffer = pSrb->DataBuffer;
						pDwordBuffer[0x0] = pDevExt->BrickState;

						pSrb->DataTransferLength = 0x4;
						}
					else if (subcode == SUBCODE_READ_REGISTER)
						{
						regoffset = pSrb->Cdb[0x2] | (pSrb->Cdb[0x3] << 0x8);

						pDwordBuffer = pSrb->DataBuffer;
						pDwordBuffer[0x0] = KernelReadRegisterUlong((PULONG)(pDevExt->pBar0 + regoffset));

						#ifdef SCSIPRINT
						DbgPrint("SCSIOP_DDRDRIVE_UNIQUE: SUBCODE_READ_REGISTER register=0x%x/value=0x%x\n",regoffset,pDwordBuffer[0x0]);
						#endif

						pSrb->DataTransferLength = 0x4;
						}
					else if (subcode == SUBCODE_WRITE_REGISTER)
						{
						regoffset = pSrb->Cdb[0x2] | (pSrb->Cdb[0x3] << 0x8);
						regvalue = pSrb->Cdb[0x4] | (pSrb->Cdb[0x5] << 0x8) | (pSrb->Cdb[0x6] << 0x10) | (pSrb->Cdb[0x7] << 0x18);

						#ifdef SCSIPRINT
						DbgPrint("SCSIOP_DDRDRIVE_UNIQUE: SUBCODE_WRITE_REGISTER register=0x%x/value=0x%x\n",regoffset,regvalue);
						#endif

						KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + regoffset),regvalue);

						pSrb->DataTransferLength = 0x0;
						}
					else if (subcode == SUBCODE_DRIVE_BACKUP)
						{
						#ifdef SCSIPRINT
						DbgPrint("SCSIOP_DDRDRIVE_UNIQUE: SUBCODE_DRIVE_BACKUP\n");
						#endif

						pSrb->DataTransferLength = 0x4;

						if (pDevExt->HbaState == 0x0)
							{
							if (pDevExt->QueueCount == 0x0)
								{
								pDevExt->HbaState = 0x1;

								KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + LED_RED_CONTROL),LED_TURN_ON);

								UpdateMapCount(pDevExt,PROM_BACKUP_COUNT_ADDRESS);

								KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + PROM_ADDRESS_REG),PROM_RESTORE_COUNT_ADDRESS);
								KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + PROM_CONTROL_REG),PROM_ERASE_SECTOR_CMD);
								do
									{
									status = KernelReadRegisterUlong((PULONG)(pDevExt->pBar0 + PROM_STATUS_REG));
									}
								while (status != 0x0);

								KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + BACKUP_RESTORE_ADDRESS),0x0);
								KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + BACKUP_RESTORE_CONTROL),0x401F0000);

								pSrb->SenseInfoBufferLength = 0x0;
								pSrb->SrbStatus = SRB_STATUS_SUCCESS;
								pSrb->ScsiStatus = SCSISTAT_GOOD;

								pDevExt->pSrbBackup = pSrb;
								return TRUE;
								}
							else
								{
								#ifdef SCSIPRINT
								DbgPrint("SUBCODE_DRIVE_BACKUP: pDevExt->QueueCount > 0x0\n");
								#endif
								pDwordBuffer = pSrb->DataBuffer;
								pDwordBuffer[0x0] = 0x10;
								}
							}
						else
							{
							#ifdef SCSIPRINT
							DbgPrint("SUBCODE_DRIVE_BACKUP: HbaState != 0x0\n");
							#endif
							pDwordBuffer = pSrb->DataBuffer;
							pDwordBuffer[0x0] = 0x2;
							}
						}
					else if (subcode == SUBCODE_DRIVE_RESTORE)
						{
						#ifdef SCSIPRINT
						DbgPrint("SCSIOP_DDRDRIVE_UNIQUE: SUBCODE_DRIVE_RESTORE\n");
						#endif

						pSrb->DataTransferLength = 0xC;

						if (pDevExt->HbaState == 0x0)
							{
							if (pDevExt->QueueCount == 0x0)
								{
								pDevExt->HbaState = 0x1;

								KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + LED_YELLOW_CONTROL),LED_TURN_ON);

								UpdateMapCount(pDevExt,PROM_RESTORE_COUNT_ADDRESS);

								KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + BACKUP_RESTORE_ADDRESS),0x0);
								KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + BACKUP_RESTORE_CONTROL),0x801F0000);

								pSrb->SenseInfoBufferLength = 0x0;
								pSrb->SrbStatus = SRB_STATUS_SUCCESS;
								pSrb->ScsiStatus = SCSISTAT_GOOD;

								pDevExt->pSrbRestore = pSrb;
								return TRUE;
								}
							else
								{
								#ifdef SCSIPRINT
								DbgPrint("SUBCODE_DRIVE_RESTORE: pDevExt->QueueCount > 0x0\n");
								#endif
								pDwordBuffer = pSrb->DataBuffer;
								pDwordBuffer[0x0] = 0x10;
								pDwordBuffer[0x1] = 0x0;
								pDwordBuffer[0x2] = 0x0;
								}
							}
						else
							{
							#ifdef SCSIPRINT
							DbgPrint("SUBCODE_DRIVE_RESTORE: HbaState != 0x0\n");
							#endif
							pDwordBuffer = pSrb->DataBuffer;
							pDwordBuffer[0x0] = 0x2;
							pDwordBuffer[0x1] = 0x0;
							pDwordBuffer[0x2] = 0x0;
							}
						}
					else if (subcode == SUBCODE_DRIVER_VERSION)
						{
						pDwordBuffer = pSrb->DataBuffer;
						pDwordBuffer[0x0] = DRIVER_VERSION;

						#ifdef SCSIPRINT
						DbgPrint("SCSIOP_DDRDRIVE_UNIQUE: SUBCODE_DEVICE_DRIVER 0x%x\n",pDwordBuffer[0x0]);
						#endif

						pSrb->DataTransferLength = 0x4;
						}
					else if (subcode == SUBCODE_FPGA_REVISION)
						{
						#ifdef SCSIPRINT
						DbgPrint("SCSIOP_DDRDRIVE_UNIQUE: SUBCODE_FPGA_REVISION 0x%x\n",pDevExt->RevisionLevel);
						#endif

						pDwordBuffer = pSrb->DataBuffer;
						pDwordBuffer[0x0] = pDevExt->RevisionLevel;

						pSrb->DataTransferLength = 0x4;
						}
					else
						{
						#ifdef SCSIPRINT
						DbgPrint("SCSIOP_DDRDRIVE_UNIQUE: Unsupported DDRdrive Subcode 0x%x *********************************\n",subcode);
						#endif
						UnsupportedVU:
						//FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_ILLEGAL_COMMAND,0x0);
						goto KernelNotification;
						}

					pSrb->SenseInfoBufferLength = 0x0;
					pSrb->SrbStatus = SRB_STATUS_SUCCESS;
					pSrb->ScsiStatus = SCSISTAT_GOOD;
					}
				else { //FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_LUN,0x0); }
				}		
			else
				{
				#ifdef SCSIPRINT
				DbgPrint("DDRdriveStartIo: Unsupported SCSI Command 0x%x *********************************\n",command);
				#endif
				//FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_ILLEGAL_COMMAND,0x0);
				}
			}
		else
			{
			#ifdef SCSIPRINT
			DbgPrint("DDRdriveStartIo: SRB_FUNCTION_EXECUTE_SCSI received invalid SCSI PathID/TargetID(0x%x/0x%x). Requested SCSI Command 0x%x\n",pSrb->PathId,pSrb->TargetId,command);
			#endif
			pSrb->SrbStatus = SRB_STATUS_SELECTION_TIMEOUT;
			}
		}
	else if (pSrb->Function == SRB_FUNCTION_RESET_BUS)
		{
		#ifdef SCSIPRINT
		DbgPrint("DDRdriveStartIo: Processing SRB_FUNCTION_RESET_BUS!\n");
		#endif
		status = KernelResetBus(pDevExt,pSrb->PathId);
		if (status == TRUE) { pSrb->SrbStatus = SRB_STATUS_SUCCESS; }
		else { pSrb->SrbStatus = SRB_STATUS_SELECTION_TIMEOUT; }
		KernelNotification(RequestComplete,pDevExt,pSrb);
		return TRUE;
		}
	else
		{
		#ifdef SCSIPRINT
		DbgPrint("DDRdriveStartIo: Unsupported SRB Function 0x%x *********************************\n",pSrb->Function);
		#endif
		pSrb->SrbStatus = SRB_STATUS_INVALID_REQUEST;
		}

	KernelNotification:
	KernelNotification(RequestComplete,pDevExt,pSrb);
	KernelNotification(NextLuRequest,pDevExt,PATHID,TARGETID,LUN);

	return TRUE;
	}

/****************************************************************************
**
**  FUNCTION: DDRdriveInterrupt()
**
*****************************************************************************/
BOOLEAN DDRdriveInterrupt(IN PVOID DevExt)
	{
	PSCSI_REQUEST_BLOCK	pSrb;
	ULONG				pcibar;
	ULONG				status;
	ULONG				result;
	PDDR_EXT			pDevExt;
	ULONG				queuecount;
	ULONG				completions;
	PULONG				pDwordBuffer;

	pDevExt = (PDDR_EXT)DevExt;

	status = KernelReadRegisterUlong((PULONG)(pDevExt->pBar0 + INT_STATUS_REG));

	result = status & ANY_INTERRUPT_MASK;
	if (result == 0x0)
		{
		#ifdef SPURIOUSPRINT
		DbgPrint("DDRdriveInterrupt: Received a spurious interrupt. status=0x%x\n",status);
		#endif
		return FALSE;
		}

	completions = status & SGL_COMPLETION_MASK;
	if (completions > pDevExt->QueueCount)
		{
		#ifdef ERRORPRINT
		DbgPrint("DDRdriveInterrupt: #completed > #queued! Completions=0x%x Queued=0x%x.\n",completions,pDevExt->QueueCount);
		#endif
		return FALSE;
		}

	result = status << 0x8;
	if (status & BRICK_PWR_ON_MASK)
		{
		result = result | 0x80000000;
		}
	if (status & BRICK_PWR_OFF_MASK)
		{
		result = result | 0x20000000;
		}
	if (status & BACKUP_COMPLETE_MASK)
		{
		result = result | 0x08000000;
		}
	if (status & RESTORE_COMPLETE_MASK)
		{
		result = result | 0x02000000;
		}
	KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + INT_STATUS_REG),result);

	queuecount = pDevExt->QueueCount;

	while (completions > 0x0)
		{
		pSrb = pDevExt->pSrbQueue[pDevExt->QueueHead];
		pDevExt->QueueHead = (pDevExt->QueueHead + 0x1) & 0x1F;
		pDevExt->QueueCount = pDevExt->QueueCount - 0x1;

		#ifdef SGLPRINT
		DbgPrint("DDRdriveInterrupt: SRB Completion! pSRB=0x%x/QueueCount=0x%x/PreCompletions=0x%x\n",pSrb,pDevExt->QueueCount,completions);
		#endif

		KernelNotification(RequestComplete,pDevExt,pSrb);
		completions = completions - 0x1;
		}

	if (queuecount == 0x20) { KernelNotification(NextLuRequest,pDevExt,PATHID,TARGETID,LUN); }

	if (status & BRICK_PWR_ON_MASK)
		{
		#ifdef INFOPRINT
		DbgPrint("DDRdriveInterrupt: BRICK_PWR_ON. status=0x%x\n",status);
		#endif
		pDevExt->BrickState = 0x1;
		}
	if (status & BRICK_PWR_OFF_MASK)
		{
		#ifdef INFOPRINT
		DbgPrint("DDRdriveInterrupt: BRICK_PWR_OFF. status=0x%x\n",status);
		#endif
		pDevExt->BrickState = 0x0;
		}
	if (status & BACKUP_COMPLETE_MASK)
		{
		#ifdef INFOPRINT
		DbgPrint("DDRdriveInterrupt: BACKUP_COMPLETE. status=0x%x\n",status);
		#endif

		pDevExt->HbaState = 0x0;
		KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + LED_RED_CONTROL),LED_TURN_OFF);

		if (pDevExt->pSrbBackup != 0x0)
			{
			pSrb = pDevExt->pSrbBackup;

			pDwordBuffer = pSrb->DataBuffer;
			pDwordBuffer[0x0] = 0x0;

			pcibar = KernelReadRegisterUlong((PULONG)(pDevExt->pBar0 + BACKUP_RESTORE_STATUS));
			if ((pcibar & 0x20000000) != 0x0)
				{
				#ifdef ERRORPRINT
				DbgPrint("SUBCODE_DRIVE_BACKUP: Backup Error - BRT0 Overflow!\n");
				#endif
				pDwordBuffer[0x0] = pDwordBuffer[0x0] | 0x00000004;
				}
			if ((pcibar & 0x10000000) != 0x0)
				{
				#ifdef ERRORPRINT
				DbgPrint("SUBCODE_DRIVE_BACKUP: Backup Error - BRT1 Overflow!\n");
				#endif
				pDwordBuffer[0x0] = pDwordBuffer[0x0] | 0x00000008;
				}

			KernelNotification(RequestComplete,pDevExt,pSrb);
			pDevExt->pSrbBackup = 0x0;
			KernelNotification(NextLuRequest,pDevExt,PATHID,TARGETID,LUN);
			}
		else
			{
			#ifdef ERRORPRINT
			DbgPrint("DDRdriveInterrupt: pDevExt->pSrbBackup = 0x0!\n");
			#endif
			}
		}
	if (status & RESTORE_COMPLETE_MASK)
		{
		#ifdef INFOPRINT
		DbgPrint("DDRdriveInterrupt: RESTORE_COMPLETE. status=0x%x\n",status);
		#endif

		pDevExt->HbaState = 0x0;
		KernelWriteRegisterUlong((PULONG)(pDevExt->pBar0 + LED_YELLOW_CONTROL),LED_TURN_OFF);

		if (pDevExt->pSrbRestore != 0x0)
			{
			pSrb = pDevExt->pSrbRestore;

			pDwordBuffer = pSrb->DataBuffer;
			pDwordBuffer[0x0] = 0x0;
			pDwordBuffer[0x1] = KernelReadRegisterUlong((PULONG)(pDevExt->pBar0 + NAND0_ECC_ERROR_TOTAL));
			pDwordBuffer[0x2] = KernelReadRegisterUlong((PULONG)(pDevExt->pBar0 + NAND1_ECC_ERROR_TOTAL));

			if ((pDwordBuffer[0x1] & 0xFFFF0000) != 0x0)
				{
				#ifdef ERRORPRINT
				DbgPrint("SUBCODE_DRIVE_RESTORE: NAND0 2bit Error!\n");
				#endif
				KernelLogError(pDevExt,0x0,PATHID,TARGETID,LUN,SP_INTERNAL_ADAPTER_ERROR,pDevExt->SerialNumber);
				}
			if ((pDwordBuffer[0x2] & 0xFFFF0000) != 0x0)
				{
				#ifdef ERRORPRINT
				DbgPrint("SUBCODE_DRIVE_RESTORE: NAND1 2bit Error!\n");
				#endif
				KernelLogError(pDevExt,0x0,PATHID,TARGETID,LUN,SP_INTERNAL_ADAPTER_ERROR,pDevExt->SerialNumber);
				}

			KernelNotification(RequestComplete,pDevExt,pSrb);
			pDevExt->pSrbRestore = 0x0;
			KernelNotification(NextLuRequest,pDevExt,PATHID,TARGETID,LUN);
			}
		else
			{
			#ifdef ERRORPRINT
			DbgPrint("DDRdriveInterrupt: pDevExt->pSrbRestore = 0x0!\n");
			#endif
			}
		}

	return TRUE;
	}

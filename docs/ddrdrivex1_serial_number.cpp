/****************************************************************************
**
** Copyright 2009 DDRdrive, LLC - All rights reserved.
**
** Filename: ddrdrivex1_serial_number.c
**			
****************************************************************************/

//
//  Code Snippet
//
	ULONG status;

	ScsiPortWriteRegisterUlong((PULONG)(pDevExt->pBar0 + 0x2C),0x001C0000);
	ScsiPortWriteRegisterUlong((PULONG)(pDevExt->pBar0 + 0x24),0x08000000);
	do
		{
		status = ScsiPortReadRegisterUlong((PULONG)(pDevExt->pBar0 + 0x28));
		}
	while (status != 0x0);

	pDevExt->SerialNumber = ScsiPortReadRegisterUlong((PULONG)(pDevExt->pBar0 + 0x30));



//
//  Code Snippet
//
			else if (command == SCSIOP_INQUIRY)
				{
				#ifdef SCSIPRINT
				DbgPrint("HwScsiStartIo: SCSIOP_INQUIRY  LUN=0x%x\n",pSrb->Lun);
				#endif

				pCdb = (PCDB)(&pSrb->Cdb[0x0]);

				#ifdef SCSIPRINT
				DbgPrint("SCSIOP_INQUIRY: EVPD=0x%x\n",pCdb->CDB6INQUIRY3.EnableVitalProductData);
				DbgPrint("SCSIOP_INQUIRY: PAGECODE=0x%x\n",pCdb->CDB6INQUIRY3.PageCode);
				DbgPrint("SCSIOP_INQUIRY: (MSB) ALLOCATION LENGTH=0x%x\n",pCdb->CDB6INQUIRY3.Reserved2);
				DbgPrint("SCSIOP_INQUIRY: ALLOCATION LENGTH (LSB)=0x%x\n",pCdb->CDB6INQUIRY3.AllocationLength);
				DbgPrint("SCSIOP_INQUIRY: CONTROL=0x%x\n",pCdb->CDB6INQUIRY3.Control);
				#endif

				if (pCdb->CDB6INQUIRY3.EnableVitalProductData == 0x0)
					{
					if (pCdb->CDB6INQUIRY3.PageCode == 0x00)
						{
						if (pCdb->CDB6INQUIRY3.AllocationLength >= 0x24)
							{
							// SPC3r23 - Standard Inquiry Data
							//
							// Byte00:  PERIPHERAL QUALIFIER[7:5]=0x0 or 0x3 / PERIPHERAL DEVICE TYPE[4:0]=0x0 or 0x1F
							// Byte01:  RMB[7]=0x0
							// Byte02:  VERSION[7:0]=0x5
							// Byte03:  NormACA[5]=0x0 / HiSup[4]=0x1 / RESPONSE DATA TYPE[3:0]=0x2
							// Byte04:  ADDITIONAL LENGTH[7:0]=(n-4) (31 bytes)
							// Byte05:  SCCS[7]=0x0 / ACC[6]=0x0 / TPGS[5:4]=0x0 / 3PC[3]=0x0 / PROTECT[0]=0x0
							// Byte06:  BQue[7]=0x0 / EncServ[6]=0x0 / MultiP[4]=0x0 / MChngr[3]=0x0
							// Byte07:  LINKED[3]=0x0 / CmdQue[1]=0x1
							// Byte08(MSB)-Byte15(LSB):  T10 VENDOR IDENTIFICATION = DDRDRIVE
							// Byte16(MSB)-Byte31(LSB):  PRODUCT IDENTIFICATION = X1
							// Byte32(MSB)-Byte35(LSB):  PRODUCT REVISION LEVEL = 0008

							pDwordBuffer = pSrb->DataBuffer;

							if (pSrb->Lun == LUN) { pDwordBuffer[0x0] = 0x12050000; }
							else { pDwordBuffer[0x0] = 0x1205007F; }
							pDwordBuffer[0x1] = 0x0200001F;
							pDwordBuffer[0x2] = 0x44524444;  // DRDD
							pDwordBuffer[0x3] = 0x45564952;  // EVIR
							pDwordBuffer[0x4] = 0x20203158;  // 1X
							pDwordBuffer[0x5] = 0x20202020;
							pDwordBuffer[0x6] = 0x20202020;
							pDwordBuffer[0x7] = 0x20202020;
							edx = pDevExt->SubSystem;
							eax = edx >> 0xC;
							if (eax < 0xA) { eax = eax + 0x30; }
							else { eax = eax + 0x37; }
							pDwordBuffer[0x8] = eax;
							eax = (edx >> 0x8) & 0x0000000F;
							if (eax < 0xA) { eax = eax + 0x30; }
							else { eax = eax + 0x37; }
							pDwordBuffer[0x8] = pDwordBuffer[0x8] | (eax << 0x8);
							eax = (edx >> 0x4) & 0x0000000F;
							if (eax < 0xA) { eax = eax + 0x30; }
							else { eax = eax + 0x37; }
							pDwordBuffer[0x8] = pDwordBuffer[0x8] | (eax << 0x10);
							edx = edx & 0x0000000F;
							if (edx < 0xA) { edx = edx + 0x30; }
							else { edx = edx + 0x37; }
							pDwordBuffer[0x8] = pDwordBuffer[0x8] | (edx << 0x18);

							pSrb->DataTransferLength = 0x24;

							if (pCdb->CDB6INQUIRY3.AllocationLength >= 0x40)
								{
								pDwordBuffer[0x9] = 0x00000000;
								pDwordBuffer[0xA] = 0x00000000;
								pDwordBuffer[0xB] = 0x00000000;
								pDwordBuffer[0xC] = 0x00000000;
								pDwordBuffer[0xD] = 0x00000000;

								pByteBuffer = pSrb->DataBuffer;

								pByteBuffer[0x38] = 0x00;
								pByteBuffer[0x39] = 0x00;

								// SPC3r23 - Standard Inquiry Data
								//
								// Byte58(MSB):  VERSION DESCRIPTOR 1 = SAM3r14
								// Byte59(LSB):  VERSION DESCRIPTOR 1 = SAM3r14
								// Byte60(MSB):  VERSION DESCRIPTOR 2 = SPC3r22 (Closest to actual SPC3r23)
								// Byte61(LSB):  VERSION DESCRIPTOR 2 = SPC3r22 (Closest to actual SPC3r23)
								// Byte62(MSB):  VERSION DESCRIPTOR 3 = SBC2r16
								// Byte63(LSB):  VERSION DESCRIPTOR 3 = SBC2r16

								pByteBuffer[0x3A] = 0x00;
								pByteBuffer[0x3B] = 0x76;
								pByteBuffer[0x3C] = 0x03;
								pByteBuffer[0x3D] = 0x0F;
								pByteBuffer[0x3E] = 0x03;
								pByteBuffer[0x3F] = 0x3B;

								pByteBuffer[0x4] = 0x3B;		// Byte04:  ADDITIONAL LENGTH[7:0]=(n-4) (59 bytes)

								pSrb->DataTransferLength = 0x40;
								}

							pSrb->SenseInfoBufferLength = 0x0;
							pSrb->SrbStatus = SRB_STATUS_SUCCESS;
							pSrb->ScsiStatus = SCSISTAT_GOOD;
							}
						else { FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_CDB,0x0); }
						}
					else { FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_CDB,0x0); }
					}
				else
					{
					if (pCdb->CDB6INQUIRY3.PageCode == 0x00)
						{
						if (pCdb->CDB6INQUIRY3.AllocationLength >= 0x7)
							{
							pByteBuffer = pSrb->DataBuffer;
							// SPC3r23 - Supported VPD Pages
							// Byte0:  PERIPHERAL QUALIFIER[7:5]=0x0 or 0x3 / PERIPHERAL DEVICE TYPE[4:0]=0x0 or 0x1F
							if (pSrb->Lun == LUN) { pByteBuffer[0x0] = 0x00; }
							else { pByteBuffer[0x0] = 0x7F; }
							pByteBuffer[0x1] = 0x00;			// Byte1:  PAGE CODE[7:0]=0x00
							pByteBuffer[0x2] = 0x00;			// Byte2:  RESERVED[7:0]=0x00
							pByteBuffer[0x3] = 0x03;			// Byte3:  PAGE LENGTH[7:0]=(n-3)
							pByteBuffer[0x4] = 0x00;			// Byte4:  Supported VPD Page
							pByteBuffer[0x5] = 0x80;			// Byte5:  Unit Serial Number VPD Page
							pByteBuffer[0x6] = 0x83;			// Byte6:  Device Identification VPD Page

							pSrb->DataTransferLength = 0x7;
							pSrb->SenseInfoBufferLength = 0x0;
							pSrb->SrbStatus = SRB_STATUS_SUCCESS;
							pSrb->ScsiStatus = SCSISTAT_GOOD;
							}
						else { FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_CDB,0x0); }
						}
					else if (pCdb->CDB6INQUIRY3.PageCode == 0x80)
						{
						if (pCdb->CDB6INQUIRY3.AllocationLength >= 0xC)
							{
							pByteBuffer = pSrb->DataBuffer;
							// SPC3r23 - Unit Serial Number VPD Page
							// Byte00:  PERIPHERAL QUALIFIER[7:5]=0x0 or 0x3 / PERIPHERAL DEVICE TYPE[4:0]=0x0 or 0x1F
							if (pSrb->Lun == LUN) { pByteBuffer[0x0] = 0x00; }
							else { pByteBuffer[0x0] = 0x7F; }
							pByteBuffer[0x1] = 0x80;			// Byte01:  PAGE CODE[7:0]=0x80
							pByteBuffer[0x2] = 0x00;			// Byte02:  RESERVED[7:0]=0x00
							pByteBuffer[0x3] = 0x08;			// Byte03:  PAGE LENGTH[7:0]=(n-3)
							edx = pDevExt->SerialNumber;
							eax = edx >> 0x1C;
							if (eax < 0xA) { eax = eax + 0x30; }
							else { eax = eax + 0x37; }
							pByteBuffer[0x4] = (UCHAR) eax;		// Byte04:  Serial Number[31:28] Ascii
							eax = (edx >> 0x18) & 0x0000000F;
							if (eax < 0xA) { eax = eax + 0x30; }
							else { eax = eax + 0x37; }
							pByteBuffer[0x5] = (UCHAR) eax;		// Byte05:  Serial Number[27:24] Ascii
							eax = (edx >> 0x14) & 0x0000000F;
							if (eax < 0xA) { eax = eax + 0x30; }
							else { eax = eax + 0x37; }
							pByteBuffer[0x6] = (UCHAR) eax;		// Byte06:  Serial Number[23:20] Ascii
							eax = (edx >> 0x10) & 0x0000000F;
							if (eax < 0xA) { eax = eax + 0x30; }
							else { eax = eax + 0x37; }
							pByteBuffer[0x7] = (UCHAR) eax;		// Byte07:  Serial Number[19:16] Ascii
							eax = (edx >> 0xC) & 0x0000000F;
							if (eax < 0xA) { eax = eax + 0x30; }
							else { eax = eax + 0x37; }
							pByteBuffer[0x8] = (UCHAR) eax;		// Byte08:  Serial Number[15:12] Ascii
							eax = (edx >> 0x8) & 0x0000000F;
							if (eax < 0xA) { eax = eax + 0x30; }
							else { eax = eax + 0x37; }
							pByteBuffer[0x9] = (UCHAR) eax;		// Byte09:  Serial Number[11:8] Ascii
							eax = (edx >> 0x4) & 0x0000000F;
							if (eax < 0xA) { eax = eax + 0x30; }
							else { eax = eax + 0x37; }
							pByteBuffer[0xA] = (UCHAR) eax;		// Byte10:  Serial Number[7:4] Ascii
							edx = edx & 0x0000000F;
							if (edx < 0xA) { edx = edx + 0x30; }
							else { edx = edx + 0x37; }
							pByteBuffer[0xB] = (UCHAR) edx;		// Byte11:  Serial Number[3:0] Ascii

							pSrb->DataTransferLength = 0xC;
							pSrb->SenseInfoBufferLength = 0x0;
							pSrb->SrbStatus = SRB_STATUS_SUCCESS;
							pSrb->ScsiStatus = SCSISTAT_GOOD;
							}
						else { FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_CDB,0x0); }
						}
					else if (pCdb->CDB6INQUIRY3.PageCode == 0x83)
						{
						if (pCdb->CDB6INQUIRY3.AllocationLength >= 0x28)
							{
							pByteBuffer = pSrb->DataBuffer;
							// SPC3r23 - Device Identification VPD Page
							// Byte00:  PERIPHERAL QUALIFIER[7:5]=0x0 or 0x3 / PERIPHERAL DEVICE TYPE[4:0]=0x0 or 0x1F
							if (pSrb->Lun == LUN) { pByteBuffer[0x0] = 0x00; }
							else { pByteBuffer[0x0] = 0x7F; }
							pByteBuffer[0x1] = 0x83;					// Byte01:  PAGE CODE[7:0]=0x83
							pByteBuffer[0x2] = 0x00;					// Byte02:  (MSB) PAGE LENGTH[7:0]=0x00
							pByteBuffer[0x3] = 0x24;					// Byte03:  (LSB) PAGE LENGTH[7:0]=(n-3)

							edx = pDevExt->SerialNumber;

							// Identification Descriptor (FIRST)
							pByteBuffer[0x4] = 0x01;					// Byte00:  PROTOCOL IDENTIFIER[7:4]=0x0 / CODE SET[3:0]=0x1
							pByteBuffer[0x5] = 0x02;					// Byte01:  PIV[7]=0x0 / RESERVED[6]=0x0 / ASSOCIATION[5:4]=0x0 / IDENTIFICATION TYPE[3:0]=0x2
							pByteBuffer[0x6] = 0x00;					// Byte02:  RESERVED[7:0]=0x00
							pByteBuffer[0x7] = 0x08;					// Byte03:  IDENTIFIER LENGTH[7:0]=(m-3)
							// EUI-64 Identifier
							pByteBuffer[0x8] = 0x00;					// Byte04:  DDRDRIVE IEEE OUI=0x00 (MSB)
							pByteBuffer[0x9] = 0x23;					// Byte05:  DDRDRIVE IEEE OUI=0x23
							pByteBuffer[0xA] = 0x29;					// Byte06:  DDRDRIVE IEEE OUI=0x29 (LSB)
							pByteBuffer[0xB] = (UCHAR)(edx >> 0x18);	// Byte07:  DDRdrive X1 Serial Number[31:24]
							pByteBuffer[0xC] = (UCHAR)(edx >> 0x10);	// Byte08:  DDRdrive X1 Serial Number[23:16]
							pByteBuffer[0xD] = (UCHAR)(edx >> 0x8);		// Byte09:  DDRdrive X1 Serial Number[15:8]
							pByteBuffer[0xE] = (UCHAR)(edx);			// Byte10:  DDRdrive X1 Serial Number[7:0]
							pByteBuffer[0xF] = 0x4C;					// Byte11:  DDRdrive LUN Designator (Ascii 'L'=0x4C)

							// Identification Descriptor (SECOND)
							pByteBuffer[0x10] = 0x01;					// Byte00:  PROTOCOL IDENTIFIER[7:4]=0x0 / CODE SET[3:0]=0x1
							pByteBuffer[0x11] = 0x12;					// Byte01:  PIV[7]=0x0 / RESERVED[6]=0x0 / ASSOCIATION[5:4]=0x1 / IDENTIFICATION TYPE[3:0]=0x2
							pByteBuffer[0x12] = 0x00;					// Byte02:  RESERVED[7:0]=0x00
							pByteBuffer[0x13] = 0x08;					// Byte03:  IDENTIFIER LENGTH[7:0]=(m-3)
							// EUI-64 Identifier
							pByteBuffer[0x14] = 0x00;					// Byte04:  DDRDRIVE IEEE OUI=0x00 (MSB)
							pByteBuffer[0x15] = 0x23;					// Byte05:  DDRDRIVE IEEE OUI=0x23
							pByteBuffer[0x16] = 0x29;					// Byte06:  DDRDRIVE IEEE OUI=0x29 (LSB)
							pByteBuffer[0x17] = (UCHAR)(edx >> 0x18);	// Byte07:  DDRdrive X1 Serial Number[31:24]
							pByteBuffer[0x18] = (UCHAR)(edx >> 0x10);	// Byte08:  DDRdrive X1 Serial Number[23:16]
							pByteBuffer[0x19] = (UCHAR)(edx >> 0x8);	// Byte09:  DDRdrive X1 Serial Number[15:8]
							pByteBuffer[0x1A] = (UCHAR)(edx);			// Byte10:  DDRdrive X1 Serial Number[7:0]
							pByteBuffer[0x1B] = 0x50;					// Byte11:  DDRdrive Target Port Designator (Ascii 'P'=0x50)

							// Identification Descriptor (THIRD)
							pByteBuffer[0x1C] = 0x01;					// Byte00:  PROTOCOL IDENTIFIER[7:4]=0x0 / CODE SET[3:0]=0x1
							pByteBuffer[0x1D] = 0x22;					// Byte01:  PIV[7]=0x0 / RESERVED[6]=0x0 / ASSOCIATION[5:4]=0x2 / IDENTIFICATION TYPE[3:0]=0x2
							pByteBuffer[0x1E] = 0x00;					// Byte02:  RESERVED[7:0]=0x00
							pByteBuffer[0x1F] = 0x08;					// Byte03:  IDENTIFIER LENGTH[7:0]=(m-3)
							// EUI-64 Identifier
							pByteBuffer[0x20] = 0x00;					// Byte04:  DDRDRIVE IEEE OUI=0x00 (MSB)
							pByteBuffer[0x21] = 0x23;					// Byte05:  DDRDRIVE IEEE OUI=0x23
							pByteBuffer[0x22] = 0x29;					// Byte06:  DDRDRIVE IEEE OUI=0x29 (LSB)
							pByteBuffer[0x23] = (UCHAR)(edx >> 0x18);	// Byte07:  DDRdrive X1 Serial Number[31:24]
							pByteBuffer[0x24] = (UCHAR)(edx >> 0x10);	// Byte08:  DDRdrive X1 Serial Number[23:16]
							pByteBuffer[0x25] = (UCHAR)(edx >> 0x8);	// Byte09:  DDRdrive X1 Serial Number[15:8]
							pByteBuffer[0x26] = (UCHAR)(edx);			// Byte10:  DDRdrive X1 Serial Number[7:0]
							pByteBuffer[0x27] = 0x54;					// Byte11:  DDRdrive Target Device Designator (Ascii 'T'=0x54)

							pSrb->DataTransferLength = 0x28;
							pSrb->SenseInfoBufferLength = 0x0;
							pSrb->SrbStatus = SRB_STATUS_SUCCESS;
							pSrb->ScsiStatus = SCSISTAT_GOOD;
							}
						else { FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_CDB,0x0); }
						}
					else { FillSenseInfoBuffer(pSrb,SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_INVALID_CDB,0x0); }
					}
				}

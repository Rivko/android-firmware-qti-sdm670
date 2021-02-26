/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* THE PRESENT SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES
* OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, FOR THE SOLE
* PURPOSE TO SUPPORT YOUR APPLICATION DEVELOPMENT.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following condition is met:
* THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
*
*******************************************************************************/
#ifndef _QINEMO_FWUPDATERCORE_H_
#define _QINEMO_FWUPDATERCORE_H_

#define QiNEMO_I2C_ADDR			0x41		//7bit address

#define QiNEMO_HID_DESC_REG		0x0001
#define	QiNEMO_VID				0x0483
#define QiNEMO_PID				0x7017

typedef enum {HUBMODE_Unknown=0, HUBMODE_Bootloader, HUBMODE_HidSensor} TSensorHubMode;

#define LOWEST_SUPPORTED_VERSION	0x0005

#pragma	pack(1)
typedef struct
{
	UINT16 wHidDescLength;
	UINT16 bcdVersion;
	UINT16 wReportDescLength;
	UINT16 wReportDescRegister;
	UINT16 wInputRegister;
	UINT16 wMaxInputLength;
	UINT16 wOutputRegister;
	UINT16 wMaxOutputLength;
	UINT16 wCommandRegister;
	UINT16 wDataRegister;
	UINT16 wVendorID;
	UINT16 wProductID;
	UINT16 wVersionID;
	UINT32 Reserved;
}THidDescriptor;

typedef struct
{
	CHAR8 Tag[16];
	EFI_GUID PackageGuid;
	UINT32 PackageSize;
	CHAR8 PackDate[12];
	UINT16 VendorID;
	UINT16 ProductID;
	UINT16 VersionID;
	UINT16 BuildNum;
	UINT32 VariationCount;
	UINT32 FirstFIEAddr;
}TFwReleasePackageHeader;

typedef struct
{
	CHAR8 Tag[16];
	UINT32 NextFIEAddr;
	UINT32 FwProductCode;
	UINT32 FwImageAddr;
	UINT32 FwImageSize;
}TFwImageEntry;	//FIE

#pragma pack()

typedef struct
{
	TSensorHubMode SensorHubMode;
	UINT8 QinemoProductCode;
	UINT16 QinemoHidSsrFwVer_bu;
	UINT8 QinemoHidSsrBldNum_bu;
	UINT8 QinemoBtldFwBldNum_bu;
	UINT16 QinemoHidSsrFwVer_au;
	UINT32 LastAttemptedVersion;
	UINT32 UpdateStatus;
}TQinemoFwUpdaterCoreDev;

extern TQinemoFwUpdaterCoreDev UpdaterCore;
extern const EFI_GUID QiNEMO_FRP_GUID;

//QiNEMO_I2cHid.c
extern const UINT8 WrHid_HidDescRegAddr[2];
extern const UINT8 WrHid_RptDescRegAddr[2];
extern const UINT8 WrHid_SetPowerOn[4];
extern const UINT8 WrHid_SetPowerSleep[4];
extern const UINT8 WrHid_HostInitReset[4];
extern THidDescriptor HidDesc;
EFI_STATUS ReadHidDescriptor(VOID);
EFI_STATUS ReadReportDescriptor(VOID);
EFI_STATUS GetFeatureReport(UINT8 RptId, UINT8 RptPkt[], UINT32 PktLen);
EFI_STATUS SetFeatureReport(UINT8 RptId, UINT8 RptPkt[], UINT32 PktLen);
EFI_STATUS GetSyncInputReport(UINT8 RptId, UINT8 RptPkt[], UINT32 PktLen);
EFI_STATUS SetSyncOutputReport(UINT8 RptId, UINT8 RptPkt[], UINT32 PktLen);
EFI_STATUS GetAsynInputReport(UINT8 RptPkt[], UINT32 PktLen);
EFI_STATUS GetAsynInputReportAsyn(UINT8 RptPkt[], UINT32 PktLen, BOOLEAN *pIsNewPacket);
EFI_STATUS EnumerateQinemo(VOID);

//QiNEMO_HidSensor.c
EFI_STATUS GetHidSensorInfo(VOID);
EFI_STATUS TriggerIap(VOID);
TFwImageEntry* MatchFirmwareImage(VOID* pPayload);
EFI_STATUS UpdateFlash(VOID* pPayload);

//QiNEMO_Bootloader.c
EFI_STATUS IapFirmware(VOID* pPayload, TFwImageEntry* pFie);

#endif

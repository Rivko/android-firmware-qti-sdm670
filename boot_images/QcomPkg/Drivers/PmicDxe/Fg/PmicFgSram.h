#ifndef __PMICFGSRAM_H__
#define __PMICFGSRAM_H__

 /*! @file PmicFgSram.h
 *
 * PMIC Battery Profile SRAM/OTP access functionalities 
 * FG OTP Starts at address 0x0.
 * 256x24 (3 banks of 256x8) byte addressable within a 32 bit word.
 * MSByte of each 32 bit word is invalid and will readback 0x00 and will not be programmed.
 * 
 * FG RAM Starts at address 0x400.
 * 128x32 byte addressable.
 *
 * Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*=============================================================================
                              EDIT HISTORY


 when         who       what, where, why
 --------------------------------------------------------------------------------------
 06/15/17     va        Detect the bad conditions of FG SRAM (trim registers) and issue VDD hard reset on PM/PMi to recover
 06/15/17     va        Decoupling sram one time configs from profile load
 01/31/17     va        Adding DMA access for profile load
 11/07/16     cs        Added API to disable ESR Pulse
 07/26/16     va        Restarting FG Changes on warm boot 
 03/22/16     va        Update to make FG Protocol compile for WP and remove not necessary code
 01/27/16     sm        Added APIs to Configure HW JEITA
 11/20/15     va        Rslow clear sequnce on boot up 
 05/28/15     va        Adding Fule Gauge initial register settting suggested by PSV team
 04/16/15     al        Added Batteryconfiguration as param
 03/31/15     va        Rslow workaround Changes
 02/20/15     al        Adding API to read battery id
 01/26/15     SM        Added TERM_CURRENT and TMP_FRAC_SHIFT register address MACROs 
 01/19/15     al        Adding helper function for battery id
 01/13/15     va        Correcting FG SRAM system termination address
 01/05/15     va        Added Multiple Battery Profile and Profile Parser Status API
 12/26/14     al        Adding SRAM volt reading
 09/25/14     va        Update for New Battery Profile Format
 10/06/14     va        Release FG Sram access during UEFI exit
 06/06/14     va        New file.
=============================================================================*/

/**
  PMIC Platform interface
 */

#include <Protocol/EFIPlatformInfo.h>
#include "DDIPlatformInfo.h"
#include "PmicFgBattProfile.h"
#include "pm_fg_batt.h"


/*===========================================================================
                               MACROS
===========================================================================*/
#define BATTARY_PROFILE_MAX_SIZE 0x80 /* 128 bytes */

// Refer to scratch pad register table start from 0x540 - 0x5FF
#if 0
#define FG_SRAM_SYS_TERM_CURRENT_REG_ADDR 0x40C
#define FG_SRAM_TMP_FRAC_SHIFT_REG_ADDR   0x4A4
#define FG_SRAM_TERM_CURRENT_REG_ADDR     0x4F8
#define FG_SRAM_KI_COEFF_PRED_FULL_REG_ADDR 0x408 /*Ki coefficient when predicting the empty SoC value*/
#define FG_SRAM_KI_COEFF_PRED_FULL_VALUE    4 * 1000 /*Ki coefficient when predicting the empty SoC value*/
#define FG_SRAM_BAT_FULL_RS_COMP_90PCT_REG_ADDR         0x530 //Offset 1,2
#define FG_SRAM_BAT_FULL_RS_COMP_90PCT_REG_DATA         0x3373
#define FG_SRAM_SYS_AUTORCHG_MASKING_DISABLED_REG_ADDR  0x4B0 //offset 1
#define FG_SRAM_SYS_AUTORCHG_MASKING_DISABLED_REG_DATA  0x79
#define FG_SRAM_THERM_C1_COFF_REG_ADDR       0x444 /*2,3*/
#define FG_SRAM_THERM_C2_C3_COFF_REG_ADDR    0x448/*C2 = 0,1 C3= 2,3*/
#endif
#define FG_SRAM_VOLT_REG_ADDR             0x5CC
#define FG_SRAM_VEMPTY_IRQ_REG_ADDR       0x0F  /* dec 15 */ //0x458

#define FG_SRAM_PROFILE_START_ADDR     0x18
#define FG_SRAM_PROFILE_END_ADDR       0x4F
#define FG_SRAM_PROFILE_ABS_START_ADDR FG_SRAM_PROFILE_START_ADDR
#define FG_SRAM_PROFILE_ABS_END_ADDR   FG_SRAM_PROFILE_END_ADDR
#define FG_SRAM_START_ADDR     0x00
#define FG_SRAM_END_ADDR       0x80

/* 
DMA starts at 0x4800 with System Partion (RAM.sp)
DMA starts at 0x4900 with Battery Profile (RAM.bp)
DMA starts at 0x4A00 with Workspace (RAM.wk)
*/
#define FG_SRAM_START_DMA_ADDR           0x4800
#define FG_SRAM_START_DMA_PROFILE_ADDR   0x4900
#define FG_SRAM_START_DMA_PROFILE_OFFSET 0x20
#define FG_SRAM_END_DMA_PROFILE_ADDR     0x49FF
#define FG_SRAM_END_DMA_ADDR             0x4BFF


#define NUM_BITS_IN_BYTE sizeof(UINT8) * 8
#define NUM_BYTE_IN_WORD sizeof(UINT32)
#define FG_SRAM_RD_WR_BUS_WIDTH         4
#define FG_SRAM_RD_WR_OFFSET_WIDTH      3

/*
SRAM Block      Offset            SRAM Address
System            0x00-0xBF     0x400 - 0x4BF
Profile              0xC0-0x13F    0x4C0 - 0x53F
Scratchpad       0x140-0x1FF  0x540 -  0x5FF
*/
#define FG_SRAM_MAX_SIZE 0x1FF /* Absoulte Sram Max mMemory Size */

/* Max supported custom reads using IMA in one handshake */
#define FG_SRAM_MAX_READ_CUST_COUNT 32

/*=========================================================================
                            GLOBAL VARIABLES
===========================================================================*/
/**
  Sram State
*/
typedef enum _FgSramState{
  FG_SRAM_STATUS_INIT,
  FG_SRAM_STATUS_LOAD_PROFILE,
  FG_SRAM_STATUS_IN_USE,
  FG_SRAM_STATUS_POLLING,
  FG_SRAM_STATUS_AVAILABLE,
  FG_SRAM_STATUS_INVALID
}FgSramState;

/**
  Sram Address Data Pair
  Structure to hold address, data which would be updated in single Sram access
*/
typedef struct {
  UINT32 SramAddr;
  UINT32 SramData;
}FgSramAddrData;

/**
  Sram Address Data Pair with Data Offset and Size
  Structure to hold address, data which would be updated in single Sram access
*/
typedef struct {
  UINT32 SramAddr;
  UINT32 SramData;
  UINT8  DataOffset; //Offset from Sram Address given valid values 0 -3
  UINT8  DataSize;   //Number of bytes to write (0 - 4): Skip configuring if DataSize=0
}FgSramAddrDataEx;

/**
  Sram Address Data Pair with UINT8 data and offset 
  Structure to hold Address, Data (UINT8), Offset of provide data which would be updated in single Sram access
*/

typedef struct {
  UINT32 SramAddr;//Sram Start Address for the given offset 
  UINT8  SramData;//UINT8 Data
  UINT8  DataOffset;//Offset from Sram Address given valid values 0 -3
}FgSramAddrDataOffset;

/**
  Battery ID and Battery Detect Value read from FG Sram, Tolernace limit would be used for exact read battery id from Sram
*/
typedef struct {
  INT32      Bid;//Battery ID
  UINT32      BatteryIdTolerance;
}AdcRr_BattId;

typedef enum{

  PM_FG_MEM_IF_MEMORY_ACCESS_READ,
  PM_FG_MEM_IF_MEMORY_ACCESS_WRITE,
  PM_FG_MEM_IF_MEMORY_ACCESS_INVALID
}PM_FG_MEM_IF_MEMORY_ACCESS;

typedef enum{

  PM_FG_MEM_IF_MEMORY_ACCESS_TYPE_SINGLE,
  PM_FG_MEM_IF_MEMORY_ACCESS_TYPE_BURST,
  PM_FG_MEM_IF_MEMORY_ACCESS_TYPE_INVALID
}PM_FG_MEM_IF_MEMORY_ACCESS_TYPE;

typedef enum{
  FG_MEMORY_ACCESS_REQ_FALSE,
  FG_MEMORY_ACCESS_REQ_TRUE,
  FG_MEMORY_ACCESS_REQ_TYPE_INVALID
}PM_FG_MEM_IF_MEMORY_ACCESS_REQ_TYPE;

typedef enum{
  FG_MEMORY_ACCESS_SLCT_DMA,
  FG_MEMORY_ACCESS_SLCT_IMA,
  FG_MEMORY_ACCESS_SLCT_TYPE_INVALID
}PM_FG_MEM_IF_MEMORY_ACCESS_SLCT_TYPE;

typedef enum{
  FG_MEMORY_ACCESS_INTF_CTL_READ_ACCESS,
  FG_MEMORY_ACCESS_INTF_CTL_WRITE_ACCESS,
  FG_MEMORY_ACCESS_INTF_CTL_INVALID
}PM_FG_MEM_IF_MEMORY_ACCESS_INTF_CTL_IMA_WR_EN;

typedef enum{
  FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_SINGLE,
  FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_BURST,
  FG_MEMORY_ACCESS_INTF_CTL_BURST_INVALID
}PM_FG_MEM_IF_MEMORY_ACCESS_INTF_CTL_BURST;

typedef struct 
{
  PM_FG_MEM_IF_MEMORY_ACCESS_TYPE memAccessType;
  PM_FG_MEM_IF_MEMORY_ACCESS      memAccess;
}FgMemAccesstype;

/*===========================================================================
                               FUNCTION DEFINITIONS
===========================================================================*/

EFI_STATUS PmicFgSram_Init(UINT32 PmicDeviceIndex, FgSramState FgSramState, EFI_PM_FG_CFGDATA_TYPE *FgConfigData);

EFI_STATUS PmicFgSram_GetState(FgSramState *FgSramSt);

EFI_STATUS PmicFgSram_Dump(UINT32  PmicDeviceIndex, UINT32 DumpSramStartAddr,
                                    UINT8 noOFWordToRead);

EFI_STATUS PmicFgSram_ReadIntegrityBit(UINT32 PmicDeviceIndex, BOOLEAN *bIntergrityBit);

EFI_STATUS PmicFgSram_ImaRead(UINT32 PmicDeviceIndex, UINT16 ReadAddress, UINT32 *fg_memif_data);

EFI_STATUS PmicFgSram_LoadBattProfile(UINT32  PmicDeviceIndex,
                                               IN FgBattProfile *ProfileData,
                                               IN FgSramAddrDataEx * AddrDataPairEx, IN UINT32 AddrDataCount,
                                               AdcRr_BattId *Batt_Id);

EFI_STATUS PmicFgSram_ReleaseFgSramImaAccess(UINT32 PmicDeviceIndex, BOOLEAN *bExceptionOccured);

EFI_STATUS PmicFgSram_CondRestart(UINT32  PmicDeviceIndex, EFI_PM_FG_CFGDATA_TYPE CfgDataType);

EFI_STATUS PmicFgSram_ValidateCheckSum( FgBattProfile *profileData, BOOLEAN *bValid);

EFI_STATUS PmicFgSram_ReadBattID(AdcRr_BattId *Batt_Id);

EFI_STATUS PmicFgSram_PickProfile(AdcRr_BattId *Batt_Id, IN FgBattProfile *Profile, UINT32 *ProfileCount);

BOOLEAN    PmicFgSram_BatteryIdInToleranceLimit(AdcRr_BattId *Batt_Id, UINT32 Profile_Batt_Id);

EFI_STATUS PmicFgSram_GetHALFEncode( INT32 value, UINT16* pEncodedValue);

EFI_STATUS PmicFgSram_VerifyProfileMatch(UINT32 PmicDeviceIndex, AdcRr_BattId Battery_Id, OUT BOOLEAN *bProfileMatch);

EFI_STATUS PmicFgSram_EnableESRPulse(UINT32 PmicDeviceIndex, BOOLEAN enable);
EFI_STATUS PmicFgSram_ReleaseFgSramDmaAccess(UINT32 PmicDeviceIndex);

EFI_STATUS PmicFgSram_SetOneTimeSramCfgs(UINT32 PmicDeviceIndex);

EFI_STATUS PmicFgSram_SramHealthy(UINT32 PmicDeviceIndex);

#endif //__PMICFGSRAM_H__


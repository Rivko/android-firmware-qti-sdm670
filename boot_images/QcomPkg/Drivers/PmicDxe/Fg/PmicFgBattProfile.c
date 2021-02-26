/*! @file PmicFgBattProfile.c
 *
 *  PMIC-Fuel Gauge Module Related Definations
 *  This file contains functions and variable declarations to support
 *  the PMIC Fuel Gauge module - battery profile APIs.
 *
 *  Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/17   va      Enable Charger App Changes 
01/30/17   sm      Adding profile rev to battery profile info
01/23/17   sm      CFG parameter Structure changes
09/20/16   sv      fixing klocwork issues
05/20/16   va      Update for Profile read from FV, LogFs or EFIESP 
03/22/16   va      Update to make FG Protocol compile for WP and remove not necessary code
01/27/16   sm      Function/parameter change to get config file parameters
11/08/15   va      Rslow Clear - profile index fix
10/22/15   al      KW fix
10/12/15   mic     Support updated Dpp protocol
07/13/15   al      Enable CHI platform
05/28/15   va      Adding Fuel Gauge initial register settting suggested by PSV team
06/03/15   al      If battery profile doesn't match then compare range  
05/12/15   mr      Enabled Charger functionality for 8952 (CR-846387)
05/14/15   va      Rslow Clear Seq Threshol value fix
05/14/15   va      Profile Integrity Fix
05/12/15   al      Remove overwriting profile battery id with SRAM battery ID
04/08/15   va      Fixed Convert API for Therm cofficients
04/15/15   al      Making data type changes to make it compatible with 64bit
04/16/15   mic     Use updated DppProtocol
04/08/15   va      Rslow WA Changes
03/13/15   al      Adding changes for optimization
02/20/15   al      Adding reading battery id for case battery profile not parsed
01/26/15   sm      Writing to TERM_CURRENT and TMP_FRAC_SHIFT SRAM registers in PmicFgBattHandleProfile
01/13/15   va      Correcting FG SRAM system termination address
01/13/15   va      Logging for ChargerApp states
01/05/15   va      Added Multiple Battery Profile and Profile Parser Status API, Init FG Termination current
12/26/14   al      Adding header file and CHI support
11/17/14   al      Replacing UEFI banned function
10/28/14   va      Added valid profile check before parsing battery profile
09/25/14   va      Update for New Battery Profile Format
09/23/14   al      Moving API declaration from source file to header file
09/15/14   va      Enable File Logging
09/04/14   al      KW Errors Fix
08/27/14   va      KW Errors Fix.
05/16/14   va      New file.
--------   ---     ----------------------------------------------------------
01/19/15   al         Adding API to return battery type
01/13/15   va         Correcting FG SRAM system termination address
01/13/15   va         Logging for ChargerApp states
01/05/15   va         Added Multiple Battery Profile and Profile Parser Status API, Init FG Termination current
12/26/14   al         Adding header file and CHI support
11/17/14   al         Replacing UEFI banned function
10/28/14   va         Added valid profile check before parsing battery profile
09/25/14   va         Update for New Battery Profile Format
09/23/14   al         Moving API declaration from source file to header file
09/15/14   va         Enable File Logging
09/04/14   al         KW Errors Fix
08/27/14   va         KW Errors Fix.
05/16/14   va         New file.
===========================================================================*/

 /*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
/**
  EFI interfaces
 */
#include <Library/QcomBaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomLib.h>
#include "PmicFileLog.h"
#include <Protocol/EFIVariableServices.h>

/**
  PMIC Lib interfaces
 */
#include "pm_uefi.h"
#include "pm_fg_batt_soc.h"

/**
  DPP Dependencies
 */
#include <Protocol/EFIDpp.h>

/**
  PMIC Platform interface
 */
#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/EFIPmicFg.h>
#include "string.h"
#include "ParserLib.h"
#include "DDIPlatformInfo.h"
#include <Guid/FileSystemInfo.h>
#include <Guid/FileInfo.h>
#include <Guid/Gpt.h>
#include <Library/QcomLib.h>

/**
  FG Dependencies
 */

#include "PmicFgBattProfile.h"
#include <Library/MemoryAllocationLib.h>
#include "pm_fg_batt_info.h"
#include "PmicFgSram.h"

/*===========================================================================
                            MACRO
===========================================================================*/
#define NULL_CHAR              '\0'
#define FILE_INFO_SIZE (SIZE_OF_EFI_FILE_INFO + 256)

/* Batter Profile Strings */
#define BATT_PROFILE_LEN_IN_4_BYTE      0x7F //128/4 bytes
#define BATT_PROFILE_DATA_4_OFFSET      0x03
#define BATT_PROFILE_DATA_TKN_IN_LINE   4

#define TAG_BATTERY_RPOFILE             "BATTERY_PROFILE"
#define TAG_GUI_VERSION                 "GUIVersion"
#define TAG_BATTERY_ID                  "Batt_id"
#define TAG_PROFILE_CHKSUM              "Checksum"
#define TAG_FLOAT_VOLTAGE               "Float_Voltage"
#define TAG_NOMINAL_BATTERY_CAPACITY    "Nom_Batt_Capacity"
#define TAG_THERMISTOR_B                "Therm_B"
#define TAG_BATTERY_NAME                "Battery_name"
#define TAG_THERM_C1_COEFF              "Therm_C1_Coeff" // 85EC
#define TAG_THERM_C2_COEFF              "Therm_C2_Coeff" // 4A76
#define TAG_THERM_C3_COEFF              "Therm_C3_Coeff" // 35FC
#define TAG_PROFILE_VERSION             "Profile_Revision"

#if 0
#define FG_SRAM_TERM_CURRENT_ENCODING 152588 // 5 div 2 to powr 15 in nA
#define FG_SRAM_TERM_ENCODING_SCALE_NA 1000000 // in nA
#define FG_SRAM_ROUNDING_COSNT     5
#endif

#ifdef _MSC_VER
#pragma warning( disable : 4244)
#endif
FgBattProfile FgProfile;
STATIC  FgBattProfileParams ProfileCachedParams;
STATIC  BOOLEAN profileValid = FALSE;
STATIC  UINT32 ProfileAddrCount  = 0;
/* STATIC  PM_FG_BATT_TYPE FgBattType = EFI_PM_FG_BATT_TYPE_UNKNOWN;
STATIC BOOLEAN BatteryDetected = FALSE; */

/* Battery Type variable - persist value across boot */
#define BATTERY_TYPE                 L"FgBatteryType"
/* Profile Index variable - persist value across boot  */
#define BATT_PROFILE_INDEX           L"FgBattProfileIndex"

/*==========================================================================
                        MACRO APIs
===========================================================================*/


/*==========================================================================
                        LOCAL VARIABLES
===========================================================================*/

STATIC EFI_PM_FG_PROFILE_PARSER_STATUS BatteryType_Parserstatus = PM_PARSER_STATUS_PARSE_ERROR;

/*==========================================================================
                        LOCAL API PROTOTYPES
===========================================================================*/

VOID       PmicFgBattProfileTokenPtrArrCallBack(UINT8* Section, UINTN SectionOffset, UINT8* TokenPtrArray[], UINTN MaxTokens);
EFI_STATUS PmicFgBattProfile_ProcessTokenArray(UINT8* Section, UINTN SectionOffset, UINT8* TokenPtrArray[], UINTN MaxTokens);

INT32      PmicFgBattProfileAsciiToInt( CHAR8 *Str);
UINT32     PmicFgBattProfileAsciiStrToHex (CHAR8* Str, UINT32 StrSize);

EFI_STATUS PmicFgBatteryType(AdcRr_BattId *RrBatt_Id, FgBattProfile *FgProfile);
STATIC CHAR8 * AsciiStrNDup(CONST CHAR8 *Source, UINTN  Length);

STATIC EFI_PM_FG_CFGDATA_TYPE      gFgConfigData;

/*==========================================================================
                        GLOBAL API DEFINITION
===========================================================================*/

/**
PmicFgBattProfileInit

@brief
Initializes parameters for Battery Profile
**/
EFI_STATUS PmicFgBattProfileInit(EFI_PM_FG_CFGDATA_TYPE *pFgConfigData)
{
  UINT32 Count = 0;

  SetMem (&gFgConfigData,  sizeof(EFI_PM_FG_CFGDATA_TYPE), 0x00);

    /* Assign global Config Data */
  CopyMemS(&gFgConfigData, sizeof(EFI_PM_FG_CFGDATA_TYPE), pFgConfigData, sizeof(EFI_PM_FG_CFGDATA_TYPE));

  FgProfile.ProfileCount    = 0;
  FgProfile.SelectedProfile = 0;
  //Set Elements to zero
  for(Count = 0; Count < MAX_NUM_OF_BATT_PROFILE; Count++)
  {
    FgProfile.Pd[Count].addrData = NULL;
    SetMem(&FgProfile.Pd[Count].Pp, sizeof(FgBattProfileParams), 0x00);
  }
  /* Initialize Profile params cache */
  SetMem(&ProfileCachedParams, sizeof(FgBattProfileParams), 0x00);

  return EFI_SUCCESS;
}


#if 0
EFI_STATUS PmicFgBatteryType(AdcRr_BattId *RrBatt_Id, FgBattProfile *FgProfile)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 ProfileCount;
  BOOLEAN BatIdInToleranceLimit = FALSE;
  UINTN TempBattType = 0;

  /*only if battery profile was found*/
  if (NULL!= FgProfile)
  {
    Status = PmicFgSram_PickProfile(RrBatt_Id, FgProfile, &ProfileCount);
    BatIdInToleranceLimit = PmicFgSram_BatteryIdInToleranceLimit(RrBatt_Id, FgProfile->Pd[ProfileCount].Pp.BatteryId);
  }
  else if ((RrBatt_Id->Bid >= gFgConfigData.BattIdCfgData.AnalogBattIdMin) && (RrBatt_Id->Bid <= gFgConfigData.BattIdCfgData.AnalogBattIdMax))
  {
    FgBattType = EFI_PM_FG_BATT_TYPE_NORMAL;
    BatteryDetected = TRUE;
    return Status;
  }

  if (TRUE == BatIdInToleranceLimit)
  {
    //SMART or REAL BATTERY: KNOWN battery
    FgBattType = (RrBatt_Id->DetId == BID_DETECT_BATTERY_SMART) ? EFI_PM_FG_BATT_TYPE_SMART : EFI_PM_FG_BATT_TYPE_NORMAL;
  }
  else if ((RrBatt_Id->Bid >= gFgConfigData.BattIdCfgData.AnalogBattIdMin) && (RrBatt_Id->Bid <= gFgConfigData.BattIdCfgData.AnalogBattIdMax))
  { /*if battery profile was not found then we still need to make sure if it is analog battery, which is regular battery and smart battery*/
     /*have range from 15k to 450K for these target*/
    FgBattType = EFI_PM_FG_BATT_TYPE_NORMAL;
  }
  else if ((RrBatt_Id->Bid >= gFgConfigData.BattIdCfgData.DebugBoardBattIdMin) && (RrBatt_Id->Bid <= gFgConfigData.BattIdCfgData.DebugBoardBattIdMax))
  {
    FgBattType = EFI_PM_FG_BATT_TYPE_DEBUG_BOARD;
  }
  else if ((RrBatt_Id->Bid >= gFgConfigData.BattIdCfgData.DummyBattIdMin1 && RrBatt_Id->Bid <= gFgConfigData.BattIdCfgData.DummyBattIdMax1))
  {
    //dummy battery
    FgBattType = EFI_PM_FG_BATT_BATT_EMULATOR;
  }
  else
  {
    //unknown battery
    FgBattType = EFI_PM_FG_BATT_TYPE_UNKNOWN;
  }

  BatteryDetected = TRUE;

  TempBattType = (UINTN)FgBattType;

  /*storing battery type as UEFI variable*/
  PmicUefiVariable(BATTERY_TYPE, &TempBattType, UEFI_VARIABLE_OP_SET);

  return Status;
}
#endif

#define SRAM_TMP_TRAC_SHIFT 0x12A2

/**
PmicFgBattHandleProfile

@brief
Read battery profile from EFIESP/PLAT parition and Set Battery Profile respectivly
**/
EFI_STATUS PmicFgBattHandleProfile(UINT32 PmicDeviceIndex)
{
  EFI_STATUS             Status            = EFI_SUCCESS;
  AdcRr_BattId           Battery_Id       = {0};
  UINT32                 AddrDataCount     = 0;
  FgSramAddrDataEx       AddrDataPairEx[6];
  EFI_PM_FG_PROFILE_PARSER_STATUS Parserstatus = PM_PARSER_STATUS_GOOD_MATCH;
  UINT32                 Count             = 0;
/*
  UINT32                 SysTermCurrent    = 0;
  UINT32                 TermCurrent       = 0;
  UINT16                 KiCoeffPred       = 0; */
  UINTN                  SelectedProfile   = 0;

  SetMem(AddrDataPairEx, sizeof(FgSramAddrDataEx) * 6, 0x00);

  /* Load profile here and retrieve the sram battery ID so that check for dummy battery id can be done here */
  if ((EFI_SUCCESS == Status) && (NULL != FgProfile.Pd[0].addrData) && (PM_PARSER_STATUS_PARSE_ERROR != Parserstatus))
  {
    /* Set and Pass Tolerance level */
    Battery_Id.BatteryIdTolerance = gFgConfigData.FgBattIdCfg.BatteryIdTolerance;

    Status |= PmicFgSram_LoadBattProfile(PmicDeviceIndex, &FgProfile, AddrDataPairEx,  AddrDataCount, &Battery_Id);
    SelectedProfile = (UINTN )FgProfile.SelectedProfile;
    Status |= PmicUefiVariable(BATT_PROFILE_INDEX, &SelectedProfile, UEFI_VARIABLE_OP_SET);
    /* Update battery type*/
    /* PmicFgBatteryType(&Battery_Id, &FgProfile); */
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a Can not load Battery profile Parserstatus : (%d)\r\n", __FUNCTION__,Parserstatus));
    Status |= PmicFgSram_Init(PmicDeviceIndex, FG_SRAM_STATUS_AVAILABLE, &gFgConfigData);
  }

  if ((PM_PARSER_STATUS_PARSE_ERROR == Parserstatus )|| (PM_PARSER_STATUS_CONFIG_ERROR == Parserstatus)
      || (PM_PARSER_STATUS_NO_ID_MATCH == Parserstatus))
  {
    /* Select OTP profile if the forecd OTP profile flag is set Check Config flag  */
    /* DO nothing as OTP profile is in place */
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a Can not load Battery profile Parserstatus : (%d)\r\n", __FUNCTION__,Parserstatus));
  }

  /* Release not required data/memory */
  if(NULL != FgProfile.Pd[0].addrData)
  {
    /* Free allocated profile */
    for(Count = 0; (Count < MAX_NUM_OF_BATT_PROFILE) && (NULL != FgProfile.Pd[Count].addrData); Count++)
    {
      Status |= gBS->FreePool(FgProfile.Pd[Count].addrData);
      FgProfile.Pd[Count].addrData = NULL;
    }
    for(Count = 0; (Count < MAX_NUM_OF_BATT_PROFILE) && (NULL != FgProfile.Pd[Count].Pp.BatteryName); Count++)
    {
      Status |= gBS->FreePool(FgProfile.Pd[Count].Pp.BatteryName);
      FgProfile.Pd[Count].Pp.BatteryName = NULL;
    }
  }

  //Assign Parser Status for battery /debug board detection
  BatteryType_Parserstatus = Parserstatus;

  return Status;
}

#define SECTION_START_CHAR    '['
/**
PmicBattProfileParseDataBuffer

@brief
Parse battery profile Data on given Buffer
**/
 EFI_PM_FG_PROFILE_PARSER_STATUS PmicBattProfileParseDataBuffer
(
 IN  UINT8      *pBufferData,
 IN  UINTN       DataBufferSize
)
{
  INTN  Pd = 0;
  EFI_PM_FG_PROFILE_PARSER_STATUS ParserStatus = PM_PARSER_STATUS_GOOD_MATCH;

  if((!pBufferData) || (!DataBufferSize))
  {
    return PM_PARSER_STATUS_BAD_PARAM;
  }

  Pd = OpenParser (pBufferData, DataBufferSize, NULL);

  if (Pd < 0)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a Pmic Battery Profile- Parser open failed on read batter profile data \r\n", __FUNCTION__));
    ParserStatus = PM_PARSER_STATUS_PARSE_ERROR;
  }
  else
  {
    /* BackWard Compatibility for battery profile */
    if (pBufferData[0] == SECTION_START_CHAR)
    {
      /* Kick off Pmic CFG file interpreter */
      EnumCommaDelimSectionItems (Pd, (UINT8 *)"BATTERY_PROFILE", PmicFgBattProfileTokenPtrArrCallBack);
    }
    else
    {
      EnumCommaDelimSectionItems (Pd, NULL, PmicFgBattProfileTokenPtrArrCallBack);
    }
    /* Clean up resources */
    CloseParser(Pd);
  }
  if (TRUE == profileValid){
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a Battery Profile- Success \r\n", __FUNCTION__));
  }else
  {
    ParserStatus = PM_PARSER_STATUS_PARSE_ERROR;
    profileValid = FALSE;
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a Pmic Battery Profile- Parser failed (%d)\r\n",__FUNCTION__, ParserStatus));
  }

  return ParserStatus;

}


/**
PmicFgBattProfileTokenPtrArrCallBack

@brief
Token ptr array call back for battey profile data parsing
**/
VOID
PmicFgBattProfileTokenPtrArrCallBack(UINT8* Section, UINTN SectionOffset, UINT8* TokenPtrArray[], UINTN MaxTokens)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 tokenCount = 0;
  UINT32 absAddr    = 0;
  UINT32 strLen     = 0;
  STATIC UINTN SectionCnt = 0;//SectionOffset start with 1

  /* PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: Section = (%a) , SectionOffset = (%d), TokenPtrArray = (%a), MaxTokens = (%d)\r\n",
              Section, SectionOffset, TokenPtrArray[0], MaxTokens)); */
  /* BackWard Compatibility for battery profile */
  if(0 == SectionOffset)
    SectionOffset =1;
/* Enable below when backward support for battery profile is not needed */
  if(AsciiStriCmp((CHAR8 *)Section, (CHAR8*)TAG_BATTERY_RPOFILE) != 0)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a Profile Tag Mis Match (%a) (%a)\r\n", __FUNCTION__, TAG_BATTERY_RPOFILE, Section));
    profileValid = FALSE;
    return;
  }

  if(MAX_NUM_OF_BATT_PROFILE < SectionOffset)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a Profile support limit reached SectionOffset = %d\r\n",__FUNCTION__, MAX_NUM_OF_BATT_PROFILE, SectionOffset));
    profileValid = FALSE;
    return;
  }

  if((SectionCnt != SectionOffset ) && (NULL == FgProfile.Pd[SectionOffset - 1].addrData))
  {
    /* Increment section count */
    ++SectionCnt;
    if(AsciiStrCmp((CHAR8 *)TokenPtrArray[SectionOffset - SectionCnt], TAG_GUI_VERSION) == 0)
    {
      PMIC_UART_DEBUG(( EFI_D_WARN, "PmicDxe: Valid Profile GUI Version = (%a)\r\n", TokenPtrArray[(SectionOffset - SectionCnt) + 1]));
      PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: Valid Profile GUI Version = (%a )\r\n", TokenPtrArray[(SectionOffset - SectionCnt) + 1]));
      profileValid = TRUE;
    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a Profile (%d) Invalid \r\n", __FUNCTION__, SectionOffset));
      profileValid = FALSE;
    }
    /* Allocate Memory for one profile */
    Status = gBS->AllocatePool( EfiBootServicesData,
                            sizeof(FgBattAddrData) * BATTARY_PROFILE_MAX_SIZE,
                            (VOID**)&FgProfile.Pd[SectionOffset - 1].addrData);
    (void)FgProfile.Pd[SectionCnt];
    if(EFI_SUCCESS != Status || NULL == FgProfile.Pd[SectionOffset - 1].addrData)
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a Unable to allocate pool! Error Code = %r \r\n", __FUNCTION__, Status));
      return ;
    }
    SetMem(FgProfile.Pd[SectionOffset - 1].addrData, sizeof(FgBattAddrData) * BATTARY_PROFILE_MAX_SIZE, 0x00);
    /* Keep Parsed profile counter updated */
    FgProfile.ProfileCount = (UINT32 )SectionOffset;
    /* Reset Profile Address Count */
    ProfileAddrCount = 0;
  }

  if (TRUE == profileValid)
  {
    for (tokenCount = 0; ((tokenCount <= MaxTokens) && (NULL != TokenPtrArray[tokenCount])); tokenCount++, absAddr = 0)
    {
      //PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: Token(%d) = (%a) \r\n", tokenCount, TokenPtrArray[tokenCount]));

      if(AsciiStriCmp((CHAR8 *)TokenPtrArray[tokenCount], TAG_BATTERY_ID) == 0)
      {
         FgProfile.Pd[SectionOffset - 1].Pp.BatteryId = PmicFgBattProfileAsciiToInt((CHAR8 *)TokenPtrArray[tokenCount + 1]);
        break;
      }
      else if(AsciiStriCmp((CHAR8 *)TokenPtrArray[tokenCount], TAG_PROFILE_CHKSUM) == 0)
      {
        FgProfile.Pd[SectionOffset - 1].Pp.CheckSum = AsciiStrToHex((CHAR8 *)TokenPtrArray[tokenCount + 1], (UINT32)AsciiStrLen((CHAR8 *)TokenPtrArray[tokenCount + 1]) + 1 );
        break;
      }
      else if(AsciiStriCmp((CHAR8 *)TokenPtrArray[tokenCount], TAG_FLOAT_VOLTAGE) == 0)
      {
        FgProfile.Pd[SectionOffset - 1].Pp.FloatVoltage = PmicFgBattProfileAsciiToInt((CHAR8 *)TokenPtrArray[tokenCount + 1]);
        break;
      }
	  else if(AsciiStriCmp((CHAR8 *)TokenPtrArray[tokenCount], TAG_PROFILE_VERSION) == 0)
      {
        FgProfile.Pd[SectionOffset - 1].Pp.ProfileRev = PmicFgBattProfileAsciiToInt((CHAR8 *)TokenPtrArray[tokenCount + 1]);
        PMIC_UART_DEBUG(( EFI_D_WARN, "PmicDxe: %a Profile_Revision = %d \r\n", __FUNCTION__, FgProfile.Pd[SectionOffset - 1].Pp.ProfileRev));
        break;
      }
      else if(AsciiStriCmp((CHAR8 *)TokenPtrArray[tokenCount], TAG_NOMINAL_BATTERY_CAPACITY) == 0)
      {
        FgProfile.Pd[SectionOffset - 1].Pp.NomBattCapacity = PmicFgBattProfileAsciiToInt((CHAR8 *)TokenPtrArray[tokenCount + 1]);
        break;
      }
      else if(AsciiStriCmp((CHAR8 *)TokenPtrArray[tokenCount], TAG_BATTERY_NAME) == 0)
      {
        //Ignore battery name just log the name
        PMIC_UART_DEBUG(( EFI_D_WARN,  "PmicDxe: Battery Name = (%a) \r\n", TokenPtrArray[tokenCount + 1]));
        PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: Battery Name = (%a) \r\n", TokenPtrArray[tokenCount + 1]));
        FgProfile.Pd[SectionOffset - 1].Pp.BatteryName = (UINT8 *) AsciiStrNDup((CHAR8 *)TokenPtrArray[tokenCount + 1], (UINT32)AsciiStrLen((CHAR8 *)TokenPtrArray[tokenCount + 1]) + 1);
        break;
      }
      else if(AsciiStriCmp((CHAR8 *)TokenPtrArray[tokenCount], TAG_THERMISTOR_B) == 0)
      {
        FgProfile.Pd[SectionOffset - 1].Pp.ThermistorValue = PmicFgBattProfileAsciiToInt((CHAR8 *)TokenPtrArray[tokenCount + 1]);
        break;
      }
      else if(AsciiStriCmp((CHAR8 *)TokenPtrArray[tokenCount], TAG_THERM_C1_COEFF) == 0)
      {
        strLen = AsciiStrLen((CHAR8 *)TokenPtrArray[tokenCount + 1]);
        FgProfile.Pd[SectionOffset - 1].Pp.ThermCoff.ThermC1Coeff = PmicFgBattProfileAsciiStrToHex((CHAR8 *)TokenPtrArray[tokenCount + 1], strLen );
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: ThermC1Coeff = (0x%x) \r\n", FgProfile.Pd[SectionOffset - 1].Pp.ThermCoff.ThermC1Coeff));
        break;
      }
      else if(AsciiStriCmp((CHAR8 *)TokenPtrArray[tokenCount], TAG_THERM_C2_COEFF) == 0)
      {
        strLen = AsciiStrLen((CHAR8 *)TokenPtrArray[tokenCount + 1]);
        FgProfile.Pd[SectionOffset - 1].Pp.ThermCoff.ThermC2Coeff = PmicFgBattProfileAsciiStrToHex((CHAR8 *)TokenPtrArray[tokenCount + 1], strLen );
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: ThermC2Coeff = (0x%x) \r\n", FgProfile.Pd[SectionOffset - 1].Pp.ThermCoff.ThermC2Coeff));
        break;
      }
      else if(AsciiStriCmp((CHAR8 *)TokenPtrArray[tokenCount], TAG_THERM_C3_COEFF) == 0)
      {
        strLen = AsciiStrLen((CHAR8 *)TokenPtrArray[tokenCount + 1]);
        FgProfile.Pd[SectionOffset - 1].Pp.ThermCoff.ThermC3Coeff = PmicFgBattProfileAsciiStrToHex((CHAR8 *)TokenPtrArray[tokenCount + 1], strLen );
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: ThermC3Coeff = (0x%x) \r\n", FgProfile.Pd[SectionOffset - 1].Pp.ThermCoff.ThermC3Coeff));
        break;
      }else{
        strLen = AsciiStrLen((CHAR8 *)TokenPtrArray[tokenCount]);
        absAddr = PmicFgBattProfileAsciiStrToHex((CHAR8 *)TokenPtrArray[tokenCount], strLen );
        //PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: absAddr(%d) \r\n", absAddr));
        if ((absAddr >= FG_SRAM_PROFILE_ABS_START_ADDR ) && (absAddr <= FG_SRAM_PROFILE_ABS_END_ADDR )){
          PmicFgBattProfile_ProcessTokenArray(Section, SectionOffset, TokenPtrArray, MaxTokens);
          break;//break for loop as we read entire line here
        }
      }
    }
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a Profile Invalid \r\n", __FUNCTION__));
  }
}


EFI_STATUS PmicFgBattProfile_ProcessTokenArray(UINT8* Section, UINTN SectionOffset, UINT8* TokenPtrArray[], UINTN MaxTokens)
{
  STATIC UINT32 PartialData = 0;
  UINT32 Addr = 0, Addr2 = 0, Offset = 0, Data = 0;

  /* BackWard Compatibility for battery profile */
  if(SectionOffset == 0)
    SectionOffset =1;

  //Validate input params
  if ((MaxTokens < BATT_PROFILE_DATA_TKN_IN_LINE)&&
    ((NULL != TokenPtrArray[0]) && (NULL != TokenPtrArray[1]) && (NULL != TokenPtrArray[2]) && (NULL != TokenPtrArray[3])))
    return EFI_INVALID_PARAMETER;
  Addr   = AsciiStrToHex((CHAR8 *)TokenPtrArray[0], StrLen ((UINT16 *)TokenPtrArray[0]) + 2);
  Addr2  = AsciiStrToHex((CHAR8 *)TokenPtrArray[1], StrLen ((UINT16 *)TokenPtrArray[1]) + 2);
  Offset = AsciiStrToHex((CHAR8 *)TokenPtrArray[2], StrLen ((UINT16 *)TokenPtrArray[2]) + 2);
  Data   = AsciiStrToHex((CHAR8 *)TokenPtrArray[3], StrLen ((UINT16 *)TokenPtrArray[3]) + 2);

  if ((Addr2 >= FG_SRAM_PROFILE_START_ADDR ) && (Addr2 <= FG_SRAM_PROFILE_END_ADDR ) && (ProfileAddrCount <= BATT_PROFILE_LEN_IN_4_BYTE ))
  {
    PartialData = PartialData | (Data << (Offset * NUM_BITS_IN_BYTE));
    //PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: PartialData = (0x%x)\"\n",PartialData));
    if (Offset == BATT_PROFILE_DATA_4_OFFSET){
      FgProfile.Pd[SectionOffset - 1].addrData[ProfileAddrCount].Data = PartialData;
      /* PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: BatteryProfileArray[(%d)] = (0x%x) \r\n", ProfileAddrCount, FgProfile.Pd[SectionOffset - 1].addrData[ProfileAddrCount].Data)); */
      /* Assign other elements for book keeping */
      FgProfile.Pd[SectionOffset - 1].addrData[ProfileAddrCount].Addr  = Addr;
      FgProfile.Pd[SectionOffset - 1].addrData[ProfileAddrCount].Addr2 = Addr2;
      FgProfile.Pd[SectionOffset - 1].addrData[ProfileAddrCount].Offset = Offset;

      PartialData = 0;//reset 4 byte append count
      ProfileAddrCount++;//upkeep address count
    }else{
      //PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: Profile Addr = (0x%x), Addr2 = (0x%x), Offset = (0x%x), Data = (0x%x)\r\n",Addr, Addr2, Offset, Data));
    }
  }
  else{
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a ** Profile Addr Not in Range ** \r\n", __FUNCTION__));
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Profile Addr =(0x%x), Addr2 =(0x%x), Offset =(0x%x), Data =(0x%x) ProfAddrCnt =%d\r\n",
                __FUNCTION__, Addr, Addr2, Offset, Data, ProfileAddrCount));
  }
  return EFI_SUCCESS;
}


EFI_STATUS PmicFgBattParserStatus(EFI_PM_FG_PROFILE_PARSER_STATUS *parser_status)
{
  if(!parser_status)
    return EFI_INVALID_PARAMETER;

  *parser_status = BatteryType_Parserstatus;

  return EFI_SUCCESS;
}

#if 0
/*
Read  Profile Params for flashed profile
  */
EFI_STATUS PmicFgBattProfileReadProfileParams(UINT32 PmicDeviceIndex, BOOLEAN ReadProfile, UINT32 ProfileIndex)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PM_FG_PROFILE_PARSER_STATUS Parserstatus = PM_PARSER_STATUS_OTHER_ERROR;
  UINTN FgProfileSelected = 0;
  UINT32  Count = 0;

  if(TRUE == ReadProfile)
  {
    Status |= PmicUefiVariable(BATT_PROFILE_INDEX, &FgProfileSelected, UEFI_VARIABLE_OP_GET);

    if (Status != EFI_SUCCESS) {
      // Variable was not found, default to disabled
      FgProfileSelected = 0;
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: Error in Reading Flashed Profile Index FgProfileSelected = %d \r\n", FgProfileSelected));
      //return EFI_NOT_FOUND;
    }
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: SUCCESS in Reading Flashed Profile Index (%d)\r\n",FgProfileSelected));
    /* Read Battery Profile and if available Try loading battery profile to SRAM  */
    Status |= PmicFgBattReadBatteryProfile(PmicDeviceIndex, &Parserstatus);
  }
  else
  {
     /*cold boot, profile is already parsed and selected */
    Parserstatus = PM_PARSER_STATUS_GOOD_MATCH;
    FgProfileSelected = ProfileIndex;
  }
  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: Reading data from Profile - Index (%d)\r\n",FgProfileSelected));
  // Load profile here and retrieve the sram battery ID so that check for dummy battery id can be done here
  if ((EFI_SUCCESS == Status) && (NULL != FgProfile.Pd[0].addrData) && (PM_PARSER_STATUS_PARSE_ERROR != Parserstatus))
  {
    /*Retrieve loaded battery profile index to UEFI variable */
    if(NULL != FgProfile.Pd[FgProfileSelected].addrData)
    {
      /* Cache common parameters */
      /*BatteryId and Battery Name is not cached during warm reset */
      ProfileCachedParams.FloatVoltage                = FgProfile.Pd[FgProfileSelected].Pp.FloatVoltage;
      ProfileCachedParams.NomBattCapacity             = FgProfile.Pd[FgProfileSelected].Pp.NomBattCapacity;
      ProfileCachedParams.ThermistorValue             = FgProfile.Pd[FgProfileSelected].Pp.ThermistorValue;
      ProfileCachedParams.CheckSum                    = FgProfile.Pd[FgProfileSelected].Pp.CheckSum;
      ProfileCachedParams.BatteryName                 = NULL;
      ProfileCachedParams.ThermCoff.ThermC1Coeff      = FgProfile.Pd[FgProfileSelected].Pp.ThermCoff.ThermC1Coeff;
      ProfileCachedParams.ThermCoff.ThermC2Coeff      = FgProfile.Pd[FgProfileSelected].Pp.ThermCoff.ThermC2Coeff;
      ProfileCachedParams.ThermCoff.ThermC3Coeff      = FgProfile.Pd[FgProfileSelected].Pp.ThermCoff.ThermC3Coeff;

    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: Error in Battery Profile File Read \r\n"));
    }

  }

  /* Release not required data/memory */
  if(NULL != FgProfile.Pd[0].addrData)
  {
    /* Free allocated profile */
    for(Count = 0; (Count < MAX_NUM_OF_BATT_PROFILE) && (NULL != FgProfile.Pd[Count].addrData); Count++)
    {
      Status |= gBS->FreePool(FgProfile.Pd[Count].addrData);
      FgProfile.Pd[Count].addrData = NULL;
    }
    for(Count = 0; (Count < MAX_NUM_OF_BATT_PROFILE) && (NULL != FgProfile.Pd[Count].Pp.BatteryName); Count++)
    {
      Status |= gBS->FreePool(FgProfile.Pd[Count].Pp.BatteryName);
      FgProfile.Pd[Count].Pp.BatteryName = NULL;
    }
  }

  return Status;
}


/*
Get  Profile Params for flashed profile
  */
EFI_STATUS PmicFgBattProfileGetProfileParams(OUT FgBattProfileParams *Pp)
{
  if (!Pp)
  return EFI_INVALID_PARAMETER;

  *Pp = ProfileCachedParams;

return EFI_SUCCESS;
}

#endif

/* Utils functions can be made common part of QCOM LIB of seperate file in pmic uefi */

/**
  PmicFgBattProfileAsciiToInt

  @brief
  Converts ASCII string to integer
**/
INT32 PmicFgBattProfileAsciiToInt
(
  CHAR8 *Str
)
{
    INT32 Value = 0;
    int   Sign  = 1;

    if(Str == NULL)
        return 0;

    if(*Str == '-')
    {
        Sign = -1;
        Str++;
    }

    while(*Str != NULL_CHAR)
    {
        if(*Str >= '0' && *Str <= '9')
        {
            Value = ((Value * 10) + (*Str - '0'));
        }
        Str++;
    }

    return (Value * Sign);
}

/**
  EFI_PmicBattProfileStrCat

  @brief
  Concatenates strings
**/
EFI_STATUS EFI_PmicBattProfileStrCat
(
  char *Dest,
  char *Src
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    if((Src == NULL)||(Dest == NULL))
    {
        Status = EFI_INVALID_PARAMETER;
    }

    if (Status == EFI_SUCCESS)
    {
       while(*Dest != NULL_CHAR)
       {
           Dest++;
       }

       while(*Src != NULL_CHAR)
       {
           *Dest = *Src;
           Src++;
           Dest++;
       }
       *Dest = NULL_CHAR;
    }

    return Status;
}

/**
  EFI_PmicBattProfileStrToUpper

  @brief
  Converts string to Uppercase
**/
VOID EFI_PmicBattProfileStrToUpper
(
  char *Str,
  UINT32 StrSize
)
{

    while((*Str != NULL_CHAR) && (StrSize))
    {
        if(*Str >= 'a' && *Str <= 'z')
        {
            *Str = *Str - ('a' - 'A');
        }

        Str++;
        StrSize--;
    }

}


UINT32 PmicFgBattProfileAsciiStrToHex (CHAR8* Str, UINT32 StrSize)
{
  UINT32 result = 0;
  if(Str == NULL)
    return 0;

  while( (*Str!=0) && StrSize )
  {
    // Break as soon as non-hex char encountered.
    if (!((*Str >= '0' && *Str <= '9') ||
          (*Str >= 'a' && *Str <= 'f') ||
          (*Str >= 'A' && *Str <= 'F')))
      {
        result = 0;
        break;
      }
    result = result << 4;
    if(*Str >= '0' && *Str <= '9')
      result = result + (*Str - '0');
    if(*Str >= 'a' && *Str <= 'f')
      result = result + (*Str - 'a') + 10;
    if(*Str >= 'A' && *Str <= 'F')
      result = result + (*Str - 'A') + 10;

    StrSize--;
    Str++;
  }

  return result;
}


EFI_STATUS PmicUefiVariable(UINT16 *variable, UINTN* Value, PmUefiVariableOpType OpType)
{
  EFI_VARIABLESERVICES_PROTOCOL *VariableServicesProtocol = NULL;
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN   DataSize = sizeof(*Value);
  UINT32  Attributes = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS);

  if (NULL == variable || NULL == Value)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (UEFI_VARIABLE_OP_SET == OpType)
  {
    Status = gRT->SetVariable(variable, &gQcomTokenSpaceGuid, Attributes, DataSize, Value);

    gBS->LocateProtocol(&gEfiVariableServicesProtocolGuid, NULL, (VOID **)&VariableServicesProtocol);

    if (VariableServicesProtocol != NULL)
    {
      VariableServicesProtocol->FlushVariableNV(VariableServicesProtocol);
    }
  }
  else
  {
    Status = gRT->GetVariable(variable, &gQcomTokenSpaceGuid, NULL, &DataSize, Value);
  }

  return Status;
}

/**
  Duplicates ASCII string

  @param  Source  Null Terminated string of characters
  @param  Length  source String length
  @return  Duplicated String  if successful otherwise return NULL

  note: Caller responsibility to Free returned String
**/
STATIC CHAR8 * AsciiStrNDup(
  CONST CHAR8               *Source,
  UINTN                     Length
)
{
  CHAR8 *Dest = NULL;
  if(NULL == Source || 0 == Length)
    return NULL;

  Dest = AllocatePool (Length + 1);
  if (NULL == Dest)
  {
    return NULL;
  }

  AsciiStrnCpy(Dest, Source, Length + 1);

  return Dest;
}
/* End of Util function */


#ifdef _MSC_VER
#pragma warning( default : 4244)
#endif
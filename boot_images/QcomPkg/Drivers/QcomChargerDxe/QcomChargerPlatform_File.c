/** @file QcomChargerPlatform_File.c

  This file will set up file logging, load and parse the cfg file and the configuration file from efiesp.

  Copyright (c) 2016-2017,  Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

 when         who     what, where, why
 --------     ---     -----------------------------------------------------------
 04/24/17     va      Enable Charger App Changes
 01/27/17     ai      Fix use of retail
 01/23/17     sm      CFG parameter Structure changes
 10/26/16     va      Mount logfs in common file
 08/14/16     va      Adding log and fixing bug for mass storage feature
 06/14/16     va      Adding File log support to Charger App
 05/20/16     va      Update for Profile read from FV, LogFs or EFIESP 
 04/26/16     va      Adding for config file feature
 04/20/16     sm      Resolving KW Error in ChargerPlatformFile_OpenFile
 03/28/16     va      Adding file log support
 01/27/16     sm      Initial revision

=============================================================================*/
#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ParserLib.h>
#include <Library/QcomTargetLib.h>
#include <Library/FuseControlLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/FileInfo.h>

#include <api/pmic/pm/pm_uefi.h>

/**
Protocol Dependencies
*/
#include <Protocol/EFIPlatformInfo.h>

#include "DDIPlatformInfo.h"
#include <Protocol/SimpleFileSystem.h>

/**
Charger headers
*/

#include "QcomChargerPlatform_File.h"
#include "QcomChargerPlatform_FileCommon.h"

#include "QcomChargerPlatform.h"
#include <api/pmic/charger/ChargerLibCommon.h>

#define CHARGER_CFG_FILE_IN_FV              L"QcomChargerCfg.cfg"
#define CHARGER_OVERWRITE_CFG_FILE_PATH     L"\\QcomChargerCfg.cfg"

extern VOID ChargerPlatform_ReadCfgParams( UINT8* Section, UINT8* Key, UINT8* Value);
/*===========================================================================
  FUNCTION DECLARATION
===========================================================================*/

#define NULL_CHAR              '\0'

/* LogFS Partition GUID for Config Parameters */
extern EFI_GUID gEfiLogFSPartitionGuid;


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
EFI_STATUS ChargerPlatformFile_FileLogInit( QCOM_CHARGER_PLATFORM_CFGDATA_TYPE ChargerPlatformCfgData )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (TRUE == ChargerPlatformCfgData.ChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg_to_file)
  {
    if (!RETAIL) 
    {
      /* Mounting Log fs */
      Status = MountFatPartition(L"logfs");
      CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a LogFS mount Status = %r \r\n", __FUNCTION__, Status));
    }
    Status = ChargerLib_InitFileLog(ChargerPlatformCfgData.ChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg_to_file);
  }
  else
  {
    CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a File Log Disabled  = %d \r\n", __FUNCTION__, ChargerPlatformCfgData.ChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg_to_file));
  }

  return Status;
}

/**
  ChargerPlatformFile_AsciiToInt

  @brief
  Converts ASCII string to integer
**/
INT32 ChargerPlatformFile_AsciiToInt
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

UINT32 ChargerPlatformFile_AsciiStrToHex (CHAR8* Str, UINT32 StrSize)
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

EFI_STATUS ChargerPlatformFile_OverwriteDefaultCfg( void )
{
  EFI_STATUS  Status         = EFI_SUCCESS;
  UINT8      *pFileBuffer    = NULL;
  UINTN       DataSize       = 0;
  INTN        Pd             = 0;

  /* Attempt to load Charger Config file from LogFS parition */

  /* Read from LogFs or EFIESP */

  Status = ChargerPlatformFile_DebugReadFile(CHARGER_OVERWRITE_CFG_FILE_PATH, &pFileBuffer, &DataSize);
  if ((Status != EFI_SUCCESS ) || (NULL == pFileBuffer))
  {
    CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a Could not open ChargerApp Config File = %r \r\n",__FUNCTION__, Status));
    return EFI_LOAD_ERROR;
  }

  /* This debug will print only when config file is found */
  CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a OverWrite ChargerApp Config parameters = %r, DataSize = %d \r\n",__FUNCTION__, Status, DataSize));

  Pd = OpenParser((UINT8 *)pFileBuffer, (UINT32)DataSize, NULL);
  if (Pd < 0)
  {
    Status = EFI_LOAD_ERROR;
  }
  else
  {
    /* Kick off Pmic CFG file interpreter */
    EnumKeyValues(Pd, NULL, ChargerPlatform_ReadCfgParams);
  }

  /* Clean up resources */
  CloseParser(Pd);

  if(pFileBuffer != NULL)
  {
    Status = gBS->FreePool(pFileBuffer);
  }

  return Status;

}

/**
ChargerPlatformFile_ReadDefaultCfgData()

@brief
Read Battery Parameter Default Configurations from configuration file
*/
EFI_STATUS
ChargerPlatformFile_ReadDefaultCfgData
(
  VOID
)
{
  EFI_STATUS  Status     = EFI_SUCCESS;
  UINT8*      FileBuffer = NULL;
  UINTN       FileSize   = 0;
  INTN        Pd;

  Status = ReadFromFV(CHARGER_CFG_FILE_IN_FV, (void **) &FileBuffer, &FileSize);
  if (Status == EFI_SUCCESS)
  {
    Pd = OpenParser (FileBuffer, (UINT32)FileSize, NULL);

    if (Pd < 0)
    {
      CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a  Charger Config- Parser open failed\n", __FUNCTION__));
      Status = EFI_LOAD_ERROR;
    }
    else
    {
      /* Kick off Pmic CFG file interpreter */
      EnumKeyValues (Pd, NULL, ChargerPlatform_ReadCfgParams);
    }

    /* Clean up resources */
    CloseParser(Pd);
    FreePool(FileBuffer);
  }

  /*Read from Debug (LogFS or EFIESP) partition to over write value - supported for debug mode*/
  if (!RETAIL) 
  {
    ChargerPlatformFile_OverwriteDefaultCfg();
  }

  return Status;
}


/**
ChargerPlatformFile_ReadBatteryProfile()

@brief
Read Battery Profile from Main FV and if debug supports then read from Debug LogFS partition
*/
EFI_STATUS
ChargerPlatformFile_ReadBatteryProfile( QCOM_CHARGER_PLATFORM_BATT_PROFILE_DATA *pBattProfileInfo)
{
  EFI_STATUS  Status = EFI_SUCCESS;

  if(!pBattProfileInfo)
    return EFI_INVALID_PARAMETER;

  /* Attempt to Read battery profile from respective locatios */
  Status |= ChargerPlatformFile_ReadBattProfileFile(pBattProfileInfo);
  if((EFI_SUCCESS == Status ) && (NULL != pBattProfileInfo->pFileBuffer))
  {
    CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a  FG Battery Profile Read Success %r Size = %d \r\n", __FUNCTION__, Status, pBattProfileInfo->DataSize));
  }
  else
  {
    CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a  FG Battery Profile overwriting Failed %r \r\n", __FUNCTION__, Status));
  }

  return Status;
}

/**
ChargerPlatformFile_InitFileLog(BOOLEAN )

@brief  Gets File Log Info
Returns Status
*/
EFI_STATUS ChargerPlatformFile_GetLogInfo(QCOM_CHARGER_PLATFORM_LOG_INFO *pFileLogInfo)
{
  EFI_STATUS Status  =  EFI_SUCCESS;
  CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a \r\n", __FUNCTION__));

  if(!pFileLogInfo)
    return EFI_INVALID_PARAMETER;

  if (!RETAIL)
  {
    /* Enable default file logging if production mode is disabled */
    Status = ChargerLib_GetLogInfo((chargerlib_loginfo * )pFileLogInfo);
    CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a ChargerLib_GetFileLogHandle Status = %r \r\n", __FUNCTION__, Status));
  }

  return Status;
}



/** 
  @file  ChargerPlatform_File.h
  @brief Charger Lib API definitions.
*/
/*=============================================================================
  Copyright (c) 2016 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/14/16   va      Adding File log support to Charger App
 05/20/16   va      Update for Profile read from FV, LogFs or EFIESP 
 03/28/16   va      updating file for platform init, file log related changes
 01/27/16   sm      Initial revision
=============================================================================*/

#ifndef __CHARGERPLATFORM_FILE_H__
#define __CHARGERPLATFORM_FILE_H__

#include "QcomChargerPlatform.h"

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/

/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
INT32 ChargerPlatformFile_AsciiToInt( CHAR8 *Str );

UINT32 ChargerPlatformFile_AsciiStrToHex (CHAR8* Str, UINT32 StrSize);

EFI_STATUS ChargerPlatformFile_ReadDefaultCfgData( VOID );

EFI_STATUS ChargerPlatformFile_FileLogInit( QCOM_CHARGER_PLATFORM_CFGDATA_TYPE ChargerPlatformCfgData );

EFI_STATUS ChargerPlatformFile_ReadBatteryProfile( QCOM_CHARGER_PLATFORM_BATT_PROFILE_DATA *pBattProfileInfo);

EFI_STATUS ChargerPlatformFile_GetLogInfo(QCOM_CHARGER_PLATFORM_LOG_INFO *pFileLogInfo);

#endif  /* __CHARGERPLATFORM_FILE_H__ */

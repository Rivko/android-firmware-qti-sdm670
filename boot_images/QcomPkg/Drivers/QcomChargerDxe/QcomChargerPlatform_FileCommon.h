/** 
  @file  ChargerPlatform_FileCommon.h
  @brief Charger platform File definitions.
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
 05/13/16   va      Initial revision
=============================================================================*/

#ifndef __CHARGERPLATFORM_FILE_LA_H__
#define __CHARGERPLATFORM_FILE_LA_H__

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
EFI_STATUS ChargerPlatformFile_ReadBattProfileFile( QCOM_CHARGER_PLATFORM_BATT_PROFILE_DATA *pBattProfileInfo);
EFI_STATUS ChargerPlatformFile_DebugReadFile(CHAR16 *pFilePath, UINT8 **pFileBuffer, UINTN *pDataSize);

#endif  /* __CHARGERPLATFORM_FILE_LA_H__ */

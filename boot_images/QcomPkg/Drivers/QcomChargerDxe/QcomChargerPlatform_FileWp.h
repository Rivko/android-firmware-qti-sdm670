/** 
  @file  ChargerPlatform_FileWp.h
  @brief Charger Platform File API definitions.
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

#ifndef __CHARGERPLATFORM_FILE_WP_H__
#define __CHARGERPLATFORM_FILE_WP_H__

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
EFI_STATUS ChargerPlatformFile_ReadFile(QCOM_CHARGER_PLATFORM_BATT_PROFILE_DATA *pBattProfileInfo);

#endif  /* __CHARGERPLATFORM_FILE_WP_H__ */

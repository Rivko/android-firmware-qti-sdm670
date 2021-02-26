/*! \file  SMB350Lib.h 
 *  \n
 *  \brief  SMB350 MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the SMB350 Charger module.
 *
 *
 *  &copy; Copyright 2012 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 *         Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 03/06/13   dy      Added APIs to detect Battery Presence and Charger Connected
 11/06/12   dy      created file
===========================================================================*/

#ifndef __SMB350Lib_H__
#define __SMB350Lib_H__

/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * @brief This function initializes SMB350 Charger driver
 *
 * @param  
 * 
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
**/
EFI_STATUS EFI_SMB350Init(
  void
);

/**
 * @brief This function returns battery presence
 *
 * @param[out]  BattPresent - Battery Presence
                              TRUE - Battery is present
                              FALSE - Battery is not present
 * 
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
**/
EFI_STATUS Smb350IsBatteryPresent(BOOLEAN *BattPresent);

/**
 * @brief This function returns the connected charger state
 *
 * @param[out]  ChgConnected - Charger Connected
                               TRUE  - Charger is connected
                               FALSE - Charger is not connected
 * 
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
**/
EFI_STATUS Smb350IsChargerConnected(BOOLEAN *ChgConnected);

#endif /*__SMB350Lib_H__*/


/*=======================================================================*//**
 * @file        qusb_ldr_utils.c
 * @author:     kameya
 * @date        11-Sept-2015
 *
 * @brief       Stub implementation to check forced PBL DLOAD mode 
 *              when D+ is grounded and to put USB PHY non-drive mode.
 *
 * @details     This file contains the implementation of the APIs to be used by
 *              as stub by device programmer.
 *
 * @note        
 *
 *              Copyright 2015 QUALCOMM Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/

// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 2015-09-11   kameya   First Draft
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "com_dtypes.h"           // common defines - basic types as uint32 etc
#include "qusb_ldr_utils.h"
// ===========================================================================
/**
 * @function    qusb_pbl_dload_check
 * 
 * @brief   This function will check if D+ is grounded. And if it is connected
 *          to ground, then we go into EDL.
 * 
 * @param   None
 * 
 * @return  TRUE: if it is connected to GND
 *          FALSE: if it is not connected to GND. 
 * 
 */
// ===========================================================================
boolean qusb_ldr_utils_fedl_check(void)
{
  return FALSE;
}

// ===========================================================================
/**
 * @function    qusb_ldr_utils_hs_phy_nondrive_mode_set
 * 
 * @brief   This function will put High Speed QUSB2.0 PHY 
 *          into non-drive state / mode.
 * 
 * @param   None
 * 
 * @return  None
 * 
 */
// ===========================================================================
void qusb_ldr_utils_hs_phy_nondrive_mode_set(void)
{
  return;
}
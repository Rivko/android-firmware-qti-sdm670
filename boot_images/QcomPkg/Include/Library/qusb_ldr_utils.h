#ifndef QUSB_LDR_UTILS_H
#define QUSB_LDR_UTILS_H

/*=======================================================================*//**
 * @file        qusb_pbl_dload_check.h
 * @author:     kameya
 * @date        13-Feb-2015
 *
 * @brief       QUSB check to enter PBL DLOAD mode when D+ is grounded.
 *
 * @details     This file contains the declaration of PBL DLOAD check API.
 *
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
// when          who        what, where, why
// --------   ---        ----------------------------------------------------------
// 02/13/15   kameya  Initial revision
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "com_dtypes.h"           // common defines - basic types as uint32 etc

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------

// ===========================================================================
/**
 * @function    qusb_pbl_dload_check
 * 
 * @brief   This function checks if the USB D+ line is grounded.
 *
 * 
 * @param   None.
 * 
 *
 * @return  boolean - returns whether we should enter PBL EDL mode.
 * 
 */
// ===========================================================================
boolean  qusb_ldr_utils_fedl_check(void);

// ===========================================================================
/**
 * @function    qusb_ldr_utils_hs_phy_nondrive_mode_set
 * 
 * @brief   This function puts the HS PHY into non-drive mode with
 *          pull-up and pull-down on data lines disabled.
 *
 * 
 * @param   None.
 * 
 *
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_ldr_utils_hs_phy_nondrive_mode_set(void);

#endif /* QUSB_PBL_DLOAD_CHECK_H */

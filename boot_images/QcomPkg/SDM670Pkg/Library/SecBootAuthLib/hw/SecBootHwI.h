#ifndef SECBOOT_HW_I_H
#define SECBOOT_HW_I_H
/*****************************************************************************
*
* @file  secboot_hw_i.h (Secboot Hardware internal API)
*
* @brief API to read Security Control Fuses containing authentication
*        information
*       
* Copyright (c) 2010-2014 by QUALCOMM, Technology Inc.  All Rights Reserved.
*
*****************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/28/14   hw      Add RoT support
09/02/13   hw      Init version 

============================================================================*/

/**
 * @brief This function returns the fuse value for the index of current sw root 
 *        of trust.
 * 
 * @param[in,out]  current_sw_rot    Pointer to a uint32 which will be
 *                                   populated with the CURRENT_ROT_SW
 *                                   fuse value. The uint32 is allocated by
 *                                   the caller.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned 
 * 
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses, Please check go/ipcat 
 *  for fuse details and contact support@qualcomm.com for more document information.
 *
 */
secboot_hw_etype secboot_hw_get_current_sw_rot
(
  uint32* current_sw_rot
);


/// @}
//
/// @}
#endif //SECBOOT_HW_I_H

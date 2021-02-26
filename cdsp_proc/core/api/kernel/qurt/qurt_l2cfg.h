#pragma once
/**
  @file qurt_l2cfg.h 
  @brief Operations on L2 configuration registers

EXTERNAL FUNCTIONS
   qurt_l2cfg_set 
   qurt_l2cfg_get 

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

==============================================================================*/

/**
 * @ingroup qurt_l2cfg_set 
 * Set value of a L2 configuration register. A register can be set *IFF* its 
 * initial value is configured
 *  
 * @param[in] offset Offset of L2 configuration register. Must be multiple of 4
 * @param[in] value  Value to set the register to 
 *  
 * @return 
 * #QURT_EOK -- Success \n 
 * #QURT_EFAILED -- Internal mapping covering L2CFG register file absent! Most 
 * likely a configuration problem \n
 * #QURT_EINVALID -- Argument error \n
 * #QURT_ENOTALLOWED -- Setting of this register is prohibited
 *  
 * @dependencies 
 * None 
 *  
 */
extern int qurt_l2cfg_set (unsigned short offset, unsigned int value);

/**
 * @ingroup qurt_l2cfg_get 
 * Get value of a L2 configuration register 
 *  
 * @param[in]  offset Offset of L2 configuration register. Must be multiple of 4
 * @param[out] value  Pointer to value of the register 
 *  
 * @return 
 * #QURT_EOK -- Success \n 
 * #QURT_EFAILED -- Internal mapping covering L2CFG register file absent! Most 
 * likely a configuration problem \n 
 * #QURT_EINVALID -- Argument error \n 
 *  
 * @dependencies 
 * None 
 *  
 */
extern int qurt_l2cfg_get (unsigned short offset, unsigned int * value);

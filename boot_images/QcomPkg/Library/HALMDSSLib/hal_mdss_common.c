/*=============================================================================
 
  File: hal_mdss_common.c
 
  Common mdss driver
  
 
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "hal_mdss_common.h"


/*============================================================================
*       HAL Globals
============================================================================*/

/* These are mapped addresses to the register bases supplied by OS */
uintPtr MDSS_BASE_PTR = 0;

uintPtr SEC_CTRL_BASE_PTR = 0;

/*============================================================================
*       HAL Private Functions
============================================================================*/
      

/*============================================================================
*       HAL Public Functions
============================================================================*/


/****************************************************************************
HAL_MDSS_SetBaseAddress()

Description:
*
* Setups global register base address pointers
*/
void HAL_MDSS_SetBaseAddress(HAL_MDP_SWMappedHWBaseAddressesType *pMappedBaseAddress)
{

    if (0 != pMappedBaseAddress->uMdpAddress)
    {
        MDSS_BASE_PTR      = pMappedBaseAddress->uMdpAddress;
        SEC_CTRL_BASE_PTR  = pMappedBaseAddress->uSecCtrlAddress;
    }
}



#ifdef __cplusplus
}
#endif

/*! \file */

/*
===========================================================================

FILE:         hal_mdss.c

DESCRIPTION:  MDP Initialization
  

===========================================================================

                             Edit History
$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/HALMDSSLib/hal_mdss.c#1 $
$DateTime: 2017/09/18 09:26:13 $
$Author: pwbldsvc $

===========================================================================
  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdss.h"
#include "mdsshwio.h"

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
*
** FUNCTION: HAL_MDSS_IsInterruptActive()
*/
/*!
* \brief
*     HAL_MDSS_IsInterruptActive --- Report MDSS cores that have active interrupts
*
* \param [out]   pModuleIDs            - Reports the list of modules that have active interrupts
* \param [in]    uFlags                - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDSS_IsInterruptActive(uint32              *pModuleIDs,
                                                    uint32               uFlags)
{
   HAL_MDSS_ErrorType   eStatus         = HAL_MDSS_STATUS_SUCCESS;
   uint32               uMDSSIntrStatus = HWIO_MDSS_HW_INTR_STATUS_IN;
   uint32               uModuleID       = 0;

   if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, MDP))
   {
      uModuleID |= HAL_MDSS_MODULE_MDP;
   }
   if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, DSI0))
   {
      uModuleID |= HAL_MDSS_MODULE_DSI0;
   }
   if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, DSI1))
   {
      uModuleID |= HAL_MDSS_MODULE_DSI1;
   }
   if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, HDMI))
   {
      uModuleID |= HAL_MDSS_MODULE_HDMI;
   }
   if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, EDP))
   {
      uModuleID |= HAL_MDSS_MODULE_DP;
   }
   if (HWIO_GETVAL(MDSS_HW_INTR_STATUS, uMDSSIntrStatus, ROT))
   {
      uModuleID |= HAL_MDSS_MODULE_ROT;
   }
   *pModuleIDs = uModuleID;
   
   return eStatus;
}

#ifdef __cplusplus
}
#endif

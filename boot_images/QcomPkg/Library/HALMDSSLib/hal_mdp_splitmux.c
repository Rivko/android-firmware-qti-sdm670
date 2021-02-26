/*! \file */

/*
===========================================================================

FILE:         hal_mdp_splitmux.c

DESCRIPTION:  This file contains a list of functions to work with split-muxer module

===========================================================================
===========================================================================
Copyright (c) 2012 - 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Global Data Definitions
 *------------------------------------------------------------------------ */

/*------------------------------------------------------------------------
 * Private Function Definitions
 *------------------------------------------------------------------------ */
/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_SplitMUX_BaseOffset()
*/
/*!
* \brief
*     Returns offset of the Split-MUX module
*
* \param [in] eSplitMUXId    - none, 0, 1, 2 etc.
*
* \retval uint32
*
****************************************************************************/
static uintPtr HAL_MDP_Get_SplitMUX_BaseOffset(HAL_MDP_SplitMUXId  eSplitMUXId)
{
   uintPtr   uRegOffset  = 0x00000000;

   if((HAL_MDP_SPLITMUX_NONE        == eSplitMUXId) ||
      (HAL_MDP_SPLITMUX_MAX         <= eSplitMUXId))
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      uRegOffset = uMDPSplitMUXRegBaseOffset[eSplitMUXId];
   }
   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SplitMUX_BufferConfig()
*/
/*!
* \brief
*     Split-MUX buffer module configurations
*
* \param [in]   eSplitMUXId            - Split-MUX (PPB) block id
* \param [in]   pSplitMUXBufferConfig  - Buffer configuration information
* \param [in]   uSetupFlags            - Reserved (must be 0x00)
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static void HAL_MDP_SplitMUX_BufferConfig(HAL_MDP_SplitMUXId                    eSplitMUXId,
                                          HAL_MDP_SplitMUX_BufferConfigType    *pSplitMUXBufferConfig,
                                          uint32                                uSetupFlags)
{
   uintPtr     uRegOffset        = 0;
   uint32      uRegVal           = 0;

   // get register base offset
   uRegOffset = HAL_MDP_Get_SplitMUX_BaseOffset(eSplitMUXId);

   if(FALSE == pSplitMUXBufferConfig->bSplitFifoEnable)
   {
      out_dword(HWIO_MDP_PPB0_CNTL_ADDR + uRegOffset, 0x00); // Buffer in bypss mode
   }
   else
   {
      uRegVal = HWIO_OUT_FLD(0x00, MDP_PPB0_CNTL, PPB0_MODE, 0X02);         // SplitFifo enable/disable
      out_dword(HWIO_MDP_PPB0_CNTL_ADDR + uRegOffset, uRegVal); // Horizonal Split Re-Order
   }

   uRegVal  = HWIO_OUT_FLD(0x00,    MDP_PPB0_CONFIG, PPB0_INTF_SPLIT_EN, pSplitMUXBufferConfig->bSplitFifoEnable);         // SplitFifo enable/disable
   uRegVal  = HWIO_OUT_FLD(uRegVal, MDP_PPB0_CONFIG, PPB0_INTF_SPLIT_SWAP, pSplitMUXBufferConfig->bSwapLeftRightStreams);  // Left-Right Swap
   uRegVal  = HWIO_OUT_FLD(uRegVal, MDP_PPB0_CONFIG, PPB0_INTF_SPLIT_SEL, pSplitMUXBufferConfig->eSecondaryIntfId);        // Seconday Interface ID
   uRegVal  = HWIO_OUT_FLD(uRegVal, MDP_PPB0_CONFIG, PPB0_SPLIT_OVERLAP_WIDTH, pSplitMUXBufferConfig->uOverlapPixels);     // Overlapped pixels

   out_dword(HWIO_MDP_PPB0_CONFIG_ADDR + uRegOffset, uRegVal);
}

/*------------------------------------------------------------------------
 * Public Function Definitions
 *------------------------------------------------------------------------ */

/****************************************************************************
*
** FUNCTION: HAL_MDP_SplitMUX_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular split-muxer (PPB) block
*
* \param [in]   eSplitMUXId      - Split-MUX (PPB) block id
* \param [in]   pSplitMUXConfig  - Configuration for the selected PPB block
* \param [in]   uSetupFlags      - Reserved (must be 0x00)
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_SplitMUX_Setup(HAL_MDP_SplitMUXId             eSplitMUXId,
                                          HAL_MDP_SplitMUXConfigType    *pSplitMUXConfig,
                                          uint32                         uSetupFlags)
{
   HAL_MDSS_ErrorType   eStatus  = HAL_MDSS_STATUS_SUCCESS;

   if(NULL != pSplitMUXConfig->pSplitMUXBufferConfig)
   {
      HAL_MDP_SplitMUX_BufferConfig(eSplitMUXId, pSplitMUXConfig->pSplitMUXBufferConfig, 0x00);
   }

   return eStatus;
}


#ifdef __cplusplus
}
#endif

/*========================================================================
  This file contains AFE SSR handler related API's

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: 
 ====================================================================== */
#ifndef _AFE_SSR_DRV_H_
#define _AFE_SSR_DRV_H_

/*==========================================================================
  Include Files
  ========================================================================== */

#include "qurt_elite.h"

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

/*==========================================================================
  Function declarations
  ========================================================================== */
/**
  Initialize the SSR handler for resetting audio DMA's

  @param[in]  None

  @result
  None

  @dependencies
  None.
*/
ADSPResult afe_ssr_handler_init();

/**
  Update the LPAIF DMA power ON/OFF status.

  @param[in]  dma_idx,  dma index
  @param[in]  dir,  rd or wr dma
  @param[in]  pwr_status, on (True), off (False)

  @result
  None

  @dependencies
  None.
*/
void afe_ssr_update_lpass_dma_status(uint32_t dma_type, uint32_t dir, uint32_t dma_idx, bool_t pwr_status);



#endif /* _AFE_SSR_DRV_H_ */

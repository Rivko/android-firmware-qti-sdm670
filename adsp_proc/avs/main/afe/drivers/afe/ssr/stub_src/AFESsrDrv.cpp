/*========================================================================
   This file contains AFE SSR handler implementation

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/ssr/stub_src/AFESsrDrv.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */

#include "qurt_elite.h"
#include "AFESsrDrv.h"


/*----------------------------------------------------------------------------
 * Function definition
 * -------------------------------------------------------------------------*/

ADSPResult afe_ssr_handler_init()
{      
   return ADSP_EUNSUPPORTED;
}

void afe_ssr_update_lpass_dma_status(uint32_t dma_type, uint32_t dir, uint32_t dma_idx, bool_t pwr_status)
{
   return;
}



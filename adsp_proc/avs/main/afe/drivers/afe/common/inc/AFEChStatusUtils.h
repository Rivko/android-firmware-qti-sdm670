/*========================================================================
  This file contains high level functions that are shared inside afe
  across drivers and services

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/inc/AFEChStatusUtils.h#1 $
 ====================================================================== */
#ifndef _AFE_CHSTATUS_UTILS_H_
#define _AFE_CHSTATUS_UTILS_H_

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "AFEInternal.h"
#include "mmdefs.h"

#if defined(__cplusplus)
extern "C" {
#endif


typedef struct chstatus_check_point
{
   uint16_t                  data_format;   
   /**< Supported data type.
     @values
      - #AFE_LINEAR_PCM_DATA
      - #AFE_NON_LINEAR_DATA*/

   uint32_t                  sample_rate;

   uint16_t                  num_channels;

   uint16_t                  bit_width;
} chstatus_check_point_t;

ADSPResult afe_validate_chstatus_cfg(afe_param_id_chstatus_cfg_t *p_spdif_chstatus_ptr,chstatus_check_point_t *p_chsts_check_param);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif // !defined(_AFE_CHSTATUS_UTILS_H_)


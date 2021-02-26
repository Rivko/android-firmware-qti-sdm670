/** @file HALaudioif.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pcmif/hal_v3/stub_src/AFEHalPcmV3_stub.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEPcmDriver_i.h"
#include "AFEHalPcm.h"
#include "AFEPcmHalFuncDefs.h"




void hal_pcm_get_func_table_v3(afe_pcm_hal_func_def_t *func_table)
{
   if(NULL == func_table)
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_pcm_get_func_table_v3 func_table is NULL"); 
     return;
   }
   
   func_table->hal_pcm_init_fptr                = NULL;
   func_table->hal_pcm_reset_fptr               = NULL; 
   func_table->hal_pcm_reset_release_fptr       = NULL;
   func_table->hal_pcm_config_fptr              = NULL;
   func_table->hal_pcm_enable_fptr              = NULL;
   func_table->hal_pcm_disable_fptr             = NULL;
   func_table->hal_pcm_get_intf_fptr            = NULL;

   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_pcm_get_func_table_v3 stub hal func_table set to NULL"); 

}



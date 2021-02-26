/** @file AFEHalTdm.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/hal_v1/stub_src/AFEHalTdmV1_stub.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
#include "qurt_elite.h"
#include "AFEHalTdmFuncDefs.h"

void hal_tdm_get_func_table_v1(afe_tdm_hal_func_def_t *func_table)
{
   if(NULL == func_table)
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_pcm_get_func_table_v1 func_table is NULL");
     return;
   }

   func_table->hal_tdm_init_fptr                = NULL;
   func_table->hal_tdm_reset_fptr               = NULL;
   func_table->hal_tdm_reset_release_fptr       = NULL;
   func_table->hal_tdm_config_fptr              = NULL;
   func_table->hal_tdm_enable_fptr              = NULL;
   func_table->hal_tdm_disable_fptr             = NULL;
   func_table->hal_tdm_get_intf_fptr            = NULL;
}

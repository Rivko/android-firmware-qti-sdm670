/** @file HALaudioif.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/hal_v2/stub_src/AFEHalI2sV2_stub.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */

#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEI2sDriver_i.h"
#include "AFEHalI2s.h"
#include "AFEI2sHalFuncDefs.h"


void hal_i2s_get_func_table_v2(afe_i2s_hal_func_def_t *func_table)
{
   if(NULL == func_table)
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_i2s_get_func_table_v2 func_table is NULL"); 
     return;
   }
  
   func_table->hal_i2s_init_fptr                 = NULL;
   func_table->hal_i2s_config_fptr               = NULL;
   func_table->hal_i2s_enable_fptr               = NULL;
   func_table->hal_i2s_disable_fptr              = NULL;
   func_table->hal_i2s_get_intf_fptr             = NULL;
   
   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_i2s_get_func_table_v2 stub hal func_table set to NULL");    
}




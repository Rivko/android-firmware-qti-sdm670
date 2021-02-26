/*=============================================================================

FILE:         qdss_ctrl_etm_stub.c

DESCRIPTION:  ETM not applicable

================================================================================
              Copyright (c) 2013-2016 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include <stdio.h>
#include "qdss_control.h"
#include "qdss_ctrl_etm.h"

void qdss_ctrl_etm_init_config_def(void)
{
   return;
}

int  qdss_ctrl_etm_get_config(const char *get_str,
                              char *settings_str,
                              size_t max_len)
{
   return QDSS_CONTROL_RVAL_NOT_SUPPORTED;
}
int qdss_ctrl_etm_set_config(const char *get_str)
{
   return QDSS_CONTROL_RVAL_NOT_SUPPORTED;
}

int qdss_ctrl_etm_get_param(uint16 param_id, uint32 *val)
{
   return QDSS_CONTROL_RVAL_NOT_SUPPORTED;
}

int qdss_ctrl_etm_set_param(uint16 param_id, uint32 val)
{
   return QDSS_CONTROL_RVAL_NOT_SUPPORTED;
}

int qdss_ctrl_etm_enable(void)
{
   return 1;
}

int qdss_ctrl_etm_disable(void)
{
   return 1;
}





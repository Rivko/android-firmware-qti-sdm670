/*=============================================================================

FILE:         qdss_control_stub.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "qdss.h"

uint32 qdss_get_ssc_tsctr_freq(void) 
{
   return 0;
}

int qdss_tfunnel_enable_port(const char *port_name)
{
   return 0;
}


int qdss_tfunnel_disable_port(const char *port_name)
{
   return 0;
}

int qdss_control_set_sink(uint8 sinkid)
{
   return 0;
}
void qdss_lpm_enter(void)
{
}
void qdss_lpm_exit(void)
{
}

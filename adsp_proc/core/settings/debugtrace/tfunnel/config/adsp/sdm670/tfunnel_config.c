/*=============================================================================

FILE:         tfunnel_config.c

DESCRIPTION:  
  This file has port configurations for trace funnels for a given chipset.
================================================================================
            Copyright (c) 2013 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/settings/debugtrace/tfunnel/config/adsp/sdm670/tfunnel_config.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2/7/2011   prasads  First draft created. 
===========================================================================*/
#include "com_dtypes.h"



#define IN_FUN0  0
#define IN_FUN1  1
#define IN_FUN2  2
#define MERG_FUN 3
#define SWAO_FUN 4
#define SSC_FUN  5

typedef struct TypeTFunnelPortEnableConfig TFunnelPortEnableConfig;

/*
This structure defines the topology of a funnel configuration.
*/
struct TypeTFunnelPortEnableConfig{
   uint32 funnel_id;
   uint32 port_id;
   TFunnelPortEnableConfig *pFunnelConfig;
};

/*
Define supported funnel configurations.
*/

static TFunnelPortEnableConfig tfunnel_merge_port1[]={
   {MERG_FUN,1,NULL}
};

static TFunnelPortEnableConfig tfunnel_merge_port2[]={
   {MERG_FUN,2,NULL}
};

TFunnelPortEnableConfig tfunnel_port_ssc_swao[]={
   {IN_FUN2,1,tfunnel_merge_port2}
};

TFunnelPortEnableConfig tfunnel_port_ssc[]={
   {SWAO_FUN,6,NULL}
};

TFunnelPortEnableConfig tfunnel_port_ssc_etm[]={   
    {SSC_FUN,0,tfunnel_port_ssc}
};

TFunnelPortEnableConfig tfunnel_port_lpass_etm[]={   
    {IN_FUN1,0,tfunnel_merge_port1}
};

TFunnelPortEnableConfig tfunnel_port_ssc_stm[]={   
    {SSC_FUN,1,tfunnel_port_ssc}
};

TFunnelPortEnableConfig tfunnel_port_lpass_tpda_ssc[]={
   {IN_FUN1,0,tfunnel_merge_port1}
};

TFunnelPortEnableConfig tfunnel_port_lpass_tpda_swao[]={
   {SSC_FUN,5,tfunnel_port_lpass_tpda_ssc}
};

TFunnelPortEnableConfig tfunnel_port_lpass_tpda[]={
   {SWAO_FUN,6,tfunnel_port_lpass_tpda_swao}
};
int qdss_get_ssc_fun_config(uint32 *fun, uint32 *port)
{
   *fun = SWAO_FUN;
   *port = 6;
   return 0;
}






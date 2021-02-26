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

$Header: //components/rel/core.qdsp6/2.1/settings/debugtrace/tfunnel/config/adsp/sdm830/tfunnel_config.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2/7/2011   prasads  First draft created. 
===========================================================================*/
#include "com_dtypes.h"



#define IN_FUN0  0
#define IN_FUN2  1
#define MERG_FUN 2
#define SWAO_FUN 3
#define SSC_FUN  4

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
static TFunnelPortEnableConfig tfunnel_merge_port0[]={
   {MERG_FUN,0,NULL}
};


TFunnelPortEnableConfig tfunnel_port_stm[]={
   {IN_FUN0,7,tfunnel_merge_port0}
};










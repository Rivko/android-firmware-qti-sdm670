/*=============================================================================

FILE:         QdssEtfEtb.c

DESCRIPTION:

================================================================================
              Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Drivers/QdssDxe/QdssEtfEtb.c#1 $
==============================================================================*/

#include "HalQdssTmc_EtfEtb.h"


void QdssEtfEtb_PreInit(void)
{
  HAL_qdss_etfetb_HalInit();
}

void QdssEtfEtb_Init(void)
{
  HAL_qdss_etfetb_AccessUnLock();

#ifdef QDSS_INIT_ETR
  HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_HWFIFO);
#else
  HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_CBUF);
#endif

  HAL_qdss_etfetb_SetFFCRETBDefault();
  HAL_qdss_etfetb_EnableTraceCapture();

  HAL_qdss_etfetb_AccessLock();
}

void QdssEtfEtb_Flush(void)
{
  HAL_qdss_etfetb_AccessUnLock();
  HAL_qdss_etfetb_Flush();
  HAL_qdss_etfetb_FlushWait();
  HAL_qdss_etfetb_AccessLock();
}



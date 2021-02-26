/*=============================================================================

FILE:         TPDA.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "TPDA.h"
#include "halqdss_tpda.c"  //lite_inline 
#include "qdss_tpda_config.h"

void TPDAInit(void) 
{
   HAL_qdss_tpda_HalConfigInit(QDSS_SWAO_TPDA);
}

void TPDASetTimestampFreq(uint32 ts_counter_freq)
{
   HAL_qdss_tpda_AccessUnLock();
   HAL_qdss_tpda_SetTimeStampFreq(ts_counter_freq);
   HAL_qdss_tpda_AccessLock();
}

void TPDAFreqReq(void)
{
   HAL_qdss_tpda_AccessUnLock();
   HAL_qdss_tpda_freq_pkt_req();
   HAL_qdss_tpda_AccessLock();
}


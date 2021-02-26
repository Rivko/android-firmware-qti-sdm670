#ifndef HALQDSS_TPDA_H
#define HALQDSS_TPDA_H

/*=============================================================================

FILE:         halqdss_tpda.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "HALcomdef.h"

void  HAL_qdss_tpda_HalConfigInit(uint32 nBaseAddr);
void  HAL_qdss_tpda_AccessUnLock(void);
void  HAL_qdss_tpda_AccessLock(void);
void  HAL_qdss_tpda_SetTimeStampFreq(uint32 nFreq);
void HAL_qdss_tpda_freq_pkt_req(void);



#endif //HALQDSS_TPDA_H

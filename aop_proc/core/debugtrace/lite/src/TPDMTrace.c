/*=============================================================================

FILE:         TPDMTrace.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2016 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/


#include "hal_qdss_tpdm_mcmb.h"

#define QDSS_AOP_TPDM_ESC    0xFEEEFA00
#define QDSS_AOP_MAX_REPEAT  0xFF


static struct {
   uint32 count;   //total words  written
   uint32 previous_data;
   uint8  repeat_count;
}tpdm_trace;

void WriteData(uint32 data) 
{
   if (tpdm_trace.repeat_count > 0) {
      HAL_tpdm_write_data(QDSS_AOP_TPDM_ESC|(tpdm_trace.repeat_count));
   }
   if (QDSS_AOP_TPDM_ESC==(data & 0xFFFFFF00)) {
      HAL_tpdm_write_data(QDSS_AOP_TPDM_ESC);
      HAL_tpdm_write_data(0);
   }
   HAL_tpdm_write_data(data);
   tpdm_trace.count++;
   tpdm_trace.previous_data=data;
   tpdm_trace.repeat_count=0;

}

void TPDMTrace_Data32(uint32 data) 
{
   if ( (tpdm_trace.previous_data==data) && 
        (tpdm_trace.count > 0) &&
        (tpdm_trace.repeat_count < QDSS_AOP_MAX_REPEAT) ) {
         tpdm_trace.repeat_count++;
   }
   else {
      WriteData(data);
   }
}

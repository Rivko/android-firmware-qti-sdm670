/*=============================================================================

FILE:         STMTrace.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "qdss_lite.h"
#include "STMTrace.h"

#include "qdss_stm_config.h"


STMPortAddrType qdss_stm_base;

int8 _stm_ports[QDSS_STM_NUM_STIMULUS_PORTS];

__inline int STMTrace_AllocateWindow(void)
{
   qdss_stm_base =  QDSS_STM_STIMULUS_PORT_BASE_PHYS  + 
      (QDSS_STM_START_STIMULUS_PORT << 8);

   return QDSS_SUCCESS;
}


__inline int STMTrace_NewPort(STMPortAddrType * uPort)
{
   uint8 i = 0;

   if (0==qdss_stm_base) {
      return QDSS_ERROR;
   }
   
   while ((i < QDSS_STM_NUM_STIMULUS_PORTS) && (_stm_ports[i] != 0))
   {
      i++;
   }
   if(i == QDSS_STM_NUM_STIMULUS_PORTS){
      return QDSS_ERROR;  
         
   }

   _stm_ports[i] = 1;
   *uPort = qdss_stm_base + (i << 8);
 
   return QDSS_SUCCESS;
}


__inline int STMTrace_FreePort(STMPortAddrType  uPort)
{
   uint8 i = 0;
   
   if (0==qdss_stm_base) {
      return QDSS_ERROR;
   }

   i = (uint8)((uPort - qdss_stm_base) >> 8);

   if (i >= QDSS_STM_NUM_STIMULUS_PORTS) {
      return QDSS_ERROR;
   }

   _stm_ports[i] = 0;

   return QDSS_SUCCESS;
}

__inline void STMTrace_Data32(STMPortAddrType uPort, STMTraceDataType uType, uint32 uData)
{       
   *(volatile uint32 *)((uPort)|(0x98&(~uType)))=uData;
}


__inline void STMTrace_Data16(STMPortAddrType uPort, STMTraceDataType uType, uint16 uData)
{
   *(volatile uint16 *)((uPort)|(0x98&(~uType)))=uData;
}


__inline void STMTrace_Data8(STMPortAddrType uPort, STMTraceDataType uType, uint8 uData)
{
   *(volatile uint8 *)((uPort)|(0x98&(~uType)))=uData;
}


__inline void STMTrace_Flag(STMPortAddrType uPort, STMTraceFlagType uType)
{
   *(volatile uint32 *)((uPort)|(0xE8&(~uType)))=0;
}

__inline void STMTrace_Trigger(STMPortAddrType uPort, STMTraceTriggerType uType)
{
   *(volatile uint32 *)((uPort)|(0xF8&(~uType)))=0;
}

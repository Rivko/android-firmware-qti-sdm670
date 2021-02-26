/*=============================================================================

FILE:         QdssStmTrace.c

DESCRIPTION:

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "HalQdssStm.h"
#include "QdssStmTrace.h"

//TBD This needs to be mutex protected.
//It is left out now because the immediate use cases
//do not require it.
//Must use HAL_qdss_stm_GetNumStimulusPorts() to obtain the size of the array.
static uint8 *_stm_port_in_use_array = NULL;

int QdssStmTrace_NewPort(STMPortAddrType *uPort)
{
  uint32 i = 0;
  STMPortAddrType port_start_addr = HAL_qdss_stm_GetStimulusPortStart();
  uint32 num_ports = HAL_qdss_stm_GetNumStimulusPorts();
  _stm_port_in_use_array = HAL_qdss_stm_GetStmPortInUseArray();

  while ((i < num_ports) && (_stm_port_in_use_array[i] != 0)) {
    i++;
  }
  if (i == num_ports) {
    return -1;
  }
  _stm_port_in_use_array[i] = 1;
  *uPort = port_start_addr + (i << 8); // Multiply by 256 bytes per port

  return 0;
}


void QdssStmTrace_FreePort(STMPortAddrType uPort)
{
  uint32 i;
  STMPortAddrType port_start_addr = HAL_qdss_stm_GetStimulusPortStart();
  uint32 num_ports = HAL_qdss_stm_GetNumStimulusPorts();
  _stm_port_in_use_array = HAL_qdss_stm_GetStmPortInUseArray();

  i = (uPort - port_start_addr) >> 8;   // divide by 256
  if (i < num_ports) {
    _stm_port_in_use_array[i] = 0;
  }
}


void QdssStmTrace_Data32(STMPortAddrType uPort, STMTraceDataType uType, uint32 uData)
{
  *(volatile uint32 *)((uPort) | (0x98 & (~uType))) = uData;
}


void QdssStmTrace_Data16(STMPortAddrType uPort, STMTraceDataType uType, uint16 uData)
{
  *(volatile uint16 *)((uPort) | (0x98 & (~uType))) = uData;
}


void QdssStmTrace_Data8(STMPortAddrType uPort, STMTraceDataType uType, uint8 uData)
{
  *(volatile uint8 *)((uPort) | (0x98 & (~uType))) = uData;
}


void QdssStmTrace_Flag(STMPortAddrType uPort, STMTraceFlagType uType)
{
  *(volatile uint32 *)((uPort) | (0xE8 & (~uType))) = 0;
}

void QdssStmTrace_Trigger(STMPortAddrType uPort, STMTraceTriggerType uType)
{
  *(volatile uint32 *)((uPort) | (0xF8 & (~uType))) = 0;
}

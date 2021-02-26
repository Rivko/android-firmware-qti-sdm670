#ifndef __PMICARB_H__
#define __PMICARB_H__
/**
 * @file:  PmicArb.h
 * @brief: 
 * 
 * Copyright (c) 2013-2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/core/hal/PmicArb.h#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 10/1/13  Initial Version
 */


#include "SpmiTypes.h"
#include "SpmiUtils.h"
#include "SpmiHal.h"
#include "SpmiInfo.h"

typedef struct 
{
    uint32 irqStatus;
    uint32 geniStatus;
    uint32 geniCtrl;
} PmicArbHwStatus;


Spmi_Result PmicArb_Init(SpmiInfo *pSpmiInfo);
Spmi_Result PmicArb_FindChannel(SpmiDevInfo *pDev, uint8 slaveId, uint8 periph, uint8 owner, uint16* channel);
Spmi_Result PmicArb_FindPeriph(SpmiDevInfo *pDev, uint16 channel, uint8* slaveId, uint8* periph);
Spmi_Result PmicArb_FindVrmId(SpmiDevInfo *pDev, uint8 slaveId, uint8 periph, uint16* vrmId);
Spmi_Result PmicArb_UpdateCache(SpmiInfo *pSpmiInfo);
Spmi_Result PmicArb_InvalidateCache(SpmiInfo *pSpmiInfo);
Spmi_Result PmicArb_RemoveCacheEntry(SpmiDevInfo *pDev, uint16 chan);
Spmi_Result PmicArb_AddCacheEntry(SpmiDevInfo *pDev, uint16 chan, uint8 sid, uint8 periph);
Spmi_Result PmicArb_ExecuteCommand(SpmiDevInfo *pDev,
                                   PmicArbCmd cmd,
                                   Spmi_AccessPriority priority,
                                   uint8 owner,
                                   uint16 channel,
                                   uint16 offset,
                                   uint8* data,
                                   uint32 dataLen,
                                   uint32* bytesTransacted);
Spmi_Result PmicArb_LogAccessViolation(SpmiDevInfo *pDev, uint64 faultAddress);

Spmi_Result PmicArb_GetPeriphInfo(SpmiDevInfo *pDev, uint8 slaveId, uint8 periphId, 
                                  uint16* irqOwnerMask, uint16* periphOwnerMask);
Spmi_Result PmicArb_GetChannelInfo(SpmiDevInfo *pDev, uint16 channel, uint8* slaveId, 
                                   uint8* periphId, uint16* irqOwnerMask, uint16* periphOwnerMask);

void PmicArb_GetHwStatus(SpmiDevInfo *pDev, PmicArbHwStatus* status);

Spmi_Result
PmicArb_ExecuteCommandDebug ( uint8*              dbgBaseAddrs,
                              PmicArbCmd          cmd,
                              Spmi_AccessPriority priority,
                              uint8               slaveId,
                              uint16              address,
                              uint8*              data,
                              uint32              dataLen,
                              uint32*             bytesTransacted);

#endif

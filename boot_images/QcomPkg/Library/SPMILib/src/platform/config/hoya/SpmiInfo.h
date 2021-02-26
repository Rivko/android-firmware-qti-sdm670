#ifndef __SPMIINFO_H__
#define __SPMIINFO_H__

/*===========================================================================

@file:  SpmiInfo.h
@brief:

Copyright (c) 2013-2017 by Qualcomm Technologies Incorporated.
All Rights Reserved.

===========================================================================*/

/*===========================================================================

                    EDIT HISTORY

$DateTime:
$Header:
$Change:

when       who    what, where, why
--------   ---    ---------------------------------------------------------
03/09/16   mr     Initial Version
===========================================================================*/

/*===========================================================================

                    INCLUDE FILES

===========================================================================*/
#include "SpmiTypes.h"


/*===========================================================================

                    MACRO AND TYPE DEFINITIONS

===========================================================================*/
#define SPMI_BID2ADDR(SPMI_INFO,BID) ((SPMI_INFO)->devices[BID].baseAddrs )
#define SPMI_PDEV(SPMI_INFO,BID) (&(SPMI_INFO)->devices[(BID)] )

#define SPMI_NUM_BUSES_SUPPORTED(SPMI_INFO) ((SPMI_INFO)->uNumDevices)
#define SPMI_NUM_CHANNELS_SUPPORTED(PSPMI_DEV) ((PSPMI_DEV)->pGenerics->mNumChannels)
#define SPMI_NUM_PORT_PRIORITIES(PSPMI_DEV)     ((PSPMI_DEV)->pGenerics->mNumPortPriorities)
#define SPMI_NUM_PVC_PORTS(PSPMI_DEV)    ((PSPMI_DEV)->pGenerics->mNumPvcPorts)
#define SPMI_NUM_PVC_PPIDS(PSPMI_DEV)    ((PSPMI_DEV)->pGenerics->mNumPvcPortPPIDs)
#define SPMI_NUM_MASTERS(PSPMI_DEV)    ((PSPMI_DEV)->pGenerics->mNumMasters)
#define SPMI_NUM_PROG_RAM_REGS(PSPMI_DEV)    ((PSPMI_DEV)->pGenerics->mNumProgRamRegs)
#define SPMI_NUM_PIC_ACC_STATUS_REGS(PSPMI_DEV)    ((PSPMI_DEV)->pGenerics->mNumPicAccStatusRegs)
#define SPMI_NUM_VRM_IDS_SUPPORTED(PSPMI_DEV)    ((PSPMI_DEV)->pGenerics->mNumVrmIds)
#define SPMI_NUM_MGPI_PORTS(PSPMI_DEV)    ((PSPMI_DEV)->pGenerics->mNumMgpiPorts)


typedef struct SpmiDeviceGenerics {
   uint16 mNumChannels;
   uint16 mNumPortPriorities;
   uint16 mNumPvcPorts;
   uint16 mNumPvcPortPPIDs;
   uint16 mNumMasters;
   uint16 mNumProgRamRegs;
   uint16 mNumPicAccStatusRegs;
   uint16 mNumVrmIds;
   uint16 mNumMgpiPorts;
} SpmiDeviceGenerics;

typedef struct {
    uint8 sid : 5;
    uint8 owner : 5;
    uint8 irqOwnerFlag : 1;
    uint16 vrmId : 10;
    uint16 nextIdx : 11;
} ChannelMapEntry;

typedef struct {
    uint8*                    baseAddrs;
    uint8                     uDevIdx;
    uint16                    reservedChan;
    uint16                    nextChanIdx;
    boolean                   reservedChanEn;
    boolean                   dynamicChannelMode;
    uint16                   *periphMap;
    ChannelMapEntry          *channelMap;
    uint16                   *ownerMaskMap;
    SpmiDeviceGenerics       *pGenerics;
    void                     *target;
} SpmiDevInfo;

typedef struct {
    char*       dbgBaseAddrsStr;
    uint8*      dbgBaseAddrs;
} SpmiDbgDevInfo;

typedef void (*PmicArbIsrCallback)(uint8 bid, uint16 channel, uint8 mask);

typedef struct SpmiInfo {
  uint8               owner;
  boolean             hasInterruptSupport;
  PmicArbIsrCallback  isrCallback;
  uint16              rgOwnerMask;
  uint8               uNumDevices;
  SpmiDevInfo        *devices;
  SpmiDbgDevInfo     *dbgInfo;
} SpmiInfo;

#endif // SPMIINFO_H


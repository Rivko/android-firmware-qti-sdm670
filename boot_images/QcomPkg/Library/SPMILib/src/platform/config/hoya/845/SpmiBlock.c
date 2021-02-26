/**
 * @file:  SpmiBlock.c
 * 
 * Copyright (c) 2013-2017 by Qualcomm Technologies 
 * Incorporated. All Rights Reserved. 
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/platform/config/hoya/845/SpmiBlock.c#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#include "SpmiInfo.h"
#include "SpmiTypes.h"
#include "SpmiHal.h"
#include "SpmiGenericConsts.h"
#include "SpmiUtils.h"
#include "SpmiOsTarget.h"
#include "SpmiBusCfg.h"

#define PMIC_ARB_BASE_ADDRESS 0x0C400000
#define QDSS_PMIC_ARB_BASE_ADDRESS   0x06B22000
#define SPMI_BARE_OWNER_NUMBER     0

#define SPMI_MAX_CHANNELS_SUPPORTED   (HWIO_PMIC_ARB_CHNLn_CMD_MAXn + 1)
#define SPMI_MAX_BUSES_SUPPORTED      (1)
#define SPMI_MAX_OWNER_IDS            (HWIO_PMIC_ARB_OBSq_CHNLn_CMD_MAXq + 1)

static uint16 periphMap[SPMI_MAX_PERIPH_ID + 1];
static ChannelMapEntry channelMap[SPMI_MAX_CHANNELS_SUPPORTED];
static SpmiDeviceGenerics generics = INIT_SPMI_DEV_GENERICS;

static uint16 ownerMaskMap[SPMI_MAX_OWNER_IDS] = {
   SPMI_OWNER_APPS,    // ee id 0
   SPMI_OWNER_TZ,      // ee id 1
   SPMI_OWNER_MSS,     // ee id 2
   SPMI_OWNER_LPASS,   // ee id 3
   SPMI_OWNER_AOP,     // ee id 4
   SPMI_OWNER_SSC,     // ee id 5
   SPMI_OWNER_SECPROC, // ee id 6
};

static SpmiTargetConfig spmiTgt =
{
    .hwioAddrStr    = "AOSS",
    .interruptVec   = 0, // not used
};

SpmiDevInfo spmiDevices[] = {
   {
      .baseAddrs          = (uint8*)PMIC_ARB_BASE_ADDRESS,
      .uDevIdx            = 0,
      .reservedChan       = SPMI_MAX_CHANNELS_SUPPORTED - 1,
      .nextChanIdx        = 0,
      .reservedChanEn     = TRUE,   
      .dynamicChannelMode = FALSE,   
      .periphMap          = periphMap,
      .channelMap         = channelMap,
      .ownerMaskMap       = ownerMaskMap,
      .pGenerics          = &generics,
      .target             = (void*) &spmiTgt,    
   },
};

SpmiDbgDevInfo spmiDbgDevices = {
    //Update this for BOOT 
    .dbgBaseAddrsStr = NULL,
    .dbgBaseAddrs    = (uint8*)QDSS_PMIC_ARB_BASE_ADDRESS,
};


SpmiInfo spmiInfo = {
  .owner              = SPMI_BARE_OWNER_NUMBER,
  .rgOwnerMask        = SPMI_OWNER_TZ|SPMI_OWNER_MSS,
  .uNumDevices        = SPMI_MAX_BUSES_SUPPORTED,
  .devices            = spmiDevices,
  .dbgInfo            = &spmiDbgDevices,
};

SpmiInfo * pSpmiBlockInfo = &spmiInfo;


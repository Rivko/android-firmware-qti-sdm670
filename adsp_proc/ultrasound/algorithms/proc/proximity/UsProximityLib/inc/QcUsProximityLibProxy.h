#ifndef __QCUSPROXIMITYLIBPROXY_H__
#define __QCUSPROXIMITYLIBPROXY_H__

/*============================================================================
                           QcUsProximityLibProxy.h

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
11/20/14     RD      Initial version
============================================================================*/

/*----------------------------------------------------------------------------
  Include files
----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "qurt_elite.h"
#include "QcUsProximityLibApi.h"


#ifdef __cplusplus
extern "C" {
#endif

// name of dynamic sync proximity module, loaded from /system/lib/rfsa/adsp
#define SYNC_PROXIMITY_DYNAMIC_MODULE             "us-syncproximity.so"

#define PROXIMITY_LIBRARY_SUPPORTED_VERSION_NUM   1

/*============================================================================
  FUNCTION:  syncproximity_load_module
============================================================================*/
int syncproximity_load_module(void);

/*============================================================================
  FUNCTION:  syncproximity_unload_module
============================================================================*/
void syncproximity_unload_module(void);

/*============================================================================
  FUNCTION:  QcUsProximityLibGetSizes
============================================================================*/
int QcUsProximityLibGetSizes(ProximityCfg const *pConfig,
                             uint32_t *pWorkspaceSize,
                             uint32_t *pPatternSizeSamples);

/*============================================================================
  FUNCTION:  QcUsProximityLibInit
============================================================================*/
int QcUsProximityLibInit(ProximityCfg const *pConfig,
                         int8_t *pWorkspace,
                         uint32_t workspaceSize);

/*==============================================================================
  FUNCTION:  QcUsProximityLibGetPattern
==============================================================================*/
int QcUsProximityLibGetPattern(int16_t *pPattern, uint32_t patternSize);

/*============================================================================
  FUNCTION:  QcUsProximityLibSetDynamicConfig
============================================================================*/
int QcUsProximityLibSetDynamicConfig(int* pDynamicConfig,
                                     uint32_t dynamicConfigSize);


/*============================================================================
  FUNCTION:  QcUsProximityLibEngine
============================================================================*/
extern int QcUsProximityLibEngine(int16_t const *pSamplesBuffer,
                                  ProximityOutput *pProximity);

/*============================================================================
  FUNCTION:  QcUsProximityLibGetVersion
============================================================================*/
int QcUsProximityLibGetVersion(int *pVersion);

#ifdef __cplusplus
}
#endif

#endif //__QCUSPROXIMITYLIBPROXY_H__

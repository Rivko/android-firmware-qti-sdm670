#ifndef __QCUSGESTURELIBPROXY_H__
#define __QCUSGESTURELIBPROXY_H__

/*============================================================================
                           QcUsGestureLibProxy.h

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
#include "QcUsGestureLibApi.h"


#ifdef __cplusplus
extern "C" {
#endif

// name of dynamic sync gesture module, loaded from /system/lib/rfsa/adsp
#define SYNC_GESTURE_DYNAMIC_MODULE             "us-syncgesture.so"

#define GESTURE_LIBRARY_SUPPORTED_VERSION_NUM   1

/*============================================================================
  FUNCTION:  syncgesture_load_module
============================================================================*/
int syncgesture_load_module(void);

/*============================================================================
  FUNCTION:  syncgesture_unload_module
============================================================================*/
void syncgesture_unload_module(void);

/*============================================================================
  FUNCTION:  QcUsGestureLibGetSizes
============================================================================*/
int QcUsGestureLibGetSizes(GestureCfg const *pConfig,
                             uint32_t *pWorkspaceSize,
                             uint32_t *pPatternSizeSamples);

/*============================================================================
  FUNCTION:  QcUsGestureLibInit
============================================================================*/
int QcUsGestureLibInit(GestureCfg const *pConfig,
                         int8_t *pWorkspace,
                         uint32_t workspaceSize);

/*==============================================================================
  FUNCTION:  QcUsGestureLibGetPattern
==============================================================================*/
int QcUsGestureLibGetPattern(int16_t *pPattern, uint32_t patternSize);

/*==============================================================================
  FUNCTION:  QcUsGestureLibIsBusy
==============================================================================*/
int QcUsGestureLibIsBusy(int* pIsBusy);

/*============================================================================
  FUNCTION:  QcUsGestureLibSetDynamicConfig
============================================================================*/
int QcUsGestureLibSetDynamicConfig(int* pDynamicConfig,
                                     uint32_t dynamicConfigSize);

/*============================================================================
  FUNCTION:  QcUsGestureLibEngine
============================================================================*/
extern int QcUsGestureLibEngine(int16_t const *pSamplesBuffer,
                                  GestureOutput *pGesture);

/*============================================================================
  FUNCTION:  QcUsGestureLibGetVersion
============================================================================*/
int QcUsGestureLibGetVersion(int *pVersion);

#ifdef __cplusplus
}
#endif

#endif //__QCUSGESTURELIBPROXY_H__

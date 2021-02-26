#ifndef CHRE_APP_OEM_H
#define CHRE_APP_OEM_H

/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE
#define CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE 0x00400
#endif

#define APP_OEM_ECHO  1
#define APP_OEM_ABORT 2
#define APP_OEM_TIMER 3
#define APP_OEM_EVENT_TO_SELF  4
#define APP_OEM_SENSOR_CONFIG  5

struct sensorConfig {
    uint32_t sensType;
    uint32_t mode;
    uint64_t interval;
    uint64_t latency;
};





#ifdef __cplusplus
}
#endif

#endif // CHRE_APP_OEM_H


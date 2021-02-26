/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#ifndef __IZAT_DEV_ID__
#define __IZAT_DEV_ID__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t IzatPcid_t;

IzatPcid_t getIzatPcid();

#ifdef __cplusplus
}
#endif

#endif // #ifndef __IZAT_DEV_ID__

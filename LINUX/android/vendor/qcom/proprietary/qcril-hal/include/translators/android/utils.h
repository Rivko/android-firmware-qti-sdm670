/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/legacy.h>
#ifdef __cplusplus
extern "C" {
#endif

qcril_evt_e_type qcril_android_request_get_internal_event(int request);
int qcril_event_get_android_request(qcril_evt_e_type event);

#ifdef __cplusplus
}
#endif

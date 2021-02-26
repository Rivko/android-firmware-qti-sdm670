/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef MODULES_ANDROID_VERSION_H_INCLUDED
#define MODULES_ANDROID_VERSION_H_INCLUDED

#include <telephony/ril.h>

/* Update this constant whenever a new version
   of the ril interface is completely supported.
   Do not use this constant for version
   checks in implementation.
   Instead, use QCRIL_RIL_VERSION for that purpose. */
#define QCRIL_LATEST_SUPPORTED_RIL_VERSION 15


/* DO NOT CHANGE THIS MACRO.
   This is the version reported to rild,
   and should also be used in the code to
   guard interface differences due to version changes. */
#define QCRIL_RIL_VERSION ((RIL_VERSION >= 12) ? QCRIL_LATEST_SUPPORTED_RIL_VERSION : RIL_VERSION)


#endif

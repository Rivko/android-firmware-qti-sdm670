/*
 * Copyright (c) 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#ifndef _TLV2_INC_H_
#define _TLV2_INC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if !defined(_HOST_SIM_TESTING)
#include "osapi.h"
#endif
#include "wlantype.h"
#include "tsCommon.h"
#include "cmdRspParmsInternal.h"
#include "cmdRspParmsDict.h"
#include "tlvCmdEncoder.h"
#include "tlv2Api.h"
#include "tlv2SysApi.h"

// added to overcome unused variable compiler warning
#define UNUSED(x) (void)(x)
#endif //_TLV2_INC_H_/

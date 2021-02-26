/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <stdlib.h>
#include <stdint.h>

#ifndef RIL_REQUEST_SIM_GET_ATR
/**
 * RIL_REQUEST_SIM_GET_ATR
 *
 * Get the ATR from SIM Card
 *
 * Only valid when radio state is "RADIO_STATE_ON"
 *
 * "data" is const int *
 * ((const int *)data)[0] contains the slot index on the SIM from which ATR is requested.
 *
 * "response" is a const char * containing the ATR, See ETSI 102.221 8.1 and ISO/IEC 7816 3
 *
 * Valid errors:
 *
 * SUCCESS
 * RADIO_NOT_AVAILABLE (radio resetting)
 * GENERIC_FAILURE
 */
#define RIL_REQUEST_SIM_GET_ATR 10141
#endif /* RIL_REQUEST_SIM_GET_ATR */


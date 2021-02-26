/******************************************************************************
#  Copyright (c) 2012, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
//=============================================================================
// FILE: qcril_qmi_sms_errors.h
//
// SERVICES: RIL
//
// DESCRIPTION: defines sms error cause codes
//
//=============================================================================

//=============================================================================
// DATA TYPES
//=============================================================================

//=============================================================================
// CONSTANTS
//=============================================================================

#include "wireless_messaging_service_v01.h"

//=============================================================================
// FUNCTIONS
//=============================================================================

/// returns string representation of sms cause code for debug
const char* cause_code_str(int cause_code);

/// returns string representation of sms gw cause code for debug
const char* gw_cause_code_str(int error_code);


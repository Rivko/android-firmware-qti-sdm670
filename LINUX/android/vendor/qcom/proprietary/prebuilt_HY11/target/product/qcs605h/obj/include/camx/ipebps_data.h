//-------------------------------------------------------------------------
// Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  ipebpsdefs.h
// @brief IPE BPS command & message definitions.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
//
// 11/Jan/2016       shirb        Initial version
//------------------------------------------------------------------------

#ifndef _IPE_BPS_DATA_H_
#define _IPE_BPS_DATA_H_

#include "fwdefs.h"

typedef uint32_t PerfCountResult;

// BPS_WAIT_FOR_IPE synchronization command
typedef struct _BpsWaitForIpe
{
    IpeHandle           waitFor;    // IPE handle to wait for
    FWUSERARG           userArg;    // FW-reserved argument
} BpsWaitForIpe;

// BPS_WAIT_FOR_BPS synchronization command
typedef struct _BpsWaitForBps
{
    BpsHandle           waitFor;    // BPS handle to wait for
    FWUSERARG           userArg;    // FW-reserved argument
} BpsWaitForBps;

// IPE_WAIT_FOR_BPS synchronization command
typedef struct _IpeWaitForBps
{
    BpsHandle           waitFor;    // BPS handle to wait for
    FWUSERARG           userArg;    // FW-reserved argument
} IpeWaitForBps;

// IPE_WAIT_FOR_IPE synchronization command
typedef struct _IpeWaitForIpe
{
    IpeHandle           waitFor;    // IPE handle to wait for
    FWUSERARG           userArg;    // FW-reserved argument
} IpeWaitForIpe;

// BPS/IPE WAIT command response
typedef struct _HandleWaitDone
{
    FWSTATUS            rc;         // Async return code
    FWUSERARG           userArg;    // FW-reserved argument
} HandleWaitDone;

// IPE_BPS_PERF_COUNT command
typedef struct _IpeBpsPerfCount
{
    uint32_t            id;
    FWUSERARG           userArg;    // FW-reserved argument
} IpeBpsPerfCount;

// IPE_BPS_PERF_COUNT response
typedef struct _IpeBpsPerfCountDone
{
    FWSTATUS            rc;         // Async return code
    PerfCountResult     cmdRes;     // Result data
    FWUSERARG           userArg;    // FW-reserved argument
} IpeBpsPerfCountDone;

#endif // _IPE_BPS_DATA_H_

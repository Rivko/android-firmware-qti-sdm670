//-------------------------------------------------------------------------
// Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.

//------------------------------------------------------------------------
// @file  cameracomdef.h
// @brief Abstract data types
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
//
// 01/19/2015        visweshp      Initial version
//------------------------------------------------------------------------

#ifndef _CAMERA_COMDEF_H_
#define _CAMERA_COMDEF_H_

#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif // __cplusplus

#if __STDC_VERSION__ >= 199901L
/*C99*/
#define INLINED_FUNC inline
#else
/*Not C99*/
#define INLINED_FUNC __inline
#endif

// This code is only here, since stdbool does not exist in VS2012, make sure to not remove when integrating changes from the FW here, since the FW is compiled in VS2013
//typedef int                 BOOL;
#define BOOL                  int32_t

#define HL_CASE_UNREACHABLE(case)  case: {HL_ASSERT_UNREACHABLE();}

// Various TIMEOUT values throughout FW
#ifndef FW_CSIM_DLL
#define HW_PROCESS_TIMEOUT_MS           1000
#define HW_RESET_TIMEOUT_MS               10
#define CDM_RESET_TIMEOUT_MS             500
#else
// CSIM is slow. So setting all TIMEOUTs to very large values.
#define HW_PROCESS_TIMEOUT_MS          1000000
#define HW_RESET_TIMEOUT_MS            1000000
#define CDM_RESET_TIMEOUT_MS           1000000
#endif
// Thread priorities
// Thread priorities
#define THREAD_PRIORITY_SCHEDULER        170
#define THREAD_PRIORITY_SCHEDULER_NJP    160
#define THREAD_PRIORITY_CDM_IST          150
#define THREAD_PRIORITY_HOST_DRIVER      150
#define THREAD_PRIORITY_HLD_MODULE       145
#define THREAD_PRIORITY_VCDM              50
#define THREAD_PRIORITY_IDLE_TASK         10


// Thread stack sizes
#define THREAD_STACK_SCHEDULER      (1 << 11)
#define THREAD_STACK_CDM_IST        (1 << 10)
#define THREAD_STACK_HLD_MODULE     (1 << 14)
#define THREAD_STACK_VCDM           (1 <<  9)
#define THREAD_STACK_IDLE_TASK      (1 <<  9)
#define THREAD_STACK_HOST_DRIVER    (1 << 13)

#endif //End _CAMERA_COMDEF_H_



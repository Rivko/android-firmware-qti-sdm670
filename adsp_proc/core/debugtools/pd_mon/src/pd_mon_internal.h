#ifndef PD_MON_INTERNAL_H
#define PD_MON_INTERNAL_H
/** vi: tw=128 ts=3 sw=3 et
@file pd_mon_internal.h
@brief This file contains the API details for the Protection Domain Monitor, API 1.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2012,2017-2018 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/pd_mon/src/pd_mon_internal.h#2 $
$DateTime: 2018/03/28 02:13:17 $
$Change: 15791480 $
$Author: pwbldsvc $
===============================================================================*/

#include "err.h"
#include "rcinit_qurt.h"
#include "servreg_common.h"

#include "pd_dump.h"

#include "pd_mon_restart.h"

#include "stdarg.h"
#include "stdlib.h"
#include "stringl/stringl.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define PD_MON_NAME_MAX       64                                     // Maximum Managed PD Name Length
#define PD_MON_STACK_SIZ      4096                                   // Maximum Stack Consumption for Each Program Monitor

// concurrency anysignal masks
#define PD_MON_SIG_UNLOCK     0x00000001                             // Signal Mask to unblock parent once spawn has completed

// event anysignal masks
#define PD_MON_EVT_EXIT       0x00000001                             // Indicates a process has exited
#define PD_MON_EVT_SYNC       0x00000002                             // Indicates process exit syncronization is complete

typedef struct
{
   int asid;                                                         // Child asid of last qurt_spawn
   int client_h;                                                     // Client handle
   char name[PD_MON_NAME_MAX];                                       // PD Name
   PD_MON_RESTART restart;                                           // Restart Count
   qurt_anysignal_t evt_sig;                                         // Event signal
   int exit_status;                                                  // Status return of last exit
   int public_proxy_h;                                               // Proxy for late release QDI references
   int private_proxy_h;                                              // TMS proxy for late late release
   SERVREG_MON_HANDLE servreg_h;                                     // Service registry handle to the process node

} PD_MON_STATUS_T, *PD_MON_STATUS_P;

#define PD_MON_STATUS_NO_ASID -1
#define PD_MON_STATUS_TEMP_ASID -2 

typedef struct
{
   qurt_mutex_t mutex;                                               // Concurrency Mutex

   int handle;                                                       // Dump Device Handle

} pd_mon_internal_user_t;

extern pd_mon_internal_user_t pd_mon_internal_user;

typedef struct
{
   qurt_mutex_t mutex;                                               // Concurrency Mutex
   qurt_anysignal_t signal;                                          // Concurrency Signal

   PD_MON_STATUS_P args_handle;                                      // Opaque Handle Return from Domain Monitor to API Callee (Mutex Protected for Concurrency)
   PD_MON_RESTART args_restart;                                      // (Mutex Protected for Concurrency)

   PD_MON_STATUS_T status[PD_MON_MAX_PD];                            // PD Monitor Status Array

   int tms_mgmt_tlskey;                                              // tls key to indicate a spawning thread TMS managed

} pd_mon_internal_t;

extern pd_mon_internal_t pd_mon_internal;

extern const char* pd_mon_internal_prefix;

// Function Signatures

#if defined(__cplusplus)
}
#endif

#endif

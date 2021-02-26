/*==============================================================================

FILE:      adsppm_utils.h

DESCRIPTION: ADSPPM internal utils API

This is the internal utils API for ADSPPM. This is not distributed to any clients that uses
ADSPPM.

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.

==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/inc/adsppm_utils.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/20/11   CK      Initial version
==============================================================================*/

#ifndef ADSPPM_UTILS_H
#define ADSPPM_UTILS_H

#include "DALSys.h"
#include "adsppm.h"
#include "core_internal.h"
#include "asic.h"
#include "ULog.h"
#include "ULogFront.h"
#include "msg.h"

#ifdef ADSPPM_ENABLE_QDSS
#include "tracer.h"
#include "adsppm_tracer_event_ids.h"
#endif

#include "adsppm_utils_settings.h"

#define ADSPPM_ULOG
//#define ADSPPM_PROFILING
#define ADSPPMQDI_ULOG 0
#define ADSPPM_ULOG_BUFFER_SIZE 32768
#define ADSPPM_QDIULOG_BUFFER_SIZE 8192


void adsppm_lock(DALSYSSyncHandle lock);
void adsppm_unlock(DALSYSSyncHandle lock);

ULogHandle GetUlogHandle(void);

const char *adsppm_getBusPortName(AdsppmBusPortIdType busPort);
AdsppmBusPortIdType AdsppmMapMMPMPortIdToADSPPMPortID(MmpmBwPortIdType mmpmPort);

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#if(ADSPPM_NO_MUTEX == 1)
#define ADSPPM_LOCK(hLock)
#define ADSPPM_UNLOCK(hLock)
#else
#define ADSPPM_LOCK(hLock) adsppm_lock(hLock)
#define ADSPPM_UNLOCK(hLock) adsppm_unlock(hLock)
#endif


#define ADSPPM_LOCK_RESOURCE(resourceId) adsppm_lock(GetAdsppmCoreContext()->resourceLock[resourceId - 1])
#define ADSPPM_UNLOCK_RESOURCE(resourceId) adsppm_unlock(GetAdsppmCoreContext()->resourceLock[resourceId - 1])

//Turn on/off QDSS software event log
#if(ADSPPM_QDSS_LOG == 0)
#define ADSPPM_QDSS_EVENT_0(event)
#define ADSPPM_QDSS_EVENT_1(event, arg_1)
#define ADSPPM_QDSS_EVENT_2(event, arg_1, arg_2)
#define ADSPPM_QDSS_EVENT_3(event, arg_1, arg_2, arg_3)
#define ADSPPM_QDSS_EVENT_4(event, arg_1, arg_2, arg_3, arg_4)
#define ADSPPM_QDSS_EVENT_5(event, arg_1, arg_2, arg_3, arg_4, arg_5)
#else
#define ADSPPM_QDSS_EVENT_0(event) tracer_event_simple_vargs((tracer_event_id_t)event,0)
#define ADSPPM_QDSS_EVENT_1(event, arg_1) tracer_event_simple_vargs((tracer_event_id_t)event, 1, arg_1)
#define ADSPPM_QDSS_EVENT_2(event, arg_1, arg_2) tracer_event_simple_vargs((tracer_event_id_t)event, 2, arg_1, arg_2)
#define ADSPPM_QDSS_EVENT_3(event, arg_1, arg_2, arg_3) tracer_event_simple_vargs((tracer_event_id_t)event, 3, arg_1, arg_2, arg_3)
#define ADSPPM_QDSS_EVENT_4(event, arg_1, arg_2, arg_3, arg_4) tracer_event_simple_vargs((tracer_event_id_t)event, 4, arg_1, arg_2, arg_3, arg_4)
#define ADSPPM_QDSS_EVENT_5(event, arg_1, arg_2, arg_3, arg_4, arg_5) tracer_event_simple_vargs((tracer_event_id_t)event, 5, arg_1, arg_2, arg_3, arg_4, arg_5)
#endif


//Turn on/off DIAG messages
#ifndef ADSPPM_DIAG_MSG
#define ADSPPM_DBG_MSG_0(prio, message)
#define ADSPPM_DBG_MSG_1(prio, message, arg_1)
#define ADSPPM_DBG_MSG_2(prio, message, arg_1, arg_2)
#define ADSPPM_DBG_MSG_3(prio, message, arg_1, arg_2, arg_3)
#define ADSPPM_DBG_MSG_4(prio, message, arg_1, arg_2, arg_3, arg_4)
#define ADSPPM_DBG_MSG_5(prio, message, arg_1, arg_2, arg_3, arg_4, arg_5)
#define ADSPPM_DBG_MSG_6(prio, message, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6)
#define ADSPPM_DBG_MSG_7(prio, message, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7)
#define ADSPPM_DBG_MSG_8(prio, message, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8)
#define ADSPPM_DBG_MSG_9(prio, message, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8, arg_9)
#else
#define ADSPPM_DBG_MSG_0(prio, message) MSG(MSG_SSID_QDSP6, prio, message)
#define ADSPPM_DBG_MSG_1(prio, message, arg_1) MSG_1(MSG_SSID_ADSP_PWRDEM, prio, message, arg_1)
#define ADSPPM_DBG_MSG_2(prio, message, arg_1, arg_2) MSG_2(MSG_SSID_ADSP_PWRDEM, prio, message, arg_1, arg_2)
#define ADSPPM_DBG_MSG_3(prio, message, arg_1, arg_2, arg_3) MSG_3(MSG_SSID_ADSP_PWRDEM, prio, message, arg_1 ,arg_2, arg_3)
#define ADSPPM_DBG_MSG_4(prio, message, arg_1, arg_2, arg_3, arg_4) MSG_4(MSG_SSID_ADSP_PWRDEM, prio, message, arg_1 ,arg_2, arg_3, arg_4)
#define ADSPPM_DBG_MSG_5(prio, message, arg_1, arg_2, arg_3, arg_4, arg_5) MSG_5(MSG_SSID_ADSP_PWRDEM, prio, message, arg_1 ,arg_2, arg_3, arg_4, arg_5)
#define ADSPPM_DBG_MSG_6(prio, message, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6) MSG_6(MSG_SSID_ADSP_PWRDEM, prio, message, arg_1 ,arg_2, arg_3, arg_4, arg_5, arg_6)
#define ADSPPM_DBG_MSG_7(prio, message, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7) MSG_7(MSG_SSID_ADSP_PWRDEM, prio, message, arg_1 ,arg_2, arg_3, arg_4, arg_5, arg_6, arg_7)
#define ADSPPM_DBG_MSG_8(prio, message, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8) MSG_8(MSG_SSID_ADSP_PWRDEM, prio, message, arg_1 ,arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8)
#define ADSPPM_DBG_MSG_9(prio, message, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8, arg_9) MSG_9(MSG_SSID_ADSP_PWRDEM, prio, message, arg_1 ,arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8, arg_9)
#endif


//Re-direct ULOG output to DIAG messages
#ifdef ADSPPM_ULOG_TO_DIAG
#undef ULOG_RT_PRINTF_0
#undef ULOG_RT_PRINTF_1
#undef ULOG_RT_PRINTF_2
#undef ULOG_RT_PRINTF_3
#undef ULOG_RT_PRINTF_4
#undef ULOG_RT_PRINTF_5
#undef ULOG_RT_PRINTF_6
#undef ULOG_RT_PRINTF_7
#undef ULOG_RT_PRINTF_8
#undef ULOG_RT_PRINTF_9
#define ULOG_RT_PRINTF_0(h, formatStr) UNUSED(h); ADSPPM_DBG_MSG_0(MSG_LEGACY_HIGH, formatStr)
#define ULOG_RT_PRINTF_1(h, formatStr, p1) UNUSED(h); ADSPPM_DBG_MSG_1(MSG_LEGACY_HIGH, formatStr, p1)
#define ULOG_RT_PRINTF_2(h, formatStr, p1, p2) UNUSED(h); ADSPPM_DBG_MSG_2(MSG_LEGACY_HIGH, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3) UNUSED(h); ADSPPM_DBG_MSG_3(MSG_LEGACY_HIGH, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4) UNUSED(h); ADSPPM_DBG_MSG_4(MSG_LEGACY_HIGH, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5) UNUSED(h); ADSPPM_DBG_MSG_5(MSG_LEGACY_HIGH, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6) UNUSED(h); ADSPPM_DBG_MSG_6(MSG_LEGACY_HIGH, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7) UNUSED(h); ADSPPM_DBG_MSG_7(MSG_LEGACY_HIGH, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8) UNUSED(h); ADSPPM_DBG_MSG_8(MSG_LEGACY_HIGH, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9) UNUSED(h); ADSPPM_DBG_MSG_9(MSG_LEGACY_HIGH, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#endif


#ifndef ADSPPM_ULOG
#define ULOG_RT_PRINTF_0(h, formatStr)
#define ULOG_RT_PRINTF_1(h, formatStr, p1)
#define ULOG_RT_PRINTF_2(h, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#endif


#define ADSPPM_LOG_PRINTF_0(level, formatStr) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_1(GetUlogHandle(), "%s: "formatStr, __FUNCTION__ );}

#define ADSPPM_LOG_PRINTF_1(level, formatStr, p1) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_2(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1);}

#define ADSPPM_LOG_PRINTF_2(level, formatStr, p1, p2) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_3(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1, p2);}

#define ADSPPM_LOG_PRINTF_3(level, formatStr, p1, p2, p3) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_4(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1, p2, p3);}

#define ADSPPM_LOG_PRINTF_4(level, formatStr, p1, p2, p3, p4) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_5(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1, p2, p3, p4);}

#define ADSPPM_LOG_PRINTF_5(level, formatStr, p1, p2, p3, p4, p5) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_6(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1, p2, p3, p4, p5);}

#define ADSPPM_LOG_PRINTF_6(level, formatStr, p1, p2, p3, p4, p5, p6) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_7(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1, p2, p3, p4, p5, p6);}

#define ADSPPM_LOG_PRINTF_7(level, formatStr, p1, p2, p3, p4, p5, p6, p7) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_8(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1, p2, p3, p4, p5, p6, p7);}

#define ADSPPM_LOG_PRINTF_8(level, formatStr, p1, p2, p3, p4, p5, p6, p7, p8) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_9(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1, p2, p3, p4, p5, p6, p7, p8);}


#define ADSPPM_LOG_FUNC_ENTER \
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_FN_ENTRY, "Enter\n")

#define ADSPPM_LOG_FUNC_EXIT(code) \
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_FN_ENTRY, "Exit(%u)\n", code)

#ifndef ADSPPM_PROFILING
#define ADSPPM_LOG_FUNC_PROFILESTART
#define ADSPPM_LOG_FUNC_PROFILEEND
#else
#define ADSPPM_LOG_FUNC_PROFILESTART ULOG_RT_PRINTF_1(GetUlogHandle(), "[PROFILE]: %s: ProfileStart\n", __FUNCTION__ )
#define ADSPPM_LOG_FUNC_PROFILEEND ULOG_RT_PRINTF_1(GetUlogHandle(), "[PROFILE]: %s: ProfileEnd\n", __FUNCTION__ )
#endif

#endif /* ADSPPM_UTILS_H */

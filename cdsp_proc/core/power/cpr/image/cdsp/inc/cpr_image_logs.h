/**
 * @file:  cpr_image_logs.h
 * @brief:
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/image/cdsp/inc/cpr_image_logs.h#1 $
 * $Change: 16759227 $
 */
#ifndef CPR_IMAGE_LOGS_H
#define	CPR_IMAGE_LOGS_H

#include <stdbool.h>
#include "ULogFront.h"
#include "CoreVerify.h"
#include "cpr_defs.h"

#define CPR_LOG_NAME "CprLog"
#define CPR_LOG_SIZE 8192

#define CPR_STATIC_LOG_NAME "CprStatsLog"


extern ULogHandle cprLogHandle;
extern ULogHandle cprStaticLogHandle;


#define CPR_IMAGE_LOG_INIT()\
    ULogFront_RealTimeInit(\
            &cprLogHandle,\
            CPR_LOG_NAME,\
            CPR_LOG_SIZE,\
            ULOG_MEMORY_LOCAL,\
            ULOG_LOCK_OS)

#define GET_COUNT_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,COUNT,...) COUNT
#define DO_ULOG(...) ULogFront_RealTimePrintf(cprLogHandle, GET_COUNT_MACRO(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0), __VA_ARGS__)

#if CPR_LOG_FATAL_ASSERT_ENABLED
#define CPR_IMAGE_LOG_FATAL(msg, args...)     do { DO_ULOG("Fatal: "msg, ##args); CPR_ASSERT(false); } while(false)
#else
#define CPR_IMAGE_LOG_FATAL(msg, args...)     DO_ULOG("FATAL: "msg, ##args)
#endif
#define CPR_IMAGE_LOG_ERROR(msg, args...)     DO_ULOG("ERROR: "msg, ##args)
#define CPR_IMAGE_LOG_WARNING(msg, args...)   DO_ULOG("WARN : "msg, ##args)
#define CPR_IMAGE_LOG_INFO(msg, args...)      DO_ULOG("INFO : "msg, ##args)
#define CPR_IMAGE_LOG_VERBOSE(msg, args...)   DO_ULOG("VERBOSE: "msg, ##args)
#define CPR_IMAGE_LOG_TRACE(msg, args...)     DO_ULOG("TRACE: "msg, ##args)
#define CPR_IMAGE_LOG_TRACE_RAW(msg, args...) CPR_IMAGE_LOG_TRACE(msg, ##args)


#define CPR_IMAGE_STATIC_LOG_SLOT_CPR_INFO            0
#define CPR_IMAGE_STATIC_LOG_SLOT_RAIL_INFO           1
#define CPR_IMAGE_STATIC_LOG_SLOT_MODE_INFO_OFFSET    2

#define GET_MODE_IDX(rail_state, mode_setting)  ((mode_setting) - (rail_state)->modeSettings)

#define GET_MODE_INFO_SLOT(rail_state, mode_setting) (CPR_IMAGE_STATIC_LOG_SLOT_MODE_INFO_OFFSET + GET_MODE_IDX(rail_state, mode_setting))
#define GET_ISR_INFO_SLOT(rail_state, mode_setting) (CPR_IMAGE_STATIC_LOG_SLOT_MODE_INFO_OFFSET + (rail_state)->modeSettingsCount + GET_MODE_IDX(rail_state, mode_setting))

//#define CPR_IMAGE_STATIC_LOG_INIT() cpr_image_static_log_init()
//#define CPR_IMAGE_STATIC_LOG_UPDATE_ALL() cpr_image_static_log_update_all()

#define CPR_IMAGE_STATIC_LOG_INIT()
#define CPR_IMAGE_STATIC_LOG_UPDATE_ALL()

#define CPR_IMAGE_STATIC_LOG_CPR_INFO(cpr_info)\
    ULOG_RT_STATIC_PRINTF_7(\
            cprStaticLogHandle,\
            CPR_IMAGE_STATIC_LOG_SLOT_CPR_INFO,\
            "CPR Info: (ChipVersion:0x%08X) (CprRev:0x%08X) (Foundry:%u) (DataInit:%u) (CfgInit:%u) (FullInit:%u) (VP Hash:%s)",\
            (cpr_info)->chipVersion,\
            (cpr_info)->cprRev,\
            (cpr_info)->foundry,\
            (cpr_info)->dataInit,\
            (cpr_info)->cfgInit,\
            (cpr_info)->fullInit,\
            (cpr_info)->settings->hashValue)

#define CPR_IMAGE_STATIC_LOG_RAIL_INFO(rail_state)\
    ULOG_RT_STATIC_PRINTF_7(\
            cprStaticLogHandle,\
            CPR_IMAGE_STATIC_LOG_SLOT_RAIL_INFO,\
            "Rail Info: (CtrlMode:%u) (AgingMargin:%d, ThermMargin:%d, TestMargin:%d) (DisableVotes:0x%X) (PrevMode:%d, ActiveMode:%d)",\
            (rail_state)->cMode,\
            (rail_state)->marginAdjustments[CPR_MARGIN_ADJUSTMENT_AGING],\
            (rail_state)->marginAdjustments[CPR_MARGIN_ADJUSTMENT_THERMAL],\
            (rail_state)->marginAdjustments[CPR_MARGIN_ADJUSTMENT_TEST],\
            (rail_state)->disableVotes,\
            ((rail_state)->previousMode) ? (rail_state)->previousMode->mode : 0,\
            ((rail_state)->activeMode) ? (rail_state)->activeMode->mode : 0)

#define CPR_IMAGE_STATIC_LOG_MODE_INFO(rail_state, mode_setting, submode_setting)\
    ULOG_RT_STATIC_PRINTF_10(\
            cprStaticLogHandle,\
            GET_MODE_INFO_SLOT(rail_state, mode_setting),\
            "%s Mode (%d) Info: (FuseStep:%d, FuseOffset:%d) (EnableCount:%u) (TotalMargin:%d) (Floor:%u, Ceiling:%u) (Min:%u, Current:%u)",\
            cpr_image_log_mode_name((mode_setting)->mode),\
            (mode_setting)->mode,\
            (mode_setting)->decodedFuseSteps,\
            (mode_setting)->decodedFuseOffset,\
            (mode_setting)->enableCount,\
            (mode_setting)->totalMarginMV*1000,\
            (submode_setting)->floor,\
            (submode_setting)->ceiling,\
            (submode_setting)->min,\
            (submode_setting)->current)

#define CPR_IMAGE_STATIC_LOG_ISR_INFO(rail_state, mode_setting, submode_setting)\
    ULOG_RT_STATIC_PRINTF_7(\
            cprStaticLogHandle,\
            GET_ISR_INFO_SLOT(rail_state, mode_setting),\
            "%s Mode (%d) ISR Info: (UpInt:%u, DownInt:%u) (HitFloor:%u, HitCeiling:%u, Ignored:%u)",\
            cpr_image_log_mode_name((mode_setting)->mode),\
            (mode_setting)->mode,\
            (submode_setting)->debug.up,\
            (submode_setting)->debug.down,\
            (submode_setting)->debug.floor,\
            (submode_setting)->debug.ceiling,\
            (submode_setting)->debug.ignored)

void cpr_image_static_log_init(void);
void cpr_image_static_log_update_all(void);
const char* cpr_image_log_mode_name(cpr_voltage_mode mode);
#endif


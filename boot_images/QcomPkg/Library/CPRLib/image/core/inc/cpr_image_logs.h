/**
 * @file:  cpr_image_logs.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/image/core/inc/cpr_image_logs.h#1 $
 * $Change: 14402555 $
 */
#ifndef CPR_IMAGE_LOGS_H
#define CPR_IMAGE_LOGS_H

#include "cpr_types.h"

#include <stdbool.h>


#if ((defined(CPR_LOG_COMPILE_LEVEL)) && (CPR_LOG_COMPILE_LEVEL > CPR_LOG_LEVEL_NONE))

    #define CPR_IMAGE_LOG_INIT()

    void cpr_image_log_message(const char * restrict msg, ...);

    #if CPR_LOG_FATAL_ASSERT_ENABLED
    #define CPR_IMAGE_LOG_FATAL(msg, args...)     do { cpr_image_log_message("Fatal: "msg, ##args); CPR_ASSERT(false); } while(false)
    #else
    #define CPR_IMAGE_LOG_FATAL(msg, args...)     cpr_image_log_message("FATAL: "msg, ##args)
    #endif
    #define CPR_IMAGE_LOG_ERROR(msg, args...)     cpr_image_log_message("ERROR: "msg, ##args)
    #define CPR_IMAGE_LOG_WARNING(msg, args...)   cpr_image_log_message("WARN : "msg, ##args)
    #define CPR_IMAGE_LOG_INFO(msg, args...)      cpr_image_log_message("INFO : "msg, ##args)
    #define CPR_IMAGE_LOG_VERBOSE(msg, args...)   cpr_image_log_message("VERBOSE: "msg, ##args)
    #define CPR_IMAGE_LOG_TRACE(msg, args...)     cpr_image_log_message("TRACE: "msg, ##args)
    #define CPR_IMAGE_LOG_TRACE_RAW(msg, args...) cpr_image_log_message(msg, ##args)


    #if 0
    #define CPR_IMAGE_STATIC_LOG_INIT() cpr_image_static_log_update_all()

    #define CPR_IMAGE_STATIC_LOG_UPDATE_ALL() cpr_image_static_log_update_all()

    #define CPR_IMAGE_STATIC_LOG_CPR_INFO(cpr_info)\
        CPR_IMAGE_LOG_INFO(\
                "CPR Info: (ChipVersion:0x%08X) (CprRev:0x%08X) (Foundry:%u) (DataInit:%u) (CfgInit:%u) (FullInit:%u) (EnabCfg Hash:%s) (VP Hash:%s)",\
                (cpr_info)->chipVersion,\
                (cpr_info)->cprRev,\
                (cpr_info)->foundry,\
                (cpr_info)->dataInit,\
                (cpr_info)->cfgInit,\
                (cpr_info)->fullInit,\
                (cpr_info)->cfgInfo->hashValue,\
                (cpr_info)->vpInfo->hashValue)

    #define CPR_IMAGE_STATIC_LOG_RAIL_INFO(rail_state)\
        CPR_IMAGE_LOG_INFO(\
                "Rail Info: (CtrlMode:%u) (AgingMargin:%d, ThermMargin:%d, TestMargin:%d) (DisableVotes:0x%X) (PrevMode:%d, ActiveMode:%d)",\
                (rail_state)->cMode,\
                (rail_state)->marginAdjustments[CPR_MARGIN_ADJUSTMENT_SOURCE_AGING],\
                (rail_state)->marginAdjustments[CPR_MARGIN_ADJUSTMENT_SOURCE_THERMAL],\
                (rail_state)->marginAdjustments[CPR_MARGIN_ADJUSTMENT_SOURCE_TEST],\
                (rail_state)->disableVotes,\
                ((rail_state)->previousMode) ? (rail_state)->previousMode->mode : 0,\
                ((rail_state)->activeMode) ? (rail_state)->activeMode->mode : 0)

    #define CPR_IMAGE_STATIC_LOG_MODE_INFO(rail_state, mode_setting, submode_setting)\
        CPR_IMAGE_LOG_INFO(\
                "Mode %u Info: (FuseStep:%d, FuseOffset:%d) (EnableCount:%u) (Floor:%u, Ceiling:%u) (Min:%u, Current:%u)",\
                (mode_setting)->mode,\
                (mode_setting)->decodedFuseSteps,\
                (mode_setting)->decodedFuseOffset,\
                (mode_setting)->enableCount,\
                (submode_setting)->floor,\
                (submode_setting)->ceiling,\
                (submode_setting)->min,\
                (submode_setting)->current)

    #define CPR_IMAGE_STATIC_LOG_ISR_INFO(rail_state, mode_setting, submode_setting)\
        CPR_IMAGE_LOG_INFO(\
                "Mode %u ISR Info: (UpInt:%u, DownInt:%u) (HitFloor:%u, HitCeiling:%u, Ignored:%u)",\
                (mode_setting)->mode,\
                (submode_setting)->debug.up,\
                (submode_setting)->debug.down,\
                (submode_setting)->debug.floor,\
                (submode_setting)->debug.ceiling,\
                (submode_setting)->debug.ignored)

    void cpr_image_static_log_update_all(void);
    #endif

#else

    #define CPR_IMAGE_LOG_FATAL(msg, args...)
    #define CPR_IMAGE_LOG_ERROR(msg, args...)
    #define CPR_IMAGE_LOG_WARNING(msg, args...)
    #define CPR_IMAGE_LOG_INFO(msg, args...)
    #define CPR_IMAGE_LOG_VERBOSE(msg, args...)
    #define CPR_IMAGE_LOG_TRACE(msg, args...)
    #define CPR_IMAGE_LOG_TRACE_RAW(msg, args...)

#endif /*((defined(CPR_LOG_COMPILE_LEVEL)) && (CPR_LOG_COMPILE_LEVEL > CPR_LOG_LEVEL_NONE))*/


#endif /* CPR_IMAGE_LOGS_H */


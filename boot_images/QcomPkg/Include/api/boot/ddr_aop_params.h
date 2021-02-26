#ifndef __DDR_AOP_PARAMS_H__
#define __DDR_AOP_PARAMS_H__

/*=============================================================================

                                DDR AOP PARAMETERS
                                Header File
GENERAL DESCRIPTION
This file defines the AOP parameters provided from DCB

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*==========================================================================
                               INCLUDE FILES
===========================================================================*/
#include "HALcomdef.h"

/*==============================================================================
                                  TYPES & DEFINES
==============================================================================*/

#define MAX_NUM_CLOCK_PLAN 12
#define MAX_NUM_SHUB_CLOCK_PLAN 7

/*
 *  Clock plan inputs to AOP from DDR
 */
typedef enum
{
    GCC,
    DDRCC,
    NA = 0xFF
} CLK_MODE;

typedef enum
{
    VDD_OFF     = 0x0,
    VDD_RET     = 0x10,
    MIN_SVS     = 0x30,
    LOW_SVS     = 0x40,
    SVS         = 0x80,
    SVS_L1      = 0xC0,
    NOMINAL     = 0x100,
    NOMINAL_L1  = 0x140,
    NOMINAL_L2  = 0x150,
    TURBO       = 0x180,
    TURBO_L1    = 0x1A0,
    MAX         = TURBO_L1,
    //VDD_INVALID = -1, // redefinition of enumerator
    VDD_NUM_LEVELS = 11,
} VOLTAGE_RAIL;

typedef enum
{
    PMIC_CFG_0,
    PMIC_CFG_1,
    PMIC_CFG_2,
    PMIC_CFG_3,
    PMIC_CFG_4,
    PMIC_CFG_5,
    PMIC_CFG_6,
    PMIC_CFG_7,
    PMIC_CFG_8,
    PMIC_CFG_9,
    PMIC_CFG_10,
} PMIC_MODE;

typedef struct
{
#if defined(COMPILING_LPASS_PROC)\
    || defined(COMPILING_MSS_PROC)\
    || defined(COMPILING_SC300_PROC)\
    || defined(COMPILING_SSC_PROC)\
    || defined(COMPILING_SSC_SDC_PROC)\
    || defined(COMPILING_TURING_PROC)
    uint8 clk_idx;
    uint32 freq_khz;
    uint32 clk_period;
    uint8 freq_enable;
    uint32 mode;
    uint32 vddcx;
    uint32 vddmx;
    uint32 vdda;
    uint32 pmic_mode_cfg;
    uint8 max_up_idx;
    uint8 min_down_idx;
    uint8 double_freq_switch;
#else
    uint8 clk_idx;
    uint32 freq_khz;
    uint32 clk_period;
    boolean freq_enable;
    CLK_MODE mode;
    VOLTAGE_RAIL vddcx;
    VOLTAGE_RAIL vddmx;
    VOLTAGE_RAIL vdda;
    PMIC_MODE pmic_mode_cfg;
    uint8 max_up_idx;
    uint8 min_down_idx;
    boolean double_freq_switch;
#endif
} FREQ_STATE;


/*
 *  ALC (Active Latency Code) inputs to AOP from DDR
 *
 *  The MC (memory controller) and DDR_CC (DDR clock controller) have timers
 *  controlling when to enter incrementally lower power states when idle.
 *
 *  IDLE power state entry timeline     : ==>------|---|---|---|---------------
 *                                                             /\
 *  EXIT from highest power state       : ==>-<================================
 *  EXIT from intermediate power state  : ==>-------<+++=======================
 *  EXIT from lowest power state        : ==>---------------------<++++++++====
 *                                                                        /\
 *  Above ">" indicates a memory channel becoming idle, "-" indicates the memory
 *  channel continuing to be idle, and "|" indicates enty into a lower power state.
 *
 *  Above "<" indicates a need to become active, "+" indicates waiting to become
 *  active, and "=" indicates the memory channel being active.
 *
 *  The idle hysteresis is the latency from becoming idle to entering the lowest
 *  allowed power state (from ">" to "/\" in timeline above).
 *
 *  The exit latency is the latency from needing to become active from the lowest
 *  allowed power state to being active (from "<" to "/\" in timeline above).
 *
 *  Intermediate power states will have entry and exit latencies not communicated
 *  to AOP.
 */
#define MAX_NUM_ALC_POLICY 16

typedef struct
{
    uint32 idle_hysteresis_in_ns;   // latency to lowest allowed power state from
                                    // idle (zero indicates invalid ALC policy)
    uint32 exit_latency_in_ns;      // latency from lowest allowed power state to
                                    // active (zero indicates invalid ALC policy)
} ALC_POLICY_TIMINGS;


/*
 * AOP parameters provided from DCB
 */
typedef struct
{
    FREQ_STATE mc_states[MAX_NUM_CLOCK_PLAN];
    FREQ_STATE shub_states[MAX_NUM_SHUB_CLOCK_PLAN];
    uint8 default_alc_policy;
    ALC_POLICY_TIMINGS alc_timings[MAX_NUM_ALC_POLICY];
} DDR_AOP_PARAMS;

#endif /* __DDR_AOP_PARAMS_H__ */

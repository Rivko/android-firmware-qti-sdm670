/*-----------------------------------------------------------------------
   Copyright (c) 2014 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/
#ifndef SYSMON_PROF_H
#define SYSMON_PROF_H

/*
 * sysmon_timer_state_t:
 * structure type for sysmon_timer state
 */
typedef enum{
    SYSMON_TIMER_DISABLED = 0,
    SYSMON_TIMER_IN_HFW = 1,
    SYSMON_TIMER_IN_LFW = 2,
    SYSMON_TIMER_STATE_MAX
}sysmon_timer_state_t;

/* Timer over ride state enum*/
typedef enum{
    SYSMON_TIMER_OVERRIDE_ENABLE = 1,
    SYSMON_TIMER_OVERRIDE_DISABLE = 2,
}sysmon_timer_override_t;

//----------------------Prototypes----------------------------------

/******************************************************************************
 * FUNCTION: sysmon_profiler_init
 * DESCRIPTION:
 *	Sysmon init function. To be called from main/rcinit.
 *****************************************************************************/
void sysmon_profiler_init(void);

/******************************************************************************
 * FUNCTION: sysmon_enable
 * DESCRIPTION:
 * 		handles enable request from clients or internal state change.
 *****************************************************************************/
void sysmon_enable(void);

/******************************************************************************
 * FUNCTION: sysmon_disable
 * DESCRIPTION:
 * 		handles disable request from CLIENTs or internal state change.
 *****************************************************************************/
void sysmon_disable(void);

/******************************************************************************
 * FUNCTION: sysmon_timer_freq_change_override
 * DESCRIPTION:
 *      Overrides default state changes of SysMon to a fixed mode - LFW/HFW
 * @PARAMS:
 *      enable: SYSMON_TIMER_OVERRIDE_ENABLE/SYSMON_TIMER_OVERRIDE_DISABLE
 *      state: SYSMON_TIMER_IN_HFW/SYSMON_TIMER_IN_LFW
 *****************************************************************************/
void sysmon_timer_freq_change_override(sysmon_timer_override_t enable,
                                       sysmon_timer_state_t state);

/******************************************************************************
 * FUNCTION: sysmon_profiler_timer_stop
 * DESCRIPTION:
 *      Stops on-going sysmon timer indefinitely
 *****************************************************************************/
void sysmon_profiler_timer_stop(void);

/******************************************************************************
 * FUNCTION: sysmon_profiler_timer_restart
 * DESCRIPTION:
 *      Stops on-going sysmon timer indefinitely
 *****************************************************************************/
void sysmon_profiler_timer_restart(void);

/******************************************************************************
 * FUNCTION: sysmon_configure_timer_for_hfw
 * DESCRIPTION:
 *         Configures SysMon timer for HFW mode.
 *****************************************************************************/
void sysmon_configure_timer_for_hfw(void);

/******************************************************************************
 * FUNCTION: sysmon_configure_timer_for_lfw
 * DESCRIPTION:
 *         Configures SysMon timer for LFW mode.
 *****************************************************************************/
void sysmon_configure_timer_for_lfw(void);

#endif /*SYSMON_PROF_H*/

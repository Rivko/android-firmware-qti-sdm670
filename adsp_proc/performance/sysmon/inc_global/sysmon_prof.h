/*-----------------------------------------------------------------------
   Copyright (c) 2015 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/
#ifndef SYSMON_PROF_H
#define SYSMON_PROF_H

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
 *      Re-enables SysMon profiler.
 *      To be called when sysmon_disable() is already called earlier.
 *****************************************************************************/
void sysmon_enable(void);

/******************************************************************************
 * FUNCTION: sysmon_disable
 * DESCRIPTION:
 *      Disables SysMon profiler and related activities.
 *      call sysmon_enable() to re-enable after this call.
 *****************************************************************************/
void sysmon_disable(void);

#endif /*SYSMON_PROF_H*/

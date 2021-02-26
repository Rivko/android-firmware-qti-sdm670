/*============================================================================
  aop_ulogcompatibility.c

  This file implements the front end interfaces for ULog compatibility.  
  To keep memory requirements to a minimum there is no real log structure 
  created for the logs.  All writes are directed out QDSS once QDSS has 
  been enabled by ULogCore_SetTransportToStm for any log. 
  
  This file could be fairly easily enhanced to write to a ram buffer someday
  if rpm has enough memory to provided it. 

  Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
  QUALCOMM Proprietary.  Export of this technology or software is regulated 
  by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/
/* $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ulog/src/aop_ulogcompatibility.c#1 $ */

#include "aop_ulog.h"
#include "time_service.h"
#include "rex.h"
#include "qdss_swevent.h"
#include "CoreVerify.h"
#include <stdarg.h>
#include <stdlib.h>
//#include "DALStdErr.h"


//Share one STM port across all ULogCompatibility users.
uint32 ulog_global_stmport;



static boolean swevents_init_done = FALSE;
static boolean ddr_swevents_init_done = FALSE;

static char* rpm_log_ptr;
static rex_crit_sect_type log_lock;
static rex_crit_sect_type ddr_log_lock;

extern char __aop_log_base[];
extern char __aop_log_limit[];
extern char __ddr_log_base[];
extern char __ddr_log_limit[];

static rpm_ULogHandle rpm_log;
static rpm_ULogHandle ddr_log;


/*----------------------------------------------------------------------------
swevents_init 
Creates a logging framework and defaults Ulogs transport to RAM 
Returns: RPM_ULOG_SUCCESS on a successful init
          RPM_ULOG_ERROR on errors 
----------------------------------------------------------------------------*/
rpm_ULogResult swevents_init(void)
{
    rpm_ULogResult result;
    
    if (swevents_init_done) 
    {
        return RPM_ULOG_SUCCESS;
    }
    else
    {
        rex_init_crit_sect(&log_lock);
        const uint32 size_of_rpm_log = __aop_log_limit - __aop_log_base; //4k log
        char* const rpm_log_ptr = __aop_log_base; //defined in aop.scl
        

        // create a new ULog
        rex_enter_crit_sect(&log_lock);
        result = rpm_ULog_CreateNew(&rpm_log, "AOP External Log", rpm_log_ptr, NULL, size_of_rpm_log);
        if(RPM_ULOG_SUCCESS == result)
        {
            swevents_init_done = TRUE;
        }
        rex_leave_crit_sect(&log_lock);
        return result;
    }           
}

/*----------------------------------------------------------------------------
ddr_swevents_init 
Creates a logging framework and defaults Ulogs transport to RAM 
Returns: RPM_ULOG_SUCCESS on a successful init
          RPM_ULOG_ERROR on errors 
----------------------------------------------------------------------------*/
rpm_ULogResult ddr_swevents_init(void)
{
  rpm_ULogResult result;
  
  if (ddr_swevents_init_done) 
  {
    return RPM_ULOG_SUCCESS;
  }
  else
  {
    rex_init_crit_sect(&ddr_log_lock);
    const uint32 size_of_ddr_log = __ddr_log_limit - __ddr_log_base; //1k log
    char* const ddr_log_ptr = __ddr_log_base; //defined in aop.scl
    
    // create a new ULog
    rex_enter_crit_sect(&ddr_log_lock);
    result = rpm_ULog_CreateNew(&ddr_log, "AOP DDR Log", ddr_log_ptr, NULL, size_of_ddr_log);
    if(RPM_ULOG_SUCCESS == result)
    {
      ddr_swevents_init_done = TRUE;
    }
    rex_leave_crit_sect(&ddr_log_lock);
    return result;
  }           
}



/*----------------------------------------------------------------------------
swevent_to_ram 
Changes the ulog transport method to internal RAM 
Calls swevent_init if not called by clients already
Returns: RPM_ULOG_SUCCESS on a successful change
          RPM_ULOG_ERROR on errors  
----------------------------------------------------------------------------*/
rpm_ULogResult swevent_to_ram(void)
{
    if (!swevents_init_done)
    {
         swevents_init();
    }
  if (!ddr_swevents_init_done)
  {
    ddr_swevents_init();
  }
    rex_enter_crit_sect(&log_lock);
    rpm_ULogResult result = rpm_ULog_SetTransportToRAM(rpm_log);
    rex_leave_crit_sect(&log_lock);
  CORE_VERIFY(RPM_ULOG_SUCCESS == result);

  rex_enter_crit_sect(&ddr_log_lock);
  result = rpm_ULog_SetTransportToRAM(ddr_log);
  rex_leave_crit_sect(&ddr_log_lock);
  CORE_VERIFY(RPM_ULOG_SUCCESS == result);
    return result;
}

/*----------------------------------------------------------------------------
swevent_to_qdss 
Changes logging transport to STM. Calls swevent init for the first call. 
Returns: RPM_ULOG_SUCCESS on a successful change
          RPM_ULOG_ERROR on errors  
----------------------------------------------------------------------------*/
rpm_ULogResult swevent_to_qdss(void)
{
    if (!swevents_init_done)
    {
         swevents_init();
    }
  CORE_VERIFY(rpm_log);
  if (!ddr_swevents_init_done)
  {
    ddr_swevents_init();
  }
  CORE_VERIFY(ddr_log);
    rex_enter_crit_sect(&log_lock);
    rpm_ULogResult result = rpm_ULog_SetTransportToSTM(rpm_log);
    rex_leave_crit_sect(&log_lock);
  CORE_VERIFY(RPM_ULOG_SUCCESS == result);

  rex_enter_crit_sect(&ddr_log_lock);
  result = rpm_ULog_SetTransportToSTM(ddr_log);
  rex_leave_crit_sect(&ddr_log_lock);
  CORE_VERIFY(RPM_ULOG_SUCCESS == result);
    return result;
}




/*----------------------------------------------------------------------------
aop_log_event
 Writes 5 uint32 values to log in a format compatible with Raw ULogs.
 Compatible with fast7 messages only and can be used with fast7 logs.
 Attempt to use other write methods might corrupt other messages.
 Returns: RPM_ULOG_SUCCESS on a successful write
          RPM_ULOG_ERROR on errors
 Parameters: id : A id to define event with. See tracer_event_ids.h 
             d0-d3 : 4 uint32 data words to write to the log
----------------------------------------------------------------------------*/
rpm_ULogResult aop_log_event(uint32 id, uint32 d0, uint32 d1, uint32 d2, uint32 d3)
{
  if (!swevents_init_done)
  {
     swevents_init();
  }

  //Log this as a QDSS SW event if enabled.
  //Calling this here, instead of in the block below to minimize stack usage.
  QDSS_SWEVENT(id,d0,d1,d2,d3);

  CORE_VERIFY(rpm_log);
  
  
  rex_enter_crit_sect(&log_lock);
  uint32 message[7]; //store data here
  uint64 timestamp = time_service_now();
  rpm_ULogResult result;
  message[0] = (uint32) timestamp;
  message[1] = timestamp >> 32;
  message[2] = id;
  message[3] = d0;
  message[4] = d1;
  message[5] = d2;
  message[6] = d3;

  result = rpm_ULog_Fast7WordWrite(rpm_log, message);
  rex_leave_crit_sect(&log_lock);
  return result;
 
}


/*----------------------------------------------------------------------------
ddr_log_event
 Writes 5 uint32 values to log in a format compatible with Raw ULogs.
 Compatible with fast7 messages only and can be used with fast7 logs.
 Attempt to use other write methods might corrupt other messages.
 Returns: RPM_ULOG_SUCCESS on a successful write
          RPM_ULOG_ERROR on errors
 Parameters: id : A id to define event with. See tracer_event_ids.h 
             d0-d3 : 4 uint32 data words to write to the log
----------------------------------------------------------------------------*/
rpm_ULogResult ddr_mgr_log_event(uint32 id, uint32 d0, uint32 d1, uint32 d2, uint32 d3)
{
  if (!ddr_swevents_init_done)
  {
     ddr_swevents_init();
  }

  //Log this as a QDSS SW event if enabled.
  //Calling this here, instead of in the block below to minimize stack usage.
  QDSS_SWEVENT(id,d0,d1,d2,d3);

  CORE_VERIFY(ddr_log);
  rex_enter_crit_sect(&ddr_log_lock);
  uint32 message[7]; //store data here
  uint64 timestamp = time_service_now();
  rpm_ULogResult result;
  message[0] = (uint32) timestamp;
  message[1] = timestamp >> 32;
  message[2] = id;
  message[3] = d0;
  message[4] = d1;
  message[5] = d2;
  message[6] = d3;

  result = rpm_ULog_Fast7WordWrite(ddr_log, message);
  rex_leave_crit_sect(&ddr_log_lock);
  return result;
 
}

#ifndef PROFTP_H
#define PROFTP_H
/*===========================================================================

DESCRIPTION
  Definitions of DIAG subsystem packets for configuring task profile,
  memory debug, PC profile, and code coverage 
 
Copyright (c) 2002-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //components/rel/core.qdsp6/2.1.c4/services/prof/inc/prof_tp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/02   jct     Integrated onto target
07/20/02    gr     Created

===========================================================================*/

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */
#define MAX_THREADS_IN_PACKET 32

#if defined(ADSP_PROF)
  #define PROCESSOR_ID  2
#elif defined(MPSS_PROF)
  #define PROCESSOR_ID  0
#endif

/** 
  @brief Per-software-thread profiling information that is part of the
  prof_event_periodic_profiling_t and prof_event_periodic_profiling_3_t
  payload structures.
*/
typedef PACK(struct )
{
   unsigned int	 thread_id;
   unsigned int    thread_prio;  
   char      thread_name[16];  // 16 bytes
   unsigned int    context_switches;
   unsigned int    proc_id;
   char      proc_name[16];  // 16 bytes 
   unsigned int    stack_base;
   unsigned int    stack_size;
   unsigned long long thread_pcycles[6]; 
}prof_sw_thread_info;

/** 
  @brief Profiling payload which provides a periodic update of 
  profiling statistics. The payload contains one instance of 
  this structure followed by profiled_sw_threads instances of 
  prof_sw_thread_info.
  */
typedef PACK(struct )  
{
   uint8	 version_info;
   uint8     processor; /* 0 Modem SW, 1 Modem FW, 2 Multimedia */
   uint8     hw_thread_num; /* <= 6 */
   uint8     sw_thread_num_in_packet; //<= MAX_THREADS_IN_PACKET;   
   uint32    sw_thread_num_in_system;   
   uint64    total_pcycles; 
   uint32    core_freq;
   uint64    total_sclk_ticks;
   uint32    sclk_freq;
}prof_event_periodic_profiling;

/** 
@brief Log packet strucure of taskprofiling monitor when enabled 
through QXDM.
*/
typedef PACK(struct )
{
   prof_event_periodic_profiling event_payload;
   /**< profiler event payload                  */  
   prof_sw_thread_info sw_threads_info[MAX_THREADS_IN_PACKET];
   /**< informtion of profiled SW threads       */    
   /**< space allocated for maximum sw threads  */         
} prof_event_periodic_profiling_log_t;


/** @brief log record define*/
LOG_RECORD_DEFINE(LOG_QURT_TASKPROFILE_C)
  prof_event_periodic_profiling_log_t      log_header;
LOG_RECORD_END

#endif /* PROFTP_H */
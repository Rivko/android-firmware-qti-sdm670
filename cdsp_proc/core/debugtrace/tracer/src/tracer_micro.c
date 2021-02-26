/*=============================================================================

FILE:         tracer_micro.c

DESCRIPTION:    Minimal tracer functions and data that are required to be
                in micro image to facilitate SW events 

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "tracer.h"
#include "tracer_event_ids.h"
#include "tracer_config_int.h"

#include "tracer_portmgr.h"
#include "tracer_stp.h"
#include "tracer_ost.h"
#include "tracer_micro.h"
#include "dbglog.h"
#include "uTimetick.h"

/*-------------------------------------------------------------------------*/
boolean _is_event_simple_go(tracer_event_id_t eventId)
{
   return ( ( tcfg_is_event_on(eventId) && tcfg_is_op_enabled() &&
              (tcfg_is_event_initialized()) ) ? TRUE : FALSE );
}

/*-------------------------------------------------------------------------*/
void tracer_log_time_tick(tracer_event_id_t event_id) 
{
   if (QDSS_SSC_TIMETICK != event_id) {
      uint64 ticks = uTimetick_Get();
      UTRACER_EVENT(QDSS_SSC_TIMETICK,
                    event_id,
                    ((uint32)ticks),
                    ((uint32)(ticks >> 32)));
   }
}
/*-------------------------------------------------------------------------*/

#ifdef QDSS_UART_LOG

/*Helper functions for UART logging */

static __inline char tracer_last_nibble(uint32 num)
{
   char hex_digit = num & 0xF;
   return (hex_digit <=9) ? '0'+ hex_digit : 'a'+ (hex_digit-10);
}
/*-------------------------------------------------------------------------*/
/*Prefix hex string for num at a given position in the log buffer */
static __inline char *tracer_log_num(char *log_ptr, uint32 num) 
{
   do {                                                
      *log_ptr-- = tracer_last_nibble(num);
      num >>= 4;   //next digit
   } while(num!=0);

   *log_ptr-- =' '; //space separator
   return log_ptr;
}

/*-------------------------------------------------------------------------*/
/*Prefix the event id and 'QSEU' header and sent it out on wire */
static void tracer_dbglog_finish(char *log_buf, char * log_ptr, uint32 event_id)
{
   log_ptr = tracer_log_num(log_ptr,event_id);
	   
#ifdef TRACER_USER_PD 
   *log_ptr-- ='U';
#endif
   *log_ptr-- ='E';
   *log_ptr-- ='S';
   *log_ptr   ='Q';

   UDBG_LOG_NO_LINEFEED(log_ptr,(DBG_LOG_BUF_SIZE-(log_ptr-log_buf)));
}

/*-------------------------------------------------------------------------*/
static void tracer_dbglog_event(uint32 event_id) 
{
   char log_buf[DBG_LOG_BUF_SIZE];
   char *log_ptr=log_buf+DBG_LOG_BUF_SIZE-1;
	 
   *log_ptr-- ='\n';                                                 
   *log_ptr-- ='\r'; 

   tracer_dbglog_finish(log_buf,log_ptr,event_id);
}

/*-------------------------------------------------------------------------*/

static void tracer_dbglog_event_vargs(uint32 event_id,
                                      int num_args,
                                      va_list pArgs_copy) 
{
   //This would limit the log to atmost 127 characters.
#define TRACER_MAX_EVENT_ARGS  12  
   
   char log_buf[DBG_LOG_BUF_SIZE];
   uint32 pArgs_u32[TRACER_MAX_EVENT_ARGS];	 
   char *log_ptr=log_buf+DBG_LOG_BUF_SIZE-1;
   int i;

   if (TRACER_MAX_EVENT_ARGS < num_args) {
      num_args = TRACER_MAX_EVENT_ARGS;
   }
         
   for(i=0; i < num_args; i++) {
      pArgs_u32[i] = va_arg(pArgs_copy, uint32);
   }
	 
   *log_ptr-- ='\n';                                                 
   *log_ptr-- ='\r'; 
	 
   for(i=num_args-1; i >= 0; i--) {
      log_ptr = tracer_log_num(log_ptr, pArgs_u32[i]); 
   }

   tracer_dbglog_finish(log_buf,log_ptr,event_id);
}
#endif

/*-------------------------------------------------------------------------*/
void tracer_event_simple(tracer_event_id_t event_id) 
{
#if TRACER_SWEVT
   if (_is_event_simple_go(event_id))
   {
      tracer_log_time_tick(event_id);
      tracer_stp_send_event(tracer_port_d32(), event_id);
#ifdef QDSS_UART_LOG
      tracer_dbglog_event((uint32) event_id);
#endif
   }
#endif
   return;
}
/*-------------------------------------------------------------------------*/
void tracer_event_simple_vargs(tracer_event_id_t event_id, uint32 num_args, ...)
{
#if TRACER_SWEVT
   STMTracePortType port;
   int key;
   va_list pArgs;

   if (_is_event_simple_go(event_id))
   {
      tracer_log_time_tick(event_id);
      port = tracer_port_get(&key);
      va_start(pArgs, num_args);
         
#ifdef QDSS_UART_LOG
      {
         va_list pArgs_copy;   
         va_copy(pArgs_copy, pArgs);      
         tracer_dbglog_event_vargs(event_id,num_args,pArgs_copy);
         va_end(pArgs_copy);
      }
#endif

      tracer_ost_send_event_simple_vargs(port, event_id, num_args, pArgs);
      va_end(pArgs);
      tracer_port_free(key);
   }
#endif
   return;
}


#ifdef TRACER_MICRO_TEST
//This should link with out any dependencies outside the uimage
void tracer_micro_test(void) 
{
   UTRACER_EVENT(10);
   UTRACER_EVENT(20,30,40);
}
#endif

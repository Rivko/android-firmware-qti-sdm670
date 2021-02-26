/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            PROF Task Profiling

GENERAL DESCRIPTION
  Task profile logging, timer, and control


Copyright (c) 2002-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/* ---------------------------------------------------------------------------
** Includes
** ---------------------------------------------------------------------------  */
#include "customer.h"
#include "comdef.h"
#include "timer.h"
#include <stdlib.h>
#include "log.h"
#include "msg.h"
#include <string.h>
#include "qurt.h"
#include <npa.h>
#include "tracer.h"
#include "prof_log.h"
#include "prof_tp.h"
#include "stringl.h"

#define Q6_DEFAULT_FREQ 595200 /* in KHz */
#define Q6SW_NPA_RESOURCE_NAME "/clk/cpu"

#ifndef QURT_STACK_MAGIC
#define QURT_STACK_MAGIC 0xF8F8F8F8
#endif

#ifndef QURT_STACK_FILL_SPACING
#define QURT_STACK_FILL_SPACING 32
#endif

#include <stringl/stringl.h>

/* ---------------------------------------------------------------------------
** Data
** ---------------------------------------------------------------------------  */
#define PROFTP_STACK_SIZE 4096 // wild guess
#define PROFTP_TASK_PRIO  124  //approximately 1 slot < DIAG PRIO

static void proftp_start_thread ( void );

static struct {
   boolean           enabled;
   boolean           thread_init;
   uint32            tp_period;
   uint32            max_hw_threads;
   volatile uint32   core_clk_freq;
   uint32            sleep_clk_freq; /* 8960-32768, 8974-19200000 */
   npa_event_handle  npa_evt_handle;
   uint64            idle_bases[6];
} proftp = { 0 };


static qurt_anysignal_t timer_signal;
static timer_type ats_timer;

/* Absolute time that our profiling window starts. Moved forward in
   prof_profiling_clear_data. */
static uint32 proftp_tick_base = 0;
static uint64 proftp_pcycle_base = 0;

void proftp_core_clk_freq_cb( void         *context,
                       unsigned int  event_type, 
                       void         *data,       
                       unsigned int  data_size )
{
  npa_event_data *event_data;
  
  // check event_type
  if(event_type == NPA_EVENT_CHANGE)
  {
    event_data = (npa_event_data *)data;

    if (NULL != event_data)
    {
      //check data->resource_name      
      if (strcmp(event_data->resource_name, Q6SW_NPA_RESOURCE_NAME) == 0)
      {
        //check data->state, which is freq in KHz
        proftp.core_clk_freq = event_data->state;
      }
    }
  }
}

uint32 proftp_core_clk_freq_query (void)
{
  npa_query_status status;
  npa_query_type query_result;

  // Query the resource for the current state.
  status = npa_query_by_name(Q6SW_NPA_RESOURCE_NAME, NPA_QUERY_CURRENT_STATE, &query_result);
  if ( status == NPA_QUERY_SUCCESS )
  {
    // The value query_result.data.state has the result of the query.
    return query_result.data.state;
  }
  else
  {
    return 0;
  }
}

void proftp_npa_event_register(void)
{
  /* create the event - 
    Register a callback that will be invoked everytime the cpu clock frequency changes.  
    Return handle will be NULL if event could not be created */
  proftp.npa_evt_handle = npa_create_change_event_cb( 
                              Q6SW_NPA_RESOURCE_NAME, 
                              "CPU Freq change event",   /* Event Name - info only */
                              proftp_core_clk_freq_cb,          /* Callback Function */
                              NULL ); 

  /* read current core clock frequency */
  proftp.core_clk_freq = proftp_core_clk_freq_query();
  if (0 == proftp.core_clk_freq)
  {
    proftp.core_clk_freq = Q6_DEFAULT_FREQ; 
  }
}

void proftp_npa_event_deregister(void)
{
  /* destroy the event when done */
  if (NULL != proftp.npa_evt_handle)
  {
    npa_destroy_event_handle(proftp.npa_evt_handle);
  }
}


/*===========================================================================
FUNCTION PROF_PROFILING_CLEAR_DATA

DESCRIPTION
  This function clears the profiling data collected by prof profiling.
============================================================================*/
static void prof_profiling_clear_data( void )
{
   int iter;
   qurt_thread_t thread_id;
   /* System ticks should always be the logest. So first collect ticks
    * then pcycles followed by resetting thread run pcycles */
   proftp_tick_base   = qurt_sysclock_get_hw_ticks();
   proftp_pcycle_base = qurt_get_core_pcycles ();

   iter = qurt_thread_iterator_create();
   
   while ((thread_id = qurt_thread_iterator_next(iter)) != THREAD_ITERATOR_END) {
      qurt_profile_reset_threadid_pcycles (thread_id);
   }

   qurt_thread_iterator_destroy(iter);

   return;
}


/*===========================================================================
FUNCTION PROF_ENABLE_TASK_PROFILE

DESCRIPTION
  This function fills in the system profile period, task profile period and
  number of tasks per period. It calls the task profile logging function and
  system profile logging function.
============================================================================*/
void prof_enable_task_profile(uint32 task_period)
{
	qurt_sysenv_max_hthreads_t mhwt;
   
   if( proftp.enabled == FALSE )
   {
      proftp.enabled = TRUE;
      proftp.tp_period = task_period;
      qurt_sysenv_get_max_hw_threads(&mhwt);
      proftp.max_hw_threads = mhwt.max_hthreads;
      proftp.sleep_clk_freq  = 19200000;
      
      
      prof_profiling_clear_data();
      /* get the idle thread bases which will be used to calculate idle counters */
      qurt_profile_get_idle_pcycles(proftp.idle_bases);
      qurt_profile_enable (1);
      
      if(proftp.thread_init == FALSE)
      {
         proftp.thread_init = TRUE;
      
         qurt_anysignal_init (&timer_signal);

         timer_def_osal(&ats_timer, &timer_non_defer_group, TIMER_NATIVE_OS_SIGNAL_TYPE, &timer_signal, 0x1);
         timer_set_64(&ats_timer, proftp.tp_period, proftp.tp_period, T_MSEC);
         
         proftp_npa_event_register();
         
         proftp_start_thread();
                     
		}
		else 
		{
            timer_def_osal(&ats_timer, &timer_non_defer_group, TIMER_NATIVE_OS_SIGNAL_TYPE, &timer_signal, 0x1);
            timer_set_64(&ats_timer, proftp.tp_period, proftp.tp_period, T_MSEC);
		}
   }
   return;
} /* END prof_enable_task_profile */


/*===========================================================================
FUNCTION PROF_DISABLE_TASK_PROFILE

DESCRIPTION
  This function clears the timers both the prof_sys_profile_timer and
  prof_per_task_profile_timer
============================================================================*/
void prof_disable_task_profile(void)
{
   if( proftp.enabled == TRUE )
   {
      proftp.enabled = FALSE;

      qurt_profile_enable (0);

      proftp_npa_event_deregister();

      qurt_anysignal_set(&timer_signal, 0x2);
   }
   return;
} /* END prof_disable_task_profile */

/*===========================================================================

FUNCTION proftp_task

DESCRIPTION
  handle timers expiry for task profiling
  so that AMSS TIMER task doesn't absorb
  all of the overhead. Loops forever, never exits.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void proftp_task(void* ignored)
{
   LOG_QURT_TASKPROFILE_C_type *tp_ptr;
	
   unsigned int ret_signal;
   int count, index;
   uint64 this_sclk_ticks = 0;
   uint64 this_pcycle_count = 0;
   int iter;
   qurt_thread_t thread_id = THREAD_ITERATOR_END;
   uint64 pcycles[6];
   tracer_client_handle_t handle;
   tracer_return_enum_t result;
   int tracer_registered = 0;
   unsigned int stack_base;
   unsigned int stack_size;
   unsigned int *stack_checker_ptr = 0;
   unsigned int stack_free = 0;
   int more_threads = 1;

   /* register QDSS tracer */
   result = tracer_register(&handle, TRACER_ENTITY_PROF, TRACER_PROTOCOL_PROF_TP);
   if ((result == TRACER_RETURN_SUCCESS) && (handle != 0))
   {
      tracer_registered = 1;
   }
	
   do
   {		
      ret_signal = qurt_anysignal_wait(&timer_signal, 0x3);
      if(ret_signal & 0x1)
      {         
         iter = qurt_thread_iterator_create();

         more_threads = 1;
         while (more_threads) {
            count = 0;

            if (thread_id == THREAD_ITERATOR_END) //if thread_id contains valid id from last iterration in while loop, skip reading it again
            {
               thread_id = qurt_thread_iterator_next(iter);
               if (thread_id == THREAD_ITERATOR_END)
               {
                   more_threads = 0;
                   break; //no more threads in the list.
               }
            }
                
            tp_ptr = (LOG_QURT_TASKPROFILE_C_type *)log_alloc(LOG_QURT_TASKPROFILE_C, sizeof (LOG_QURT_TASKPROFILE_C_type));

            /* Fill in the log if allocation worked */
            if (tp_ptr != NULL ) {
               tp_ptr->log_header.event_payload.version_info = 1;
               tp_ptr->log_header.event_payload.processor = PROCESSOR_ID;
               tp_ptr->log_header.event_payload.hw_thread_num = proftp.max_hw_threads;
               tp_ptr->log_header.event_payload.sclk_freq = proftp.sleep_clk_freq;
               tp_ptr->log_header.event_payload.core_freq = proftp.core_clk_freq;
            
               this_pcycle_count = qurt_get_core_pcycles ();
               this_sclk_ticks   = qurt_sysclock_get_hw_ticks();

               tp_ptr->log_header.event_payload.total_pcycles    = this_pcycle_count - proftp_pcycle_base;
               tp_ptr->log_header.event_payload.total_sclk_ticks = this_sclk_ticks - proftp_tick_base;
          
			   
               //fill in each thread, up to 32 threads
               while( more_threads && count<MAX_THREADS_IN_PACKET )
               {
                  tp_ptr->log_header.sw_threads_info[count].thread_id = thread_id;
                  qurt_thread_context_get_tname(thread_id, 
                  tp_ptr->log_header.sw_threads_info[count].thread_name, 16);
                  
                  qurt_thread_context_get_pname(thread_id, 
                  tp_ptr->log_header.sw_threads_info[count].proc_name, 16);

                  qurt_thread_context_get_pid(thread_id, 
                  &tp_ptr->log_header.sw_threads_info[count].proc_id);
                  
                  qurt_thread_context_get_prio(thread_id, 
                  (unsigned char*)(&tp_ptr->log_header.sw_threads_info[count].thread_prio));
                  
                  /* Collect run time statistics of the task from kernel */
                  qurt_profile_get_threadid_pcycles (thread_id, pcycles);
                  
                  /* initialize the array */
                  memset (tp_ptr->log_header.sw_threads_info[count].thread_pcycles, 0, proftp.max_hw_threads * 8);
                  /* Thread run time per HW thread */
                  (void) memscpy (tp_ptr->log_header.sw_threads_info[count].thread_pcycles, proftp.max_hw_threads * 8, pcycles, proftp.max_hw_threads * 8);                       
                  
                  qurt_thread_context_get_stack_base(thread_id, &stack_base);                  
                  qurt_thread_context_get_stack_size(thread_id, &stack_size);

                  /* estimate the free stack */
                  stack_checker_ptr = (unsigned int*)stack_base;
                  for (stack_free = 0; stack_free < stack_size;
                     stack_free += QURT_STACK_FILL_SPACING * 4, stack_checker_ptr += QURT_STACK_FILL_SPACING)
                  {
                     if (QURT_STACK_MAGIC != *stack_checker_ptr) break;
                  }

                  if (stack_free > stack_size)
                     stack_free = stack_size;

                  /* hijack base as stack usage percentage*/
                  tp_ptr->log_header.sw_threads_info[count].stack_base = stack_free;                  
                  tp_ptr->log_header.sw_threads_info[count].stack_size = stack_size;

                  count++;

                  thread_id = qurt_thread_iterator_next(iter);
                  if (thread_id == THREAD_ITERATOR_END)
                  {
                      more_threads = 0;
                      break;
                  }                  
               }

               if (count < MAX_THREADS_IN_PACKET)
               {
                  more_threads = 0;
               }
            
               tp_ptr->log_header.event_payload.sw_thread_num_in_packet = count;

               if (tracer_registered)
               {
                  result = tracer_data(handle, 0, sizeof (LOG_QURT_TASKPROFILE_C_type), (const char*)tp_ptr);
                  if(result != TRACER_RETURN_SUCCESS)
                  {
                     //warning message
                  }
               }
               
               log_commit (tp_ptr);
            }   
         }
         qurt_thread_iterator_destroy(iter);
         
         
         //Filling out idle thread information
         tp_ptr = (LOG_QURT_TASKPROFILE_C_type *)log_alloc(LOG_QURT_TASKPROFILE_C, sizeof (LOG_QURT_TASKPROFILE_C_type));         

         /* Fill in the log if allocation worked */
         if (tp_ptr != NULL ) {
                     
            tp_ptr->log_header.event_payload.version_info = 1;				
            tp_ptr->log_header.event_payload.processor = PROCESSOR_ID;            
            tp_ptr->log_header.event_payload.hw_thread_num = proftp.max_hw_threads;
            tp_ptr->log_header.event_payload.sclk_freq = proftp.sleep_clk_freq;
            tp_ptr->log_header.event_payload.core_freq = proftp.core_clk_freq;
            
            this_pcycle_count = qurt_get_core_pcycles ();
            this_sclk_ticks   = qurt_sysclock_get_hw_ticks();
            
            /*Collect statistics of the IDLE threads */
            qurt_profile_get_idle_pcycles (pcycles);
            
            /*get the delta idle pcycles*/
            for(index=0; index<proftp.max_hw_threads; index++)
            {
               pcycles[index] -= proftp.idle_bases[index];
            }
            
            tp_ptr->log_header.event_payload.total_pcycles    = this_pcycle_count - proftp_pcycle_base;
            tp_ptr->log_header.event_payload.total_sclk_ticks = this_sclk_ticks - proftp_tick_base;
          
            tp_ptr->log_header.sw_threads_info[0].thread_id = -1;
            
            tp_ptr->log_header.sw_threads_info[0].proc_id = 0;                
            tp_ptr->log_header.sw_threads_info[0].thread_prio = 0;
                  
            tp_ptr->log_header.sw_threads_info[0].stack_base = 0;
            tp_ptr->log_header.sw_threads_info[0].stack_size = 0;
            
            /* initialize the array */
            memset (tp_ptr->log_header.sw_threads_info[0].thread_pcycles, 0, proftp.max_hw_threads * 8);
            /* Thread run time per HW thread */
            (void) memscpy (tp_ptr->log_header.sw_threads_info[0].thread_pcycles, proftp.max_hw_threads * 8, pcycles, proftp.max_hw_threads * 8);
      
            tp_ptr->log_header.event_payload.sw_thread_num_in_packet = 1;           

            if (tracer_registered)
            {
               result = tracer_data(handle, 0, sizeof (LOG_QURT_TASKPROFILE_C_type), (const char*)tp_ptr);
               if(result != TRACER_RETURN_SUCCESS)
               {
                  //warning message
               }
            }
               
            log_commit (tp_ptr);
         }         

         qurt_anysignal_clear(&timer_signal, 0x1);
	  }
      else if(ret_signal &0x2)
      {
         timer_undef(&ats_timer);
         qurt_anysignal_clear(&timer_signal, 0x2);
      }
   } while(1);
   return;
}

static void proftp_start_thread ( void )
{
   qurt_thread_t t;
   qurt_thread_attr_t t_attr;
   char *thread_stack;
   thread_stack = malloc(PROFTP_STACK_SIZE);  
   
   qurt_thread_attr_init(&t_attr);
   qurt_thread_attr_set_name(&t_attr, "Prof TP");
   qurt_thread_attr_set_stack_addr(&t_attr, thread_stack);
   qurt_thread_attr_set_stack_size(&t_attr, PROFTP_STACK_SIZE);
   qurt_thread_attr_set_priority(&t_attr, PROFTP_TASK_PRIO);
   qurt_thread_create(&t, &t_attr, proftp_task, NULL);
} /* proftp_start_thread */


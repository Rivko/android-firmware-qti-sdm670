/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            Performance Monitoring

GENERAL DESCRIPTION
  Provides DIAG interface to configure and collect performance information.
  Sets up timers to be able to collect performance information periodically
  and send the log packets. Uses the native performance monitoring supported
  by QDSP6.


Copyright (c) 2002-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: 

when         who     what, where, why
----------   ---     --------------------------------------------------------


===========================================================================*/

/* ---------------------------------------------------------------------------
** Includes
** ---------------------------------------------------------------------------  */
#include "customer.h"
#include "comdef.h"
#include "timer.h"
#include "log.h"
#include "msg.h"
#include <string.h>
#include <stdlib.h>
#include "qurt.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "tracer.h"
#include <npa.h>
#include "prof_pmon.h"
#include "prof_log.h"
#include "stringl.h"

#ifdef FEATURE_MULTIPROCESSOR
#include "log_codes.h"
#endif

/* this is a global variable provided by QURT */
extern unsigned int QURT_MAX_HTHREADS;

#define LOG_QURT_PM_C_VER         1
#define LOG_BLAST_PM_C_VER         1

#define PROF_TASK_STACK_SIZE       4096
#define PROF_TASK_PRIO  124  //approximately 1 slot < DIAG PRIO

#define ADSP_EVENT_PERIODIC_PROFILING_3         0x00011285

#define Q6_DEFAULT_FREQ 595200 /* in KHz */
#define Q6SW_NPA_RESOURCE_NAME "/clk/cpu"

static struct {
    boolean enabled;
    uint32 max_hw_threads;
    boolean thread_init;
    uint8 evt0;
    uint8 evt1;
    uint8 evt2;
    uint8 evt3;
    uint32 period;
    uint64 tick_base; /* Absolute time that our PM starts. */
    uint64 pcycle_base;
    uint64 idle_bases[6];
    qurt_anysignal_t timer_signal;
    //qurt_timer_t timer;
    //qurt_timer_attr_t attr;
    timer_type ats_timer;

    qurt_sem_t sem;
    volatile uint32 core_clk_freq;
    npa_event_handle npa_evt_handle;
} prof_qurt_pmon = { 0 };

/*===========================================================================
FUNCTION PROF_CORE_CLK_FREQ_CB

DESCRIPTION
   Get CPU frequency
============================================================================*/
void prof_qurt_core_clk_freq_cb( void  *context,
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
                prof_qurt_pmon.core_clk_freq = event_data->state;
            }
        }
    }
}

uint32 prof_core_clk_freq_query (void)
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

void prof_qurt_npa_event_register(void)
{
    /* create the event - 
      Register a callback that will be invoked everytime the cpu clock frequency changes.  
      Return handle will be NULL if event could not be created */
     prof_qurt_pmon.npa_evt_handle = npa_create_change_event_cb( 
                                Q6SW_NPA_RESOURCE_NAME, 
                                "CPU Freq change event",   /* Event Name - info only */
                                prof_qurt_core_clk_freq_cb,          /* Callback Function */
                                NULL ); 
  
    /* read current core clock frequency */
    prof_qurt_pmon.core_clk_freq = prof_core_clk_freq_query();
    if (0 == prof_qurt_pmon.core_clk_freq)
    {
        prof_qurt_pmon.core_clk_freq = Q6_DEFAULT_FREQ; 
    }
}

/*===========================================================================
FUNCTION prof_qurt_pmon_task

DESCRIPTION
   Collects PMU counters, sclk ticks, core pcycles and idle pcycles.
   Constructs log packets and sends out. Re-creates the timer for the next
   call back.
============================================================================*/
static void prof_qurt_pmon_task (void* ignore)
{
    LOG_QURT_PM_C_type *pm_ptr;
    uint64 this_timestamp;
    uint64 this_pcycle_count; 
    unsigned int ret_signal;
    uint64 pcycles[6];
    int    index;
    tracer_client_handle_t handle;
    tracer_return_enum_t result;
    int tracer_registered = 0;
    

    /* register QDSS tracer */
    result = tracer_register(&handle, TRACER_ENTITY_PROF, TRACER_PROTOCOL_PROF_PM);
    if ((result == TRACER_RETURN_SUCCESS) && (handle != 0))
    {
        tracer_registered = 1;
    }
 
    do 
    {
        //wait for signal
        ret_signal = qurt_anysignal_wait(&prof_qurt_pmon.timer_signal, 0x3);
        if(ret_signal & 0x1)
        {
            pm_ptr = (LOG_QURT_PM_C_type *)log_alloc(LOG_QURT_PM_C,
                                  sizeof (LOG_QURT_PM_C_type));
          
            /* Fill in the log if allocation worked */
            if (pm_ptr != NULL ) 
            {
                /* version no. */
                pm_ptr->ver = LOG_QURT_PM_C_VER;
                /* App - 0 - Modem SW, 1 - Modem FW, 2 - Multi Media */
                pm_ptr->app = PROCESSOR_ID;
                pm_ptr->hw_thread_num = prof_qurt_pmon.max_hw_threads;
                 
                /* Read PM counters */
                pm_ptr->counters[0] = qurt_pmu_get (QURT_PMUCNT0);
                pm_ptr->counters[1] = qurt_pmu_get (QURT_PMUCNT1);
                pm_ptr->counters[2] = qurt_pmu_get (QURT_PMUCNT2);
                pm_ptr->counters[3] = qurt_pmu_get (QURT_PMUCNT3);

                pm_ptr->events[0] =  prof_qurt_pmon.evt0;
                pm_ptr->events[1] =  prof_qurt_pmon.evt1;
                pm_ptr->events[2] =  prof_qurt_pmon.evt2;
                pm_ptr->events[3] =  prof_qurt_pmon.evt3;
      
                /* Collect statitstics of the IDLE threads */
                qurt_profile_get_idle_pcycles (pcycles);
           
                /* get the delta idle pcycles */
                for (index=0; index<6; index++)
                {
                    pcycles[index] -= prof_qurt_pmon.idle_bases[index];
                }
           
                /* Collect the thread run time followed by system time
                 * so that system run time is always larger */
                this_pcycle_count = qurt_get_core_pcycles ();
           
                /* Collect the system time */
                this_timestamp = qurt_sysclock_get_hw_ticks ();
           
                /* IDLE time per HW thread */
                memscpy (pm_ptr->idle_pcycles, 6*8, pcycles, 6*8);
                /*  Total Pcycles */
                pm_ptr->core_pcycles = this_pcycle_count - prof_qurt_pmon.pcycle_base;
                /* Total sleep clock ticks elapsed */
                pm_ptr->timer_ticks = this_timestamp - prof_qurt_pmon.tick_base;

                pm_ptr->timer_freq = 19200000;
                pm_ptr->core_freq = prof_qurt_pmon.core_clk_freq;

                // notify the enable func that it can configure pmu again
                qurt_sem_up(&prof_qurt_pmon.sem);

                if (tracer_registered)
                {
                    result = tracer_data(handle, 0, sizeof (LOG_QURT_PM_C_type), (const char*)pm_ptr);
                    if(result != TRACER_RETURN_SUCCESS)
                    {
                        //warning message
                    }
                }
                 
                log_commit (pm_ptr);
            }        
            qurt_anysignal_clear(&prof_qurt_pmon.timer_signal, 0x1);

            /* prepare for next measurement if still enabled */
            if( prof_qurt_pmon.enabled == TRUE )
            {
                /* reset bases */
                //prof_qurt_pmon.tick_base = qurt_sysclock_get_hw_ticks ();
                //prof_qurt_pmon.pcycle_base = qurt_get_core_pcycles ();
                //qurt_profile_get_idle_pcycles(prof_qurt_pmon.idle_bases);
    
                /* restart the one-shot timer */
                timer_set_64(&prof_qurt_pmon.ats_timer, prof_qurt_pmon.period, 0, T_MSEC);
    
                /* clear pmu counters but not turn off pmu*/
                //qurt_pmu_set (QURT_PMUCNT0, 0);
                //qurt_pmu_set (QURT_PMUCNT1, 0);
                //qurt_pmu_set (QURT_PMUCNT2, 0);
                //qurt_pmu_set (QURT_PMUCNT3, 0);
            }

        }
        else if(ret_signal &0x2)
        {
           //qurt_timer_delete(prof_qurt_pmon.timer);
           qurt_anysignal_clear(&prof_qurt_pmon.timer_signal, 0x2);
        }
    } while(1);
 
    return;
}


/*===========================================================================
FUNCTION prof_enable_qurt_pmon

DESCRIPTION
  This function fills in the events and, performance monitoring period.
  It collects the sclk ticks  and core pcycles to establish the offset.
  Creates timer with call back function.
============================================================================*/
void prof_enable_qurt_pmon (uint8 evt0,
                    uint8 evt1,
                    uint8 evt2,
                    uint8 evt3,
                    uint32 period)
{
    qurt_thread_attr_t t_attr;
    qurt_thread_t t;
    char* thread_stack;
    qurt_sysenv_max_hthreads_t mhwt;
       
    if( prof_qurt_pmon.enabled == FALSE )
    {
        prof_qurt_pmon.enabled = TRUE; 
        prof_qurt_pmon.period = period; //keep it in ms

        qurt_sysenv_get_max_hw_threads(&mhwt);
        prof_qurt_pmon.max_hw_threads = mhwt.max_hthreads;        
    
        if(prof_qurt_pmon.thread_init == FALSE)
        {
            prof_qurt_pmon.thread_init = TRUE;

            thread_stack = malloc(PROF_TASK_STACK_SIZE);  
            if (thread_stack == 0)
            {
                prof_qurt_pmon.thread_init = FALSE;
                return;
            }      
            
            qurt_anysignal_init(&prof_qurt_pmon.timer_signal);
            timer_def_osal(&prof_qurt_pmon.ats_timer, &timer_non_defer_group, TIMER_NATIVE_OS_SIGNAL_TYPE, &prof_qurt_pmon.timer_signal, 0x1);
            
            prof_qurt_npa_event_register();     
            
            qurt_thread_attr_init(&t_attr);
            qurt_thread_attr_set_name(&t_attr, "QuRT Prof PMon");
            qurt_thread_attr_set_stack_addr(&t_attr, thread_stack);
            qurt_thread_attr_set_stack_size(&t_attr, PROF_TASK_STACK_SIZE);
            qurt_thread_attr_set_priority(&t_attr, PROF_TASK_PRIO);
            qurt_thread_create(&t, &t_attr, prof_qurt_pmon_task, NULL);

            //only init sem once
            qurt_sem_init_val(&prof_qurt_pmon.sem, 1);
        }
            
        //wait for the last read operation is over
        qurt_sem_down(&prof_qurt_pmon.sem);

        /* System ticks should always be the longest. So first collect ticks
         * then reset counters */
        prof_qurt_pmon.tick_base = qurt_sysclock_get_hw_ticks ();
        prof_qurt_pmon.pcycle_base = qurt_get_core_pcycles ();
        qurt_profile_get_idle_pcycles(prof_qurt_pmon.idle_bases);

        /* restart the one-shot timer */
        timer_set_64(&prof_qurt_pmon.ats_timer, prof_qurt_pmon.period, 0, T_MSEC);
        
        prof_qurt_pmon.evt0 = evt0;
        prof_qurt_pmon.evt1 = evt1;
        prof_qurt_pmon.evt2 = evt2;
        prof_qurt_pmon.evt3 = evt3;

        /* Configure PMU events. This will clear the PMU counters as well */
        qurt_pmu_set (QURT_PMUEVTCFG, evt0 | (evt1 << 8)
                                             | (evt2 << 16)
                                             | (evt3 << 24));
        
        qurt_pmu_set (QURT_PMUCFG, (1<<prof_qurt_pmon.max_hw_threads) - 1);

        /* Turn ON Performance Monitoring */
        qurt_pmu_enable (1);        
    }  
    return;
} /* END prof_enable_qurt_pmon */

/*===========================================================================
FUNCTION prof_disable_qurt_pmon

DESCRIPTION
  This function clears the timer
============================================================================*/
void prof_disable_qurt_pmon (void)
{ 
    if( prof_qurt_pmon.enabled == TRUE )
    {
        prof_qurt_pmon.enabled = FALSE;  
        qurt_pmu_enable (0);  
        qurt_anysignal_set(&prof_qurt_pmon.timer_signal, 0x2);
    }
    return;    
} /* END prof_disable_qurt_pmon */


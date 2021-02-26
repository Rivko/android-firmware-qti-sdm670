/*
#============================================================================
#  Name:
#    dog_hb.c 
#
#  Description:
#    Watchdog Heartbeat monitoring
#
# Copyright (c) 2015-2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
#============================================================================
*/

#include "stdarg.h"
#include "stdlib.h"
#include "stringl/stringl.h"
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "DALSys.h"
#include "rcinit.h"
#include "err.h"
#include "qurt.h"
#include "tms_utils_msg.h"
#include "tms_utils.h"
#include "queue.h"
#include "dog_hb_internal.h"
#include "sys_m_internal.h"
#include "rcevt_qurt.h"
#include "dog_hal.h"

/* Tunable values */
#define DOG_HB_LIST_CNT                           5                              /* Number of Linked Lists in the Queue */

#define DOG_HB_PING_ERROR_CNT                     3                              /* Ping No Response Error Count */
#define DOG_HB_CLIENT_NAME_RESERVED               "doghb"                        /* Reserved Device Name */
#define DOG_HB_SHUTDOWN_MASK                      0x00000001                     /* SYS_M_SHUTDOWN mask */
#define DOG_HB_DEVICE_PING_MASK                   0x00000002                     /* Internal ping mask used */
#define DOG_HB_DEVICE_MONITOR_MASK                0x00000004                     /* Internal monitoring mask used */

/* If this flag is set to TRUE, watchdog heartbeat will be disabled.
*  This flag should be set to TRUE only for DEBUG purposes eg TRACE32 Instrument to Disable heartbeat wdog at runtime
*/
static volatile boolean dog_hb_disable = FALSE;

/* Type casts as accessor functions */
#define dog_hb_device2dog_hb_handle(x)        ((dog_report_type)x)
#define dog_hb_handle2dog_hb_device(x)        ((dog_hb_device_p)x)

/* watchdog internal */
dog_hb_internal_t dog_hb_internal;

/* static functions */
static DOG_HB_STATUS dog_hb_ping_clients(void);
static DOG_HB_STATUS dog_hb_pong(dog_report_type handle);
static DOG_HB_STATUS dog_hb_pong_check(void);

/** =====================================================================
 * Function:
 *     dog_hb_mutex_init
 *
 * Description:
 *     Initialize mutex
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
static inline void dog_hb_mutex_init(void)
{
   if (!dog_hb_internal.mutex_inited)
   {
      qurt_pimutex_init(&dog_hb_internal.mutex);

      dog_hb_internal.mutex_inited = 1;
   }
}

/** =====================================================================
 * Function:
 *     dog_hb_mutex_lock
 *
 * Description:
 *     Lock the mutex
 *
 * Parameters:
 *    None
 *
 * Returns:
 *    None
 * =====================================================================  */
static inline void dog_hb_mutex_lock(void)
{
   if (dog_hb_internal.mutex_inited)
   {
      qurt_pimutex_lock(&dog_hb_internal.mutex);
   }
}

/** =====================================================================
 * Function:
 *     dog_hb_mutex_unlock
 *
 * Description:
 *     Unlock the mutex
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
static inline void dog_hb_mutex_unlock(void)
{
   if (dog_hb_internal.mutex_inited)
   {
      qurt_pimutex_unlock(&dog_hb_internal.mutex);
   }
}

/** =====================================================================
* Function:
*     dog_hb_ping_clients
*
* Description:
*     PINGS all the clients registered in the first list in the queue,
*     i.e sets the OS signal/mask the clients registered with
*
* Parameters:
*     none
*
* Returns:
*     DOG_HB_STATUS
* =====================================================================  */
static DOG_HB_STATUS dog_hb_ping_clients(void)
{
   DOG_HB_STATUS rc = DOG_HB_STATUS_ERROR;
   dog_hb_device_p device_p_curr = NULL;
   dog_hb_queue_p dog_hb_q_entry = NULL;

   dog_hb_mutex_lock();

   /* Work with the first list in the queue without removing the list from the queue */
   dog_hb_q_entry = (dog_hb_queue_p)q_check(dog_hb_internal.dog_hb_q);

   if(NULL != dog_hb_q_entry)
   {
      for(device_p_curr = dog_hb_q_entry->q_ping_head; NULL != device_p_curr; device_p_curr = device_p_curr->next)
      {
         if(NULL != device_p_curr->sig_p)
         {
            if(DOG_HB_SIG_QURTSIGNAL == device_p_curr->sig_t)
            {
               DOG_HB_SIGNAL_SET_QURT(device_p_curr->sig_p, device_p_curr->sig_m);
            }
            else if(DOG_HB_SIG_REXSIGNAL == device_p_curr->sig_t)
            {
               DOG_HB_SIGNAL_SET_REX(device_p_curr->sig_p, device_p_curr->sig_m);
            }
            else
            {
              ERR_FATAL("Incorrect DOG HB device type", device_p_curr->sig_t, 0, 0);
            }
            /* Increment Ping */
            device_p_curr->ping++;
         }
      }
   }

   dog_hb_mutex_unlock();

   rc = DOG_HB_STATUS_SUCCESS;

   TMS_MSG_LOW("Ping check");

   return rc;
}

/** =====================================================================
* Function:
*     dog_hb_pong
*
* Description:
*     Clients reports back (PONGS) to the watchdog.
*
* Parameters:
*     handle : Clients registered watchdog handle
*
* Returns:
*     DOG_HB_STATUS
* =====================================================================  */
static DOG_HB_STATUS dog_hb_pong(dog_report_type handle)
{
   DOG_HB_STATUS rc = DOG_HB_STATUS_ERROR;
   dog_hb_device_p device_p = NULL;

   device_p = dog_hb_handle2dog_hb_device(handle);

   if(NULL != device_p)
   {
      dog_hb_mutex_lock();

      /* Clear ping */
      device_p->ping = 0;

      dog_hb_mutex_unlock();

      rc = DOG_HB_STATUS_SUCCESS;

      TMS_MSG_SPRINTF_LOW_1("Pong by %s", device_p->name);
   }
   else
   {
      TMS_MSG_LOW("Pong failed. Handle NULL.");
   }

   return rc;
}

/** =====================================================================
* Function:
*     dog_hb_pong_check
*
* Description:
*     Dog HB checks the pong value of the registered clients in the first list in the queue
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
static DOG_HB_STATUS dog_hb_pong_check(void)
{
   DOG_HB_STATUS rc = DOG_HB_STATUS_SUCCESS;
   dog_hb_device_p device_p_curr = NULL;
   dog_hb_queue_p dog_hb_q_entry = NULL;

   dog_hb_mutex_lock();

   /* Operate only on the first list in the queue */
   dog_hb_q_entry = (dog_hb_queue_p)q_get(dog_hb_internal.dog_hb_q);

   if(NULL != dog_hb_q_entry)
   {
      for(device_p_curr = dog_hb_q_entry->q_ping_head; NULL != device_p_curr; device_p_curr = device_p_curr->next)
      {
         if(DOG_HB_PING_ERROR_CNT < device_p_curr->ping)
         {
            char msg[TMS_UTILS_BUFFER_SIZE]; /* Allocated on stack. We're about to crash anyway. */

            rc = DOG_HB_STATUS_ERROR;

            /* Restore the state of the queue */
            /* Put the list to the end of the queue. The starved task will be present in the 5th list */
            q_put(dog_hb_internal.dog_hb_q, q_link(dog_hb_q_entry, &dog_hb_q_entry->link));

            (void)tms_utils_fmt(msg, TMS_UTILS_BUFFER_SIZE, "Task starvation: %s, ping: %d, triage with owner(d.dump 0x%x)", device_p_curr->name, device_p_curr->ping, (unsigned int)device_p_curr);

            ERR_FATAL_AUX_MSG("DOG_HB detects starvation of task, check coredump.err.aux_msg", msg, TMS_UTILS_BUFFER_SIZE);

            break;
         }
      }
   }

   /* Put the list to the end of the queue */
   q_put(dog_hb_internal.dog_hb_q, q_link(dog_hb_q_entry, &dog_hb_q_entry->link));

   dog_hb_mutex_unlock();

   return rc;
}

/** =====================================================================
* Function:
*     dog_hb_register
*
* Description:
*     Registers clients for watchdog hb monitoring
*
* Parameters:
*     task_name : Client task name
*     sig_t     : Client supplied signal type information. Qurt or Rex signal type
*     sig_p     : Client supplied signal information. Qurt or Rex signal
*     sig_m     : Client supplied mask information.
*
* Returns:
*     dog_report_type : Dog heartbeat handle returned back and should be greater than 0
* =====================================================================  */
DLL_API_GLOBAL dog_report_type dog_hb_register(char const* task_name, DOG_HB_SIG sig_t, void* sig_p, unsigned int sig_m)
{
   dog_report_type rc = 0;
   dog_hb_device_p device_p = NULL;
   unsigned int i, min_value = 0;
   dog_hb_queue_p dog_hb_q_entry = NULL, dog_hb_q_entry_next = NULL, dog_hb_q_entry_min = NULL;
   boolean entry_found = FALSE;
   int len1 = 0,len2 = 0;

   if(NULL == task_name || DOG_HB_SIG_REXSIGNAL > sig_t || NULL == sig_p || 0 == sig_m)
   {
      TMS_MSG_ERROR("Invalid inputs");
      return 0;
   }

   len1 = strlen(task_name);
   if(len1 >= DOG_HB_CLIENT_NAME_LEN)
   {
      TMS_MSG_ERROR_1("Invalid task_name length: %d",len1);
      return 0;
   }

   dog_hb_mutex_lock();

   dog_hb_q_entry = (dog_hb_queue_p)q_check(dog_hb_internal.dog_hb_q);

   /* Locate existing named device */
   for(i = 0; i < DOG_HB_LIST_CNT; i++)
   {
      if(NULL != dog_hb_q_entry)
      {
         for(device_p = dog_hb_q_entry->q_ping_head; NULL != device_p; device_p = device_p->next)
         {
            len2 = strlen(device_p->name);
            if(len1 == len2 && 0 == tms_utils_chr_compare(device_p->name, len2, task_name, len1))
            {
               TMS_MSG_SPRINTF_HIGH_2("DOG_HB detects same name of task: %s & device_p->name: %s", task_name, device_p->name);
               /* Operate on existing */
               rc = dog_hb_device2dog_hb_handle(device_p);
               entry_found = TRUE;
               break;
            }
         }
      }

      if(TRUE == entry_found)
      {
         break;
      }

      dog_hb_q_entry_next = (dog_hb_queue_p)q_next(dog_hb_internal.dog_hb_q, &dog_hb_q_entry->link);
      dog_hb_q_entry = dog_hb_q_entry_next;
   }

   if(NULL == device_p)
   {
      /* Allocate new */
      device_p = (dog_hb_device_p)malloc(sizeof(dog_hb_device_t));

      if(NULL != device_p)
      {
         /* Initialize new */
         secure_memset(device_p, 0, sizeof(dog_hb_device_t));

         strlcpy(device_p->name, task_name, DOG_HB_CLIENT_NAME_LEN);

         device_p->sig_t = sig_t;
         device_p->sig_p = sig_p;
         device_p->sig_m = sig_m;
         device_p->ping = 0;

         /* Locate the shortest list in the queue */
         dog_hb_q_entry_min = (dog_hb_queue_p)q_check(dog_hb_internal.dog_hb_q);
         dog_hb_q_entry = (dog_hb_queue_p)q_next(dog_hb_internal.dog_hb_q, &dog_hb_q_entry_min->link);

         if(NULL != dog_hb_q_entry_min)
         {
            min_value = dog_hb_q_entry_min->dog_hb_total_q_clients;

            /* Need to iterate only 4 times */
            for(i = 1; i < DOG_HB_LIST_CNT; i++)
            {
               if(NULL != dog_hb_q_entry)
               {
                  if(dog_hb_q_entry->dog_hb_total_q_clients < min_value)
                  {
                     dog_hb_q_entry_min = dog_hb_q_entry;
                     min_value = dog_hb_q_entry->dog_hb_total_q_clients;
                  }

                  dog_hb_q_entry_next = (dog_hb_queue_p)q_next(dog_hb_internal.dog_hb_q, &dog_hb_q_entry->link);
                  dog_hb_q_entry = dog_hb_q_entry_next;
               }
            }

            /* Link new into device list */
            device_p->next = dog_hb_q_entry_min->q_ping_head;
            dog_hb_q_entry_min->q_ping_head = device_p;
            dog_hb_q_entry_min->dog_hb_total_q_clients = dog_hb_q_entry_min->dog_hb_total_q_clients + 1;

            rc = dog_hb_device2dog_hb_handle(device_p);
         }
      }
      else
      {
         TMS_MSG_ERROR("Malloc failed");
         rc = 0;
      }
   }

   dog_hb_mutex_unlock();

   return rc;
}

/** =====================================================================
* Function:
*     dog_hb_init
*
* Description:
*     Initialize the heartbeat watchdog driver
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
DLL_API_GLOBAL void dog_hb_init(void)
{
   unsigned int i;
   dog_hb_queue_p dog_hb_q_entry = NULL;

   dog_hb_mutex_init();

   dog_hb_mutex_lock();

   qurt_anysignal_init(&dog_hb_internal.anysignal);

   dog_hb_internal.dog_hb_q = (q_type *)malloc(sizeof(q_type));
   
   if(NULL != dog_hb_internal.dog_hb_q)
   {
      /* Initialize the queue */
      secure_memset((q_type *)dog_hb_internal.dog_hb_q, 0, sizeof(q_type));
      q_init(dog_hb_internal.dog_hb_q);

      for(i = 0; i < DOG_HB_LIST_CNT; i++)
      {
         dog_hb_q_entry = (dog_hb_queue_p)malloc(sizeof(dog_hb_queue_t));

         if(NULL != dog_hb_q_entry)
         {
            dog_hb_q_entry->list_num = i + 1;
            dog_hb_q_entry->dog_hb_total_q_clients = 0;
            dog_hb_q_entry->q_ping_head = NULL;

            q_put(dog_hb_internal.dog_hb_q, q_link(dog_hb_q_entry, &dog_hb_q_entry->link));
         }
      }
   }

   dog_hb_mutex_unlock();

   /* Register dog_hb itself as a client */
   dog_hb_internal.handle = dog_hb_register((char const*)DOG_HB_CLIENT_NAME_RESERVED, DOG_HB_SIG_QURTSIGNAL, &dog_hb_internal.anysignal, DOG_HB_DEVICE_PING_MASK);

   return;
}

/** =====================================================================
* Task :
*     dog_hb_task
*
* Description:
*     Initialize the heartbeat watchdog driver
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
DLL_API_GLOBAL void dog_hb_task( unsigned long int unused_param )
{
   unsigned int q_mask;

   /* Block for start signal */
   rcinit_handshake_startup();

   if(dog_hb_disable != TRUE)
   {
      if(rcevt_register_name_qurt(SYS_M_SHUTDOWN, &dog_hb_internal.anysignal, DOG_HB_SHUTDOWN_MASK)){;}

      /* Task forever loop */
      for (;;)
      {
         q_mask = qurt_anysignal_wait(&dog_hb_internal.anysignal, DOG_HB_SHUTDOWN_MASK | DOG_HB_DEVICE_PING_MASK | DOG_HB_DEVICE_MONITOR_MASK);

         if(q_mask & DOG_HB_SHUTDOWN_MASK)
         {
            /* Clear watchdog hb shutdown signal */
            qurt_anysignal_clear(&dog_hb_internal.anysignal, DOG_HB_SHUTDOWN_MASK);
            TMS_SHUTDOWN_MSG_HIGH("System shutdown signal received");

            dog_hb_disable = TRUE;

            if(rcevt_signal_name(SYS_M_SHUTDOWN_ACK)){;}
         }

         if(q_mask & DOG_HB_DEVICE_PING_MASK)
         {
            if(dog_hb_internal.handle > 0)
            {
               /* Self Pong if dog_hb itself has registered as a client */
               if(DOG_HB_STATUS_SUCCESS == dog_hb_pong(dog_hb_internal.handle))
               {
                  /* Clear watchdog hb self signal */
                  qurt_anysignal_clear(&dog_hb_internal.anysignal, DOG_HB_DEVICE_PING_MASK);
               }
            }
         }

         if(q_mask & DOG_HB_DEVICE_MONITOR_MASK)
         {
            /* Clear dog_hb signal */
            qurt_anysignal_clear(&dog_hb_internal.anysignal, DOG_HB_DEVICE_MONITOR_MASK);

            /* Ping Clients */
            if(DOG_HB_STATUS_SUCCESS == dog_hb_ping_clients())
            {
               /* Pong Check */
               if(DOG_HB_STATUS_SUCCESS != dog_hb_pong_check())
               {
                  TMS_MSG_ERROR("Pong check failed");
               }
            }
            else
            {
               TMS_MSG_ERROR("Ping clients failed");
            }
         }
      } /* for(;;) */
   } /* if(dog_hb_disable != TRUE) */
}

/** =====================================================================
* Function:
*     dog_hb_report
*
* Description:
*     API for clients to report to watchdog heartbeat.
*     Reports to the watchdog whether the monitored task is
*     functioning properly.
*
* Parameters:
*     ident : Dog handle used to tell the watchdog which task is reporting.
*
* Returns:
*     none
* =====================================================================  */
DLL_API_GLOBAL void dog_hb_report(dog_report_type ident)
{
   if(DOG_HB_STATUS_SUCCESS != dog_hb_pong(ident))
   {
      char task_name[QURT_MAX_NAME_LEN];
      qurt_thread_get_name(task_name, sizeof(task_name));
      
      TMS_MSG_SPRINTF_LOW_1("dog_hb_report failed by task %s", task_name);
   }
}

/** =====================================================================
* Function:
*     dog_hb_deregister
*
* Description:
*     Dynamically de-registers tasks for watchdog hb services.
*
* Parameters:
*     ident : Handle that the watchdog uses to identify the task.
*
* Returns:
*     none
* =====================================================================  */
DLL_API_GLOBAL void dog_hb_deregister(dog_report_type ident)
{
   dog_hb_device_p device_p_del = NULL, device_p_curr = NULL, device_p_prev = NULL;
   unsigned int device_found = 0, i;
   dog_hb_queue_p dog_hb_q_entry = NULL, dog_hb_q_entry_next = NULL;

   dog_hb_mutex_lock();

   device_p_del = dog_hb_handle2dog_hb_device(ident);

   dog_hb_q_entry = (dog_hb_queue_p)q_check(dog_hb_internal.dog_hb_q);

   /* Locate existing named device */
   for(i = 0; i < DOG_HB_LIST_CNT; i++)
   {
      device_p_curr = NULL;
      device_p_prev = NULL;

      if(NULL != dog_hb_q_entry)
      {
         for(device_p_curr = dog_hb_q_entry->q_ping_head; NULL != device_p_curr; device_p_curr = device_p_curr->next)
         {
            if(device_p_curr == device_p_del)
            {
               device_found = 1;

               if(NULL == device_p_prev)
               {
                  /* Delete the first node */
                  dog_hb_q_entry->q_ping_head = device_p_curr->next;
               }
               else
               {
                  device_p_prev->next = device_p_curr->next;
               }

               device_p_curr->next = NULL;

               /* Free the allocated device node */
               free(device_p_curr);

               dog_hb_q_entry->dog_hb_total_q_clients = dog_hb_q_entry->dog_hb_total_q_clients - 1;

               break; /* exit 1st for() loop */
            }

            device_p_prev = device_p_curr;
         }
      }

      if(device_found)
      {
         break;
      }

      dog_hb_q_entry_next = (dog_hb_queue_p)q_next(dog_hb_internal.dog_hb_q, &dog_hb_q_entry->link);
      dog_hb_q_entry = dog_hb_q_entry_next;
   }

   dog_hb_mutex_unlock();

   return;
}

/** =====================================================================
* Function:
*     dog_hb_event_signal
*
* Description:
*     Signals the watchdog for heartbeat monitoring
*
* Parameters:
*     None
*
* Returns:
*     None
* =====================================================================  */
void dog_hb_event_signal(void)
{
   qurt_anysignal_set(&dog_hb_internal.anysignal, DOG_HB_DEVICE_MONITOR_MASK); /* Signal DOG_HB_EVENT */
}

/** =====================================================================
* Function:
*     dog_hb_autokick
*
* Description:
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void dog_hb_autokick(boolean enable_hb_autokick)
{
   /* Tied to HW watchog so nothing to do */
   return;
}

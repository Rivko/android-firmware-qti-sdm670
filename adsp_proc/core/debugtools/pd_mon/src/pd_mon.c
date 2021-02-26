/** vi: tw=128 ts=3 sw=3 et
@file pd_mon.c
@brief This file contains the API details for the Protection Domain Monitor, API 1.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014,2017-2018,2020 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/pd_mon/src/pd_mon.c#4 $
$DateTime: 2020/06/03 07:46:12 $
$Change: 24167735 $
$Author: pwbldsvc $
===============================================================================*/

#include "pd_mon_qurt.h"
#include "pd_mon_internal.h"
#include "DDITimetick.h"  //DAV
#include "servreg_monitor.h"
#include "qurt_qdi_proxy.h"
#include "qurt_space.h"

#if defined(PD_MON_TRACER_SWEVT)
#include "tracer.h"
#include "pd_mon_tracer_swe.h"
#endif

#include "qurt_printf.h"
#include "tms_utils_msg.h"

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////
static void pd_mon_set_mgmt_tlskey(void* p);
void pd_mon_servreg_notif_sync_cb( SERVREG_CB_PARAM stat_p, SERVREG_CB_PARAM __attribute__((unused)) ignored );
PD_MON_HANDLE pd_mon_get_mgmt_tlskey(void);


/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

/**
INTERNAL, Program Domain Monitor (One Context Created Per Program Domain)
*/
static void PROGRAM_MONITOR(void* argv)
{
   PD_MON_STATUS_P stat_p = NULL;
   int asid;

  stat_p = pd_mon_internal.args_handle;                   // Pointer to Status Array (Turns into Opaque Handle)

      stat_p->client_h = 0;                                  // init client handle

      stat_p->restart = pd_mon_internal.args_restart;        // Initalize Current Status Array Structure

      stat_p->exit_status = 0;                                   // Status Return from Last Wait

      strlcpy(stat_p->name, argv, sizeof(stat_p->name));             // Copy PD Name For Reference

      pd_mon_set_mgmt_tlskey(stat_p); // indicate this is a TMS managed spawn

      // NOTE: THIS SIGNAL IS UNDER MUTEX PROTECTION. THE PROGRAM MONITOR
      // CONTEXT MUST RESPOND THAT IT HAS COMPLETED ITS UPDATE TO THE
      // MUTEX PROTECTED INFORMATION. THE CREATION AND INITIALIZATION
      // OF THE PROGRAM MONITOR CONTEXT IS CONSIDERED PROTECTED AS WELL.

      qurt_anysignal_set(&pd_mon_internal.signal, PD_MON_SIG_UNLOCK); // Signal Callee to Pick Up Return Value and Unlock Mutex

      // NOW THE PROGRAM MONITOR CONTEXT WILL HAVE TO USE THE MUTEX
      // DIRECTLY BEFORE ANY CHANGES ARE MADE TO THE MUTEX PROTECTED
      // DATA.

      stat_p->asid = asid = qurt_spawn(argv);                        // First Time Spawn of the Program

      stat_p->restart--;                                             // Restart Counter Decrements

      do                                                             // Loop for Restart
      {
         if (0 > asid)                                               // Wait for Child Change in Program Execution
         {
            TMS_MSG_ERROR("PD_MON FAILURE NO SPAWN");                // Log Failure of PD to Start (NEVER HOST FATAL)

            break;
         }

         // TMS_MSG_HIGH(WAITS FOR PROGRAM)

         // TMS_MSG_FATAL( "DAV:before qurt_wait");

         // ensure client_h was set in onspawn_cb
         if (0 > (stat_p->client_h))
         {
            ERR_FATAL("client handle not set by onspawn_cb",0,0,0);
         }

         // wait for process exit
         // Signal has been registered in onspawn_cb()
         // Signal may have already been set in the case of early termination, so do not clear before waiting.
         qurt_anysignal_wait( &(stat_p->evt_sig), PD_MON_EVT_EXIT );
         qurt_pimutex_lock(&pd_mon_internal.mutex);            // Mutex Lock

         qurt_printf("PD_MON:Kernel User PD exit complete\n");

         // process has exited -- clean up
         qurt_anysignal_clear( &(stat_p->evt_sig), PD_MON_EVT_EXIT );
         /*qurt_event_deregister does not exist*/

         // check if external service registry syncronization required
         if (NULL != (stat_p->servreg_h))
         {
            // register callback
            if (SERVREG_SUCCESS > servreg_register_sync_notif_cb(stat_p->servreg_h, 
                                                                  SERVREG_SERVICE_STATE_DOWN, 
                                                                  pd_mon_servreg_notif_sync_cb, 
                                                                  (SERVREG_CB_PARAM)stat_p, 
                                                                  (SERVREG_CB_PARAM)0))
            {
               ERR_FATAL("Could not register servreg sync notification", 0, 0, 0);
            }
            
            // ensure sync is pending before waiting
            if (0 < servreg_get_pending_acks(stat_p->servreg_h, SERVREG_SERVICE_STATE_DOWN))
            {
               qurt_pimutex_unlock(&pd_mon_internal.mutex);          // Mutex Unlock
               qurt_anysignal_wait( &(stat_p->evt_sig), PD_MON_EVT_SYNC );
               qurt_pimutex_lock(&pd_mon_internal.mutex);            // Mutex Lock

               qurt_printf("PD_MON:servreg_get_pending_acks received\n");
            }

            // sync complete -- internal clean up
            servreg_deregister_sync_notif_cb(stat_p->servreg_h, 
                                             SERVREG_SERVICE_STATE_DOWN, 
                                             pd_mon_servreg_notif_sync_cb, 
                                             (SERVREG_CB_PARAM)stat_p, 
                                             (SERVREG_CB_PARAM)0);
            qurt_anysignal_clear( &(stat_p->evt_sig), PD_MON_EVT_SYNC );
         }

         qurt_printf("PD_MON:Process Public Proxy\n");
         // sync complete -- external clean up
         if (0 != qurt_qdi_close(stat_p->public_proxy_h)) 
         {
            ERR_FATAL("could not complete public proxy clean up", 0, 0, 0);
         }

         qurt_pimutex_unlock(&pd_mon_internal.mutex);
         qurt_printf("PD_MON:Process Private Proxy\n");
         if (0 != qurt_qdi_close(stat_p->private_proxy_h)) 
         {
            ERR_FATAL("could not complete private proxy clean up", 0, 0, 0);
         }

         qurt_printf("PD_MON:UserPD exit complete\n");
         qurt_pimutex_lock(&pd_mon_internal.mutex);

         ///////////////////////////////////////////////////////////////
         // previous instance is R.I.P., free to restart based on policy
         ///////////////////////////////////////////////////////////////

         // TMS_MSG_FATAL( "DAV:after qurt_wait");

         if (0 < stat_p->restart)
         {
            /*handle for timetick*/
            // static DalDeviceHandle *hTimerHandle = NULL;
            // uint64 start, end;

            // if (DalTimetick_Attach("SystemTimer", &hTimerHandle)) {;}
            // if(hTimerHandle == NULL)
            // {
            // TMS_MSG_FATAL( "DAV:Failed to attach hTimerHandle");
            // }

            // TMS_MSG_HIGH(SPAWNS A PROGRAM)
            // qurt_timer_sleep(5000000); //sleep for 5 seconds (HLOS SPECIFIC HACK)

            // TMS_MSG_FATAL_1( "DAV:before qurt_spawn[%d]", stat_p->restart);
            // if (DalTimetick_GetTimetick64(hTimerHandle, &start)) {;}

            qurt_printf("Respawn User PD\n");
            asid = stat_p->asid = qurt_spawn(argv);                    // Child Id Return

            // if (DalTimetick_GetTimetick64(hTimerHandle, &end)) {;}
            // TMS_MSG_FATAL_2( "DAV:after qurt_spawn[%d], delta:%d ms", stat_p->restart, (end-start)/19200);

            stat_p->restart--;                                       // Restart Counter Decrements
         }

         qurt_pimutex_unlock(&pd_mon_internal.mutex);        // Callee Mutex unLock

      } while (1);                                                   // Keep Restarting Until Not Possible

      qurt_pimutex_lock(&pd_mon_internal.mutex);             // Callee Mutex Lock

      strlcpy(stat_p->name, "", sizeof(stat_p->name));               // PD Name is NULL

      stat_p->asid = PD_MON_STATUS_NO_ASID;                   // free status struct

      qurt_pimutex_unlock(&pd_mon_internal.mutex);           // Callee Mutex Unlock
}

/**
API, callback executed on EVERY qurt_spawn, regardless of caller
*/
void pd_mon_qurt_onspawn_cb(int client_handle, void __attribute__((unused)) *data_ptr)
{
   PD_MON_STATUS_P stat_p = (PD_MON_STATUS_P)pd_mon_get_mgmt_tlskey();

   if (NULL == stat_p)
   {
      TMS_MSG_LOW("Spawn by non-TMS caller, no proxy attached");
      return;  // NOT A TMS MANAGED PROCESS
   }

   if (0 > client_handle)
   {
      ERR_FATAL("client handle not set",0,0,0);
      return ;
   }

   stat_p->public_proxy_h = qurt_qdi_proxy_ref_create();          // Create public proxy

   stat_p->private_proxy_h = qurt_qdi_proxy_ref_create();         // Create private proxy

   if ((0 == stat_p->public_proxy_h) || (0 == stat_p->private_proxy_h))
   {
      ERR_FATAL("proxy creation failed while spawning new process",0,0,0);
   }

   // attach public proxy to process

   qurt_client_proxy_ref_install(client_handle, stat_p->public_proxy_h);
   stat_p->client_h = client_handle;

   //Legacy QURT requirement to indicate qurt_wait() will not be called ==> free associated memory to prevent memory leak.
   if(0 != qurt_qdi_client_detach(stat_p->client_h))
   {
     TMS_MSG_ERROR("client_handle did not refer to a detachable user client");
   }

   // register for process exit notification
   if (0 > qurt_event_register(QURT_PROCESS_EXIT, stat_p->client_h, 
                               &(stat_p->evt_sig), PD_MON_EVT_EXIT,
                               &(stat_p->exit_status), sizeof(stat_p->exit_status)))
   {
     ERR_FATAL("qurt_event_register failed", 0, 0, 0);
   }

   TMS_MSG_HIGH( "Spawn by TMS caller, proxy attached");
}

/**
API, Spawn Program Domain under Program Domain Monitor
*/
PD_MON_HANDLE pd_mon_spawn(PD_MON_RESTART* restart_p, PD_MON_NAME program, ...)
{
   int i;
   PD_MON_HANDLE rc;
   va_list ap;

   va_start(ap, program);                                            // Variable Arguments Setup

   qurt_pimutex_lock(&pd_mon_internal.mutex);                // Callee Mutex Lock

   pd_mon_internal.args_handle = (PD_MON_STATUS_P)PD_MON_ERROR;

   // Look for an unused status entry

   for (i = 0; i < sizeof(pd_mon_internal.status)/sizeof(PD_MON_STATUS_T); i++)
   {
      if (pd_mon_internal.status[i].asid == PD_MON_STATUS_NO_ASID)
      {
         pd_mon_internal.args_handle = &pd_mon_internal.status[i];
         pd_mon_internal.args_handle->asid = PD_MON_STATUS_TEMP_ASID;
         break;
      }
   }

   // Check if an unused status entry was located

   if (sizeof(pd_mon_internal.status)/sizeof(PD_MON_STATUS_T) > i)
   {
      char name[PD_MON_NAME_MAX];
      unsigned char* stack;
      qurt_thread_t tid;
      qurt_thread_attr_t attr;

      // Check if we can obtain stack resource for the Program Monitor Context

      if (NULL != (stack = malloc(PD_MON_STACK_SIZ)))                // Attempt to allocate stack for the Program Monitor Context
      {
         strlcpy(name, "%", sizeof(name));                           // Name the Program Monitor Context to reflect "%Program"
         strlcat(name, program, sizeof(name));

         qurt_thread_attr_init(&attr);                               // Init the Program Monitor Context

         qurt_thread_attr_set_name(&attr, name);
         qurt_thread_attr_set_stack_addr(&attr, stack);
         qurt_thread_attr_set_stack_size(&attr, PD_MON_STACK_SIZ);
         qurt_thread_attr_set_priority(&attr, qurt_thread_get_priority(qurt_thread_get_id()) - 1);

         if (NULL != restart_p)
         {
            pd_mon_internal.args_restart = *restart_p;       // Passing Restart Argument
         }

         else
         {
            pd_mon_internal.args_restart = PD_MON_RESTART_ALWAYS; // Setting Default
         }

         qurt_anysignal_clear(&pd_mon_internal.signal, PD_MON_SIG_UNLOCK);

         qurt_thread_create(&tid, &attr, PROGRAM_MONITOR, (void*)program); // Create Program Monitor Context

         // NOTE: THIS WAIT IS UNDER MUTEX PROTECTION. THE PROGRAM MONITOR
         // CONTEXT MUST RESPOND THAT IT HAS COMPLETED ITS UPDATE TO THE
         // MUTEX PROTECTED INFORMATION. THE CREATION AND INITIALIZATION
         // OF THE PROGRAM MONITOR CONTEXT IS CONSIDERED PROTECTED AS WELL.

         qurt_anysignal_wait(&pd_mon_internal.signal, PD_MON_SIG_UNLOCK); // Wait for the Program Monitor Context to Signal it has a Return Value
      }
   }

   rc = (PD_MON_HANDLE)pd_mon_internal.args_handle;                         // Return Value from Program Monitor Context

   qurt_pimutex_unlock(&pd_mon_internal.mutex);              // Callee Mutex Unlock

   va_end(ap);                                                       // Variable Arguments End

   return rc;
}

PD_MON_HANDLE pd_mon_spawn_restart(PD_MON_NAME domain, PD_MON_RESTART* restart_p)
{
   return pd_mon_spawn(restart_p, domain);
}

PD_MON_HANDLE pd_mon_spawn_oneshot(PD_MON_NAME domain)
{
   return (PD_MON_HANDLE)qurt_spawn(domain);
}

/**
API, Get Restart Counter of PD by Opaque Handle
*/
PD_MON_STATUS pd_mon_restart_get(PD_MON_HANDLE handle, PD_MON_RESTART* restart_p)
{
   qurt_pimutex_lock(&pd_mon_internal.mutex);             // Callee Mutex Lock

   *restart_p = ((PD_MON_STATUS_P)handle)->restart;

   qurt_pimutex_unlock(&pd_mon_internal.mutex);           // Callee Mutex Unlock

   return PD_MON_STATUS_SUCCESS;
}

/**
API, Set Restart Count of PD by Opaque Handle
*/
PD_MON_STATUS pd_mon_restart_set(PD_MON_HANDLE handle, PD_MON_RESTART* restart_p)
{
   PD_MON_RESTART tmp;

   qurt_pimutex_lock(&pd_mon_internal.mutex);             // Callee Mutex Lock

   tmp = ((PD_MON_STATUS_P)handle)->restart;

   ((PD_MON_STATUS_P)handle)->restart = *restart_p;

   *restart_p = tmp;

   qurt_pimutex_unlock(&pd_mon_internal.mutex);           // Callee Mutex Unlock

   return PD_MON_STATUS_SUCCESS;
}

/**
API, Initialization of service prior to use
@return
None.
*/
void pd_mon_host_init(void)
{
   int i;

   secure_memset(&pd_mon_internal, 0, sizeof(pd_mon_internal)); // Initialize Local Storage

   qurt_pimutex_init(&pd_mon_internal.mutex);                // Initalize Mutex

   qurt_anysignal_init(&pd_mon_internal.signal);             // Initalize Signal

   for (i = 0; i < sizeof(pd_mon_internal.status)/sizeof(PD_MON_STATUS_T); i++)
   {
      pd_mon_internal.status[i].asid = PD_MON_STATUS_NO_ASID;
      qurt_anysignal_init(&pd_mon_internal.status[i].evt_sig);             // Initalize Signal
   }
  
   // create TLS key for indicating a spawn thread is TLS managed
   if ( QURT_EOK != qurt_tls_create_key (&pd_mon_internal.tms_mgmt_tlskey, NULL))
   {
      ERR_FATAL("Could not create QURT tls key",0,0,0);
   } 

   // register callback called during initialization of EVERY qurt_spawn
   if ( 0 > qurt_callback_register_onspawn(pd_mon_qurt_onspawn_cb, NULL))
   {
      ERR_FATAL("Could not register onspawn callback with QURT",0,0,0);
   }
}

/**
Termination of service following use
@return
None.
*/
void pd_mon_host_term(void)
{
   /* NULL */ /* DECISION TO NOT CLEANUP SERVICE FOR POST MORTEM REASONS */
}

/**
INTERNAL, sets a TLS item indicating that this thread is managed
          by TMS, allows TMS qurt_onspawn callbacks to determine
          if the spawning process is TMS managed
*/
static void pd_mon_set_mgmt_tlskey(void* p)
{
   if ( QURT_EOK != qurt_tls_set_specific(pd_mon_internal.tms_mgmt_tlskey, p))
   {
      ERR_FATAL("Could not set QURT tls key",0,0,0);
   } 
}

/**
INTERNAL, get TLS item indicating if this thread is managed
          by TMS, allows TMS qurt_onspawn callbacks to determine
          if the spawning process is TMS managed
*/
PD_MON_HANDLE pd_mon_get_mgmt_tlskey(void)
{
   return ((PD_MON_HANDLE)(qurt_tls_get_specific(pd_mon_internal.tms_mgmt_tlskey)));
}

/**
INTERNAL, attach a QDI handle to the private proxy internally 
          associated to a client ASID 
*/
int pd_mon_private_proxy_attach(int client_asid, qurt_qdi_obj_t *obj_ptr)
{
   int i, ret = -1;
   PD_MON_STATUS_P stat_p = NULL;
   int proxy_handle;

   qurt_pimutex_lock(&pd_mon_internal.mutex);          

   for (i = 0; i < sizeof(pd_mon_internal.status)/sizeof(PD_MON_STATUS_T); i++)
   {
      if (pd_mon_internal.status[i].asid == client_asid)
      {
         stat_p = &pd_mon_internal.status[i];

         proxy_handle = stat_p->private_proxy_h;

         TMS_MSG_HIGH_1( "pd_mon_private_proxy_attach, private_proxy = 0x%x", proxy_handle);

         ret = qurt_qdi_proxy_ref_add_by_object(proxy_handle, obj_ptr);
      }
   }

   qurt_pimutex_unlock(&pd_mon_internal.mutex);
   return ret;
}

/**
INTERNAL, associate a servreg handle with a given process 
          for proxy closure purposes 
*/
PD_MON_STATUS pd_mon_set_servreg_handle(int client_asid, SERVREG_NAME servreg_n)
{
   int i;

   qurt_pimutex_lock(&pd_mon_internal.mutex);          

   for (i = 0; i < sizeof(pd_mon_internal.status)/sizeof(PD_MON_STATUS_T); i++)
   {
      if (pd_mon_internal.status[i].asid == client_asid)
      {
         pd_mon_internal.status[i].servreg_h = servreg_alloc_monitor_handle(servreg_n, SERVREG_NULL);
         if(SERVREG_NULL == pd_mon_internal.status[i].servreg_h)
         {
            ERR_FATAL("Servreg pd handle is null", 0, 0, 0);
         }
         qurt_pimutex_unlock(&pd_mon_internal.mutex);  
         return PD_MON_STATUS_SUCCESS;        
      }
   }

   qurt_pimutex_unlock(&pd_mon_internal.mutex);  
   return PD_MON_STATUS_ERROR;
}

void pd_mon_servreg_notif_sync_cb( SERVREG_CB_PARAM stat_p, SERVREG_CB_PARAM __attribute__((unused)) ignored )
{
   // ensure we are pointing into status array
   if ( (pd_mon_internal.status > (PD_MON_STATUS_P)stat_p)||((pd_mon_internal.status + sizeof(pd_mon_internal.status))<=(PD_MON_STATUS_P)stat_p) )
   {
      ERR_FATAL("Invalid parameter %x to notif_sync_cb", stat_p, 0, 0);
   }
   else
   {
      qurt_anysignal_set( &(((PD_MON_STATUS_P)stat_p)->evt_sig), PD_MON_EVT_SYNC );
   }

}

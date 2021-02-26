/*==========================================================================
Description
  Target-specific diagnostic system routines.  These were moved to a separate
  file to avoid coupling with MC and other IS95 DMSS components.

Copyright (c) 2001-2011, 2013-14 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            Edit History

$Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagmm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/14   ph      Added start signal to be used for handshake with osal
                   in the new task created using osal_create_thread().
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
11/28/13   ph      Fixed the bug where diag_sleep_state won't be updated when diag
                   is switched to NRT mode.
09/27/13   xy      Removed code under FEATURE_WINCE and removed references to
                   TMC code in B family and later PLs
09/28/11   is      Support dynamic sleep voting
06/16/11   is      Support transition of initialization and tasks from CoreImg
                   into RCINIT framework
09/17/10   vs      set rpt timer based on run-time params
05/11/10   is      Replaced external feature FEATURE_IS2000 with internal
                   feature DIAG_IS2000
05/10/10   is      Mapped external features for call manager functionality with
                   internal feature DIAG_CM_AVAIL.
04/29/10   is      Removed unused function mc_set_digital_offline()
03/18/10   JV      Replaced deprecated APIs
12/23/09   vg      removed call to srch_rude_wakeup( FALSE );
09/01/09   JV      Fixed compile error.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
07/31/09   JV      Brought back FEATURE_DIAG_NV
07/31/09   JV      Merged Q6 diag code back to mainline
07/10/09   as      Mainlined API for FEATURE_OTASP and FEATURE_UIM_RUIM
06/23/09   JV      Introduced OS abstraction layer
05/07/09   vk      changed includes from external to internal headers
03/02/09   is      Fix featurizations to keep modem/apps procs in sync in using
                   new vs old sleep API
02/03/09   is      Fix featurization for WM builds related to diag_sleep_okts_handle
01/19/09   is      Switch to new sleep voting interface
12/15/06   as      Fixed critical lint errors.
10/31/05   as      Fixed lint errors.
02/23/05   as      Included srchmc.h to prevent compiler warnings
01/07/03   djm     add mc set digital offline function for WPLT use, mode change
01/08/03   ma      featurization for GSM PLT
12/08/02   djm     corrected WPLT featurization issue
11/20/01   as      Created file.

===========================================================================*/

#include "comdef.h"

#include "customer.h"
#include "diagtarget.h"

#include "diag_v.h"
#include "diagi_v.h"
#include "log.h"
#include "msg.h"
#include "osal.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "sleep.h"
#include "assert.h"
#endif

#ifdef DIAG_CM_AVAIL
#include "cm.h"
#endif

#if !defined (DIAG_QDSP6_APPS_PROC)
#include "dog.h"
#include "tmc.h"
#endif

#if defined (FEATURE_DIAG_NV)
#include "diagnv_v.h"
#endif

#if defined(FEATURE_RCINIT)
#include "rcevt.h"
#include "rcinit.h"
#endif

#if defined (DIAG_DYNAMIC_SLEEP)
#include "npa.h" /* For npa_client_handle, etc */
#endif

#ifndef SLEEP_DIAG_OKTS_SIG
sleep_okts_handle diag_sleep_okts_handle;
#endif

#if defined (DIAG_DYNAMIC_SLEEP)
npa_client_handle diag_npa_handle = NULL;
npa_client_handle diagtest_npa_handle = NULL;
#endif

extern osal_tcb_t  diag_tcb;

#if defined(DIAG_DOG_SUPPORT)
extern uint32 diag_dog_rpt_time_var;
extern dog_report_type diag_dog_rpt_var;
#endif
#ifndef SLEEP_DIAG_OKTS_SIG
extern diag_sleep_state_enum_type diag_sleep_state; /*from diag.c */
#endif
/*===========================================================================

FUNCTION DIAG_OFFLINE

DESCRIPTION
  This function performs processing required when the diag task receives
  a signal to go offline.  This processing consists of clearing the signal
  DIAG_OFFLINE_COMPLETE_SIG and acknowledging mc's offline signal.

===========================================================================*/
void diag_offline (void)
{
  MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Got offline state change");

  /* Notify the task manager that DIAG has gone offline.  DIAG doesn't
     actually do anything differnt, but it must ACK the offline
     command nonetheless. */

} /* diag_offline */



/*===========================================================================

FUNCTION DIAG_READY_START_TASK

DESCRIPTION
  This procedure is called by diag when it is ready to start its task.
  Diag will wait on DIAG_TASK_START_SIG (0x8000) after this call.

===========================================================================*/
void diag_ready_start_task (void)
{
  MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"DIAG task is started");

#if defined (DIAG_QDSP6_APPS_PROC)
  int return_val = 0;
  return_val=osal_handshake_start(&diag_tcb);
  ASSERT(OSAL_SUCCESS == return_val);

#else

  #if defined(FEATURE_RCINIT)
    /* Handshake allows startup to progress */
    rcinit_handshake_startup();

  #endif

#endif
} /* diag_ready_start_task */



/*===========================================================================

FUNCTION DIAG_SHUTDOWN

DESCRIPTION
  This procedure performs shutdown processing for the Diagnostic task.

  After this procedure is called the Diagnostic task will not be running.

===========================================================================*/
void diag_shutdown (void)
{
   osal_sigs_t return_sigs;
   int return_val = 0;
   MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Diag shutdown state change");

   return_val = osal_reset_sigs(&diag_tcb, DIAG_TASK_STOP_SIG, &return_sigs);
   ASSERT(OSAL_SUCCESS == return_val);

} /* diag_shutdown */



/*===========================================================================

FUNCTION DIAG_POWERDOWN

DESCRIPTION
  This procedure sends the command to MC to power down the phone.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The phone must powerdown as a result of this call.

===========================================================================*/
void diag_powerdown (void)
{
#ifdef DIAG_CM_AVAIL
    (void) cm_ph_cmd_oprt_mode(
                        NULL,
                        NULL,
                        CM_CLIENT_ID_ANONYMOUS,         /* Dummy client ID */
                        SYS_OPRT_MODE_PWROFF
                       );
#endif /* DIAG_CM_AVAIL */
} /* diag_powerdown */


/*===========================================================================

FUNCTION DIAG_KICK_WATCHDOG

DESCRIPTION
  This procedure reports to the watchdog and resets the timer.

===========================================================================*/
void diag_kick_watchdog(void)
{
#if defined(DIAG_DOG_SUPPORT)
  int return_val = 0;
  dog_report(diag_dog_rpt_var);
  return_val = osal_set_timer(&diag_rpt_timer, diag_dog_rpt_time_var);
  ASSERT(OSAL_SUCCESS == return_val);
#endif
  return;
}


/*===========================================================================

FUNCTION DIAG_GET_CPU_CLIENT_HANDLE_CB

DESCRIPTION
  This callback is called when sleep task is ready for NPA handles to be
  created.

===========================================================================*/
#if defined (DIAG_DYNAMIC_SLEEP)
static void diag_get_cpu_client_handle_cb( void *context,
                                           unsigned int event_type,
                                           void *data,
                                           unsigned int data_size )
{
  diag_npa_handle = npa_create_sync_client( "/core/cpu/vdd",
                                            "DIAG_NPA_CLIENT",
                                            NPA_CLIENT_REQUIRED );

  diagtest_npa_handle = npa_create_sync_client( "/core/cpu/vdd",
                                                "DIAGTEST_NPA_CLIENT",
                                                NPA_CLIENT_REQUIRED );

} /* diag_get_cpu_client_handle_cb */
#endif


/*===========================================================================

FUNCTION DIAG_SLEEP_INIT

DESCRIPTION
  This procedure initializes variables for Diag sleep voting.

===========================================================================*/
void diag_sleep_init( void )
{
#if defined (DIAG_DYNAMIC_SLEEP)

  /* Registering a callback function to the resource to obtain its client
     and query handles. The CB will be called immediately if the
     NPA node already exists, so it's safe to just register the CB here. */
  npa_resource_available_cb( "/core/cpu/vdd",
                             diag_get_cpu_client_handle_cb,
                             NULL );

#elif !defined (SLEEP_DIAG_OKTS_SIG)
  diag_sleep_okts_handle = sleep_register( "DIAG", TRUE );
#else
  return;
#endif

} /* diag_sleep_init */


/*===========================================================================

FUNCTION DIAG_ALLOW_SLEEP

DESCRIPTION
  This procedure votes for sleep for the Diag task.

===========================================================================*/
void diag_allow_sleep (void)
{
#ifndef SLEEP_DIAG_OKTS_SIG
diag_sleep_state= DIAG_SLEEP_STATE_ASLEEP;
#endif

#if defined (DIAG_DYNAMIC_SLEEP)
  if( diag_npa_handle )
  {
    npa_cancel_request( diag_npa_handle );
  }
#elif defined  (DIAG_QDSP6_APPS_PROC)
  return;
#else
  #if (defined SLEEP_DIAG_OKTS_SIG )
    osal_sigs_t return_sigs;
    (void)osal_set_sigs(&sleep_tcb, SLEEP_DIAG_OKTS_SIG, &return_sigs);
  #elif !defined (SLEEP_DIAG_OKTS_SIG)
    sleep_assert_okts( diag_sleep_okts_handle );
  #endif
#endif

} /* diag_allow_sleep */


/*===========================================================================

FUNCTION DIAG_FORBID_SLEEP

DESCRIPTION
  This procedure votes against sleep for the Diag task.

===========================================================================*/
void diag_forbid_sleep(void)
{
#ifndef SLEEP_DIAG_OKTS_SIG
diag_sleep_state = DIAG_SLEEP_STATE_AWAKE;
#endif

#if defined (DIAG_DYNAMIC_SLEEP)
  if( diag_npa_handle )
      npa_issue_required_request( diag_npa_handle, 1);
#elif defined  (DIAG_QDSP6_APPS_PROC)
  return;
#else
  #if (defined SLEEP_DIAG_OKTS_SIG )
    osal_sigs_t return_sigs;
    int return_val = 0;
    (void)osal_reset_sigs(&sleep_tcb, SLEEP_DIAG_OKTS_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
  #elif !defined (SLEEP_DIAG_OKTS_SIG)
    sleep_negate_okts( diag_sleep_okts_handle );
  #endif
#endif

} /* diag_forbid_sleep */

/*lint +esym(715,pkt_len,req_ptr) */



/*===========================================================================

FUNCTION DIAG_OTASP_UPDATE

DESCRIPTION
  This procedure tells DIAG to refresh its status cache from NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void diag_otasp_update( void )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  return_val = osal_set_sigs(&diag_tcb, DIAG_REFRESH_CACHE_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
} /* diag_otasp_update */


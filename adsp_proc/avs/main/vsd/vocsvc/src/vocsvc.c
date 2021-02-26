/*===========================================================================

                         V O I C E   S E R V I C E S

DESCRIPTION
  This source file defines APIs for initializing all voice related modules.

REFERENCES
  None.

Copyright (c) 2008 - 2014, 2016-2017 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/avs.adsp/2.8.5/vsd/vocsvc/src/vocsvc.c#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/10   act     Adding ftm_audio_init().
07/25/08   ymc     Initial revision.

===========================================================================*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "msg.h"

#include "apr_errcodes.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "apr_misc.h"

#include "cvp_api.h"
#include "cvs_api.h"
#include "mvm_api.h"
#include "cvd_vfr_api.h"
#include "cvd_task.h"
#include "cvd_diag_api.h"
#include "vocsvc.h"
#if defined(AVS_MPSS_TEST)
#include "voiceinit.h"
#endif 
/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef enum vocsvc_thread_state_enum_t
{
  VOCSVC_THREAD_STATE_ENUM_INIT,
  VOCSVC_THREAD_STATE_ENUM_READY,
  VOCSVC_THREAD_STATE_ENUM_EXIT
}
  vocsvc_thread_state_enum_t;

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

static apr_event_t vocsvc_event_handle;

#ifdef USE_SINGLE_AUDSVC_THREAD
static apr_thread_t vocsvc_task_handle;
static vocsvc_thread_state_enum_t vocsvc_task_state = VOCSVC_THREAD_STATE_ENUM_INIT;
static uint8_t vocsvc_task_stack[ VOCSVC_TASK_STACK_SIZE ];
#endif

/****************************************************************************
 * VOICE SERVICES TASK                                                      *
 ****************************************************************************/

APR_INTERNAL void vocsvc_signal_task_adsp ( void )
{
  ( void ) apr_event_signal( vocsvc_event_handle );
}

#ifdef USE_SINGLE_AUDSVC_THREAD
static int32_t vocsvc_service_task ( void* param )
{
  int32_t rc;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Entering voice services task" );

  rc = apr_event_create( &vocsvc_event_handle );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Exiting voice services task due to failure" );
    return APR_EFAILED;
  }

  vocsvc_task_state = VOCSVC_THREAD_STATE_ENUM_READY;

  do
  {
    rc = apr_event_wait( vocsvc_event_handle );

    /* Run services. */
  }
  while ( rc == APR_EOK );

  ( void ) apr_event_destroy( vocsvc_event_handle );
  vocsvc_task_state = VOCSVC_THREAD_STATE_ENUM_EXIT;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Exiting voice services task" );

  return APR_EOK;
}
#endif

/****************************************************************************
 * VOICE SERVICES INITIALIZATION ROUTINES                                   *
 ****************************************************************************/

static int32_t vocsvc_init ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== vocsvc_init()======" );

#ifdef USE_SINGLE_AUDSVC_THREAD
  { /* Bring up the service task. */
    ( void ) apr_thread_create(
               &vocsvc_task_handle, VOCSVC_SERVICE_TASK_NAME,
               VOCSVC_SERVICE_TASK_PRIORITY, 
               vocsvc_task_stack, VOCSVC_TASK_STACK_SIZE, vocsvc_service_task,
               NULL );

    while ( vocsvc_task_state != VOCSVC_THREAD_STATE_ENUM_READY )
    {
      ( void ) apr_misc_sleep( 1000000 ); /* Sleep for 1ms. */
    }
  }
#endif

  { 
    /* Initialize the drivers and services. */
    ( void ) mvm_call( DRV_CMDID_INIT, NULL, 0 );
    ( void ) cvd_vfr_call( DRV_CMDID_INIT, NULL, 0 );
    ( void ) cvd_diag_call( DRV_CMDID_INIT, NULL, 0 );
    
  }

  return APR_EOK;
}

static int32_t vocsvc_postinit ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== vocsvc_postinit()======" );

  { 
     /* Post-initialize the drivers and services. */
    ( void ) mvm_call( DRV_CMDID_POSTINIT, NULL, 0 );
    ( void ) cvd_vfr_call( DRV_CMDID_POSTINIT, NULL, 0 );
  }

  return APR_EOK;
}

static int32_t vocsvc_predeinit ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== vocsvc_predeinit()======" );

  { /* Pre-deinitialize drivers and services (in reverse order of initialization). */
    ( void ) cvd_vfr_call( DRV_CMDID_PREDEINIT, NULL, 0 );
    ( void ) mvm_call( DRV_CMDID_PREDEINIT, NULL, 0 );
  }

  return APR_EOK;
}

static int32_t vocsvc_deinit ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== vocsvc_deinit()======" );

  { /* De-initialize drivers and services (in reverse order of initialization). */
    ( void ) cvd_vfr_call( DRV_CMDID_DEINIT, NULL, 0 );
    ( void ) mvm_call( DRV_CMDID_DEINIT, NULL, 0 );
    ( void ) cvd_diag_call( DRV_CMDID_DEINIT, NULL, 0 );
  }

#ifdef USE_SINGLE_AUDSVC_THREAD
  { /* Trigger the service task to shut down. */
    ( void ) apr_event_signal_abortall( vocsvc_event_handle );

    while ( vocsvc_task_state != VOCSVC_THREAD_STATE_ENUM_EXIT )
    {
      ( void ) apr_misc_sleep( 1000000 ); /* Sleep for 1ms. */
    }

    ( void ) apr_thread_destroy( vocsvc_task_handle );
  }
#endif

  return APR_EOK;
}

/*===========================================================================

FUNCTION vocsvc_call_adsp

DESCRIPTION
  This procedure provides a single entry-point into the voice services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
APR_EXTERNAL int32_t vocsvc_call_adsp (
  vocsvc_callindex_enum_t index,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( index )
  {
  case VOCSVC_CALLINDEX_ENUM_INIT:
    rc = vocsvc_init( );
#if defined ( AVS_MPSS_TEST )
    // Initialize VS
    ( void ) voiceinit_call( DRV_CMDID_INIT, NULL, 0 );
#endif
    break;

  case VOCSVC_CALLINDEX_ENUM_POSTINIT:
    rc = vocsvc_postinit( );
#if defined ( AVS_MPSS_TEST )
    // Initialize VS
    ( void ) voiceinit_call( DRV_CMDID_POSTINIT, NULL, 0 );
#endif
    break;

  case VOCSVC_CALLINDEX_ENUM_PREDEINIT:
    rc = vocsvc_predeinit( );
#if defined(AVS_MPSS_TEST)
    ( void ) voiceinit_call( DRV_CMDID_PREDEINIT, NULL, 0 );
#endif
    break;

  case VOCSVC_CALLINDEX_ENUM_DEINIT:
    rc = vocsvc_deinit( );
#if defined(AVS_MPSS_TEST)
    ( void ) voiceinit_call( DRV_CMDID_DEINIT, NULL, 0 );
#endif
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Unsupported callindex (%d)",
                                            index );
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}


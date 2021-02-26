/*
 Copyright (c) 2014 - 2015, 2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_vfr/src/cvd_vfr_module.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include <stddef.h>
#include "msg.h"
#include "err.h"
#include "mmstd.h"

#include "Elite.h"
#include "qurt_elite.h"

#include "apr_errcodes.h"
#include "apr_lock.h"

#include "vss_private_if.h"
#include "VoiceTimerMsgs.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define CVD_VFR_VTM_EXPIRY_MASK ( 0x01 )
#define CVD_VFR_VTM_STOP_MASK ( 0x02 )

#define CVD_VFR_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

#define CVD_VFR_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL,\
                       "CVD_VFR: Error[0x%08x]", rc ); } }

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/


/* Lock Management */
static apr_lock_t cvd_vfr_int_lock;

static uint32_t cvd_vfr_client_count;
static Vtm_SubUnsubMsg_t cvd_vfr_vtm_sub_unsub_msg;
static qurt_elite_signal_t cvd_vfr_expiry_signal;
static qurt_elite_signal_t cvd_vfr_stop_signal;
static qurt_elite_channel_t cvd_vfr_elite_channel;


/****************************************************************************
 * INTERNAL ROUTINES                                                    *
 ****************************************************************************/

static int32_t cvd_vfr_init ( void )
{
  uint32_t rc;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== cvd_vfr_init()======" );

  cvd_vfr_client_count = 0;

  rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &cvd_vfr_int_lock );
  CVD_VFR_PANIC_ON_ERROR(rc);

  qurt_elite_channel_init( &cvd_vfr_elite_channel );
  ( void ) qurt_elite_signal_init( &cvd_vfr_expiry_signal );
  ( void ) qurt_elite_signal_init( &cvd_vfr_stop_signal );
  
  ( void ) qurt_elite_channel_add_signal(
             &cvd_vfr_elite_channel, &cvd_vfr_expiry_signal, CVD_VFR_VTM_EXPIRY_MASK );
  ( void ) qurt_elite_channel_add_signal(
             &cvd_vfr_elite_channel, &cvd_vfr_stop_signal, CVD_VFR_VTM_STOP_MASK );

  return APR_EOK;
}

static int32_t cvd_vfr_post_init ( void )
{
  return APR_EOK;
}

static int32_t cvd_vfr_pre_deinit ( void )
{
  return APR_EOK;
}

static int32_t cvd_vfr_deinit ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== cvd_vfr_deinit()======" );

  ( void ) apr_lock_destroy( cvd_vfr_int_lock );

  return APR_EOK;
}


/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/

/**
 * NOTE: Currently supports single VTM registration for VFR_NONE, This might need to be 
 * enhanced if needed in future.
 */
APR_EXTERNAL uint32_t cvd_vfr_subscribe_to_vtm ( 
  uint32_t vfr_cycle_duration_us
)
{
  uint32_t rc = APR_EOK;
  uint32_t size;
  elite_msg_any_t msg;
  EliteMsg_CustomVtSubUnsubHeaderType* payload = NULL;

  ( void ) apr_lock_enter( cvd_vfr_int_lock );

  for( ;; )
  {
    if ( cvd_vfr_client_count > 0 ) 
    {
      cvd_vfr_client_count++;
      break;
    }

    if ( qurt_elite_globalstate.pVoiceTimerCmdQ == NULL )
    {
      rc = APR_EFAILED;
      CVD_VFR_REPORT_FATAL_ON_ERROR(rc);
      break;
    }

    size = sizeof( EliteMsg_CustomVtSubUnsubHeaderType );

    rc = elite_msg_create_msg( &msg, &size, ELITE_CUSTOM_MSG, NULL, 0, APR_EOK );
    if ( rc )
    {
      CVD_VFR_REPORT_FATAL_ON_ERROR(rc);
      break;
    }

    cvd_vfr_vtm_sub_unsub_msg.signal_ptr = &cvd_vfr_expiry_signal;
    cvd_vfr_vtm_sub_unsub_msg.signal_end_ptr = &cvd_vfr_stop_signal;
    cvd_vfr_vtm_sub_unsub_msg.resync_signal_ptr = NULL;
    cvd_vfr_vtm_sub_unsub_msg.offset = 0; /* To receive VFR signal right on when VFR fires. */
    cvd_vfr_vtm_sub_unsub_msg.signal_enable = 1;
    cvd_vfr_vtm_sub_unsub_msg.vfr_mode = VSS_ICOMMON_VFR_MODE_SOFT;
    cvd_vfr_vtm_sub_unsub_msg.vfr_cycle = vfr_cycle_duration_us;
    cvd_vfr_vtm_sub_unsub_msg.client_id = VOICE_DRIVER;
    cvd_vfr_vtm_sub_unsub_msg.vsid = 0;
    cvd_vfr_vtm_sub_unsub_msg.avtimer_timestamp_us = 0;
    cvd_vfr_vtm_sub_unsub_msg.timing_ver = 0; /* Ignored for SOFT VFR, shall set to 0 */
 
    payload = ( ( EliteMsg_CustomVtSubUnsubHeaderType* ) msg.pPayload );
    payload->unSecOpCode = VOICE_TIMER_SUBSCRIBE;
    payload->vtm_sub_unsub_payload_ptr = &cvd_vfr_vtm_sub_unsub_msg;

    ( void ) qurt_elite_signal_clear( &cvd_vfr_expiry_signal );
    rc = qurt_elite_queue_push_back( qurt_elite_globalstate.pVoiceTimerCmdQ, ( uint64_t* )&msg );
    if ( APR_EOK != rc )
    {
      ( void ) elite_msg_return_payload_buffer( &msg );
      CVD_VFR_REPORT_FATAL_ON_ERROR( rc );
      rc = APR_EFAILED;
      break;
    }

    ( void ) qurt_elite_channel_wait( &cvd_vfr_elite_channel, CVD_VFR_VTM_EXPIRY_MASK );

    qurt_elite_signal_clear( &cvd_vfr_expiry_signal );
    cvd_vfr_client_count++;

    break;
  }

  ( void ) apr_lock_leave( cvd_vfr_int_lock );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "CVD_VFR: SUBSCRIBE_VTM: cvd_vfr_client_count=%d", cvd_vfr_client_count );

  return rc;
}

APR_EXTERNAL uint32_t cvd_vfr_unsubscribe_to_vtm( 
  void
)
{
  uint32_t rc = APR_EOK;
  uint32_t size;
  elite_msg_any_t msg;
  EliteMsg_CustomVtSubUnsubHeaderType* payload = NULL;

  ( void ) apr_lock_enter( cvd_vfr_int_lock );

  for( ;; )
  {
    if ( cvd_vfr_client_count > 1 )
    {
      cvd_vfr_client_count--;
      break;
    }

    if( 0 == cvd_vfr_client_count ) break;

    if ( qurt_elite_globalstate.pVoiceTimerCmdQ == NULL )
    {
      rc = APR_EFAILED;
      CVD_VFR_REPORT_FATAL_ON_ERROR(rc);
      break;
    }
   
    size = sizeof( EliteMsg_CustomVtSubUnsubHeaderType );
    
    rc = elite_msg_create_msg( &msg, &size, ELITE_CUSTOM_MSG, NULL, 0, APR_EOK );
    if ( rc )
    {
      CVD_VFR_REPORT_FATAL_ON_ERROR(rc);
      break;
    }

    payload = ( ( EliteMsg_CustomVtSubUnsubHeaderType* ) msg.pPayload );
    payload->unSecOpCode = VOICE_TIMER_UNSUBSCRIBE;
    payload->vtm_sub_unsub_payload_ptr = &cvd_vfr_vtm_sub_unsub_msg;

    rc = qurt_elite_queue_push_back( qurt_elite_globalstate.pVoiceTimerCmdQ, ( uint64_t* )&msg );
    if ( APR_EOK != rc )
    {
      ( void ) elite_msg_return_payload_buffer( &msg );
      CVD_VFR_REPORT_FATAL_ON_ERROR( rc );
      rc = APR_EFAILED;
      break;
    }

    ( void ) qurt_elite_channel_wait( &cvd_vfr_elite_channel, CVD_VFR_VTM_STOP_MASK );
    ( void ) qurt_elite_signal_clear( &cvd_vfr_stop_signal );
    cvd_vfr_client_count--;

    break;
  }

  ( void ) apr_lock_leave( cvd_vfr_int_lock );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "CVD_VFR: UNSUBSCRIBE_VTM: cvd_vfr_client_count=%d", cvd_vfr_client_count );

  return rc;
}

APR_EXTERNAL uint32_t cvd_vfr_get_voice_frame_reference( 
  uint64_t *vfrTimestamp
)
{
  *vfrTimestamp = cvd_vfr_vtm_sub_unsub_msg.avtimer_timestamp_us;
  return APR_EOK;
}

APR_EXTERNAL int32_t cvd_vfr_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( cmd_id )
  {
  case DRV_CMDID_INIT:
    rc = cvd_vfr_init( );
    break;

  case DRV_CMDID_POSTINIT:
    rc = cvd_vfr_post_init( );
    break;

  case DRV_CMDID_PREDEINIT:
    rc = cvd_vfr_pre_deinit( );
    break;

  case DRV_CMDID_DEINIT:
    rc = cvd_vfr_deinit( );
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Unsupported cmd_id (0x%08X)", cmd_id );
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}


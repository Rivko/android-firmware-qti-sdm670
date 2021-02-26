/*
  Copyright (c) 2013-2017 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/mvm/src/mvm_ccm.c#2 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include <stddef.h>
#include "msg.h"
#include "err.h"

#ifndef WINSIM
  #include "mmpm.h"
  #include "npa.h"
  #include "ClockDefs.h"
#endif /* !WINSIM */

#include "mmstd.h"

#include "apr_list.h"
#include "apr_lock.h"
#include "apr_memmgr.h"
#include "aprv2_api_inline.h"
#include "apr_timer.h"

#include "adsp_vpm_api.h"

#include "vss_public_if.h"
#include "vss_private_if.h"

#include "mvm_ccm_api_i.h"
#include "vocsvc_avtimer_api.h"
/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define MVM_CCM_HEAP_SIZE_V ( 500 )

#define MVM_CCM_MAX_ACTIVE_MVM_SESSIONS_V ( 5 )
  /**< Maximum number of MVM sessions that can be active concurrently.
       TODO: This number is tentative. It is based on the maximum number of
       MVM full control sessions that can be created. Do we want to limit
       the number of active voice sessions in CVD or do we rely on HLOS to
       reject un-supported concurrency use cases? */

#define MVM_CCM_NUM_RSCS_PER_MMPM_REQUEST_V ( 3 )
  /**< Number of resources to request/release for each MMPM request/release.
       Currently in each MMPM request/release, MVM-CCM requests/releases 3
       resources: MIPS, Bus bandwidth and sleep latency. */

#define MVM_CCM_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

#define MVM_CCM_DEFAULT_MPPS_EXTERNAL_SCALE_FACTOR ( 20 )    /**< actual value = 2.0 */
#define MVM_CCM_DEFAULT_MPPS_INTERNAL_SCALE_FACTOR ( 10 )    /**< actual value = 1.0 */
#define MVM_CCM_DEFAULT_BW_SCALE_FACTOR   ( 10 )    /**< actual value = 1.0 */
#define MVM_CCM_SCALE_FACTOR_DIV        ( 10.0 )
#define MVM_CCM_MAX_NUM_CLK_PERF_LEVELS ( 15 )
#define MVM_CCM_PROCESSING_TIME_THRESHOLD ( 18000 )

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/
typedef struct mvm_ccm_clk_control_config_t
{
  uint32_t minor_version;
  uint32_t size;
  uint32_t num_columns;
  void* columns;
}
  mvm_ccm_clk_control_config_t;

typedef struct mvm_csm_config_t
{
   bool_t enable;
   bool_t client_info[ MVM_CSM_CLIENT_ENUM_MAX ];
   bool_t is_tx_active;
   bool_t is_rx_active;
   bool_t is_active;
   uint32_t enc_pkt_count;
   uint64_t tx_wakeup;
   uint64_t rx_wakeup;
   uint32_t vptx_start_offset;
   uint32_t vdec_req_offset;
   uint64_t first_vptx_start;
   uint64_t last_vptx_start;
   uint32_t vptx_frame_size;
   uint32_t vfr_cycle_us;
   uint32_t num_venc_instances;
   uint32_t samples_at_vptx_start;
   mvm_voice_timing_t voice_timing; 
   mvm_voice_processing_delays_t processing_times;
   uint32_t threshold_us;
   uint32_t safety_margin_us;
} mvm_csm_config_t;

typedef struct mvm_ccm_system_config_t
{
  uint16_t num_voice_sessions;
    /**< Number of voice sessions active in the system.
         Note that we could use mvm_ccm_active_mvm_session_tracking_used_q.size
         to keep track of the number of active voice sessions. However, having
         num_voice_sessions together with the rest of the system configurations
         makes the code clean. */
  uint16_t num_nb_streams;
    /**< Number of narrow-band (8 KHz) streams active in the system. */
  uint16_t num_wb_streams;
    /**< Number of wide-band (16 KHz) streams active in the system. */
  uint16_t num_swb_streams;
    /**< Number of super wide-band (32 KHz) streams active in the system. */
  uint16_t num_fb_plus_streams;
    /**< Number of full-band (48 KHz) or higher streams active in the system. */
  uint16_t num_nb_vocprocs;
    /**< Number of narrow-band (8 KHz) vocprocs active in the system. */
  uint16_t num_wb_vocprocs;
    /**< Number of wide-band (16 Khz) vocprocs active in the system. */
  uint16_t num_swb_vocprocs;
    /**< Number of super wide-band (32 KHz) vocprocs active in the system. */
  uint16_t num_fb_plus_vocprocs;
    /**< Number of full-band (48 Khz) or higher vocprocs active
         in the system. */
  uint32_t enc_kpps;
    /**< Encoder KPPS requirements. Applicable only if there is a single stream
         attached to this MVM session. It is ignored otherwise. */
  uint32_t dec_kpps;
    /**< Decoder KPPS requirements. Applicable only if there is a single stream
         attached to this MVM session. It is ignored otherwise. */
  uint32_t dec_pp_kpps;
    /**< Stream RX post-processing block KPPS requirements. Applicable only if
         there is a single stream attached to this MVM session. It is ignored
         otherwise. */
  uint32_t vp_rx_kpps;
    /**< Vocproc RX KPPS requirements. Applicable only if there is a single vocproc
         attached to this MVM session. It is ignored otherwise. */
  uint32_t vp_tx_kpps;
    /**< Vocproc TX KPPS requirements. Applicable only if there is a single vocproc
         attached to this MVM session. It is ignored otherwise. */		 
  uint32_t total_kpps;
    /**< Total KPPS for all the streams and vocprocs active in the system. */
  uint32_t tx_afe_port_sr;
    /** Tx AFE port SR. */
  uint32_t total_core_kpps;
    /**< VDSP's current Q6 core KPPS that has been requested by
         MVM-CCM to MMPM. */
  uint32_t tx_topology_id;
    /**< Tx vocproc topology ID. Applicable only if there is a single active
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint32_t rx_topology_id;
    /**< Rx vocproc topology ID. Applicable only if there is a single active
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint32_t media_id;
    /**< Media ID. Applicable only if there is a single active
         voice session which has a single vocproc and/or stream attached to it.
         It is ignored otherwise. */
  uint16_t vfr_mode;
    /**< The VFR mode. Applicable only if there is a single active voice
         session in the system, which has a single vocproc and/or stream
         attached to it. It is ignored otherwise.
         The supported values:\n
         #VSS_ICOMMON_VFR_MODE_SOFT \n
         #VSS_ICOMMON_VFR_MODE_HARD */
  uint32_t tx_num_channels;
    /**< Tx Number of channels. Applicable only if there is a single active
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint32_t num_clk_perf_levels;
    /**< Number of clock levels. */
  uint32_t clk_perf_level[ MVM_CCM_MAX_NUM_CLK_PERF_LEVELS ];
    /**< Clock levels. */
  uint32_t tx_mpps_scale_factor;
    /** Tx MPPS scale factor. */
  uint32_t tx_bw_scale_factor;
    /** Tx BW scale factor. */
  uint32_t rx_mpps_scale_factor;
    /** Rx MPPS scale factor. */
  uint32_t rx_bw_scale_factor;
    /** Rx BW scale factor. */
  mvm_voice_processing_delays_t processing_times;
}
  mvm_ccm_system_config_t;

typedef struct mvm_ccm_active_mvm_session_tracking_obj_t
{
  uint16_t handle;
    /**< Handle to this MVM session. */
  uint16_t num_nb_streams;
    /**< Number of narrow-band (8 KHz) streams connected to this MVM
         session. */
  uint16_t num_wb_streams;
    /**< Number of wide-band (16 KHz) streams connected to this MVM
         session */
  uint16_t num_swb_streams;
    /**< Number of super wide-band (32 KHz) streams connected to this MVM
         session */
  uint16_t num_fb_plus_streams;
    /**< Number of full-band (48 KHz) or higher streams connected to this MVM
         session */
  uint16_t num_nb_vocprocs;
    /**< Number of narrow-band (8 KHz) vocprocs connected to this MVM
         session. */
  uint16_t num_wb_vocprocs;
    /**< Number of wide-band (16 Khz) vocprocs connected to this MVM
         session */
  uint16_t num_swb_vocprocs;
    /**< Number of super wide-band (32 KHz) vocprocs connected to this MVM
         session */
  uint16_t num_fb_plus_vocprocs;
    /**< Number of full-band (48 Khz) or higher vocprocs connected to this MVM
         session */
   uint32_t enc_kpps;
    /**< Encoder KPPS requirements. Applicable only if there is a single stream
         attached to this MVM session. It is ignored otherwise. */
   uint32_t dec_kpps;
    /**< Decoder KPPS requirements. Applicable only if there is a single stream
         attached to this MVM session. It is ignored otherwise. */
   uint32_t dec_pp_kpps;
    /**< Stream RX post-processing block KPPS requirements. Applicable only if
         there is a single stream attached to this MVM session. It is ignored
         otherwise. */
   uint32_t vp_rx_kpps;
    /**< Vocproc RX KPPS requirements. Applicable only if there is a single vocproc
         attached to this MVM session. It is ignored otherwise. */
   uint32_t vp_tx_kpps;
    /**< Vocproc TX KPPS requirements. Applicable only if there is a single vocproc
         attached to this MVM session. It is ignored otherwise. */
  uint32_t total_kpps;
    /**< Total KPPS for all the streams and vocprocs connected to this MVM
         session. */
  uint32_t tx_afe_port_sr;
    /** Tx AFE port SR. */
  uint32_t tx_topology_id;
    /**< Tx vocproc topology ID. Applicable only if there is a single vocproc
         connected to this MVM session. It is ignored otherwise. */
  uint32_t rx_topology_id;
    /**< Rx vocproc topology ID. Applicable only if there is a single vocproc
         connected to this MVM session. It is ignored otherwise. */
  uint32_t media_id;
    /**< Media ID. Applicable only if there is a single vocproc and/or stream
         connected to this MVM session. It is ignored otherwise. */
  uint16_t vfr_mode;
    /**< The VFR mode. The supported values:\n
         The supported values:\n
         #VSS_ICOMMON_VFR_MODE_SOFT \n
         #VSS_ICOMMON_VFR_MODE_HARD */
  uint32_t tx_num_channels;
    /**< Tx Number of channels. Applicable only if there is a single active
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint32_t tx_mpps_scale_factor;
    /** Tx MPPS scale factor. */
  uint32_t tx_bw_scale_factor;
    /** Tx BW scale factor. */
  uint32_t rx_mpps_scale_factor;
    /** Rx MPPS scale factor. */
  uint32_t rx_bw_scale_factor;
    /** Rx BW scale factor. */
  mvm_voice_timing_t voice_timing; 
  mvm_voice_processing_delays_t processing_times;
  mvm_vfr_subscription_info_t vfr_info;
  uint32_t vptx_frame_size;
}
  mvm_ccm_active_mvm_session_tracking_obj_t;

typedef struct mvm_ccm_active_mvm_session_tracking_list_item_t
{
  apr_list_node_t link;
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj;
}
  mvm_ccm_active_mvm_session_tracking_list_item_t;

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

/* Heap Management. */
static uint8_t mvm_ccm_heap_pool[ MVM_CCM_HEAP_SIZE_V ];
static apr_memmgr_type mvm_ccm_heapmgr;

/* Active MVM session management. */
static mvm_ccm_active_mvm_session_tracking_list_item_t mvm_ccm_active_mvm_session_tracking_list_item_pool[ MVM_CCM_MAX_ACTIVE_MVM_SESSIONS_V ];
static apr_list_t mvm_ccm_active_mvm_session_tracking_free_q;
static apr_list_t mvm_ccm_active_mvm_session_tracking_used_q;

/* Overall system configuration management. */
static mvm_ccm_system_config_t mvm_ccm_system_config;
static cvd_devcfg_preemption_safety_margins_values_t preemption_safety_margins;
/* Overall central sleep manager configuration. */
static mvm_csm_config_t mvm_csm_config;

static uint32_t mvm_ccm_mvm_service_apr_handle;
static uint16_t mvm_ccm_mvm_service_apr_addr;

#if ( ADSPPM_INTEGRATION == 1 )
static char_t mvm_ccm_mmpm_client_name[] = "MVM_CCM";
static uint32_t mvm_ccm_mmpm_client_id;
#endif

/* CSM task lock. */
static apr_lock_t mvm_csm_task_lock;

/* CSM timers. */
apr_timer_t mvm_csm_tx_timer;
apr_timer_t mvm_csm_rx_timer;
apr_timer_t mvm_csm_enable_timer;
/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/
static int32_t mvm_csm_task_lock_fn ( void )
{
   if ( NULL != mvm_csm_task_lock ) 
   {
     ( void ) apr_lock_enter( mvm_csm_task_lock );
     return APR_EOK;
   }
   return APR_EBADPARAM;
}

static void mvm_csm_task_unlock_fn ( void )
{
  if ( NULL != mvm_csm_task_lock ) 
  {
     ( void ) apr_lock_leave( mvm_csm_task_lock );
  }
}
static bool_t mvm_ccm_is_not_internal_topology( uint32_t topology_id )
{
  bool_t result = FALSE;

  switch ( topology_id )
  {
    case VPM_TX_NONE:
    case VPM_TX_SM_ECNS:
    case VPM_TX_SM_FLUENCEV5:
    case VPM_TX_SM_ECNS_V2:
    case VPM_TX_DM_FLUENCE:
    case VPM_TX_DM_FLUENCEV5:
    case VPM_TX_DM_VPECNS:
    case VPM_TX_DM_FLUENCEV5_BROADSIDE:
    case VPM_TX_QM_FLUENCE_PROV2:
    case VPM_RX_DEFAULT:
    case VPM_RX_DEFAULT_V2:
    case VPM_RX_DEFAULT_V3:
      result = FALSE;
      break;

    default:
      result = TRUE;
      break;
  }

  return result;
}

static int32_t mvm_ccm_get_clock_level_info( void )
{
  int32_t rc = APR_EOK;

  rc = cvd_devcfg_parser_get_clk_perf_levels( &( mvm_ccm_system_config.num_clk_perf_levels ),
                                            mvm_ccm_system_config.clk_perf_level);
  if( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
  	     "mvm_ccm_get_clock_level_info(): Failed to get clock levels" );
    return APR_EFAILED;
  }
  
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "mvm_ccm_get_clock_level_info(): num_clk_perf_levels = %d", 
         mvm_ccm_system_config.num_clk_perf_levels );

  for ( uint32_t i = 0; i < mvm_ccm_system_config.num_clk_perf_levels; i++)
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "mvm_ccm_get_clock_level_info(): "
           "CLOCK_NPA_QUERY_PERF_LEVEL_KHZ[%d] = %d", i, mvm_ccm_system_config.clk_perf_level[i] );
  }

  return rc;
}
static uint32_t mvm_ccm_map_clock_to_perf_level (
  uint32_t clock_in_khz
)
{
  uint32_t i;
  uint32_t max_q6_core_clock_in_mhz;

  ( void ) cvd_devcfg_parser_get_max_q6_core_clock( &max_q6_core_clock_in_mhz );

  /* Cap to max supported clcok if input is too high. */
  if ( clock_in_khz > ( max_q6_core_clock_in_mhz * 1000 ) )
  {
    return ( max_q6_core_clock_in_mhz * 1000 );
  }

  if ( !mvm_ccm_system_config.num_clk_perf_levels )
  {
    return clock_in_khz;
  }

  for ( i = 0; i < mvm_ccm_system_config.num_clk_perf_levels; i++ )
  {
    if ( clock_in_khz <= mvm_ccm_system_config.clk_perf_level[i] )
    {
      clock_in_khz = mvm_ccm_system_config.clk_perf_level[i];
      break;
    }
  }

  return clock_in_khz;
}

static uint32_t mvm_ccm_get_next_perf_level (
  uint32_t clock_in_khz
)
{
  uint32_t i;
  uint32_t max_q6_core_clock_in_mhz;
  uint32_t next_clock_in_khz = 0;

  for ( ;; ) 
  {
    ( void ) cvd_devcfg_parser_get_max_q6_core_clock( &max_q6_core_clock_in_mhz );
    
    /* Cap to max supported clcok if input is too high. */
    if ( clock_in_khz >= ( max_q6_core_clock_in_mhz * 1000 ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_ccm_get_next_perf_level():  "
           "already running at max clock %d %d",clock_in_khz, max_q6_core_clock_in_mhz );
      break;
    }

    if ( !mvm_ccm_system_config.num_clk_perf_levels )
    {
      break;
    }

    for ( i = 0; i < ( mvm_ccm_system_config.num_clk_perf_levels - 1 ); i++ )
    {
      if ( clock_in_khz <= mvm_ccm_system_config.clk_perf_level[i] )
      {
        next_clock_in_khz = mvm_ccm_system_config.clk_perf_level[ i+1 ];
        break;
      }
    }
    break;
  }
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "mvm_ccm_get_next_perf_level():  "
           "next_clock: %d, max_clock: %d",next_clock_in_khz, max_q6_core_clock_in_mhz );
  return next_clock_in_khz;
}
/****************************************************************************
 * MVM-CCM CORE ROUTINES                                                    *
 ****************************************************************************/

static int32_t mvm_ccm_get_active_mvm_session_tracking_obj (
  uint16_t mvm_session_handle,
  mvm_ccm_active_mvm_session_tracking_obj_t** ret_session_tracking_obj
)
{
  int32_t rc = APR_EOK;
  mvm_ccm_active_mvm_session_tracking_list_item_t* session_tracking_list_item;
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj = NULL;

  if ( ret_session_tracking_obj == NULL )
    MVM_CCM_PANIC_ON_ERROR( APR_EUNEXPECTED );

  session_tracking_list_item =
    ( ( mvm_ccm_active_mvm_session_tracking_list_item_t* )
      &mvm_ccm_active_mvm_session_tracking_used_q.dummy );

  for ( ;; )
  {
    rc = apr_list_get_next(
           &mvm_ccm_active_mvm_session_tracking_used_q,
           ( ( apr_list_node_t* ) session_tracking_list_item ),
           ( ( apr_list_node_t** ) &session_tracking_list_item ) );
    if ( rc ) break;

    session_tracking_obj = session_tracking_list_item->session_tracking_obj;

    if ( session_tracking_obj->handle == mvm_session_handle )
    { /* Found a session with the client specified MVM session handle. */
      break;
    }
    else
    { /* Keep looking. */
      session_tracking_obj = NULL;
    }
  }

  if ( session_tracking_obj == NULL )
  {
    rc = APR_ENOTEXIST;
  }
  else
  {
    rc = APR_EOK;
    *ret_session_tracking_obj = session_tracking_obj;
  }

  return rc;
}

static int32_t mvm_ccm_create_active_mvm_session_tracking_obj (
  mvm_ccm_active_mvm_session_tracking_obj_t** ret_session_tracking_obj
)
{
  int32_t rc = APR_EOK;
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj;

  if ( ret_session_tracking_obj == NULL )
    MVM_CCM_PANIC_ON_ERROR( APR_EUNEXPECTED );

  for ( ;; )
  {
    session_tracking_obj = apr_memmgr_malloc( &mvm_ccm_heapmgr,
                             sizeof( mvm_ccm_active_mvm_session_tracking_obj_t ) );
    if ( session_tracking_obj == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_ccm_create_active_mvm_session_tracking_obj(): Failed to "
           "allocate memory for session tracking object." );
      rc = APR_ENORESOURCE;
      break;
    }

    { /* Initialize the session tracking object. */
      session_tracking_obj->handle = APR_NULL_V;
      session_tracking_obj->num_nb_streams = 0;
      session_tracking_obj->num_wb_streams = 0;
      session_tracking_obj->num_swb_streams = 0;
      session_tracking_obj->num_fb_plus_streams = 0;
      session_tracking_obj->num_nb_vocprocs = 0;
      session_tracking_obj->num_wb_vocprocs = 0;
      session_tracking_obj->num_swb_vocprocs = 0;
      session_tracking_obj->num_fb_plus_vocprocs = 0;
      session_tracking_obj->enc_kpps = 0;
      session_tracking_obj->dec_kpps = 0;
      session_tracking_obj->dec_pp_kpps = 0;
      session_tracking_obj->vp_rx_kpps = 0;
      session_tracking_obj->vp_tx_kpps = 0;
      session_tracking_obj->total_kpps = 0;
      session_tracking_obj->tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
      session_tracking_obj->rx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
      session_tracking_obj->media_id = VSS_MEDIA_ID_NONE;
      session_tracking_obj->vfr_mode = VSS_ICOMMON_VFR_MODE_SOFT;
    }

    *ret_session_tracking_obj = session_tracking_obj;
    break;
  }

  return rc;
}

static int32_t mvm_ccm_destroy_active_mvm_session_tracking_obj (
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj
)
{
  if ( session_tracking_obj == NULL )
    MVM_CCM_PANIC_ON_ERROR( APR_EUNEXPECTED );

  apr_memmgr_free( &mvm_ccm_heapmgr, session_tracking_obj );

  return APR_EOK;
}

static int32_t mvm_ccm_queue_active_mvm_session_tracking_obj (
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj
)
{
  int32_t rc = APR_EOK;
  mvm_ccm_active_mvm_session_tracking_list_item_t* session_tracking_list_item;

  if ( session_tracking_obj == NULL )
    MVM_CCM_PANIC_ON_ERROR( APR_EUNEXPECTED );

  for ( ;; )
  {
    /* Get a free active MVM session tracking list item. */
    rc = apr_list_remove_head(
           &mvm_ccm_active_mvm_session_tracking_free_q,
           ( ( apr_list_node_t** ) &session_tracking_list_item ) );
    if ( rc )
    { /* No free session tracking list item is available. */
      rc = APR_ENORESOURCE;
      break;
    }

    /* Queue the new active MVM session tracking obj. */
    session_tracking_list_item->session_tracking_obj = session_tracking_obj;

    ( void ) apr_list_add_tail(
               &mvm_ccm_active_mvm_session_tracking_used_q,
               &session_tracking_list_item->link );

    break;
  }

  return rc;
}

static int32_t mvm_ccm_dequeue_active_mvm_session_tracking_obj (
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj
)
{
  int32_t rc;
  mvm_ccm_active_mvm_session_tracking_list_item_t* session_tracking_list_item;

  if ( session_tracking_obj == NULL )
    MVM_CCM_PANIC_ON_ERROR( APR_EUNEXPECTED );

  session_tracking_list_item =
    ( ( mvm_ccm_active_mvm_session_tracking_list_item_t* )
      &mvm_ccm_active_mvm_session_tracking_used_q.dummy );

  for ( ;; )
  {
    rc = apr_list_get_next(
           &mvm_ccm_active_mvm_session_tracking_used_q,
           ( ( apr_list_node_t* ) session_tracking_list_item ),
           ( ( apr_list_node_t** ) &session_tracking_list_item ) );
    if ( rc ) break;

    if ( session_tracking_obj == session_tracking_list_item->session_tracking_obj )
    {
      ( void ) apr_list_delete(
                 &mvm_ccm_active_mvm_session_tracking_used_q,
                 &session_tracking_list_item->link );
      ( void ) apr_list_add_tail(
                 &mvm_ccm_active_mvm_session_tracking_free_q,
                 &session_tracking_list_item->link );
      break;
    }
  }

  return rc;
}


#ifndef WINSIM

static int32_t mvm_ccm_mmpm_register ( void )
{
  int32_t rc = APR_EOK;

#if ( ADSPPM_INTEGRATION == 1 )

  MmpmRegParamType register_param;
  cvd_devcfg_parser_mmpm_core_info_t mmpm_core_info;
  MmpmClientClassType class_type;
  MmpmParameterConfigType param_cfg;
  MMPM_STATUS status = MMPM_STATUS_SUCCESS;
  MmpmDcvsParticipationType dcvs_participation;

  for ( ;; )
  {
    rc = cvd_devcfg_parser_get_mmpm_core_info( &mmpm_core_info );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_ccm_mmpm_register(): Failed to get mmpm core info from "
             "cvd devcfg parser, rc = 0x%08X", rc );
      break;
    }

    register_param.rev = MMPM_REVISION;
    register_param.coreId = mmpm_core_info.core_id;
    register_param.instanceId = mmpm_core_info.instance_id;
    register_param.pClientName = mvm_ccm_mmpm_client_name;
    register_param.pwrCtrlFlag = PWR_CTRL_NONE;
    register_param.callBackFlag = CALLBACK_NONE;
    register_param.MMPM_Callback = NULL;
    register_param.cbFcnStackSize = 0;

    mvm_ccm_mmpm_client_id = MMPM_Register_Ext( &register_param );
    if ( mvm_ccm_mmpm_client_id == 0 )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_ccm_mmpm_register(): Failed to register with MMPM." );
      rc = APR_EFAILED;
      MVM_CCM_PANIC_ON_ERROR( rc );
      break;
    }

    //Register for client class as Voice to ADSPPM.
    class_type = MMPM_VOICE_CLIENT_CLASS;
    param_cfg.pParamConfig = (void *)&class_type;
    param_cfg.paramId = MMPM_PARAM_ID_CLIENT_CLASS;
    status = MMPM_SetParameter( mvm_ccm_mmpm_client_id, &param_cfg );
    if(status != MMPM_STATUS_SUCCESS)
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_ccm_mmpm_register(): MMPM_SetParameter class type failed for "
             "client id %lu with status %lu ", mvm_ccm_mmpm_client_id, (uint32_t)status);
      rc = APR_EFAILED;
      break;
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "mvm_ccm_mmpm_register(): MMPM_SetParameter class type success "
             "for client id %lu", mvm_ccm_mmpm_client_id);
    }

    //Register for DCVS up only
    //CVD's core/bus votes are subject to automatic up adjustment by DCVS
    dcvs_participation.enable = TRUE;
    dcvs_participation.enableOpt = MMPM_DCVS_ADJUST_ONLY_UP;
    param_cfg.pParamConfig = (void*)&dcvs_participation;
    param_cfg.paramId = MMPM_PARAM_ID_DCVS_PARTICIPATION;
    status = MMPM_SetParameter( mvm_ccm_mmpm_client_id, &param_cfg );
    if(status != MMPM_STATUS_SUCCESS)
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_ccm_mmpm_register(): MMPM_SetParameter DCVS participation "
             "for Only UP failed for client id %lu with status %lu ",
             mvm_ccm_mmpm_client_id, (uint32_t)status);
      rc = APR_EFAILED;
      break;
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "mvm_ccm_mmpm_register(): MMPM_SetParameter DCVS participation "
             "for Only UP success for client id %lu", mvm_ccm_mmpm_client_id);
    }

    break;
  }

#endif

  return rc;
}

static int32_t mvm_ccm_mmpm_deregister ( void )
{

#if ( ADSPPM_INTEGRATION == 1 )

  MMPM_STATUS mmpm_rc;

  mmpm_rc = MMPM_Deregister_Ext( mvm_ccm_mmpm_client_id );
  if ( mmpm_rc != MMPM_STATUS_SUCCESS )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_ccm_mmpm_deregister(): Failed to de-register with MMPM for "
           "client ID %d, mmpm_rc = %d ", mvm_ccm_mmpm_client_id, mmpm_rc );
    MVM_CCM_PANIC_ON_ERROR( APR_EFAILED );
    return APR_EFAILED;
  }

#endif

  return APR_EOK;
}

static int32_t mvm_ccm_mmpm_request (
  MmpmMppsReqType* mpps,
  MmpmGenBwReqType* bw,
  uint32_t sleep_latency_us
)
{
  int32_t rc = APR_EOK;

#if ( ADSPPM_INTEGRATION == 1 )

  uint32_t bw_req_count;
  MMPM_STATUS mmpm_global_rc;
  MMPM_STATUS mmpm_individual_rc[ MVM_CCM_NUM_RSCS_PER_MMPM_REQUEST_V ];
  MmpmRscParamType rsc_param[ MVM_CCM_NUM_RSCS_PER_MMPM_REQUEST_V ];
  MmpmRscExtParamType request_param;

  if ( ( mpps == NULL ) || ( bw == NULL ) )
  {
    MVM_CCM_PANIC_ON_ERROR( APR_EUNEXPECTED );
  }

  rsc_param[ 0 ].rscId = MMPM_RSC_ID_MPPS;
  rsc_param[ 0 ].rscParam.pMppsReq = mpps;

  rsc_param[ 1 ].rscId = MMPM_RSC_ID_GENERIC_BW_EXT;
  rsc_param[ 1 ].rscParam.pGenBwReq = bw;

  rsc_param[ 2 ].rscId = MMPM_RSC_ID_SLEEP_LATENCY;
  rsc_param[ 2 ].rscParam.sleepMicroSec = sleep_latency_us;

  request_param.apiType = MMPM_API_TYPE_SYNC;
  request_param.numOfReq = MVM_CCM_NUM_RSCS_PER_MMPM_REQUEST_V;
  request_param.pReqArray = rsc_param;
  request_param.pStsArray = mmpm_individual_rc;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "mvm_ccm_mmpm_request(): Total MPPS = %d, Core Floor Clock = %d, "
         "Num of BW req = %d", mpps->mppsTotal, mpps->adspFloorClock,
         bw->numOfBw );

  for ( bw_req_count = 0; bw_req_count < bw->numOfBw; ++bw_req_count )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "mvm_ccm_mmpm_request(): BW req master port = %d, Slave port = %d",
           bw->pBandWidthArray[bw_req_count].busRoute.masterPort,
           bw->pBandWidthArray[ bw_req_count ].busRoute.slavePort );

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "mvm_ccm_mmpm_request(): Bytes per sec = %d, Usage percentage = %d, "
           "Usage type = %d", bw->pBandWidthArray[bw_req_count].bwValue.busBwValue.bwBytePerSec,
           bw->pBandWidthArray[bw_req_count].bwValue.busBwValue.usagePercentage,
           bw->pBandWidthArray[ bw_req_count ].bwValue.busBwValue.usageType );
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "mvm_ccm_mmpm_request(): Sleep latency = %d", sleep_latency_us );

  mmpm_global_rc = MMPM_Request_Ext( mvm_ccm_mmpm_client_id, &request_param );
  if ( mmpm_global_rc != MMPM_STATUS_SUCCESS )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_ccm_mmpm_request(): MMPM request failed. MIPS req rc = %d, "
           "BW req rc = %d, Sleep latency req rc = %d", mmpm_individual_rc[0],
           mmpm_individual_rc[1], mmpm_individual_rc[ 2 ] );
    rc = APR_EFAILED;
    MVM_CCM_PANIC_ON_ERROR( APR_EFAILED );
  }

#endif

  return rc;
}

static int32_t mvm_ccm_mmpm_release ( void )
{
  int32_t rc = APR_EOK;

#if ( ADSPPM_INTEGRATION == 1 )

  MMPM_STATUS mmpm_global_rc;
  MMPM_STATUS mmpm_individual_rc[ MVM_CCM_NUM_RSCS_PER_MMPM_REQUEST_V ];
  MmpmRscParamType rsc_param[ MVM_CCM_NUM_RSCS_PER_MMPM_REQUEST_V ];
  MmpmRscExtParamType release_param;

  /* MMPM release only requires resource IDs to be indicated, not the resource
   * values such as MIPS number. All the resources under the resource IDs will
   * be released. */
  rsc_param[ 0 ].rscId = MMPM_RSC_ID_MPPS;
  rsc_param[ 1 ].rscId = MMPM_RSC_ID_GENERIC_BW_EXT;
  rsc_param[ 2 ].rscId = MMPM_RSC_ID_SLEEP_LATENCY;

  release_param.apiType = MMPM_API_TYPE_ASYNC;
  release_param.numOfReq = MVM_CCM_NUM_RSCS_PER_MMPM_REQUEST_V;
  release_param.pReqArray = rsc_param;
  release_param.pStsArray = mmpm_individual_rc;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,
       "mvm_ccm_mmpm_release(): Releasing MMPM resources." );

  mmpm_global_rc = MMPM_Release_Ext( mvm_ccm_mmpm_client_id, &release_param );
  if ( mmpm_global_rc != MMPM_STATUS_SUCCESS )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_ccm_mmpm_release(): MMPM release failed. MIPS release rc = %d, "
           "BW release rc = %d, Sleep release rc = %d", mmpm_individual_rc[0],
           mmpm_individual_rc[1], mmpm_individual_rc[ 2 ] );
    rc = APR_EFAILED;
    MVM_CCM_PANIC_ON_ERROR( APR_EFAILED );
  }

#endif

  return rc;
}

/* Compute clock voting for the topology use cases, which are not part of devcfg.
   This is applicable for custom topology scenarios also
 */
static int32_t mvm_ccm_compute_topo_clock_and_bw ( mvm_ccm_system_config_t* use_case,
  cvd_devcfg_parser_target_specific_config_t* clock_and_bus_preempt_margin_config
)
{
  MmpmBusBWDataUsageType *busBwValue;
  uint32_t mpps_scale_factor;
  uint32_t bw_scale_factor;
  uint32_t i;
  MmpmGenBwReqType* bw;
  uint32_t clock_in_hz;

  if ( ( use_case == NULL ) || ( clock_and_bus_preempt_margin_config == NULL ) )
  {
    return APR_EBADPARAM;
  }
  /* Pick the maximum scale factor */
  if ( use_case->tx_mpps_scale_factor > use_case->rx_mpps_scale_factor )
    mpps_scale_factor = use_case->tx_mpps_scale_factor;
  else
    mpps_scale_factor = use_case->rx_mpps_scale_factor;

  if ( use_case->tx_bw_scale_factor > use_case->rx_bw_scale_factor )
    bw_scale_factor = use_case->tx_bw_scale_factor;
  else
    bw_scale_factor = use_case->rx_bw_scale_factor;

  if ( bw_scale_factor == 0 )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "mvm_ccm_compute_topo_clock_and_bw(): No scale factors, "
         "use defaults" );
    bw_scale_factor = MVM_CCM_DEFAULT_BW_SCALE_FACTOR;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "mvm_ccm_compute_topo_clock_and_bw(): mpps_scale_factor "
         "%d/10, bw_scale_factor %d/10", mpps_scale_factor, bw_scale_factor );

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "mvm_ccm_compute_topo_clock_and_bw(): clock_hz %d, "
         "cycles_per_thousand_instr_packets %d, total_kpps %d",
         clock_and_bus_preempt_margin_config->core_floor_clock_hz,
         clock_and_bus_preempt_margin_config->cycles_per_thousand_instr_packets,
         mvm_ccm_system_config.total_kpps );

  /* floor_clock = total_mpps * cpp * mpps_scale_factor */
  clock_in_hz = ( mvm_ccm_system_config.total_kpps * clock_and_bus_preempt_margin_config->cycles_per_thousand_instr_packets / 
                  MVM_CCM_SCALE_FACTOR_DIV * mpps_scale_factor );
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "mvm_ccm_compute_topo_clock_and_bw(): clock_hz %d"
         " before mapping", clock_in_hz );

  clock_and_bus_preempt_margin_config->core_floor_clock_hz = mvm_ccm_map_clock_to_perf_level( clock_in_hz / 1000 ) * 1000;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "mvm_ccm_compute_topo_clock_and_bw(): clock_hz %d after "
         "mapping", clock_and_bus_preempt_margin_config->core_floor_clock_hz );

  /* bw = avg_bw * bw_scale_factor */
  bw = &clock_and_bus_preempt_margin_config->bw_requirement;
  for ( i = 0; i < bw->numOfBw; i++ )
  {
    busBwValue = &bw->pBandWidthArray[i].bwValue.busBwValue;
    busBwValue->bwBytePerSec =
    ( busBwValue->bwBytePerSec / MVM_CCM_SCALE_FACTOR_DIV * bw_scale_factor );

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "mvm_ccm_compute_topo_clock_and_bw(): BW [MSB=0x%x, "
           "LSB=0x%08x] bps, usage percentage %d", ( busBwValue->bwBytePerSec >> 32 ),
           ( busBwValue->bwBytePerSec & ( (uint32_t) 0XFFFFFFFF ) ),
           busBwValue->usagePercentage );
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "mvm_ccm_compute_topo_clock_and_bw(): clock_hz %d, "
         "sleep_latency_us %d", clock_and_bus_preempt_margin_config->core_floor_clock_hz,
         clock_and_bus_preempt_margin_config->sleep_latency_us );

  return APR_EOK;
}

#endif /* !WINSIM */

static int32_t mvm_ccm_update_system_config (
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj,
  bool_t is_addition
)
{
  int32_t rc = APR_EOK;
  mvm_ccm_active_mvm_session_tracking_list_item_t* session_tracking_list_item;

  if ( session_tracking_obj == NULL )
    MVM_CCM_PANIC_ON_ERROR( APR_EUNEXPECTED );

  if ( is_addition )
  { /* Add the session configurations into the overall system configurations. */
    //mvm_ccm_system_config.num_voice_sessions += 1;

    mvm_ccm_system_config.num_nb_streams += session_tracking_obj->num_nb_streams;

    mvm_ccm_system_config.num_wb_streams += session_tracking_obj->num_wb_streams;

    mvm_ccm_system_config.num_swb_streams += session_tracking_obj->num_swb_streams;

    mvm_ccm_system_config.num_fb_plus_streams += session_tracking_obj->num_fb_plus_streams;

    mvm_ccm_system_config.num_nb_vocprocs += session_tracking_obj->num_nb_vocprocs;

    mvm_ccm_system_config.num_wb_vocprocs += session_tracking_obj->num_wb_vocprocs;

    mvm_ccm_system_config.num_swb_vocprocs += session_tracking_obj->num_swb_vocprocs;

    mvm_ccm_system_config.num_fb_plus_vocprocs += session_tracking_obj->num_fb_plus_vocprocs;

    mvm_ccm_system_config.total_kpps += session_tracking_obj->total_kpps;
    mvm_ccm_system_config.enc_kpps += session_tracking_obj->enc_kpps;
    mvm_ccm_system_config.dec_kpps += session_tracking_obj->dec_kpps;
    mvm_ccm_system_config.dec_pp_kpps += session_tracking_obj->dec_pp_kpps;
    mvm_ccm_system_config.vp_rx_kpps += session_tracking_obj->vp_rx_kpps;
    mvm_ccm_system_config.vp_tx_kpps += session_tracking_obj->vp_tx_kpps;
  }
  else
  { /* Remove the session configurations from the overall system configurations. */
    //mvm_ccm_system_config.num_voice_sessions -= 1;

    mvm_ccm_system_config.num_nb_streams -= session_tracking_obj->num_nb_streams;

    mvm_ccm_system_config.num_wb_streams -= session_tracking_obj->num_wb_streams;

    mvm_ccm_system_config.num_swb_streams -= session_tracking_obj->num_swb_streams;

    mvm_ccm_system_config.num_fb_plus_streams -= session_tracking_obj->num_fb_plus_streams;

    mvm_ccm_system_config.num_nb_vocprocs -= session_tracking_obj->num_nb_vocprocs;

    mvm_ccm_system_config.num_wb_vocprocs -= session_tracking_obj->num_wb_vocprocs;

    mvm_ccm_system_config.num_swb_vocprocs -= session_tracking_obj->num_swb_vocprocs;

    mvm_ccm_system_config.num_fb_plus_vocprocs -= session_tracking_obj->num_fb_plus_vocprocs;

    mvm_ccm_system_config.total_kpps -= session_tracking_obj->total_kpps;
    mvm_ccm_system_config.enc_kpps -= session_tracking_obj->enc_kpps;
    mvm_ccm_system_config.dec_kpps -= session_tracking_obj->dec_kpps;
    mvm_ccm_system_config.dec_pp_kpps -= session_tracking_obj->dec_pp_kpps;
    mvm_ccm_system_config.vp_rx_kpps -= session_tracking_obj->vp_rx_kpps;
    mvm_ccm_system_config.vp_tx_kpps -= session_tracking_obj->vp_tx_kpps;
  }
  mvm_ccm_system_config.tx_afe_port_sr = session_tracking_obj->tx_afe_port_sr;

  /* Note if there are multiple active MVM sessions, the topology IDs, and
   * vfr_mode are don't cares.
   */
  if ( mvm_ccm_system_config.num_voice_sessions == 1 )
  {
    rc = apr_list_peak_head( &mvm_ccm_active_mvm_session_tracking_used_q,
           ( ( apr_list_node_t** ) &session_tracking_list_item ) );
    MVM_CCM_PANIC_ON_ERROR( rc );

    mvm_ccm_system_config.tx_topology_id =
      session_tracking_list_item->session_tracking_obj->tx_topology_id;

    mvm_ccm_system_config.rx_topology_id =
      session_tracking_list_item->session_tracking_obj->rx_topology_id;

    mvm_ccm_system_config.vfr_mode =
      session_tracking_list_item->session_tracking_obj->vfr_mode;

    mvm_ccm_system_config.media_id =
      session_tracking_list_item->session_tracking_obj->media_id;

    mvm_ccm_system_config.tx_num_channels =
      session_tracking_list_item->session_tracking_obj->tx_num_channels;
    mvm_ccm_system_config.tx_mpps_scale_factor =
    session_tracking_list_item->session_tracking_obj->tx_mpps_scale_factor;
    mvm_ccm_system_config.tx_bw_scale_factor =
    session_tracking_list_item->session_tracking_obj->tx_bw_scale_factor;
    mvm_ccm_system_config.rx_mpps_scale_factor =
    session_tracking_list_item->session_tracking_obj->rx_mpps_scale_factor;
    mvm_ccm_system_config.rx_bw_scale_factor =
    session_tracking_list_item->session_tracking_obj->rx_bw_scale_factor;
  }
  
  return rc;
}

static int32_t mvm_csm_update_system_config (
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj
)
{
  if ( session_tracking_obj == NULL )
    MVM_CCM_PANIC_ON_ERROR( APR_EUNEXPECTED );

  mvm_csm_config.voice_timing = session_tracking_obj->voice_timing;
  mvm_csm_config.processing_times = session_tracking_obj->processing_times;
  mvm_csm_config.vfr_cycle_us = session_tracking_obj->vfr_info.vfr_cycle_duration_us;
  mvm_csm_config.vptx_frame_size = session_tracking_obj->vptx_frame_size;
  mvm_csm_config.num_venc_instances = mvm_csm_config.vfr_cycle_us / MVM_VOICE_FRAME_SIZE_US;

  return APR_EOK;
}

static void mvm_ccm_broadcast_configuration( void )
{
  int32_t rc = APR_EOK;
  vss_iccm_evt_active_sessions_t evt_active_sessions;
  mvm_ccm_active_mvm_session_tracking_list_item_t* session_tracking_list_item;
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj;

  /* Broadcast the system configuration to all active MVM sessions. */
  evt_active_sessions.num_voice_sessions = mvm_ccm_system_config.num_voice_sessions;
  evt_active_sessions.num_nb_streams = mvm_ccm_system_config.num_nb_streams;
  evt_active_sessions.num_wb_streams = mvm_ccm_system_config.num_wb_streams;
  evt_active_sessions.num_swb_streams = mvm_ccm_system_config.num_swb_streams;
  evt_active_sessions.num_fb_plus_streams = mvm_ccm_system_config.num_fb_plus_streams;
  evt_active_sessions.num_nb_vocprocs = mvm_ccm_system_config.num_nb_vocprocs;
  evt_active_sessions.num_wb_vocprocs = mvm_ccm_system_config.num_wb_vocprocs;
  evt_active_sessions.num_swb_vocprocs = mvm_ccm_system_config.num_swb_vocprocs;
  evt_active_sessions.num_fb_plus_vocprocs = mvm_ccm_system_config.num_fb_plus_vocprocs;
  evt_active_sessions.total_core_kpps = mvm_ccm_system_config.total_core_kpps;
  evt_active_sessions.vptx_preemption_safety_margin = preemption_safety_margins.vptx_preemption_safety_margin;
  evt_active_sessions.vprx_preemption_safety_margin = preemption_safety_margins.vprx_preemption_safety_margin;
  evt_active_sessions.venc_preemption_safety_margin = preemption_safety_margins.venc_preemption_safety_margin;
  evt_active_sessions.vdec_preemption_safety_margin = preemption_safety_margins.vdec_preemption_safety_margin;
  evt_active_sessions.vdec_proc = mvm_ccm_system_config.processing_times.vdec_proc;
  evt_active_sessions.venc_proc = mvm_ccm_system_config.processing_times.venc_proc;
  evt_active_sessions.vprx_proc = mvm_ccm_system_config.processing_times.vprx_proc;
  evt_active_sessions.vptx_proc = mvm_ccm_system_config.processing_times.vptx_proc;

  session_tracking_list_item =
    ( ( mvm_ccm_active_mvm_session_tracking_list_item_t* )
      &mvm_ccm_active_mvm_session_tracking_used_q.dummy );

  for ( ;; )
  {
    rc = apr_list_get_next( &mvm_ccm_active_mvm_session_tracking_used_q,
                            ( ( apr_list_node_t* ) session_tracking_list_item ),
                            ( ( apr_list_node_t** ) &session_tracking_list_item ) );
    if ( rc ) break;

    session_tracking_obj = session_tracking_list_item->session_tracking_obj;

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "mvm_ccm_broadcast_configuration(): mvm_handle=0x%08x",
           session_tracking_obj->handle );

    {
      rc = __aprv2_cmd_alloc_send(
               mvm_ccm_mvm_service_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               mvm_ccm_mvm_service_apr_addr, APR_NULL_V,
               mvm_ccm_mvm_service_apr_addr, session_tracking_obj->handle,
               0, VSS_ICCM_EVT_ACTIVE_SESSIONS,
               &evt_active_sessions, sizeof( evt_active_sessions ) );
      MVM_CCM_PANIC_ON_ERROR( rc );
    }
  }

  return;
}
static bool_t mvm_is_afe_port_fractional_rate ( 
  uint32_t sampling_rate
)
{
  bool_t   is_afe_fractional = FALSE;

  switch(sampling_rate)
  {
    case 11025:
    case 22050:
    case 44100:
    case 88200:
    case 176000:
      is_afe_fractional = TRUE;
       break;

    default:
      break;
  }

  return is_afe_fractional;
}

static int32_t mvm_ccm_calculate_processing_times (
   cvd_devcfg_parser_target_specific_config_t *clock_and_bus_preempt_margin_config
)
{
  int32_t rc = APR_EOK;
  uint32_t total_core_kpps = 0;
  uint32_t clock_in_khz = 0;
  for ( ;; )
  {
    if ( mvm_ccm_system_config.num_voice_sessions > 1 )
    {
      /* For multi-session (multiple calls or multiple streams or multiple
       * vocprocs) use case, give each processing stage (encoding, decoding,
       * stream/vocproc pre/post-processing) MVM_VOICE_FRAME_SIZE_US. */
      mvm_ccm_system_config.processing_times.venc_proc = MVM_VOICE_FRAME_SIZE_US;
      mvm_ccm_system_config.processing_times.vdec_proc = MVM_VOICE_FRAME_SIZE_US;
      mvm_ccm_system_config.processing_times.vptx_proc = MVM_VOICE_FRAME_SIZE_US;
      mvm_ccm_system_config.processing_times.vprx_proc = MVM_VOICE_FRAME_SIZE_US;
    }
    else if ( mvm_ccm_system_config.total_core_kpps ) 
    {
      /* For single voice call with single stream and single vocproc use case,
       * calculate the processing time based on the required processing KPPS and
       * the available Q6 core KPPS to achieve the best RTD.
       */
      total_core_kpps =
        mvm_ccm_system_config.total_core_kpps;

      mvm_ccm_system_config.processing_times.venc_proc =
        ( ( uint16_t ) ( ( mvm_ccm_system_config.enc_kpps *
                           MVM_VOICE_FRAME_SIZE_US ) / total_core_kpps ) );

      mvm_ccm_system_config.processing_times.vdec_proc =
        ( ( uint16_t ) ( ( mvm_ccm_system_config.dec_kpps *
                           MVM_VOICE_FRAME_SIZE_US ) / total_core_kpps ) );

      mvm_ccm_system_config.processing_times.vptx_proc =
        ( ( uint16_t ) ( ( mvm_ccm_system_config.vp_tx_kpps *
                           MVM_VOICE_FRAME_SIZE_US ) / total_core_kpps ) );

      mvm_ccm_system_config.processing_times.vprx_proc =
        ( ( uint16_t ) ( ( mvm_ccm_system_config.vp_rx_kpps *
                           MVM_VOICE_FRAME_SIZE_US ) / total_core_kpps ) );

      /* Additional 1ms safety margin in vptx/vprx timing offsets to compensate sample
       * slippling/stuffing. */
      mvm_ccm_system_config.processing_times.vptx_proc += MVM_VOCPROC_PROCESSING_TIME_SAFETY_MARGIN_US;
      mvm_ccm_system_config.processing_times.vprx_proc += MVM_VOCPROC_PROCESSING_TIME_SAFETY_MARGIN_US;

      /* Additional safety margin in the VpTx, VpRx, Vdec and Venc processing times 
      * to take care of thread preemption delay. */
      mvm_ccm_system_config.processing_times.vptx_proc += preemption_safety_margins.vptx_preemption_safety_margin;
      mvm_ccm_system_config.processing_times.vprx_proc += preemption_safety_margins.vprx_preemption_safety_margin;
      mvm_ccm_system_config.processing_times.vdec_proc += preemption_safety_margins.vdec_preemption_safety_margin;
      mvm_ccm_system_config.processing_times.venc_proc += preemption_safety_margins.venc_preemption_safety_margin;
      
      /* Additional 1000us (1ms) safety margin in the VpTx.
       check for fractional sampling rate at AFE endpoint and adjust the VpTx
       and VpRx delivery times accordingly. The effective time is reduced by 
       1ms to accommodate the jitter that is present due to data processing in 
       fractional resampler in AFE. */
  
      if(mvm_is_afe_port_fractional_rate( mvm_ccm_system_config.tx_afe_port_sr ))
      {
         mvm_ccm_system_config.processing_times.vptx_proc  += MVM_FRACT_RESAMP_BUFFER_TIME_US;
      }

      if ( ( mvm_ccm_system_config.processing_times.venc_proc > MVM_CCM_PROCESSING_TIME_THRESHOLD ) ||
           ( mvm_ccm_system_config.processing_times.vdec_proc > MVM_CCM_PROCESSING_TIME_THRESHOLD ) ||
           ( mvm_ccm_system_config.processing_times.vptx_proc > MVM_CCM_PROCESSING_TIME_THRESHOLD ) ||
           ( mvm_ccm_system_config.processing_times.vprx_proc > MVM_CCM_PROCESSING_TIME_THRESHOLD ) )
      {
        MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "MVM: goto next level: CURRENT: encProc:%d us, decProc:%d us, "
               "vptxProc:%d us, vprxProc:%d us, totalCoreKpps:%d ",
               mvm_ccm_system_config.processing_times.venc_proc, mvm_ccm_system_config.processing_times.vdec_proc,
               mvm_ccm_system_config.processing_times.vptx_proc, mvm_ccm_system_config.processing_times.vprx_proc,
               total_core_kpps );
        clock_in_khz = mvm_ccm_get_next_perf_level( clock_and_bus_preempt_margin_config->core_floor_clock_hz / 1000 );
        if ( clock_in_khz ) 
        {
          clock_and_bus_preempt_margin_config->core_floor_clock_hz = clock_in_khz * 1000;
          mvm_ccm_system_config.total_core_kpps = clock_and_bus_preempt_margin_config->core_floor_clock_hz /
                                                    clock_and_bus_preempt_margin_config->cycles_per_thousand_instr_packets;
          continue;
        }
      }
    }

    break;
  }

  MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: VOICE_PROCESSING_DELAYS: encProc:%d us, decProc:%d us, "
         "vptxProc:%d us, vprxProc:%d us, totalCoreKpps:%d ",
         mvm_ccm_system_config.processing_times.venc_proc, mvm_ccm_system_config.processing_times.vdec_proc,
         mvm_ccm_system_config.processing_times.vptx_proc, mvm_ccm_system_config.processing_times.vprx_proc,
         total_core_kpps );

  return rc;
}

static int32_t mvm_ccm_commit_system_config ( void )
{
  int32_t rc, i;
#ifndef WINSIM
  uint32_t total_million_packet_per_second;
  cvd_devcfg_parser_voice_use_case_t use_case;
  cvd_devcfg_parser_target_specific_config_t clock_and_bus_preempt_margin_config;
  MmpmMppsReqType mpps_request;
  MmpmGenBwValType bw;
#endif /* !WINSIM */

  for ( ;; )
  {

#if defined( WINSIM )

    if ( mvm_ccm_system_config.num_voice_sessions == 0 )
    {
      rc = APR_EOK;
      break;
    }

    /* Using a fake total_core_kpps value on simulator. */
    mvm_ccm_system_config.total_core_kpps = 100000;

    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "mvm_ccm_commit_system_config(): WINSIM DEFINED!!!" );

#else

    if ( mvm_ccm_system_config.num_voice_sessions == 0 )
    { /* Release the MMPM resources if there are no active MVM sessions. */
      rc = mvm_ccm_mmpm_release( );
      break;
    }

    {
      /* Get the clock and bus configuration from device config based on the
       * current use case.
       */
      use_case.num_voice_sessions = mvm_ccm_system_config.num_voice_sessions;
      use_case.num_nb_streams = mvm_ccm_system_config.num_nb_streams;
      use_case.num_wb_streams = mvm_ccm_system_config.num_wb_streams;
      use_case.num_swb_streams = mvm_ccm_system_config.num_swb_streams;
      use_case.num_fb_plus_streams = mvm_ccm_system_config.num_fb_plus_streams;
      use_case.num_nb_vocprocs = mvm_ccm_system_config.num_nb_vocprocs;
      use_case.num_wb_vocprocs = mvm_ccm_system_config.num_wb_vocprocs;
      use_case.num_swb_vocprocs = mvm_ccm_system_config.num_swb_vocprocs;
      use_case.num_fb_plus_vocprocs = mvm_ccm_system_config.num_fb_plus_vocprocs;
      use_case.tx_topology_id = mvm_ccm_system_config.tx_topology_id;
      use_case.rx_topology_id = mvm_ccm_system_config.rx_topology_id;
      use_case.media_id = mvm_ccm_system_config.media_id;
      use_case.vfr_mode = mvm_ccm_system_config.vfr_mode;

      if ( mvm_ccm_is_not_internal_topology( mvm_ccm_system_config.tx_topology_id ) )
      {
        /* Replace with equivalent internal topologies */
        if( 1 == mvm_ccm_system_config.tx_num_channels )
        {
          use_case.tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_TX_SM_ECNS;
        }
        else if ( 2 == mvm_ccm_system_config.tx_num_channels )
        {
          use_case.tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_TX_DM_FLUENCEV5;
        }
        else
        {
          use_case.tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_TX_QM_FLUENCE_PROV2;
        }
      }

      if ( mvm_ccm_is_not_internal_topology( mvm_ccm_system_config.rx_topology_id ) )
      {
        /* Replace with equivalent internal topologies */
        use_case.rx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_RX_DEFAULT;
      }

      /* Update the bw request params array. */
      clock_and_bus_preempt_margin_config.bw_requirement.numOfBw = 1;
      clock_and_bus_preempt_margin_config.bw_requirement.pBandWidthArray= &bw;

      rc = cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config( &use_case, &clock_and_bus_preempt_margin_config );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "mvm_ccm_commit_system_config(): Cannot find clock and bus "
               "configuration for usecase rc = 0x%08X", rc );
        /* incase of internal topology which is not matching to any valid clock use case in devcfg,
        clock is computed using MPPS and scale factor. For internal topologies, scale factor is set to 1.0
        */
        if ( ( !( mvm_ccm_is_not_internal_topology( mvm_ccm_system_config.tx_topology_id ) ) &&
             !( mvm_ccm_is_not_internal_topology( mvm_ccm_system_config.rx_topology_id ) ) ) )
        {
          /* Update clock voting for internal topology without valid entry in devcfg*/
          mvm_ccm_system_config.tx_mpps_scale_factor = MVM_CCM_DEFAULT_MPPS_INTERNAL_SCALE_FACTOR;
          mvm_ccm_system_config.rx_mpps_scale_factor = MVM_CCM_DEFAULT_MPPS_INTERNAL_SCALE_FACTOR;

          /* Compute clock voting based on mpps and scale factors */
          rc = mvm_ccm_compute_topo_clock_and_bw(&mvm_ccm_system_config, &clock_and_bus_preempt_margin_config );
          if ( rc )
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_ccm_commit_system_config(): Cannot compute clock and bus "
             "configuration for internal topology usecase rc = 0x%08X", rc );
            break;
          }
        }
      }

      if ( ( ( mvm_ccm_is_not_internal_topology( mvm_ccm_system_config.tx_topology_id ) ) ||
             ( mvm_ccm_is_not_internal_topology( mvm_ccm_system_config.rx_topology_id ) ) ) )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"custom topology");
        /* incase of external topology whether use case defintion matches or not to any valid clock use case in devcfg,
        floor clock is computed using use case MPPS and scale factor (provided by ACDB or default value 2.0).
        */
        if (mvm_ccm_system_config.tx_mpps_scale_factor == 0)
        {
            mvm_ccm_system_config.tx_mpps_scale_factor = MVM_CCM_DEFAULT_MPPS_EXTERNAL_SCALE_FACTOR;
        }
        if (mvm_ccm_system_config.rx_mpps_scale_factor == 0)
        {
            mvm_ccm_system_config.rx_mpps_scale_factor = MVM_CCM_DEFAULT_MPPS_EXTERNAL_SCALE_FACTOR;
        }

        /* Compute clock voting based on mpps and scale factors */
        rc = mvm_ccm_compute_topo_clock_and_bw(&mvm_ccm_system_config, &clock_and_bus_preempt_margin_config );
        if ( rc )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "mvm_ccm_commit_system_config(): Cannot compute clock and bus "
               "configuration for custom topology usecase rc = 0x%08X", rc );
          break;
        }
      }
    }

    preemption_safety_margins.vptx_preemption_safety_margin = clock_and_bus_preempt_margin_config.vptx_preemption_safety_margin;
    preemption_safety_margins.vprx_preemption_safety_margin = clock_and_bus_preempt_margin_config.vprx_preemption_safety_margin;
    preemption_safety_margins.venc_preemption_safety_margin = clock_and_bus_preempt_margin_config.venc_preemption_safety_margin;
    preemption_safety_margins.vdec_preemption_safety_margin = clock_and_bus_preempt_margin_config.vdec_preemption_safety_margin;

    /* Request resources from MMPM. */
    total_million_packet_per_second =
      ( ( mvm_ccm_system_config.total_kpps + 1000 - 1 ) / 1000 );

    mvm_ccm_system_config.total_core_kpps =
          ( clock_and_bus_preempt_margin_config.core_floor_clock_hz /
              ( clock_and_bus_preempt_margin_config.cycles_per_thousand_instr_packets ) );
    mpps_request.mppsTotal = ( total_million_packet_per_second );
    
    ( void ) mvm_ccm_calculate_processing_times(&clock_and_bus_preempt_margin_config);
    mpps_request.adspFloorClock = ( clock_and_bus_preempt_margin_config.core_floor_clock_hz / 1000000 );
    
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "mvm_ccm_commit_system_config(): num_clock_levels = %d",mvm_ccm_system_config.num_clk_perf_levels);

    for ( i = 0; i < mvm_ccm_system_config.num_clk_perf_levels; i++)
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "mvm_ccm_commit_system_config(): "
           "CLOCK_NPA_QUERY_PERF_LEVEL_KHZ[%d] = %d", i, mvm_ccm_system_config.clk_perf_level[i] );
    }

    rc = mvm_ccm_mmpm_request( &mpps_request, &clock_and_bus_preempt_margin_config.bw_requirement,
                               clock_and_bus_preempt_margin_config.sleep_latency_us );
    if ( rc ) break;

#endif /* WINSIM */

    break;
  }

  return rc;
}

APR_INTERNAL int32_t mvm_ccm_update_voice_session_config (
 uint16_t mvm_session_handle,
 vss_iccm_mvm_voice_session_update_t *in_args
)
{
  int32_t rc = APR_EOK;
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj = NULL;

  for ( ;; )
  {
    rc = mvm_ccm_get_active_mvm_session_tracking_obj( mvm_session_handle,
                                                      &session_tracking_obj );
    MVM_CCM_PANIC_ON_ERROR( rc );

    /* Configuration changes on an existing active session. 
     * Remove the existing session configurations from the system.
     */
    rc = mvm_ccm_update_system_config( session_tracking_obj, FALSE );
    MVM_CCM_PANIC_ON_ERROR( rc );


    /* Update this session's configurations. */
    session_tracking_obj->num_nb_streams = in_args->num_nb_streams;
    session_tracking_obj->num_wb_streams = in_args->num_wb_streams;
    session_tracking_obj->num_swb_streams = in_args->num_swb_streams;
    session_tracking_obj->num_fb_plus_streams = in_args->num_fb_plus_streams;
    session_tracking_obj->num_nb_vocprocs = in_args->num_nb_vocprocs;
    session_tracking_obj->num_wb_vocprocs = in_args->num_wb_vocprocs;
    session_tracking_obj->num_swb_vocprocs = in_args->num_swb_vocprocs;
    session_tracking_obj->num_fb_plus_vocprocs = in_args->num_fb_plus_vocprocs;
    session_tracking_obj->total_kpps = in_args->total_kpps;
    session_tracking_obj->enc_kpps = in_args->enc_kpps;
    session_tracking_obj->dec_kpps = in_args->dec_kpps;
    session_tracking_obj->dec_pp_kpps = in_args->dec_pp_kpps;
    session_tracking_obj->vp_rx_kpps = in_args->vp_rx_kpps;
    session_tracking_obj->vp_tx_kpps = in_args->vp_tx_kpps;
    session_tracking_obj->tx_afe_port_sr = in_args->tx_afe_port_sr;
    session_tracking_obj->tx_topology_id = in_args->tx_topology_id;
    session_tracking_obj->rx_topology_id = in_args->rx_topology_id;
    session_tracking_obj->media_id = in_args->media_id;
    session_tracking_obj->vfr_mode = in_args->vfr_mode;
    session_tracking_obj->tx_num_channels = in_args->tx_num_channels;
    session_tracking_obj->tx_mpps_scale_factor = in_args->tx_mpps_scale_factor;
    session_tracking_obj->tx_bw_scale_factor = in_args->tx_bw_scale_factor;
    session_tracking_obj->rx_mpps_scale_factor = in_args->rx_mpps_scale_factor;
    session_tracking_obj->rx_bw_scale_factor = in_args->rx_bw_scale_factor;
    
    rc = mvm_ccm_update_system_config( session_tracking_obj, TRUE );
    MVM_CCM_PANIC_ON_ERROR( rc );

    /* Request clock as per system configuration. */
    mvm_ccm_commit_system_config( );
    mvm_ccm_broadcast_configuration( );

    return APR_EOK;
  }

  return rc;
}

APR_INTERNAL int32_t mvm_csm_update_voice_session_config (
 uint16_t mvm_session_handle,
 vss_icsm_mvm_voice_config_update_t *in_args
)
{
  int32_t rc = APR_EOK;
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj = NULL;

  for ( ;; )
  {
    rc = mvm_ccm_get_active_mvm_session_tracking_obj( mvm_session_handle,
                                                      &session_tracking_obj );
    MVM_CCM_PANIC_ON_ERROR( rc );
   
    /* Update this session's configurations. */
    session_tracking_obj->voice_timing = in_args->voice_timing;
    session_tracking_obj->processing_times = in_args->processing_times;
    session_tracking_obj->vfr_info = in_args->vfr_info;
    session_tracking_obj->vptx_frame_size = in_args->vptx_frame_size;

    rc = mvm_csm_update_system_config( session_tracking_obj );
    MVM_CCM_PANIC_ON_ERROR( rc );
    break;
  }
  return APR_EOK;
}

APR_INTERNAL int32_t mvm_ccm_add_voice_session (
 uint16_t mvm_session_handle
)
{
  int32_t rc = APR_EOK;
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj = NULL;


  rc = mvm_ccm_get_active_mvm_session_tracking_obj( mvm_session_handle,
                                                    &session_tracking_obj );
  if ( APR_ENOTEXIST == rc )
  { /* A new session is becoming active. */
    rc = mvm_ccm_create_active_mvm_session_tracking_obj( &session_tracking_obj );
    MVM_CCM_PANIC_ON_ERROR( rc );

    session_tracking_obj->handle = mvm_session_handle;
    mvm_ccm_system_config.num_voice_sessions += 1;

    rc = mvm_ccm_queue_active_mvm_session_tracking_obj( session_tracking_obj );
    MVM_CCM_PANIC_ON_ERROR( rc );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "mvm_ccm_add_voice_session(): MVM:(0x%08X) successfully added",
           mvm_session_handle );
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "mvm_ccm_add_voice_session(): MVM:(0x%08X) ALREADY added ",
           mvm_session_handle );
  }

  return rc;
}

APR_INTERNAL int32_t mvm_ccm_remove_voice_session (
 uint16_t mvm_session_handle
)
{
  int32_t rc = APR_EOK;
  mvm_ccm_active_mvm_session_tracking_obj_t* session_tracking_obj = NULL;

  for ( ;; )
  {
    rc = mvm_ccm_get_active_mvm_session_tracking_obj( mvm_session_handle,
                                                      &session_tracking_obj );
    if ( rc == APR_ENOTEXIST )
    {
      MVM_CCM_PANIC_ON_ERROR( rc );
      break;
    }

    rc = mvm_ccm_dequeue_active_mvm_session_tracking_obj( session_tracking_obj );
    MVM_CCM_PANIC_ON_ERROR( rc );

    mvm_ccm_system_config.num_voice_sessions -= 1;
    mvm_ccm_update_system_config( session_tracking_obj, FALSE );

   /* Request clock as per system configuration. */
    mvm_ccm_commit_system_config( );
    mvm_ccm_broadcast_configuration( );

    rc = mvm_ccm_destroy_active_mvm_session_tracking_obj( session_tracking_obj );
    MVM_CCM_PANIC_ON_ERROR( rc );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "mvm_ccm_remove_voice_session(): MVM:(0x%08X)", mvm_session_handle );

    break;
  }

  return rc;
}

static void mvm_csm_enable_timer_cb( )
{
   if( APR_EOK != mvm_csm_task_lock_fn() )
   {
      return;
   }
   mvm_csm_config.enable = TRUE;
   mvm_csm_config.is_active = TRUE;
   mvm_csm_config.is_tx_active = TRUE;
   mvm_csm_config.is_rx_active = TRUE; 

   mvm_csm_task_unlock_fn();
}

static void mvm_csm_tx_timer_cb( )
{
   int32_t rc = APR_EOK;
   vss_imvm_csm_cmd_request_vote_t req_vote;

   req_vote.client = MVM_CSM_CLIENT_ENUM_VPTX;
   req_vote.vote = TRUE;
   {
      rc = __aprv2_cmd_alloc_send(
               mvm_ccm_mvm_service_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               mvm_ccm_mvm_service_apr_addr, APR_NULL_V,
               mvm_ccm_mvm_service_apr_addr, APR_NULL_V,
               0, VSS_IMVM_CSM_CMD_REQUEST_VOTE,
               &req_vote, sizeof(vss_imvm_csm_cmd_request_vote_t) );
      MVM_CCM_PANIC_ON_ERROR( rc );
   }
}

static void mvm_csm_rx_timer_cb( )
{
   int32_t rc = APR_EOK;
   vss_imvm_csm_cmd_request_vote_t req_vote;

   req_vote.client = MVM_CSM_CLIENT_ENUM_VDEC;
   req_vote.vote = TRUE;
   {
      rc = __aprv2_cmd_alloc_send(
               mvm_ccm_mvm_service_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               mvm_ccm_mvm_service_apr_addr, APR_NULL_V,
               mvm_ccm_mvm_service_apr_addr, APR_NULL_V,
               0, VSS_IMVM_CSM_CMD_REQUEST_VOTE,
               &req_vote, sizeof(vss_imvm_csm_cmd_request_vote_t) );
      MVM_CCM_PANIC_ON_ERROR( rc );
   }
}

bool_t mvm_csm_evaluate_sleep_possibility( void )
{
   bool_t rsp_status = TRUE;
   mvm_csm_client_enum_t client_id;
   vocsvc_avtimer_timestamp_t time;
  
   for ( ;; ) 
   {
      ( void )vocsvc_avtimer_get_time( &time );

      /* Check if any of the dynamic threads are active. 
       */
      if ( ( mvm_csm_config.is_tx_active ) || ( mvm_csm_config.tx_wakeup < ( mvm_csm_config.threshold_us + time.timestamp_us ) ) )
      {
         rsp_status = FALSE;
         break;
      }
      if ( ( mvm_csm_config.is_rx_active ) || ( mvm_csm_config.rx_wakeup < ( mvm_csm_config.threshold_us + time.timestamp_us ) ) )
      {
         rsp_status = FALSE;
         break;
      }

      /* Check if any of the static threads are active.
       */
      for ( client_id = MVM_CSM_CLIENT_ENUM_VSM; client_id < MVM_CSM_CLIENT_ENUM_MAX; client_id++ ) 
      {
         if ( TRUE == mvm_csm_config.client_info[ client_id ] )
         {
            rsp_status = FALSE;
            break;
         }
      }
      break;
   }
   return rsp_status;
}

bool_t mvm_csm_dl_sleep_manager()
{
  vocsvc_avtimer_timestamp_t time;
  bool_t rc = TRUE;

  for ( ;; ) 
  {
    ( void )vocsvc_avtimer_get_time( &time );

    if ( mvm_csm_config.rx_wakeup < time.timestamp_us ) 
    {
      mvm_csm_config.rx_wakeup += mvm_csm_config.vfr_cycle_us; 
    }

    if ( mvm_csm_config.rx_wakeup > ( mvm_csm_config.threshold_us + time.timestamp_us ) )
    {  
      mvm_csm_config.is_rx_active = FALSE;
      ( void )apr_timer_start ( mvm_csm_rx_timer,
                              ( mvm_csm_config.rx_wakeup - time.timestamp_us - mvm_csm_config.safety_margin_us ) * 1000 );
      rc = FALSE;
      break;
    }
    break;
  }
  return rc;
}

bool_t mvm_csm_ul_sleep_manager( void )
{
  vocsvc_avtimer_timestamp_t time;
  bool_t rc = TRUE;
  
  vocsvc_avtimer_get_time( &time );
  for ( ;; ) 
  {
    
     if( mvm_csm_config.enc_pkt_count < ( mvm_csm_config.num_venc_instances - 1 ) )
     {
        rc = TRUE;
        break;
     }
     else if ( ( mvm_csm_config.enc_pkt_count == ( mvm_csm_config.num_venc_instances - 1 ) ) && ( mvm_csm_config.vptx_frame_size == MVM_VOICE_FRAME_SIZE_US ) )
     {
        mvm_csm_config.tx_wakeup = mvm_csm_config.last_vptx_start;

        //Check if enough time gap available from the next VpTx start.
        if ( mvm_csm_config.tx_wakeup > ( mvm_csm_config.threshold_us + time.timestamp_us ) )
        {
           mvm_csm_config.is_tx_active = FALSE;
           apr_timer_start ( mvm_csm_tx_timer,
                             ( mvm_csm_config.tx_wakeup - time.timestamp_us - mvm_csm_config.safety_margin_us ) * 1000 );
           rc = FALSE;
           break;
        }
     }
     else if ( mvm_csm_config.enc_pkt_count == mvm_csm_config.num_venc_instances )
     {
        mvm_csm_config.enc_pkt_count = 0;
        mvm_csm_config.first_vptx_start += mvm_csm_config.vfr_cycle_us;
        mvm_csm_config.tx_wakeup = mvm_csm_config.first_vptx_start;
        mvm_csm_config.last_vptx_start += mvm_csm_config.vfr_cycle_us;

        //Check if enough time gap available from the next VpTx start.
        if ( mvm_csm_config.tx_wakeup > ( mvm_csm_config.threshold_us + time.timestamp_us ) )
        {  
            mvm_csm_config.is_tx_active = FALSE;
            apr_timer_start ( mvm_csm_tx_timer,
                                ( mvm_csm_config.tx_wakeup - time.timestamp_us - mvm_csm_config.safety_margin_us ) * 1000 );
            rc = FALSE;
            break;
        }
     }
     break;
  }
  return rc;
}

static bool_t mvm_csm_are_multiple_sessions_active()
{
   bool_t rc = FALSE;
   if ( ( mvm_ccm_system_config.num_voice_sessions > 1 ) ||
        ( mvm_ccm_system_config.num_fb_plus_streams > 1 ) ||
        ( mvm_ccm_system_config.num_fb_plus_vocprocs > 1 ) ||
        ( mvm_ccm_system_config.num_nb_streams > 1 ) ||
        ( mvm_ccm_system_config.num_nb_vocprocs > 1 ) ||
        ( mvm_ccm_system_config.num_swb_streams > 1 ) ||
        ( mvm_ccm_system_config.num_swb_vocprocs > 1 ) ||
        ( mvm_ccm_system_config.num_wb_streams > 1 ) ||
        ( mvm_ccm_system_config.num_wb_vocprocs > 1 ) )
   {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                  "mvm_csm_update_sleep_manager(): multiple sessions detected");
      rc = TRUE;
   }
   return rc;
}

void mvm_csm_update_sleep_manager( bool_t enable )
{
   mvm_voice_timing_t* voice_timing = NULL;
   mvm_voice_processing_delays_t* processing_times = NULL;

   MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "mvm_csm_update_sleep_manager(): CSM_enable_flag: %d," \
          "voting_devoting_latency_us: %d, safety_margin: %d", 
          enable, mvm_csm_config.threshold_us, mvm_csm_config.safety_margin_us );

   if( APR_EOK != mvm_csm_task_lock_fn() )
   {
      return;
   }

   /** If CSM is ACTIVE, stop active timers if any and disable it. */
   if ( TRUE == mvm_csm_config.enable ) 
   {
      apr_timer_stop( mvm_csm_enable_timer );
      apr_timer_stop( mvm_csm_tx_timer );
      apr_timer_stop( mvm_csm_rx_timer );

      mvm_csm_config.enable = FALSE;
      mvm_csm_config.enc_pkt_count = 0;
      if ( FALSE == mvm_csm_config.is_active ) 
      {
         mvm_ccm_commit_system_config();
      }
      mvm_csm_config.is_active = FALSE;
   }

   voice_timing = &mvm_csm_config.voice_timing; 
   processing_times = &mvm_csm_config.processing_times;

   /* Enable CSM based off the new timing offsets. */
   if ( ( TRUE == enable ) && ( NULL != voice_timing ) && ( NULL != processing_times ) ) 
   {
      if ( ( processing_times->venc_proc >= MVM_VOICE_FRAME_SIZE_US ) ||
           ( processing_times->vdec_proc >= MVM_VOICE_FRAME_SIZE_US ) ||
           ( processing_times->vptx_proc >= MVM_VOICE_FRAME_SIZE_US ) ||
           ( processing_times->vprx_proc >= MVM_VOICE_FRAME_SIZE_US ) ||
           ( mvm_csm_config.vfr_cycle_us == MVM_VOICE_FRAME_SIZE_US ) ||
             mvm_csm_are_multiple_sessions_active() )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "mvm_csm_update_sleep_manager(): Dynamic voting-devoting is disabled for the current configuration");
      }
      else
      {
        uint64_t vfrTimestamp;
        vocsvc_avtimer_timestamp_t time;
            
        ( void )vocsvc_avtimer_get_time( &time );
        cvd_vfr_get_voice_frame_reference( &vfrTimestamp );

        mvm_csm_config.vptx_start_offset = voice_timing->vptx_proc_start_offset;
        mvm_csm_config.vdec_req_offset = voice_timing->dec_req_offset;
           
        if ( voice_timing->vptx_proc_start_offset > voice_timing->vptx_buf_start_offset ) 
        {
           mvm_csm_config.samples_at_vptx_start = voice_timing->vptx_proc_start_offset - voice_timing->vptx_buf_start_offset;
        }
        else
        {
           mvm_csm_config.samples_at_vptx_start = mvm_csm_config.vfr_cycle_us - ( voice_timing->vptx_buf_start_offset - voice_timing->vptx_proc_start_offset );
        }
        /* Current and the next VFR cycle are bypassed while calculating
           the absolute wake up TS of VpTx and Vdec threads. This is done
           to avoid corner cases that could arise with various timing offset values.
         */
        mvm_csm_config.rx_wakeup = vfrTimestamp + voice_timing->dec_req_offset + mvm_csm_config.vfr_cycle_us * 2; 
        mvm_csm_config.first_vptx_start = vfrTimestamp + voice_timing->vptx_proc_start_offset + mvm_csm_config.vfr_cycle_us * 2; 
        mvm_csm_config.tx_wakeup = mvm_csm_config.first_vptx_start; 
        mvm_csm_config.last_vptx_start = mvm_csm_config.tx_wakeup + mvm_csm_config.vfr_cycle_us - mvm_csm_config.samples_at_vptx_start;
     
        if ( mvm_csm_config.tx_wakeup > mvm_csm_config.rx_wakeup ) 
        {
           /* CSM enable is always aligned with Tx wakeup TS.
              Rx wakeup should not occur before CSM is enabled,
              hence moving Rx wake up to the next VFR cycle.
            */
           mvm_csm_config.rx_wakeup += mvm_csm_config.vfr_cycle_us; 
        }

        /* Setup Timer for enabling CSM at the Tx wakeup TS. */
        ( void ) apr_timer_start ( mvm_csm_enable_timer, 
                                   ( mvm_csm_config.tx_wakeup - time.timestamp_us ) * 1000 );
      }
   }
   mvm_csm_task_unlock_fn();
}

APR_INTERNAL int32_t mvm_csm_cmd_request_vote( 
   vss_imvm_csm_cmd_request_vote_t* req
)
{
   bool_t is_vote_needed = FALSE;

   if( APR_EOK != mvm_csm_task_lock_fn() )
   {
      return APR_EOK;
   }
   for ( ;; ) 
   {
      if ( FALSE == mvm_csm_config.enable ) 
      {
        break;
      }
      
      switch ( req->client ) 
      {
         case MVM_CSM_CLIENT_ENUM_VPTX:
         {
           mvm_csm_config.is_tx_active = TRUE;
           is_vote_needed = TRUE;
         }
         break;

         case MVM_CSM_CLIENT_ENUM_VENC:
         {
           mvm_csm_config.enc_pkt_count++;
           is_vote_needed = mvm_csm_ul_sleep_manager();
         }
         break;

         case MVM_CSM_CLIENT_ENUM_VDEC:
         {
           mvm_csm_config.is_rx_active = TRUE;
           is_vote_needed = TRUE;
         }
         break;

         case MVM_CSM_CLIENT_ENUM_VPRX:
         {
           is_vote_needed = mvm_csm_dl_sleep_manager();
         }
         break;

         case MVM_CSM_CLIENT_ENUM_VSM:
         case MVM_CSM_CLIENT_ENUM_VPM:
         case MVM_CSM_CLIENT_ENUM_MVM:
         {
           mvm_csm_config.client_info[ req->client ] = req->vote;
           is_vote_needed = req->vote;
         }
         break;

         default:
            break;
      }
      
      if ( is_vote_needed && !mvm_csm_config.is_active ) 
      {
         mvm_csm_config.is_active = TRUE;
         mvm_ccm_commit_system_config();
      }
      else if ( !is_vote_needed && mvm_csm_config.is_active && mvm_csm_evaluate_sleep_possibility() ) 
      {
         mvm_csm_config.is_active = FALSE;
         ( void ) mvm_ccm_mmpm_release( );
      }
      break;
   }
   mvm_csm_task_unlock_fn();
   return APR_EOK;
}

APR_INTERNAL int32_t mvm_ccm_init (
  uint32_t mvm_service_apr_handle,
  uint16_t mvm_service_apr_addr
)
{
  uint32_t session_count;
  uint32_t rc = APR_EOK;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "====== mvm_ccm_init()======" );

  { /* Initialize the custom heap. */
    apr_memmgr_init_heap(
      &mvm_ccm_heapmgr, ( ( void* ) &mvm_ccm_heap_pool ),
      sizeof( mvm_ccm_heap_pool ), NULL, NULL );
      /**< Note that locking is not required since the custom heap is always
           used in the MVM TASK context. */
  }

  { /* Initialize the active MVM session tracking lists. */
    ( void ) apr_list_init_v2(
               &mvm_ccm_active_mvm_session_tracking_free_q, NULL, NULL );
      /**< Note that locking is not required since the list is always used in
           the MVM TASK context. */

    for ( session_count = 0; session_count < MVM_CCM_MAX_ACTIVE_MVM_SESSIONS_V; ++session_count )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &mvm_ccm_active_mvm_session_tracking_list_item_pool[ session_count ] );
      ( void ) apr_list_add_tail(
                 &mvm_ccm_active_mvm_session_tracking_free_q,
                 ( ( apr_list_node_t* )
                   &mvm_ccm_active_mvm_session_tracking_list_item_pool[ session_count ] ) );
    }

    ( void ) apr_list_init_v2( &mvm_ccm_active_mvm_session_tracking_used_q,
                               NULL, NULL );
      /**< Note that locking is not required since the list is always used in
           the MVM TASK context. */
  }

  { /* Initialize the system configuration. */
    uint32_t i;
    mvm_ccm_system_config.num_voice_sessions = 0;
    mvm_ccm_system_config.num_nb_streams = 0;
    mvm_ccm_system_config.num_wb_streams = 0;
    mvm_ccm_system_config.num_swb_streams = 0;
    mvm_ccm_system_config.num_fb_plus_streams = 0;
    mvm_ccm_system_config.num_nb_vocprocs = 0;
    mvm_ccm_system_config.num_wb_vocprocs = 0;
    mvm_ccm_system_config.num_swb_vocprocs = 0;
    mvm_ccm_system_config.num_fb_plus_vocprocs = 0;
    mvm_ccm_system_config.enc_kpps = 0;
    mvm_ccm_system_config.dec_kpps = 0;
    mvm_ccm_system_config.dec_pp_kpps = 0;
    mvm_ccm_system_config.vp_rx_kpps = 0;
    mvm_ccm_system_config.vp_tx_kpps = 0;
    mvm_ccm_system_config.total_kpps = 0;
    mvm_ccm_system_config.total_core_kpps = 0;
    mvm_ccm_system_config.tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
    mvm_ccm_system_config.rx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
    mvm_ccm_system_config.media_id = VSS_MEDIA_ID_NONE;
    mvm_ccm_system_config.vfr_mode = VSS_ICOMMON_VFR_MODE_SOFT;
    mvm_ccm_system_config.num_clk_perf_levels = 0;
    for ( i = 0; i < MVM_CCM_MAX_NUM_CLK_PERF_LEVELS; i++)
    {
      mvm_ccm_system_config.clk_perf_level[i] = 0;
    }
    mvm_ccm_system_config.tx_mpps_scale_factor = 0;
    mvm_ccm_system_config.tx_bw_scale_factor = 0;
    mvm_ccm_system_config.rx_mpps_scale_factor = 0;
    mvm_ccm_system_config.rx_bw_scale_factor = 0;
  }

  { /* Store the MVM service APR handle and address. */
    mvm_ccm_mvm_service_apr_handle = mvm_service_apr_handle;
    mvm_ccm_mvm_service_apr_addr = mvm_service_apr_addr;
  }

  {
     /* Initialize the lock. */
     ( void ) apr_lock_create( APR_LOCK_TYPE_MUTEX, &mvm_csm_task_lock );
     
     /* Initialize CSM config and timers. */
     ( void ) mmstd_memset( &mvm_csm_config, 0, sizeof( mvm_csm_config ) );
     ( void ) cvd_devcfg_parser_get_csm_sleep_threshold( &mvm_csm_config.threshold_us );
     ( void ) cvd_devcfg_parser_get_csm_safety_margin( &mvm_csm_config.safety_margin_us );
    
     rc = apr_timer_create( &mvm_csm_tx_timer,
                            mvm_csm_tx_timer_cb, NULL );
     rc |= apr_timer_create( &mvm_csm_rx_timer,
                            mvm_csm_rx_timer_cb, NULL );
     rc |= apr_timer_create( &mvm_csm_enable_timer,
                            mvm_csm_enable_timer_cb, NULL );
     if ( rc )
     {
       MSG_ERROR ( "mvm_ccm_init():Failed to create CSM timers "
                   "with rc=(0x%08x)", rc, 0, 0 );
     }
  }

#ifndef WINSIM
  {  /* Register with MMPM. */
    ( void ) mvm_ccm_mmpm_register( );

    ( void ) mvm_ccm_get_clock_level_info();
  }
#endif /* !WINSIM */

  return APR_EOK;
}

APR_INTERNAL int32_t mvm_ccm_deinit ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "====== mvm_ccm_deinit()======" );

#ifndef WINSIM

  /* Deregister with MMPM. */
  ( void ) mvm_ccm_mmpm_deregister( );

#endif /* !WINSIM */

  { /* Release the active MVM session tracking lists. */
    ( void ) apr_list_destroy( &mvm_ccm_active_mvm_session_tracking_used_q );
    ( void ) apr_list_destroy( &mvm_ccm_active_mvm_session_tracking_free_q );
  }
  ( void ) apr_lock_destroy( mvm_csm_task_lock );
  mvm_csm_task_lock = NULL;
  ( void ) apr_timer_destroy ( mvm_csm_tx_timer );
  ( void ) apr_timer_destroy ( mvm_csm_rx_timer );
  ( void ) apr_timer_destroy ( mvm_csm_enable_timer );
  return APR_EOK;
}


#ifndef GLINK_TRACER_INTERNAL
#define GLINK_TRACER_INTERNAL
/**
 * @file tracer_packet.h
 *
 * API for Tracer Packet only to be used inside G-Link
 * This is used in order to measure IPC transmit performance
 */

/*==============================================================================
     Copyright (c) 2015 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/15   bc      Initial version for tracer packet
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
  
/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/
#define GLINK_PROFILE_TRACER_PKT_CH_ID    ( 0 )
#define GLINK_PROFILE_TRACER_PKT_ID       ( 1 )
#define GLINK_PROFILE_TRACER_PKT_PRIV_LEN ( 8 ) /* Two words necessary */

//#define GLINK_MEMCPY_PROFILE_ENABLED
#define GLINK_TX_MEMCPY_EVENT             ( 7 )
#define GLINK_RX_MEMCPY_EVENT             ( 8 )
#define GLINK_RX_INTERRUPT                ( 9 )
  
/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
typedef enum
{
  TX,
  RX,
  INVALID_DIR
}glink_tracer_cache_dir_type;
  
/*===========================================================================
                      GLINK_TRACER_INTERAL EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      tracer_packet_cache_rx_timetick
===========================================================================*/
/** 
 * Cache timetick in RX side. This must be only called in rx side
 * Cached timetick will be used to capture exact rx complete time
 *
 * @param[in]  event_id     event_id to cache
 * 
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
void tracer_packet_cache_rx_timetick
(
  uint32   event_id
);

/*===========================================================================
FUNCTION      tracer_packet_cache_tx_timetick
===========================================================================*/
/** 
 * Cache timetick in TX side. This must be only called in tx side
 * Cached timetick will be used to capture exact tx complete time
 *
 * @param[in]  tracer_pkt   tracer packet. this is used to get packet priv info
 * @param[in]  event_id     event_id to cache
 * 
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
void tracer_packet_cache_tx_timetick
(
  void    *tracer_pkt,
  uint32   event_id
);

/*===========================================================================
FUNCTION      tracer_packet_get_final_pkt
===========================================================================*/
/** 
 * Get tracer packet that is finally merged with cached packet.
 * Cached packet contains accurate timetick for GLINK_XPRT_TX
 * 
 * @param[in/out]  tracer_pkt   tracer_pkt
 * @param[in]      cache_dir    timetick cache direction 
 * 
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
void tracer_packet_get_final_pkt
(
  void                        *tracer_pkt,
  glink_tracer_cache_dir_type  cache_dir
);

/*===========================================================================
FUNCTION      tracer_packet_log_timestamp
===========================================================================*/
/** 
 * Log tracer packet event to ulog
 * 
 * @param[in]  tracer_pkt   tracer_pkt 
 * 
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
void tracer_packet_log_timestamp
(
  void     *tracer_pkt
);

#ifdef __cplusplus
}
#endif

#endif //GLINK_TRACER_INTERNAL_H

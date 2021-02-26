#ifndef GLINK_TRACER_H
#define GLINK_TRACER_H

/**
 * @file tracer_packet.h
 *
 * Public API for Tracer Packet
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
08/31/15   an      Add getters to packet tracer
02/04/15   bc      Initial version for tracer packet
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "stddef.h"

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/
  
/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
typedef enum 
{
  TRACER_PKT_STATUS_SUCCESS,
  TRACER_PKT_STATUS_FAILURE,
  TRACER_PKT_NULL_PKT,
  TRACER_PKT_INVALID_PKT_SIZE,
  TRACER_PKT_OUT_OF_EVENT_SPACE
}tracer_pkt_result_type;

typedef struct tracer_pkt_init_config
{
  /* Specify whether to use qdss tracing instead of qtimer */
  boolean qdss_trace_enable;
  
  /* Client defined event configuration */
  uint16  client_event_cfg;
  
  /* Glink event configuration */
  uint32  glink_event_cfg;
  
  /* Client Cookie (private data), maximum 3 words */
  uint32  pkt_priv[3];
  
  /* Length of client cookie, maximum 3 words */
  uint32  pkt_priv_len;

}tracer_pkt_init_config_type;

/* Trace Event for G-Link 
 * Each value represents bit position */
typedef enum
{
  /* When Glink tx/txv is called */
  GLINK_CORE_TX= 1,
  
  /* When packet is scheduled for tx */
  GLINK_QUEUE_TO_SCHEDULER = 2,
  
  /* When Glink Tx thread start transfer the scheduled packet */
  GLINK_SCHEDULER_TX = 3,
  
  /* When transfer is done */
  GLINK_XPRT_TX = 4,
  
  /* When remote side received the packet */
  GLINK_XPRT_RX = 5,
  
  /* Right before notifying client of the packet */
  GLINK_CORE_RX = 6
}tracer_pkt_glink_event;

/*===========================================================================
                      TRACER PACKET PUBLIC API
===========================================================================*/
/*===========================================================================
FUNCTION      tracer_packet_init
===========================================================================*/
/** 
 * Initializes tracer packet. 
 *
 * @param[in/out]  tracer_pkt       Packet to trace. User is supposed to allocate
 *                                  the buffer with the size they choose to trace.
 *                                  Allocated buffer will be initialized
 *                                  by this function. The size needs to be greater
 *                                  than TRACER_PKT_MIN_SIZE  which is
 *                                  defined in glink_tracer.c(36 byte) if user
 *                                  plan to use qtimer tracing. 
 * 
 * @param[in]      pkt_len          length of tracer packet
 * 
 * @param[in]      init_config      tracer packet initialization config.
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_init
(
  void                        *tracer_pkt,
  uint32                       packet_len,
  tracer_pkt_init_config_type *init_config
);

/*===========================================================================
FUNCTION      tracer_packet_set_event_cfg
===========================================================================*/
/**
 * Sets the trace event configuration in case user wants to trace assymetrical
 * events. 
 * For example, user may want to trace n number of events for transmission yet
 * m number of events for receive.
 * 
 * @param[in/out]  tracer_pkt       Packet to trace. 
 *                                  Packet must be initialized. 
 * 
 * @param[in]      client_event_cfg configuration for client defined event. 
 *                                  0 if not needed
 * 
 * @param[in]      glink_event_cfg  configuration for IPC event. 
 *                                  For ex. glink 
 *                                  (currently only glink supports tracer pkt)
 *                                  0 if not needed
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_set_event_cfg
(
  void    *tracer_pkt,
  uint16   client_event_cfg,
  uint32   glink_event_cfg
);

/*===========================================================================
FUNCTION      tracer_packet_log_event
===========================================================================*/
/**
 * Log the time stamp for the event given
 * 
 * @param[in/out]  tracer_pkt  Packet to trace. 
 *                             Packet must be initialized. 
 
 * @param[in]      event_id    event id to trace
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_log_event
(
  void    *tracer_pkt,
  uint16   event_id
);

/*===========================================================================
FUNCTION      tracer_packet_calc_hex_dump_size
===========================================================================*/
/**
 * Calculate the size of traced packet in hexadecimal format
 * User can use return value to allocate buffer to save this dump
 * 
 * @param[in/out]  tracer_pkt  Packet traced
 
 * @param[in]      pkt_len     length of packet
 *
 * @return         size of tracer packet in hex format
 *
 * @sideeffects    None
 */
/*=========================================================================*/
uint32 tracer_packet_calc_hex_dump_size
(
  void    *tracer_pkt,
  uint32   pkt_len
);

/*===========================================================================
FUNCTION      tracer_packet_calc_hex_dump_size
===========================================================================*/
/**
 * Dump the traced packet into given buffer. The buffer will be in hex format
 * 
 * @param[in/out]  dest_buf        Buffer to dump to
 * 
 * @param[in]      dest_buf_len    length of dest_buf
 * 
 * @param[in/out]  traced_pkt      Packet traced
 
 * @param[in]      traced_pkt_len  length of traced packet
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_hex_dump
(
  void    *dest_buf,
  uint32   dest_buf_len,
  void    *traced_pkt,
  uint32   traced_pkt_len
);

/*===========================================================================
FUNCTION      tracer_packet_get_timestamp
===========================================================================*/
/**
 * This function extracts timestamp for the given event
 * 
 * @param[in]      traced_pkt      Packet traced
 * @param[in]      traced_pkt_len  length of traced packet
 * @param[in]      event_id        Event id
 * @param[out]     timestamp       Pointer to timestamp memory
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_get_timestamp
(
  void    *traced_pkt,
  uint32   traced_pkt_len,
  uint16   event_id,
  uint64   *timestamp
);

/*===========================================================================
FUNCTION      tracer_packet_get_pkt_priv
===========================================================================*/
/**
 * This function extracts private data the given tracer packet
 * 
 * @param[in]      traced_pkt      Packet traced
 * @param[in]      traced_pkt_len  length of traced packet
 * @param[in]      index           Index of pkt_priv (must be less than 3)
 * @param[out]     value           Pointer to data memory
 *
 * @return         Standard tracer packet error code
 *
 * @sideeffects    None
 */
/*=========================================================================*/
tracer_pkt_result_type tracer_packet_get_pkt_priv
(
  void    *traced_pkt,
  uint32   traced_pkt_len,
  uint32   index,
  uint32   *value
);

#ifdef __cplusplus
}
#endif

#endif //GLINK_TRACER_H

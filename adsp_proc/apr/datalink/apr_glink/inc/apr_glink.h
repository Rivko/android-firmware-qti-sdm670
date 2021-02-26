#ifndef _APR_GLINK_H_
#define _APR_GLINK_H_
/*=============================================================================
  Copyright (C) 2015,2016,2018-2019 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

===============================================================================
                                 Edit History

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia2/apr/apr2/mainline/main/latest/datalink/apr_glink/inc/apr_glink.h
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ------------------------------------------------------------
08/08/2015   rs    Initial version.
=============================================================================*/

/******************************************************************************
 * Includes                                                                   *
 *****************************************************************************/
#include "apr_comdef.h"
#include "glink.h"

/******************************************************************************
 * Defines                                                                    *
 *****************************************************************************/

#define APR_GLINK_MAX_CHANNEL_NAME_SIZE     ( 16 )
  /**< Max channel name size allowed. */

#define APR_GLINK_APPS_REMOTE_SS_ID         ( 0x00011318 )
  /**< Apps remote subsystem ID. */

#define APR_GLINK_MODEM_REMOTE_SS_ID        ( 0x00011319 )
  /**< Modem remote subsystem ID. */

#define APR_GLINK_QDSP_REMOTE_SS_ID         ( 0x0001131A )
  /**< QDSP remote subsystem ID. */
  
#define APR_GLINK_SDSP_REMOTE_SS_ID         ( 0x0001131B )
  /**< CDSP remote subsystem ID. */
  
#define APR_GLINK_CDSP_REMOTE_SS_ID         ( 0x0001131C )
  /**< CDSP remote subsystem ID. */

/*TODO: Assign guids */
#define APR_GLINK_DISCONNECTED            ( 0 )
  /**< APR GLink connection is disconnected. */

#define APR_GLINK_CONNECTED               ( 1 )
  /**< APR GLink connection is fully connected with remote proc. */

#define APR_GLINK_REMOTE_DISCONNECTED     ( 2 )
  /**< APR GLink connection is disconnected by remote proc. */

#define APR_GLINK_CONNECTING              ( 3 )
  /**< APR GLink connection is connecting. Wainting for ack from glink. */

#define APR_GLINK_DISCONNECTING           ( 4 )
  /**< APR GLink connection is disconnecting. Wainting for ack from glink. */

/******************************************************************************
 * Type Definitions                                                           *
 *****************************************************************************/

typedef uint32_t apr_glink_port_handle_t;
  /**< Client communicates with APR GLINK using a handle of this type. */
void aprv2_ipc_glink_link_state_cb (  glink_link_info_type *link_info,void *priv);
typedef int32_t ( *apr_glink_rx_cb_t ) ( void *cb_data,
                                         void *rx_read_buf, uint32_t size );
  /**<
   * The rx callback function type definition.
   *
   * The client receives this callback when there is data available for read.
   * The rx_read_buf contains the data to be read. The rx_read_buf is valid
   * outside of this callback function as well. The client is expected to call
   * apr_glink_rx_done() when done with using the buffer.
   *
   * The callback is processed in ISR context. Client shall perform minimal
   * amount of processing in the callback and absolutely no call blocking
   * routines in the callback.
   *
   * \param[in] cb_data The client supplied data pointer when opening a port.
   * \param[in] rx_read_buf The buffer to be read.
   * \param[in] size The size of the buffer in bytes.
   *
   * \return APR_EOK on success, APR error code on failure.
   */

typedef int32_t ( *apr_glink_tx_done_cb_t ) ( void *cb_data,
                                       void *rx_write_buf,  uint32_t size );
  /**<
   * The tx done callback function type definition.
   *
   * The client receives this callback when APR GLINK is done with the tx
   * buffer.
   *
   * \param[in] cb_data The client supplied data pointer when opening a port.
   * \param[in] rx_write_buf The buffer supplied during write.
   * \param[in] size The size of the buffer in bytes.
   *
   * \return APR_EOK on success, APR error code on failure.
   */

typedef int32_t ( *apr_glink_state_cb_t ) (void *cb_data, uint32_t event );

  /**<
   * APR Glink state notification callback function type definition.
   *
   * The client receives the notification when APR GLINK connection state has
   * changed.
   *
   * \param[in] cb_data The client supplied data pointer when opening a port.
   * \param[in] event The connection state change event.
   *
   * \return APR_EOK on success, APR error code on failure.
   */

/******************************************************************************
 * Core Routines                                                              *
 *****************************************************************************/

/**
 * Initializes the APR GLINK module.
 *
 * This function must be called before any other APR GLINK functions are
 * called.
 * This function must only be called once.
 *
 * \return APR_EOK on success, APR error code on failure.
 */
APR_INTERNAL int32_t apr_glink_init ( void  );
APR_INTERNAL int32_t aprv2_ipc_rx_cb (
  apr_glink_port_handle_t port_handle,
  void* rx_read_buf,
  uint32_t size);
/**
 * Deinitializes the APR GLINK module.
 *
 * After a call to this function, none of the other APR GLINK functions (except
 * apr_glink_init) can be called.
 * This function must only be called once.
 *
 * \return APR_EOK on success, APR error code on failure.
 */
APR_INTERNAL int32_t apr_glink_deinit ( void );

/**
 * Opens the port name port_name with the remote processor specified by
 * remote_ssid.
 * 
 * \param[in] port_name The name for the port. The max size of the name allowed
 *                      is defined by #APR_GLINK_MAX_CHANNEL_NAME_SIZE.
 *
 * \param[in] port_name_size The size of the port name including the NULL
 *                           termination character.
 *
 * \param[in] remote_ssid The remote subsystem id identifying remote
 *                        processors to which this channel communicates.
 *                        Supported values:
 *                        #APR_GLINK_APPS_REMOTE_SS_ID
 *                        #APR_GLINK_MODEM_REMOTE_SS_ID
 *                        #APR_GLINK_QDSP_REMOTE_SS_ID
 *
 * \param[in] rx_cb The rx callback function defined by #apr_glink_rx_cb_t. The
 *                  client receives this callback when there is data available
 *                  for read.
 *
 * \param[in] rx_cb_data The client data pointer for the rx callback.
 *
 * \param[in] tx_done_cb The tx done callback function defined by
 *                       #apr_glink_tx_done_cb_t. The client receives this
 *                       callback when APR GLINK is done with a tx buffer.
 *
 * \param[in] tx_done_cb_data The client data pointer for tx_done callback.
 *
 * \param[in] num_intents The number of intents to be queued for this channel.
 *
 * \param[in] intent_size Size of each intent to be queued.
 *
 * \param[in] qos_latency_us QoS latency requirement specified in microseconds
 *                           for given packet size thru qos_pkt_size.
 *
 * \param[in] qos_pkt_size Packet size for which qos_latency_us is specified.
 *
 * \param[out] port_handle Positive APR GLINK port handle on success. NULL on
 *                         failure.
 *
 * \return APR_EOK on success, APR error code on failure.
 */
APR_INTERNAL int32_t apr_glink_open (
   char_t *port_name,
  uint32_t port_name_size,
  uint32_t remote_ssid,
  apr_glink_port_handle_t port_handle,
   char_t *remote_ss
);

/**
 * Close a port specified by the port_handle.
 *
 * \param[in] port_handle The handle of the port to be closed.
 *
 * \return APR_EOK on success, APR error code on failure.
 */
APR_INTERNAL int32_t apr_glink_close ( apr_glink_port_handle_t port_handle );

/**
 * Send data to the specified APR GLINK port.
 *
 * \param[in] port_handle The handle of the port to write data.
 * \param[in] buf The data to be written.
 * \param[in] size The size of the data in bytes.
 *
 * \return APR_EOK on success, APR error code on failure.
 */
APR_INTERNAL int32_t apr_glink_tx (
  apr_glink_port_handle_t port_handle,
  void *buf,
  uint32_t size
);

/**
 * Indicate APR GLINK that a previously given Rx buffer is done.
 *
 * \param[in] port_handle The handle of the port for which rx buffer is done.
 * \param[in] buf The rx buffer pointer for which rx is done.
 *
 * \return APR_EOK on succes, APR error code on failure.
 */
APR_INTERNAL int32_t apr_glink_rx_done (
  apr_glink_port_handle_t port_handle,
  void *buf
);

#endif /* _APR_GLINK_H_ */

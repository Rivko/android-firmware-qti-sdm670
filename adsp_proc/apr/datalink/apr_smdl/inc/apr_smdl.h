#ifndef _APR_SMDL_H_
#define _APR_SMDL_H_

/*
  Copyright (C) 2010-2012,2016 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/datalink/apr_smdl/inc/apr_smdl.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

/****************************************************************************
 * Defines                                                                  *
 ****************************************************************************/

#define APR_SMDL_MIN_FIFO ( 8 * 1024 )
  /**< Size of a small FIFO buffer (8 KB). */

#define APR_SMDL_STANDARD_FIFO ( 8 * 1024 )
  /**< Default size of a FIFO buffer (8 KB). */

#define APR_SMDL_MAX_FIFO ( 64 * 1024 )
  /**< Size of a large FIFO buffer (64 KB). This is the largest size permitted. */

#define APR_SMDL_CHANNEL_NAME_SIZE_MAX ( 16 )
  /**< Max size of the channel name. */

#define APR_SMDL_APPS_MODEM_CHANNEL_ID ( 0x00011318 )
  /**< Apps to modem channel ID. */

#define APR_SMDL_APPS_QDSP_CHANNEL_ID ( 0x00011319 )
  /**< Apps to QDSP channel ID. */

#define APR_SMDL_MODEM_QDSP_CHANNEL_ID ( 0x0001131A )
  /**< Modem to QDSP channel ID. */

/****************************************************************************
 * Definitions                                                              *
 ****************************************************************************/

typedef uint32_t apr_smdl_port_handle_t;
  /**< Client communicates with APR SMDL using a handle of this type. */

typedef int32_t ( *apr_smdl_rx_cb_t ) ( void* cb_data, void* rx_read_buf,
                                        uint32_t size );
  /**<
   * The rx callback function type definition.
   *
   * The client receives this callback when there is data available for read.
   * The rx_read_buf contains the data to be read. The rx_read_buf is only valid
   * within this callback function.
   *
   * The callback is processed in ISR context. Client shall perform minimal amount 
   * of processing in the callback and absolutely no call blocking routines in the 
   * callback.
   *
   * \param[in] cb_data The client supplied data pointer when opening a port.
   *
   * \param[in] rx_read_buf The buffer to be read.
   *
   * \param[in] size The size of the buffer in bytes.
   *
   * \return APR_EOK on success, APR error code on failure.
   */

/****************************************************************************
 * Core Routines                                                            *
 ****************************************************************************/

/**
 * Initializes the APR SMDL module.
 *
 * This function must be called before any other APR SMDL functions are called.
 * This function must only be called once.
 *
 * \return APR_EOK on success, APR error code on failure.
 */
APR_INTERNAL int32_t apr_smdl_init ( void );

/**
 * Deinitializes the APR SMDL module.
 *
 * After a call to this function, none of the other APR SMDL functions
 * (except apr_smdl_init) can be called. This function must only be
 * called once.
 *
 * \return APR_EOK on success, APR error code on failure.
 */
APR_INTERNAL int32_t apr_smdl_deinit ( void );

/**
 * Opens the port named port_name between the processors specified channel_type.
 * (The current processor must be one of the two processors connected by
 * channel_type.).
 *
 * \param[in] port_name The name for the port. The max size of the name allowed is
 *                      defined by #APR_SMDL_CHANNEL_NAME_SIZE_MAX.
 *
 * \param[in] port_name_size The size of the port name.
 *
 * \param[in] channel_id The connection channel type identifying both processors,
 *                       on which this channel communicates. The channel_type must 
 *                       include the current processor.
 *                       Supported values: 
 *                       #APR_SMDL_APPS_MODEM_CHANNEL_ID
 *                       #APR_SMDL_APPS_QDSP_CHANNEL_ID
 *                       #APR_SMDL_MODEM_QDSP_CHANNEL_ID
 *
 * \param[in] rx_cb The rx callback function defined by #apr_smdl_rx_cb_t.The client
 *                  receives this callback when there is data available for read.
 *
 * \param[in] rx_cb_data The client supplied data pointer for the rx callback.
 *
 * \param[in] fsize The size of the FIFO. This is the physical
 *                  size in memory, which is always one byte greater than
 *                  the total capacity of the FIFO. The SMD packet
 *                  header adds 20 bytes per packet, further reducing
 *                  the capacity. The FIFO size must be a multiple of 32
 *                  and must not be less than APR_SMDL_MIN_FIFO or
 *                  greater than APR_SMDL_MAX_FIFO. If this parameter is 0, the
 *                  default size( APR_SMDL_STANDARD_FIFO ) will be used.
 *                  It is recommended to set the FIFO to default size.fsize 
 *                  needs to be the same on both processors.
 *
 * \param[out] port_handle Positive APR SMDL port handle on success. NULL on failure.
 *
 * \return APR_EOK on success, APR error code on failure.
 */
APR_INTERNAL int32_t apr_smdl_open (
  char_t* port_name,
  uint32_t port_name_size,
  uint32_t channel_id,
  apr_smdl_rx_cb_t rx_cb,
  void* rx_cb_data,
  uint32_t fsize,
  apr_smdl_port_handle_t* port_handle
);

/**
 * Close a port specified by the port_handle.
 *
 * \param[in] port_handle The handle of the port to be closed.
 *
 * \return APR_EOK on success, APR error code on failure.
 */
APR_INTERNAL int32_t apr_smdl_close (
  apr_smdl_port_handle_t port_handle
);

/**
 * Write data to the specified APR SMDL port.
 *
 * \param[in] port_handle The handle of the port to write data.
 * \param[in] buf The data to be written.
 * \param[in] size The size of the data in bytes.
 *
 * \return APR_EOK on success, APR error code on failure.
 */
APR_INTERNAL int32_t apr_smdl_write (
  apr_smdl_port_handle_t port_handle,
  void* buf,
  uint32_t size
);

#endif /*_APR_SMDL_H_*/


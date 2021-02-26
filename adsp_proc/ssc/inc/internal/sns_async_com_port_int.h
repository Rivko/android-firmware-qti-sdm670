#pragma once
/**
 * sns_async_com_port_int.h
 *
 * Asynchronous Communication Port (ASCP) API to access COM 
 * buses. This is an internal header file used by ASCP Sensor 
 * and bus specific implementations. 
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdint.h>
#include <stdlib.h>
#include "sns_com_port_types.h"
#include "sns_rc.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"

/**
 * Com port version struct.
 */
typedef struct sns_async_com_port_version
{
  uint16_t   major;
  uint16_t   minor;
} sns_async_com_port_version;

/**
*  Callback that will be called when an asynchronous read/write operation is
*  completed.
*
*  @param[i] port_handle      Port handle for the bus.
*  @param[i] num_vectors      Number of Vectors
*  @param[i] vectors          Vector for read/write operation.
*  @param[i] xfer_bytes       Total bytes actually read/written.
*  @param[i] rw_err           Error code for the transaction
*  @param[i] user_args        User defined parameter.
*/
typedef void (*sns_ascp_callback)(void  *port_handle,
                                  uint8_t num_vectors,
                                  sns_port_vector vectors[], //Warning: Elements in array are not of same size
                                  uint32_t xfer_bytes,
                                  sns_rc rw_err,
                                  void *user_args);

typedef struct sns_ascp_port_api
{

  size_t struct_len;

  /**
  * The ASCP is a subclass of the SCP. The SCP API provides the utilities to
  * open and close a port and control bus power rails. The ASCP API adds the
  * abilities to register for a callback and provides the asynchronous register
  * read operation only.
  */
  sns_sync_com_port_service_api *sync_com_port;

  /**
  * Get the version of the Synchronous COM Port API.
  *
  * @param[o] version         version of this API
  *
  * @return
  * SNS_RC_INVALID_VALUE - version parameter is NULL
  * SNS_RC_SUCCESS - action succeeded.
  */
  sns_rc (*sns_ascp_get_version)(sns_sync_com_port_version *version);

  /**
  * Register for a callback to be notified of a completed asynchronous
  * register read operation.
  *
  * @param[i] port_handle         Port handle for the bus.
  * @param[i] callback            Callback to be registered
  * @param[i] args                Callback function parameters
  */
  sns_rc (*sns_ascp_register_callback)(void  *port_handle,
                                       sns_ascp_callback callback,
                                       void* args);

  /**
  *  Read register asynchronusly.
  *
  *  This is a vectored API. It can be used for to read  a
  *  single register
  *
  *  This operation is valid only on a port that has been
  *  successfully opened using sns_scp_open().
  *
  *  @param[i] port_handle      Port handle for the bus.
  *  @param[i] num_vectors      Number of vectors
  *  @param[i] vectors          A single register read
  *                             operations.
  *  @param[i] save_write_time  true if time of the bus write
  *                             transaction must be saved. If
  *                             there are multiple write vectors
  *                             then this will only save the time
  *                             of the last write vector.
  *
  *  @return
  *  SNS_RC_FAILED - unable to transfer over bus.
  *  SNS_RC_SUCCESS - action succeeded
  */
  sns_rc (*sns_ascp_register_rw)(void  *port_handle,
                                uint8_t num_vectors,
                                sns_port_vector vectors[],
                                bool save_write_time);

  /**
  *  Gets the timestamp for most recent write operation.
  *
  *  This API must be called only when save_write_time is 'true'
  *  for the most recent write operation on the bus. It is
  *  typically used for S4S mode of operation to get the start
  *  time when the ST command is sent to the physical sensor. For
  *  I2C and I3C, the timestamp is when the START condition occurs
  *  for the write transfer. For SPI, the timestamp is an instance
  *  during the write transfer.
  *  This operation is valid only on a port that has been
  *  successfully opened using sns_ascp_open().
  *
  *  @param[i] port_handle    Port handle for the bus.
  *  @param[o] write_time     System timestamp in (ticks * 32).
  *
  *  @return
  *  SNS_RC_FAILED - unable to get write time.
  *  SNS_RC_SUCCESS - action succeeded.
  *
  * Note:
  *   The returned write_time is in units of System Ticks * 32.
  *   This value should be divided by 32 when comparing to the
  *   current system time.
  */
  sns_rc (*sns_ascp_get_write_time)(void  *port_handle,
                                    sns_time *write_time);


} sns_ascp_port_api;


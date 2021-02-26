#pragma once
/*=============================================================================
  @file sns_sync_com_port_service.h

  Manages sync com port service.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include <stdlib.h>
#include "sns_com_port_types.h"
#include "sns_rc.h"
#include "sns_sensor_uid.h"
#include "sns_service.h"
#include "sns_time.h"


/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor;
struct sns_sensor_instance;


/*=============================================================================
  Type Definitions
  ===========================================================================*/

//TODO To be cleaned  in drivers later
typedef void * sns_sync_com_port_handle;

/**
 * The Sync Com Port Service Manager.  Will be obtained from sns_service_manager::get_service.
 */
typedef struct sns_sync_com_port_service
{
  /* Service information */
  sns_service service;

  /* Public api provided by the framework to be used by the sensor. */
  struct sns_sync_com_port_service_api *api;
} sns_sync_com_port_service;

/**
 * Com port version struct.
 */
typedef struct sns_sync_com_port_version
{
  uint16_t   major;
  uint16_t   minor;
} sns_sync_com_port_version;



typedef struct sns_sync_com_port_service_api
{

  size_t struct_len;

  /**
  *  Registers a bus COM port with the Synchronous COM Port (SCP)
  *  utility.
  *
  *  Each physical sensor shall register with the SCP before any
  *  SCP API can be used. Note that this does not open a COM port.
  *
  *  @param[i] com_config    COM port config for the bus.
  *  @param[o] port_handle   Port handle for the bus.
  *
  *  @return
  *  SNS_RC_INVALID_VALUE - input parameters are invalid.
  *  SNS_RC_FAILED - com port registration failed.
  *  SNS_RC_SUCCESS - action succeeded.
  */
  sns_rc (*sns_scp_register_com_port)(sns_com_port_config const *com_config,
                                      sns_sync_com_port_handle **port_handle);

  /**
  *  Deregisters a bus COM port with the Synchronous COM Port (SCP)
  *  utility and sets the port handle to NULL.
  *  This will power off and close the com port, if powered on
  *  or open.
  *
  *  @param[i] port_handle   Reference to the port handle.
  *
  *  @return
  *  SNS_RC_INVALID_VALUE - Passed in port handle is invalid.
  *  SNS_RC_SUCCESS -       Port handle is freed.
  */
  sns_rc (*sns_scp_deregister_com_port)(sns_sync_com_port_handle **port_handle);

  /**
  * Get the version of the Synchronous COM Port API.
  *
  * @param[o] version         version of this API
  *
  * @return
  * SNS_RC_INVALID_VALUE - version parameter is NULL
  * SNS_RC_SUCCESS - action succeeded.
  */
  sns_rc (*sns_scp_get_version)(sns_sync_com_port_version *version);

  /**
  *  Opens a new COM port with bus configuration com_config.
  *
  *  The bus is powered ON after this function returns
  *  successfully. Each physical sensor driver that needs access
  *  to a communication bus (I2C/SPI/I3C/UART) shall use this API
  *  typically in initialization routine. The physical sensor
  *  driver must save port handle for future use of the bus.
  *
  *  @param[i] port_handle   Port handle for the bus.
  *
  *  @return
  *  SNS_RC_INVALID_VALUE - requested bus and/or port handle are invalid.
  *  SNS_RC_FAILED - unable to open underlying bus.
  *  SNS_RC_SUCCESS- action succeeded or already open
  *   */
  sns_rc (*sns_scp_open)(sns_sync_com_port_handle *port_handle);

  /**
  *  Closes and powers off the COM port.
  *
  *  The physical sensor driver would typically close the port when
  *  it's being un-installed or unloaded. This operation is valid
  *  only on a port that has been successfully opened using
  *  sns_scp_open(). Once a bus is closed, it shall not be
  *  accessed by the driver.
  *
  *  @param[i] port_handle  Port handle for the bus.
  *
  *  @return
  *  SNS_RC_INVALID_VALUE - passed in port handle is invalid.
  *  SNS_RC_FAILED - unable to close underlying bus.
  *  SNS_RC_SUCCESS - action succeeded or already closed
  */
  sns_rc (*sns_scp_close)(sns_sync_com_port_handle *port_handle);

  /**
  *  Updates bus power status to ON or OFF.
  *
  *  Typically, a physical sensor driver shall turn bus power ON
  *  before initiating a series of COM transfers. It shall turn
  *  bus power off after to save power. A COM port cannot be used
  *  unless it is in power ON state. It is required for all device
  *  drivers to turn the bus OFF when not in use.
  *  This operation is valid only on a port that has been
  *  successfully opened using sns_scp_open().
  *
  *  @param[i] port_handle    Port handle for the bus.
  *  @param[i] power_bus_on   true to power ON.
  *                           false to power OFF.
  *  @return
  *  SNS_RC_INVALID_VALUE - passed in port handle is invalid.
  *  SNS_RC_FAILED - unable to update bus power.
  *  SNS_RC_SUCCESS- action succeeded or no update needed
  */
  sns_rc (*sns_scp_update_bus_power)(sns_sync_com_port_handle *port_handle,
                                     bool power_bus_on);

  /**
  *  Read and/or write multiple registers.
  *
  *  This is a vectored API. It can be used for:
  *  1) Read single/multiple registers in the same
  *     transfer.
  *  2) Write to single/multiple register in same transfer.
  *  3) Write to and read from single/multiple registers in same
  *     transfer.
  *
  *  This operation is valid only on a port that has been
  *  successfully opened using sns_scp_open().
  *
  *  @param[i] port_handle      Port handle for the bus.
  *  @param[i] vectors          An array of register read/write
  *                             operations.
  *  @param[i] num_vectors      Number of elements in vectors
  *                             array.
  *  @param[i] save_write_time  true if time of the bus write
  *                             transaction must be saved. If
  *                             there are multiple write vectors
  *                             then this will only save the time
  *                             of the last write vector.
  *  @param[o] xfer_bytes       Total number of bytes read &
  *                             written for all registers in this
  *                             vectored transfer. May be NULL.
  *
  *  @return
  *  SNS_RC_INVALID_VALUE - passed in port handle is invalid.
  *  SNS_RC_FAILED - unable to transfer over bus.
  *  SNS_RC_SUCCESS - action succeeded
  */
  sns_rc (*sns_scp_register_rw)(sns_sync_com_port_handle *port_handle,
                                sns_port_vector *vectors,
                                int32_t num_vectors,
                                bool save_write_time,
                                uint32_t *xfer_bytes);

  /**
  *  Read/write operation directly to the slave.
  *  This operation is valid only on a port that has been
  *  successfully opened using sns_scp_open().
  *
  *  @param[i] port_handle      Port handle
  *  @param[i] is_write         true for write operation else
  *                             false.
  *  @param[i] save_write_time  true if time of the bus write
  *                             transaction must be saved.
  *  @param[io] buffer          Buffer for read/write operation.
  *  @param[i] bytes            Number of bytes to read/write
  *  @param[o] xfer_bytes       Total bytes actually read/written.
  *
  *  @return
  *  SNS_RC_INVALID_VALUE - passed in port handle is invalid.
  *  SNS_RC_FAILED - unable to transfer over bus.
  *  SNS_RC_NOT_AVAILABLE - action not supported
  *  SNS_RC_SUCCESS - action succeeded
  */
  sns_rc (*sns_scp_simple_rw)(sns_sync_com_port_handle *port_handle,
                              bool is_write,
                              bool save_write_time,
                              uint8_t *buffer,
                              uint32_t bytes,
                              uint32_t *xfer_bytes);

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
  *  successfully opened using sns_scp_open().
  *
  *  @param[i] port_handle    Port handle for the bus.
  *  @param[o] write_time     System timestamp in ticks.
  *
  *  @return
  *  SNS_RC_INVALID_VALUE - passed in port handle is invalid.
  *  SNS_RC_FAILED - unable to get write time.
  *  SNS_RC_SUCCESS - action succeeded.
  *
  */
  sns_rc (*sns_scp_get_write_time)(sns_sync_com_port_handle *port_handle,
                                   sns_time *write_time);


} sns_sync_com_port_service_api;

/**
 * Com port handle struct definition.
 *
 * Note: Clients shall not alter the handle.
 */
/*typedef struct sns_sync_com_port_handle
{
  size_t                     struct_len;
  struct sns_sync_com_port_api   *com_port_api;
} sns_sync_com_port_handle;*/


#pragma once
/**
 * sns_com_port_priv.h
 *
 * Private definitions used by synch_com_port (SCP) and
 * asynch_com_port (ACP).
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdbool.h>
#include <stdint.h>
#include "sns_async_com_port_int.h"
#include "sns_com_port_types.h"
#include "sns_rc.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"


/**------------------------------- I3C BUS ---------------------------------*/
// TODO: move I3C to bus specific files when we have QuPv3 drivers
typedef struct sns_i3c_info
{
  //TODO
} sns_i3c_info;


/**------------------------------ BUS HANDLER ------------------------------*/
typedef struct sns_bus_info
{
  sns_bus_type              bus_type;
  /** bus_config is allocated by bus specific implementation.*/
  uint8_t                  *bus_config;
  bool                      power_on:1;
  bool                      opened:1;
} sns_bus_info;

typedef struct sns_com_port_vector_xfer_map
{
  sns_port_vector *orig_vector_ptr;
  uint8_t        **xfer_buffer_ptrs;// memory allocated for reg+data for each vector
  uint8_t          num_vectors;
} sns_com_port_vector_xfer_map;

typedef struct sns_com_port_priv_handle
{
  sns_sync_com_port_handle        port_handle;
  sns_com_port_config             com_config;
  sns_bus_info                    bus_info;
  sns_ascp_callback               callback;
  sns_com_port_vector_xfer_map    xfer_map;
  void *                          args;
} sns_com_port_priv_handle;


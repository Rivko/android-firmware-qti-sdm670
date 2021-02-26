/**
 * sns_sync_com_port_service.c
 *
 * The Synchronous COM Port Service implementation
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "sns_com_port_types.h"

#include "sns_assert.h"
#include "sns_com_port_priv.h"
#include "sns_fw_sync_com_port_service.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_printf_int.h"
#include "sns_sync_com_port_service.h"

/** Version:
 */
static sns_sync_com_port_version scp_version SNS_SECTION(".data.sns")=
{
  .major = 1,
  .minor = 0
};

static sns_sync_com_port_service sync_com_port_service SNS_SECTION(".data.sns");
/**
 * Private state definition.
 */
/*
struct sns_fw_sync_com_port_service
{
  sns_sync_com_port_service service;
} ;
*/
/** Vtables for bus specific API.
 */
extern sns_sync_com_port_service_api i2c_port_api;
extern sns_sync_com_port_service_api spi_port_api;


/** Interface functions for supported bus ports.
 *
 *  IMPORTANT: This list must match enum definitions in
 *  sns_bus_type.
 */
sns_sync_com_port_service_api *scp_port_apis[] SNS_SECTION(".data.sns")=
{
  &i2c_port_api,  // SNS_BUS_I2C
  &spi_port_api,  // SNS_BUS_SPI
  NULL,       //placeholder for SNS_BUS_UART &uart_port,
  NULL        //placeholder for SNS_BUS_I3C &i3c_port
};

SNS_SECTION(".text.sns") sns_rc sns_scp_open(sns_sync_com_port_handle *port_handle)
{
  if (port_handle == NULL)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "null port_handle");
    //SNS_ASSERT(NULL != port_handle);
    return SNS_RC_INVALID_VALUE;
  }
  // If the SCP state was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)port_handle))
  {
    SNS_ISLAND_EXIT();
  }
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_rc return_code = SNS_RC_SUCCESS;

  if(bus_info->bus_type < SNS_BUS_MIN
      || bus_info->bus_type > SNS_BUS_MAX)
  {
    return SNS_RC_INVALID_VALUE;
  }

  if(bus_info->opened == true)
  {
    return SNS_RC_SUCCESS;
  }

  return_code = scp_port_apis[bus_info->bus_type]->sns_scp_open(port_handle);

  if(return_code != SNS_RC_SUCCESS)
  {
    bus_info->opened = false;
  }
  else
  {
    bus_info->opened   = true;
    bus_info->power_on = true;
  }

  return return_code;
}

SNS_SECTION(".text.sns") sns_rc sns_scp_update_bus_power(sns_sync_com_port_handle *port_handle,
                                bool power_bus_on)
{
  if (port_handle == NULL)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "null port_handle");
    //SNS_ASSERT(NULL != port_handle);
    return SNS_RC_INVALID_VALUE;
  }
  // If the SCP state was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)port_handle))
  {
    SNS_ISLAND_EXIT();
  }
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_rc return_code = SNS_RC_FAILED;

  if(bus_info->opened == true)
  {
    if(bus_info->power_on != power_bus_on)
    {
      return_code = scp_port_apis[bus_info->bus_type]->
        sns_scp_update_bus_power(port_handle, power_bus_on);
      if(return_code == SNS_RC_SUCCESS)
      {
        bus_info->power_on = power_bus_on;
      }
    }
    else
    {
      // Bus already in selected power state
      return_code = SNS_RC_SUCCESS;
    }
  }
  return return_code;
}

SNS_SECTION(".text.sns") sns_rc sns_scp_close(sns_sync_com_port_handle *port_handle)
{
  if (port_handle == NULL)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "null port_handle");
    //SNS_ASSERT(NULL != port_handle);
    return SNS_RC_INVALID_VALUE;
  }
  // If the SCP state was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)port_handle))
  {
    SNS_ISLAND_EXIT();
  }
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_rc return_code = SNS_RC_SUCCESS;

  if(bus_info->opened == true)
  {
    sns_scp_update_bus_power(port_handle, false);
    return_code = scp_port_apis[bus_info->bus_type]->sns_scp_close(port_handle);
    bus_info->opened = false;
  }

  return return_code;
}

SNS_SECTION(".text.sns") sns_rc sns_scp_register_rw(sns_sync_com_port_handle *port_handle,
                           sns_port_vector *vectors,
                           int32_t num_vectors,
                           bool save_write_time,
                           uint32_t *xfer_bytes)
{
  if (port_handle == NULL)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "null port_handle");
    //SNS_ASSERT(NULL != port_handle);
    return SNS_RC_INVALID_VALUE;
  }
  // If the SCP state was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)port_handle))
  {
    SNS_ISLAND_EXIT();
  }
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_rc return_code = SNS_RC_FAILED;

  if(bus_info->opened == true && bus_info->power_on == true)
  {
    return_code = scp_port_apis[bus_info->bus_type]->
      sns_scp_register_rw(port_handle,
                          vectors,
                          num_vectors,
                          save_write_time,
                          xfer_bytes);
  }

  return return_code;

}

SNS_SECTION(".text.sns") sns_rc sns_scp_simple_rw(sns_sync_com_port_handle *port_handle,
                         bool is_write,
                         bool save_write_time,
                         uint8_t *buffer,
                         uint32_t bytes,
                         uint32_t *xfer_bytes)
{
  if (port_handle == NULL)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "null port_handle");
    //SNS_ASSERT(NULL != port_handle);
    return SNS_RC_INVALID_VALUE;
  }
  // If the SCP state was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)port_handle))
  {
    SNS_ISLAND_EXIT();
  }
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_rc return_code = SNS_RC_FAILED;

  if(bus_info->opened == true && bus_info->power_on == true)
  {
    return_code = scp_port_apis[bus_info->bus_type]->
      sns_scp_simple_rw(port_handle,
                        is_write,
                        save_write_time,
                        buffer,
                        bytes,
                        xfer_bytes);
  }
  return return_code;
}

SNS_SECTION(".text.sns") sns_rc sns_scp_get_write_time(sns_sync_com_port_handle *port_handle,
                              sns_time *write_time)
{
  if (port_handle == NULL)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "null port_handle");
    //SNS_ASSERT(NULL != port_handle);
    return SNS_RC_INVALID_VALUE;
  }
  // If the SCP state was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)port_handle))
  {
    SNS_ISLAND_EXIT();
  }
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_rc return_code = SNS_RC_FAILED;

  if(bus_info->opened == true && bus_info->power_on == true)
  {
    if(write_time != NULL)
    {
      return_code = scp_port_apis[bus_info->bus_type]->
        sns_scp_get_write_time(port_handle, write_time);
    }
  }
  return return_code;
}

SNS_SECTION(".text.sns") sns_rc sns_scp_get_version(sns_sync_com_port_version *version)
{
  sns_rc return_code = SNS_RC_INVALID_VALUE;
  if(version != NULL)
  {
    sns_memscpy(version, sizeof(sns_sync_com_port_version), &scp_version,
                 sizeof(sns_sync_com_port_version));
    return_code = SNS_RC_SUCCESS;
  }

  return return_code;
}

SNS_SECTION(".text.sns") sns_rc sns_scp_deregister_com_port(sns_sync_com_port_handle **port_handle)
{
  if( (port_handle == NULL) || (*port_handle == NULL))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "null port_handle");
    return SNS_RC_INVALID_VALUE;
  }

  // If the SCP state was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)port_handle))
  {
    SNS_ISLAND_EXIT();
  }

  sns_scp_update_bus_power(*port_handle, false);
  sns_scp_close(*port_handle);
  sns_free(*port_handle);
  *port_handle = NULL;

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_scp_register_com_port(sns_com_port_config const *com_config,
                                 sns_sync_com_port_handle **port_handle)
{
  sns_com_port_priv_handle *port_priv_handle;

  if(com_config == NULL || port_handle == NULL)
  {
    return SNS_RC_INVALID_VALUE;
  }
  if(NULL != *port_handle)
  {
    sns_scp_deregister_com_port(port_handle);
  }

  port_priv_handle = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_com_port_priv_handle));
//  SNS_ASSERT(NULL != port_priv_handle);
  if (NULL == port_priv_handle)
  {
    // We ran out of island heap, so let's throw out a warning message and then
    // allocate the sync_com_port state in DDR. This isn't ideal for power, but
    // will help with stability.
    SNS_ISLAND_EXIT();
    SNS_PRINTF(HIGH, sns_fw_printf, "Alloc SCP state in DDR!");
    port_priv_handle = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_com_port_priv_handle));
    SNS_ASSERT(NULL != port_priv_handle); // We're in big trouble if we can't allocate the SCP state
  }

  sns_memscpy(&port_priv_handle->com_config,
               sizeof(sns_com_port_config),
               com_config,
               sizeof(sns_com_port_config));

  port_priv_handle->bus_info.bus_type = com_config->bus_type;
  port_priv_handle->bus_info.power_on = false;
  port_priv_handle->bus_info.opened   = false;
  /** bus_info::bus_config shall be allocated by bus specific
   *  implementation.
   */

  *port_handle = (sns_sync_com_port_handle *)port_priv_handle;

  return SNS_RC_SUCCESS;
}


sns_sync_com_port_service_api scp_port_service_api SNS_SECTION(".data.sns")=
{
  .struct_len                  = sizeof(sns_sync_com_port_service_api),
  .sns_scp_register_com_port   = &sns_scp_register_com_port,
  .sns_scp_deregister_com_port = &sns_scp_deregister_com_port,
  .sns_scp_open                = &sns_scp_open,
  .sns_scp_close               = &sns_scp_close,
  .sns_scp_update_bus_power    = &sns_scp_update_bus_power,
  .sns_scp_register_rw         = &sns_scp_register_rw,
  .sns_scp_simple_rw           = &sns_scp_simple_rw,
  .sns_scp_get_write_time      = &sns_scp_get_write_time,
  .sns_scp_get_version         = &sns_scp_get_version
};

sns_sync_com_port_service* sns_sync_com_port_service_init(void)
{

  sync_com_port_service.api = &scp_port_service_api;
  sync_com_port_service.service.type= SNS_SYNC_COM_PORT_SERVICE;
  sync_com_port_service.service.version = scp_version.major;

  return &sync_com_port_service;
}

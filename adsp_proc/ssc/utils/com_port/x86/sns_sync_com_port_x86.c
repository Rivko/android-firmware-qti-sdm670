/**
 * sns_sync_com_port.c
 *
 * The Synchronous COM Port Utility implementation
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "sns_assert.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_sync_com_port_service.h"
#include "sns_types.h"

#include "sns_fw_sync_com_port_service.h"
#include "sns_osa_lock.h"
//#include "sns_com_port_priv.h"
/** Version:
 */
static sns_sync_com_port_version scp_version =
{
  .major = 1,
  .minor = 0
};


static sns_sync_com_port_service sync_com_port_service ;

sns_rc sns_scp_open(sns_sync_com_port_handle *port_handle)
{
  UNUSED_VAR(port_handle);
  return SNS_RC_SUCCESS;
}

sns_rc sns_scp_close(sns_sync_com_port_handle *port_handle)
{
  sns_free(port_handle);
  return SNS_RC_SUCCESS;
}

sns_rc sns_scp_update_bus_power(sns_sync_com_port_handle *port_handle,
                                bool power_bus_on)
{
  UNUSED_VAR(port_handle);
  UNUSED_VAR(power_bus_on);
  return SNS_RC_SUCCESS;
}

sns_rc sns_scp_register_rw(sns_sync_com_port_handle *port_handle,
                           sns_port_vector *vectors,
                           int32_t num_vectors,
                           bool save_write_time,
                           uint32_t *xfer_bytes)
{
  UNUSED_VAR(port_handle);
  UNUSED_VAR(save_write_time);
  int i;
  *xfer_bytes = 0;
  for(i = 0; i < num_vectors; i++)
  {
    *xfer_bytes += vectors[i].bytes;
  }
  return SNS_RC_SUCCESS;
}

sns_rc sns_scp_simple_rw(sns_sync_com_port_handle *port_handle,
                         bool is_write,
                         bool save_write_time,
                         uint8_t *buffer,
                         uint32_t bytes,
                         uint32_t *xfer_bytes)
{
  UNUSED_VAR(port_handle);
  UNUSED_VAR(is_write);
  UNUSED_VAR(save_write_time);
  UNUSED_VAR(buffer);
  *xfer_bytes = bytes;
  return SNS_RC_SUCCESS;
}

sns_rc sns_scp_get_write_time(sns_sync_com_port_handle *port_handle,
                              sns_time *write_time)
{
  UNUSED_VAR(port_handle);
  *write_time = 0;
  return SNS_RC_SUCCESS;
}

sns_rc sns_scp_get_version(sns_sync_com_port_version *version)
{
  if(version)
  {
    sns_memscpy(version, sizeof(sns_sync_com_port_version), &scp_version,
                 sizeof(sns_sync_com_port_version));
    return SNS_RC_SUCCESS;
  }

  return SNS_RC_INVALID_VALUE;
}


sns_rc sns_scp_register_com_port(sns_com_port_config const *com_config,
                                 sns_sync_com_port_handle **port_handle)
{
  if(com_config == NULL || *port_handle == NULL)
  {
    return SNS_RC_INVALID_VALUE;
  }

  *port_handle = NULL;

  *port_handle = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_sync_com_port_handle));
  SNS_ASSERT(NULL != *port_handle);

  return SNS_RC_SUCCESS;
}
sns_sync_com_port_service_api scp_port_service_api =
{
  .struct_len                  = sizeof(sns_sync_com_port_service_api),
  .sns_scp_register_com_port   = &sns_scp_register_com_port,
  .sns_scp_deregister_com_port = NULL,
  .sns_scp_open                = &sns_scp_open,
  .sns_scp_close               = &sns_scp_close,
  .sns_scp_update_bus_power    = &sns_scp_update_bus_power,
  .sns_scp_register_rw         = &sns_scp_register_rw,
  .sns_scp_simple_rw           = &sns_scp_simple_rw,
  .sns_scp_get_write_time      = &sns_scp_get_write_time,
  .sns_scp_get_version         = &sns_scp_get_version
};


sns_sync_com_port_service * sns_sync_com_port_service_init(void)
{
  sync_com_port_service.api = &scp_port_service_api;
  sync_com_port_service.service.type= SNS_SYNC_COM_PORT_SERVICE;
  sync_com_port_service.service.version = scp_version.major;

  return &sync_com_port_service;
}


/**
 * sns_sync_com_port_i2c.c
 *
 * I2C implementation
 *
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "sns_assert.h"
#include "sns_async_com_port_int.h"
#include "sns_com_port_priv.h"
#include "sns_com_port_types.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_sem.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_types.h"

#include "i2c_api.h"
#include "qurt_atomic_ops.h"

#define MAX_I2C_DESCRIPTORS 4

/**--------------------------------------------------------------------------*/
#define SLAVE_MAX_CLOCK_STRETCH_US 0   //Maximum time in microseconds
                                       //that a slave may hold the SCL line low

/**--------------------------------------------------------------------------*/
typedef struct sns_i2c_info
{
  void         *i2c_handle;
  sns_time      txn_start_time;
  unsigned int  xfer_in_progress;
} sns_i2c_info;

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns") static void sns_i2c_callback( uint32 transfer_status, uint32 transferred, void *ctxt )
{
  if(!sns_island_is_island_ptr((intptr_t)ctxt))
  {
    SNS_ISLAND_EXIT();
  }
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)ctxt;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_i2c_info *i2c_info = (sns_i2c_info *)bus_info->bus_config;
  sns_com_port_config *com_config = &priv_handle->com_config;
  sns_com_port_vector_xfer_map *vec_map = &priv_handle->xfer_map;
  int num_vectors;
  int register_len = 0;
  sns_port_vector *vector_ptr;
  sns_rc rw_err = SNS_RC_FAILED;
  int i;

  if (!sns_island_is_island_ptr((intptr_t)i2c_info) ||
      !sns_island_is_island_ptr((intptr_t) vec_map)||
      !sns_island_is_island_ptr((intptr_t) vec_map->xfer_buffer_ptrs) ||
      !sns_island_is_island_ptr((intptr_t) vec_map->orig_vector_ptr))
  {
    SNS_ISLAND_EXIT();
  }
  vector_ptr = vec_map->orig_vector_ptr;
  num_vectors = vec_map->num_vectors;

  switch( com_config->reg_addr_type )
  {
    case SNS_REG_ADDR_16_BIT:
      register_len = 2;
      break;
    case SNS_REG_ADDR_32_BIT:
      register_len = 4;
      break;
    default:
    case SNS_REG_ADDR_8_BIT:
      register_len = 1;
      break;
  }

  if( I2C_SUCCESS(transfer_status) )
  {
    rw_err = SNS_RC_SUCCESS;
    for( i = 0; i < num_vectors; i++ )
    {
      if( !vec_map->orig_vector_ptr[i].is_write )
      {
        // For read operations, copy data from PRAM back to client buffer
        if( !sns_island_is_island_ptr((intptr_t)vector_ptr[i].buffer) )
        {
          SNS_ISLAND_EXIT();
        }
        sns_memscpy( vector_ptr[i].buffer, vector_ptr[i].bytes,
                     vec_map->xfer_buffer_ptrs[i]+register_len, vector_ptr[i].bytes );
      }
      sns_free( vec_map->xfer_buffer_ptrs[i] );
      transferred -= register_len;
    }
  }
  else
  {
    SNS_ISLAND_EXIT();
    SNS_PRINTF(ERROR, sns_fw_printf,
                 "i2c_transfer transfer_status 0x%X", transfer_status);
    for( i = 0; i < num_vectors; i++ )
    {
      sns_free( vec_map->xfer_buffer_ptrs[i] );
      transferred -= register_len;
    }
  }

  if( priv_handle->callback != NULL )
  {
    priv_handle->callback(priv_handle, num_vectors, vector_ptr, transferred,
                          rw_err, priv_handle->args);
  }

  sns_free( vec_map->xfer_buffer_ptrs );
  vec_map->xfer_buffer_ptrs = NULL;

  qurt_atomic_set( &i2c_info->xfer_in_progress, false );
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns") sns_rc sns_open_i2c(sns_sync_com_port_handle *port_handle)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_com_port_config *com_config = &priv_handle->com_config;
  i2c_status       result;
  sns_i2c_info    *i2c_info;

  if(com_config->bus_instance <= 0
      ||
      com_config->bus_instance > I2C_INSTANCE_MAX)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Invalid configuration");
    return SNS_RC_INVALID_VALUE;
  }

  SNS_ISLAND_EXIT();

  bus_info->bus_config = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_i2c_info));
//  SNS_ASSERT(NULL != bus_info->bus_config);
  if(NULL == bus_info->bus_config)
  {
    // We ran out of island heap, so let's throw out a warning message and then
    // allocate the i2c config in DDR. This isn't ideal for power, but
    // will help with stability.
    SNS_PRINTF(HIGH, sns_fw_printf, "Alloc I2C config in DDR!");
    bus_info->bus_config = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_i2c_info));
    SNS_ASSERT(NULL != bus_info->bus_config); // We're in big trouble if we can't allocate the I2C config
  }

  i2c_info = (sns_i2c_info *)bus_info->bus_config;

  result = i2c_setup_lpi_resource((i2c_instance)(com_config->bus_instance),
                                  com_config->max_bus_speed_KHz);
  if(I2C_ERROR(result))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "i2c_setup_lpi_resource failure. i2c_instance %d err %d",
                com_config->bus_instance, result);
    sns_free(bus_info->bus_config);
    bus_info->bus_config = NULL;
    return SNS_RC_FAILED;
  }

  result = i2c_open((i2c_instance)(com_config->bus_instance),
                     &i2c_info->i2c_handle);
  if(I2C_ERROR(result))
  {
    SNS_PRINTF(HIGH, sns_fw_printf, "i2c_open failure. i2c_instance %d err %d",
                com_config->bus_instance, result);
    result = i2c_reset_lpi_resource((i2c_instance)(com_config->bus_instance));
    sns_free(bus_info->bus_config);
    bus_info->bus_config = NULL;
    return SNS_RC_FAILED;
  }

  result = i2c_power_on(i2c_info->i2c_handle);
  if(I2C_ERROR(result))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "i2c_power_on failure. i2c_instance %d err %d",
                com_config->bus_instance, result);
    result = i2c_close(i2c_info->i2c_handle);
    result = i2c_reset_lpi_resource((i2c_instance)(com_config->bus_instance));
    sns_free(bus_info->bus_config);
    bus_info->bus_config = NULL;
    return SNS_RC_FAILED;
  }

  return SNS_RC_SUCCESS;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns") sns_rc sns_pwr_update_i2c(sns_sync_com_port_handle *port_handle,
                                                   bool power_bus_on)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_i2c_info   *i2c_info = (sns_i2c_info *)bus_info->bus_config;
  i2c_status      status = I2C_SUCCESS;

  // If the I2C config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)i2c_info))
  {
    SNS_ISLAND_EXIT();
  }

  if(power_bus_on)
  {
    status = i2c_power_on(i2c_info->i2c_handle);
  }
  else
  {
    /** If a client wants to turn bus power off then cancel any
     *  pending transfer over this port. */
    {
      i2c_status cancel_status = I2C_SUCCESS;

      cancel_status = i2c_cancel_transfer(i2c_info->i2c_handle);

      if(cancel_status == I2C_TRANSFER_FORCE_TERMINATED)
      {
        sns_time one_ms = sns_convert_ns_to_ticks( 1000000 );
        bool xfer_in_progress;

        SNS_PRINTF(MED, sns_fw_printf,
                   "i2c_cancel_transfer returned FORCE_TERMINATED. Waiting for completion...");
        qurt_atomic_barrier();
        xfer_in_progress = (volatile unsigned int)i2c_info->xfer_in_progress;
        while( xfer_in_progress )
        {
          sns_busy_wait( one_ms );
          qurt_atomic_barrier();
          xfer_in_progress = (volatile unsigned int)i2c_info->xfer_in_progress;
        }
        SNS_PRINTF(MED, sns_fw_printf, "Done waiting");
      }
    }
    status = i2c_power_off(i2c_info->i2c_handle);
  }

  if(!I2C_SUCCESS(status))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "i2c_pwr failure: on:%d status: %d",
               power_bus_on, status);
    rc = SNS_RC_FAILED;
  }

  return rc;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns") sns_rc sns_close_i2c(sns_sync_com_port_handle *port_handle)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_i2c_info *i2c_info = (sns_i2c_info *)bus_info->bus_config;
  sns_com_port_config *com_config = &priv_handle->com_config;
  i2c_status status = I2C_SUCCESS;
  sns_rc rv = SNS_RC_SUCCESS;

  SNS_ISLAND_EXIT();

  status = i2c_close(i2c_info->i2c_handle);
  if(I2C_ERROR(status))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "i2c_close failure. err %d",
               status);
    rv = SNS_RC_FAILED;
  }

  status = i2c_reset_lpi_resource((i2c_instance)(com_config->bus_instance));
  if(I2C_ERROR(status))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "i2c_reset_lpi_resource failure. err %d",
               status);
  }

  sns_free(bus_info->bus_config);
  bus_info->bus_config = NULL;

  return rv;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns") sns_rc sns_get_write_time_i2c(sns_sync_com_port_handle *port_handle, sns_time *write_time)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_i2c_info  *i2c_info = (sns_i2c_info *)bus_info->bus_config;
  uint32_t bus_timestamp;

  // If the I2C config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)i2c_info))
  {
    SNS_ISLAND_EXIT();
  }

  bus_timestamp = i2c_get_start_timestamp( i2c_info->i2c_handle );

  if( bus_timestamp >= (uint32_t)i2c_info->txn_start_time )
  {
    *write_time =
      (i2c_info->txn_start_time & 0xFFFFFFFF00000000ULL) | bus_timestamp;
  }
  else
  {
    *write_time =
      ((i2c_info->txn_start_time & 0xFFFFFFFF00000000ULL) + 0x100000000ULL) | bus_timestamp;
  }

  return SNS_RC_SUCCESS;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns") sns_rc sns_simple_txfr_port_i2c(sns_sync_com_port_handle *port_handle,
                                bool is_write,
                                bool save_write_time,
                                uint8_t *buffer,
                                uint32_t bytes,
                                uint32_t *xfer_bytes)
{
  sns_rc            return_code = SNS_RC_SUCCESS;
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_i2c_info     *i2c_info = (sns_i2c_info *)bus_info->bus_config;
  sns_com_port_config *com_config = &priv_handle->com_config;
  uint8_t          *xfer_buffer_ptr = sns_malloc(SNS_HEAP_PRAM, bytes);
  i2c_status        status;

  // If the I2C config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)i2c_info))
  {
    SNS_ISLAND_EXIT();
  }

  i2c_descriptor descriptor =
    {
      .buffer = xfer_buffer_ptr,
      .length = bytes,
      .flags = is_write ? I2C_WRITE_TRANSFER : I2C_READ_TRANSFER
    };

  i2c_slave_config config =
    {
      .bus_frequency_khz          = com_config->max_bus_speed_KHz,
      .slave_address              = com_config->slave_control,
      .mode                       = I2C,
      .slave_max_clock_stretch_us = SLAVE_MAX_CLOCK_STRETCH_US,
      .core_configuration1        = 0,
      .core_configuration2        = 0
    };

  if(( xfer_buffer_ptr == NULL ) && (0 != bytes))
  {
    return SNS_RC_NOT_AVAILABLE;
  }

  // For write transfers, copy the write data into PRAM for the transfer
  if( is_write )
  {
    if( !sns_island_is_island_ptr((intptr_t)buffer) )
    {
      SNS_ISLAND_EXIT();
    }
    sns_memscpy( xfer_buffer_ptr, bytes, buffer, bytes );
  }

  if( save_write_time )
  {
    descriptor.flags |= I2C_FLAG_TIMESTAMP;
    i2c_info->txn_start_time = sns_get_system_time();
  }
  if((0 == bytes) && (NULL == buffer))
  {
    descriptor.flags = I2C_FLAG_START;
    descriptor.length = 0;
    descriptor.buffer = NULL;
  }
  status = i2c_transfer( i2c_info->i2c_handle, &config, &descriptor, 1,
                         NULL, 0, 0, xfer_bytes);
  if( I2C_ERROR(status) )
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "i2c transfer response: %u, slave address:%d",status,com_config->slave_control);
    return_code = SNS_RC_FAILED;
  }

  if( !is_write )
  {
    // For read operations, copy data from PRAM back to client buffer
    if( !sns_island_is_island_ptr((intptr_t)buffer) )
    {
      SNS_ISLAND_EXIT();
    }
    sns_memscpy( buffer, bytes, xfer_buffer_ptr, bytes );
  }
  sns_free( xfer_buffer_ptr );

  return return_code;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns") sns_rc sns_vectored_rw_i2c(sns_sync_com_port_handle *port_handle,
                           sns_port_vector *vectors,
                           int32_t num_vectors,
                           bool save_write_time,
                           uint32_t *xfer_bytes,
                           bool async)
{
  sns_rc                    return_code = SNS_RC_SUCCESS;
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info             *bus_info = &priv_handle->bus_info;
  sns_i2c_info             *i2c_info = (sns_i2c_info *)bus_info->bus_config;
  sns_com_port_config      *com_config = &priv_handle->com_config;
  sns_com_port_vector_xfer_map *xfer_map = &priv_handle->xfer_map;
  int                       i, j;
  int                       register_len;
  i2c_status                status = I2C_SUCCESS;
  i2c_descriptor            i2c_desc[num_vectors * 2];
  i2c_slave_config          slave_config =
    {
      .bus_frequency_khz          = com_config->max_bus_speed_KHz,
      .slave_address              = com_config->slave_control,
      .mode                       = I2C,
      .slave_max_clock_stretch_us = SLAVE_MAX_CLOCK_STRETCH_US,
      .core_configuration1        = 0,
      .core_configuration2        = 0
    };

  // If the I2C config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)i2c_info))
  {
    SNS_ISLAND_EXIT();
  }

  // Verify bus speed supported by I2C driver
  if( slave_config.bus_frequency_khz >= I2C_FAST_MODE_PLUS_FREQ_KHZ )
  {
    slave_config.bus_frequency_khz = I2C_FAST_MODE_PLUS_FREQ_KHZ;
  }
  else if( slave_config.bus_frequency_khz >= I2C_FAST_MODE_FREQ_KHZ )
  {
    slave_config.bus_frequency_khz = I2C_FAST_MODE_FREQ_KHZ;
  }
  else
  {
    slave_config.bus_frequency_khz = I2C_STANDARD_MODE_FREQ_KHZ;
  }

  switch( com_config->reg_addr_type )
  {
    case SNS_REG_ADDR_16_BIT:
      register_len = 2;
      break;
    case SNS_REG_ADDR_32_BIT:
      register_len = 4;
      break;
    default:
    case SNS_REG_ADDR_8_BIT:
      register_len = 1;
      break;
  }
  if( xfer_bytes != NULL )
  {
    *xfer_bytes = 0;
  }

  // Allocate space in PRAM for the registers & data
  xfer_map->orig_vector_ptr = vectors;
  xfer_map->xfer_buffer_ptrs = sns_malloc(SNS_HEAP_ISLAND, sizeof(uint8_t*)*num_vectors);
  if( xfer_map->xfer_buffer_ptrs == NULL )
  {
    SNS_ISLAND_EXIT();
    xfer_map->xfer_buffer_ptrs = sns_malloc(SNS_HEAP_MAIN, sizeof(uint8_t*)*num_vectors);
    if( xfer_map->xfer_buffer_ptrs == NULL )
    {
      xfer_map->orig_vector_ptr = NULL;
      SNS_PRINTF(ERROR, sns_fw_printf,
                   "Unable to malloc in DDR SNS_RC_NOT_AVAILABLE");
      return SNS_RC_NOT_AVAILABLE;
    }
  }
  xfer_map->num_vectors = num_vectors;
  for(i = 0; i < num_vectors; i++)
  {
    xfer_map->xfer_buffer_ptrs[i] = sns_malloc(SNS_HEAP_PRAM,
                                               register_len + vectors[i].bytes);
    if( xfer_map->xfer_buffer_ptrs[i] == NULL )
    {
      for(; i >= 0; i--)
      {
        sns_free( xfer_map->xfer_buffer_ptrs[i] );
      }
      sns_free( xfer_map->xfer_buffer_ptrs );
      xfer_map->xfer_buffer_ptrs = NULL;
      xfer_map->orig_vector_ptr = NULL;
      SNS_ISLAND_EXIT();
      SNS_PRINTF(ERROR, sns_fw_printf,
                   "Unable to malloc vectors in PRAM SNS_RC_NOT_AVAILABLE");
      return SNS_RC_NOT_AVAILABLE;
    }
  }

  if( save_write_time )
  {
    i2c_info->txn_start_time = sns_get_system_time();
  }

  if( async )
  {
    qurt_atomic_set( &i2c_info->xfer_in_progress, true );
  }

  i = 0, j=0;
  while (i < num_vectors)
  {
      // writes use 1 descriptor, reads 2
      int num_new_descriptors = vectors[i].is_write ? 1 : 2;
      if( j + num_new_descriptors > MAX_I2C_DESCRIPTORS )
      {
          i2c_desc[j-1].flags |= I2C_FLAG_STOP;
          i2c_transfer( i2c_info->i2c_handle,
                         &slave_config,
                         i2c_desc,
                         j,
                         NULL,
                         NULL,
                         0 /* Delay */,
                         xfer_bytes );
          j = 0;
      }
      switch(register_len)
      {
        case 1:
          xfer_map->xfer_buffer_ptrs[i][0] = (uint8_t)(vectors[i].reg_addr & 0xff);
          break;

        case 2:
          xfer_map->xfer_buffer_ptrs[i][0] = (uint8_t)((vectors[i].reg_addr >> 8) & 0xff);
          xfer_map->xfer_buffer_ptrs[i][1] = (uint8_t)((vectors[i].reg_addr >> 0) & 0xff);
          break;

        case 4:
          xfer_map->xfer_buffer_ptrs[i][0] = (uint8_t)((vectors[i].reg_addr >> 24) & 0xff);
          xfer_map->xfer_buffer_ptrs[i][1] = (uint8_t)((vectors[i].reg_addr >> 16) & 0xff);
          xfer_map->xfer_buffer_ptrs[i][2] = (uint8_t)((vectors[i].reg_addr >> 8)  & 0xff);
          xfer_map->xfer_buffer_ptrs[i][3] = (uint8_t)((vectors[i].reg_addr >> 0)  & 0xff);
          break;

        default:
          xfer_map->xfer_buffer_ptrs[i][0] = (uint8_t)(vectors[i].reg_addr & 0xff);
          break;
      }      
      i2c_desc[j].length = register_len;
      i2c_desc[j].buffer = xfer_map->xfer_buffer_ptrs[i];
      i2c_desc[j].flags  = I2C_FLAG_START | I2C_WR_RD_WRITE;

      if( vectors[i].is_write )
      {
        // For write transfers, copy the write data into PRAM for the transfer
        if( !sns_island_is_island_ptr((intptr_t)vectors[i].buffer) )
        {
          SNS_ISLAND_EXIT();
        }
        sns_memscpy( xfer_map->xfer_buffer_ptrs[i] + register_len, vectors[i].bytes,
                     vectors[i].buffer, vectors[i].bytes );
        i2c_desc[j].length += vectors[i].bytes;
        if( save_write_time )
        {
          i2c_desc[j].flags |= I2C_FLAG_TIMESTAMP;
        }
      }
      else // Read
      {
        j++;
        i2c_desc[j].flags = I2C_FLAG_START | I2C_FLAG_READ;
        i2c_desc[j].length = vectors[i].bytes;
        i2c_desc[j].buffer = xfer_map->xfer_buffer_ptrs[i] + register_len;
      }
      i++, j++;
  }

  if( j > 0 )
  {
      i2c_desc[j-1].flags |= I2C_FLAG_STOP;
      status = i2c_transfer( i2c_info->i2c_handle,
                         &slave_config,
                         i2c_desc,
                         j,
                         async ? sns_i2c_callback : NULL,
                         async ? (void*)port_handle : NULL,
                         0 /* Delay */,
                         xfer_bytes );
  }

  if( xfer_bytes != NULL )
  {
    *xfer_bytes -= register_len * num_vectors;
  }

  if(!I2C_SUCCESS(status))
  {
    SNS_ISLAND_EXIT();
    SNS_PRINTF(ERROR, sns_fw_printf,
                 "i2c_transfer failed 0x%X", status);
    if( async )
    {
      qurt_atomic_set( &i2c_info->xfer_in_progress, false );
    }
	return_code = SNS_RC_FAILED;
  }

  if (!async)
  {
    for( i = 0; i < num_vectors; i++ )
    {
      if( !vectors[i].is_write && I2C_SUCCESS(status))
      {
        // For read operations, copy data from PRAM back to client buffer
        if( !sns_island_is_island_ptr((intptr_t)vectors[i].buffer) )
        {
          SNS_ISLAND_EXIT();
        }
        sns_memscpy( vectors[i].buffer, vectors[i].bytes,
                     xfer_map->xfer_buffer_ptrs[i]+register_len, vectors[i].bytes );
      }
      sns_free( xfer_map->xfer_buffer_ptrs[i] );
    }
    sns_free(xfer_map->xfer_buffer_ptrs);
  }

  return return_code;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns") sns_rc sns_ascp_register_i2c_callback(void *port_handle,
                                      sns_ascp_callback callback,
                                      void *args)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  priv_handle->callback = callback;
  priv_handle->args = args;
  return SNS_RC_SUCCESS;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns") sns_rc sns_sync_vectored_rw_i2c(sns_sync_com_port_handle *port_handle,
                                sns_port_vector *vectors,
                                int32_t num_vectors,
                                bool save_write_time,
                                uint32_t *xfer_bytes)
{
  return sns_vectored_rw_i2c(port_handle, vectors, num_vectors,
                             save_write_time, xfer_bytes, false);
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns") sns_rc sns_async_vectored_rw_i2c(void  *port_handle,
                                 uint8_t num_vectors,
                                 sns_port_vector vectors[],
                                 bool save_write_time)
{
  return  sns_vectored_rw_i2c(port_handle, vectors, num_vectors,
                              save_write_time, NULL, true);
}


/**--------------------------------------------------------------------------*/
sns_sync_com_port_service_api i2c_port_api SNS_SECTION(".data.sns")=
{
  .struct_len               = sizeof(sns_sync_com_port_service_api),
  .sns_scp_register_com_port = NULL,
  .sns_scp_deregister_com_port = NULL,
  .sns_scp_open             = &sns_open_i2c,
  .sns_scp_close            = &sns_close_i2c,
  .sns_scp_update_bus_power = &sns_pwr_update_i2c,
  .sns_scp_register_rw      = &sns_sync_vectored_rw_i2c,
  .sns_scp_simple_rw        = &sns_simple_txfr_port_i2c,
  .sns_scp_get_write_time   = &sns_get_write_time_i2c,
  .sns_scp_get_version      = NULL
};

/**--------------------------------------------------------------------------*/
sns_ascp_port_api i2c_async_port_api SNS_SECTION(".data.sns") =
{
  .struct_len                  = sizeof(sns_ascp_port_api),
  .sync_com_port               = &i2c_port_api,
  .sns_ascp_register_callback  = &sns_ascp_register_i2c_callback,
  .sns_ascp_register_rw        = &sns_async_vectored_rw_i2c,
};

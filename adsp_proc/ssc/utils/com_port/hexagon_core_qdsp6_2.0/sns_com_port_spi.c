/**
 * sns_sync_com_port_spi.c
 *
 * SPI implementation
 *
 * Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
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

#include "qurt_atomic_ops.h"
#include "spi_api.h"

// TODO: implement SPI functionality
typedef struct sns_spi_info
{
  void                  *spi_handle;
  sns_time               txn_start_time;
  unsigned int           xfer_in_progress;
} sns_spi_info;

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns")
static void sns_spi_callback( uint32 transfer_status, void *ctxt )
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)ctxt;
  sns_com_port_config *com_config = &priv_handle->com_config;
  sns_com_port_vector_xfer_map *vec_map = &priv_handle->xfer_map;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_spi_info *spi_info;
  int num_vectors;
  int register_len = 0;
  sns_port_vector *vector_ptr;
  sns_rc rw_err = SNS_RC_FAILED;
  int i;
  uint32_t transferred = 0;

  if( !sns_island_is_island_ptr((intptr_t) vec_map)                  ||
      !sns_island_is_island_ptr((intptr_t) vec_map->xfer_buffer_ptrs) ||
      !sns_island_is_island_ptr((intptr_t) vec_map->orig_vector_ptr) ||
      !sns_island_is_island_ptr((intptr_t) bus_info)                   )
  {
    SNS_ISLAND_EXIT();
  }
  vector_ptr = vec_map->orig_vector_ptr;
  num_vectors = vec_map->num_vectors;
  spi_info = (sns_spi_info *)bus_info->bus_config;

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

  if( SPI_SUCCESS(transfer_status) )
  {
    rw_err = SNS_RC_SUCCESS;
    for( i = 0; i < num_vectors; i++ )
    {
      if( !vec_map->orig_vector_ptr[i].is_write  &&
          (NULL != vector_ptr[i].buffer ))
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
      transferred += vector_ptr[i].bytes;
    }
  }
  else
  {
    for( i = 0; i < num_vectors; i++ )
    {
      sns_free( vec_map->xfer_buffer_ptrs[i] );
    }
  }
  sns_free( vec_map->xfer_buffer_ptrs );

  qurt_atomic_set( &spi_info->xfer_in_progress, false );

  if( priv_handle->callback != NULL )
  {
    priv_handle->callback(priv_handle, num_vectors, vector_ptr,
                          transferred, rw_err, priv_handle->args);
  }
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns")
sns_rc sns_open_spi(sns_sync_com_port_handle *port_handle)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_com_port_config *com_config = &priv_handle->com_config;
  spi_status_t     result;
  sns_spi_info    *spi_info;

  if (com_config->bus_instance <= 0
      ||
      com_config->bus_instance > SPI_INSTANCE_MAX)
  {
    return SNS_RC_INVALID_VALUE;
  }
  SNS_ISLAND_EXIT();

  bus_info->bus_config = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_spi_info));
//  SNS_ASSERT(bus_info->bus_config != NULL );
  if (NULL == bus_info->bus_config)
  {
    // We ran out of island heap, so let's throw out a warning message and then
    // allocate the spi config in DDR. This isn't ideal for power, but
    // will help with stability.
    SNS_PRINTF(HIGH, sns_fw_printf, "Alloc SPI config in DDR!");
    bus_info->bus_config = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_spi_info));
    SNS_ASSERT(NULL != bus_info->bus_config); // We're in big trouble if we can't allocate the SPI config
  }

  spi_info = (sns_spi_info *)bus_info->bus_config;

  result = spi_setup_island_resource( (spi_instance_t)(com_config->bus_instance),
                     com_config->max_bus_speed_KHz * 1000);
  if(SPI_ERROR(result))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "spi setup island resource failure. spi_instance %d err %d",
                com_config->bus_instance, result );
    sns_free(bus_info->bus_config);
    bus_info->bus_config = NULL;
    return SNS_RC_FAILED;
  }

  result = spi_open( (spi_instance_t)(com_config->bus_instance),
                     &spi_info->spi_handle );
  if(SPI_ERROR(result))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "spi_open failure. spi_instance %d err %d",
                com_config->bus_instance, result );
    result = spi_reset_island_resource((spi_instance_t)(com_config->bus_instance));
    sns_free(bus_info->bus_config);
    bus_info->bus_config = NULL;
    return SNS_RC_FAILED;
  }

  result = spi_power_on(spi_info->spi_handle);
  if(SPI_ERROR(result))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "spi_power_on failure. spi_instance %d err %d",
                com_config->bus_instance, result );
    result = spi_close(spi_info->spi_handle);
    result = spi_reset_island_resource((spi_instance_t)(com_config->bus_instance));
    sns_free(bus_info->bus_config);
    bus_info->bus_config = NULL;
    return SNS_RC_FAILED;
  }

  return SNS_RC_SUCCESS;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns")
sns_rc sns_close_spi(sns_sync_com_port_handle *port_handle)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_com_port_config *com_config = &priv_handle->com_config;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_spi_info *spi_info = (sns_spi_info *)bus_info->bus_config;
  spi_status_t result;

  SNS_ISLAND_EXIT();

  // TODO: handle closing SPI with transaction outstanding.
  spi_close(spi_info->spi_handle);
  result = spi_reset_island_resource((spi_instance_t)(com_config->bus_instance));
  if(SPI_ERROR(result))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "spi reset island resource failure. spi_instance %d err %d",
                com_config->bus_instance, result );
  }

  sns_free(bus_info->bus_config);
  bus_info->bus_config = NULL;
  return SNS_RC_SUCCESS;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns")
sns_rc sns_pwr_update_spi(sns_sync_com_port_handle *port_handle, bool power_bus_on)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_spi_info *spi_info = (sns_spi_info *)bus_info->bus_config;
  spi_status_t  status = SPI_SUCCESS;

  // If the SPI config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)spi_info))
  {
    SNS_ISLAND_EXIT();
  }

  if(power_bus_on)
  {
    status = spi_power_on(spi_info->spi_handle);
  }
  else
  {
    bool xfer_in_progress;
    qurt_atomic_barrier();
    xfer_in_progress = (volatile unsigned int)spi_info->xfer_in_progress;
    if( xfer_in_progress )
    {
      sns_time one_ms = sns_convert_ns_to_ticks( 1000000 );
      SNS_PRINTF(MED, sns_fw_printf, "Waiting for SPI txn complete before powering off...");
      while( xfer_in_progress )
      {
        sns_busy_wait( one_ms );
        qurt_atomic_barrier();
        xfer_in_progress = (volatile unsigned int)spi_info->xfer_in_progress;
      }
      SNS_PRINTF(MED, sns_fw_printf, "Done waiting");
    }
    status = spi_power_off(spi_info->spi_handle);
  }

  if( !SPI_SUCCESS(status) )
  {
    SNS_ISLAND_EXIT();
    SNS_PRINTF(ERROR, sns_fw_printf, "spi_pwr failure: on:%d status: %d",
               power_bus_on, status);
    rc = SNS_RC_FAILED;
  }

  return rc;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns")
sns_rc sns_get_write_time_spi(sns_sync_com_port_handle *port_handle, sns_time *write_time)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_spi_info  *spi_info = (sns_spi_info *)bus_info->bus_config;
  uint64_t bus_timestamp = 0;
  spi_status_t status;

  // If the SPI config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)spi_info))
  {
    SNS_ISLAND_EXIT();
  }

  status = spi_get_timestamp( spi_info->spi_handle, &bus_timestamp, NULL );

  if( bus_timestamp >= (uint32_t)spi_info->txn_start_time )
  {
    *write_time =
      (spi_info->txn_start_time & 0xFFFFFFFF00000000LL) | bus_timestamp;
  }
  else
  {
    *write_time =
      ((spi_info->txn_start_time & 0xFFFFFFFF00000000LL) + 0x100000000LL) | bus_timestamp;
  }

  return SNS_RC_SUCCESS;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns")
sns_rc sns_simple_txfr_port_spi(sns_sync_com_port_handle *port_handle,
                                bool is_write,
                                bool save_write_time,
                                uint8_t *buffer,
                                uint32_t bytes,
                                uint32_t *xfer_bytes)
{
  sns_rc            return_code = SNS_RC_SUCCESS;
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_spi_info     *spi_info = (sns_spi_info *)bus_info->bus_config;
  sns_com_port_config *com_config = &priv_handle->com_config;
  spi_status_t      status;
  uint8_t          *xfer_buffer_ptr = sns_malloc(SNS_HEAP_PRAM, bytes);

  // If the SPI config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)spi_info))
  {
    SNS_ISLAND_EXIT();
  }

  spi_descriptor_t descriptor =
    {
      // TODO: Put buffer in PRAM
      .tx_buf = is_write ? xfer_buffer_ptr : NULL,
      .rx_buf = is_write ? NULL : xfer_buffer_ptr,
      .len = bytes
    };

  spi_config_t config =
    {
      .spi_mode = SPI_MODE_0,
      .spi_cs_polarity = SPI_CS_ACTIVE_LOW,
      .endianness = SPI_NATIVE,
      .spi_bits_per_word = 8,
      .spi_slave_index = com_config->slave_control,
      .clk_freq_hz = com_config->max_bus_speed_KHz * 1000,
      .cs_clk_delay_cycles = 1,
      .inter_word_delay_cycles = 0,
      .cs_toggle = 0,
      .loopback_mode = 0,
    };

  if( xfer_buffer_ptr == NULL )
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

  status = spi_full_duplex( spi_info->spi_handle, &config, &descriptor, 1,
                            NULL, 0, save_write_time);

  if( SPI_ERROR(status) )
  {
    return_code = SNS_RC_FAILED;
  }
  *xfer_bytes = bytes;

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
SNS_SECTION(".text.sns")
sns_rc sns_vectored_rw_spi(sns_sync_com_port_handle *port_handle,
                           sns_port_vector *vectors,
                           int32_t num_vectors,
                           bool save_write_time,
                           uint32_t *xfer_bytes,
                           bool async)
{
  sns_rc                    return_code = SNS_RC_FAILED;
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info             *bus_info = &priv_handle->bus_info;
  sns_spi_info             *spi_info = (sns_spi_info *)bus_info->bus_config;
  sns_com_port_config      *com_config = &priv_handle->com_config;
  sns_com_port_vector_xfer_map *xfer_map = &priv_handle->xfer_map;
  int                       i;
  int                       register_len;
  spi_status_t              status;
  spi_descriptor_t          spi_desc[num_vectors];
  int                       num_bytes = 0;
  uint32_t                  bit_shift_width = 0xFF;
  spi_config_t              slave_config =
    {
      .spi_mode = SPI_MODE_0,
      .spi_cs_polarity = SPI_CS_ACTIVE_LOW,
      .endianness = SPI_NATIVE,
      .spi_bits_per_word = 8,
      .spi_slave_index = com_config->slave_control,
      .clk_freq_hz = com_config->max_bus_speed_KHz * 1000,
      .cs_clk_delay_cycles = 1,
      .inter_word_delay_cycles = 0,
      .cs_toggle = 0,
      .loopback_mode = 0,
    };

  // If the SPI config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)spi_info))
  {
    SNS_ISLAND_EXIT();
  }

  switch( com_config->reg_addr_type )
  {
    case SNS_REG_ADDR_16_BIT:
      register_len = 2;
      bit_shift_width = 0xFFFF;
      break;
    case SNS_REG_ADDR_32_BIT:
      register_len = 4;
      bit_shift_width = 0xFFFFFFFF;
      break;
    default:
    case SNS_REG_ADDR_8_BIT:
      register_len = 1;
      bit_shift_width = 0xFF;
      break;
  }

  xfer_map->orig_vector_ptr = vectors;
  xfer_map->xfer_buffer_ptrs = sns_malloc(SNS_HEAP_ISLAND, sizeof(uint8_t*)*num_vectors);
  if( xfer_map->xfer_buffer_ptrs == NULL )
  {
    SNS_ISLAND_EXIT();
    xfer_map->xfer_buffer_ptrs = sns_malloc(SNS_HEAP_MAIN, sizeof(uint8_t*)*num_vectors);
    if( xfer_map->xfer_buffer_ptrs == NULL )
    {
      xfer_map->orig_vector_ptr = NULL;
      return SNS_RC_NOT_AVAILABLE;
    }
  }
  xfer_map->num_vectors = num_vectors;
  for(i = 0; i < num_vectors; i++)
  {
    uint32_t temp_reg_addr = 0;

    // Malloc the memory for the buffers
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
      return SNS_RC_NOT_AVAILABLE;
    }

    // Copy the register address into the buffer after modifing the read/write bit
    if( vectors[i].is_write )
    {
      temp_reg_addr = vectors[i].reg_addr & (bit_shift_width >> 1);
    }
    else
    {
      temp_reg_addr = vectors[i].reg_addr | (1 << 7);
    }

    sns_memscpy( xfer_map->xfer_buffer_ptrs[i], register_len,
                 (uint8_t*)&temp_reg_addr, register_len );

    // Update the SPI descriptor
    if( vectors[i].is_write )
    {
      if( !sns_island_is_island_ptr((intptr_t)vectors[i].buffer) )
      {
        SNS_ISLAND_EXIT();
      }
      sns_memscpy( xfer_map->xfer_buffer_ptrs[i] + register_len, vectors[i].bytes,
                   vectors[i].buffer, vectors[i].bytes );
      spi_desc[i].tx_buf = xfer_map->xfer_buffer_ptrs[i];
      spi_desc[i].rx_buf = NULL;
    }
    else
    {
      spi_desc[i].tx_buf = xfer_map->xfer_buffer_ptrs[i];
      spi_desc[i].rx_buf = xfer_map->xfer_buffer_ptrs[i];
    }
    spi_desc[i].len = vectors[i].bytes + register_len;
    spi_desc[i].leave_cs_asserted = false;
    num_bytes += vectors[i].bytes;
  }
  if( save_write_time )
  {
    spi_info->txn_start_time = sns_get_system_time();
  }

  if( async )
  {
    qurt_atomic_set( &spi_info->xfer_in_progress, true );
  }

  status = spi_full_duplex( spi_info->spi_handle,
                            &slave_config,
                            spi_desc,
                            num_vectors,
                            async ? sns_spi_callback : NULL,
                            async ? (void*)port_handle : NULL,
                            save_write_time );

  if(SPI_SUCCESS(status))
  {
    if(xfer_bytes != NULL)
    {
      *xfer_bytes = num_bytes;
    }
    return_code = SNS_RC_SUCCESS;
  }
  else
  {
    SNS_ISLAND_EXIT();
    SNS_PRINTF(ERROR, sns_fw_printf, "spi_full_duplex failure. err %d", status);
    if( async )
    {
      qurt_atomic_set( &spi_info->xfer_in_progress, false );
    }
  }

  if (!async)
  {
    for( i = 0; i < num_vectors; i++ )
    {
      if( !vectors[i].is_write && SPI_SUCCESS(status))
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
SNS_SECTION(".text.sns")
sns_rc sns_ascp_register_spi_callback(void *port_handle,
                                      sns_ascp_callback callback,
                                      void *args)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  priv_handle->callback = callback;
  priv_handle->args = args;
  return SNS_RC_SUCCESS;
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns")
sns_rc sns_sync_vectored_rw_spi(sns_sync_com_port_handle *port_handle,
                                sns_port_vector *vectors,
                                int32_t num_vectors,
                                bool save_write_time,
                                uint32_t *xfer_bytes)
{
  return sns_vectored_rw_spi(port_handle, vectors, num_vectors,
                             save_write_time, xfer_bytes, false);
}

/**--------------------------------------------------------------------------*/
SNS_SECTION(".text.sns")
sns_rc sns_async_vectored_rw_spi(void  *port_handle,
                                 uint8_t num_vectors,
                                 sns_port_vector vectors[],
                                 bool save_write_time)
{
  return sns_vectored_rw_spi(port_handle, vectors, num_vectors,
                             save_write_time, NULL, true);
}

sns_sync_com_port_service_api spi_port_api SNS_SECTION(".data.sns") =
{
  .struct_len               = sizeof(sns_sync_com_port_service_api),
  .sns_scp_register_com_port = NULL,
  .sns_scp_deregister_com_port = NULL,
  .sns_scp_open             = &sns_open_spi,
  .sns_scp_close            = &sns_close_spi,
  .sns_scp_update_bus_power = &sns_pwr_update_spi,
  .sns_scp_register_rw      = &sns_sync_vectored_rw_spi,
  .sns_scp_simple_rw        = &sns_simple_txfr_port_spi,
  .sns_scp_get_write_time   = &sns_get_write_time_spi,
  .sns_scp_get_version      = NULL
};

sns_ascp_port_api spi_async_port_api SNS_SECTION(".data.sns") =
{
  .struct_len                  = sizeof(sns_ascp_port_api),
  .sync_com_port               = &spi_port_api,
  .sns_ascp_register_callback  = &sns_ascp_register_spi_callback,
  .sns_ascp_register_rw        = &sns_async_vectored_rw_spi,
};

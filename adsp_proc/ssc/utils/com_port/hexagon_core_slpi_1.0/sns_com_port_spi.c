/**
 * sns_sync_com_port_spi.c
 *
 * SPI implementation
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "SpiDriver.h"
#include "sns_assert.h"
#include "sns_async_com_port_int.h"
#include "sns_com_port_priv.h"
#include "sns_com_port_types.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_rc.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_types.h"
/*
  TODO: Remove spi_open() and spi_close() wrappers around each SPI transfer once
  SPI support for multiple clients is implemented
*/

uint8_t sns_spi_init SNS_SECTION(".data.sns") = 0;
/**-------------------------------------------------------------------------*/
// In units of 1/1048576 seconds.
#define SPI_BUS_TXN_SETUP_TIME 1042 /* 54.3 uSec in 19.2MHz clock ticks */

//SPI read buffer size in bytes corresponding to LSM6DSM FIFO size
#define SNS_SPI_BUF_SIZE       4096

#define SNS_DEFAULT_SPI_BITS_PER_WORD     8
#define SNS_DEFAULT_SPI_MIN_CLK_FRQ_HZ    (0*1000*1000)
#define SNS_DEFAULT_SPI_MAX_CLK_FRQ_HZ    (33*100*1000)
#define SNS_DEFAULT_SPI_DEASSERT_TIME     0
#define SNS_DEFAULT_SPI_LOOPBACK_MODE     0

/**-------------------------------------------------------------------------*/

typedef struct sns_spi_info
{
  spi_device_id_t        dev_id;
  spi_config_t           cfg;
  // TODO: This will go away in new QuPv3 bus API
  sns_time               write_time;
} sns_spi_info;

sns_sync_com_port_service_api spi_port_api SNS_SECTION(".data.sns");
sns_ascp_port_api spi_async_port_api SNS_SECTION(".data.sns");
static uint8_t   sns_spi_read_buffer[SNS_SPI_BUF_SIZE]SNS_SECTION(".data.sns");
sns_osa_lock    *sns_spi_lock SNS_SECTION(".data.sns") = NULL;

/**------------------ Forward Declarations ---------------------------------*/

sns_rc sns_populate_spi_cfg(spi_config_t *spi_cfg,
                            sns_com_port_config const *com_config);
sns_rc sns_spi_write(sns_spi_info           *spi_info,
                     uint32_t                reg_addr,
                     const uint8_t          *buffer,
                     uint32_t                bytes,
                     uint32_t               *write_count,
                     bool                    save_write_time);

sns_rc sns_spi_read(sns_spi_info           *spi_info,
                    uint16_t                reg_addr,
                    uint8_t                *buffer,
                    uint32_t                bytes,
                    uint32_t               *read_count);

sns_rc sns_spi_data_transfer_timed(sns_spi_info  *spi_info,
                                   uint8_t       *read_buffer, //NULL allowed for write operation
                                   uint32_t       read_len,
                                   const uint8_t *write_buffer,
                                   uint32_t       write_len,
                                   uint32_t      *tx_count,
                                   bool           save_write_time);

sns_rc sns_spi_data_transfer(sns_spi_info  *spi_info,
                             uint8_t       *read_buffer, //NULL allowed for write operation
                             uint32_t       read_len,
                             const uint8_t *write_buffer,
                             uint32_t       write_len,
                             uint32_t      *tx_count );

/**--------------------------------------------------------------------------*/

SNS_SECTION(".text.sns") sns_rc sns_open_spi(sns_sync_com_port_handle *port_handle)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  //SPI_RESULT    result = SPI_SUCCESS;
  sns_com_port_config *com_config = &priv_handle->com_config;
  sns_spi_info *spi_info;

  if (com_config->bus_instance <= 0
      ||
      com_config->bus_instance > SPI_DEVICE_COUNT)
  {
    return SNS_RC_INVALID_VALUE;
  }

  if (NULL == sns_spi_lock)
  {
    sns_osa_lock_attr attr;
    sns_osa_lock_attr_init(&attr);
    sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
    SNS_ASSERT(SNS_RC_SUCCESS == sns_osa_lock_create(&attr, &sns_spi_lock));
    SNS_ASSERT(NULL != sns_spi_lock);
  }

  bus_info->bus_config = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_spi_info));
//  SNS_ASSERT(NULL != bus_info->bus_config);
  if (NULL == bus_info->bus_config)
  {
    // We ran out of island heap, so let's throw out a warning message and then
    // allocate the i2c config in DDR. This isn't ideal for power, but
    // will help with stability.
    SNS_ISLAND_EXIT();
    SNS_PRINTF(HIGH, sns_fw_printf, "Alloc SPI config in DDR!");
    bus_info->bus_config = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_spi_info));
    SNS_ASSERT(NULL != bus_info->bus_config); // We're in big trouble if we can't allocate the I2C config
  }

  spi_info = (sns_spi_info *)bus_info->bus_config;

  /* Initialize member params */
  // SPI bus ID = (BLSP number in registry) - 1
  spi_info->dev_id = (spi_device_id_t)(com_config->bus_instance - 1);
  sns_populate_spi_cfg(&spi_info->cfg, com_config);

  /* Open SPI port*/
 // result = spi_open(spi_info->dev_id);
 // if ( result != SPI_SUCCESS )
 // {
 //   // TODO add debug log
 //   sns_free(bus_info->bus_config);
 //   return SNS_RC_FAILED;
 // }

  /* Close device */
  //result = spi_close(spi_info->dev_id);
  //if ( result != SPI_SUCCESS )
  //{
  // TODO add debug log
  //  return SNS_RC_FAILED;
  //}

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_close_spi(sns_sync_com_port_handle *port_handle)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  // TODO: may not need any close implementation
  sns_free(bus_info->bus_config);
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_pwr_update_spi(sns_sync_com_port_handle *port_handle, bool power_bus_on)
{
  UNUSED_VAR(port_handle);
  UNUSED_VAR(power_bus_on);
  //SPI_RESULT    result = SPI_SUCCESS;
  //sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  //sns_bus_info *bus_info = &priv_handle->bus_info;
  //sns_spi_info *spi_info = (sns_spi_info *)bus_info->bus_config;

  //if(power_bus_on)
  //{
   // result = spi_open(spi_info->dev_id);
   // if ( result != SPI_SUCCESS )
   // {
      // TODO add debug log
   //   return SNS_RC_FAILED;
   // }
  //}
  //else
  //{
    //result = spi_close(spi_info->dev_id);
    //if ( result != SPI_SUCCESS )
    //{
      // TODO add debug log
    //  return SNS_RC_FAILED;
   // }
  //}

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_get_write_time_spi(sns_sync_com_port_handle *port_handle, sns_time *write_time)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_spi_info *spi_info = (sns_spi_info *)bus_info->bus_config;

  // If the SPI config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)spi_info))
  {
    SNS_ISLAND_EXIT();
  }

  if(write_time == NULL)
  {
    return SNS_RC_INVALID_VALUE;
  }

  *write_time = spi_info->write_time;

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_vectored_rw_spi(sns_sync_com_port_handle *port_handle,
                           sns_port_vector *vectors,
                           int32_t num_vectors,
                           bool save_write_time,
                           uint32_t *xfer_bytes)
{
  int               i;
  uint32_t          txn_count;
  spi_device_id_t   spi_dev_id;
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info      *bus_info = &priv_handle->bus_info;
  sns_spi_info      *spi_info = (sns_spi_info *)bus_info->bus_config;

  // If the SPI config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)spi_info))
  {
    SNS_ISLAND_EXIT();
  }

  spi_dev_id = spi_info->dev_id;

  if(xfer_bytes != NULL)
  {
    *xfer_bytes = 0;
  }

  // Assumption: spi_open() is already called as part of sns_pwr_update_spi(.., true)
  if (0 == sns_spi_init)
  {
    SNS_ISLAND_EXIT();
    sns_spi_init = 1;
  }
  sns_osa_lock_acquire(sns_spi_lock);
  spi_open(spi_info->dev_id);
  for( i = 0; i < num_vectors; i++ )
  {
    txn_count = 0;
    if( vectors[i].is_write )
    {
      sns_spi_write( spi_info,
                     vectors[i].reg_addr,
                     vectors[i].buffer,
                     vectors[i].bytes,
                     &txn_count,
                     save_write_time);
    }
    else
    {
      sns_spi_read( spi_info,
                    vectors[i].reg_addr,
                    vectors[i].buffer,
                    vectors[i].bytes,
                    &txn_count );
    }
    if(xfer_bytes != NULL)
    {
      *xfer_bytes += txn_count;
    }
  }
  spi_close(spi_info->dev_id);
  sns_osa_lock_release(sns_spi_lock);
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_simple_txfr_port_spi(sns_sync_com_port_handle *port_handle,
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
  UNUSED_VAR(bytes);
  UNUSED_VAR(xfer_bytes);
  return SNS_RC_NOT_AVAILABLE;
}


SNS_SECTION(".text.sns") sns_rc sns_populate_spi_cfg(spi_config_t *spi_cfg,
                            sns_com_port_config const *com_config)
{
  spi_cfg->spi_clk_polarity    = SPI_CLK_IDLE_HIGH;
  spi_cfg->spi_shift_mode      = SPI_OUTPUT_FIRST_MODE;
  spi_cfg->spi_cs_polarity     = SPI_CS_ACTIVE_LOW;
  spi_cfg->spi_cs_mode         = SPI_CS_KEEP_ASSERTED;
  spi_cfg->spi_clk_always_on   = SPI_CLK_NORMAL;

  //SPI bits per word, any value from 3 to 31
  spi_cfg->spi_bits_per_word   = (uint8)SNS_DEFAULT_SPI_BITS_PER_WORD;

  /* Slave index, beginning at 0, if multiple SPI devices are connected
     to the same master */
  spi_cfg->spi_slave_index     = (uint8)com_config->slave_control;
  spi_cfg->min_slave_freq_hz   = (uint32)(com_config->min_bus_speed_KHz * 1000);
  spi_cfg->max_slave_freq_hz   = (uint32)(com_config->max_bus_speed_KHz * 1000);
  spi_cfg->deassertion_time_ns = (uint32)SNS_DEFAULT_SPI_DEASSERT_TIME;

  // If set, SPI controller will enable loopback mode, used primarily for testing
  spi_cfg->loopback_mode       = (boolean)SNS_DEFAULT_SPI_LOOPBACK_MODE;

  return SNS_RC_SUCCESS;
}

/**
 * @brief Performs a bus write operation.
 *
 * @param[in]  handle       The I/O port handle that identifies a device. This
 *                          is given to drivers as part of initialization.
 * @param[in]  reg_addr     Address of the register to write on the device.
 * @param[in]  buffer       Location where value to be written is stored. Note
 *                          that size of this buffer must >= @a bytes argument.
 * @param[in]  bytes        Number of bytes to write.
 * @param[out] write_count  Number of bytes actually written from the buffer.
 * @param[out] write_time   System time write was performed, in SSC ticks << 5.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
SNS_SECTION(".text.sns") sns_rc sns_spi_write
(
  sns_spi_info           *spi_info,
  uint32_t                reg_addr,
  const uint8_t          *buffer,
  uint32_t                bytes,
  uint32_t               *write_count,
  bool                    save_write_time
)
{
  sns_rc return_code = SNS_RC_SUCCESS;
  //bytes+1 total spi transfers, 1 byte spi protocol overhead
  uint8_t          write_buffer[bytes + 1]; //+1 byt
  uint8_t          reg_addr8 = (uint8_t)reg_addr;

  // If the SPI config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)spi_info))
  {
    SNS_ISLAND_EXIT();
  }

  sns_memscpy(&write_buffer[1], sizeof(write_buffer), buffer, bytes);

  reg_addr8       &= (0xFF >> 1);        //set RW bit = 0 for write
  write_buffer[0]  = (uint8_t)reg_addr8; //NOTE: currently 8bit reg addr only
  if (0 == sns_spi_init)
  {
    SNS_ISLAND_EXIT();
    sns_spi_init = 1;
  }
  sns_osa_lock_acquire(sns_spi_lock);
  spi_open(spi_info->dev_id);
  return_code = sns_spi_data_transfer_timed(
                  spi_info,
                  NULL, //read_dummy_buffer, NULL is allowed for write operation
                  0,    //read_len
                  write_buffer,
                  bytes + 1,
                  write_count,
                  save_write_time);
  spi_close(spi_info->dev_id);
  sns_osa_lock_release(sns_spi_lock);
  if(return_code != SNS_RC_SUCCESS)
  {
    // TODO add debug log
    return return_code;
  }
  else
  {
    *write_count = bytes;
  }

  return SNS_RC_SUCCESS;
}

/**
 * @brief Performs the spi bus read operation.
 *
 * @param[in]  handle      The I/O port handle that identifies a device. This is
 *                         given to drivers as part of initialization.
 * @param[in]  reg_addr    Address of the register to read on the device.
 * @param[out] buffer      Location where result will be stored. Note size of
 *                         this buffer must >= @a bytes argument.
 * @param[in]  bytes       Number of bytes to read.
 * @param[out] read_count  Number of bytes actually read into the buffer.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
SNS_SECTION(".text.sns") sns_rc sns_spi_read
(
  sns_spi_info           *spi_info,
  uint16_t                reg_addr,
  uint8_t                *buffer,
  uint32_t                bytes,
  uint32_t               *read_count
)
{
  sns_rc           return_code = SNS_RC_SUCCESS;
  uint8_t          reg_addr8 = (uint8_t)reg_addr;

  if((bytes + 1) > SNS_SPI_BUF_SIZE)
  {
    return SNS_RC_INVALID_VALUE;
  }

  // If the SPI config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)spi_info))
  {
    SNS_ISLAND_EXIT();
  }

  sns_memset(sns_spi_read_buffer, 0, (bytes+1));

  reg_addr8 |= (1 << 7);   //set RW bit = 1 for read

  if (0 == sns_spi_init)
  {
    SNS_ISLAND_EXIT();
    sns_spi_init = 1;
  }
  sns_osa_lock_acquire(sns_spi_lock);
  spi_open(spi_info->dev_id);
  return_code = sns_spi_data_transfer(
                  spi_info,
                  sns_spi_read_buffer,
                  bytes + 1,
                  &reg_addr8,
                  sizeof(reg_addr8),
                  read_count );
  spi_close(spi_info->dev_id);
  sns_osa_lock_release(sns_spi_lock);
  if (return_code != SNS_RC_SUCCESS)
  {
    /* SNS_PRINTF(sns_fw_printf, _STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                              "spi_data_transfer failed %u",status); */
    return return_code;
  }
  else
  {
    *read_count = bytes;
  }

  sns_memscpy(buffer, bytes, &sns_spi_read_buffer[1], SNS_SPI_BUF_SIZE);
  //len+1 total spi transfers, 1 byte spi protocol overhead

  return SNS_RC_SUCCESS;
}

/**
*  @brief Performs SPI data transfer
*
* @param[in]   handle        The I/O port handle that identifies a device. This
*                            is given to drivers as part of initialization.
* @param[out]  read_buffer   Pointer to the buffer to which data read is deposited to.
*                            NULL allowed for write operation.
* @param[in]   read_len      Maximum number of bytes that can be stored in read_buffer.
*                            0 allowed for write operation.
* @param[in]   write_buffer  Pointer to buffer that contains the data to be written.
* @param[in]   write_len     Maximum number of bytes that can be stored in write_buffer.
* @param[out]  tx_count      Number of bytes transfered.
* @param[out]  write_time    System time write was performed, in SSC ticks << 5.
*
* @return SNS_DDF_SUCCESS if the operation was done successfully.
*         Otherwise SNS_DDF_EINVALID_PARAM or SNS_DDF_EBUS to indicate the error.
*
*/
SNS_SECTION(".text.sns") sns_rc sns_spi_data_transfer_timed
(
  sns_spi_info           *spi_info,
  uint8_t                *read_buffer,  //NULL allowed for write operation
  uint32_t                read_len,
  const uint8_t          *write_buffer,
  uint32_t                write_len,
  uint32_t               *tx_count,
  bool                    save_write_time
)
{


  SPI_RESULT        spi_result; //spi_errors.h
  spi_transaction_t read_transaction_info;
  spi_transaction_t write_transaction_info;
  spi_device_id_t   spi_dev_id;

  if (spi_info       == NULL ||
      write_buffer == NULL ||
      write_len    == 0)
  {
    return SNS_RC_INVALID_VALUE;
  }

  // If the SPI config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)spi_info))
  {
    SNS_ISLAND_EXIT();
  }

  spi_dev_id = spi_info->dev_id;

  read_transaction_info.buf_virt_addr = (void *)read_buffer;
  read_transaction_info.buf_phys_addr = (void *)read_buffer;
  read_transaction_info.buf_len       = read_len;

  write_transaction_info.buf_virt_addr = (void *)write_buffer;
  write_transaction_info.buf_phys_addr = (void *)write_buffer;
  write_transaction_info.buf_len       = write_len;

// Assumption: spi_open() is already called as part of sns_pwr_update_spi(.., true)

  if(save_write_time)
  {
    sns_time start_time = sns_get_system_time();
    // wait for next 32kHz clock tick, to align with clock edge
    do
    {
      spi_info->write_time = sns_get_system_time();
    }
    while ( spi_info->write_time == start_time );
    // Hard-code SPI bus initilization time
    spi_info->write_time += SPI_BUS_TXN_SETUP_TIME;
  }

  // Perform a full duplex transfer ----------------------
  if (0 == sns_spi_init)
  {
    SNS_ISLAND_EXIT();
    sns_spi_init = 1;
  }
  sns_osa_lock_acquire(sns_spi_lock);
  spi_open(spi_info->dev_id);
  spi_result = spi_full_duplex(spi_dev_id,
                               &spi_info->cfg,
                               &write_transaction_info, &read_transaction_info);
  spi_close(spi_info->dev_id);
  sns_osa_lock_release(sns_spi_lock);
  if ( spi_result != SPI_SUCCESS )
  {
    return SNS_RC_FAILED;
  }

  if ( tx_count != NULL )
  {
    *tx_count = read_len + write_len;  //public API backward compatibility
    //NOTE: *write_count / *read_count not implemented in SPI driver
    //Those should be the actual number of bytes written/read
  }

  return SNS_RC_SUCCESS;
}


/**
*  @brief Performs SPI data transfer
*
* @param[in]   handle        The I/O port handle that identifies a device. This
*                            is given to drivers as part of initialization.
* @param[out]  read_buffer   Pointer to the buffer to which data read is deposited to.
*                            NULL allowed for write operation.
* @param[in]   read_len      Maximum number of bytes that can be stored in read_buffer.
*                            0 allowed for write operation.
* @param[in]   write_buffer  Pointer to buffer that contains the data to be written.
* @param[in]   write_len     Maximum number of bytes that can be stored in write_buffer.
* @param[out]  read_count    Number of bytes transfered.
*
* @return SNS_DDF_SUCCESS if the operation was done successfully.
*         Otherwise SNS_DDF_EINVALID_PARAM or SNS_DDF_EBUS to indicate the error.
*
*/
SNS_SECTION(".text.sns") sns_rc sns_spi_data_transfer
(
  sns_spi_info           *spi_info,
  uint8_t                *read_buffer,  //NULL allowed for write operation
  uint32_t                read_len,
  const uint8_t          *write_buffer,
  uint32_t                write_len,
  uint32_t               *tx_count )
{
  sns_rc ret_val = SNS_RC_FAILED;

  // If the SPI config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)spi_info))
  {
    SNS_ISLAND_EXIT();
  }

  if (0 == sns_spi_init)
  {
    SNS_ISLAND_EXIT();
    sns_spi_init = 1;
  }
  sns_osa_lock_acquire(sns_spi_lock);
  spi_open(spi_info->dev_id);
  ret_val = sns_spi_data_transfer_timed(spi_info, read_buffer, read_len,
                                     write_buffer, write_len, tx_count, false);
  spi_close(spi_info->dev_id);
  sns_osa_lock_release(sns_spi_lock);
  return ret_val;
}

SNS_SECTION(".text.sns") sns_rc sns_ascp_register_spi_callback(void *port_handle,
                                      sns_ascp_callback callback,
                                      void* args)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  priv_handle->callback = callback;
  priv_handle->args = args;
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_async_vectored_rw_spi(void *port_handle,
                           uint8_t num_vectors,
                           sns_port_vector vectors[],
                           bool save_write_time)
{
  uint32_t xfer_bytes = 0;
  sns_rc rw_err = SNS_RC_FAILED;
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  rw_err = sns_vectored_rw_spi(port_handle,
                      vectors,
                      num_vectors,
                      save_write_time,
                      &xfer_bytes);
  priv_handle->callback(port_handle, num_vectors, vectors, xfer_bytes, rw_err, priv_handle->args);
  return SNS_RC_SUCCESS;
}

sns_sync_com_port_service_api spi_port_api SNS_SECTION(".data.sns")=
{
  .struct_len               = sizeof(sns_sync_com_port_service_api),
  .sns_scp_register_com_port = NULL,
  .sns_scp_deregister_com_port = NULL,
  .sns_scp_open             = &sns_open_spi,
  .sns_scp_close            = &sns_close_spi,
  .sns_scp_update_bus_power = &sns_pwr_update_spi,
  .sns_scp_register_rw      = &sns_vectored_rw_spi,
  .sns_scp_simple_rw        = &sns_simple_txfr_port_spi,
  .sns_scp_get_write_time   = &sns_get_write_time_spi,
  .sns_scp_get_version      = NULL
};

sns_ascp_port_api spi_async_port_api SNS_SECTION(".data.sns")=
{
  .struct_len                  = sizeof(sns_ascp_port_api),
  .sync_com_port               = &spi_port_api,
  .sns_ascp_register_callback  = &sns_ascp_register_spi_callback,
  .sns_ascp_register_rw        = &sns_async_vectored_rw_spi,
};

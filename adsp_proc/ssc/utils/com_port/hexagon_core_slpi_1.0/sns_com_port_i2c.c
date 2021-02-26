/**
 * sns_sync_com_port_i2c.c
 *
 * I2C implementation
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "I2cDriver.h"
#include "sns_assert.h"
#include "sns_async_com_port_int.h"
#include "sns_com_port_priv.h"
#include "sns_com_port_types.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_types.h"

/**--------------------------------------------------------------------------*/

#define SNS_DEFAULT_I2C_BUS_FREQ   400   // I2C bus frequency in KHz
#define SNS_DEFAULT_BYTE_XFER_TMO  2500  // byte xfer timeout in uS
#define I2C_BUS_TXN_SETUP_TIME     662   // 34.5uSec in the 19.2MHz clocks
#define PNOC_AB  10000  
#define PNOC_IB  10000 

/**--------------------------------------------------------------------------*/

typedef struct sns_i2c_info
{
  sns_reg_addr_type      reg_addr_type;
  I2cDrv_I2cBus          i2c_bus;
  // TODO: This will go away in new QuPv3 bus API
  sns_time               write_time;
} sns_i2c_info;

sns_sync_com_port_service_api i2c_port_api;
sns_ascp_port_api i2c_async_port_api;

/**--------------------------------------------------------------------------*/

SNS_SECTION(".text.sns") sns_rc sns_open_i2c(sns_sync_com_port_handle *port_handle)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_com_port_config *com_config = &priv_handle->com_config;
  int32_t          result;
  sns_i2c_info    *i2c_info;

  if (com_config->bus_instance <= 0
      ||
      com_config->bus_instance > I2CDRV_I2C_NUM)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "sns_open_i2c invalid instance %d", com_config->bus_instance);
    return SNS_RC_INVALID_VALUE;
  }

  bus_info->bus_config = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_i2c_info));
//  SNS_ASSERT(NULL != bus_info->bus_config);
  if (NULL == bus_info->bus_config)
  {
    // We ran out of island heap, so let's throw out a warning message and then
    // allocate the i2c config in DDR. This isn't ideal for power, but
    // will help with stability.
    SNS_ISLAND_EXIT();
    SNS_PRINTF(HIGH, sns_fw_printf, "Alloc I2C config in DDR!");
    bus_info->bus_config = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_i2c_info));
    SNS_ASSERT(NULL != bus_info->bus_config); // We're in big trouble if we can't allocate the I2C config
  }

  i2c_info = (sns_i2c_info *)bus_info->bus_config;

  /** Initialize member params */
  i2c_info->reg_addr_type = com_config->reg_addr_type;
  // I2C bus ID = (BLSP number in registry) - 1
  i2c_info->i2c_bus.i2cId = (I2cDrv_I2cBusId)(com_config->bus_instance - 1);
  i2c_info->i2c_bus.clntCfg.uSlaveAddr  = com_config->slave_control;
  i2c_info->i2c_bus.clntCfg.uBusFreqKhz = com_config->min_bus_speed_KHz;
  i2c_info->i2c_bus.clntCfg.uByteTransferTimeoutUs = SNS_DEFAULT_BYTE_XFER_TMO;

  /* Obtain the handle for the port. */
  result = I2cDrv_Open(i2c_info->i2c_bus.i2cId, &i2c_info->i2c_bus, 0);
  if(I2C_RES_SUCCESS != result)
  {
    sns_free(bus_info->bus_config);
    SNS_PRINTF(ERROR, sns_fw_printf, "I2cDrv_Open error 0x%X", result);
    return SNS_RC_FAILED;
  }

  return SNS_RC_SUCCESS;
}


SNS_SECTION(".text.sns") sns_rc sns_close_i2c(sns_sync_com_port_handle *port_handle)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_i2c_info *i2c_info = (sns_i2c_info *)bus_info->bus_config;

  // If the I2C config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)i2c_info))
  {
    SNS_ISLAND_EXIT();
  }

  I2cDrv_Close(&i2c_info->i2c_bus);
  sns_free(bus_info->bus_config);

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_pwr_update_i2c(sns_sync_com_port_handle *port_handle, bool power_bus_on)
{
  UNUSED_VAR(port_handle);
  UNUSED_VAR(power_bus_on);
 return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_pwr_update_i2c_internal(sns_sync_com_port_handle *port_handle, bool power_bus_on)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  int32_t         i2c_ret = I2C_RES_SUCCESS;
  I2cDrvCmdType   clock_cmd;
  I2cDrvCmdType  pnoc_cmd;
  I2cDrvPnocVoteData vote_data;
  sns_i2c_info   *i2c_info = (sns_i2c_info *)bus_info->bus_config;

  // If the I2C config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)i2c_info))
  {
    SNS_ISLAND_EXIT();
  }

  //SNS_PRINTF(HIGH, sns_fw_printf, "sns_pwr_update_i2c vote %d", power_bus_on);

  if(power_bus_on)
  {
    clock_cmd = I2cDrvCmd_Vote_I2cClocsOn;
    pnoc_cmd = I2cDrvCmd_Vote_PnocOn;
    vote_data.uPnocAb = PNOC_AB;
    vote_data.uPnocIb = PNOC_IB;
  }
  else
  {
    clock_cmd = I2cDrvCmd_Vote_I2cClocsOff;
    pnoc_cmd = I2cDrvCmd_Vote_PnocOff;
    vote_data.uPnocAb = 0;
    vote_data.uPnocIb = 0;
  }

  i2c_ret = I2cDrv_Cmd(&i2c_info->i2c_bus, clock_cmd, NULL);
  if(I2C_RES_SUCCESS != i2c_ret)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "I2cDrv_Cmd clock_cmd error 0x%X", i2c_ret);
    return SNS_RC_FAILED;
  }

  i2c_ret = I2cDrv_Cmd(&i2c_info->i2c_bus, pnoc_cmd, &vote_data);
  if(I2C_RES_SUCCESS != i2c_ret)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "I2cDrv_Cmd pnoc_cmd error 0x%X", i2c_ret);
    return SNS_RC_FAILED;
  }

  return SNS_RC_SUCCESS;
}


SNS_SECTION(".text.sns") sns_rc sns_get_write_time_i2c(sns_sync_com_port_handle *port_handle, sns_time *write_time)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_i2c_info  *i2c_info = (sns_i2c_info *)bus_info->bus_config;

  if(write_time == NULL)
  {
    return SNS_RC_INVALID_VALUE;
  }

  // If the I2C config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)i2c_info))
  {
    SNS_ISLAND_EXIT();
  }

  *write_time = i2c_info->write_time;

  return SNS_RC_SUCCESS;
}

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
  int32_t           result;
  I2cSimpleTrObj    tr_obj;
  uint32_t          data_bytes = 0;
  I2cTranDirection  xfer_dir;
  if((0 == bytes) && (NULL == buffer))
  {
    return SNS_RC_NOT_SUPPORTED;
  } 
  sns_pwr_update_i2c_internal(port_handle, true);

  int32_t(*xfer_fn)(I2cDrv_I2cBus *pI2cBus,
                    I2cTransfer   *pTransfer,
                    uint32        *puNumCompleted);

  if(is_write)
  {
    xfer_fn = I2cDrv_Write;
    xfer_dir = I2cTranDirOut;
  }
  else
  {
    xfer_fn = I2cDrv_Read;
    xfer_dir = I2cTranDirIn;
  }

  if((NULL == i2c_info) || (NULL == buffer) || (NULL == xfer_bytes))
  {
    return SNS_RC_INVALID_VALUE;
  }

  // If the I2C config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)i2c_info))
  {
    SNS_ISLAND_EXIT();
  }

  /* Init count */
  *xfer_bytes = 0;

  I2c_TrObj_Init(&tr_obj.aTr[0], &tr_obj.io_vec[0], xfer_dir,
                 I2cTrCtxNotASequence);

  I2c_TrObj_AddBuffer(&tr_obj.aTr[0], 0, buffer, bytes);

  if(save_write_time)
  {
    sns_time start_time = sns_get_system_time();
    // wait for next 32kHz clock tick, to align with clock edge
    do
    {
      i2c_info->write_time = sns_get_system_time();
    }
    while ( i2c_info->write_time == start_time );
    // Hard-code I2C bus initilization time
    i2c_info->write_time += I2C_BUS_TXN_SETUP_TIME;
  }

  /* Start a I2C transfer */
  result = xfer_fn(&i2c_info->i2c_bus, &tr_obj.aTr[0], &data_bytes);
  if ( I2C_RES_SUCCESS != result )
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "xfer_fn error 0x%X", result);
    return_code = SNS_RC_FAILED;
  }

  *xfer_bytes = (uint32_t)data_bytes;

  sns_pwr_update_i2c_internal(port_handle, false);

  return return_code;
}

SNS_SECTION(".text.sns") sns_rc sns_vectored_rw_i2c(sns_sync_com_port_handle *port_handle,
                           sns_port_vector *vectors,
                           int32_t num_vectors,
                           bool save_write_time,
                           uint32_t *xfer_bytes)
{

  int32_t i;
  I2cRegSeqObj reg_obj[num_vectors];
  I2cIoResult  ioRes;
  int32_t      result;
  I2cSequence  seq;
  I2cTransfer  transfer[num_vectors * 2];
  int32_t      txn_count = 0;
  I2cRegOffsetType offset_type;
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  sns_bus_info *bus_info = &priv_handle->bus_info;
  sns_i2c_info *i2c_info = (sns_i2c_info *)bus_info->bus_config;
  uint8_t reg_size = 0;

  // If the I2C config was allocated in DDR, then vote to exit island mode
  if (!sns_island_is_island_ptr((intptr_t)i2c_info))
  {
    SNS_ISLAND_EXIT();
  }

  switch(i2c_info->reg_addr_type)
  {
    case SNS_REG_ADDR_8_BIT:
      offset_type = I2cRegOffset_8Bit;
      reg_size = sizeof(uint8_t);
      break;
    case SNS_REG_ADDR_16_BIT:
      offset_type = I2cRegOffset_16Bit;
      reg_size = sizeof(uint16_t);
      break;
    case SNS_REG_ADDR_32_BIT:
      offset_type = I2cRegOffset_16Bit;// TODO: need I2cRegOffset_32Bit;
      reg_size = sizeof(uint16_t);
      break;
    default:
      offset_type = I2cRegOffset_16Bit;
      reg_size = sizeof(uint16_t);
      break;
  }

  sns_pwr_update_i2c_internal(port_handle, true);

  for( i = 0; i < num_vectors; i++ )
  {
    if( vectors[i].is_write )
    {
      I2c_RegObj_InitWrite(&reg_obj[i]);
      I2c_RegObj_SetWriteIoVector(&reg_obj[i], vectors[i].buffer, vectors[i].bytes);
    }
    else
    {
      I2c_RegObj_InitRead(&reg_obj[i]);
      I2c_RegObj_SetReadIoVector(&reg_obj[i], vectors[i].buffer, vectors[i].bytes);
    }



    I2c_RegObj_SetOffset(&reg_obj[i], vectors[i].reg_addr, offset_type);

    transfer[txn_count] = reg_obj[i].aTr[0];
    transfer[txn_count].eTranCtxt = I2cTrCtxSeqContinue;
    if( !vectors[i].is_write )
    {
      txn_count++;
      transfer[txn_count] = reg_obj[i].aTr[1];
      transfer[txn_count].eTranCtxt = I2cTrCtxSeqContinue;
    }
    txn_count++;
  }

  if( txn_count == 1 )
  {
    transfer[txn_count - 1].eTranCtxt = I2cTrCtxNotASequence;
  }
  else
  {
    transfer[0].eTranCtxt = I2cTrCtxSeqStart;
    transfer[txn_count - 1].eTranCtxt = I2cTrCtxSeqEnd;
  }

  I2c_SeqObj_Init( &seq, transfer, txn_count);

  if(save_write_time)
  {
    sns_time start_time = sns_get_system_time();
    // wait for next 32kHz clock tick, to align with clock edge
    do
    {
      i2c_info->write_time = sns_get_system_time();
    }
    while ( i2c_info->write_time == start_time );
    // Hard-code I2C bus initilization time
    i2c_info->write_time += I2C_BUS_TXN_SETUP_TIME;
  }

  result = I2cDrv_BatchTransfer(&i2c_info->i2c_bus,
                                &seq,
                                &ioRes);
  if(xfer_bytes != NULL )
  {
    *xfer_bytes = (uint32_t)(ioRes.uOutByteCnt) +
                  (uint32_t)(ioRes.uInByteCnt) -
                  (num_vectors * reg_size);
  }

  if (result != I2C_RES_SUCCESS)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "xfer_fn error 0x%X", result);
    sns_pwr_update_i2c_internal(port_handle, false);
    return SNS_RC_FAILED;;
  }

  sns_pwr_update_i2c_internal(port_handle, false);
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_ascp_register_i2c_callback(void  *port_handle,
                                      sns_ascp_callback callback,
                                      void* args)
{
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;
  priv_handle->callback = callback;
  priv_handle->args = args;
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc sns_async_vectored_rw_i2c(void  *port_handle,
                           uint8_t num_vectors,
                           sns_port_vector vectors[],
                           bool save_write_time)
{
  uint32_t xfer_bytes = 0;
  sns_rc rw_err = SNS_RC_FAILED;
  sns_com_port_priv_handle *priv_handle = (sns_com_port_priv_handle *)port_handle;

  sns_pwr_update_i2c_internal(port_handle, true);

  rw_err = sns_vectored_rw_i2c(port_handle,
                      vectors,
                      num_vectors,
                      save_write_time,
                      &xfer_bytes);
  priv_handle->callback(port_handle, num_vectors, vectors, xfer_bytes, rw_err, priv_handle->args);

  sns_pwr_update_i2c_internal(port_handle, false);

  return SNS_RC_SUCCESS;
}
sns_sync_com_port_service_api i2c_port_api SNS_SECTION(".data.sns")=
{
  .struct_len               = sizeof(sns_sync_com_port_service_api),
  .sns_scp_register_com_port = NULL,
  .sns_scp_deregister_com_port = NULL,
  .sns_scp_open             = &sns_open_i2c,
  .sns_scp_close            = &sns_close_i2c,
  .sns_scp_update_bus_power = &sns_pwr_update_i2c,
  .sns_scp_register_rw      = &sns_vectored_rw_i2c,
  .sns_scp_simple_rw        = sns_simple_txfr_port_i2c,
  .sns_scp_get_write_time   = &sns_get_write_time_i2c,
  .sns_scp_get_version      = NULL
};

sns_ascp_port_api i2c_async_port_api SNS_SECTION(".data.sns") =
{
  .struct_len                  = sizeof(sns_ascp_port_api),
  .sync_com_port               = &i2c_port_api,
  .sns_ascp_register_callback  = &sns_ascp_register_i2c_callback,
  .sns_ascp_register_rw        = &sns_async_vectored_rw_i2c,
};

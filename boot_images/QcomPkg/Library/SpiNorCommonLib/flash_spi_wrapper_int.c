/*====================================================================
 *
 * FILE:        flash_spi_wrapper_int.c
 *
 * SERVICES:    Interrupt mode SPI support for flash drivers
 *
 * DESCRIPTION: SPI support for all flash drivers
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 08/21/12     sb      DAL SPI dependency is only for MPSS
 * 10/06/11     sv      SPI NOR command log support
 * 08/25/11     sv      SPI NOR support for 9x15
 * 07/20/11     sv      Virtual to Physical addr conversion changes
 * 03/10/11     eo      Fix issue in event timeout
 * 02/10/11     eo      Remove workaround code forcing block mode transfers   
 * 02/01/11     eo      Add Maxcronix SPI NOR device support
 * 09/02/10     sv      NOR tools support
 * 08/11/10     nr      Added stats framework support
 * 06/29/10     eo      Add Babylon support
 * 06/23/10     eo      Support SPI DAL Device ID configuration
 * 04/06/10     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "SpiApi.h"
   
#include "flash_nor_spi.h"
#include "flash_nor_config.h"
#include "flash_spi_wrapper.h"
#include "flash_dal_util.h"

#define  FLASH_SPI_EVENT_TIMEOUT_US 3000000 /* 3 sec */
/* Minimun Transfer length supported by DAL SPI Driver */
#define SPI_MIN_TX_LEN 4

/* SPI_NOR_FREQ_HZ can be defined in the .builds file if any
 * other frequency other than the default(20MHz) is required.
 */
#ifndef SPI_NOR_FREQ_HZ
  #define SPI_NOR_FREQ_HZ  20000000
#endif

/* Flash SPI memory information  */
struct flash_spi_meminfo
{
    DALSYSMemHandle  handle_spi_mem;   /* SPI Workspace mem handle */
    DALSYSMemInfo    spi_mem_info;     /* SPI Workspace mem info */
};

struct flash_spi_data_addr
{
  uint8 *data_addr;
  uint32 data_len;
};

/* SPI transfer buffer information */
struct flash_spi_xfer_buf_info
{
  struct flash_spi_data_addr tx_data;
  struct flash_spi_data_addr rx_data;
};

/* Flash SPI Interface properties */
struct flash_spi_props
{
  struct flash_spi_xfer_buf_info spi_xfer_data; /* SPI xfer output info */
  uint8 *transfer_buf;                       /* Buffer used for spi xfers */
  struct flash_spi_meminfo mem_info;         /* SPI memory allocation info */
};

/* SPI device handle */
void *spi_handle = NULL;

/* used to determine if cache maintenance is required */
extern struct nor_entry_data nor_entry_init_data;

/* Global variable for flash SPI properties */
struct flash_spi_props flash_spi_info; // = {{0}};

/* Global to store SPI NOR Debug info */
struct flash_nor_spi_dbg_info nor_spi_dbg_info; // = {{0}};

/* Global to stort spi core instance */
spi_instance flash_nor_spi_instance = SPI_INSTANCE_001;

/* SPI bus device configurations */
static struct flash_spi_core_config nor_spi_device_cfg =
{
  {SPI_CLK_NORMAL,
   SPI_CLK_IDLE_HIGH,     /* Same as NOR tools */
   SPI_OUTPUT_FIRST_MODE, /* Same as NOR tools */
   0,                    /* De-assertion time */
   0,                    /* Min slave freq (Hz) */
   SPI_NOR_FREQ_HZ,      /* Max slave freq (Hz) */
   SPI_CS_ACTIVE_LOW,
   SPI_CS_KEEP_ASSERTED
  },
  {0,                    /* Chip Select Slave uses */
   SPI_CORE_MODE_MASTER
  },
  {8,                    /* N: number of bits tranferred in 1 SPI xfer */
   SPI_LOOPBACK_DISABLED
//   SPI_INPUT_PACKING_DISABLED,
//   SPI_OUTPUT_UNPACKING_DISABLED,
//   0                     /* Slave time out in usec */
  }
};

static SpiDeviceInfoType spi_config_info;

/* SPI Bus get master device settings */
static void flash_spi_get_master_setting(SpiDeviceInfoType *spi_cfg_info,
struct flash_spi_core_config *spi_cfg)
{
  SpiDeviceParametersType *dev_param;
  SpiDeviceBoardInfoType *dev_board_info;
  SpiTransferParametersType *dev_xfer_param;
  
  dev_param = (SpiDeviceParametersType *)&spi_cfg_info->deviceParameters;
  dev_board_info = (SpiDeviceBoardInfoType *)&spi_cfg_info->deviceBoardInfo;
  dev_xfer_param = (SpiTransferParametersType *)&spi_cfg_info->transferParameters;

  /* Initialize SPI device parameters */
  dev_param->eClockMode = 
    (SpiClockModeType)spi_cfg->dev_param.clock_mode;
  dev_param->eClockPolarity = 
    (SpiClockPolarityType)spi_cfg->dev_param.clock_polarity;
  dev_param->eShiftMode = 
    (SpiShiftModeType)spi_cfg->dev_param.shift_mode;
  dev_param->u32DeassertionTime = 
    spi_cfg->dev_param.cs_deassert_time;
  dev_param->u32MinSlaveFrequencyHz = 
    spi_cfg->dev_param.min_slave_freq_hz;
  dev_param->u32MaxSlaveFrequencyHz = 
    spi_cfg->dev_param.max_slave_freq_hz;
  dev_param->eCSPolarity = 
    (SpiCSPolarityType)spi_cfg->dev_param.cs_polarity;
  dev_param->eCSMode = 
    (SpiCSModeType)spi_cfg->dev_param.cs_mode;
  dev_param->bHSMode = TRUE;
  
  /* Initialize SPI device board information */
  dev_board_info->nSlaveNumber = 
    spi_cfg->dev_board.cs_num;
  dev_board_info->eCoreMode = 
    (SpiCoreModeType)spi_cfg->dev_board.core_mode;

  /* Initialize SPI device transfer parameters */
  dev_xfer_param->nNumBits = 
    spi_cfg->dev_transfer.num_bits;
  dev_xfer_param->eLoopbackMode = 
    (SpiLoopbackModeType)spi_cfg->dev_transfer.loopback_mode;

} // flash_spi_get_master_setting

/* Function to configure the SPI device */
static int flash_spi_dev_config()
{
  int result = FLASH_DEVICE_FAIL;

  flash_spi_get_master_setting(&spi_config_info, &nor_spi_device_cfg);

  return result;
}

/*  This is a SPI transfer wrapper for SPI driver. It transfers
 *  the flash command and data to the Flash device through the 
 *  SPI Interface
 */
static int flash_spi_transfer(struct flash_spi_xfer_buf_info *spi_transfer_info,
  uint32 cmd_len, uint32 data_len)
{
  int result = FLASH_DEVICE_DONE; 
  spi_status tmp_result = SPI_SUCCESS;
  uint32 spi_cmd_log_indx = 0;
  uint32 spi_cmd_log_sz = 0;

  FLASH_CORE_PROFILE_START(FLASH_PROFILE_BUS_TRANSFER, FLASH_STATS_DEFAULT_NPAGES);

  if(cmd_len >= NOR_SPI_CMD_LOG_BUF_SIZE)
  {
    spi_cmd_log_sz = NOR_SPI_CMD_LOG_BUF_SIZE;
  }
  else
  {
    spi_cmd_log_sz = cmd_len;
  }	   
 
  spi_cmd_log_indx = nor_spi_dbg_info.nor_spi_next_cmd_log_indx;
  DALSYS_memset(nor_spi_dbg_info.nor_spi_cmd_log[spi_cmd_log_indx].spi_cmd_buf,
    0, NOR_SPI_CMD_LOG_BUF_SIZE);
  DALSYS_memcpy(nor_spi_dbg_info.nor_spi_cmd_log[spi_cmd_log_indx].spi_cmd_buf,
   (uint8 *)spi_transfer_info->tx_data.data_addr, spi_cmd_log_sz);
    
  nor_spi_dbg_info.nor_spi_cmd_log[spi_cmd_log_indx].spi_total_tx_bytes =
    cmd_len;

  nor_spi_dbg_info.nor_spi_next_cmd_log_indx++;
  if (nor_spi_dbg_info.nor_spi_next_cmd_log_indx >= NOR_SPI_MAX_CMD_LOG_ENTRY)
  {
    nor_spi_dbg_info.nor_spi_next_cmd_log_indx = 0;
  }    

  tmp_result = spi_transfer(spi_handle, &spi_config_info, 
    spi_transfer_info->tx_data.data_addr, spi_transfer_info->tx_data.data_len,
    spi_transfer_info->rx_data.data_addr, spi_transfer_info->rx_data.data_len);

  /* Error fatal, if the SPI transfer is failed */
  if (SPI_SUCCESS != tmp_result)
  {
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                        "Flash: SPI Transfer failed!");
	
    result = FLASH_DEVICE_FAIL;
  }

 FLASH_CORE_PROFILE_END(FLASH_PROFILE_BUS_TRANSFER);

  return result;
}

/* This function initializes SPI specific data structures
              The following actions are performed:
                1.  Attach the SPI device
                2.  Initialize flash SPI transfer request.
*/
int flash_spi_init(flash_client_ctxt *client_ctxt)
{
  spi_instance spi_device_id = flash_nor_spi_instance;
  uint32 alloc_attr;
  uint32 spi_mem_size, temp_addr;
  struct flash_spi_meminfo *spi_mem = &flash_spi_info.mem_info; 

  DALSYS_InitMod(NULL);

  /* Open the SPI device */
  if(SPI_SUCCESS != spi_open(spi_device_id, &spi_handle))
  {
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
      "Flash: Failed to open SPI!");

    return FLASH_DEVICE_FAIL;
  }

  /* Get the driver workspace memory size from property table */
  spi_mem_size = flash_get_property(FLASH_SPI_WS_BUFF_SIZE_ID);

  if (FLASH_PROPERTY_NOT_DEFINED == spi_mem_size)
  {
    FLASHLOG(2, ("Flash: Get SPIWS Memory property failed!\n"));
    return FLASH_DEVICE_FAIL;
  }

  /* Set the allocation attribute to allocate uncached - physically contiguous
   * region
   */
  alloc_attr = DALSYS_MEM_PROPS_PHYS_CONT | DALSYS_MEM_PROPS_UNCACHED;

  /* Workspace Memory for SPI WRAPPER usage */
  if (DAL_SUCCESS != DALSYS_MemRegionAlloc(alloc_attr,
    DALSYS_MEM_ADDR_NOT_SPECIFIED, DALSYS_MEM_ADDR_NOT_SPECIFIED,
    spi_mem_size, &spi_mem->handle_spi_mem, NULL))
  {
    FLASHLOG(2, ("Flash: DRVWS Memory allocation failed!\n"));
    return FLASH_DEVICE_FAIL;
  }
  else
  {
    /* If the allocation is success, get the memory address
     * information */
    DALSYS_MemInfo(spi_mem->handle_spi_mem, &spi_mem->spi_mem_info);

    /* Add SPI memory details to the virtual
     * to physical conversion list */
    flash_vtop_add_item(
      spi_mem->spi_mem_info.VirtualAddr,
      spi_mem->spi_mem_info.PhysicalAddr,
      spi_mem->spi_mem_info.dwLen);

    temp_addr = FLASH_ALIGN_32(spi_mem->spi_mem_info.VirtualAddr);

    /* Data buffer used for SPI input/output transfers */
    flash_spi_info.transfer_buf = (uint8 *)((UINTN)temp_addr);
  }

  return FLASH_DEVICE_DONE;
}

/*  This function detaches the data mover handle.
 */
int flash_spi_close(void)
{
  int result = FLASH_DEVICE_DONE;

  /* Close the SPI device handle */
  if(SPI_SUCCESS != spi_close(spi_handle))
  {
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
      "Flash: Failed to close SPI!");

    return FLASH_DEVICE_FAIL;
  }

  if (flash_spi_info.mem_info.handle_spi_mem != 0)
  {
    /* Free memory allocated by the driver */
    if (DAL_SUCCESS != DALSYS_DestroyObject(
      (DALSYSObjHandle) flash_spi_info.mem_info.handle_spi_mem))
    {
      result = FLASH_DEVICE_FAIL;
    }
    else
    {        
      flash_spi_info.mem_info.handle_spi_mem = 0;
    }
  }    

  return result;
}

/*  This is SPI read wrapper for SPI driver. This is used to read data from a   
 *  device connected through SPI Interface
 */
void flash_spi_read_op(struct nor_spi_xfer_buf_info *flash_buf_info, int *status)
{
  uint32 phys_addr = 0;
  struct flash_spi_data_addr *spi_tx_xfer_info;
  struct flash_spi_data_addr *spi_rx_xfer_info;

  spi_tx_xfer_info = &flash_spi_info.spi_xfer_data.tx_data;
  spi_rx_xfer_info = &flash_spi_info.spi_xfer_data.rx_data;

  phys_addr = flash_vtop_get_physical((uint32)flash_buf_info->spi_cmd_buf);
  spi_tx_xfer_info->data_addr = (uint8 *)((UINTN)phys_addr);
  spi_tx_xfer_info->data_len = flash_buf_info->cmd_buf_len;

  phys_addr = flash_vtop_get_physical((uint32)flash_spi_info.transfer_buf);
  spi_rx_xfer_info->data_addr = (uint8 *)((UINTN)phys_addr);
  
  spi_rx_xfer_info->data_len = flash_buf_info->cmd_buf_len + flash_buf_info->data_buf_len;
  
  *status = flash_spi_transfer(&flash_spi_info.spi_xfer_data, 
    flash_buf_info->cmd_buf_len, flash_buf_info->data_buf_len);

  //FLASH_CORE_PROFILE_START(FLASH_STATS_DAL_MEMCPY, 0x1);

  DALSYS_memcpy(flash_buf_info->spi_data_buf, 
    &flash_spi_info.transfer_buf[flash_buf_info->cmd_buf_len], 
    flash_buf_info->data_buf_len);
 
  //FLASH_CORE_PROFILE_END(FLASH_STATS_DAL_MEMCPY);
}

/*  This is SPI write wrapper for SPI driver. This is used to write data to a   
 *  device connected through SPI Interface
 */
void flash_spi_write_op(struct nor_spi_xfer_buf_info *flash_buf_info, int *status)
{
  uint32 phys_addr = 0;
  struct flash_spi_data_addr *spi_tx_xfer_info;
  struct flash_spi_data_addr *spi_rx_xfer_info;

  spi_tx_xfer_info = &flash_spi_info.spi_xfer_data.tx_data;
  spi_rx_xfer_info = &flash_spi_info.spi_xfer_data.rx_data;

  //FLASH_CORE_PROFILE_START(FLASH_STATS_DAL_MEMCPY, 0x1);

  DALSYS_memcpy(&flash_spi_info.transfer_buf[0], 
    flash_buf_info->spi_cmd_buf, flash_buf_info->cmd_buf_len);

  DALSYS_memcpy(&flash_spi_info.transfer_buf[flash_buf_info->cmd_buf_len], 
    flash_buf_info->spi_data_buf, flash_buf_info->data_buf_len);

  //FLASH_CORE_PROFILE_END(FLASH_STATS_DAL_MEMCPY);

  spi_tx_xfer_info = &flash_spi_info.spi_xfer_data.tx_data;
  spi_rx_xfer_info = &flash_spi_info.spi_xfer_data.rx_data;

  phys_addr = flash_vtop_get_physical((uint32)flash_spi_info.transfer_buf);
  spi_tx_xfer_info->data_addr = (uint8 *)((UINTN)phys_addr);
  spi_tx_xfer_info->data_len = flash_buf_info->cmd_buf_len + flash_buf_info->data_buf_len;

  spi_rx_xfer_info->data_addr  = NULL;
  spi_rx_xfer_info->data_len = 0;

  *status = flash_spi_transfer(&flash_spi_info.spi_xfer_data, 
    flash_buf_info->cmd_buf_len, flash_buf_info->data_buf_len);
  
}

/*
 * Init/Configure the SPI wrapper APIs
 */
void flash_spi_configure(void *spi_configs)
{
  struct nor_spi_wrapper *wrapper = (struct nor_spi_wrapper *)spi_configs;

  if (NULL != wrapper)
  {
    wrapper->init = flash_spi_init;
    wrapper->close = flash_spi_close;
    wrapper->dev_cfg = flash_spi_dev_config;
    wrapper->read_op = flash_spi_read_op;
    wrapper->write_op = flash_spi_write_op;
  }
}

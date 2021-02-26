/*=============================================================================
 *
 * FILE:      flash_nor_spi_init.c
 *
 * DESCRIPTION: This file initializes the flash SPI-NOR driver.
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010-2012, 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 03/18/14     whc     Add device operating frequency support
 * 09/03/12     sb      Clean up
 * 03/19/12     sv      SPI NOR power management changes
 * 12/14/11     eo      Fix compiler warnings
 * 11/01/11     eo      Remove unnecessary function label
 * 10/06/11     sv      Use 8 blocks for SFS parti for 2MB SPI NOR
 * 07/20/11     eo      Add virtual to physical address changes
 * 05/20/11     sv      Flash Driver Optimization
 * 03/10/11     sv      Invoke flash_spi_close() in nor_spi_close()
 * 03/07/11     sv      Remove sleep module initialization
 * 10/07/10     sv      Fix issue with page_size_shift with multi clients 
 * 10/04/10     sv      Move SPI init and read, write wrappers to here
 * 09/13/10     eo      Add formalized partition support
 * 09/02/10     sv      SPI NOR tools support
 * 08/18/10     eo      Add interim partition support
 * 06/21/10     eo      Add Power management support
 * 03/15/10     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nor_spi.h"

/* Aligns an address to 32 byte */
#define FLASH_ALIGN(a) ((a)&0x1F)?((((a)+32)>>5)<<5):(a)

/*------------------------------------------------------------------------------
  Variables global in this driver.
 ------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Extern variable(s).
 ------------------------------------------------------------------------------*/
/* SPI NOR base virtual table */
extern flash_vtbl nor_spi_base_vtbl;

/* NOR entry init data */
extern struct nor_entry_data nor_entry_init_data;

struct nor_spi_wrapper nor_spi_ops;  /* NOR SPI function pointers */

/*------------------------------------------------------------------------------
  Following functions are local to this driver.
 ------------------------------------------------------------------------------*/

/*
 * SPI initialization
 */
static int nor_spi_init(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_FAIL;
  flash_spi_config_tbl *spi_tbl;

  spi_tbl = (flash_spi_config_tbl *)flash_get_property(
    FLASH_SPI_CONFIG_TABLE_ID);

  if (FLASH_PROPERTY_NOT_DEFINED != (uint32)spi_tbl)
  {
    if (spi_tbl && spi_tbl->spi_config)
    {
      spi_tbl->spi_config(&nor_spi_ops);

      if (FLASH_DEVICE_DONE ==
        nor_spi_ops.init(client_ctxt))
      {
        result = FLASH_DEVICE_DONE;
      }
    }
  }

  return result;
}

/* Wrapper function to initialize SPI device paramaters */
void nor_spi_dev_set_params(void)
{
  nor_spi_ops.dev_cfg();
}

/* Wrapper function for spi read operation */
void nor_spi_read(struct nor_spi_xfer_buf_info *spi_buf_info, int *status)
{
  nor_spi_ops.read_op(spi_buf_info, status);
}

/* Wrapper function for spi write operation */
void nor_spi_write(struct nor_spi_xfer_buf_info *spi_buf_info, int *status)
{
  nor_spi_ops.write_op(spi_buf_info, status);
}

/*------------------------------------------------------------------------------
  Following functions are extended in flash_handle Interface.
 ------------------------------------------------------------------------------*/
/*
 * Power event
 */
static int nor_spi_power_event(flash_client_ctxt *client_ctxt, DalPowerCmd power_cmd,
  DalPowerDomain power_domain)
{
  return FLASH_DEVICE_DONE;
}

/*
 * Allocate memory for all driver usage
 */
static int nor_spi_mem_init(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_FAIL;
  uint32 size_bytes = 0;
  struct nor_spi_dev_pdata *nor_spi_pdata;
  struct nor_flash_meminfo *meminfo;

  meminfo = &nor_entry_init_data.meminfo;

  nor_spi_pdata = GET_NOR_PDATA(client_ctxt);

  if (TRUE == nor_entry_init_data.nor_mem_initialized)
  {
    /* Allocate even number bytes for cmd and address bytes */
    nor_spi_pdata->cmd_addr_buffer =
      (uint8 *)meminfo->drv_mem_info.VirtualAddr;

    size_bytes += NOR_SPI_CMD_ADDR_BUF_LEN;

   /* Allocate even number bytes for status bytes */
    nor_spi_pdata->status_buffer =
      (uint8 *)(meminfo->drv_mem_info.VirtualAddr + size_bytes);

    result = FLASH_DEVICE_DONE;
  }

  return result;
}

/*
 * De-allocate all flash driver allocated memory
 */
static int nor_spi_mem_deinit(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);

  if (nor_spi_pdata)
  {
    DALSYS_Free(nor_spi_pdata);
  }
  
  client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data = 0;

  return result;
}

/*
 * Close a NOR client
 */
static int nor_spi_close(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;
  
  /* If alloted, release primary dev_id/hal_id */
  if (client_ctxt->client_data.client_specific_data)
  {
    /* Free any malloced extra data for client */
    result = DALSYS_Free(client_ctxt->client_data.client_specific_data);
    client_ctxt->client_data.client_specific_data = 0;
    FLASHLOG(3, ("Freed client extra data in nor_spi_close\n"));
  }

  return result;
}

/*
 * De-initializes the NOR SPI driver
 */
static int nor_spi_deinit (flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;

  if (FLASH_DEVICE_DONE != nor_spi_ops.close())
  {
    result = FLASH_DEVICE_FAIL;
  }

  /* Free any malloced extra data for client */
  nor_spi_mem_deinit(client_ctxt);

  /* Release any allocated device private data */
  if (client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data)
  {
    DALSYS_Free(
      client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data);
    client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data = NULL;
  }

  if (client_ctxt->flash_dev_ctxt->parti_ptr)
  {
    /* Free any malloced extra data for client */
    DALSYS_Free(client_ctxt->flash_dev_ctxt->parti_ptr);
    client_ctxt->flash_dev_ctxt->parti_ptr = NULL;
  }

  FLASHLOG(3, ("Freed any device specific data (shared across clients)\n"));

  return result;
}

/*
 * Initilializes the client information
 */
int nor_spi_init_client_info(flash_client_ctxt *client_ctxt)
{
  int status = FLASH_DEVICE_DONE;
  uint32 block_size;
  uint32 page_size;
  struct nor_dev_data *dev_data;
  struct flash_client_data *client_data = &client_ctxt->client_data;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  dev_data = &dev_ctxt->dev_info.nor;

  /* Init Client context */
  block_size = dev_data->pages_per_block;
  page_size = dev_data->page_size_bytes;

  client_data->pages_per_block = dev_data->pages_per_block;
  client_data->page_size_bytes = dev_data->page_size_bytes;
  client_data->total_page_size_bytes = dev_data->page_size_bytes;
  client_data->base_address = dev_data->base_address;
  client_data->block_size_shift  = 0;

  /* Find the nth bit set to determine number of block size shifts */
  while (!(block_size & 0x1))
  {
    block_size >>= 1;
    ++client_data->block_size_shift;
  }

  client_data->page_size_shift  = 0;
  /* Find the nth bit set to determine number of page size shifts */
  while (!(page_size & 0x1))
  {
    page_size >>= 1;
    ++client_data->page_size_shift;
  }

  dev_ctxt->dev_base_vtbl = &nor_spi_base_vtbl;

  /* Client specific data is unused */
  client_ctxt->client_data.client_specific_data = NULL;

  /* Set Client Ctxt default Vtbl to the Base Vtbl in Device Ctxt */
  client_ctxt->device_type_vtbl = dev_ctxt->dev_base_vtbl;

  return status;
}

/*
 * Open partition for a nor client and init client info
 */
static int nor_spi_open(flash_client_ctxt *client_ctxt)
{
  int    status = FLASH_DEVICE_DONE;

  FLASHLOG(4, ("nor_spi_open called\n"));

  /* Initialize client info */
  status = nor_spi_init_client_info(client_ctxt);

  return status;
}

/*
 * Configure the device ctxt fields
 */
static int nor_spi_configure_device_ctxt(flash_client_ctxt *client_ctxt,
  struct flash_nor_params *devices)
{
  int status = FLASH_DEVICE_DONE;
  uint32 unit_size;
  struct nor_dev_data *dev_data;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  /* Initialize one time device context Information */
  dev_ctxt->dev_base_vtbl = &nor_spi_base_vtbl;
  dev_data = &dev_ctxt->dev_info.nor;

  dev_ctxt->dev_info.common.device_name = 
    (const char *)&devices->device_name[0];
  dev_ctxt->dev_info.common.type = devices->type;
  dev_ctxt->dev_info.common.width = (enum flash_width)devices->width;
  dev_ctxt->dev_info.common.write_style = devices->write_style;

  /* Copy the device name */
  flash_strcpy((uint8 *) dev_data->device_name,
   (uint8 *)&devices->device_name[0], 32);

  dev_data->num_ids = devices->num_ids;
  dev_data->dev_ids[0] = devices->dev_ids[0];
  dev_data->dev_ids[1] = devices->dev_ids[1];
  dev_data->dev_ids[2] = devices->dev_ids[2];
  dev_data->dev_ids[3] = devices->dev_ids[3];
  dev_data->family = devices->family;
  dev_data->block_count = devices->block_count;
  dev_data->pages_per_block = devices->pages_per_block;
  dev_data->page_size_bytes = devices->page_size_bytes;
  dev_data->device_size_MB = devices->device_size_MB;
  dev_data->wbuffer_size_bytes = devices->wbuffer_size_bytes;
  dev_data->flags1 = devices->flags1;
  dev_data->clk = devices->clk;
  dev_data->base_address = devices->base_address;
  dev_data->dpd_delay_time_us = devices->dpd_delay_time_us;
  dev_data->rel_dpd_delay_time_us = devices->rel_dpd_delay_time_us;

  /* Determine number of address bytes based on density of device */
  dev_data->num_addr_bytes = (dev_data->device_size_MB > 16)?(0x4):(0x3);
  
  unit_size = dev_data->page_size_bytes;
  /* Find the nth bit set to determine number of page size shifts */
  while (!(unit_size & 0x1))
  {
    unit_size >>= 1;
    ++client_ctxt->client_data.page_size_shift;
  }

  unit_size = dev_data->pages_per_block;
  /* Find the nth bit set to determine number of block size shifts */
  while (!(unit_size & 0x1))
  {
    unit_size >>= 1;
    ++client_ctxt->client_data.block_size_shift;
  }

  /* Initialize client info */
  status = nor_spi_init_client_info(client_ctxt);

  return status;
}

/*
 * Configure the pdata fields
 */
static int nor_spi_configure_pdata(flash_client_ctxt *client_ctxt,
  struct flash_nor_params *devices)
{
  int result = FLASH_DEVICE_DONE;
  uint32 size_bytes = 0, *temp_addr = NULL;
  struct nor_spi_dev_pdata *nor_spi_pdata;
  struct nor_flash_meminfo *flash_mem_data;

  nor_spi_pdata = GET_NOR_PDATA(client_ctxt);
  nor_spi_pdata->config_data = devices;
  nor_spi_pdata->flash_mem_info = &nor_entry_init_data.meminfo;

  flash_mem_data = GET_NOR_MEM_INFO(client_ctxt);

  /* Set up the driver buffers */
  temp_addr = (uint32 *)(FLASH_ALIGN_32(flash_mem_data->drv_mem_info.VirtualAddr));
  nor_spi_pdata->cmd_addr_buffer = (uint8 *)temp_addr;

  /* Allocate enough memory for device cmd/addresses */
  size_bytes += NOR_SPI_CMD_ADDR_BUF_LEN;
  nor_spi_pdata->status_buffer = 
    (uint8 *)(flash_mem_data->drv_mem_info.VirtualAddr + size_bytes);

  /* Set 3 or 4 Address mode */
  nor_spi_pdata->num_addr_bytes = (devices->device_size_MB > 16)?(0x4):(0x3);
  
  return result;
}

/*
 * Configure the client and device context info.
 */
int nor_spi_configure(flash_client_ctxt *client_ctxt, struct flash_nor_params
  *devices)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_spi_dev_pdata *nor_spi_pdata;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  /* Allocate private data shared across clients */
  if (dev_ctxt->dev_info.common.dev_specific_data == NULL)
  {
    if (DAL_SUCCESS != DALSYS_Malloc(sizeof(struct nor_spi_dev_pdata),
      (void ** )&dev_ctxt->dev_info.common.dev_specific_data))
    {
      FLASHLOG(3, ("\nMalloc failed\n"));
      result = FLASH_DEVICE_FAIL;
      goto exitfunction;
    }
  }

  /* Set default device vtbl so that nor_spi_entry can redirect
     to the open_partition API */
  nor_spi_base_vtbl.power_event    = nor_spi_power_event;
  nor_spi_base_vtbl.close          = nor_spi_close;
  nor_spi_base_vtbl.deinit         = nor_spi_deinit;
  nor_spi_base_vtbl.device_open    = nor_spi_open;
  dev_ctxt->dev_base_vtbl = &nor_spi_base_vtbl;

  nor_spi_pdata = GET_NOR_PDATA(client_ctxt);
  nor_spi_pdata->config_data = devices;
  nor_spi_pdata->flash_mem_info = &nor_entry_init_data.meminfo;

  /* Initialize one time device context Information */
  result = nor_spi_configure_device_ctxt(client_ctxt, devices);

  if (FLASH_DEVICE_DONE != nor_spi_configure_pdata(client_ctxt, devices))
  {
    result = FLASH_DEVICE_FAIL;
  }

exitfunction:
  return result;
}

/*
 * Check the passed device match with the given ID.
 */
int nor_spi_device_lookup(struct flash_nor_params *devices, uint8 *ids)
{
  uint32 id_count = devices->num_ids;
  int result = FLASH_DEVICE_DONE;

  /* Scan for the codes.  Check indicated number of ids for
     each component in the list.  Stop when we find a match.
     our indicator of no match. */
  while (id_count)
  {
    if (devices->dev_ids[id_count-1] != ids[id_count-1])
    {
      result = FLASH_DEVICE_FAIL;
      break;
    }

    id_count--;
  }

  return result;
}

/*
 * Probe the NOR device and do all one time initialization for NOR driver
 */
int nor_spi_probe(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_NOT_SUPPORTED;
  uint8 ids[4];
  uint32 cfg_count = 0, dev_count = 0;
  struct nor_spi_dev_pdata *nor_spi_pdata;
  struct flash_nor_cfg_data *nor_cfgs;
  struct nor_spi_probe_cfgs probe_cfg;
  struct flash_nor_params *devices = NULL;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;
  struct nor_dev_data *dev_data = &dev_ctxt->dev_info.nor;
  
  nor_spi_pdata = GET_NOR_PDATA(client_ctxt);
  probe_cfg.dev_type = FLASH_UNKNOWN;

  /* Allocate private data shared across clients */
  if (nor_spi_pdata == NULL)
  {
    if (DAL_SUCCESS != DALSYS_Malloc(sizeof(struct nor_spi_dev_pdata),
      (void ** )&dev_ctxt->dev_info.common.dev_specific_data))
    {
      FLASHLOG(3, ("\nMalloc failed\n"));
      result = FLASH_DEVICE_FAIL;
      goto exitfunction;
    }
    nor_spi_pdata = GET_NOR_PDATA(client_ctxt);
  }

  /* Initialize SPI Interface */
  if (FLASH_DEVICE_DONE != nor_spi_init(client_ctxt))
  {
    FLASHLOG(3, ("\n Flash Probe: SPI Driver initialization failed.\n"));
    result = FLASH_DEVICE_FAIL;
    goto exitfunction;
  }

  /* Get the spi nor configurations */
  flash_nor_spi_get_configs(&nor_cfgs);

  /* Search the BSP to see if it has any NOR device configured to be
     probed */
  for (cfg_count = 0; (FLASH_UNKNOWN != nor_cfgs[cfg_count].dev_type);
    cfg_count++)
  {
    if (FLASH_NOR_SPI == nor_cfgs[cfg_count].dev_type)
    {
      devices = nor_cfgs[cfg_count].dev_params;

      /* Basic configuration for flash probe */
      probe_cfg.chip_sel = nor_cfgs[cfg_count].cs_num;
      probe_cfg.dev_type = nor_cfgs[cfg_count].dev_type;
      break;
    }
  }

  if (FLASH_UNKNOWN == probe_cfg.dev_type) 
  {
    /* Couldn't find a valid configuration */
    goto exitfunction;
  }

  /* Initialize Memory */
  if (FLASH_DEVICE_DONE != nor_spi_mem_init(client_ctxt))
  {
    FLASHLOG(3, ("\nNOR SPI Memory initialization failed!\n"));
    result = FLASH_DEVICE_NO_MEM;
    goto exitfunction;
  }

  /* Default setting to allow flash operations to proceed. */
  nor_spi_pdata->erase_in_progress = FALSE;

  /* Set the Release from Deep Power Down Delay to the Max value */
  dev_data->rel_dpd_delay_time_us = NOR_SPI_REL_DPD_DELAY_MAX;
  
  for (dev_count = 0; dev_count < nor_cfgs[cfg_count].num_devices;
    dev_count++)
  {
    /* Basic configuration for SPI bus client */
    nor_spi_dev_set_params();

	/* Release from Deep Power Down mode to ensure that the device
     * is in known(stand-by) power mode state.
     */
    if (FLASH_DEVICE_DONE != nor_spi_release_deep_power_down(client_ctxt))
    {	
      goto exitfunction;
    }
	
    if (FLASH_DEVICE_DONE == nor_spi_read_id(client_ctxt, &ids[0]))
    {
      if (FLASH_DEVICE_DONE == nor_spi_device_lookup(devices, &ids[0]))
      {
        result = FLASH_DEVICE_DONE;
        break;
      }
    }

    devices++;
  }

  if (FLASH_DEVICE_DONE == result)
  {
    result = nor_spi_configure(client_ctxt, devices);
  }

  /* Open all blocks in device for erase/programming */
  if (FLASH_DEVICE_DONE == result)
  {
    result = nor_spi_unlock_all_blocks(client_ctxt);
  }

  /* Determine and set 3 or 4 byte address mode */
  if (FLASH_DEVICE_DONE == result)
  {
    result = nor_spi_set_num_address_bytes(client_ctxt);
  }

  /* Set the client context in the flash debug structure */
  SET_FLASH_DBG_CLIENT_CTX(client_ctxt);

  if (FLASH_DEVICE_DONE != result)
  {
    nor_spi_close(client_ctxt);
    nor_spi_mem_deinit(client_ctxt);
  }

  /* Update partition table to use 8 blocks(256KB) for SFS for 2MB SPI NOR */
//  nor_spi_update_usr_parti_tbl(client_ctxt);

exitfunction:
  return result;
}

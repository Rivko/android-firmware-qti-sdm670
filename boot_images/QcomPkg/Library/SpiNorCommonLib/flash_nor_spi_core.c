/*=============================================================================
 *
 * FILE:      flash_nor_spi_core.c
 *
 * DESCRIPTION: This file initializes the flash NOR core APIs
 *              using predefined HAL APIs.
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010, 2015 Qualcomm Technologies Incorporated.
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
 * 04/28/15     eo      Fix 64bit buffer addressing
 * 04/17/15     bb      Fix klocwork error
 * 03/15/10     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nor_spi.h"

/*------------------------------------------------------------------------------
  Variables global to this file.
 ------------------------------------------------------------------------------*/
#define dalsys_mem_desc_list  DALSysMemDescList

/* SPI NOR interface functions */
flash_vtbl nor_spi_base_vtbl;

/* global that stores failed status register value */
uint8 nor_spi_status_reg = 0;
/*------------------------------------------------------------------------------
  Variables local in this driver.
 ------------------------------------------------------------------------------*/
/* DAL Memory descriptor for page operations */
static DALSYS_MEM_DESC_LIST_OBJECT(buffer_desc, 1);
static DALBOOL buff_desc_initialized = FALSE;	

/*------------------------------------------------------------------------------
  Following functions are local to this driver.
 ------------------------------------------------------------------------------*/
static DALSysMemDescBuf *dalfw_mem_desc_buf_ptr(DALSysMemDescList * pMemDescList, uint32 idx)
{
  if(pMemDescList && (idx < pMemDescList->dwNumDescBufs))
  {
    return &(pMemDescList->BufInfo[idx]);
  }
  return NULL;
}
	
static void init_buffer_descriptor(flash_client_ctxt *client_ctxt,
  void *buff_addr, uint32 buff_size, dalsys_mem_desc_list *buf_desc,
  uint32 num_pages)
{
  DALSysMemDescBuf *desc_buff;

  if (FALSE == buff_desc_initialized)
  {
    buf_desc->hOwnerProc = 0xFFFFFFFF;
    buf_desc->PhysicalAddr = (DALSYSMemAddr)0xFFFFFFFF;
    buf_desc->VirtualAddr = (DALSYSMemAddr)0xFFFFFFFF;
    buf_desc->dwObjInfo = 0;
    buf_desc->hOwnerProc &= ~3;
    buf_desc->thisVirtualAddr = (DALSYSMemAddr)buf_desc;
    buf_desc->dwNumDescBufs = 1;
    buf_desc->dwCurBufIdx = 0;
    DALSYS_memset(buf_desc->BufInfo, -1, sizeof(DALSysMemDescBuf));
    buff_desc_initialized = TRUE;
  }
	
  desc_buff = dalfw_mem_desc_buf_ptr(buf_desc, 0);
  buf_desc->dwNumDescBufs = 1;

  if (desc_buff != NULL)
  {
    desc_buff->VirtualAddr = (DALSYSMemAddr)buff_addr;
    desc_buff->PhysicalAddr = (DALSYSMemAddr)desc_buff->VirtualAddr;
    desc_buff->size = buff_size;
    desc_buff->user = num_pages;
  }
}

/*
 * Reads any register of SPI NOR flash device. read_reg_cmd is used to 
 * specify which register to read.
 * Two buffers - 1 for command read and other to read status are required 
 */
static int nor_spi_read_register(flash_client_ctxt *client_ctxt,
  uint8 read_reg_cmd, uint8 *status)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_spi_xfer_buf_info spi_read_buf;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);
  
  spi_read_buf.spi_cmd_buf = nor_spi_pdata->cmd_addr_buffer;
  spi_read_buf.cmd_buf_len = NOR_SPI_CMD_LEN;
  spi_read_buf.spi_data_buf = nor_spi_pdata->status_buffer;
  spi_read_buf.data_buf_len = NOR_SPI_STATUS_REG_LEN;
  spi_read_buf.spi_cmd_buf[0] = read_reg_cmd;

  nor_spi_read(&spi_read_buf, &result);

  if(result == FLASH_DEVICE_DONE)
  {
    *status = spi_read_buf.spi_data_buf[0];
  }      

  return result;
}

/* 
 * Check the error status for each operation. If any of the desired error bits 
 * are set in the status registers, it indicates that the operation has failed
 * and the function returns failure.
 */
static int nor_spi_check_error_status(flash_client_ctxt *client_ctxt, 
  uint8 status)
{
  int result = FLASH_DEVICE_DONE;
  uint8 status_mask = 0;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);

  switch(nor_spi_pdata->config_data->family)
  {
    /* For Atmel, just use the status register value passed to verify the 
	 * status. */
    case FLASH_NOR_SPI_ATMEL:
      status_mask = NOR_SPI_STATUS_AT_ERROR_MASK;
      break;

   /* For Macronix, read the security status register to check for the error 
    * status of the operation.*/
    case FLASH_NOR_SPI_MACRONIX:
      status_mask = NOR_SPI_STATUS_MX_PFAIL_MASK | NOR_SPI_STATUS_MX_EFAIL_MASK;
      result = nor_spi_read_register(client_ctxt, NOR_SPI_READ_SECURITY_CMD,
        &status);
      break;

    /* For Micron, read the flag status register to check for the error 
	   status of the operation. */
    case FLASH_NOR_SPI_MICRON:
      status_mask = NOR_SPI_STATUS_MI_PFAIL_MASK | NOR_SPI_STATUS_MI_EFAIL_MASK;
      result = nor_spi_read_register(client_ctxt, NOR_SPI_READ_FLAG_CMD,
        &status);
      break;
     
	 /* For Winbond No status error check is required. */
	case FLASH_NOR_SPI_WINBOND:
	  return result;
	  
     /* For any other NOR family, return Failure */
    default:
      result = FLASH_DEVICE_FAIL;
  }
  
  /* Check device status */
  if ((result == FLASH_DEVICE_DONE) && (status & status_mask))
  {
    result = FLASH_DEVICE_FAIL;
    nor_spi_status_reg = status;
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
      "Flash: nsces - Device program/erase error bit set!");
  }
  
  return result;
}

/* 
 * Check the error status for each operation. This function polls the 
 * device until the current operation completes or timeout occurs and checks 
 * the status of the operation. If the operation doesnot complete within the
 * timeout period or the status registers shows that an operation has failed, 
 * then the function returns failure.
 */
static int nor_spi_check_status(flash_client_ctxt *client_ctxt, 
  enum nor_spi_dev_op nor_spi_op)
{
  uint8 status = 0, flag_status = 0;
  uint32 timeout = 0;
  int result = FLASH_DEVICE_FAIL;
  
  /* Obtain the timeout based on the operation */
  switch (nor_spi_op)
  {
    case NOR_SPI_ERASE_OP:
	   timeout = NOR_SPI_ERASE_TIMEOUT;
	   break;

    case NOR_SPI_WRITE_OP:
	   timeout = NOR_SPI_PROGRAM_TIMEOUT;
       break;

	default:
	   return result;
  }	   

  /* Query the device to see if the device completed the operation 
   * Loop until the operation completes or timeout is reached
   */
  result = nor_spi_read_register(client_ctxt, NOR_SPI_READ_STATUS_CMD, &status);
  
  while ((status & NOR_SPI_STATUS_BUSY_MASK) && (--timeout) && 
    (result == FLASH_DEVICE_DONE))
  {
    flash_watchdog_type *watchdog = NULL;
	
    watchdog = (flash_watchdog_type *)flash_get_property(FLASH_KICK_WATCHDOG_ID);
    if (FLASH_PROPERTY_NOT_DEFINED == (uint32)watchdog)
    {
      FLASHLOG(3,(" Flash: Failed to get watchdog function.\n"));
      return FLASH_DEVICE_FAIL;
    }

    result = nor_spi_read_register(client_ctxt, NOR_SPI_READ_STATUS_CMD, 
      &status);

    result = nor_spi_read_register(client_ctxt, NOR_SPI_READ_FLAG_CMD, 
      &flag_status);

    /* Kick the watchdog timer */
    watchdog->kick_watchdog();  
  }
  
  /* Check if timeout occurred */
  if (!timeout) 
  {
    result = FLASH_DEVICE_FAIL;
    nor_spi_status_reg = status;  
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
      "Flash: nscs - Timeout expired, during operation %u!", nor_spi_op);
  }

  /* Check of error status of the operation that has just completed */
  if (result == FLASH_DEVICE_DONE)
  {
    result = nor_spi_check_error_status(client_ctxt, status);
  }
   
  return result;
}

/* 
 * Write enable register for SPI NOR flash device.
 */
static int nor_spi_write_enable(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;
  uint8 status = 0;
  struct nor_spi_xfer_buf_info spi_write_buf;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);

  spi_write_buf.spi_cmd_buf = nor_spi_pdata->cmd_addr_buffer;
  spi_write_buf.cmd_buf_len = NOR_SPI_CMD_LEN;
  spi_write_buf.spi_data_buf = NULL;
  spi_write_buf.data_buf_len = 0;
  spi_write_buf.spi_cmd_buf[0] = NOR_SPI_WRITE_ENABLE_CMD;

  nor_spi_write(&spi_write_buf, &result);

  if (result == FLASH_DEVICE_DONE)
  {
    /* Read the Status Register and check for Write Enable Latch Bit Enabled */
	result = nor_spi_read_register(client_ctxt, NOR_SPI_READ_STATUS_CMD,
      &status);
    if ((result == FLASH_DEVICE_DONE) && 
        !(status & NOR_SPI_STATUS_WR_EN_MASK))
    {
      result = FLASH_DEVICE_FAIL;
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
        "Flash: nswe - Enabling Write Enable Latch Failed!");
    }    
  }

  return result;
}

/* Check for block erase operation completion. */
static int nor_spi_is_block_erase_done(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);

  if (nor_spi_pdata->erase_in_progress == TRUE)
  {
    /* Poll for Operation Completion and check the error status */
    result = nor_spi_check_status(client_ctxt, NOR_SPI_ERASE_OP);

    /* At this point, device is in ready state. */
    nor_spi_pdata->erase_in_progress = FALSE; 
  }

  return result;
}

/* 
 * Initiate erase block in SPI NOR flash device.
 */
static int nor_spi_start_block_erase(flash_client_ctxt *client_ctxt, 
  uint32 blk_num)
{
  int result = FLASH_DEVICE_DONE;
  uint32 page_num, page_addr;
  uint32 pages_per_block, page_size_bytes;
  struct nor_spi_xfer_buf_info spi_write_buf;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);

  spi_write_buf.spi_cmd_buf = nor_spi_pdata->cmd_addr_buffer;
  spi_write_buf.cmd_buf_len = NOR_SPI_CMD_LEN + NOR_SPI_ADDR_LEN;
  spi_write_buf.spi_data_buf = NULL;
  spi_write_buf.data_buf_len = 0;

  page_num = blk_num << client_ctxt->client_data.block_size_shift;
  page_addr = page_num << client_ctxt->client_data.page_size_shift;
  pages_per_block = client_ctxt->client_data.pages_per_block;
  page_size_bytes = client_ctxt->client_data.page_size_bytes;

  switch (pages_per_block * page_size_bytes)
  {
    case BLK_SIZE_4K:
      spi_write_buf.spi_cmd_buf[0] = NOR_SPI_ERASE_4KB_BLK_CMD;
      break;
    case BLK_SIZE_32K:
      spi_write_buf.spi_cmd_buf[0] = NOR_SPI_ERASE_32KB_BLK_CMD;
      break;
    case BLK_SIZE_64K:
      spi_write_buf.spi_cmd_buf[0] = NOR_SPI_ERASE_64KB_BLK_CMD;
      break;
  }

  if (nor_spi_pdata->num_addr_bytes == 0x3)
  {
    spi_write_buf.spi_cmd_buf[1] = (uint8)((page_addr >> 16) & 0xFF);
    spi_write_buf.spi_cmd_buf[2] = (uint8)((page_addr >> 8) & 0xFF);
    spi_write_buf.spi_cmd_buf[3] = (uint8)((page_addr) & 0xFF);
  }
  else if (nor_spi_pdata->num_addr_bytes == 0x4)
  {
	spi_write_buf.cmd_buf_len = NOR_SPI_CMD_LEN + NOR_SPI_ADDR_LEN + 1;

    spi_write_buf.spi_cmd_buf[1] = (uint8)((page_addr >> 24) & 0xFF);
    spi_write_buf.spi_cmd_buf[2] = (uint8)((page_addr >> 16) & 0xFF);
    spi_write_buf.spi_cmd_buf[3] = (uint8)((page_addr >> 8) & 0xFF);
    spi_write_buf.spi_cmd_buf[4] = (uint8)((page_addr) & 0xFF);
  }
  
  nor_spi_write(&spi_write_buf, &result);

  return result;
}    

/* 
 * Initiate programming page data in SPI NOR flash device.
 */
static int nor_spi_start_page_write(flash_client_ctxt *client_ctxt, 
  uint32 page_num, uint32 *page_buf, uint32 page_size)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_spi_xfer_buf_info spi_write_buf;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);

  spi_write_buf.spi_cmd_buf = nor_spi_pdata->cmd_addr_buffer;
  spi_write_buf.cmd_buf_len = 
    NOR_SPI_CMD_LEN + NOR_SPI_ADDR_LEN;
  spi_write_buf.spi_data_buf = (uint8 *)((UINTN *)page_buf);
  spi_write_buf.data_buf_len = page_size;

  spi_write_buf.spi_cmd_buf[0] = NOR_SPI_PROG_PAGE_CMD;
  if (nor_spi_pdata->num_addr_bytes == 0x3)
  {
    spi_write_buf.spi_cmd_buf[1] = (uint8)((page_num >> 16) & 0xFF);
    spi_write_buf.spi_cmd_buf[2] = (uint8)((page_num >> 8) & 0xFF);
    spi_write_buf.spi_cmd_buf[3] = (uint8)((page_num) & 0xFF);
  }
  else if (nor_spi_pdata->num_addr_bytes == 0x4)
  {
	spi_write_buf.cmd_buf_len = NOR_SPI_CMD_LEN + NOR_SPI_ADDR_LEN + 1;

    spi_write_buf.spi_cmd_buf[1] = (uint8)((page_num >> 24) & 0xFF);
    spi_write_buf.spi_cmd_buf[2] = (uint8)((page_num >> 16) & 0xFF);
    spi_write_buf.spi_cmd_buf[3] = (uint8)((page_num >> 8) & 0xFF);
    spi_write_buf.spi_cmd_buf[4] = (uint8)((page_num) & 0xFF);
  }

   nor_spi_write(&spi_write_buf, &result);

  return result;
}

/* 
 * Erase block operation for SPI NOR flash device.
 */
static int nor_spi_erase_block(flash_client_ctxt *client_ctxt,
  uint32 blk_num, uint8 poll_device_busy)
{
  int result = FLASH_DEVICE_DONE;

  result = nor_spi_write_enable(client_ctxt);

  if (result == FLASH_DEVICE_DONE)
  {   
    result = nor_spi_start_block_erase(client_ctxt, blk_num);
  }

  if (poll_device_busy && (result == FLASH_DEVICE_DONE))
  {
    /* Poll for Operation Completion and check the error status */
    result = nor_spi_check_status(client_ctxt, NOR_SPI_ERASE_OP);
  }

  return result;
}    

/* 
 * Program page data operation for SPI NOR flash device.
 */
static int nor_spi_write_page(flash_client_ctxt *client_ctxt, uint32 page_num, 
  DALSysMemDescBuf *buf_desc)
{
  int result = FLASH_DEVICE_DONE;
  int32 bytes_to_send = 0;
  uint32 page_addr = 0, *buf_addr = 0;
  uint32 num_shifts = client_ctxt->client_data.page_size_shift;
  uint32 page_size = client_ctxt->client_data.page_size_bytes;
  flash_watchdog_type *watchdog = NULL;
  
  watchdog = (flash_watchdog_type *)flash_get_property(FLASH_KICK_WATCHDOG_ID);

  if (FLASH_PROPERTY_NOT_DEFINED == (uint32)watchdog)
  {
    FLASHLOG(3,(" Flash: Failed to get watchdog function.\n"));
    return FLASH_DEVICE_FAIL;
  }
  
  bytes_to_send = (int32)buf_desc->size;
  page_addr = page_num << num_shifts;
  buf_addr = (uint32 *)&buf_desc->VirtualAddr;

  while ((bytes_to_send > 0) && (result == FLASH_DEVICE_DONE))
  {
    result = nor_spi_write_enable(client_ctxt);

    if (result == FLASH_DEVICE_DONE)
    {
      result = nor_spi_start_page_write(client_ctxt, page_addr, (uint32 *)*((UINTN *)buf_addr), 
        page_size);
    }

    if (result == FLASH_DEVICE_DONE)
    {   
      /* Poll for Operation Completion and check the error status */
      result = nor_spi_check_status(client_ctxt, NOR_SPI_WRITE_OP);
    }

    bytes_to_send -= page_size;
    *buf_addr += page_size;
    page_addr += page_size;
  }

  return result;
}    

/* 
 * Program partial page data operation for SPI NOR flash device.
 */
static int nor_spi_write_partial_page(flash_client_ctxt *client_ctxt, 
  uint32 page, uint32 page_offset, uint32 byte_count, 
  DALSysMemDescBuf *buf_desc)
{
  int result = FLASH_DEVICE_DONE;
  uint32 phys_page = 0, bytes_to_send = 0;
  uint32 offset = 0, page_addr = 0, buf_addr = 0;
  uint32 num_shifts = client_ctxt->client_data.page_size_shift;
  uint32 max_page_size = client_ctxt->client_data.page_size_bytes;
  flash_watchdog_type *watchdog = NULL;
  
  watchdog = (flash_watchdog_type *)flash_get_property(FLASH_KICK_WATCHDOG_ID);

  if (FLASH_PROPERTY_NOT_DEFINED == (uint32)watchdog)
  {
    FLASHLOG(3,(" Flash: Failed to get watchdog function.\n"));
    return FLASH_DEVICE_FAIL;
  }

  offset = page_offset;
  phys_page = NOR_LP_TO_PP(client_ctxt, page);
  bytes_to_send = byte_count;
  
  /* Check number of bytes to program is within a page's length */
  if ((bytes_to_send + offset) > max_page_size)
  {
    return FLASH_DEVICE_FAIL;
  }
  page_addr = (phys_page << num_shifts) + offset;
  buf_addr = buf_desc->VirtualAddr;

  result = nor_spi_write_enable(client_ctxt);

  if (result == FLASH_DEVICE_DONE)
  {      
    result = nor_spi_start_page_write(client_ctxt, page_addr, (uint32 *)&buf_addr, 
      bytes_to_send);
  }

  if (result == FLASH_DEVICE_DONE)
  {   
    /* Poll for Operation Completion and check the error status */
    result = nor_spi_check_status(client_ctxt, NOR_SPI_WRITE_OP);
  }

  return result;
}    

/* 
 * Initiate read page data for SPI NOR flash device.
 */
static int nor_spi_read_page(flash_client_ctxt *client_ctxt, uint32 page_num, 
  DALSysMemDescBuf *buf_desc)
{
  int result = FLASH_DEVICE_DONE;
  int32 bytes_to_receive = 0;
  uint32 page_addr = 0, *buf_addr = NULL, buf_page_cnt = 0;
  uint32 page_size = client_ctxt->client_data.page_size_bytes;
  uint32 num_shifts = client_ctxt->client_data.page_size_shift;
  struct nor_spi_xfer_buf_info spi_read_buf;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);

  bytes_to_receive = (int32)buf_desc->size;
  page_addr = page_num << num_shifts;
  buf_addr = (uint32 *)&buf_desc->VirtualAddr;

  /* 'user' parameter denotes total pages this buffer descriptor can hold */
  buf_page_cnt = 
    (buf_desc->user < nor_entry_init_data.multi_rw_support_count) ?  
    buf_desc->user : nor_entry_init_data.multi_rw_support_count;

  page_size *= buf_page_cnt;

  spi_read_buf.spi_cmd_buf = nor_spi_pdata->cmd_addr_buffer;
  spi_read_buf.cmd_buf_len = NOR_SPI_CMD_LEN + NOR_SPI_ADDR_LEN;

  while ((bytes_to_receive > 0) && (result == FLASH_DEVICE_DONE))
  {
    spi_read_buf.spi_data_buf = (uint8 *)*((UINTN *)buf_addr);
    spi_read_buf.data_buf_len = page_size;

    spi_read_buf.spi_cmd_buf[0] = NOR_SPI_READ_PAGE_CMD;
	if (nor_spi_pdata->num_addr_bytes == 0x3)
	{
	  spi_read_buf.spi_cmd_buf[1] = (uint8)((page_addr >> 16) & 0xFF);
	  spi_read_buf.spi_cmd_buf[2] = (uint8)((page_addr >> 8) & 0xFF);
	  spi_read_buf.spi_cmd_buf[3] = (uint8)((page_addr) & 0xFF);
	}
	else if (nor_spi_pdata->num_addr_bytes == 0x4)
	{
	  spi_read_buf.cmd_buf_len = NOR_SPI_CMD_LEN + NOR_SPI_ADDR_LEN + 1;
	
	  spi_read_buf.spi_cmd_buf[1] = (uint8)((page_addr >> 24) & 0xFF);
	  spi_read_buf.spi_cmd_buf[2] = (uint8)((page_addr >> 16) & 0xFF);
	  spi_read_buf.spi_cmd_buf[3] = (uint8)((page_addr >> 8) & 0xFF);
	  spi_read_buf.spi_cmd_buf[4] = (uint8)((page_addr) & 0xFF);
	}
     
    nor_spi_read(&spi_read_buf, &result);

    bytes_to_receive -= page_size;
    *buf_addr +=page_size;
    page_addr += page_size;
     
    if (bytes_to_receive < page_size)
    {
      page_size = bytes_to_receive;
    }
  }

  return result;
}

/*
 * Retrieve SPI NOR device specific information.
 */
static int nor_spi_get_info (flash_client_ctxt *client_ctxt,
  enum flash_info_type info_type, void *get_data)
{
  int result = FLASH_DEVICE_DONE;

  if (client_ctxt->refs)
  {
    if (FLASH_DEVICE_INFO == info_type)
    {
      struct flash_info *info = (struct flash_info *)get_data;

      /* Copy the device name */
      flash_strcpy((uint8 *)info->device_name,
       (uint8 *)client_ctxt->flash_dev_ctxt->dev_info.common.device_name,32);

      /* Get Device type and width  */
      info->type = client_ctxt->flash_dev_ctxt->dev_info.common.type;
      info->width = client_ctxt->flash_dev_ctxt->dev_info.common.width;
      info->block_count = 
       client_ctxt->flash_dev_ctxt->dev_info.nor.block_count;

      info->write_style =
       client_ctxt->flash_dev_ctxt->dev_info.common.write_style;

      /* Get Device manufacturer/make ID, device ID, version  */
      info->maker_id = client_ctxt->flash_dev_ctxt->dev_info.nor.dev_ids[0];
      info->device_id = client_ctxt->flash_dev_ctxt->dev_info.nor.dev_ids[1];
      info->version_id =
       client_ctxt->flash_dev_ctxt->dev_info.nor.dev_ids[2];

      /* Get Partition specific information  */
      info->pages_per_block =
       client_ctxt->client_data.pages_per_block;
      info->page_size_bytes =
       client_ctxt->client_data.page_size_bytes;
    }
    else if (FLASH_NOR_BLOCK_ERASE_CFG_INFO == info_type)
    {
      uint32 cfg_count = 0;
      struct nor_spi_dev_pdata *nor_spi_pdata;
      struct flash_nor_block_erase_cfg_info *erase_data;
      struct flash_nor_block_erase_cfg_info *info = 
        (struct flash_nor_block_erase_cfg_info *)get_data;

      nor_spi_pdata = GET_NOR_PDATA(client_ctxt);
      erase_data = &nor_spi_pdata->config_data->erase_cfg_info;
      info->erase_cfg_count = erase_data->erase_cfg_count;

      while (cfg_count < info->erase_cfg_count)
      {
        info->block_cfg[cfg_count].block_count = 
          erase_data->block_cfg[cfg_count].block_count;
        info->block_cfg[cfg_count].pages_per_block = 
          erase_data->block_cfg[cfg_count].pages_per_block;
          
        cfg_count++;  
      }
    }
    else if(FLASH_NOR_ERASE_REGION_INFO == info_type)
    {
      struct flash_nor_erase_region_info *info = 
        (struct flash_nor_erase_region_info *)get_data;

      info->erase_region_count = 1;
      info->erase_region[0].block_count = 
        client_ctxt->flash_dev_ctxt->dev_info.nor.block_count;
      info->erase_region[0].block_size_in_bytes = 
        client_ctxt->client_data.pages_per_block *
        client_ctxt->client_data.page_size_bytes;
    }        
  }
  else
  {
    get_data = NULL;
    result = FLASH_DEVICE_FAIL;
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(FLASH_DEVICE_DONE);

  return result;
}

/* 
 * Set 4 byte address mode in SPI NOR flash device.
 */
int nor_spi_set_num_address_bytes(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_spi_xfer_buf_info spi_write_buf;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);
	
  /* Change device to 4 byte address mode */
  if (nor_spi_pdata->num_addr_bytes == 0x4)
  {
    result = nor_spi_write_enable(client_ctxt);
	
    if (result == FLASH_DEVICE_DONE)
    {
      spi_write_buf.spi_cmd_buf = nor_spi_pdata->cmd_addr_buffer;
      spi_write_buf.cmd_buf_len = NOR_SPI_CMD_LEN;
      spi_write_buf.spi_data_buf = NULL;
      spi_write_buf.data_buf_len = 0;

      spi_write_buf.spi_cmd_buf[0] = NOR_SPI_ENTER_4B_ADDR_CMD;
	
      nor_spi_write(&spi_write_buf, &result);
    }
  }
  
  return result;
}

/* 
 *  Unlock all blocks in SPI NOR flash device.
 */
int nor_spi_unlock_all_blocks(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;
  uint8 status, mask = 0xFF;
  uint32 wrsr_timeout = NOR_SPI_PROGRAM_TIMEOUT;
  struct nor_spi_xfer_buf_info spi_write_buf;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);

  result = nor_spi_write_enable(client_ctxt);

  if (result == FLASH_DEVICE_DONE)
  {
    spi_write_buf.spi_cmd_buf = nor_spi_pdata->cmd_addr_buffer;
    spi_write_buf.cmd_buf_len = NOR_SPI_CMD_LEN + NOR_SPI_STATUS_REG_LEN;
    spi_write_buf.spi_data_buf = NULL;
    spi_write_buf.data_buf_len = 0;

    spi_write_buf.spi_cmd_buf[0] = NOR_SPI_WRITE_STATUS_CMD;
    spi_write_buf.spi_cmd_buf[1] = NOR_SPI_GBL_UNPROTECT_BLK;

    nor_spi_write(&spi_write_buf, &result);

    if (result == FLASH_DEVICE_DONE)
    {   
      while (wrsr_timeout--)
      {
        /* Read the Status Register and check for Operation Completion */
		result = nor_spi_read_register(client_ctxt, NOR_SPI_READ_STATUS_CMD,
		  &status);

        if ((result == FLASH_DEVICE_FAIL) ||
          !(status & NOR_SPI_STATUS_BUSY_MASK))
        {
          break;
        } 
      }

      if (!wrsr_timeout)
      {
        result = FLASH_DEVICE_FAIL;
        nor_spi_status_reg = status;
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
          "Flash: nsuab - Timeout expired, wrsr update failed. !");
      }      
    }
 
    /* Check device status */ 
    if (result == FLASH_DEVICE_DONE)
    {
      /* Read the Status Register and verify if all sectors/blocks are 
       * unprotected */
      result = nor_spi_read_register(client_ctxt, NOR_SPI_READ_STATUS_CMD,
        &status);
        
      switch (nor_spi_pdata->config_data->family)
      {
        case FLASH_NOR_SPI_ATMEL:
          mask = NOR_SPI_AT_UNPROTECT_MASK;
          break;
        case FLASH_NOR_SPI_MACRONIX:
          mask = NOR_SPI_MX_UNPROTECT_MASK;
          break;
        case FLASH_NOR_SPI_WINBOND:
          mask = NOR_SPI_WB_UNPROTECT_MASK;
          break;
        case FLASH_NOR_SPI_MICRON:
          mask = NOR_SPI_MI_UNPROTECT_MASK;
          break;
        default:
          mask = 0xFF;
          result = FLASH_DEVICE_FAIL;
          DALSYS_LogEvent(0, DALSYS_LOGEVENT_ERROR,
            "Flash: nsuab - Unknown NOR Family wrsr update failed. !");
      }

      if ((result == FLASH_DEVICE_DONE) && (status & mask))
      {
        result = FLASH_DEVICE_FAIL;
        nor_spi_status_reg = status;
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_ERROR,
          "Flash: nsuab - wrsr update failed!");
      }
	}
  }

  return result;
}    

/*
 * Detect the presence of a SPI NOR flash device.
 */
int nor_spi_read_id (flash_client_ctxt *client_ctxt, uint8 *ids)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_spi_xfer_buf_info spi_read_buf; 
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);

  spi_read_buf.spi_cmd_buf = nor_spi_pdata->cmd_addr_buffer;
  spi_read_buf.cmd_buf_len = NOR_SPI_CMD_LEN;
  spi_read_buf.spi_data_buf = ids;
  spi_read_buf.data_buf_len = NOR_SPI_ID_LEN;

  /* Read ID */
  spi_read_buf.spi_cmd_buf[0] = NOR_SPI_READDID_CMD;

  nor_spi_read(&spi_read_buf, &result);

  return result;
}/* nor_spi_read_id */

/* Enter into Deep Power-Down mode */
static int nor_spi_deep_power_down(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;
  
  struct nor_spi_xfer_buf_info spi_write_buf;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);
  struct nor_dev_data *dev_data;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  dev_data = &dev_ctxt->dev_info.nor;

  spi_write_buf.spi_cmd_buf = nor_spi_pdata->cmd_addr_buffer;
  spi_write_buf.cmd_buf_len = NOR_SPI_CMD_LEN;
  spi_write_buf.spi_data_buf = NULL;
  spi_write_buf.data_buf_len = 0;
  spi_write_buf.spi_cmd_buf[0] = NOR_SPI_DP_PWR_DWN_CMD;
  
  nor_spi_write(&spi_write_buf, &result);
  
  //DALSYS_BusyWait(dev_data->dpd_delay_time_us);

  return result;
}

/* Release from Deep Power-Down mode */
int nor_spi_release_deep_power_down(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;
  
  struct nor_spi_xfer_buf_info spi_write_buf;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);
  struct nor_dev_data *dev_data;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  dev_data = &dev_ctxt->dev_info.nor;
    
  spi_write_buf.spi_cmd_buf = nor_spi_pdata->cmd_addr_buffer;
  spi_write_buf.cmd_buf_len = NOR_SPI_CMD_LEN;
  spi_write_buf.spi_data_buf = NULL;
  spi_write_buf.data_buf_len = 0;
  spi_write_buf.spi_cmd_buf[0] = NOR_SPI_REL_DP_PWR_DWN_CMD;

  nor_spi_write(&spi_write_buf, &result);
  
  //DALSYS_BusyWait(dev_data->rel_dpd_delay_time_us);

  return result;
}

/* 
 * Set the power mode for flash 
 */
static int nor_spi_set_power_mode(flash_client_ctxt *client_ctxt, 
  enum flash_power_mode power_mode)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_spi_dev_pdata *nor_spi_pdata = GET_NOR_PDATA(client_ctxt);
  
  /* Atmel SPI NOR currently has issues with Deep Power Down support so 
   * do nothing for power mode setting for Atmel SPI NOR until issue is 
   * resolved 
   */
  if (nor_spi_pdata->config_data->family != FLASH_NOR_SPI_ATMEL)
  { 
    switch(power_mode)
    {
      case FLASH_POWER_LOW_PWR_MODE:
        result = nor_spi_deep_power_down(client_ctxt);
        break;
      case FLASH_POWER_NORMAL_MODE:
        result = nor_spi_release_deep_power_down(client_ctxt);
        break;
      default:
        result = FLASH_DEVICE_FAIL;
    }
  }
  return result;  
}

/*
 * Erase blocks in SPI NOR flash device.
 */
static int nor_spi_erase_blocks(flash_client_ctxt *client_ctxt,
  uint32 start_block, uint32 block_count)
{
  int result = FLASH_DEVICE_DONE;
  uint32 phys_block = 0, tmp_block_count = 0;
  
  /* Make sure block erase operation is not in progress */
  result = nor_spi_is_block_erase_done(client_ctxt);
  if (result != FLASH_DEVICE_DONE)
  {
    return result;
  }

  (void)nor_spi_erase_block;

  /* Get starting physical block */
  phys_block = NOR_LB_TO_PB(client_ctxt, start_block);

  /* Erase for the range of blocks specified */
  while (tmp_block_count < block_count)
  {
    /* Set the overall result as failure if one erase fails */
    if (FLASH_DEVICE_DONE != nor_spi_erase_block(client_ctxt, phys_block, TRUE))
    {
      result = FLASH_DEVICE_FAIL;
    }
    tmp_block_count++;
    phys_block++;
  }
  
  return result;
}

/*
 * Writes page_count pages in SPI NOR flash device
 */
static int nor_spi_write_pages(flash_client_ctxt *client_ctxt,
  void *buffer,
  uint32 start_page, uint32 page_count)
{
  int result = FLASH_DEVICE_DONE;
  int32  total_bytes_to_send;
  uint32 page_num;
  uint32 num_shifts = client_ctxt->client_data.page_size_shift; 
  struct nor_dev_data *dev_data;
  DALSysMemDescBuf *buf_desc;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  /* Make sure block erase operation is not in progress */
  result = nor_spi_is_block_erase_done(client_ctxt);
  if (result != FLASH_DEVICE_DONE)
  {
    return result;
  }

  dev_data = &dev_ctxt->dev_info.nor;
  page_num = NOR_LP_TO_PP(client_ctxt, start_page);

  total_bytes_to_send = (int32)(page_count * dev_data->page_size_bytes);

  init_buffer_descriptor(client_ctxt, buffer, total_bytes_to_send, 
	(DALSysMemDescList *)&buffer_desc, page_count);	  

  /* Get starting buffer descriptor from buffer desc list */
  buf_desc = dalfw_mem_desc_buf_ptr((DALSysMemDescList *)&buffer_desc, 0);
  if (buf_desc == NULL)
  {
    result = FLASH_DEVICE_FAIL;
  }
  
  while ((total_bytes_to_send > 0) && (result == FLASH_DEVICE_DONE))
  {
    result = nor_spi_write_page(client_ctxt, page_num, buf_desc);
    total_bytes_to_send -= buf_desc->size;
    page_num += (buf_desc->size >> num_shifts); 
  }

  return result;
}

/*
 * Writes page_count partial pages in SPI NOR flash device
 */
static int nor_spi_write_partial_pages(flash_client_ctxt *client_ctxt,
  void *buffer, 
  uint32 page, uint32 page_offset, uint32 byte_count)
{
  int result = FLASH_DEVICE_FAIL; 
  DALSysMemDescBuf *buf_desc;

  /* Make sure block erase operation is not in progress */
  result = nor_spi_is_block_erase_done(client_ctxt);
  if (result != FLASH_DEVICE_DONE)
  {
    return result;
  }

  init_buffer_descriptor(client_ctxt, buffer, byte_count, 
	(DALSysMemDescList *)&buffer_desc, 1);	  

  /* Get starting buffer descriptor from buffer desc list */
  buf_desc = dalfw_mem_desc_buf_ptr((DALSysMemDescList *)&buffer_desc, 0);
  if ((buffer_desc.dwNumDescBufs > 2) || (buf_desc == NULL))
  {
    result = FLASH_DEVICE_FAIL;
    return result;
  }

  result = nor_spi_write_partial_page(client_ctxt, page, page_offset,
    byte_count, buf_desc);

  return result;
}

/*
 * Reads page_count pages in SPI NOR flash device
 */
static int nor_spi_read_pages(flash_client_ctxt *client_ctxt,
  void *buffer,
  uint32 start_page, uint32 page_count)
{
  int result = FLASH_DEVICE_DONE;
  int32  total_bytes_to_receive;
  uint32 page_num;
  uint32 num_shifts = client_ctxt->client_data.page_size_shift; 
  struct nor_dev_data *dev_data;
  DALSysMemDescBuf *buf_desc;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  /* Make sure block erase operation is not in progress */
  result = nor_spi_is_block_erase_done(client_ctxt);
  if (result != FLASH_DEVICE_DONE)
  {
    return result;
  }

  dev_data = &dev_ctxt->dev_info.nor;
  page_num = NOR_LP_TO_PP(client_ctxt, start_page);

  total_bytes_to_receive = (int32)(page_count * dev_data->page_size_bytes);

  init_buffer_descriptor(client_ctxt, buffer, total_bytes_to_receive, 
	(DALSysMemDescList *)&buffer_desc, page_count);	  

  /* Get starting buffer descriptor from buffer desc list */
  buf_desc = dalfw_mem_desc_buf_ptr((DALSysMemDescList *)&buffer_desc, 0);
  if (buf_desc == NULL)
  {
    result = FLASH_DEVICE_FAIL;
  }
  
  while ((total_bytes_to_receive > 0) && (result == FLASH_DEVICE_DONE))
  {
    result = nor_spi_read_page(client_ctxt, page_num, buf_desc);
    
    total_bytes_to_receive -= buf_desc->size;
    page_num += (buf_desc->size >> num_shifts); 
  }
  
  return result;
}

/*
 * Erase blocks of specific size in SPI NOR flash device.
 */
static int nor_spi_erase_blocks_by_size(flash_client_ctxt *client_ctxt,
  enum block_size_opcode block_size_type, 
  uint32 start_block, uint32 block_count)
{
  int result = FLASH_DEVICE_DONE;

  /* Call erase blocks API */
  result = nor_spi_erase_blocks(client_ctxt, start_block, block_count);

  return result;
}

/*------------------------------------------------------------------------------
  VTABLE Initialization
 ------------------------------------------------------------------------------*/

flash_vtbl nor_spi_base_vtbl = {
  NULL,                      /* Power event */
  NULL,                      /* Close */
  NULL,                      /* De-init */
  NULL,                      /* Device Open */
  nor_spi_get_info,          /* get_info */
  nor_spi_erase_blocks,      /* Erase blocks */
  nor_spi_write_pages,       /* Write pages */
  nor_spi_write_partial_pages, /* write_partial_pages */
  nor_spi_read_pages,        /* read pages */
  nor_spi_erase_blocks_by_size, /* erase blocks by size */
  nor_spi_set_power_mode     /* Set power mode */
};

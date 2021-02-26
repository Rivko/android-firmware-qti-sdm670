/*=============================================================================
 *
 * FILE:      flash_dal.c
 *
 * DESCRIPTION: This file implements a DAL interface to the flash driver.
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2010, 2012-2014 Qualcomm Technologies Incorporated.
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
 *
 * when         who     what, where, why
 * ---------   ---     ----------------------------------------------
 * 03/18/14     whc     Add OEM custom EEPROM support
 * 12/17/13     whc     Add RF EEPROM I2C support
 * 09/05/12     bb      Remove flash dal layer for Public APIs
 * 03/19/12     sv      Add SPI NOR power management support
 * 03/19/12     eo      Support for erase blocks by size
 * 09/15/10     bb      Support for multiple flash device IDs
 * 03/15/10     bb      Support for isolating NOR build from NAND
 * 12/28/09     sc      Added 2x support in DAL
 * 02/23/09     bb      Read/Write API changes with iovec design changes
 * 11/15/08     mm      Add properties for NAND and NOR Probe table
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_dal.h"
#include "flash_properties.h"

/*----------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
-----------------------------------------------------------------------------*/
int flash_dal_driver_init(flash_drv_ctxt *drv_ctxt)
{
  FLASHLOG(4, ("Driver Init Called\n"));

  return DAL_SUCCESS;
}

int flash_dal_driver_deinit(flash_drv_ctxt *drv_ctxt)
{
  FLASHLOG(4, ("Driver DeInit Called\n"));

  return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
Following functions are declared in IDevice Interface.
----------------------------------------------------------------------------*/
int flash_dal_device_init(flash_client_ctxt *client_ctxt)
{
  int status = FLASH_DEVICE_FAIL;
  enum flash_property_id prob_table_id = FLASH_PROPERTY_ID_MAX;
  flash_probe_tbl *flash_prober = (flash_probe_tbl*)FLASH_PROPERTY_NOT_DEFINED;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  FLASHLOG(4, ("Flash DAL Device Init Called\n"));

  /*-------------------------------------------------------------------------
  Initialize Synchronization Object for this device.
  This is used to synchronize between client calls and interrupt context
  on our variables and HAL.
  --------------------------------------------------------------------------*/
  if(DAL_SUCCESS == DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
    &(dev_ctxt->handle_dev_sync), &(dev_ctxt->dev_sync_object)))
  {
    switch (dev_ctxt->dev_id)
    {
      case DALDEVICEID_FLASH_DEVICE_1:
        prob_table_id = FLASH_PROBE_TABLE_ID;
        break;
      case DALDEVICEID_FLASH_DEVICE_2:
	  case DALDEVICEID_FLASH_DEVICE_3:
        prob_table_id = FLASH_PROBE_TABLE_ID_2;
        break;
      default:
        break;
    }
  }

  if (prob_table_id != FLASH_PROPERTY_ID_MAX)
  {
    /* Get probe configuration from properties.h */
    flash_prober = (flash_probe_tbl *) flash_get_property(prob_table_id);
  }

  /* Initialize memory for NAND driver to use */
  if ((FLASH_PROPERTY_NOT_DEFINED != (uint32) flash_prober)  &&
      (flash_prober->device_prober))
  {
    /* Probe device */
    while ((NULL != flash_prober) && (flash_prober->device_prober))
    {
      status = flash_prober->device_prober(client_ctxt);

      if (FLASH_DEVICE_DONE == status)
      {
        break;
      }

      ++flash_prober;
    }
  }

  return ((status == FLASH_DEVICE_DONE) ? DAL_SUCCESS : DAL_ERROR);
}

int flash_dal_device_deinit(flash_client_ctxt *client_ctxt)
{
  int status = FLASH_DEVICE_FAIL;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  FLASHLOG(4, ("Device DeInit Called\n"));

  status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->deinit(client_ctxt);

  if (status == FLASH_DEVICE_DONE)
  {
    if(DAL_SUCCESS != DALSYS_DestroyObject(dev_ctxt->handle_dev_sync))
    {
      status = FLASH_DEVICE_FAIL;
    }
  }

  return ((status == FLASH_DEVICE_DONE) ? DAL_SUCCESS : DAL_ERROR);
}

/* ================================================================================== */

/*=============================================================================
 *
 * FILE:      flash_dal_info.c
 *
 * DESCRIPTION: Function declarations specific to Flash DAL Info
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright © 2008-2009, 2011, 2013-2014 Qualcomm Technologies Incorporated.
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
 * --------    ---     ----------------------------------------------
 * 03/18/14     whc     Add OEM custom EEPROM support
 * 12/17/13     whc     Add RF EEPROM I2C support
 * 05/20/11     bb      Added support for multiple flash devices
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"

#include "flash_dal_fwk.h"


static DALDEVICEID DALFlash_DeviceId[3] = {DALDEVICEID_FLASH_DEVICE_1, 
  DALDEVICEID_FLASH_DEVICE_2, DALDEVICEID_FLASH_DEVICE_3};

const DALREG_DriverInfo DALFlash_DriverInfo =
  {flash_fwk_attach, 3, DALFlash_DeviceId};



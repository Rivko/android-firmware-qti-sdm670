#ifndef __GPIO_BOOT_H__
#define __GPIO_BOOT_H__
/*===========================================================================

                         G P I O    F U N C T I O N A L   
                      D R I V E R   D E F I N I T I O N S

DESCRIPTION
  This module is the shared GPIO functional driver for MSM targets.  It is
  to be used with the TLMM driver BSP and HAL modules.

REFERENCES

===========================================================================
             Copyright (c) 2010 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/29/10   dcf     Created for SBL3 boot-loader.

===========================================================================*/

#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"

/*===========================================================================

  FUNCTION GPIO_INIT

  DESCRIPTION
        This function initializes the GPIOs for the TLMM configuration
        specified at boot time.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    It uses the stack

===========================================================================*/

static __inline boolean gpio_init(void)
{
  DalDeviceHandle* hTlmmDriver = NULL;
  DALResult eResult = DAL_ERROR;

  /* Initialize the TLMM handle. */
  eResult = DAL_DeviceAttach(DALDEVICEID_TLMM, &hTlmmDriver);
  if(DAL_SUCCESS == eResult)
  {
    /* 
     * Only need to initialize TLMM internal structures.  No need to
     * maintain a handle.
     */
    eResult = DAL_DeviceDetach(hTlmmDriver);
  }

  if(DAL_SUCCESS == eResult)
  {
    return(TRUE);
  }
  return(FALSE);
}

#endif /* __GPIO_BOOT_H__ */


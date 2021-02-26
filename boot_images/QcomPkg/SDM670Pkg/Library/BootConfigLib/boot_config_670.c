/** @file boot_config_Sdm660.c

  BootConfigLib is used to get boot configuration information

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   ---     -----------------------------------------------------------
08/04/17     aus     Initial version

=============================================================================*/

#include <Uefi.h>
#include "HALhwio.h"
#include "BootConfig.h"

#define BOOT_CONFIG_REG             0x00786070
#define BOOT_DEVICE_MASK(val)       (((val) & 0x3E) >> 1)

#define BOOT_DEFAULT_OPTION       0  /* UFS HS G1-> SDC -> USB */
#define BOOT_SDC_THEN_UFS_OPTION  1  /* 1, SDC -> UFS */
#define BOOT_SDC_OPTION           2  /* 2, SDC */
#define BOOT_USB_OPTION           3  /* 3, USB */
#define BOOT_EMMC_THEN_SDC_OPTION 4  /* 4, EMMC -> SDC */
#define BOOT_SDC_THEN_EMMC_OPTION 5  /* 5, SDC -> EMMC */

/******************************************************************************
* FUNCTION      boot_from_ufs
*
* DESCRIPTION   This function returns if device boots from UFS
*
* PARAMETERS    NONE
*
* RETURN VALUE  TRUE if device boots from UFS
*
******************************************************************************/
BOOLEAN boot_from_ufs (void)
{
   UINT32 val = 0;
   val = BOOT_DEVICE_MASK(in_dword(BOOT_CONFIG_REG));
   
   if (val == BOOT_DEFAULT_OPTION) 
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}  

/******************************************************************************
* FUNCTION      boot_from_emmc
*
* DESCRIPTION   This function returns if device boots from eMMC
*
* PARAMETERS    NONE
*
* RETURN VALUE  TRUE if device boots from eMMC
*
******************************************************************************/
BOOLEAN boot_from_emmc (VOID)
{
   UINT32 val = BOOT_DEVICE_MASK(in_dword(BOOT_CONFIG_REG));

   if (val == BOOT_EMMC_THEN_SDC_OPTION)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }       
}

/******************************************************************************
* FUNCTION      boot_from_spi_nor
*
* DESCRIPTION   This function returns if device boots from SPI NOR
*
* PARAMETERS    NONE
*
* RETURN VALUE  TRUE if device boots from SPI NOR
*
******************************************************************************/
BOOLEAN boot_from_spi_nor (VOID)
{
   return FALSE;
}
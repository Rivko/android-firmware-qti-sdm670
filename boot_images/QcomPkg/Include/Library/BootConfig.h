#ifndef BOOTCONFIG_H
#define BOOTCONFIG_H

/** @file BootConfig.h
   
  This file is header file for BootConfigLib

  Copyright (c) 2015, Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
    
**/

/*=============================================================================
                              EDIT HISTORY


when         who    what, where, why
----------   ---    --------------------------------------------------------------
2015-04-30   as     Add SPI NOR functionality.
2015-03-31   rm     Initial version

=============================================================================*/

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
BOOLEAN boot_from_ufs (void);

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
BOOLEAN boot_from_emmc (void);

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
BOOLEAN boot_from_spi_nor (void);

#endif /* #ifndef BOOTCONFIG_H */


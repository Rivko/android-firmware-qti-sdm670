/*=============================================================================
                        System Debug ChipInfo Header File 

GENERAL DESCRIPTION     
  This module defines anything specific to the chipset family. It is applicable to 
  all members of the chipset family.
  
 Copyright 2015 - 2016 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdebug/sysdbg_chipinfo.h#2 $

when       who          what, where, why
--------   ---          --------------------------------------------------
03/03/2015	AJCheriyan	Created for System debug

===========================================================================*/

#ifndef __SYSDBG_CHIPINFO_H
#define __SYSDBG_CHIPINFO_H

#include "comdef.h"

typedef enum
{
    FAMILY_A  = 0,
    FAMILY_B  = 0x1,
    FAMILY_BR = 0x2,
    FAMILY_HB = 0x3,
    FAMILY_AT = 0x4,
    FAMILY_SR = 0x5,
    FAMILY_MAX = 0x6,
}sysdbg_chipfamilytype;

typedef enum
{
    CHIP_MSM8996 = 0,
    CHIP_MSM8998 = 1,
    CHIP_MSMRSV1 = 2,
    CHIP_MSMRSV2 = 3,
    CHIP_MSMRSV3 = 4,
    CHIP_MSMRSV4 = 5,
    CHIP_MSMRSV5 = 6,
    CHIP_MSMRSV6 = 7,
    CHIP_QDF2432 = 8,
    CHIP_QDF4842 = 9,
    CHIP_MAX     = 10,
}sysdbg_chipidtype;



/* Chipinfo structure type */
typedef struct
{
	uint32 minor_rev:8;
	uint32 major_rev:8;
	uint32 device_num:8;
    uint32 family_num:8;
}__attribute__ ((packed)) sysdbg_chipinfotype;

/*===========================================================================
**  Function :  SysdbgGetChipFamily(void)
** ==========================================================================
*/
/*!
* 
* @brief
* 	Returns chipset family (i.e. variants of the same chip)
* 
* @param[in] 
* 	None
*  
* @par Dependencies
*   will call ChipInfo init
* 
* @retval
*   None
* 
* @par Side Effects
*   None
*/
uint32 SysdbgGetChipFamily(void);

/*===========================================================================
**  Function :  SysdbgGetChipId(void)
** ==========================================================================
*/
/*!
* 
* @brief
* 	Returns chipset id number (i.e. unique device number of the SoC)
* 
* @param[in] 
* 	None
*  
* @par Dependencies
*   will call ChipInfo init
* 
* @retval
*   None
* 
* @par Side Effects
*   None
*/
uint32 SysdbgGetChipId(void);

/*===========================================================================
**  Function :  SysdbgGetChipRev(void)
** ==========================================================================
*/
/*!
* 
* @brief
* 	Returns chipset revision number (i.e. major + minor rev of the sachip)
* 
* @param[in] 
* 	None
*  
* @par Dependencies
*   will call ChipInfo init
* 
* @retval
*   None
* 
* @par Side Effects
*   None
*/
uint32 SysdbgGetChipRev(void);


#endif /* __SYSDBG_CHIPINFO_H */
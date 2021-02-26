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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SystemDebugLib/sysdbg_chipinfo.c#2 $

when       who          what, where, why
--------   ---          --------------------------------------------------
03/03/2015	AJCheriyan	Created for System debug

===========================================================================*/

#include "sysdbg_target.h"
#include "sysdbg_chipinfo.h"

/* Main Chipset Information structure */
static sysdbg_chipinfotype SysdbgChipInfo;


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
uint32 SysdbgGetChipFamily(void)
{
    /* Get chip information */
    SysdbgGetChipInfo(&SysdbgChipInfo);

    switch (SysdbgChipInfo.family_num)
    {
        case 0x50:
        default:
            return FAMILY_SR;
            break;
    }

}

/*===========================================================================
**  Function :  SysdbgGetChipId(void)
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
uint32 SysdbgGetChipId(void)
{
    /* Get chip information */
    SysdbgGetChipInfo(&SysdbgChipInfo);

    switch (SysdbgChipInfo.device_num)
    {
        case 0:
            return CHIP_QDF2432;
            break;
        case 1:
            return CHIP_QDF4842;
            break;
        default:
            return CHIP_QDF2432;
            break;
    }

}

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
uint32 SysdbgGetChipRev(void)
{

    /* Get chip information */
    SysdbgGetChipInfo(&SysdbgChipInfo);

    return ((SysdbgChipInfo.major_rev << 8) + SysdbgChipInfo.minor_rev);
}





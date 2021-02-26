#ifndef __PM_CHG_H__
#define __PM_CHG_H__

/*! \file
*  \n
*  \brief  pm_chg.h PMIC-SCHG MODULE RELATED DECLARATION 
*  \details  This header file contains common functions and variable declarations 
*  to support Qualcomm PMIC SCHG (Switch-Mode Battery Charger) module.
*  The structure, pm_chg_info_type, defined in this file is shared with XBL Core.
*  \n &copy; Copyright 2016 QUALCOMM Technologies Inc, All Rights Reserved
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who    what, where, why
--------   ---    ---------------------------------------------------------- 
07/01/16   aab    Initial version. 
========================================================================== */
#include "com_dtypes.h"
#include "pm_err_flags.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef struct
{
  uint32  version;        /**< Version Info */
  int32   sbl_entry_mV;   /**< XBL loader entry Mv*/
  int32   sbl_entry_soc;  /**< XBL loader entry Soc; if in case the integrity bit is already set */
  int32   uefi_entry_mV;  /**< XBL Core Entry Mv*/
  int32   uefi_entry_soc; /**< XBL Core entry Soc */
  int32   uefi_exit_mV;   /**< XBL Core exit Mv*/
  int32   uefi_exit_soc;  /**< XBL Core Exit Soc */
}pm_chg_info_type;


#endif /* __PM_CHG_H__ */


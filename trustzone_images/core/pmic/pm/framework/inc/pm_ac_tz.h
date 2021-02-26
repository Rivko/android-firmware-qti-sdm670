#ifndef __PM_AC_TZ_H__
#define __PM_AC_TZ_H__

/*! \ file pm_ac_tz.h
*
 * \brief   <Brief Comment Block/>
 * \details <Detailed Comment Block/>
 * \n &copy; Copyright 2015-2017 QUALCOMM Technolgoies Incorporated, All Rights Reserved
 */

/*===================================================================
                    EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tz/2.0.2/pmic/pm/framework/inc/pm_ac_tz.h#1 $
$Author: pwbldsvc $

when       who     what, where, why
--------    ---     ------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
01/13/15    vtw     Created.
====================================================================*/

#include "SpmiCfg.h"
#include "AccessControlTz.h"
#include "pm_lib_err.h"

/* ============================================================================
**    Function Prototype.
** ==========================================================================*/
/**
 * @brief Fetch the address of the channel cfg table structure and size
 *
 * @details This function gets the address of the channel cfg table structure and size
 *
 *  @param chanCfg_ptr [in/out]  Pointer to channel cfg table .
 *  @param cfg_size [in/out]  Pointer to SPMI RGs array.
 *  @param rg_size [in]  Size of SPMI RGs array.
 *
 * @return pm_err_flag_type
 *  PM_ERR_FLAG__SUCCESS -- on success otherwise PMIC error code. \n
 * @dependencies
 *  None.
 * @sa None.
 *
 */
SpmiCfg_ChannelCfg* pm_get_spmi_channel_cfg( uint32* cfg_size);


#endif // PM_AC_TZ_H

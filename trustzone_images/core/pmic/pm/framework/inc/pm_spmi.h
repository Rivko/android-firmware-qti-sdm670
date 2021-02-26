#ifndef __PM_SPMI_H__
#define __PM_SPMI_H__

/*! \ file pm_spmi.h
*
 * \brief   <Brief Comment Block/>
 * \details <Detailed Comment Block/>
 * \n &copy; Copyright 2015 - 2017 QUALCOMM Technolgoies Incorporated, All Rights Reserved
 */

/*===================================================================
                    EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tz/2.0.2/pmic/pm/framework/inc/pm_spmi.h#1 $
$Author: pwbldsvc $

when       who     what, where, why
--------    ---     ------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
01/13/15    vtw     Created.
====================================================================*/

#include "SpmiCfg.h"

/*
 * pm_periph_owner
 *
 * A type to choose the interrupt and peripehral owner.
 */
typedef enum {
  PM_APPS_HLOS_OWNER = 0,
  PM_APPS_TZ_OWNER,
  PM_MSS_OWNER,
  PM_LPASS_OWNER,
  PM_RPM_OWNER,
  PM_WCONNECT_OWNER
} pm_periph_owner;


#endif // PM_SPMI_H
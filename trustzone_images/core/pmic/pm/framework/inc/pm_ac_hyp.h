#ifndef __PM_AC_DEF_H__
#define __PM_AC_DEF_H__

/*! \ file pm_ac_def.h
*
 * \brief   <Brief Comment Block/>
 * \details <Detailed Comment Block/>
 * \n &copy; Copyright 2015 - 2017 QUALCOMM Technolgoies Incorporated, All Rights Reserved
 */

/*===================================================================
                    EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tz/2.0.2/pmic/pm/framework/inc/pm_ac_hyp.h#2 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
01/13/15   vtw     Created.
====================================================================*/

#include "SpmiCfg.h"
#include "AccessControlHyp.h"

typedef struct {
  ACVirtualMachineId  vm_id;
  ACMapAttributesType ac_attr;
} pm_hyp_map_type;

typedef struct {
  ACVirtualMachineId  vm_id;
  ACUnmapAttributesType unmap_attr;
} pm_hyp_unmap_type;

typedef struct{
	uint32 sid;
	uint32 ppid;
}pm_periph_data_type;

/**
 * @brief Fetch the address of the peripheral data defined in table structure and size
 *
 * @details This function used to get the sid and ppid info of the peripheral table structure and size
 *
 *  @param arr_ptr  [in/out]  Pointer to peripheral address table .
 *  @param arr_size [in/out]  Pointer to size of array.
 *
 *  @dependencies
 *  None.
 * @sa None.
 *
 */

pm_periph_data_type* pm_get_periph_data( uint32* arr_size);

#endif // PM_AC_DEF_H

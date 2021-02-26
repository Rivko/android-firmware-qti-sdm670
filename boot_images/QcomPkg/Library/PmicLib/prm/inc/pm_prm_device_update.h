#ifndef __PM_PRM_DEVICE_UPDATE_H__
#define __PM_PRM_DEVICE_UPDATE_H__
/*===========================================================================
DESCRIPTION
This file contains prototype definitions prm LDO device layer

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/prm/inc/pm_prm_device_update.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/22/16   rl      Created
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include "pm_prm_device.h"

/*===========================================================================

                        FUNCTION DECLARATION

===========================================================================*/
   
npa_resource_state
pm_prm_get_vreg_rsrc_state(npa_client_handle client_handle, 
                          pm_prm_vreg_data_type *pam_data);

                          
npa_resource_state 
pm_prm_vreg_update_fcn(npa_resource *resource, npa_client_handle client);

void
pm_prm_vreg_populate_sets(npa_resource *resource, boolean is_nas_req);


boolean
pm_prm_get_vreg_status(npa_client_handle client_handle, 
                      pm_prm_vreg_data_type *pam_data);
                      
npa_resource_state
pm_prm_get_clk_rsrc_state(npa_client_handle client_handle, 
                          pm_prm_clk_data_type *pam_data);

                          
                          
npa_resource_state 
pm_prm_clk_update_fcn(npa_resource *resource, npa_client_handle client);

void
pm_prm_clk_populate_sets(npa_resource *resource, boolean is_nas_req);

boolean
pm_prm_get_clk_status(npa_client_handle client_handle, 
                      pm_prm_clk_data_type *pam_data);


                          
#endif // __PM_PRM_DEVICE_UPDATE_H__

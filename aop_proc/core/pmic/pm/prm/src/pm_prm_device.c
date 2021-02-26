/*! \file pm_prm_device.c 
*  
*  \brief This file contains driver functions for PMIC PRM Layer
*  \n  
*  \n &copy; Copyright 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/prm/src/pm_prm_device.c#1 $
  
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/17   akm     Created
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_prm_device.h"

unpa_resource_state
pm_prm_get_vreg_rsrc_state(vrm_settings rsrc_key, 
                           pm_prm_vreg_data_type *pam_data)
{
    unpa_resource_state state = 0;
    
    if(pam_data == NULL)
    {
        //PM_LOG_MSG_ERR_FATAL("Invalid Inputs");
        CORE_VERIFY(0);
    }
    
    switch(rsrc_key)
    {
        case VRM_MODE:
            state = pam_data->mode; 
            break;
        case VRM_VOLTAGE:
            state = pam_data->voltage;
            break;
        case VRM_ENABLE:
            state = pam_data->enable;
            break;
        default:
            CORE_VERIFY(0);
            //PM_LOG_MSG_ERR_FATAL("Invalid RSRC Key provided for %s", 
                                        //client_handle->resource_name);
    }
    return state; 
}

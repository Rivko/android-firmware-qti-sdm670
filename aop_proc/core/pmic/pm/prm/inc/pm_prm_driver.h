#ifndef __PM_PRM_DRIVER_H__
#define __PM_PRM_DRIVER_H__

/*===========================================================================
DESCRIPTION
  This file contains prm driver functions for pam and device layer. 
  
Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/prm/inc/pm_prm_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/17   akm     Created
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_prm_device.h"
#include "pm_rpmh.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//Since we only support required clients, dep offset is 1  
#define DEP_OFFSET  1

// Invalid initial resource state used to know the first request on the resource
#define PM_PRM_RSRC_INVALID_INIT_STATE 0xFFFFFFFE

typedef struct
{
    boolean initialized;
    rex_crit_sect_type lock;
    uint8 lock_count;
    pm_rpmh_set_type rpmh_data;
    vrm_rsrc_vote_data active_set;
}pm_prm_driver_info_type;

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

pm_err_flag_type 
pm_prm_driver_init(void);

unpa_resource_state
pm_prm_pam_driver_fcn (unpa_resource *resource,
                       unpa_client*  client,
                       unpa_resource_state mode_id);

unpa_resource_state
pm_prm_scalar_driver_fcn (unpa_resource *resource,
                          unpa_client* client,
                          unpa_resource_state value);

void
pm_prm_barrier_request(uint32 msg_id, 
                       boolean is_single);

void
pm_prm_notify_completion(uint32 msg_id, 
                         boolean is_single);

#endif /* __PM_PRM_H__ */

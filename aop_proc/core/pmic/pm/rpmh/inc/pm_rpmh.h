#ifndef __PM_RPMH_H__
#define __PM_RPMH_H__

/*===========================================================================
DESCRIPTION
  This file contains pmic rpmh layer declarations
  
Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/rpmh/inc/pm_rpmh.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/17   akm     Created
===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_prm_device.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/



/*===========================================================================

                        FUNCTION DECLARTIONS

===========================================================================*/
void 
pm_rpmh_init(void);

rpmh_client_handle 
pm_rpmh_get_handle(void);

void 
pm_rpmh_create_cmd_set(vrm_rsrc_vote_data *active_set, 
                       pm_rpmh_set_type *rsrc_rpmh_data,
                       vrm_rsrc_category rsrc_category,
                       uint32 rsrc_id,
                       vrm_settings rsrc_key,
                       boolean is_fnf);
                       
void  
pm_rpmh_post_set(vrm_rsrc_vote_data *active_set, 
                 uint32 *current_msg_id);
                
void 
pm_rpmh_barrier_all(uint32 msg_id);

void 
pm_rpmh_barrier_single(uint32 msg_id);

void
pm_rpmh_register_completion_callback(rpmh_completion_cb callback);
                
void 
pm_rpmh_notify_completion_all(uint32 msg_id);

void 
pm_rpmh_notify_completion_single(uint32 msg_id);


#endif //__PM_RPMH_H__

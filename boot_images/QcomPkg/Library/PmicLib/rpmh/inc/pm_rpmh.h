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

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/rpmh/inc/pm_rpmh.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/17   akm     Created
===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "npa.h"
#include "npa_resource.h"
#include "rpmh_client.h"
#include "rpmh_common.h"
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
pm_rpmh_create_cmd_set(rpmh_command_set_t *active_set, 
                       rpmh_command_set_t *sleep_set,
                       pm_rpmh_data_type  *rpmh_data,
                       boolean             is_nas_req);

                       
void  
pm_rpmh_post_set(rpmh_command_set_t *active_set, 
                 rpmh_command_set_t *sleep_set, 
                 pm_rpmh_data_type  *rsrc_rpmh_data,
                 uint32             *curr_msg_id,
                 boolean             is_nas_req);
                
void 
pm_rpmh_barrier_all(uint32 msg_id);

void 
pm_rpmh_barrier_single(uint32 msg_id);


#endif //__PM_RPMH_H__

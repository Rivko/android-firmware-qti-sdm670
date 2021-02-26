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

$Header: //components/rel/core.tz/2.0.2/pmic/pm/app/rpmh/src/pm_rpmh.h#1 $
$DateTime: 2018/02/06 02:57:17 $
$Author: pwbldsvc $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/17   akm     Created
===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "rpmh_client.h"
#include "rpmh_common.h"
#include "pm_prm_device.h"
#include "pm.h"


/*===========================================================================

                        FUNCTION DECLARTIONS

===========================================================================*/
pm_err_flag_type  
pm_rpmh_init(void);


pm_err_flag_type 
pm_rpmh_vreg_cfg_req( uint32 vrm_addr_ldo,  
                      pm_prm_vreg_data_type* ldo_active_data, 
                      pm_prm_vreg_data_type* ldo_sleep_data);


pm_err_flag_type  
pm_rpmh_create_vreg_cmd_set(rpmh_command_set_t *active_set, 
                            uint32 vrm_addr,
                            pm_prm_vreg_data_type *data,
                            rpmh_set_enum set_type );


pm_err_flag_type   
pm_rpmh_post_set(rpmh_command_set_t *active_set, 
                 uint32 *current_msg_id);

pm_err_flag_type 
pm_rpmh_issue_command(rpmh_set_enum set, uint32 address, uint32 data, uint32* msg_id);


pm_err_flag_type  
pm_rpmh_barrier_single(uint32 msg_id);

#endif //__PM_RPMH_H__

/*! \file
 *  
 *  \brief  pm_prm_device_init.c ----This file contains initialization functions
 *                                   prm device layer
 *  
 * Copyright (c) 2017 - 2018 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
 * QUALCOMM Proprietary.  Export of this technology or software is regulated 
 * by the U.S. Government. Diversion contrary to U.S. law prohibited.
 *
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/prm/src/pm_prm_device_init.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/17   akm     Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_prm_device_init.h"
#include "pm_prm_driver.h"
#include "pm_config_aop_pam.h"

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

#define PRM_CMD_DB_SUPPORTED_VER 0x100
#define PRM_CMD_DB_AUX_DATA_LEN 4
#define PRM_CMD_DB_AUX_REG_TYPE 1

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/


pm_prm_resource_type      *pm_prm_rsrc_data = NULL; 
unpa_resource_definition  pm_prm_dev_rsrc_def[PM_PRM_NUM_DEV_RSRC] = {0}; 


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
__attribute__((section("pm_cram_reclaim_pool")))
void pm_prm_device_init (void)
{
    uint32 i = 0;
    unpa_resource *dev_rsrc = NULL;
    vrm_result rslt = VRM_SUCCESS; 
    uint32 vrm_address = 0;
    uint16 cmd_db_ver = 0;
    cmd_db_err_type cmd_db_err = CMD_DB_SUCCESS;
    uint8 aux_data_len = 0;
    uint8 aux_data[PRM_CMD_DB_AUX_DATA_LEN] = {0};
  
    pm_prm_rsrc_data = (pm_prm_resource_type*)pm_target_information_get_specific_info(PM_PROP_PRM_DEVICE_RSRCS);

    if ((pm_prm_rsrc_data == NULL) || (pm_prm_rsrc_data->num_rsrcs <= 0))
    {
        return;
    }

    if (pm_prm_rsrc_data->num_rsrcs != PM_PRM_NUM_DEV_RSRC)
    {
        // number of resources doesn't match the PM_PRM_NUM_DEV_RSRC defined in pm_config_aop_pam.h file
        CORE_VERIFY(0);
    }
    
    for (i = 0; i < pm_prm_rsrc_data->num_rsrcs; i++)
    {
        pm_prm_dev_rsrc_def[i].name = (char*)pm_prm_rsrc_data->resource_data[i].rsrc_name;
        pm_prm_dev_rsrc_def[i].update_fcn = unpa_max_update_fcn;
        pm_prm_dev_rsrc_def[i].driver_fcn = pm_prm_scalar_driver_fcn;
        pm_prm_dev_rsrc_def[i].max = 0x1FFF; //Add Correct Maxes here
        pm_prm_dev_rsrc_def[i].client_types = UNPA_CLIENT_REQUIRED;
        pm_prm_dev_rsrc_def[i].attributes = UNPA_RESOURCE_DEFAULT;

        cmd_db_ver = cmd_db_query_version(pm_prm_rsrc_data->resource_data[i].cmd_db_name);

        if (cmd_db_ver != PRM_CMD_DB_SUPPORTED_VER)
        {
            // Abort if CMD DB version not supported by PRM driver
            CORE_VERIFY(0);
        }

        aux_data_len = cmd_db_query_len(pm_prm_rsrc_data->resource_data[i].cmd_db_name);

        if (aux_data_len != PRM_CMD_DB_AUX_DATA_LEN)
        {
            // Abort if CMD DB aux data length doesn't match expected length
            CORE_VERIFY(0);
        }

        cmd_db_err = cmd_db_query_aux_data(pm_prm_rsrc_data->resource_data[i].cmd_db_name, &aux_data_len, aux_data);

        if ((cmd_db_err != CMD_DB_SUCCESS) || (aux_data[0] != PRM_CMD_DB_AUX_REG_TYPE))
        {
            // Abort if resource type from CMD DB aux data is not regulator
            // XOBs/local control is not supported in AOP PRM driver
            CORE_VERIFY(0);
        }

        vrm_address = cmd_db_query_addr(pm_prm_rsrc_data->resource_data[i].cmd_db_name);
        CORE_VERIFY(vrm_address);

        rslt = vrm_get_rsrc_id(pm_prm_rsrc_data->resource_data[i].rsrc_category, vrm_address, &pm_prm_rsrc_data->resource_data[i].rsrc_id);
        CORE_VERIFY(rslt == VRM_SUCCESS);

        /*Create nodes for each resource*/
        dev_rsrc = unpa_create_resource (&pm_prm_dev_rsrc_def[i], 0);
        CORE_VERIFY_PTR(dev_rsrc);

        dev_rsrc->user_data = (void*)&pm_prm_rsrc_data->resource_data[i];
    }    
} 


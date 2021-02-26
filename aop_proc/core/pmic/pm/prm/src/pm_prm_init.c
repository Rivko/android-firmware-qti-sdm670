/*! \file
 *  
 *  \brief  pm_prm_init.c ---- Initializes PRM PAM Client Layer 
 *  
 * Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
 * QUALCOMM Proprietary.  Export of this technology or software is regulated 
 * by the U.S. Government. Diversion contrary to U.S. law prohibited.  
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/prm/src/pm_prm_init.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/17   akm     Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pmapp_npa.h"
#include "pm_prm_driver.h"
#include "pm_prm_device_init.h"
#include "pm_prm_init.h"
#include "pm_config_aop_pam.h"
#include "pm_prm_device_defs.h"

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/
pm_prm_node_resource_info          *pm_prm_client_rsrc_data = NULL;
unpa_resource_definition           pm_prm_client_rsrc_def[PM_PRM_NUM_CLIENT_RSRC] = {0}; 

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION pm_prm_init

DESCRIPTION
This function initializes the NPA for PMIC.

It does the following:
* It initializes the PMIC NPA Node and resources.

INPUT PARAMETERS
None.

RETURN VALUE
None.

DEPENDENCIES
NPA Framework should be available in the build being compiled for

SIDE EFFECTS
NONE.

===========================================================================*/
__attribute__((section("pm_cram_reclaim_pool")))
void
pm_prm_init (void)
{
    uint32 i = 0, j = 0;
    pm_prm_node_resource_info  *node_rsrc = NULL;
    pm_prm_npa_node_dependency *node_dep = NULL;
    unpa_resource *client_rsrc = NULL; 
    uint32 *num_of_pam_nodes = NULL;
    uint32 vrm_address = 0;
    vrm_result rslt = VRM_SUCCESS;
    
    /* Get the PAM info */
    pm_prm_client_rsrc_data = (pm_prm_node_resource_info*)pm_target_information_get_specific_info(PM_PROP_PRM_PAM_RSRCS);
    num_of_pam_nodes = (uint32*)pm_target_information_get_specific_info(PM_PROP_PRM_PAM_RSRC_NUM);

    if ((num_of_pam_nodes == NULL) || (pm_prm_client_rsrc_data == NULL))
    {
        //PM_LOG_MSG_INFO("No PAM Nodes Defined");
        return;
    }

    /* Device Layer Init */
    pm_prm_device_init ();

    /* Driver init to mainly initialize locks which can be used while processing rsrc */
    pm_prm_driver_init ();

    if (*num_of_pam_nodes != PM_PRM_NUM_CLIENT_RSRC)
    {
        // number of pam rsrcs doesn't match the PM_PRM_NUM_CLIENT_RSRC defined in pm_config_aop_pam.h file
        CORE_VERIFY(0);
    }

    /*pm_malloc(sizeof(unpa_resource_definition)*(*num_of_pam_nodes), (void**)&pm_prm_client_rsrc_def);
    CORE_VERIFY_PTR(pm_prm_client_rsrc_def);*/
    
    /* Construct the PMIC Client Nodes and Resources */
    for(i = 0; i < (*num_of_pam_nodes); i++)
    {
        node_rsrc = &pm_prm_client_rsrc_data[i];
        
        /* Populate Resource Data for PMIC nodes */
        pm_prm_client_rsrc_def[i].name = node_rsrc->group_name;
        pm_prm_client_rsrc_def[i].update_fcn = unpa_max_update_fcn;
        pm_prm_client_rsrc_def[i].driver_fcn = pm_prm_pam_driver_fcn;
        pm_prm_client_rsrc_def[i].max = node_rsrc->max_mode;
        pm_prm_client_rsrc_def[i].client_types = UNPA_CLIENT_REQUIRED;
        pm_prm_client_rsrc_def[i].attributes = node_rsrc->resource_attributes | UNPA_RESOURCE_DRIVER_UNCONDITIONAL;

        /* Register PMIC Client Layer Nodes and Resources with NPA */
        // sending an invalid initial value to always send all the resource settings request on 
        // the first request on the resource and short-circuit the resource setting from
        // the second requests on the resource in PRM driver layer 
        client_rsrc = unpa_create_resource (&pm_prm_client_rsrc_def[i], PM_PRM_RSRC_INVALID_INIT_STATE);
        CORE_VERIFY_PTR(client_rsrc);

        client_rsrc->user_data = (void*)node_rsrc;

        for (j = 0; j < node_rsrc->dependency_count; j++)
        {
            node_dep = &node_rsrc->node_dependencies[j];

            // client_handle could be NULL for non-shared device level resources and this is handled in pm_prm_driver function
            node_dep->client_handle = unpa_create_client(node_rsrc->node_name, node_dep->client_type, node_dep->rsrc_name);

            vrm_address = cmd_db_query_addr(node_dep->cmd_db_name);
            CORE_VERIFY(vrm_address);

            rslt = vrm_get_rsrc_id(node_dep->rsrc_category, vrm_address, &node_dep->rsrc_id);
            CORE_VERIFY(rslt == VRM_SUCCESS);
        }
    }

} /* end of pm_prm_init() */

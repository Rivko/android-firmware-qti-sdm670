/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"
#include "pm_prm_device_update.h"
#include "pm_prm_driver.h"
#include "pm_target_information.h"
#include "pm_ulog.h"
#include "CoreVerify.h"
#include "pm_rpmh.h"
#include "DDITimetick.h"
#include "DALSys.h"
#include "pm_malloc.h"

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

#define PM_PRM_GET_RSRC_STATE(rsrc_type, rsrc)                                \
case rsrc_type:                                                               \
    data_ptr = (pm_prm_##rsrc##_data_type *)pam_data[index].rail_data;        \
    data_ptr = ((pm_prm_##rsrc##_data_type *)data_ptr + mode_id);             \
    size     =  sizeof(pm_prm_##rsrc##_data_type)/sizeof(npa_resource_state); \
    break
    

//Helper function to process dependencies and post device layer requests
uint32
pm_prm_process_rsrc (npa_client_handle   client,
                     npa_resource       *resource,
                     npa_resource_state  mode_id,
                     uint32              index, 
                     uint32              dep_idx,
                     uint32             *curr_msg_id)
{
    pm_prm_rsrc_data_type       *rsrc_key_data = NULL;
    npa_client_handle            client_dep_h  = NULL;
    void                        *data_ptr      = NULL; 
    unsigned int                 size          = 0;
    npa_resource                *res_lock      = NULL;
    pm_prm_pam_client_cfg_type  *pam_data      = NULL;
    uint32 dep_inc = 0;
    
    pam_data = (pm_prm_pam_client_cfg_type *) resource->definition->data;
        
    //Get initial dep to get a lock on the resource node
    client_dep_h = NPA_DEPENDENCY( resource, dep_idx );
    
    res_lock = client_dep_h->resource;
    
    npa_resource_lock(res_lock);

    client_dep_h = NPA_DEPENDENCY( resource, dep_idx );
    
    rsrc_key_data = 
    
    (pm_prm_rsrc_data_type*)client_dep_h->resource->definition->data;
    
    switch(pam_data[index].rail_type)
    {
        PM_PRM_GET_RSRC_STATE(PM_PRM_RSRC_VRM_VREG, vreg);
        PM_PRM_GET_RSRC_STATE(PM_PRM_RSRC_VRM_CLK,  clk);
        default:
            CORE_VERIFY(0);
    }
    
    rsrc_key_data->pam_req = TRUE;
    
    npa_pass_request_attributes( client, client_dep_h);

    npa_issue_vector_request(client_dep_h, size,
                             (npa_resource_state *)data_ptr);
    
    // Clear pam request flag for resource
    rsrc_key_data->pam_req = FALSE;

    dep_inc += DEP_OFFSET;
    dep_idx += DEP_OFFSET;    

    *curr_msg_id = rsrc_key_data->rpmh_data.msg_id;
    
    rsrc_key_data->rpmh_data.msg_id=0;
    
    //Unlock node once data is sent to RPMH driver
    npa_resource_unlock(res_lock);

    return dep_inc;
}

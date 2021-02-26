#ifndef __PM_PRM_DRIVER_H__
#define __PM_PRM_DRIVER_H__
/*===========================================================================
DESCRIPTION
  This file contains prm driver functions for pam and device layer. 
  
Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/prm/inc/pm_prm_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/2016 rl      Change file to prm convention and add support for RPMh 
12/14/2010 umr     NPA Transaction support
06/03/2010 umr     Created.
===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_prm_device.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

//Since we only support suppressible and required clients, 
//Dependency adder is 2  
//If suppressible2 is to be supported this should be increased to 3
#define DEP_OFFSET  2

/*===========================================================================

                        GLOBAL VARIABLES

===========================================================================*/

extern npa_resource_plugin pm_prm_vector_plugin;

/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/

npa_resource_state
pm_prm_pam_driver_fcn (npa_resource      *resource,
                       npa_client_handle  client,
                       npa_resource_state mode_id);
npa_resource_state
pm_prm_vector_driver_fcn (npa_resource      *resource,
                          npa_client_handle  client,
                          npa_resource_state value);
                     
npa_resource_state
pm_prm_update_vector(npa_resource *resource, 
                     npa_client_handle client);
                     

npa_resource_state
pm_prm_scalar_driver_fcn (npa_resource      *resource,
                          npa_client_handle  client,
                          npa_resource_state value);
                     
                     
void 
pm_prm_create_vector(npa_client *client);


void
pm_prm_destroy_vector(npa_client *client);


void
pm_prm_barrier_request(npa_client_handle client, 
                       uint32 msg_id, 
                       boolean is_single);
                       
npa_query_status 
pm_prm_query_fcn(npa_resource *resource,
                 unsigned int query_id,
                 npa_query_type *query_result);


#endif /* __PM_PRM_H__ */

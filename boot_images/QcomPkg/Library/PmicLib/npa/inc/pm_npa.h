#ifndef PM_NPA_H
#define PM_NPA_H
/*===========================================================================


                  P M    NPA   H E A D E R    F I L E

DESCRIPTION
  This file contains prototype definitions to support interaction
  with the QUALCOMM Power Management ICs.

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/14/2010 umr     NPA Transaction support
06/03/2010 umr     Created.
===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "rpm.h"
#include "pm_npa_device.h"
/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
typedef struct
{
  void *rail_data; /* Type of client to create */
  pm_npa_resouce_type rail_type;
}pm_npa_pam_client_cfg_type;

typedef struct pm_npa_node_resource_info_type
{
    const char                              *group_name;     /* Name of resource */
    unsigned                                 max_mode;       /* Maximum state/length of vector resource */
    unsigned int                             resource_attributes;     /* Resource Attributes */
    pm_npa_pam_client_cfg_type              *resource_data;  /* User data field  */

    const char                              *node_name;      /* Node name */
    unsigned int                             node_attributes;   /* Node Attributes */
    npa_user_data                           *node_data;       /* User data */
    npa_node_dependency                     *node_dependencies;     /* Dep array */
    unsigned int                            dependency_count;
}pm_npa_node_resource_info;


typedef struct
{
    const char                              *rsrc_name;            /* Name of resource */
    const char                              *rmt_rsrc_name;        /* Name of resource */
}pm_npa_remote_name_type;

typedef struct
{
   pm_npa_remote_name_type                  *remote_resource;
   unsigned int                             num_resources;
}pm_npa_remote_resource_type;

typedef struct pm_pwr_resource_info_type
{
    rpm_resource_type resource_type;
    unsigned          resource_index; //internal resource index
    void*             data1; // data associated with this rail
    void*             data2; // data associated with this rail
    void*             data3; // data associated with this rail
}pm_pwr_resource_info_type;;

/*===========================================================================

                        DEFINITIONS

===========================================================================*/

/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/

npa_resource_state
pm_npa_resource_set (npa_resource      *resource,
                     npa_client_handle  client,
                     npa_resource_state mode_id);

npa_resource_state
pm_npa_process_rsrc (npa_client_handle client,
                     npa_resource *resource,
                     npa_resource_state mode_id,
                     uint32 index, uint32 dep_idx);

npa_resource_state
pm_npa_resource_set_vdd_uvol (npa_resource      *resource,
                             npa_client_handle  client,
                             npa_resource_state req_uv);

#endif /* PM_NPA_H */

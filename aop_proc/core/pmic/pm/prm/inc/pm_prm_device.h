#ifndef __PM_PRM_DEVICE_H__
#define __PM_PRM_DEVICE_H__
/*===========================================================================
DESCRIPTION
This file contains prototype definitions prm device layer

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/prm/inc/pm_prm_device.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/17   akm     Created
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include "unpa.h"
#include "unpa_resource.h"
#include "rpmh_client.h"
#include "rpmh_common.h"
#include "CoreVerify.h"
#include "pm_target_information.h"
#include "pm_utils.h"
#include "device_info.h"
#include "cmd_db.h"
#include "vrm_inc.h"


/*===========================================================================

TYPE DEFINITIONS

===========================================================================*/
#define PM_PRM_SCALAR

#define NPA_ARRAY_SIZE( array ) (sizeof(array)/sizeof(array[0]))

#define PM_PRM_RSRC_VRM_VREG VRM_RSRC_REGULATOR
#define PM_PRM_RSRC_VRM_CLK VRM_RSRC_XO_BUFFER
 
#define PM_PRM_DEV_RSRC_NAME(chipid, rtype, index, rkey)  \
    "/pm/" #rtype #chipid #index "/" #rkey

/*! 
 * RPMH Set Type 
 */
typedef struct
{
    uint32     active_set;            /*ActiveSetValue*/
    boolean    dirty_active;          /*Active set dirty flag*/
    uint32     msg_id;                /*RPMh Message Id for Vectors*/
}pm_rpmh_set_type;

/*! 
 * Generic enable,
 */
typedef enum
{
    PM_PRM_DEV_DISABLE = 0, // default
    PM_PRM_DEV_ENABLE  = 1,
    PM_PRM_DEV_INVALID
}pm_prm_generic_enable_type;

/*! 
 * Vreg Mode Type 
 */
typedef enum
{
    PM_PRM_VREG_MODE_NONE = 0,
    PM_PRM_VREG_MODE_RETENTION = 4,
    PM_PRM_VREG_MODE_LPM       = 5,
    PM_PRM_VREG_MODE_AUTO      = 6,
    PM_PRM_VREG_MODE_NPM       = 7,
    PM_PRM_VREG_MODE_INVALID    
}pm_prm_vreg_mode_type;

/*! 
 * Device Layer Resource Data 
 */
typedef struct
{   /*Resource data*/
    const char               *rsrc_name;     /* NPA resource name */
    const char               *cmd_db_name;   /* Name to refer command db*/
    vrm_rsrc_category         rsrc_category;
    vrm_settings              rsrc_key;      /* En,Volt, Mode, Hr*/
    /*VRM Data*/                             
    uint32                    rsrc_id;       /*VRM resource id*/
    /*Pam Request*/                          
    boolean                   pam_req;       /*Flag for notifying PAM Request*/
    /*RPMh Data*/                           
    pm_rpmh_set_type          rpmh_data;     /*Data for RPMh Set information*/
}pm_prm_rsrc_data_type;


/*! 
 * Device Layer Data 
 */
typedef struct
{
   pm_prm_rsrc_data_type                  *resource_data;
   uint32                                  num_rsrcs;
}pm_prm_resource_type;

/*! 
 * PAM Layer Rail Data and type 
 */
typedef struct
{
  void *rail_data;
  vrm_rsrc_category rsrc_type;
}pm_prm_pam_client_cfg_type;

typedef struct pm_prm_npa_node_dependency_type
{
  const char               *rsrc_name;     /* Name of resource */
  const char               *cmd_db_name;   /* Name to refer command db*/
  vrm_rsrc_category         rsrc_category;
  vrm_settings              rsrc_key;      /* En,Volt, Mode, Hr*/
  uint32                    rsrc_id;       /* VRM resource id */
  uint32                    client_type;   /* Type of client to create */
  unpa_client              *client_handle; /* Type of client to create */
} pm_prm_npa_node_dependency;

/*! 
 * PAM Layer Node Data 
 */
typedef struct pm_prm_node_resource_info_type
{
    const char                              *group_name;           /* Name of resource */
    uint32                                   max_mode;             /* Maximum state/length of vector resource */
    uint32                                   resource_attributes;  /* Resource Attributes */
    pm_prm_pam_client_cfg_type              *resource_data;        /* User data field  */
    const char                              *node_name;            /* Node name */
    uint32                                   node_attributes;      /* Node Attributes */
    void                                    *node_data;            /* User data */
    pm_prm_npa_node_dependency              *node_dependencies;    /* Dep array */
    uint32                                   dependency_count;
}pm_prm_node_resource_info;

/*! 
 * Clock Resource Data 
 */
typedef struct
{
    unpa_resource_state enable;
}pm_prm_clk_data_type; 

/*! 
 * LDO Resource Data 
 */
typedef struct
{
    unpa_resource_state mode:5;
    unpa_resource_state voltage:13;
    unpa_resource_state headroom:13;
    unpa_resource_state enable:1;
}pm_prm_vreg_data_type;

/*===========================================================================

DEFINITIONS

===========================================================================*/
/*! 
 * CLK Resource keys and offsets 
 */
#define NUM_OF_CLK_KEYS             1
#define PM_CLK_ENABLE_OFFSET        0

/*! 
 * SMPS Resource keys and offsets 
 */
#define NUM_OF_VREG_KEYS            3
#define PM_VREG_VOLTAGE_OFFSET      0
#define PM_VREG_ENABLE_OFFSET       4
#define PM_VREG_MODE_OFFSET         8
//#define PM_VREG_HEADROOM_OFFSET     12

/*===========================================================================

                        FUNCTION DECLARATION

===========================================================================*/
unpa_resource_state
pm_prm_get_vreg_rsrc_state(vrm_settings rsrc_key, 
                           pm_prm_vreg_data_type *pam_data);
    
#endif /* __PM_PRM_DEVICE_H__ */


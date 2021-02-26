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

$Header: //components/rel/core.tz/2.0.2/pmic/pm/app/rpmh/src/pm_prm_device.h#1 $
$DateTime: 2018/02/06 02:57:17 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/30/17   akm     Created
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include "comdef.h"

/*===========================================================================

TYPE DEFINITIONS

===========================================================================*/
/*! 
 * LDO Data Type
 */
typedef struct
{
   uint32 mode;
   uint32 voltage;
   uint32 headroom;
   uint32 enable;
}pm_prm_vreg_data_type; 

/*! 
 * Generic Enable/Disable Type
 */
typedef enum
{
    PM_PRM_DEV_DISABLE = 0,
    PM_PRM_DEV_ENABLE  = 1,
    PM_PRM_DEV_INVALID
}pm_prm_generic_enable_type;


/*! 
 * Vreg Mode Type
 */
typedef enum
{   
    PM_PRM_VREG_MODE_RETENTION = 4,
    PM_PRM_VREG_MODE_LPM       = 5,
    PM_PRM_VREG_MODE_AUTO      = 6,
    PM_PRM_VREG_MODE_NPM       = 7,
    PM_PRM_VREG_MODE_INVALID    
}pm_prm_vreg_mode_type;



/*===========================================================================

                         DEFINITIONS

===========================================================================*/
#define NUM_OF_VREG_KEYS 1

#define PM_VREG_VOLTAGE_OFFSET  0
#define PM_VREG_ENABLE_OFFSET   4
#define PM_VREG_MODE_OFFSET     8
#define PM_VREG_HEADROOM_OFFSET 12

#endif /* __PM_PRM_DEVICE_H__ */


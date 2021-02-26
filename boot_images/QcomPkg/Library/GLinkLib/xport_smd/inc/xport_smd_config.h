#ifndef XPORT_SMD_CONFIG_H
#define XPORT_SMD_CONFIG_H

/**
 * @file xport_smd_config.h
 *
 * Internal definitions for SMD Glink transport configurations
 */

/*==============================================================================
     Copyright (c) 2014 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/14   bm      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "glink_os_utils.h"
#include "smem_type.h"
#include "smd_type.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

typedef struct _xport_smd_config_type
{
  const char* remote_ss;            /* Remote host name        */
  smd_channel_type edge;            /* SMD edge                */
} xport_smd_config_type;       

#endif //XPORT_SMD_CONFIG_H


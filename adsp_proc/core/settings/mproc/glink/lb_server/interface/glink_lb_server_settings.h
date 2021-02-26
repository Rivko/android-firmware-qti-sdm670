#ifndef GLINK_LB_SERVER_SETTINGS_H
#define GLINK_LB_SERVER_SETTINGS_H

/**
 * @file glink_lb_server_settings.h
 *
 * Internal definitions for GLink Loopback Server settings structure
 */

/*==============================================================================
     Copyright (c) 2017 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/settings/mproc/glink/lb_server/interface/glink_lb_server_settings.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/17   jg      Initial version
===========================================================================*/
#include "DALStdDef.h"

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
typedef struct
{
  const char *host;               /* This subsystem name  */
  const char *ctrl_channel;       /* Control channel name */
  const char *transport;          /* Transport name       */
  uint32      num_subsytems;      /* Number of Subsytems  */
  const char *subsytems[];        /* Subsytems names      */
} glink_lb_server_config_type;


#endif //GLINK_LB_SERVER_SETTINGS_H

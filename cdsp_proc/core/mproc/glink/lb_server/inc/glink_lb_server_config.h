#ifndef GLINK_LB_SERVER_CONFIG_H
#define GLINK_LB_SERVER_CONFIG_H

/**
 * @file glink_lb_server_config.h
 *
 * Internal definitions for GLink Loopback Server configurations
 */

/*==============================================================================
     Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/glink/lb_server/inc/glink_lb_server_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/17   jg      Split out settings structure definition to separate .h file
10/20/14   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink_lb_server_settings.h"


/*===========================================================================
                     FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      glink_lb_server_config_get
===========================================================================*/
/**
  This function initializes and returns configuration.
  
  @param    None.

  @return   Configuration.
*/
/*=========================================================================*/
const glink_lb_server_config_type *glink_lb_server_config_get(void);

#endif //GLINK_LB_SERVER_CONFIG_H

/*===========================================================================

            GLink Loopback Server Device Configuration

=============================================================================

  @file
    glink_lb_server_<proc>.c

    Contains structures to be used in GLink Loopback Server configuration.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/13/15   db      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "DALStdDef.h"
#include "glink_lb_server_settings.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/


/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

const glink_lb_server_config_type glink_lb_server_devcfg = 
{
  "dsps",                            /* This subsystem name  */
  "LOOPBACK_CTL_DSPS",               /* Control channel name */
  "SMEM",                            /* Transport name       */
  3,                                 /* Number of Subsytems  */
  {"apss", "mpss", "lpass"}          /* Subsytems names      */
};


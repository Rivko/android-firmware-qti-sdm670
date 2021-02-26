#ifndef ERR_INJECT_CRASH_H
#define ERR_INJECT_CRASH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR service crash simulator
                
GENERAL DESCRIPTION
  Command handler for injecting various types of crashes for testing

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2014-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/eic/src/err_inject_crash.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/25/17   jlg     Established settings interface header file
10/28/11   mcg     File created.

===========================================================================*/

#include "tms_utils.h"
#include "tms_utils_msg.h"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
void err_inject_crash_init(void);

#define ERR_DIAG_PROC_ID       DIAG_QDSP6_PROC

#endif /* ERR_INJECT_CRASH_H */

#ifndef BOOT_SHARED_FUNCTIONS_CONSUMER_H
#define BOOT_SHARED_FUNCTIONS_CONSUMER_H

/*===========================================================================

                                Boot Shared Functions Consumer
                                Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for Boot's shared 
  functions (Consumer). 

Copyright  2014, 2017 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who          what, where, why
--------   --------     ------------------------------------------------------
05/09/17   sj    		Added Api to qurey the battery status in XBL ramdump
07/28/14   ck           Initial creation
============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_shared_functions.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
boolean boot_shared_functions_init(void);

boolean boot_check_display_allowed();

#endif /* BOOT_SHARED_FUNCTIONS_CONSUMER_H */



#ifndef COMDEF_H
#define COMDEF_H
/*===========================================================================

                   S T A N D A R D    D E C L A R A T I O N S

DESCRIPTION
  This header file contains general data types that are of use to all modules.
  The values or definitions are dependent on the specified
  target.  T_WINNT specifies Windows NT based targets, otherwise the
  default is for ARM targets.

  T_WINNT  Software is hosted on an NT platforn, triggers macro and
           type definitions, unlike definition above which triggers
           actual OS calls

Copyright (c) 2014, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/15   bh      Unifying comdef.h, moving to Include folder
07/18/14   kedara  Initial creation.

===========================================================================*/
#include "com_dtypes.h"

#ifndef PACKED_POST
#define PACKED_POST __attribute__((__packed__))
#endif

#endif /* COMDEF_H */

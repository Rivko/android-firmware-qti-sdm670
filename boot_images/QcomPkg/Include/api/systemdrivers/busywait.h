#ifndef BUSYWAIT_H
#define BUSYWAIT_H
/*===========================================================================

                         Busywait Header File

DESCRIPTION
  This header file contains the public API for the busywait component.

      Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/12   cmd     Initial version.

===========================================================================*/

#include "com_dtypes.h"     /* Common Definitions and Types */
#include "DALSys.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*==========================================================================

  FUNCTION      BUSYWAIT_INIT

  DESCRIPTION   This function initializes the busywait functionality.

  PARAMETERS    none

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS if the initialization is successful
                and DAL_ERROR otherwise.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult busywait_init (void);

/*==========================================================================

  FUNCTION      BUSYWAIT

  DESCRIPTION   This function pauses the execution of a thread for a
                specified time.

  PARAMETERS    pause_time_us - Time to pause in microseconds

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void busywait (uint32 pause_time_us);

#endif /* BUSYWAIT_H */


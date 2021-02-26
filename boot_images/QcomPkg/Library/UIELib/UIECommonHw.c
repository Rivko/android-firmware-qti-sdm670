/*===========================================================================
                Unified Image Encryption (UIE) Common HW Utils

GENERAL DESCRIPTION
 Provide chipset independent hardware-related helper routines.

Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                                 INCLUDE FILES
===========================================================================*/
#include <stdbool.h>
//#include <stddef.h>
#include <string.h>
#include "UIE.h"
#include "UIEPriv.h"
#include "UIEEnv.h"

#define BREAKIF(cond) if (cond) break

bool uie_fec_error_occurred()
{
  boolean retval = false;
  uint16 value = uie_get_fec();

  do {
    /* If zero, error not detected */
    BREAKIF(value == 0);

    /* Clear the FEC_ESR register by writing non-zero */
    uie_clear_fec();
    retval = true;
  } while(0);

  return retval;
}
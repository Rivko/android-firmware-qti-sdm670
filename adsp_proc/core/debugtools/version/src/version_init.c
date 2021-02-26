/*===========================================================================

GENERAL DESCRIPTION
  This file contains functions and other defines used for
  image version reporting feature.

Copyright (c) 2013 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/debugtools/version/src/version_init.c#1 $

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "version.h"

#include "comdef.h"
#include "image_version.h"

void version_init(void)
{
   // IMAGE_INDEX is defined in version.scons
   if (0 != image_version_populate_version(IMAGE_INDEX))
   {
      //ERR("unable to populate version", 0, 0, 0);
   }
}

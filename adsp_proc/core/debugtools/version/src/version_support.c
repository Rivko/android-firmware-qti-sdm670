/*===========================================================================

GENERAL DESCRIPTION
  This file contains functions and other defines used for
  image version reporting feature.

Copyright 2013-17 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/debugtools/version/src/version_support.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "version.h"

VERSION_HANDLE version_map_name(VERSION_NAME name)
{
   return (VERSION_NULL);
}

VERSION_NAME version_map_handle(VERSION_HANDLE handle)
{
   return (VERSION_NULL);
}

VERSION_HANDLE version_create_name(VERSION_NAME name)
{
   return (VERSION_NULL);
}

VERSION_BOOL version_search_name(VERSION_NAME name)
{
   return (VERSION_FALSE);
}

VERSION_HANDLE version_register_name(VERSION_NAME name, VERSION_P ver)
{
   return (VERSION_NULL);
}

VERSION_HANDLE version_register_name_ex(VERSION_NAME name, VERSION_BUF buffer, VERSION_BUFLEN len)
{
   return (VERSION_NULL);
}

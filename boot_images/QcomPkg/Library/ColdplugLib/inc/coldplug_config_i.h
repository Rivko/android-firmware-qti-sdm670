/***********************************************************************
 * coldplug
 *
 * Coldplug Config items
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * Coldplug internal configuration items.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/inc/coldplug_config_i.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2017-05-18   rp    Create

  ===========================================================================*/

#ifndef __COLDPLUG_CONFIG_I_H__
#define __COLDPLUG_CONFIG_I_H__

#include "coldplug_config.h"

#define COLDPLUG_DEBUG_ENABLE

#ifndef COLDPLUG_DEBUG_INFO_MAX_COUNT
  #define COLDPLUG_DEBUG_INFO_MAX_COUNT (50)
#endif

#endif /* __COLDPLUG_CONFIG_I_H__ */

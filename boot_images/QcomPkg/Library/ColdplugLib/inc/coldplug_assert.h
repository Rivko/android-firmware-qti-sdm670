/***********************************************************************
 * coldplug_assert.h
 *
 * Coldplug assert module.
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * Coldplug assert macros.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/inc/coldplug_assert.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2017-05-18   rp    Create

===========================================================================*/

#ifndef __COLDPLUG_ASSERT_H__
#define __COLDPLUG_ASSERT_H__

#include "coldplug_config_i.h"
#include "assert.h"

#define COLDPLUG_ASSERT(condn)  ASSERT((condn))

#endif /* __COLDPLUG_ASSERT_H__ */

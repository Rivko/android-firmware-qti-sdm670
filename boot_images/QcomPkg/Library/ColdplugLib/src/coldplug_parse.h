/***********************************************************************
 * Coldplug partition parser module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug partition parser module.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_parse.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#ifndef __COLDPLUG_PARSE_H__
#define  __COLDPLUG_PARSE_H__

#include "coldplug_config_i.h"
#include "coldplug_comdef.h"
#include "coldplug_device.h"

int coldplug_parse_lun(struct coldplug_lun *clun);

#endif /*  __COLDPLUG_PARSE_H__ */

/***********************************************************************
 * tftp_comdef_modem.h
 *
 * Short description
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/tftp/os/inc/tftp_comdef_modem.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-12-30   dks  Fixes to config and log modules.
2014-10-20   dks  Remove rfs and tftp dependency
2014-09-19   dks  Add hooks to extract performance numbers.
2014-06-04   rp   Create

===========================================================================*/

#ifndef __TFTP_COMDEF_MODEM_H__
#define __TFTP_COMDEF_MODEM_H__

#include "tftp_config_i.h"

#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NHLOS Builds"
#endif

#include "comdef.h"
#include <stdint.h>

#endif /* not __TFTP_COMDEF_MODEM_H__ */

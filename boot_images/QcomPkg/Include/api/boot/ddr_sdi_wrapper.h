#ifndef __DDR_SDI_WRAPPER_H__
#define __DDR_SDI_WRAPPER_H__

/*=============================================================================
                              DDR HAL
                            Header File
GENERAL DESCRIPTION
This is the target header file for DDR HAL.

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/20/15   tw      Initial revision.
=============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"

boolean ddr_sdi_init(void);
boolean ddr_sdi_exit(void);
#endif /*__DDR_SDI_WRAPPER_H__*/

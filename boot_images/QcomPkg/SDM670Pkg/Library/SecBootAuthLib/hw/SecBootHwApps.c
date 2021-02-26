/*****************************************************************************
*
* @file  secboot_hw.h (Secboot Hardware API)
*
* @brief Secboot Hardware API to get authentication information from the
*        Security Control fuses
*        
*
* Copyright (c) 2013-2014 by QUALCOMM, Technology Inc.  All Rights Reserved.
*
*****************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/28/14   hw       Add RoT support
09/03/13   hw      Init version, Code update for bear pbl
============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "secboot_hw.h"
#include "HALhwio.h"
#include "secboot_hwio.h"
#include "secboot.h"
/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

secboot_hw_etype secboot_hw_get_current_sw_rot
(
  uint32* current_sw_rot
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;

  if (NULL == current_sw_rot)
  {
    ret_val = E_SECBOOT_HW_INVALID_PARAM;
    return ret_val;
  }

  *current_sw_rot = HWIO_INF(SW_ROT_CONFIG, CURRENT_SW_ROT_APPS);

  return ret_val;
}


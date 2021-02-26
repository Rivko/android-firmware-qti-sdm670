/*========================================================================
avtimer.c

This file contains avtimer hardware abstraction layer.

Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/resampler/src/hal_rs_common.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "HAL_rs.h"
#include "HAL_rs_v1.h"
#include "HAL_rs_v2.h"
#include "HAL_rs_v3.h"

#include "qurt_elite.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

hal_rs_func_def_t* hal_rs_func_def_ptr_g = NULL;

/**

@return
*/
void HAL_rs_init_common(uint32 base_virt_addr, uint32 hw_version, void* hal_func_def_ptr)
{
   hal_rs_func_def_ptr_g = (hal_rs_func_def_t*)hal_func_def_ptr;

   switch(hw_version)
   {
      case RESAMPLER_HW_VERSION_2:
      default:
      {
         HAL_rs_init_v1(base_virt_addr, hal_rs_func_def_ptr_g);
      }
      break;
      case RESAMPLER_HW_VERSION_3:
      {
         HAL_rs_init_v2(base_virt_addr, hal_rs_func_def_ptr_g);
      }
      break;
      case RESAMPLER_HW_VERSION_4:
      {
         HAL_rs_init_v3(base_virt_addr, hal_rs_func_def_ptr_g);
      }
      break;
   }
}



/*
============================================================================

FILE:          HAL_rs_v1.h

DESCRIPTION:   Resampler HW driver HAL header file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary	
============================================================================

============================================================================
  EDIT HISTORY FOR MODULE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2011-03-21   qm   Initial version.
============================================================================
*/
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef HAL_RS_V1_H
#define HAL_RS_V1_H

#include "qurt_elite.h"
#include "common.h" //reg_wr reg_rd
#include "HAL_rs.h"

void HAL_rs_init_v1(uint32 base_virt_addr, hal_rs_func_def_t* hal_func_def_ptr);

#endif

#ifdef __cplusplus
}
#endif //__cplusplus



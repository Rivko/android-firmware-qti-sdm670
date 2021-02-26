/*========================================================================

*//** @file elite_thrd_prio.h
This file contains structure definition for Elite thread priorities.

Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: 

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/27/16   mk     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "mmdefs.h"
#include "adsp_error_codes.h"
#include "elite_thrd_prio_dev_cfg.h"

#ifndef ELITE_THRD_PRIO_H
#define ELITE_THRD_PRIO_H

#if defined(__cplusplus)
extern "C" {
#endif

/**
  Initializes elite thread priorities from device config

  @param[in] None

  @return
  ADSPResult

  @dependencies
  None
*/
ADSPResult elite_thrd_prio_init(void);

/**
  Returns priority of a thread for a given thread id

  @param[in] thrd_prio_id - Thread priority ID

  @return
  ADSPResult

  @dependencies
  None
*/
uint32_t elite_get_thrd_prio(elite_thrd_prio_id_t thrd_prio_id);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //ELITE_THRD_PRIO_H



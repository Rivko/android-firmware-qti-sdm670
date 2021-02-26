#ifndef _RSC_INTERNAL_H_
#define _RSC_INTERNAL_H_
/*==============================================================================
  FILE:         rsc_internal.h

  OVERVIEW:     This file provides prototypes for internal or utility functions 
                for rsc driver.

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/rsc/common/rsc_internal.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "rsc.h"
#include "rsc_common.h"

/*=======================================================================
 *                           RSC INTERNAL MACROS
 *======================================================================*/
/* Macro to check rsc type validity */
#define RSC_VALID_TYPE(h) (h < RSC_TOTAL_NUM)

/*=======================================================================
 *                           RSC INERNAL FUNCTIONS
 *======================================================================*/
/** 
 * rsc_target_init
 * 
 * @brief Target specific initialization. This function must be called
 *        as part of rsc common driver initialization.
 */
void rsc_target_init(void);

/** 
 * rsc_setup 
 *  
 * @brief Setup HW
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_setup(void);

/** 
 * rsc_mode_index 
 *  
 * @brief Returns the index of the input mode for a given RSC instance.
 *
 * @param type:   RSC type
 * @param lpm:    Mode id for which we want to find index. 
 * @param index:  Outputed mode index for given lpm id
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_mode_index(rsc_type type, uint8 lpm_id, uint32 *index);

#endif /* _RSC_INTERNAL_H_ */


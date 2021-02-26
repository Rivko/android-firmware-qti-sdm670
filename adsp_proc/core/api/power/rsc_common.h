#ifndef _RSC_COMMON_H_
#define _RSC_COMMON_H_
/*==============================================================================
  FILE:         rsc_common.h

  OVERVIEW:     This file contains APIs for external modules to interact with
                RSC HW block(s).

  DEPENDENCIES: RSC driver must be initialized before calling any APIs.

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/
#include "comdef.h"

/*=======================================================================
 *                          TYPE DEFINITIONS
 *======================================================================*/
/* Generic enumeration that are used to indicate which RSC actions are
 * intended for. */
typedef enum
{
  RSC_CHILD   = 0,  /* Typically Q6 RSC */
  RSC_PARENT  = 1,  /* Typically subsystem RSC */

  RSC_TOTAL_NUM
}rsc_type;

/* Enum to indicate RSC sequencer API return types. */
typedef enum
{
  RSC_SUCCESS = 0,
  RSC_INVALID_HANDLE = 1,
  RSC_INVALID_PARAM = 2,
  RSC_NO_SEQ_MEMORY = 3,
  RSC_INVALID_MODE_INDEX = 4,
  RSC_MEM_ALLOC_FAIL = 5,
  RSC_ERROR = 0xFFFFFFFF
}rsc_result_t;

/* Externally software triggerable modes */
typedef enum
{
  /* Child Sequences */
  RSC_SW_TRIG_BUS_ISOLATION = 0,
  RSC_SW_TRIG_BUS_DEISOLATION,

  /* Parent Sequences */
  /* NONE CURRENTLY */

}rsc_sw_trig_mode;

/*=======================================================================
 *                         FUNCTIONS DECLARATIONS
 *======================================================================*/
/**
 * rsc_trigger_seq
 *
 * @brief Trigger RSC sequencer for a given RSC and mode id.
 *
 * @param type: RSC type to the RSC instance to trigger.
 * @param mode_id: LPM mode id to be trigger.
 *
 * @return RSC_SUCCESS if sequence was updated successfully else error
 *         code - see rsc_result_t.
 */
rsc_result_t rsc_trigger_seq(rsc_type type, rsc_sw_trig_mode mode_id);

#endif


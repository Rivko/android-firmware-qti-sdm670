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

//===========================================================================
//                      Includes and Type Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------
#include "comdef.h"

typedef struct rsc_s* rsc_handle;
typedef uint16 rsc_cmd_t;

//---------------------------------------------------------------------------
//  Type Declarations
//---------------------------------------------------------------------------
/* Enum to indicate RSC sequencer API return types. */
typedef enum
{
  RSC_SUCCESS = 0,
  RSC_INVALID_HANDLE = 1,
  RSC_INVALID_PARAM = 2,
  RSC_NO_SEQ_MEMORY = 3,
  RSC_INVALID_MODE_INDEX = 4,
  RSC_MEM_ALLOC_FAIL = 5,
  RSC_ERROR = -1
}rsc_result_t;

//===========================================================================
//                       Function Declarations
//===========================================================================
/**
 * <!-- rsc_get_count -->
 *
 * @brief Returns the number of RSC instances found for a given entity(i.e name)
 *
 * @param entity: Pointer to the RSC instance name
 *
 * @return 0 = If no RSC instances found
          >0 = if the given instance is found
 */
uint32 rsc_get_count(const char *entity);

/**
 * <!-- rsc_get_handles -->
 *
 * @brief Returns the RSC handles associated with input entity in passed
 *        buffer. It will return minimum of available handles and handle
 *        buffer size.
 *
 * @param entity: Name of the RSC handles are requested for (e.g qdsp6, mss etc).
 * @param count: Number of elements input buffer can accomodate.
 * @param handles: Input buffer where RSC handles will be returned.
 *
 * @return Number of RSC handles returned for the entity (ideally it should
 *         be equal to count parameter).
 */
uint32 rsc_get_handles(const char *entity, uint32 count, rsc_handle *handle);


/**
 * <!-- rsc_program_dyn_cmd_seq -->
 *
 * @brief Programs input command sequence to RSC HW dynamically.
 *
 * @param handle: Handle to the RSC where we want to program the sequence.
 * @param seq: Array of RSC commands.
 * @param seq_length: Length of above array.
 * @mode_id: Dynamic mode id assosicated with the new command sequence.
 *
 * @return RSC_SUCCESS if sequence was updated successfully else error
 *         code - see rsc_result_t.
 */

rsc_result_t rsc_program_dyn_cmd_seq(rsc_handle handle, rsc_cmd_t  *seq,
                              uint16 seq_length, uint32 *mode_id);

/**
 * <!-- rsc_trigger_seq -->
 *
 * @brief Trigger RSC sequencer for a given handle and mode id.
 *
 * @param handle: Handle to the RSC instance to trigger.
 * @param mode_id: LPM mode id to be trigger.
 *
 * @return RSC_SUCCESS if sequence was updated successfully else error
 *         code - see rsc_result_t.
 */
rsc_result_t rsc_trigger_seq(rsc_handle handle, uint32 mode_id);

#endif

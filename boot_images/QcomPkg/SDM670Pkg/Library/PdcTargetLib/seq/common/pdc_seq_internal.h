#ifndef __PDC_SEQ_INTERNAL_H__
#define __PDC_SEQ_INTERNAL_H__
/*===========================================================================
                              pdc_seq.h

DESCRIPTION:
  This file provides APIs, macros and typedefs that are used internally
  for PDC sequencer driver.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                    Includes and Type Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------
#include "pdc_seq.h"

//---------------------------------------------------------------------------
//  Macros
//---------------------------------------------------------------------------

/* Macro to check basic PDC sequencer handle validity */
#define PDC_SEQ_VALID_HANDLE(h) (((h) != NULL) && ((h)->addr != NULL))

/* Macro to fill out array and size fields */
#define PDC_SEQ_ARRAY(arr) arr, (sizeof(arr)/sizeof(arr[0]))

//===========================================================================
//                       Function Declarations
//===========================================================================

/**
 * <!-- pdc_seq_target_init -->
 *
 * @brief Target specific initialization for the PDC sequencer driver.
 *
 * Usually this function is expected to be called as part of common driver
 * initialization and will obtain different PDCs driver will talk to.
 */
void pdc_seq_target_init (void);

/**
 * <!-- pdc_seq_handle_target_init -->
 *
 * @brief Performs any target specific initialization required for a given
 *        pdc handle e.g. disabling certain feature or updating some settings.
 *
 * This is an internal function expected to be called during PDC
 * handle initialization.
 *
 * @note No input validation as it is an internal function.
 *
 * @param handle: PDC handle to be initialized.
 *
 * @return PDC Sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_handle_target_init (pdc_seq_handle handle);

/**
 * <!-- pdc_seq_handle_env_init -->
 *
 * @brief Environment specific initialization for a PDC handle like
 *        mapping physical address to perform HW IO.
 *
 * This is an internal function expected to be called during PDC
 * handle initialization. 
 *
 * @note No input validation as it is an internal function.
 *
 * @param handle: PDC handle to be initialized.
 *
 * @return PDC Sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_handle_env_init (pdc_seq_handle handle);

/**
 * <!-- pdc_seq_handle_internal_cfg -->
 *
 * @brief Performs initialization of various internal fields/registers
 *        for PDC sequencer.
 *
 * This function is expected to be used internally from driver during
 * initialization and so there is no handle/input validation.
 *
 * @param handle: PDC handle that is being initialized.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_handle_internal_cfg (pdc_seq_handle handle);

/**
 * <!-- pdc_seq_copy_cmd_seq -->
 *
 * @brief Copies given PDC handle's command sequences in to actual HW.
 *
 * This function is expected to be used internally from driver during
 * initialization and so there is no handle/input validation.
 *
 * @param handle: PDC handle for which we are copying command sequences.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_copy_cmd_seq (pdc_seq_handle handle);

/**
 * <!-- pdc_seq_mode_index -->
 *
 * @brief Returns the index of an input mode for a given PDC instance.
 *
 * This allows different and sparse mode ordering as opposed to having
 * consecutive ordering for modes. This function is expected to be used
 * internally from driver, so there is no handle/input validation.
 *
 * @param handle: Handle to PDC instance
 * @param mode_id: Mode id for which we are querying index
 *
 * @return Index corresponding to mode else error code.
 */
int pdc_seq_mode_index (pdc_seq_handle handle, uint16 mode_id);

#endif // __PDC_SEQ_INTERNAL_H__

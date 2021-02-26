#ifndef __PDC_SEQ_INTERNAL_H__
#define __PDC_SEQ_INTERNAL_H__
/*==============================================================================
  FILE:         pdc_seq.h

  OVERVIEW:     This file provides APIs, macros and typedefs that are used internally
                for PDC sequencer driver.

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/seq/common/pdc_seq_internal.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "pdc_seq.h"

/*=============================================================================
                        GLOBAL EXTERNAL VARIABLES
 =============================================================================*/
extern pdc_seq_t *g_pdcSeq;

/*=============================================================================
                            TYPES AND MACROS
 =============================================================================*/
/* Macro to fill out array and size fields */
#define PDC_SEQ_ARRAY(arr) arr, (sizeof(arr)/sizeof(arr[0]))

//===========================================================================
//                       Function Declarations
//===========================================================================
/**
 * pdc_seq_target_init
 *
 * @brief Target specific initialization for the PDC sequencer driver.
 *
 * Usually this function is expected to be called as part of common driver
 * initialization and will obtain different PDCs driver will talk to.
 */
void pdc_seq_target_init (void);

/**
 * pdc_seq_handle_internal_cfg
 *
 * @brief Performs initialization of various internal fields/registers
 *        for PDC sequencer.
 *
 * This function is expected to be used internally from driver during
 * initialization and so there is no handle/input validation.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_handle_internal_cfg(void);

/**
 * pdc_seq_copy_cmd_seq
 *
 * @brief Copies given PDC handle's command sequences in to actual HW.
 *
 * This function is expected to be used internally from driver during
 * initialization and so there is no handle/input validation.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_copy_cmd_seq(void);

/**
 * pdc_seq_mode_index
 *
 * @brief Returns the index of an input mode for a given PDC instance.
 *
 * This allows different and sparse mode ordering as opposed to having
 * consecutive ordering for modes. This function is expected to be used
 * internally from driver, so there is no handle/input validation.
 *  
 * @param mode_id: Mode id for which we are querying index
 *
 * @return Index corresponding to mode else error code.
 */
int pdc_seq_mode_index(pdc_low_power_mode_type mode_id);

#endif /* __PDC_SEQ_INTERNAL_H__ */


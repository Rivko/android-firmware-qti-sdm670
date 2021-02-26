/*===========================================================================
                             pdc_seq_target.c

DESCRIPTION:
  This file implements functions that are specific to environment where
  PDC sequencer driver is running (processor/os/exception level).

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                   Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------

#include "msmhwiobase.h"
#include "pdc_seq.h"
#include "pdc_seq_internal.h"

//---------------------------------------------------------------------------
//  Static Variables
//---------------------------------------------------------------------------

/* Subsystem base address - obtained by different mechanism depending on 
 * the environment. This will be used in conjuction with PDC offset */
static uint8* g_pdc_seq_ss_base = NULL;

//===========================================================================
//                       Function Defintions
//===========================================================================

//---------------------------------------------------------------------------
//  Public Function Definitions
//---------------------------------------------------------------------------

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
pdc_seq_result_t pdc_seq_handle_env_init (pdc_seq_handle handle)
{
  if (g_pdc_seq_ss_base == NULL)
  {
    g_pdc_seq_ss_base = (uint8 *)AOSS_BASE;
  }

  if (handle == NULL)
  {
    return PDC_SEQ_INVALID_PARAM;
  }

  /* AOP has one to one physical to virtual mapping */
  handle->addr = handle->offset + g_pdc_seq_ss_base;
  
  if (handle->rsc_offset != NULL)
  {
    /* PDC has attached RSC - get its address too */
    handle->rsc_addr = handle->rsc_offset + g_pdc_seq_ss_base;
  }

  return PDC_SEQ_SUCCESS;
}

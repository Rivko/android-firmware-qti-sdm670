/*===========================================================================
                            pdc_seq_internal.c

DESCRIPTION:
  This file implements some common, internal and utility functions for
  PDC sequencer driver.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include "DALSys.h"
#include "pdc_seq.h"
#include "pdc_seq_hal.h"
#include "pdc_seq_hal_bsp.h"

//---------------------------------------------------------------------------
//  External References
//---------------------------------------------------------------------------

/* Array of PDC sequencer handles */
extern pdc_seq_t *g_pdc_seqs;

/* Number of PDC sequencer handles in the array above */
extern uint32 g_pdc_seq_count;

//===========================================================================
//                       Function Defintions
//===========================================================================

//---------------------------------------------------------------------------
//  Public Function Definitions
//---------------------------------------------------------------------------

/**
 * <!-- pdc_seq_count -->
 *
 * @brief Returns the number of PDC associated with input entity. This
 *        function is used before querying for PDC sequencer handles.
 *
 * On a given subsystem, driver will contain only PDCs for entities it
 * handles. It will not be exhaustive for entire system.
 *
 * Current expectation is that each entity will have only one associated
 * PDC but we are keeping interfce a generic, mainly to be compatible with
 * other sequencers we have and have better flexibility going forward.
 *
 * @dependency pdc_seq_driver_init
 *
 * @param entity: Name of the entity for which we are querying PDC count
 *                (e.g. "aop", "mss", etc)
 *
 * @return Number of PDC sequencers associated with input entity.
 */
uint32 pdc_seq_count (const char *entity)
{
  uint32 i;
  uint32 count = 0;

  if (entity == NULL)
  {
    return count;
  }

  for (i = 0; i < g_pdc_seq_count; i++)
  {
    if (strcmp (entity, g_pdc_seqs[i].name) == 0)
    {
      count++;
    }
  }

  return count;
}

/**
 * <!-- pdc_seq_handles -->
 *
 * @brief Returns the array of pdc handles associated with input entity
 *        in an input buffer. It will return minimum of available handles
 *        and handle buffer size.
 *
 * Current expectation is that each entity will have only one associated
 * PDC but we are keeping interfce a generic, mainly to be compatible with
 * other sequencers we have and have better flexibility going forward.
 *
 * @dependency pdc_seq_driver_init
 *             pdc_seq_get_count (if not known, also allows more flexibility)
 *
 * @param entity: Name of the entity for which we are querying PDC count
 *                (e.g. "aop", "mss", etc)
 * @param count: Number of handles input handles' buffer (next param) can
 *               contain (ideally obtained from get_count function for
 *               flexibility).
 * @param handles: Input handles' buffer where PDC sequencer handles will
 *                 be returned.
 *
 * @return Number of PDC sequencer handles returned for the input entity.
 *         It should ideally be same as input param 'count'.
 */
uint32 pdc_seq_handles
(
  const char *entity,
  uint32 count,
  pdc_seq_handle *handles
)
{
  uint32 i;
  uint32 handles_copied = 0;

  if ((entity == NULL) || (count == 0) || (handles == NULL))
  {
    return handles_copied;
  }

  for (i = 0; i < g_pdc_seq_count; i++)
  {
    if (strcmp (entity, g_pdc_seqs[i].name) == 0)
    {
      if (handles_copied < count)
      {
        handles[handles_copied++] = &g_pdc_seqs[i];
      }
      else
      {
        break;     /* Match but no more space - just stop */
      }
    }
  }

  return handles_copied;
}

/**
 * <!-- pdc_seq_handle_internal_cfg -->
 *
 * @brief Performs initialization of various internal fields/registers
 *        for PDC sequencer.
 *
 * @param handle: PDC handle that is being initialized.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_handle_internal_cfg (pdc_seq_handle handle)
{
  uint32 i;
  uint32 pf_ts_count;
  const pdc_seq_hal_bsp_cfg_t *cfg = handle->cfg;
  int ret_val;

  /* PDC branch configuration */
  for (i = 0; i < cfg->br_count; i++)
  {
    ret_val = pdc_seq_hal_cfg_br_addr (handle->addr, i, cfg->br_addr[i]);
    if (ret_val)
    {
      return PDC_SEQ_INVALID_PARAM;
    }
  }

  /* PDC delay configuration */
  for (i = 0; i < cfg->delay_count; i++)
  {
    ret_val = pdc_seq_hal_cfg_delay (handle->addr, i, cfg->delay[i]);
    if (ret_val)
    {
      return PDC_SEQ_INVALID_PARAM;
    }
  }

  /* Allocating buffer for profiling timestamps */
  pf_ts_count = pdc_seq_hal_profile_ts_count (handle->addr);
  if (pf_ts_count > 0)
  {
    if (DALSYS_Malloc(pf_ts_count * sizeof (uint64),
                      (void **)&(handle->profile_ts)) == DAL_SUCCESS)
    {
       DALSYS_memset(handle->profile_ts, 0x0, pf_ts_count * sizeof (uint64));
    }
    else
    {
      return PDC_SEQ_ERROR;
    }
  }

  /* Any additional configuration */

  return PDC_SEQ_SUCCESS;
}

/**
 * <!-- pdc_seq_copy_cmd_seq -->
 *
 * @brief Copies given PDC handle's command sequences in to actual HW.
 *
 * @param handle: PDC handle for which we are copying command sequences.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_copy_cmd_seq (pdc_seq_handle handle)
{
  uint32 i, j;
  uint32 copied_cmds = 0;
  pdc_seq_result_t result = PDC_SEQ_SUCCESS;
  pdc_seq_hal_bsp_mode_t *curr_mode;
  const pdc_seq_hal_bsp_mode_t *ref_mode = NULL;

  for (i = 0; i < handle->mode_count; i++)
  {
    curr_mode = &handle->modes[i];
    if (curr_mode->length == 0)
    {
      result = PDC_SEQ_INVALID_PARAM;
      break;
    }

    /* Check if this mode shares sequence with any previous modes */
    for (j = 0; j < i; j++)
    {
      ref_mode = &handle->modes[j];
      if (ref_mode->cmds == curr_mode->cmds)
      {
        break;
      }
    }

    if (j == i)
    {
      /* Mode has a different sequence than previously programmed */
      copied_cmds =
        pdc_seq_hal_copy_cmd_seq (handle->addr, handle->copied_cmds,
                                  curr_mode->cmds, curr_mode->length);

      if (copied_cmds == 0)
      {
        result = PDC_SEQ_NO_MEM;       /* No more space available - return */
        break;
      }

      curr_mode->start_addr = (int16)handle->copied_cmds;
      handle->copied_cmds += copied_cmds;
    }
    else
    {
      curr_mode->start_addr = ref_mode->start_addr;
    }
  }

  return result;
}

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
int pdc_seq_mode_index (pdc_seq_handle handle, uint16 mode_id)
{
  uint32 i;
  pdc_seq_hal_bsp_mode_t *mode;

  for (i = 0; i < handle->mode_count; i++)
  {
    mode = &handle->modes[i];
    if (mode->mode_id == mode_id)
    {
      return i;
    }
  }

  return PDC_SEQ_UNSUPPORTED_MODE;
}

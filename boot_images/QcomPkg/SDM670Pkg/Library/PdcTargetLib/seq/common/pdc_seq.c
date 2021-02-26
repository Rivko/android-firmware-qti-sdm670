/*===========================================================================
                              pdc_seq.c

DESCRIPTION:
  This file implements some of the APIs defined in PDC Sequencer public
  header file.

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

#include "comdef.h"
#include "pdc_seq.h"
#include "pdc_seq_internal.h"
#include "pdc_seq_hal.h"
#include "pdc_seq_hal_bsp.h"
#include "CoreVerify.h"


//===========================================================================
//                       Function Defintions
//===========================================================================

//---------------------------------------------------------------------------
//  Global Function Definitions
//---------------------------------------------------------------------------

/**
 * <!-- pdc_seq_driver_init -->
 *
 * @brief Performs common driver initialization for PDC sequencer
 *        independent of PDC instance.
 *
 * @note This function must be called before calling any other PDC sequencer
 *       functions.
 */
void pdc_seq_driver_init (void)
{
  pdc_seq_target_init();
}

/**
 * <!-- pdc_seq_handle_init -->
 *
 * @brief Performs PDC sequencer instance specific initialization.
 *
 * @note This function must be called on a handle/instance before calling
 *       any other APIs that take handle as input.
 *
 * @dependency pdc_seq_driver_init
 *
 * @param handle: PDC sequencer handle to initialize.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_handle_init (pdc_seq_handle handle)
{
  pdc_seq_result_t result;

  if ((handle == NULL) || (handle->cfg == NULL) ||
      (handle->modes == NULL) || (handle->offset == 0))
  {
    return PDC_SEQ_INVALID_HANDLE;
  }

  result = pdc_seq_handle_env_init (handle);
  if (result != PDC_SEQ_SUCCESS)
  {
    return result;
  }

  result = pdc_seq_handle_target_init (handle);
  if (result != PDC_SEQ_SUCCESS)
  {
    return result;
  }

  result = pdc_seq_handle_internal_cfg (handle);
  if (result != PDC_SEQ_SUCCESS)
  {
    return result;
  }

  result = pdc_seq_copy_cmd_seq (handle);

  return result;
}

/**
 * <!-- pdc_enable -->
 *
 * @brief Enables or disables PDC.
 *
 * There is no dedicated sequencer enable logic. Enabling logic applies to
 * entire PDC block.
 *
 * @dependency pdc_seq_driver_init,
 *             pdc_seq_handle_init
 *
 * @param handle: Handle to PDC sequencer that we want to enable or disable.
 * @param enable: TRUE to enable PDC sequencer else FALSE.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_enable (pdc_seq_handle handle, boolean enable)
{
  if (!PDC_SEQ_VALID_HANDLE (handle))
  {
    return PDC_SEQ_INVALID_HANDLE;
  }

  pdc_seq_hal_enable (handle->addr, (enable == TRUE ? 1 : 0));

  return PDC_SEQ_SUCCESS;
}

/**
 * <!-- pdc_set_wakeup_time -->
 *
 * #define pdc_set_wakeup_time pdc_seq_set_wakeup_time
 *
 * @brief Sets up the wake up time for the given PDC.
 *
 * @dependency pdc_seq_driver_init,
 *             pdc_seq_handle_init
 *
 * @param handle: Handle to PDC sequencer for which we are configuring
 *                wakeup time.
 * @param wakeup: Absolute time at which we want PDC to wake up.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_set_wakeup_time (pdc_seq_handle handle, uint64 wakeup)
{
  if (!PDC_SEQ_VALID_HANDLE (handle))
  {
    return PDC_SEQ_INVALID_HANDLE;
  }

  /* Preferred method is through RSC but PDC may not always have RSC e.g
   * for debug subsystem */
  if (handle->rsc_addr != NULL)
  {
    pdc_seq_hal_cfg_wakeup_time (handle->rsc_addr, wakeup);
  }
  else
  {
    pdc_seq_hal_cfg_wakeup_time_ex (handle->addr, wakeup);
  }

  return PDC_SEQ_SUCCESS;
}

/**
 * <!-- pdc_seq_set_lpm -->
 *
 * @brief Sets the low power mode (lpm) configuration for the PDC sequencer.
 *        This configuration will be effective when the PDC sequencer kicks
 *        in next time.
 *
 * @dependency pdc_seq_driver_init,
 *             pdc_seq_handle_init
 *
 * @param handle: Handle to PDC sequencer we are configuring for LPM.
 * @param mode_id: low power mode id.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_set_lpm (pdc_seq_handle handle, uint16 mode_id)
{
  int mode_index;
  pdc_seq_hal_bsp_mode_t *mode;

  if (!PDC_SEQ_VALID_HANDLE (handle))
  {
    return PDC_SEQ_INVALID_HANDLE;
  }

  mode_index = pdc_seq_mode_index (handle, mode_id);
  if (mode_index == PDC_SEQ_UNSUPPORTED_MODE)
  {
    return PDC_SEQ_UNSUPPORTED_MODE;
  }

  mode = &handle->modes[mode_index];

  /* Preferred method is through RSC but PDC may not always have RSC e.g
   * for debug subsystem */
  if (handle->rsc_addr != NULL)
  {
    pdc_seq_hal_cfg_start_addr (handle->rsc_addr, mode->start_addr);
  }
  else
  {
    pdc_seq_hal_cfg_start_addr_ex (handle->addr, mode->start_addr);
  }

  return PDC_SEQ_SUCCESS;
}

/**
 * <!-- pdc_seq_profile_ts_count -->
 *
 * #define pdc_enable pdc_seq_enable
 *
 * @brief Returns number of profiling timestamps available for a PDC instance.
 *
 * Expected usage is calling this function once during init time.
 *
 * @dependency pdc_seq_driver_init,
 *             pdc_seq_handle_init
 *
 * @param handle: PDC handle for which we are querying timestamp count.
 *
 * @return Number of profiling timestamps
 *         Non-zero if present
 *         0 if no profiling timestamps or invalid input.
 */
uint32 pdc_seq_profile_ts_count (pdc_seq_handle handle)
{
  if (!PDC_SEQ_VALID_HANDLE (handle))
  {
    return 0;
  }

  return pdc_seq_hal_profile_ts_count (handle->addr);
}

/**
 * <!-- pdc_seq_enable_profile_ts -->
 *
 * @brief Enables profiling timestamps for PDC sequencer.
 *
 * @dependency pdc_seq_driver_init,
 *             pdc_seq_handle_init
 *
 * @param handle: PDC handle to enable profiling timestamps for.
 * @param enable: TRUE to enable profiling else FALSE.
 *                (Enabled by default)
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_enable_profile_ts
(
  pdc_seq_handle handle,
  boolean enable
)
{
  uint32 i;
  uint32 ts_count;

  if (!PDC_SEQ_VALID_HANDLE (handle))
  {
    return PDC_SEQ_INVALID_HANDLE;
  }

  ts_count = pdc_seq_profile_ts_count (handle);

  for (i = 0; i < ts_count; i++)
  {
    pdc_seq_hal_enable_profile_ts (handle->addr, i, (enable == TRUE ? 1 : 0));
  }

  return PDC_SEQ_SUCCESS;
}

/**
 * <!-- pdc_seq_profile_ts -->
 *
 * @brief Returns a pointer to buffer that has latest profiling timestamps.
 *
 * @dependency pdc_seq_driver_init,
 *             pdc_seq_handle_init
 *
 * @param handle: PDC handle for which we want to read profiling timestamps.
 * @param count: Number of timestamps read (output)
 *
 * @return Pointer to timestamp buffer if successful else NULL.
 */
const uint64* pdc_seq_profile_ts (pdc_seq_handle handle, uint32 *count)
{
  uint32 i;
  uint32 ts_count;

  if (!PDC_SEQ_VALID_HANDLE (handle))
  {
    return NULL;
  }

  ts_count = pdc_seq_profile_ts_count (handle);

  for (i = 0; i < ts_count; i++)
  {
    handle->profile_ts[i] = pdc_seq_hal_profile_ts (handle->addr, i);
  }

  return handle->profile_ts;  /* Returned as const ptr to avoid modifications */
}

/**
 * <!-- pdc_enable_arc_timer -->
 *
 * #define pdc_enable_arc_timer pdc_seq_enable_arc_timer
 *
 * @brief Enables timer valid signal to ARC from a given PDC. Also allows
 *        propagating PDC timer match value to ARC in HW.
 *
 * By default, this is enabled and should be overrided only when necessary.
 *
 * @dependency pdc_seq_driver_init,
 *             pdc_seq_handle_init
 *
 * @param handle: Handle to PDC for which we enable/disable timer sent to ARC.
 * @param enable: TRUE to enable else FALSE
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_enable_arc_timer
(
  pdc_seq_handle handle,
  boolean enable
)
{
  if (!PDC_SEQ_VALID_HANDLE (handle))
  {
    return PDC_SEQ_INVALID_HANDLE;
  }

  pdc_seq_hal_enable_arc_timer (handle->addr, (enable == TRUE ? 1 : 0));

  return PDC_SEQ_SUCCESS;
}

/**
 * <!-- pdc_seq_sys_init -->
 *
 * @brief This function initializes common PDC sequencer driver as well
 *        as all the PDC instances supported on a given subsystem.
 *
 * This function is provided where it is difficult for primary PDC
 * sequencer client to have it intiailized and enabled before RPMh
 * communication is up. The intended usage is to plug this funciton
 * in subsystem initialization mechanism.
 *
 * @note If a subsystem calls this function, PDC sequencer clients do
 *       not need to intialize their respective PDC instances separately.
 */
void pdc_seq_sys_init (void)
{
  uint32 i;
  pdc_seq_t *seq;
  pdc_seq_result_t result;

  extern pdc_seq_t *g_pdc_seqs;
  extern uint32 g_pdc_seq_count;

  /* Instance agnostic common initialization */
  pdc_seq_driver_init ();

  /* instance specific initialization */
  for (i = 0; i < g_pdc_seq_count; i++)
  {
    seq = &g_pdc_seqs[i];
    result = pdc_seq_handle_init (seq);
    CORE_VERIFY (result == PDC_SEQ_SUCCESS);

    /* Enabling PDC */
    result = pdc_seq_enable (seq, TRUE);
    CORE_VERIFY (result == PDC_SEQ_SUCCESS);
  }
}

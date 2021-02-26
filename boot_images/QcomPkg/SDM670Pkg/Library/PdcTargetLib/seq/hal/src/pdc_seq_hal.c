/*===========================================================================
                            pdc_seq_hal.c

DESCRIPTION:
  This file implements the HAL interface for PDC sequencer.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                       Includes and Defines
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "HALhwio.h"
#include "pdc_seq_hal.h"
#include "pdc_seq_hal_bsp.h"
#include "pdc_seq_hwio.h"

//===========================================================================
//                       Function Defintions
//===========================================================================

//---------------------------------------------------------------------------
//  Local Function Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  Public Function Definitions
//---------------------------------------------------------------------------

/**
 * <!-- pdc_seq_hal_enable -->
 *
 * @brief Enables/Disables PDC HW block (not just PDC sequencer. All PDC 
 *        components share a single enable bit).
 *
 * @param pdc_base: PDC instance's base address.
 * @param enable: Value to set PDC enabled or disabled. Expected use case
 *                is only one time enablement during init.
 *                (1 to enable, else 0)
 */
void pdc_seq_hal_enable (uint8 *pdc_base, uint8 enable)
{
  enable = (enable != FALSE) ? 1 : 0;
  HWIO_OUTXF (pdc_base, ENABLE_PDC, ENABLE_PDC, enable);
}

/**
 * <!-- pdc_seq_hal_clk_gate_enable -->
 *
 * @brief Enables/disables clock gating for the PDC HW block.
 *
 * @param pdc_base: PDC instance's base address.
 * @param enable: Value to set for PDC clock gating. Expected use case is
 *                only one time during init.
 *                (1 to enable, 0 to disable)
 */
void pdc_seq_hal_clk_gate_enable (uint8 *pdc_base, uint8 enable)
{
  uint8 disable = enable ? 0 : 1;
  HWIO_OUTXF (pdc_base, ENABLE_PDC, DISABLE_CLK_GATE, disable);
}

/**
 * <!-- pdc_seq_hal_cfg_wakeup_time -->
 *
 * @brief Configures the wake up time for the PDC.
 *
 * @param rsc_base: Base of attached RSC block that will send the
 *                  wake up time via hidden tcs during execution.
 * @param wakeup_time: Time to wake up the PDC.
 */
void pdc_seq_hal_cfg_wakeup_time (uint8 *rsc_base, uint64 wakeup_time)
{
  const uint32 mask = (uint32)(-1);
  const uint32 valid_mask = 
    HWIO_FMSK (HIDDEN_TCS_CMD0_DATA_DRVd, PDC_MATCH_VALUE_VALID);
  uint32 low = wakeup_time & mask;
  uint32 high = (wakeup_time >> 32) & mask;
  high |= valid_mask;

  HWIO_OUTXI (rsc_base, HIDDEN_TCS_CMD1_DATA_DRVd, 0, low);
  HWIO_OUTXI (rsc_base, HIDDEN_TCS_CMD0_DATA_DRVd, 0, high);
}

/**
 * <!-- pdc_seq_hal_cfg_wakeup_time_ex -->
 *
 * @brief Configures the wake up time for the PDC directly/explicitly.
 *
 * @note This is NOT a preferred mechanism as it assumes config noc 
 * path to be ON which is not guaranteed. However, there are some
 * PDC (e.g. Debug) which do not have an RSC attached. This method
 * is for such PDC instances.
 *
 * @param pdc_base: PDC instance's base address.
 * @param wakeup_time: Time to wake up the PDC.
 */
void pdc_seq_hal_cfg_wakeup_time_ex (uint8 *pdc_base, uint64 wakeup_time)
{
  const uint32 mask = (uint32)(-1);
  uint32 low = wakeup_time & mask;
  uint32 high = (wakeup_time >> 32) & mask;

  HWIO_OUTX (pdc_base, TIMER_MATCH_VALUE_LO, low);
  HWIO_OUTX (pdc_base, TIMER_MATCH_VALUE_HI, high);
}

/**
 * <!-- pdc_seq_hal_cfg_start_addr -->
 *
 * @brief Sets the start address from where the PDC sequencer will execute
 *        next time it is triggered.
 *
 * @param rsc_base: Base of attached RSC block that will send the
 *                  start address via hidden tcs during execution.
 * @param start_addr: Start address.
 */
void pdc_seq_hal_cfg_start_addr (uint8 *rsc_base, uint16 start_addr)
{
  uint32 start_addr_tcs = start_addr |
   HWIO_FMSK (HIDDEN_TCS_CMD2_DATA_DRV0, PDC_SEQ_START_ADDR_VALID);
   
  HWIO_OUTX (rsc_base, HIDDEN_TCS_CMD2_DATA_DRV0, start_addr_tcs);
}

/**
 * <!-- pdc_seq_hal_cfg_start_addr_ex -->
 *
 * @brief Sets the start address from where the PDC sequencer will execute
 *        next time it is triggered.
 *
 * @note This is NOT a preferred mechanism as it assumes config noc 
 * path to be ON which is not guaranteed. However, there are some
 * PDC (e.g. Debug) which do not have an RSC attached. This method
 * is for such PDC instances.
 *
 * @param pdc_base: PDC instance's base address.
 * @param start_addr: Start address.
 */
void pdc_seq_hal_cfg_start_addr_ex (uint8 *pdc_base, uint16 start_addr)
{
  HWIO_OUTXF (pdc_base, SEQ_START_ADDR, SEQ_START_ADDR, start_addr);
}

/**
 * <!-- pdc_seq_hal_enable_arc_timer -->
 *
 * @brief Enables/disable timer valid signal sent to ARC. By default
 *        it is enabled.
 *
 * @param pdc_base: PDC instance's base address for which timer for
 *                  ARC needs to be enabled/disabled.
 * @param enable: 1 (non-zero) to enable, 0 to disable
 */
void pdc_seq_hal_enable_arc_timer (uint8 *pdc_base, uint8 enable)
{
  uint8 disable = (enable != 0) ? 0 : 1;    /* Reverse logic */
  HWIO_OUTX (pdc_base, TIMER_DISABLED_FOR_ARC, disable);
}

/**
 * <!-- pdc_seq_hal_cfg_pwr_ctl_override -->
 *
 * @brief Programs override configuration for the power controls handled
 *        by the sequencer.
 *
 * @param pdc_base: PDC instance's base address.
 * @param mask: Bit mask - one bit set per applicable pwr_ctl
 *             (bit position corresponds to pwr_ctl number).
 * @param value: Override value mask - one bit per set pwr_ctls.
 */
void pdc_seq_hal_cfg_pwr_ctl_override
(
  uint8 *pdc_base,
  uint32 mask, 
  uint32 value
)
{
  HWIO_OUTX (pdc_base, SEQ_PWR_CTRL_OVERRIDE_MASK, mask);
  HWIO_OUTX (pdc_base, SEQ_PWR_CTRL_OVERRIDE_VAL, value);
}

/**
 * <!-- pdc_seq_hal_cfg_wait_event_override -->
 *
 * @brief Programs override configuration for the wait events in the PDC
 *        sequences.
 *
 * @param pdc_base: PDC instance's base address.
 * @param mask: Bit mask - one bit set per applicable wait_event.
 *              (bit position corresponds to wait_event number).
 * @param value: Override value mask - one bit per set wait_events.
 */
void pdc_seq_hal_cfg_wait_event_override
(
  uint8 *pdc_base, 
  uint32 mask, 
  uint32 value
)
{
  HWIO_OUTX (pdc_base, SEQ_WAIT_EVENT_OVERRIDE_MASK, mask);
  HWIO_OUTX (pdc_base, SEQ_WAIT_EVENT_OVERRIDE_VAL, value);
}

/**
 * <!-- pdc_seq_hal_cfg_br_event_override -->
 *
 * @brief Programs override configuration for the branch events that are
 *        used in the PDC sequences.
 *
 * @param pdc_base: PDC instance's base address.
 * @param mask: Bit mask - one bit set per applicable branch_event
 *              (bit position corresponds to br_event number).
 * @param value: Override value mask - one bit per set br_event.
 */
void pdc_seq_hal_cfg_br_event_override
(
  uint8 *pdc_base,
  uint32 mask, 
  uint32 value
)
{
  HWIO_OUTX (pdc_base, SEQ_BR_EVENT_OVERRIDE_MASK, mask);
  HWIO_OUTX (pdc_base, SEQ_BR_EVENT_OVERRIDE_VAL, value);
}

/**
 * <!-- pdc_seq_hal_cfg_bf_addr -->
 *
 * @brief Configure the branch address at a given index.
 *
 * @param pdc_base: PDC instance's base address.
 * @param br_index: Index of the branch for which we want to config address.
 * @param addr: Actual address to jump to for the given branch index.
 *
 * @return 0 if successful, non-zero (negative) otherwise
 */
int pdc_seq_hal_cfg_br_addr (uint8 *pdc_base, uint16 br_index, uint8 addr)
{
  if (br_index >= PDC_SEQ_HAL_BR_ADDR_REG_COUNT)
  {
    return PDC_SEQ_HAL_INVALID_INDEX;
  }

  HWIO_OUTXI (pdc_base, SEQ_CFG_BR_ADDR_b, br_index, addr);
  return 0;
}

/**
 * <!-- pdc_seq_hal_cfg_delay -->
 *
 * @brief Configures a delay value for the given delay indexed register.
 *
 * @param pdc_base: PDC instance's base address.
 * @param delay_index: Index of the register that needs to be configured.
 * @param delay: Actual delay value
 *
 * @return 0 if successful, non-zero (negative) otherwise
 */
int pdc_seq_hal_cfg_delay (uint8 *pdc_base, uint16 delay_index, uint32 delay)
{
  if (delay_index >= PDC_SEQ_HAL_DELAY_REG_COUNT)
  {
    return PDC_SEQ_HAL_INVALID_INDEX;
  }

  HWIO_OUTXI (pdc_base, SEQ_CFG_DELAY_VAL_v, delay_index, delay);
  return 0;
}

/**
 * <!-- pdc_seq_hal_profile_ts_count -->
 *
 * @brief Returns number of profiling timestamps available for a PDC instance.
 *
 * @param pdc_base: PDC instance's base address.
 * 
 * @return Number of profiling timestamps.
 */
uint32 pdc_seq_hal_profile_ts_count (uint8 *pdc_base)
{
  uint32 count = 
    HWIO_INXFI (pdc_base, PDC_PARAM_RESOURCE_DRVd, 0, BLK_NUM_PROFILING_UNIT);

  return count;
}

/**
 * <!-- pdc_seq_hal_enable_profile_ts -->
 *
 * @brief Enables the given profiling time stamp.
 *
 * @param pdc_base: PDC instance's base address.
 * @param index: Index of the profiling timestamp to be enabled/disabled
 * @param enable: Non-zero to enable, 0 to disable
 *
 * @return 0 if successful else non-zero (negative) error.
 */
int pdc_seq_hal_enable_profile_ts (uint8 *pdc_base, uint8 index, uint8 enable)
{
  if (index >= pdc_seq_hal_profile_ts_count (pdc_base))
  {
    return PDC_SEQ_HAL_INVALID_INDEX;
  }

  enable = (enable != 0) ? 1 : 0;
  HWIO_OUTXI (pdc_base, PROFILING_UNITpf_EN, index, enable);
  return 0;
}

/**
 * <!-- pdc_seq_hal_profile_ts -->
 *
 * @brief Reads the specified profiling time stamp.
 *
 * @param pdc_base: PDC instance's base address.
 * @param index: Index of the profiling timestamp to be read.
 *
 * @return Actual non-zero timestamp if event has occured else
 *         0 (event did not occur or invalid event which would not occur)
 */
uint64 pdc_seq_hal_profile_ts (uint8 *pdc_base, uint8 index)
{
  uint32 low;
  uint64 high;

  if (index >= pdc_seq_hal_profile_ts_count (pdc_base))
  {
    return 0;
  }

  low = HWIO_INXI2 (pdc_base, PROFILING_UNITpf_DRVd_TIMESTAMP_LO, 0, index);
  high = HWIO_INXI2 (pdc_base, PROFILING_UNITpf_DRVd_TIMESTAMP_HI, 0, index);

  return ((high << 32) | low);
}

/**
 * <!-- pdc_seq_hal_copy_cmd_seq -->
 *
 * @brief Programs a sequencer memory with input sequence starting from a
 *        specified address
 *
 * @param pdc_base: PDC instance's base address.
 * @param start_addr: Address (offset in seq memory) to start copying at.
 * @param cmds: Array of sequence commands
 * @param length: Length of command sequence.
 *
 * @return Number of commands copied to HW (in successful call it must 
 *         be equal to 'length' param) 
 */
uint16 pdc_seq_hal_copy_cmd_seq
(
  uint8 *pdc_base,
  uint16 start_addr,
  pdc_seq_cmd_t *cmds,
  uint16 length
)
{
  uint32 mask = 0;
  uint32 value = 0;
  uint32 cmd_shift = 0;
  uint16 cmds_copied = 0;
  uint16 i = 0;
  uint16 addr = start_addr;
  const uint16 cmds_per_reg = 4;
  uint32 max_cmds = 
    HWIO_INXFI (pdc_base, PDC_PARAM_SEQ_CONFIG_DRVd, 0, BLK_NUM_SEQ_CMD_WORDS) *
    cmds_per_reg;

  if ((start_addr >= max_cmds) || (start_addr + length > max_cmds))
  {
    /* If start address is out of range or if we cannot fit entire
     * sequence, bail out */
    return cmds_copied;
  }

  for (i = 0; i < length; i++, addr++)
  {
    switch (addr % cmds_per_reg)
    {
      case 0:
        if (0 != cmds_copied)
        {
          /* We hit register boundary and had copied some commands earlier -
           * write it to register */
          HWIO_OUTXMI (pdc_base, SEQ_MEM_m, (addr-1)/cmds_per_reg, mask, value);
          value = 0;
        }
        cmd_shift = 0 * 0x8;
        break;

      case 1:
        cmd_shift = 1 * 0x8;
        break;

      case 2:
        cmd_shift = 2 * 0x8;
        break;

      case 3:
        cmd_shift = 3 * 0x8;
        break;
    }

    mask |= (0xFF << cmd_shift);
    value |= ((uint32)(cmds[i]) << cmd_shift);
    cmds_copied++;
  }

  HWIO_OUTXMI (pdc_base, SEQ_MEM_m, (addr-1)/cmds_per_reg, mask, value);

  return cmds_copied;
}


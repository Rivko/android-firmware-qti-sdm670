#ifndef __PDC_SEQ_HAL_H__
#define __PDC_SEQ_HAL_H__
/*==============================================================================
  FILE:         pdc_seq_hal.h
  
  OVERVIEW:     This file defines HAL interface for the PDC sequencer.
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/seq/hal/inc/pdc_seq_hal.h#2 $
$DateTime: 2017/10/12 22:35:09 $
==============================================================================*/
#include "comdef.h"
#include "pdc_seq_hal_bsp.h"

/*==============================================================================
                           TYPES AND DEFINITIONS
 =============================================================================*/
/* Invalid index */
#define PDC_SEQ_HAL_INVALID_INDEX (-1)

/* PDC Configuration Macros - Ideally these should be queriable from PDC 
 * config register but they are not. They are fixed currently but if
 * they changed move it to target build file */
#define PDC_SEQ_HAL_BR_ADDR_REG_COUNT 4
#define PDC_SEQ_HAL_DELAY_REG_COUNT   16

/*==============================================================================
                              GLOBAL FUNCTIONS
 =============================================================================*/
/**
 * pdc_seq_hal_enable
 *
 * @brief Enables/Disables PDC HW block (not just PDC sequencer. All PDC 
 *        components share a single enable bit).
 *
 * @param pdc_base: PDC instance's base address.
 * @param enable: Value to set PDC enabled or disabled. Expected use case
 *                is only one time enablement during init.
 *                (1 to enable, else 0)
 */
void pdc_seq_hal_enable(uint32 pdc_base, uint8 enable);

/**
 * pdc_seq_hal_clk_gate_enable
 *
 * @brief Enables/disables clock gating for the PDC HW block.
 *
 * @param pdc_base: PDC instance's base address.
 * @param enable: Value to set for PDC clock gating. Expected use case is
 *                only one time during init.
 *                (1 to enable, 0 to disable)
 */
void pdc_seq_hal_clk_gate_enable(uint32 pdc_base, uint8 enable);

/**
 * pdc_seq_hal_cfg_wakeup_time
 *
 * @brief Configures the wake up time for the PDC.
 *
 * @param rsc_base: Base of attached RSC block that will send the
 *                  wake up time via hidden tcs during execution.
 * @param wakeup_time: Time to wake up the PDC.
 */
void pdc_seq_hal_cfg_wakeup_time(uint32 rsc_base, uint64 wakeup_time);

/**
 * pdc_seq_hal_cfg_wakeup_time_ex
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
void pdc_seq_hal_cfg_wakeup_time_ex(uint32 pdc_base, uint64 wakeup_time);

/**
 * pdc_seq_hal_cfg_start_addr
 *
 * @brief Sets the start address from where the PDC sequencer will execute
 *        next time it is triggered.
 *
 * @param rsc_base: Base of attached RSC block that will send the
 *                  start address via hidden tcs during execution.
 * @param start_addr: Start address.
 */
void pdc_seq_hal_cfg_start_addr(uint32 rsc_base, uint16 start_addr);

/**
 * pdc_seq_hal_cfg_start_addr_ex
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
void pdc_seq_hal_cfg_start_addr_ex(uint32 pdc_base, uint16 start_addr);

/**
 * pdc_seq_hal_cfg_br_addr
 *
 * @brief Configure the branch address at a given index.
 *
 * @param pdc_base: PDC instance's base address.
 * @param br_index: Index of the branch for which we want to config address.
 * @param addr: Actual address to jump to for the given branch index.
 *
 * @return 0 if successful, non-zero (negative) otherwise
 */
int pdc_seq_hal_cfg_br_addr(uint32 pdc_base, uint16 br_index, uint8 addr);

/**
 * pdc_seq_hal_cfg_delay
 *
 * @brief Configures a delay value for the given delay indexed register.
 *
 * @param pdc_base: PDC instance's base address.
 * @param delay_index: Index of the register that needs to be configured.
 * @param delay: Actual delay value
 *
 * @return 0 if successful, non-zero (negative) otherwise
 */
int pdc_seq_hal_cfg_delay(uint32 pdc_base, uint16 delay_index, uint32 delay);

/**
 * pdc_seq_hal_copy_cmd_seq
 *
 * @brief Programs a sequencer memory with input sequence starting from a
 *        specified address
 *
 * @param pdc_base: PDC instance's base address.
 * @param cmds: Array of sequence commands
 * @param length: Length of command sequence.
 *
 * @return Number of commands copied to HW (in successful call it must 
 *         be equal to 'length' param) 
 */
uint16 pdc_seq_hal_copy_cmd_seq(uint32 pdc_base, uint32 *cmds, uint16 length);

#endif /* __PDC_SEQ_HAL_H__ */


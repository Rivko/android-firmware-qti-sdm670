#ifndef _RSC_HAL_H_
#define _RSC_HAL_H_
/*==============================================================================
  FILE:         rsc_seq_hal.h

  OVERVIEW:     This file provides Hardware layer (HAL) APIs for rsc driver. 
                rsc driver should use these APIs to perform actual IO on rsc
                HW block.

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/rsc/hal/inc/rsc_seq_hal.h#1 $
$DateTime: 2017/10/12 22:35:09 $
==============================================================================*/
#include "comdef.h"

/*========================================================================
 *                           HAL SPECIFIC MACROS
 *========================================================================*/
/* Macros to indicate invalid indices for different purposes */
#define RSC_HAL_INVALID_INDEX           (-1)
#define RSC_HAL_INVALID_MODE_INDEX      RSC_HAL_INVALID_INDEX
#define RSC_HAL_INVALID_SEQ_START_ADR   RSC_HAL_INVALID_INDEX

/*========================================================================
 *                             GLOBAL FUNCTIONS
 *========================================================================*/
/**
 * rsc_hal_get_num_seq_cmd_words
 *
 * @brief Read the number of sequence command words from RSC_CONFIG register.
 *
 * @param base: RSC instance's base address.
 *
 * @return Returns NUM_SEQ_CMD_WORDS field from RSC_CONFIG register.
 */
uint32 rsc_hal_get_num_seq_cmd_words(uint32 base);

/**
 * rsc_hal_config_br_addr
 *
 * @brief Configure branch adresses in CFG_BR_ADDR register.
 *
 * @param base: RSC instance's base address.
 * @param idx: Branch address index [0..3].
 * @param val: Branch address value.
 */
void rsc_hal_config_br_addr(uint32 base, uint32 idx, uint16 val);

/**
 * rsc_hal_enable_ts_unit
 *
 * @brief Enable/Disable timestamp unit in TIMESTAMP_UNITm_EN_DRVd register.
 *
 * @param base: RSC instance's base address.
 * @param unit_num: Unit number to be enable/disable.
 * @param enable: Boolean value either 1 or 0..
 */
void rsc_hal_enable_ts_unit(uint32 base, uint8 unit_num, boolean enable);

/**
 * rsc_hal_get_ts_unit_l
 *
 * @brief Read timestamp low data from TIMESTAMP_UNITm_TIMESTAMP_L register.
 *
 * @param base: RSC instance's base address.
 * @param unit_num: Unit number to be read.
 * 
 * @return Returns TS_DATA_L value from TIMESTAMP_UNITm_TIMESTAMP_L register. 
 */
uint32 rsc_hal_get_ts_unit_l(uint32 base, uint8 unit_num);

/**
 * rsc_hal_get_ts_unit_h
 *
 * @brief Read timestamp low data from TIMESTAMP_UNITm_TIMESTAMP_H register.
 *
 * @param base: RSC instance's base address.
 * @param unit_num: Unit number to be read.
 * 
 * @return Returns TS_DATA_H value from TIMESTAMP_UNITm_TIMESTAMP_H register. 
 */
uint32 rsc_hal_get_ts_unit_h(uint32 base, uint8 unit_num);

/**
 * rsc_hal_get_ts_overflow
 *
 * @brief Read timestamp overflow info from  TIMESTAMP_UNITm_OUTPUT register.
 *
 * @param base: RSC instance's base address.
 * @param unit_num: Unit number to be read.
 * 
 * @return Returns TS_OVERFLOW value from TIMESTAMP_UNITm_OUTPUT register. 
 */
uint32 rsc_hal_get_ts_overflow(uint32 base, uint8 unit_num);

/**
 * rsc_hal_get_ts_valid
 *
 * @brief Read timestamp valid info from  TIMESTAMP_UNITm_OUTPUT register.
 *
 * @param base: RSC instance's base address.
 * @param unit_num: Unit number to be read.
 * 
 * @return Returns TS_VALID value from TIMESTAMP_UNITm_OUTPUT register. 
 */
uint32 rsc_hal_get_ts_valid(uint32 base, uint8 unit_num);

/**
 * rsc_hal_set_seq_override_start_addr
 *
 * @brief Set sequence override start address in SEQ_OVERRIDE_START_ADDR.
 *
 * @param base: RSC instance's base address.
 */
void rsc_hal_set_seq_override_start_addr(uint32 base, uint8 addr);

/**
 * rsc_hal_set_seq_override_start_addr_valid
 *
 * @brief Set sequence override start address valid field in SEQ_OVERRIDE_START_ADDR.
 *
 * @param base: RSC instance's base address.
 */
void rsc_hal_set_seq_override_start_addr_valid(uint32 base, uint8 valid);

/**
 * rsc_hal_set_trigger_seq
 *
 * @brief Set this register(RSC_SEQ_OVERRIDE_TRIGGER_DRV0) to trigger the sequencer.
 *
 * @param base: RSC instance's base address.
 * @param val: write 0 then write 1 to trigger the sequencer to run
 */
void rsc_hal_set_trigger_seq(uint32 base, uint8 val);

/**
 * rsc_hal_set_trigger_start_address
 *
 * @brief Set start address in RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0 register.
 *
 * @param base: RSC instance's base address.
 * @param addr: Sequence address to trigger.
 */
void rsc_hal_set_trigger_start_address(uint32 base, uint16 addr);

/**
 * rsc_hal_seq_busy
 *
 * @brief Return the value to indicates sequencer is busy are not.
 *
 * @param base: RSC instance's base address.
 *
 * @return Returns the value whether sequencer busy are not.
 */
uint32 rsc_hal_seq_busy(uint32 base);

/**
 * rsc_hal_copy_cmd_seq
 *
 * @brief Copy simple command sequence to RSC sequencer memory.
 *
 * @param base: RSC instance's base address.
 * @param start_adr: Address (command index) to start copying at.
 * @param cmds: Array containing commands to be programmed.
 * @param size: Size of above array
 *
 * @return Number of commands copied to actual RSC HW (In successful call
          it must be equal to size).
 */
uint16 rsc_hal_copy_cmd_seq(uint32 base, uint32 *cmds, uint16 size);

/**
 * rsc_hal_set_br_event
 *
 * @brief Program the br event register for achieving branching functionality in
 *        RSC.
 *
 * @param base: base address.
 * @param val:  value to be programmed.
 */
void rsc_hal_set_br_event(uint32 base, uint16 val);

#endif /* _RSC_HAL_H_ */


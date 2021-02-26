#ifndef _RSC_HAL_H_
#define _RSC_HAL_H_

/*==============================================================================
  FILE:         rsc_seq_hal.h

  OVERVIEW:     This file provides Hardware layer (HAL) APIs for rsc driver. 
                rsc driver should use these APIs to perform actual IO on rsc
                HW block.

  DEPENDENCIES: This interface is specific to SAW/rsc version.
                
                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/

#include "HALcomdef.h"

/*========================================================================
 *                           HAL SPECIFIC MACROS
 *========================================================================*/

/**
 * @brief Macros to indicate invalid indices for different purposes.
 */
#define RSC_HAL_INVALID_INDEX  (-1)
#define RSC_HAL_INVALID_MODE_INDEX  RSC_HAL_INVALID_INDEX
#define RSC_HAL_INVALID_SEQ_START_ADR  RSC_HAL_INVALID_INDEX

/*========================================================================
 *                             GLOBAL FUNCTIONS
 *========================================================================*/

/**
 * <!-- rsc_hal_get_major_ver -->
 *
 * @brief Read the major version from RSC_ID register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return MAJOR_VER field from RSC_ID register.
 */
uint32 rsc_hal_get_major_ver(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_minor_ver -->
 *
 * @brief Read the minor version from RSC_ID register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return MINOR_VER field from RSC_ID register.
 */
uint32 rsc_hal_get_minor_ver(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_step_ver -->
 *
 * @brief Read the step version from RSC_ID register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return STEP_VER field from RSC_ID register.
 */
uint32 rsc_hal_get_step_ver(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_solver_cfg -->
 *
 * @brief Read the solver configuration from SOLVER_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns SOLVER_CONFIG register value.
 */
uint32 rsc_hal_get_solver_cfg(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_num_drvs -->
 *
 * @brief Read the number of DRVs from RSC_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns NUM_DRV field from RSC_CONFIG register.
 */
uint32 rsc_hal_get_num_drvs(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_num_seq_cmd_words -->
 *
 * @brief Read the number of sequence command words from RSC_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns NUM_SEQ_CMD_WORDS field from RSC_CONFIG register.
 */
uint32 rsc_hal_get_num_seq_cmd_words(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_num_ts_events -->
 *
 * @brief Read the number of timestampable events from RSC_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns NUM_TS_EVENTS field from RSC_CONFIG register.
 */
uint32 rsc_hal_get_num_ts_events(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_delay_cntr_bitwidth -->
 *
 * @brief Read the size of delay counter from RSC_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns DELAY_CNTR_BITWIDTH field from RSC_CONFIG register.
 */
uint32 rsc_hal_get_delay_cntr_bitwidth(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_parentchild_cfg -->
 *
 * @brief Read the rent/child configuration from PARENTCHILD_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns the PARENTCHILD_CONFIG register value.
 */
uint32 rsc_hal_get_parentchild_cfg(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_status0 -->
 *
 * @brief Read the RSC_STATUS0 register value.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns the RSC_STATUS0 register value.
 */
uint32 rsc_hal_get_status0(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_status1 -->
 *
 * @brief Read the RSC_STATUS1 register value.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns the RSC_STATUS1 register value.
 */
uint32 rsc_hal_get_status1(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_status2 -->
 *
 * @brief Read the RSC_STATUS2 register value.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns the RSC_STATUS2 register value.
 */
uint32 rsc_hal_get_status2(uint8* rsc_base);

/**
 * <!-- rsc_hal_config_br_addr -->
 *
 * @brief Configure branch adresses in CFG_BR_ADDR register.
 *
 * @param rsc_base: RSC instance's base address.
 * @param idx: Branch address index [0..3].
 * @param val: Branch address value.
 */
void rsc_hal_config_br_addr(uint8* rsc_base, uint32 idx, uint16 val);

/**
 * <!-- rsc_hal_set_ts_unit_owner -->
 *
 * @brief Configure time stamp unit owner TIMESTAMP_UNIT_OWNER`.
 *
 * @param rsc_base: RSC instance's base address.
 * @param drv_owner: Owner DRV number.
 */
void rsc_hal_set_ts_unit_owner(uint8* rsc_base, uint8 drv_owner);

/**
 * <!-- rsc_hal_enable_ts_unit -->
 *
 * @brief Enable/Disable timestamp unit in TIMESTAMP_UNITm_EN_DRVd register.
 *
 * @param rsc_base: RSC instance's base address.
 * @param unit_num: Unit number to be enable/disable.
 * @param enable: Boolean value either 1 or 0..
 */
void rsc_hal_enable_ts_unit(uint8* rsc_base, uint8 unit_num, boolean enable);

/**
 * <!-- rsc_hal_get_ts_unit_l -->
 *
 * @brief Read timestamp low data from TIMESTAMP_UNITm_TIMESTAMP_L register.
 *
 * @param rsc_base: RSC instance's base address.
 * @param unit_num: Unit number to be read.
 * 
 * @return Returns TS_DATA_L value from TIMESTAMP_UNITm_TIMESTAMP_L register. 
 */
uint32 rsc_hal_get_ts_unit_l(uint8* rsc_base, uint8 unit_num);

/**
 * <!-- rsc_hal_get_ts_unit_h -->
 *
 * @brief Read timestamp low data from TIMESTAMP_UNITm_TIMESTAMP_H register.
 *
 * @param rsc_base: RSC instance's base address.
 * @param unit_num: Unit number to be read.
 * 
 * @return Returns TS_DATA_H value from TIMESTAMP_UNITm_TIMESTAMP_H register. 
 */
uint32 rsc_hal_get_ts_unit_h(uint8* rsc_base, uint8 unit_num);

/**
 * <!-- rsc_hal_get_ts_overflow -->
 *
 * @brief Read timestamp overflow info from  TIMESTAMP_UNITm_OUTPUT register.
 *
 * @param rsc_base: RSC instance's base address.
 * @param unit_num: Unit number to be read.
 * 
 * @return Returns TS_OVERFLOW value from TIMESTAMP_UNITm_OUTPUT register. 
 */
uint32 rsc_hal_get_ts_overflow(uint8* rsc_base, uint8 unit_num);

/**
 * <!-- rsc_hal_get_ts_valid -->
 *
 * @brief Read timestamp valid info from  TIMESTAMP_UNITm_OUTPUT register.
 *
 * @param rsc_base: RSC instance's base address.
 * @param unit_num: Unit number to be read.
 * 
 * @return Returns TS_VALID value from TIMESTAMP_UNITm_OUTPUT register. 
 */
uint32 rsc_hal_get_ts_valid(uint8* rsc_base, uint8 unit_num);

/**
 * <!-- rsc_hal_get_seq_override_start_addr_valid -->
 *
 * @brief Read sequence override start address valid info.
 *
 * @param rsc_base: RSC instance's base address.
 * 
 * @return Returns VALID field from SEQ_OVERRIDE_START_ADDR register.
 */
uint32 rsc_hal_get_seq_override_start_addr_valid(uint8* rsc_base);

/**
 * <!-- rsc_hal_set_seq_override_start_addr_valid -->
 *
 * @brief Set sequence override start address valid field in SEQ_OVERRIDE_START_ADDR.
 *
 * @param rsc_base: RSC instance's base address.
 */
void rsc_hal_set_seq_override_start_addr_valid(uint8* rsc_base, uint8 valid);

/**
 * <!-- rsc_hal_set_seq_override_start_addr -->
 *
 * @brief Set sequence override start address in SEQ_OVERRIDE_START_ADDR.
 *
 * @param rsc_base: RSC instance's base address.
 */
void rsc_hal_set_seq_override_start_addr(uint8* rsc_base, uint8 addr);

/**
 * <!-- rsc_hal_set_trigger_seq -->
 *
 * @brief Set this register(RSC_SEQ_OVERRIDE_TRIGGER_DRV0) to trigger the sequencer.
 *
 * @param rsc_base: RSC instance's base address.
 * @param val: write 0 then write 1 to trigger the sequencer to run
 */
void rsc_hal_set_trigger_seq(uint8* rsc_base, uint8 val);

/**
 * <!-- rsc_hal_set_trigger_start_address -->
 *
 * @brief Set start address in RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0 register.
 *
 * @param rsc_base: RSC instance's base address.
 * @param addr: Sequence address to trigger.
 */
void rsc_hal_set_trigger_start_address(uint8 * rsc_base, uint16 addr);

/**
 * <!-- rsc_hal_seq_busy -->
 *
 * @brief Return the value to indicates sequencer is busy are not.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns the value whether sequencer busy are not.
 */
uint32 rsc_hal_seq_busy(uint8* rsc_base);

/**
 * <!-- rsc_hal_get_seq_program_counter -->
 *
 * @brief Return the last command executed by RSC sequencer.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns program counter value from SEQ_PROGRAM_COUNTER_DRV0 register.
 */
uint16 rsc_hal_get_seq_program_counter(uint8* rsc_base);

/**
 * <!-- rsc_hal_copy_simple_cmd_seq -->
 *
 * @brief Copy simple command (8bit) sequence to RSC sequencer memory.
 *
 * @param rsc_base: RSC instance's base address.
 * @param start_adr: Address (command index) to start copying at.
 * @param cmds: Array containing commands to be programmed.
 * @param size: Size of above array
 *
 * @return Number of commands copied to actual RSC HW (In successful call
          it must be equal to size).
 */
uint16 rsc_hal_copy_simple_cmd_seq(uint8 *rsc_base, uint16 start_adr, uint16* cmds, uint16 size);

/**
 * <!-- rsc_hal_copy_extended_cmd_seq -->
 *
 * @brief Copy extended command (8bit) sequence to RSC sequencer memory.
 *
 * @param rsc_base: RSC instance's base address.
 * @param start_adr: Address (command index) to start copying at.
 * @param cmds: Array containing commands to be programmed.
 * @param size: Size of above array
 *
 * @return Number of commands copied to actual RSC HW (In successful call
          it must be equal to size).
 */
uint16 rsc_hal_copy_extended_cmd_seq(uint8 *rsc_base, uint16 start_adr, uint16* cmds, uint16 size);
#endif 

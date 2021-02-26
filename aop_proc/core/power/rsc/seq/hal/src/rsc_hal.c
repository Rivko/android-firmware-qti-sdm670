/*==============================================================================
  FILE:         rsc_hal.c

  OVERVIEW:     This file implements HAL APIs exposed to rsc driver and performs
                actual HW register read and write.

  DEPENDENCIES: The implementation can be SAW/rsc version specific.
                
                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/

#include "rsc_seq_hal.h"
#include "rsc_hal_hwio.h"
#include "HALhwio.h"

/*===========================================================================
 *                            INTERNAL VARIABLES
 *==========================================================================*/

/**
 * @brief Volatile variable which will be used to read back rsc register 
 *        being written to. This is to ensure that write value actually got 
 *        propagated to HW.
 */
volatile uint32 rsc_hal_read_back_val;

/*==========================================================================
 *                             INTERNAL MACROS
 *=========================================================================*/

/* Simple Sequence Macros */
#define RSC_HAL_SIMPLE_SEQ_MEM_CMD3_BMSK  0xff000000
#define RSC_HAL_SIMPLE_SEQ_MEM_CMD3_SHFT  0x18
#define RSC_HAL_SIMPLE_SEQ_MEM_CMD2_BMSK  0xff0000
#define RSC_HAL_SIMPLE_SEQ_MEM_CMD2_SHFT  0x10
#define RSC_HAL_SIMPLE_SEQ_MEM_CMD1_BMSK  0xff00
#define RSC_HAL_SIMPLE_SEQ_MEM_CMD1_SHFT  0x8
#define RSC_HAL_SIMPLE_SEQ_MEM_CMD0_BMSK  0xff
#define RSC_HAL_SIMPLE_SEQ_MEM_CMD0_SHFT  0x0

#define RSC_HAL_SIMPLE_CMD_SEQ_REG_SIZE  4
#define RSC_HAL_SIMPLE_CMD_SIZE  1
#define RSC_HAL_SIMPLE_CMDS_PER_REG  (RSC_HAL_SIMPLE_CMD_SEQ_REG_SIZE/RSC_HAL_SIMPLE_CMD_SIZE)

/* Extended Sequence Macros */
#define RSC_HAL_EXTENTED_SEQ_MEM_CMD1_BMSK  0xffff0000
#define RSC_HAL_EXTENDED_SEQ_MEM_CMD1_SHFT  0x10
#define RSC_HAL_EXTENDED_SEQ_MEM_CMD0_BMSK  0xffff
#define RSC_HAL_EXTENDED_SEQ_MEM_CMD0_SHFT  0x0

#define RSC_HAL_EXTENDED_CMD_SEQ_REG_SIZE  4
#define RSC_HAL_EXTENDED_CMD_SIZE  2
#define RSC_HAL_EXTENDED_CMDS_PER_REG  (RSC_HAL_EXTENDED_CMD_SEQ_REG_SIZE/RSC_HAL_EXTENDED_CMD_SIZE)

/**
 * @brief Macro to readback an rsc register.
 */
#define RSC_HAL_READBACK_REG(base, reg) \
  (rsc_hal_read_back_val = RSC_HWIO_INX(base, reg))

/*===========================================================================
 *                            EXTERNAL FUNCTIONS
 *==========================================================================*/
/**
 * <!-- rsc_hal_get_major_ver -->
 *
 * @brief Read the major version from RSC_ID register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return MAJOR_VER field from RSC_ID register.
 */
uint32 rsc_hal_get_major_ver(uint8* rsc_base)
{
  return HWIO_INXFI(rsc_base, RSC_ID_DRVd, 0, MAJOR_VER);
}

/**
 * <!-- rsc_hal_get_minor_ver -->
 *
 * @brief Read the minor version from RSC_ID register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return MINOR_VER field from RSC_ID register.
 */
uint32 rsc_hal_get_minor_ver(uint8* rsc_base)
{
  return HWIO_INXFI(rsc_base, RSC_ID_DRVd, 0, MINOR_VER);
}

/**
 * <!-- rsc_hal_get_step_ver -->
 *
 * @brief Read the step version from RSC_ID register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return STEP_VER field from RSC_ID register.
 */
uint32 rsc_hal_get_step_ver(uint8* rsc_base)
{
  return HWIO_INXFI(rsc_base, RSC_ID_DRVd, 0, STEP_VER);
}

/**
 * <!-- rsc_hal_get_solver_cfg -->
 *
 * @brief Read the solver configuration from SOLVER_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns SOLVER_CONFIG register value.
 */
uint32 rsc_hal_get_solver_cfg(uint8* rsc_base)
{
  return HWIO_INXI(rsc_base, RSC_PARAM_SOLVER_CONFIG_DRVd, 0);
}

/**
 * <!-- rsc_hal_get_num_drvs -->
 *
 * @brief Read the number of DRVs from RSC_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns NUM_DRV field from RSC_CONFIG register.
 */
uint32 rsc_hal_get_num_drvs(uint8* rsc_base)
{
  return HWIO_INXFI(rsc_base, RSC_PARAM_RSC_CONFIG_DRVd, 0, NUM_DRV);
}

/**
 * <!-- rsc_hal_get_num_seq_cmd_words -->
 *
 * @brief Read the number of sequence command words from RSC_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns NUM_SEQ_CMD_WORDS field from RSC_CONFIG register.
 */
uint32 rsc_hal_get_num_seq_cmd_words(uint8* rsc_base)
{
  return HWIO_INXFI(rsc_base, RSC_PARAM_RSC_CONFIG_DRVd, 0, NUM_SEQ_CMD_WORDS);
}

/**
 * <!-- rsc_hal_get_num_ts_events -->
 *
 * @brief Read the number of timestampable events from RSC_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns NUM_TS_EVENTS field from RSC_CONFIG register.
 */
uint32 rsc_hal_get_num_ts_events(uint8* rsc_base)
{
  return HWIO_INXFI(rsc_base, RSC_PARAM_RSC_CONFIG_DRVd, 0, NUM_TS_EVENTS);
}

/**
 * <!-- rsc_hal_get_delay_cntr_bitwidth -->
 *
 * @brief Read the size of delay counter from RSC_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns DELAY_CNTR_BITWIDTH field from RSC_CONFIG register.
 */
uint32 rsc_hal_get_delay_cntr_bitwidth(uint8* rsc_base)
{
  return HWIO_INXFI(rsc_base, RSC_PARAM_RSC_CONFIG_DRVd, 0, DELAY_CNTR_BITWIDTH);
}

/**
 * <!-- rsc_hal_get_parentchild_cfg -->
 *
 * @brief Read the rent/child configuration from PARENTCHILD_CONFIG register.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns the PARENTCHILD_CONFIG register value.
 */
uint32 rsc_hal_get_parentchild_cfg(uint8* rsc_base)
{
  return HWIO_INXI(rsc_base, RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd, 0);
}

/**
 * <!-- rsc_hal_get_status0 -->
 *
 * @brief Read the RSC_STATUS0 register value.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns the RSC_STATUS0 register value.
 */
uint32 rsc_hal_get_status0(uint8* rsc_base)
{
  return HWIO_INXI(rsc_base, RSC_STATUS0_DRVd, 0);
}

/**
 * <!-- rsc_hal_get_status1 -->
 *
 * @brief Read the RSC_STATUS1 register value.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns the RSC_STATUS1 register value.
 */
uint32 rsc_hal_get_status1(uint8* rsc_base)
{
  return HWIO_INXI(rsc_base, RSC_STATUS1_DRVd, 0);
}

/**
 * <!-- rsc_hal_get_status2 -->
 *
 * @brief Read the RSC_STATUS2 register value.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns the RSC_STATUS2 register value.
 */
uint32 rsc_hal_get_status2(uint8* rsc_base)
{
  return HWIO_INXI(rsc_base, RSC_STATUS2_DRVd, 0);
}

/**
 * <!-- rsc_hal_set_ts_unit_owner -->
 *
 * @brief Configure time stamp unit owner TIMESTAMP_UNIT_OWNER`.
 *
 * @param rsc_base: RSC instance's base address.
 * @param drv_owner: Owner DRV number.
 */
void rsc_hal_set_ts_unit_owner(uint8* rsc_base, uint8 drv_owner)
{
  HWIO_OUTX(rsc_base, RSC_TIMESTAMP_UNIT_OWNER_DRV0, drv_owner);
}

/**
 * <!-- rsc_hal_enable_ts_unit -->
 *
 * @brief Enable/Disable timestamp unit in TIMESTAMP_UNITm_EN_DRVd register.
 *
 * @param rsc_base: RSC instance's base address.
 * @param unit_num: Unit number to be enable/disable.
 * @param enable: Boolean value either 1 or 0..
 */
void rsc_hal_enable_ts_unit(uint8* rsc_base, uint8 unit_num, boolean enable)
{
  HWIO_OUTXFI2(rsc_base, RSC_TIMESTAMP_UNITm_EN_DRVd, 0, unit_num, ENABLE, enable);
}

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
uint32 rsc_hal_get_ts_unit_l(uint8* rsc_base, uint8 unit_num)
{
  return HWIO_INXFI2(rsc_base, RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd, 0, unit_num, TS_DATA_L);
}  

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
uint32 rsc_hal_get_ts_unit_h(uint8* rsc_base, uint8 unit_num)
{
  return HWIO_INXFI2(rsc_base, RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd, 0, unit_num, TS_DATA_H);
}

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
uint32 rsc_hal_get_ts_overflow(uint8* rsc_base, uint8 unit_num)
{
  return HWIO_INXFI2(rsc_base, RSC_TIMESTAMP_UNITm_OUTPUT_DRVd, 0, unit_num, TS_OVERFLOW);
}

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
uint32 rsc_hal_get_ts_valid(uint8* rsc_base, uint8 unit_num)
{
  return HWIO_INXFI2(rsc_base, RSC_TIMESTAMP_UNITm_OUTPUT_DRVd, 0, unit_num, TS_VALID);
}

/**
 * <!-- rsc_hal_get_seq_override_start_addr_valid -->
 *
 * @brief Read sequence override start address valid info.
 *
 * @param rsc_base: RSC instance's base address.
 * 
 * @return Returns VALID field from SEQ_OVERRIDE_START_ADDR register.
 */
uint32 rsc_hal_get_seq_override_start_addr_valid(uint8* rsc_base)
{
  return HWIO_INXF(rsc_base, RSC_SEQ_OVERRIDE_START_ADDR_DRV0, VALID); 
}

/**
 * <!-- rsc_hal_set_seq_override_start_addr_valid -->
 *
 * @brief Set sequence override start address valid field in SEQ_OVERRIDE_START_ADDR.
 *
 * @param rsc_base: RSC instance's base address.
 */
void rsc_hal_set_seq_override_start_addr_valid(uint8* rsc_base, uint8 valid)
{
  HWIO_OUTXF(rsc_base, RSC_SEQ_OVERRIDE_START_ADDR_DRV0, VALID, valid); 
}

/**
 * <!-- rsc_hal_set_seq_override_start_addr -->
 *
 * @brief Set sequence override start address in SEQ_OVERRIDE_START_ADDR.
 *
 * @param rsc_base: RSC instance's base address.
 */
void rsc_hal_set_seq_override_start_addr(uint8* rsc_base, uint8 addr)
{
  HWIO_OUTXF(rsc_base, RSC_SEQ_OVERRIDE_START_ADDR_DRV0, ADDR, addr); 
}

/**
 * <!-- rsc_hal_set_trigger_seq -->
 *
 * @brief Set this register(RSC_SEQ_OVERRIDE_TRIGGER_DRV0) to trigger the sequencer.
 *
 * @param rsc_base: RSC instance's base address.
 * @param val: write 0 then write 1 to trigger the sequencer to run
 */
void rsc_hal_set_trigger_seq(uint8* rsc_base, uint8 val)
{
  HWIO_OUTX(rsc_base, RSC_SEQ_OVERRIDE_TRIGGER_DRV0, val);
}

/**
 * <!-- rsc_hal_set_trigger_start_address -->
 *
 * @brief Set start address in RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0 register.
 *
 * @param rsc_base: RSC instance's base address.
 * @param addr: Sequence address to trigger.
 */
void rsc_hal_set_trigger_start_address(uint8 * rsc_base, uint16 addr)
{
  HWIO_OUTX(rsc_base, RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0, addr);
}

/**
 * <!-- rsc_hal_seq_busy -->
 *
 * @brief Return the value to indicates sequencer is busy are not.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns the value whether sequencer busy are not.
 */
uint32 rsc_hal_seq_busy(uint8* rsc_base)
{
  return HWIO_INX(rsc_base, RSC_SEQ_BUSY_DRV0);
}

/**
 * <!-- rsc_hal_get_seq_program_counter -->
 *
 * @brief Return the last command executed by RSC sequencer.
 *
 * @param rsc_base: RSC instance's base address.
 *
 * @return Returns program counter value from SEQ_PROGRAM_COUNTER_DRV0 register.
 */
uint16 rsc_hal_get_seq_program_counter(uint8* rsc_base)
{
  return HWIO_INX(rsc_base, RSC_SEQ_PROGRAM_COUNTER_DRV0);
}

/**
 * <!-- rsc_hal_config_br_addr -->
 *
 * @brief Configure branch adresses in CFG_BR_ADDR register.
 *
 * @param rsc_base: RSC instance's base address.
 * @param idx: Branch address index [0..3].
 * @param val: Branch address value.
 */
void rsc_hal_config_br_addr(uint8* rsc_base, uint32 idx, uint16 val)
{
  HWIO_OUTXI(rsc_base, RSC_SEQ_CFG_BR_ADDR_i_DRV0, idx, val);
}  

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
uint16 rsc_hal_copy_simple_cmd_seq(uint8 *rsc_base, uint16 start_adr, uint16* cmds, uint16 size)
{
  uint16 max_cmds     = 0;
  uint16 cmds_copied  = 0;
  uint32 mask         = 0;
  uint32 val          = 0;
  uint32 cmd_mask     = 0;
  uint32 cmd_shift    = 0;
  uint16 addr         = start_adr;
  uint32 i;

  max_cmds = rsc_hal_get_num_seq_cmd_words(rsc_base) * RSC_HAL_SIMPLE_CMDS_PER_REG;

  if((start_adr >= max_cmds) || ((start_adr + size) > max_cmds))
  {
    return cmds_copied;
  }

  for(i = 0; i < size; i++, addr++)
  {
    switch(addr % RSC_HAL_SIMPLE_CMDS_PER_REG)
    {
      case 0:
        if(0 != cmds_copied)
        {
          HWIO_OUTXMI(rsc_base, SEQ_MEM_m_DRV0,
                            ((addr-1)/RSC_HAL_SIMPLE_CMDS_PER_REG), mask, val);
          mask = val = 0;
        }
        cmd_mask = RSC_HAL_SIMPLE_SEQ_MEM_CMD0_BMSK;
        cmd_shift = RSC_HAL_SIMPLE_SEQ_MEM_CMD0_SHFT;
        break;
      case 1:
        cmd_mask = RSC_HAL_SIMPLE_SEQ_MEM_CMD1_BMSK;
        cmd_shift = RSC_HAL_SIMPLE_SEQ_MEM_CMD1_SHFT;
        break;
      case 2:
        cmd_mask = RSC_HAL_SIMPLE_SEQ_MEM_CMD2_BMSK;
        cmd_shift = RSC_HAL_SIMPLE_SEQ_MEM_CMD2_SHFT;
        break;
      case 3:
        cmd_mask = RSC_HAL_SIMPLE_SEQ_MEM_CMD3_BMSK;
        cmd_shift = RSC_HAL_SIMPLE_SEQ_MEM_CMD3_SHFT;
        break;
    }
    mask |= cmd_mask;
    val |=(((uint32)(cmds[i])) << cmd_shift);
    cmds_copied++;
  }
 
  HWIO_OUTXMI(rsc_base, SEQ_MEM_m_DRV0,
             ((addr-1)/RSC_HAL_SIMPLE_CMDS_PER_REG), mask, val);

  return cmds_copied;
}

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
uint16 rsc_hal_copy_extended_cmd_seq(uint8 *rsc_base, uint16 start_adr, uint16* cmds, uint16 size)
{
  uint16 max_cmds     = 0;
  uint16 cmds_copied  = 0;
  uint32 mask         = 0;
  uint32 val          = 0;
  uint32 cmd_mask     = 0;
  uint32 cmd_shift    = 0;
  uint16 addr         = start_adr;
  uint32 i;

  max_cmds = rsc_hal_get_num_seq_cmd_words(rsc_base) * RSC_HAL_EXTENDED_CMDS_PER_REG;

  if((start_adr >= max_cmds) || ((start_adr + size) > max_cmds))
  {
    return cmds_copied;
  }

  for(i = 0; i < size; i++, addr++)
  {
    switch(addr % RSC_HAL_EXTENDED_CMDS_PER_REG)
    {
      case 0:
        if(0 != cmds_copied)
        {
          HWIO_OUTXMI(rsc_base, SEQ_MEM_m_DRV0,
                            ((addr-1)/RSC_HAL_EXTENDED_CMDS_PER_REG), mask, val);
          mask = val = 0;
        }
        cmd_mask = RSC_HAL_EXTENDED_SEQ_MEM_CMD0_BMSK;
        cmd_shift = RSC_HAL_EXTENDED_SEQ_MEM_CMD0_SHFT;
        break;
      case 1:
        cmd_mask = RSC_HAL_EXTENTED_SEQ_MEM_CMD1_BMSK;
        cmd_shift = RSC_HAL_EXTENDED_SEQ_MEM_CMD1_SHFT;
        break;
    }
    mask |= cmd_mask;
    val |=(((uint32)(cmds[i])) << cmd_shift);
    cmds_copied++;
  }

  HWIO_OUTXMI(rsc_base, SEQ_MEM_m_DRV0,
             ((addr-1)/RSC_HAL_EXTENDED_CMDS_PER_REG), mask, val);

  return cmds_copied;
}


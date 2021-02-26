#ifndef __VRM_HAL_H__
#define __VRM_HAL_H__

/*! \file vrm_hal.h
*
*  \brief Interface header file for VRM HAL layer.
*  \n
*  &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/hal/vrm_hal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_types.h"
#include "vrm_utils.h"
#include "vrm_hwio.h"
#include "HALhwio.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_HWIO_FIELD_VALUE(value, field)  (((value) & field##_BMSK) >> field##_SHFT)

/**
 * @brief struct containing VRM sequencer related data. 
 */
typedef struct
{
   vrm_rsrc_category rsrc_category;
   uint32 rsrc_id;
   uint32 setting;
   uint32 settling_time_usec;
}vrm_hal_seq_data_type;


//******************************************************************************
// Public API Functions
//******************************************************************************

//GENERIC HAL INITIALIZATION AND CFG HAL APIS
vrm_result vrm_hal_init(vrm_info* vrm_ptr);
vrm_result vrm_hal_cfg_vrm_enable(vrm_info* vrm_ptr, uint32 slave_id);
vrm_result vrm_hal_cfg_unblock_cmd_syn(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting);
vrm_result vrm_hal_settling_wdog_error_enable(vrm_info* vrm_ptr, uint32 seq_id, boolean enable);
vrm_result vrm_hal_seq_data_read(vrm_info* vrm_ptr, uint32 seq_id, vrm_hal_seq_data_type* seq_data);

//INTERRUPT HAL APIS
vrm_result vrm_hal_interrupt_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data);

vrm_result vrm_hal_interrupt_dep_irq_enable(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, boolean enable);
vrm_result vrm_hal_interrupt_change_irq_enable(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, boolean enable);
vrm_result vrm_hal_interrupt_change_irq_clear(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting);
vrm_result vrm_hal_interrupt_settled_irq_enable(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, boolean enable);
vrm_result vrm_hal_interrupt_settled_irq_clear(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting);

vrm_result vrm_hal_interrupt_dep_irq_status_read(vrm_info* vrm_ptr, uint32 setting, uint64* status);
vrm_result vrm_hal_interrupt_dep_irq_enable_read(vrm_info* vrm_ptr, uint32 setting, uint64* status);
vrm_result vrm_hal_interrupt_change_irq_status_read(vrm_info* vrm_ptr, uint32 setting, uint64* status);
vrm_result vrm_hal_interrupt_change_irq_enable_read(vrm_info* vrm_ptr, uint32 setting, uint64* status);
vrm_result vrm_hal_interrupt_settled_irq_status_read(vrm_info* vrm_ptr, uint32 setting, uint64* status);
vrm_result vrm_hal_interrupt_settled_irq_enable_read(vrm_info* vrm_ptr, uint32 setting, uint64* status);

vrm_result vrm_hal_interrupt_settling_wdog_irq_status_read(vrm_info* vrm_ptr, uint64* status);
vrm_result vrm_hal_interrupt_settling_wdog_irq_enable_read(vrm_info* vrm_ptr, uint64* status);
vrm_result vrm_hal_interrupt_settling_wdog_irq_clear(vrm_info* vrm_ptr, uint32 seq_id);

vrm_result vrm_hal_interrupt_max_volt_irq_status_read(vrm_info* vrm_ptr, uint64* status);
vrm_result vrm_hal_interrupt_max_volt_irq_enable_read(vrm_info* vrm_ptr, uint64* status);
vrm_result vrm_hal_interrupt_ctt_overflow_irq_status_read(vrm_info* vrm_ptr, uint32* status);
vrm_result vrm_hal_interrupt_pmic_arb_failure_irq_status_read(vrm_info* vrm_ptr, uint32* status);

//RESCFG HAL APIS
vrm_result vrm_hal_rescfg_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data);
vrm_result vrm_hal_rescfg_ars_read(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32* data);
vrm_result vrm_hal_rescfg_wrs_read(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32* data);
vrm_result vrm_hal_rescfg_crs_read(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32* data);
vrm_result vrm_hal_rescfg_vreg_dep_enable(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, boolean enable);
vrm_result vrm_hal_rescfg_base_addr_write(vrm_info* vrm_ptr, uint32 rsrc_id, uint8 vrm_arb_id);
vrm_result vrm_hal_rescfg_settling_block_enable(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, boolean enable);

//TYPECFG HAL APIS
vrm_result vrm_hal_typecfg_init(vrm_info* vrm_ptr, uint32 type_id, vrm_rsrc_type_data* type_data);
vrm_result vrm_hal_typecfg_vreg_clr_offset_data_write(vrm_info* vrm_ptr, uint32 type_id, uint8 addr_offset, uint8 data);

//VOTETABLE HAL APIS
vrm_result vrm_hal_votetable_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data);
vrm_result vrm_hal_votetable_vreg_get_addr_offset(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32 vrm_slave_id, uint32* addr_offset);
vrm_result vrm_hal_votetable_xob_get_addr_offset(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32 vrm_slave_id, uint32* addr_offset);
vrm_result vrm_hal_votetable_vreg_mode_update(vrm_info* vrm_ptr, uint32 drv_id, uint32 rsrc_id, uint32 mode);


#endif


/*========================================================================
Copyright (c) 2016 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/src/AFEHalTdmFuncDefs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/20/16   sivanaga   Created file.
========================================================================== */
/**
@file AFEHalTdmFuncDefs.h

@brief This file contains afe hardware abstraction layer.
 */

#ifndef __AFE_HAL_TDM_FUNC_DEFS_H__
#define __AFE_HAL_TDM_FUNC_DEFS_H__

#include "AFEHalTdm.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

  /**
   * Initialize register to default values and return the version number of the
   * audio HAL. It also sets the base virtual address of the audio hardware
   * registers.
   *
   */


  typedef void (*hal_tdm_init_func_t)(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t tdm_offset);
  typedef void (*hal_tdm_reset_func_t)(uint32_t tdm_index, uint32_t dir);
  typedef void (*hal_tdm_reset_release_func_t)(uint32_t tdm_index, uint32_t dir);
  typedef void (*hal_tdm_config_func_t)(uint32_t tdm_index, uint32_t dir, tdm_config_t *config_ptr);
  typedef void (*hal_tdm_enable_func_t)(uint32_t tdm_index, uint32_t dir);
  typedef void (*hal_tdm_disable_func_t)(uint32_t tdm_index, uint32_t dir);
  typedef uint32_t (*hal_tdm_get_intf_func_t)(uint32_t tdm_index, uint32_t dir);



  typedef struct
  {
    hal_tdm_init_func_t                   hal_tdm_init_fptr;
    hal_tdm_reset_func_t                  hal_tdm_reset_fptr;
    hal_tdm_reset_release_func_t          hal_tdm_reset_release_fptr;
    hal_tdm_config_func_t                 hal_tdm_config_fptr;
    hal_tdm_enable_func_t                 hal_tdm_enable_fptr;
    hal_tdm_disable_func_t                hal_tdm_disable_fptr;
    hal_tdm_get_intf_func_t               hal_tdm_get_intf_fptr;
  } afe_tdm_hal_func_def_t;


  void hal_tdm_get_func_table_v1(afe_tdm_hal_func_def_t *func_table);
  void hal_tdm_get_func_table_v2(afe_tdm_hal_func_def_t *func_table);
  void hal_tdm_get_func_table_v3(afe_tdm_hal_func_def_t *func_table);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__AFE_HAL_TDM_FUNC_DEFS_H__

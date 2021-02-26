#ifndef __HAL_QDSS_TPDM_H__
#define __HAL_QDSS_TPDM_H__

/*
  ===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tpdm/hal/inc/hal_qdss_tpdm.h#1 $
  ===========================================================================
*/

#include "HALcomdef.h"


void HAL_qdss_tpdm_SetBaseAddress(uint32 virtAddr);
void HAL_qdss_tpdm_cmb_set_enable_disable(uint32 enable_bit);
void HAL_qdss_tpdm_dsb_set_enable_disable(uint32 enable_bit);
void HAL_qdss_tpdm_bc_set_enable_disable(uint32 enable_bit);
void HAL_qdss_tpdm_tc_set_enable_disable(uint32 enable_bit);
void HAL_qdss_tpdm_impldef_set_enable_disable(uint32 enable_bit);
void HAL_qdss_tpdm_cmb_set_mode(uint32 bit);
void HAL_qdss_tpdm_cmb_set_flow_control(uint32 bit);
void HAL_qdss_tpdm_cmb_set_patt_match(uint32 bit);
void HAL_qdss_tpdm_cmb_set_ext_trigger_on_off(uint32 bit);
void HAL_qdss_tpdm_cmb_set_ts_patt_val_lsw(uint32 val);
void HAL_qdss_tpdm_cmb_set_ts_patt_val_msw(uint32 val);
void HAL_qdss_tpdm_cmb_set_ts_patt_mask_lsw(uint32 mask);
void HAL_qdss_tpdm_cmb_set_ts_patt_mask_msw(uint32 mask);
void HAL_qdss_tpdm_dsb_set_mode(uint32 mode);
void HAL_qdss_tpdm_dsb_set_ext_trigger_on_off(uint32 on_off_bit);
void HAL_qdss_tpdm_dsb_set_patt_type(uint8 type);
void HAL_qdss_tpdm_dsb_set_patt_match(uint32 on_off_bit);
void HAL_qdss_tpdm_dsb_set_edcmr(uint32 index,uint32 val);
uint32 HAL_qdss_tpdm_SetGPR(uint32 GPRIndex,uint32 val);
void HAL_qdss_tpdm_dsb_set_tpmr(uint32 index,uint32 val);


uint32 HAL_qdss_tpdm_cmb_get_enable_disable(void);
uint32 HAL_qdss_tpdm_dsb_get_enable_disable(void);
uint32 HAL_qdss_tpdm_bc_get_enable_disable(void);
uint32 HAL_qdss_tpdm_tc_get_enable_disable(void);
uint32 HAL_qdss_tpdm_cmb_get_mode(void);
uint32 HAL_qdss_tpdm_cmb_get_flow_control(void);
uint32 HAL_qdss_tpdm_cmb_get_patt_match(void);
uint32 HAL_qdss_tpdm_cmb_get_ext_trigger_on_off(void);
uint32 HAL_qdss_tpdm_cmb_get_ts_patt_val_lsw(void);
uint32 HAL_qdss_tpdm_cmb_get_ts_patt_val_msw(void);
uint32 HAL_qdss_tpdm_cmb_get_ts_patt_mask_lsw(void);
uint32 HAL_qdss_tpdm_cmb_get_ts_patt_mask_msw(void);
uint32 HAL_qdss_tpdm_dsb_get_mode(void);
uint8 HAL_qdss_tpdm_dsb_get_patt_type(void);
uint32 HAL_qdss_tpdm_dsb_get_ext_trigger_on_off(void);
uint32 HAL_qdss_tpdm_dsb_get_patt_match(void);
uint32 HAL_qdss_tpdm_dsb_get_edcmr(uint32 index);
uint32 HAL_qdss_tpdm_GetGPR(uint32 GPRIndex);

void HAL_qdss_tpdm_unlock_access(void);
void HAL_qdss_tpdm_lock_access(void);

#endif /* __HAL_QDSS_TPDM_H__ */

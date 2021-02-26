/*========================================================================
This file contains fuctions headers for the AFE Spdif Util functions.

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/spdif/src/AFESpdifDriverUtils_i.h#1 $
 ====================================================================== */


#ifndef _AFE_SPDIF_DRIVER_UTILS_I_H_
#define _AFE_SPDIF_DRIVER_UTILS_I_H_


#if defined(__cplusplus)
extern "C" {
#endif



ADSPResult afe_spdif_device_cfg_init(lpasshwio_prop_spdif_struct_t * pSpdifTxHWInfo);

void afe_spdif_deallocate_hardware_buf(afe_spdif_hw_buf_info_t *p_hw_buf_info);

ADSPResult afe_spdif_allocate_hardware_buf(spdif_state_struct_t *p_dev_state, afe_spdif_hw_buf_info_t *p_hw_buf_info);

ADSPResult afe_spdif_start(afe_spdif_driver_t *p_spdif_driver, spdif_state_struct_t *p_dev_state);

ADSPResult afe_spdif_stop(void);

void afe_spdif_interrupt_callback(void * args );

void afe_spdif_gettime_per_sample(uint32 sample_rate, uint32 *p_time, int32 *p_format);

ADSPResult afe_spdif_init_chstatus(spdif_state_struct_t *p_dev_state);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // !defined(_AFE_SPDIF_DRIVER_UTILS_I_H_)




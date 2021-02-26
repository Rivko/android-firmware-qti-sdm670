#ifndef __AFE_SLIMBUS_VFR_DRIVER_H__
#define __AFE_SLIMBUS_VFR_DRIVER_H__
/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusTimerUtils_i.h#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFESlimbusTimerUtils_i.h

DESCRIPTION: Main Interface to the AFE Slimbus driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusTimerUtils_i.h#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
08-08-2010  mspk  Initial Draft


============================================================================*/
#include "AFESlimbusDriverUtils_i.h"

/*=====================================================================
 Macros
 ======================================================================*/
#define VFR_FRAME_SIZE 20
#define FALSE_ALARM_MAX_FRAME_CNT 6

/*=====================================================================
 Typedef's
 ======================================================================*/
typedef struct sample_rate_struct
{
    uint32_t sample_rate;   ///< Sampling Rate
    uint32_t time_per_sample; ///< 1 / (Sampling Rate) microsecs (Q25 format)
    uint16_t samples_per_ms;   ///< Approximate sample count transferred per ms. 
    uint16_t drift_unit;
} sample_rate_info_t;
/*=====================================================================
 Functions
 ======================================================================*/
ADSPResult afe_sb_alloc_progress_counter(afe_slimbus_state_struct_t *p_dev_state);
ADSPResult afe_sb_dealloc_progress_counter(afe_slimbus_state_struct_t *p_dev_state);
ADSPResult afe_sb_read_progress_counter(afe_slimbus_state_struct_t *p_dev_state, uint64_t *samp_cnt, 
                                        SlimBusCounterType type, uint16_t vfr_src_idx);
void afe_get_sample_rate(int16_t index, uint32_t *pRate);
void afe_get_samples_per_ms(int16_t index, uint16_t *pSamples);
void afe_get_time_per_sample(int16_t index, uint32_t *pTime, uint16_t *pFormat);
void afe_get_sample_rate_index(uint32_t samplingRate, int16_t *pIndex);
void afe_read_avt_and_sb_counters_atomically(afe_dev_port_t *dev_port_ptr, uint64_t *av_time, uint64_t *sb_time);

#if ( __QDSP6_ARCH__ >= 4 )
extern "C" {
   void afe_sb_read_avt_sb_time_asm(afe_dev_port_t *dev_port_ptr, uint64_t *sb_time, uint64_t *av_time);
}
#endif //( __QDSP6_ARCH__ >= 3 )

#endif /*#ifndef __AFE_SLIMBUS_VFR_DRIVER_H__*/

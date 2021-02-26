/*========================================================================
  This file contains AV timer drift calculation and rate matching apis

  Copyright (c) 2009-2012,2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
 QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFEAvtDrift.h#1 $
 ====================================================================== */
#ifndef _AFE_AVTDRIFT_H_
#define _AFE_AVTDRIFT_H_

#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
	  Macro Defines
========================================================================== */
/* max PPM designed for */
#define AFE_MAX_PPM 200

/** Every AV_TIMER_DRIFT_CALC_PERIOD ms the acc_avt_drift_us field in afe_drift_info_t
 * is updated, triggerred by device interrupt.*/
#define AFE_AV_TIMER_DRIFT_CALC_PERIOD 100

/**
 * AV timer drift frame counter threshold. if the difference between current and
 * previous frame counters is between these value (including these), then drift is valid.
 * Else drift is invalid & previous value of AV timer drift in the client should be reinit'ed.
 *
 * 1 is the typical diff, but diff of 2 could be due to jitter.
 */
#define AFE_AV_TIMER_DRIFT_FRM_CNTR_HIGH_THRESH 2
#define AFE_AV_TIMER_DRIFT_FRM_CNTR_LOW_THRESH 1

/**
 * maximum drift expected at once in us
 */
#define AFE_AV_TIMER_DRIFT_THRESH_US (1<<15)

/**< accumulated AV timer drift value can be obtained from
 *  afe_drift_info_t.avt_drift_info by shifting right with
 *  below val & type-casting to signed 32 bits.*/
#define AFE_ACC_AVT_DRIFT_US_SHIFT  32
/**
 * by typecasting the afe_drift_info_t.avt_drift_info to unsigned 32 bits
 * we get frame counter that counts from zero when port is started & increments every 100 DMAs.
 */
#define AFE_AVT_DRIFT_FRAME_COUNTER_SHIFT   0

#define AVT_DRIFT_INFO_IDX      0

/** Drift feedback to AFE clients */
typedef struct	afe_drift_info
{
   volatile uint64_t avt_drift_info;
	/**< consists of accumulated avt drift in us (Most significant 32bit word: AFE_ACC_AVT_DRIFT_US_SHIFT) &
	 * a frame counter (LSW: AFE_AVT_DRIFT_FLAGS_SHIFT).
                                     * only frame counter difference of 1 or 2 gives valid drift. If frame counter difference is not 1 or 2,
                                     * then instantaneous drift should be treated as zero.
                                     * positive drift => AV timer is faster than device.
                                     * overflow in accumulated drift should be handled by client.*/
   volatile uint64_t vfr_drift_info;
	/**< consists of accumulated vfr drift in us (Most significant 32bit word: AFE_ACC_VFR_DRIFT_US_SHIFT) &
	 * a frame counter (LSW: AFE_VFR_DRIFT_FLAGS_SHIFT).
                                     * positive drift => VFR is faster than device.
                                     * overflow in accumulated drift should be handled by client.*/
   int16_t           vfr_enabled;
                     /**< whether VFR is enabled or not */
   int16_t           is_first_vfr;
                     /**< If it is first VFR interrupt */
   int32_t           samples_in_vfr_frame;
                     /**< Number of samples in VFR frame */
   uint64_t          dev_latched_counter_curr;
                     /**< Current device latched counter */
   uint64_t          dev_latched_counter_prev;
                     /**< Current device latched counter */
   int16_t           inst_sample_drift;
                     /**< Accumulated instantaneous drift in samples 
                          used for thresholding */
   int32_t           vfr_acc_sample_drift_to_update;
                     /**< Accumulated drift in samples to be updated */
   uint64_t          curr_vfr_intr_dev_av_timestamp;
                     /**< Derived AV timer timestamp at VFR interrupt */
   uint64_t          prev_avt_drift_info;
                     /**< Previous AVT drift info */
   uint64_t          init_av_timestamp;
                     /**< Initial AV timestamp for port */
   uint32_t          running_dev_int_counter;
                     /**< running device interrupt counter. Reset whenever port starts or VFR is enabled*/
   uint64_t          latest_dev_av_timestamp;
                     /**< av timestamp associated with latest device interrupt */
   uint32_t          init_fifo_samples;
                     /**< samples in FIFO for first device interrupt for this port & VFR source*/
   uint32_t          latest_fifo_samples;
                     /**< samples in FIFO for latest device interrupt for this port & VFR source*/
   uint32_t          running_vfr_int_counter;
                     /**< Running counter of VFR interrupts wrt this port */
   uint64_t          init_vfr_av_timestamp;
                     /**< timestamp for the initial VFR interrupts */

   qurt_elite_mutex_t port_vfr_mutex;
                     /**< mutex for a particular VFR associated with a particular port */

   bool_t             port_vfr_mutex_init;
                      /**< flag to see if the mutex has been initialized or not */

} afe_drift_info_t;

/*
 * drift info that's obtained after reading the 64bit avt_drift_info from AFE.
 */
typedef struct afe_client_avt_drift_info
{
   int32_t     acc_avt_drift_us;
               /**< avt drift in us */
   uint32_t    frame_counter;
               /**< frame counter */
} afe_client_avt_drift_info_t;

/**
 * reads all 64 bits of afe_avt_drift_info in one shot & gives the values in acc_avt_drift & frame_counter
 */
static inline void afe_port_read_avt_drift(const volatile uint64_t *afe_avt_drift_info, afe_client_avt_drift_info_t *drift_info )
{
   uint64_t avt_drift_info  = *(afe_avt_drift_info); //read all 64 bits in one shot, MEMD
   drift_info->acc_avt_drift_us  = (int32_t) (avt_drift_info>>AFE_ACC_AVT_DRIFT_US_SHIFT);
   drift_info->frame_counter     = (uint32_t)(avt_drift_info>>AFE_AVT_DRIFT_FRAME_COUNTER_SHIFT);
}

/**
 * Used as a wrapper function to update device interface timing statistics
 */
ADSPResult afe_get_timing_stats(void *dev_port, int8_t* param_buffer_ptr, int32_t param_buf_len, uint16_t* params_buffer_len_req_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // _AFE_AVTDRIFT_H_

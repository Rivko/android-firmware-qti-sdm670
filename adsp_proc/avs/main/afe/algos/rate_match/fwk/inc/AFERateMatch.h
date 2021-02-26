/*========================================================================
 This file contains Rate matching high level apis

 Copyright (c) 2015 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
 QUALCOMM Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.

 $Header: //components/rel/avs.adsp/2.8.5/afe/algos/rate_match/fwk/inc/AFERateMatch.h#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 8/7/2015   rv       Created
 ====================================================================== */
#ifndef _AFE_RATE_MATCH_H_
#define _AFE_RATE_MATCH_H_

/*==========================================================================
 File includes
 ========================================================================== */
#include "AFEInternal.h"

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

/*==========================================================================
 Macro
 ========================================================================== */

/**
 * number of buffers to be released or collected before sample slip
 */
#define AFE_NUM_BUFS_RELEASE_B4_SS 1
#define AFE_NUM_BUFS_COLLECT_B4_SS 1
/* Need two buffers to collect in rate matching algorithm as we can enter the scenario where
 * two Rx intr can occur in between two Tx intr, thereby rate match algo can underrun.)
 * In order to resolve this, we will collect one more buffer and in effect it will introduce 1 frame delay.
 */
#define AFE_NUM_BUFS_COLLECT_B4_SS_FRAC_RESAMPLE 2



/*==========================================================================
 Function Defines
 ========================================================================== */
/**
 * this function doesn't directly modify the afe_client_info_ptr->rate_match_info_ptr because, that might need mutex hold.
 * to avoid that ptr to rate_match_info_ptr is passed.
 *
 * it does following:
 * -obtains params that depend on sampling rate
 * -allocate data buffer for rate matching by first freeing if necessary.
 * -initializes rate match data buffer to zero.
 * -release any client buffers holding in pcm_bufs circular array
 * -initialize sample slip library by deinitializing (since it might already be initialized)
 * -reset the drift into & drift emory.
 */
ADSPResult afe_port_rate_match_info_reinit(afe_dev_port_t* pDevPort,
                                           afe_client_info_t *afe_client_info_ptr,
                                           afe_rate_match_info_t *rate_match_info_ptr);
/**
 * cleans up rate_match_info_ptr from afe_client_info_ptr
 */
ADSPResult afe_port_rate_match_info_destroy(afe_client_info_t *afe_client_info_ptr);

/**
 * this function releases any holding buffers in pcm_bufs and resets the tracker counters to zero.
 */
void afe_port_rate_match_client_buf_release(afe_rate_match_info_t *rate_match_info_ptr);

/**
 * Calculates the client drift. Positive client_port_drift implies client is faster than port.
 */
void afe_port_client_calc_drift(afe_client_info_t *client_info_ptr, afe_dev_port_t *dev_port_ptr);

/**
 * positive client_port_drift implies client is faster than port.
 * Hence, +ve, samples will be slip, and when negative -> stuff.
 */
void afe_port_client_get_drift_to_correct(afe_client_info_t *client_info_ptr,
                                          afe_dev_port_t *dev_port_ptr,
                                          int32_t *drift_to_correct_samples);

/**
 * copies samples from client_buf to rate_match_buf
 * number of bytes copied: bytes_per_ch_to_copy
 * channel spacing in rate match buf:  ch_spacing_in_rate_match_buf
 *
 * this function also takes care of sample slipping.
 * Suppose, 49(48+1) samples are copied: samples slipping by 1 is taken because ch_spacing_in_rate_match_buf is set to 48 (assume samples).
 * since channels are copied one by one (deint), extra sample of chan=0 is replaced by 1st sample of chan=1 etc.
 * last channel will exceed the buffer by a sample.
 */
void afe_port_cpy_client_to_rate_match_buf(int8_t *rate_match_buf, int8_t *client_buf,
                                           bool_t interleaved, uint32_t bytes_per_ch_to_copy,
                                           uint32_t ch_spacing_bytes, uint16_t num_chan,
                                           uint32_t ch_spacing_in_rate_match_buf,
                                           uint32_t rate_match_buf_size);
/**
 * assumes client_port_drift_samples is 1 or 0 or -1.
 *
 * positive client_port_drift implies client is faster than port.
 * Hence, +ve, samples will be slip, and when negative -> stuff.
 */
void afe_port_client_sample_slip_stuff(afe_rate_match_info_t *rate_match_info_ptr,
                                       uint32_t num_samples_in_buf, bool_t interleaved,
                                       uint16_t bytes_per_channel, uint16_t num_chan,
                                       int32_t client_port_drift_samples,
                                       int32_t client_samples_per_int);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // _AFE_RATE_MATCH_H_

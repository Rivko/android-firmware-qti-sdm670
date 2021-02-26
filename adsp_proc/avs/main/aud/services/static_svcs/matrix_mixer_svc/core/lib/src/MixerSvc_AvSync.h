/**
@file MixerSvc_AvSync.cpp
@brief This file defines matrix mixer AV-Sync utilities.
 */

/*========================================================================
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_AvSync.h#1 $

when              who   what, where, why
--------              ---      -------------------------------------------------------
06/24/2015  kr      Created file.
========================================================================== */
#ifndef MIXER_SVC_AV_SYNC_H
#define MIXER_SVC_AV_SYNC_H

#include "qurt_elite.h"
#include "Elite.h"
#include "avsync_lib.h"
#include "AFEInterface.h"
#include "capi_v2_sample_slip.h"
#include "avtimer_drv_api.h"
#include "capi_v2_adsp_error_code_converter.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define MTMX_TIME_Q_FACTOR 45
#define MTMX_TIME_FRAC_MASK (~(((uint64_t)(-1)) << MTMX_TIME_Q_FACTOR))

/* Notes on the following adsp_time structure and time/clock mgmt in the Mixer
 *
 * Error accumulation corrections are performed in three ways:
 * 1) Slow error, highly optimized code - uses the adsp_time structure to correct error.
 *
 *    This is used by all the internal state values that deal with time/timestamps in the mixer in/out
 *    port state structures.
 *
 * 2) No error, non-optimized code - uses aggregate number of samples processed to calculate timestamp
 *    and clock values. Must perform conversion between samples <-> time which requires 64-bit division.
 *
 *    This is used by the session clock updates from the output port context and render decisions in the
 *    input port context.
 *
 * 3) No correction - some code is not corrected for error accumulation due to trunctaion
 *
 *    This only applies to the expected session clock updates during input port underflow events.
 *
 *    TODO: move the adsp_time struct into a separate header/implementation file and update the
 *    avsync lib to use this construct. This will resolve the STC/Expected time clock workarounds
 *    in the mixer service layer and clean up the code.
 */
typedef struct adsp_time
{
	int64_t int_part;
	int64_t frac_part;   // Q19.45
} adsp_time;

static inline void mt_mx_copy_time_value(adsp_time *to, const adsp_time from)
{
	if (NULL == to) return;
	to->int_part = from.int_part;
	to->frac_part = from.frac_part;
	return;
}

static inline void mt_mx_clear_time(adsp_time *a)
{
	if (NULL == a) return;
	a->int_part = a->frac_part = 0;
	return;
}

static inline void mt_mx_overflow_time_value(adsp_time *a)
{
	if (NULL == a) return;
	// overflow any digits left of the radix point into the integer part of the time value
	a->int_part += (a->frac_part >> MTMX_TIME_Q_FACTOR);
	// efficiently clear values left of radix point preserving the sign.
	// Note: 1 | ( x >> 63) evaluates to 1 if x > 0 and -1 otherwise.
	a->frac_part = (1 | (a->frac_part >> 63)) * (a->frac_part & MTMX_TIME_FRAC_MASK);
	return;
}

static inline void mt_mx_decrement_time(adsp_time *a, adsp_time b)
{
	mt_mx_overflow_time_value(a);
	mt_mx_overflow_time_value(&b);

	// compute a -= b
	a->int_part -= b.int_part;
	a->frac_part -= b.frac_part;

	mt_mx_overflow_time_value(a);
	return;
}

static inline void mt_mx_increment_time(adsp_time *a, adsp_time b)
{
	mt_mx_overflow_time_value(a);
	mt_mx_overflow_time_value(&b);

	// compute a += b
	a->int_part += b.int_part;
	a->frac_part += b.frac_part;

	mt_mx_overflow_time_value(a);
	return;
}

typedef struct mt_mx_sampleslip_t mt_mx_sampleslip_t;
struct mt_mx_sampleslip_t
{
	capi_v2_t       *pSampleSlipCapiv2;
	capi_v2_buf_t   inBufs[8]; //8 channels
	capi_v2_buf_t   outBufs[8]; //8 channels
	uint32_t        frame_size;
	uint32_t        adj_frame_interval;
	uint32_t  unBufSizeInSamples; //Internal sample slip buffer size in samples
	uint32_t  unAlgDelay;       // Sample slip Algorithm delay in usec
	uint32_t  inBufOffset;      //Keeps track of number of samples present in the internal input buffer 
	uint32_t  outBufOffset;     //Keeps track of number of samples present in the internal output buffer 
  
	adsp_time totalBufOffsetInUsec; // Total internal(input+output)buffer samples in Usec
	/*
	 Timestamp of the samples duration to be adjusted after internal buffers are either full/empty.
	 In stuff scenario, over the time when internal input buffer becomes full then process out those extra samples first.
	 In slip scenario, over the time when internal output buffer becomes empty then refill the buffer from the current data buffer.
	*/
	adsp_time adjustSamplesInUsec; 
};

typedef struct mt_mx_inport_avsync_t mt_mx_inport_avsync_t;
struct mt_mx_inport_avsync_t
{
    bool_t              bUseDefaultWindowForRendering; //The first buffer after a Run command will use default window for rendering
    bool_t              bShouldActiveInputPortBeReProcessed; //When sample slip library is enabled, the i/p port processing may need to be delayed, esp. in low latency
    bool_t              bHasInputBufferUpdatedDrift; //Flag to indicate whether the i/p buffer has updated its drift atleast once.
    bool_t              bHasInputBufferUpdatedStats; //Flag to indicate whether the i/p buffer has updated the stats and drift atleast once.
    bool_t              bShouldSessionTimeBeAdjWhenSendingBufDown;
    bool_t              bIsStrClkSyncdWithSessClk;
    bool_t              bIsSampleAddDropEnabled;
    bool_t              bIsThisFirstSampleAddOrDrop;
    bool_t              bIsAdjSessClkFineCtlEnabled;
    uint8_t             samplesAddOrDropMask; //Mask that indicates whether samples should be added or dropped in order to achieve required session clock adjustment
    uint64_t            unNumRemSamplesAdj; //Number of samples that still need to be added or dropped to achieve required session clock adjustment
    int64_t             llAdjDuration; //Adjustment to session clock in us
    uint32_t            unStartFlag;       //Start flag provided in the Run command
    adsp_time           ullInBufHoldDurationInUsec; //Hold duration for current input buffer in usec
    mt_mx_sampleslip_t  structSampleSlip; //Structure for SampleSlip module needed for stream-to-device rate-matching
    void                *pAVSyncLib; //Pointer to AvSync Lib, it stores the most common AVSync variables across mtmx and strtr
    volatile const      afe_drift_info_t *pAfeDriftPtr; //AFE drift pointer. Applicable to TX matrix only.
};

typedef struct mt_mx_outport_avsync_t mt_mx_outport_avsync_t;
struct mt_mx_outport_avsync_t
{
	adsp_time                      ullOutBufHoldDurationInUsec; //Hold duration for current output buffer in microseconds
	volatile const afe_drift_info_t *pAfeDriftPtr;                 //AFE drift pointer. Applicable to RX matrix only.
};

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //MIXER_SVC_AV_SYNC_H

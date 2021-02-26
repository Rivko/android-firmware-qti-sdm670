/* ======================================================================== */
/**
    @file capi_v2_sample_slip.h

    Header file to implement the Audio Post Processor Interface for
    sample slip library.
*/

/* =========================================================================
Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================= */

/* =========================================================================
   Edit History

     when       who          what, where, why
   --------   -------     -------------------------------------------------
   1/8/15     hbansal        Initial creation
   ======================================================================== */

/*------------------------------------------------------------------------
 * Include files and Macro definitions
 * -----------------------------------------------------------------------*/
#ifndef CAPI_V2_SAMPLE_SLIP_H
#define CAPI_V2_SAMPLE_SLIP_H

#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#include "AFEAvtDrift.h"

/*------------------------------------------------------------------------
 * Macros, Defines, Type declarations
 * -----------------------------------------------------------------------*/
enum ss_port_direction
{
   Playback,
   Record
};

const uint32_t AUDPROC_PARAM_ID_SAMPLESLIP_ENABLE = 1;
struct audproc_sampleslip_enable_t
{
   uint32_t enable; // 0 = disable, else enable
};

const uint32_t AUDPROC_PARAM_ID_SAMPLESLIP_DRIFT_POINTER = 2;
struct audproc_sampleslip_drift_pointer_t
{
    volatile const afe_drift_info_t *primary_drift_info_ptr;
    volatile const afe_drift_info_t *current_drift_info_ptr;
    volatile const int32_t *stream_to_device_drift_info_ptr; //This pointer to be set to NULL for regular drift correction on COPP path
};

const uint32_t AUDPROC_PARAM_ID_SAMPLESLIP_SAMPLE_ADJUST = 3;
struct audproc_sampleslip_sample_adjust_t
{
    int64_t  compensated_drift;     //compensated_drift value in us.
};

//  internal param id to communicate the compensated drift value to PP service
const uint32_t AUDPROC_PARAM_ID_SAMPLESLIP_DIRECTION = 3;
struct audproc_sampleslip_direction_t
{
   ss_port_direction direction;
};

const uint32_t AUDPROC_PARAM_ID_SAMPLESLIP_FRAMESIZE_INFO = 4;
struct audproc_sampleslip_framesize_info_t
{
   uint32_t frame_size;
   uint32_t adj_frame_interval;
};

//internal param id to get the internal buffer sizes created for sample slip library
const uint32_t AUDPROC_PARAM_ID_SAMPLESLIP_INTERNAL_BUFFER_SIZE = 5;
struct audproc_sampleslip_internal_buffer_size_t
{
   uint32_t buffer_size_in_samples; //size of the internal buffer
};

//internal param id to get the history data length present in both the input and output internal buffers 
const uint32_t AUDPROC_PARAM_ID_SAMPLESLIP_DELAYLINE_OFFSET_VALUE = 6;
struct audproc_sampleslip_delayline_offset_value_t
{
   uint32_t delayline_in_offset_value;  //number of samples present in the internal input buffer
   uint32_t delayline_out_offset_value; //number of samples present in the internal output buffer
};

/*------------------------------------------------------------------------
* Function declarations
* -----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

capi_v2_err_t capi_v2_sample_slip_get_static_properties (capi_v2_proplist_t *init_set_properties, capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_sample_slip_init (capi_v2_t *_pif, capi_v2_proplist_t *init_set_properties);

#ifdef __cplusplus
}
#endif

#endif //CAPI_V2_SAMPLE_SLIP_H


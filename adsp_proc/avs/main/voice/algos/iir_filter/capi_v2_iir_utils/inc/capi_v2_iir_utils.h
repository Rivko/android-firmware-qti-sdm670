/**
@file capi_v2_iir_utils.h

@brief capi v2 iir utils for capi v2 iir wrappers.

*/

/*========================================================================

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/16/2014  kn       Created
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
---------------------------------------------------------------------------*/

#ifndef CAPI_V2_IIR_UTILS
#define CAPI_V2_IIR_UTILS

#include "shared_lib_api.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "Elite_CAPI_V2.h"


/*----------------------------------------------------------------------------
 * Defines
 * -------------------------------------------------------------------------*/
#define  VOICE_ROUNDTO8(x) ((((uint32_t)(x) + 7) >> 3) << 3)
#define VOICE_NB_SAMPLING_RATE      (8000)
#define VOICE_WB_SAMPLING_RATE      (16000)
#define VOICE_FB_SAMPLING_RATE      (48000)
#define VOICE_SWB_SAMPLING_RATE     (32000)

#define VOICE_SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))
#define NO_OF_BITS_PER_SAMPLE 16

#define VOICE_IIR_MAX_NUM_CHANNELS     4     // Max number of channels supported by iir filter
#define VOICE_IIR_MAX_STAGES          10     // Maximum number of iir stages
#define VOICE_IIR_SHIFT                2     // Qfactor shift required for iir_tdf2 implementation
#define VOICE_IIR_NUM_COEFF            3     // Number of numerator coefficients in iir filter
#define VOICE_IIR_DEN_COEFF            2     // Number of denominator coefficients in iir filter

typedef struct voice_iir_cfg_t
{
   /**< Array of num and den coefficients for IIR Tuning filter */
   int32_t na_coeff[VOICE_IIR_MAX_STAGES * (VOICE_IIR_NUM_COEFF + VOICE_IIR_DEN_COEFF)];
   /**< Memory needed for iir_tdf2. 2 for a stage */
   QURT_ELITE_ALIGN(int32_t na_state_memory[VOICE_IIR_MAX_STAGES * VOICE_IIR_DEN_COEFF], 8);
   /**< No of Tuning Filter Stages */
   int16_t  num_of_stages;
   /**< shift factor used for numerator */
   int16_t  shift_fact_num[VOICE_IIR_MAX_STAGES];
   /**< shift factor used for Denominator */
   int16_t  shift_fact_den[VOICE_IIR_MAX_STAGES];
   /**< PreGain for compensating filter response */
   uint16_t pre_gain;
   /**< Flag to enable or disable IIR filter. 1-enable, 0-disable */
   int8_t   enable;
}voice_iir_cfg_t;

typedef struct{
   capi_v2_set_get_media_format_t media_format;
   capi_v2_standard_data_format_t data_format;
} iir_media_format_t;


typedef struct capi_v2_voice_iir_t
{
   capi_v2_vtbl_t              *vtbl;
   /**< IIR Library instance pointer*/
   void *lib_ptr;
   /**< Configured sampling rate */
   uint32_t sampling_rate;
   /**< Configured no of channels */
   uint32_t num_channels;
   /**< Memory pointer */
   void *mem_ptr;
   /**< Heap ID to be used for memory allocation */
   QURT_ELITE_HEAP_ID heap_id;
   /**< IIR mpps information in killo packets per second (kpps) unit */
   uint32_t kpps;
   /**< Delay in us */
   uint32_t delay;
   /**< Callback function to issue to framework to report delay and kpps */
   capi_v2_event_cb_f event_cb;
   /**< Event context that must be issued in the callback */
   void* event_context;
   /**< Flag to indicate algorithmic state reset required after set properties. 1-Reset required, 0-not required */
   uint8_t  algo_reset;
}capi_v2_voice_iir_t;

typedef struct voice_iir_mic_lib_t
{
	voice_iir_cfg_t voice_iir_cfg;

} voice_iir_mic_lib_t;

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

capi_v2_err_t capi_v2_voice_iir_get_static_properties (capi_v2_proplist_t *init_set_properties, capi_v2_proplist_t *static_properties);
capi_v2_err_t capi_v2_voice_iir_set_properties (capi_v2_t* _pif, capi_v2_proplist_t *props_ptr);
capi_v2_err_t capi_v2_voice_iir_get_properties (capi_v2_t* _pif, capi_v2_proplist_t *props_ptr);
capi_v2_err_t capi_v2_voice_iir_init (voice_iir_cfg_t *voice_iir_cfg_ptr);
capi_v2_err_t capi_v2_voice_iir_process (voice_iir_cfg_t *voice_iir_cfg_ptr, int16_t* in_ptr, int16_t* out_ptr, int16_t framesize, uint16_t in_buf_size);
capi_v2_err_t capi_v2_voice_iir_end (capi_v2_t *_pif);
capi_v2_err_t capi_v2_voice_iir_set_param (voice_iir_cfg_t *voice_iir_cfg_ptr, uint32_t param_id, capi_v2_buf_t *params_ptr);
capi_v2_err_t capi_v2_voice_iir_get_param (voice_iir_cfg_t *voice_iir_cfg_ptr, uint32_t param_id, capi_v2_buf_t *params_ptr);
capi_v2_err_t capi_v2_voice_iir_reset (voice_iir_cfg_t *voice_iir_cfg_ptr);
uint32_t capi_v2_voice_iir_get_kpps (uint32_t sampling_rate, int16_t num_of_stages);
uint32_t capi_v2_voice_iir_get_delay (uint32_t sampling_rate, int16_t num_of_stages);
capi_v2_err_t capi_v2_voice_iir_send_kpps (capi_v2_voice_iir_t* me);
capi_v2_err_t capi_v2_voice_iir_send_delay (capi_v2_voice_iir_t* me);

capi_v2_err_t capi_v2_voice_iir_mic_set_properties (capi_v2_t* _pif, capi_v2_proplist_t *props_ptr);
capi_v2_err_t capi_v2_voice_iir_mic_init (capi_v2_t *_pif, capi_v2_proplist_t  *init_set_properties, capi_v2_vtbl_t *vtbl_ptr);
capi_v2_err_t capi_v2_voice_iir_mic_set_param (capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr);
capi_v2_err_t capi_v2_voice_iir_mic_get_param (capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr);
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //__CAPI_V2_IIR_UTILS

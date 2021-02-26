
/*========================================================================

This file contains declarations of Audio DecoderService utility functions.

Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/utils/inc/audio_module_chain_wrapper.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/17   rishabht   Created file.

========================================================================== */
#ifndef AUDIO_MODULE_CHAIN_WRAPPER_H
#define AUDIO_MODULE_CHAIN_WRAPPER_H


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "adsp_core_api.h"
#include "shared_aud_cmn_lib.h"
#include "adsp_asm_stream_commands.h"
#include "adsp_media_fmt.h"
#include "adsp_media_fmt_prop.h"
#include "adsp_private_api_ext.h"
#include "Elite_CAPI_V2.h"
#include "Elite_CAPI_V2_private_params.h"
#include "capi_v2_adsp_error_code_converter.h"
#include "qurt_elite_adsppm_wrapper.h"
#include "Elite_fwk_extns_compr_over_pcm.h"
#include "adsp_avs_common_api.h"
#include "adsp_amdb_static.h"




/* -----------------------------------------------------------------------
 ** Global definitions/forward declarations
 ** ----------------------------------------------------------------------- */

#define MODULE_MAX_INPUT_STREAMS                  (1)
#define MODULE_MAX_OUTPUT_STREAMS                 (1)
#define MAX_NUM_CHANNEL                           (8)

static const uint32_t AUD_FORMAT_INTERLEAVED = 1;
static const uint32_t AUD_FORMAT_DEINTERLEAVED = 0;

// Define the only media format type supported.
typedef struct aud_module_media_fmt_t
{
    capi_v2_data_format_header_t h;
    capi_v2_standard_data_format_t f;
}aud_module_media_fmt_t;

typedef struct aud_module_media_event_t
{
   bool_t event_is_pending;
   aud_module_media_fmt_t event;
}aud_module_media_event_t;

// Module input port structure and its methods
typedef struct module_port_t
{
   aud_module_media_event_t media_event;
}module_port_t;

typedef struct module_stream_io_struct{
  capi_v2_buf_t                       *in_buf[MODULE_MAX_INPUT_STREAMS];
  capi_v2_buf_t                       *out_buf[MODULE_MAX_OUTPUT_STREAMS];

  capi_v2_stream_data_t               inputs[MODULE_MAX_INPUT_STREAMS];
  capi_v2_stream_data_t               outputs[MODULE_MAX_OUTPUT_STREAMS];

  capi_v2_stream_data_t               *inputs_ptr[MODULE_MAX_INPUT_STREAMS];
  capi_v2_stream_data_t               *outputs_ptr[MODULE_MAX_OUTPUT_STREAMS];
}module_stream_io_struct;

// The module structure and its methods
typedef struct module_info_t
{
   capi_v2_t*      module_ptr;
   uint32_t        module_id;
   uint32_t        instance_id;
   bool_t          is_in_place;
   bool_t          is_enabled;
   uint32_t        KPPS;
   uint32_t        bandwidth;
   uint32_t        delay_in_us;
   capi_v2_port_num_info_t port_info;
   module_port_t *input_ports_ptr;   // check if this canbe removed
   module_port_t *output_ports_ptr;
   void *port_mem;
   bool_t forced_disabled;
   module_stream_io_struct  stream;
} module_info_t;


typedef struct module_CAPI_container_t
{

   module_info_t  module_info;
   adsp_amdb_module_handle_info_t *handle_info_ptr;
   int8_t *OutBuf[MODULE_MAX_INPUT_STREAMS];  // correct naming , first one should not be CAPs
}module_CAPI_container_t;

typedef struct _AudioModuleCapiWrapper_t
{
    module_CAPI_container_t               *module_capi_container; // modules CAPI info
    uint32_t                              num_modules;
    int8_t                                *pStream_buf_mem;
    uint32_t                              stream_buf_size;
    int8_t                                *OutBuf[MODULE_MAX_INPUT_STREAMS];
    uint32_t                              OutBufSize;
}AudioModuleCapiWrapper_t;

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

void audio_module_chain_wrapper_process(AudioModuleCapiWrapper_t *modules, uint64_t *pPayload);
ADSPResult audio_module_chain_wrapper_set_param(AudioModuleCapiWrapper_t *modules, void *pPayload, uint32_t payloadSize, uint32_t ulParamId);
ADSPResult audio_module_chain_wrapper_create(AudioModuleCapiWrapper_t *modules, uint32_t *modules_stack_size, uint32_t num_modules, avcs_module_instance_info_t *module_instance_info_array);
void audio_module_chain_wrapper_set_media_fmt(AudioModuleCapiWrapper_t *modules, elite_msg_any_t *msg, uint32_t out_buf_num_unit_frames, uint32_t bitstream_format);
int32_t audio_module_chain_wrapper_get_bw(AudioModuleCapiWrapper_t *modules);
int32_t audio_module_chain_wrapper_get_kpps(AudioModuleCapiWrapper_t *modules);
ADSPResult audio_module_chain_wrapper_get_param(AudioModuleCapiWrapper_t *modules, void *pPayload, uint32_t *pParamsize, uint32_t max_size, uint32_t ulParamId);
void audio_module_chain_wrapper_destroy(AudioModuleCapiWrapper_t *modules);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_MODULE_CHAIN_WRAPPER_H

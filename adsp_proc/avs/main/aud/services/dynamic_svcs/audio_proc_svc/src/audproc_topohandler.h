
/**
@file audproc_topohandler.h

   Header file for the generic APPI topology data structures and
   functions.
*/

/*========================================================================

Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/29/11   wj      add per-channel IIR topology
03/28/11   ss      Introducing PROCESS_CHECK for Resampler and Downmix (APPI Rev D)
01/12/11   ss      Delay accumulation mechanism for EOS, so that all the
                   intermediate buffers are flushed out before processing EOS
1/4/11     DG      Created file.

========================================================================== */

#ifndef AUDPROC_TOPOHANDLER_H
#define AUDPROC_TOPOHANDLER_H


/*----------------------------------------------------------------------------
 * Include files
 * -------------------------------------------------------------------------*/

#ifndef TST_TOPO_LAYER
#ifdef AUDPPSVC_TEST
#include "hexagon_sim_timer.h"
#endif // AUDPPSVC_TEST
#include "adsp_media_fmt.h"
#include "capi_v2_sample_slip.h"
#include "adsp_amdb_static.h"
#include "adsp_audproc_api.h"
#include "AudDynaPPSvc.h"
#include "EliteCmnTopology_db_if.h"
#include "EliteCmnTopology_db.h"
#else
#include "tst_topo_layer.h"
#endif

/*---------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/
#define AUDPROC_COPP_TOPOLOGY_ID_MCHAN_V2_DEFAULT   0xABCDEFE3
//#define PP_TOPO_ENABLE_LOW_LEVEL_MSG  0
#define MAX_SUPPORTED_DATA_THRESHOLD_UNIT_FRAMES                   (80) // max supported data threshold unit frames in topo layer. Any value greater than this will be ignored

const uint32_t AUDPROCTST_MODULE_ID_BYTESHIFTER_CAPIV2 = 100;

/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

// post processing chain format information structure
typedef struct _AdspAudioMediaFormatInfo_t
{
   uint32_t channels;
   uint32_t  bitsPerSample;
   uint32_t  samplingRate;
   uint32_t isSigned;
   uint32_t isInterleaved;
   uint8_t channelMapping[8];
   uint32_t bitstreamFormat;
} AdspAudioMediaFormatInfo_t;

typedef struct _AudPP_BufInfo_t
{
   int8_t *pBuf;
   uint32_t offsetInSamples; // This value is per channel
   uint32_t sizeInSamples;   // This value is per channel
   capi_v2_stream_flags_t flags;
   int64_t timestamp;
} AudPP_BufInfo_t;

struct topo_struct;

struct topo_event_client_t;
struct topo_event_vtable_t
{
   void (*KPPS)(topo_event_client_t *obj_ptr, uint32_t new_KPPS);
   void (*bandwidth)(topo_event_client_t *obj_ptr, uint32_t new_bandwidth);
   void (*output_media_format)(topo_event_client_t *obj_ptr, const AdspAudioMediaFormatInfo_t *new_format);
   void (*algorithmic_delay)(topo_event_client_t *obj_ptr, uint32_t new_delay);
   void (*custom_event) (topo_event_client_t *obj_ptr, void *payload, uint32_t payloadsize);
};

struct topo_event_client_t
{
   const topo_event_vtable_t *vtbl;
};

typedef enum topo_set_get_param_mode_t
{
	/*set param Payload : asm_stream_param_data_v2_t */
	FIRST_INSTANCE = 0,	/* Set/Get-Param is done to first instance of a given module. */
	ZERO_INSTANCE,	/* Set/Get-Param is done to instance with zero ID of a given module.*/
	ALL_INSTANCES, /* Set-Param is done to all the instances of a given module. Get-Param not supported.*/

	/*set param Payload : asm_stream_param_data_v3_t */
	GIVEN_INSTANCE,  /* Set/Get-Param is done to given instance of given module. */

	UNDEFINED_INSTANCE
}topo_set_get_param_mode_t;

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/


ADSPResult topo_get_module_list(const topo_struct *topo_obj, uint32_t* topo_module_list_ptr, uint32_t payload_size, uint32_t secOpCode);

ADSPResult topo_init(const audproc_topology_definition_t *def_ptr,
      const uint16_t obj_id,
      const uint32_t ouput_buffer_num_unit_frames,
      topo_struct **topo_obj,
      AudPP_AudProcType pp_type,
      uint32_t *stack_size_ptr,
      topo_event_client_t *event_client_ptr,
      bool_t initialize_with_compressed_format = FALSE,
	  bool_t is_input_output_realtime = FALSE,
	  uint32_t num_active_input_ports = 1);

ADSPResult common_topo_init(elite_cmn_topo_db_entry_t *def_ptr,
		const uint16_t obj_id,
		const uint32_t ouput_buffer_num_unit_frames,
		topo_struct **topo_obj,
		AudPP_AudProcType pp_type,
		uint32_t *stack_size_ptr,
		topo_event_client_t *event_client_ptr,
		bool_t initialize_with_compressed_format = FALSE,
		bool_t is_input_output_realtime = FALSE,
		uint32_t num_active_input_ports = 1);

#ifdef TST_TOPO_LAYER
ADSPResult topo_init_dag(const topo_tst_graph_helper &def,
      const uint16_t instance_id,
      const uint32_t max_output_samples,
      topo_struct **topo_obj,
      AudPP_AudProcType pp_type,
      uint32_t *stack_size_ptr,
      topo_event_client_t *event_client_ptr,
      bool_t initialize_with_compressed_format = FALSE,
		bool_t is_input_output_realtime = FALSE);
#endif

uint64_t topo_get_internal_buffer_delay (topo_struct *obj_ptr);

ADSPResult topo_process(topo_struct *obj_ptr,
		uint32_t num_in_ports,
      AudPP_BufInfo_t **in_bufs_ptr,
      uint32_t num_out_ports,
      AudPP_BufInfo_t **out_bufs_ptr);

void topo_change_input_media_type (topo_struct *obj_ptr,
      const AdspAudioMediaFormatInfo_t *pInMediaFormat,uint32_t port_id);

void topo_reset (topo_struct *obj_ptr);

ADSPResult topo_set_param (topo_struct *obj_ptr,
      void *pPayload,
      uint32_t payloadSize,
	  topo_set_get_param_mode_t mode);

ADSPResult topo_get_param (topo_struct *obj_ptr,
      void *pPayload,
      const uint16_t payloadSize,
      const uint32_t module_id,
	  const uint32_t instance_id, /*Ignored for any other mode value except GIVEN_INSTANCE.*/
      const uint32_t paramId,
      uint32_t *pParamSize,
	  topo_set_get_param_mode_t mode);

ADSPResult topo_module_register_events(topo_struct *topo_ptr, uint32_t event_id,uint32_t module_id,
		uint16_t instance_id, uint32_t config_mask);

uint32_t topo_module_get_module_id(topo_struct *topo_ptr, uint32_t index);
uint32_t topo_module_get_instance_id(topo_struct *topo_ptr, uint32_t index);
uint32_t topo_module_get_num_modules(topo_struct *topo_ptr);
ADSPResult topo_module_find_module_instance_presence(const topo_struct *topo_ptr, const uint32_t module_id, const uint32_t instance_id);
void topo_destroy (topo_struct *obj_ptr);

void topo_buffer_reset (topo_struct *obj_ptr,
      AudPP_BufInfo_t *outbuf_ptr,
      AudPP_BufInfo_t *prev_inbuf_ptr,
      bool_t* buffer_reset_flag_ptr,
      bool_t is_bg_thread_created,
      void *bg_thread_handle);

void topo_handle_eos(topo_struct *topo_ptr, uint32_t port_id, bool_t is_eos);

void topo_free_eos_buffer (topo_struct *topo_ptr,
      bool_t* buffer_reset_flag_ptr);

bool_t topo_is_module_present(topo_struct *topo_ptr, const uint32_t module_id);

void topo_compressed_reinit_with_output_sample_size(topo_struct *topo_obj, uint16_t max_output_samples);

void topo_set_volume1_state_to_pause(topo_struct *topo_ptr);

void  topo_set_start_pause(topo_struct *topo_ptr , audproc_soft_pause_params_t *softPauseParamPayload);

void  topo_set_ramp_on_resume(topo_struct *topo_ptr);

ADSPResult topo_send_capiv2_output_media_format (topo_struct *topo_ptr, const AudPPSvcOutputMediaTypeParams_t *pParams, uint32_t port_index = 0);

void topo_gen_out_media_fmt(topo_struct *topo_ptr);

uint32_t topo_get_algo_delay(topo_struct *topo_ptr);

bool_t topo_isBgThreadReq(topo_struct *topo_ptr);

QURT_ELITE_HEAP_ID topo_get_heap_id(bool_t use_lpa, AudPP_AudProcType pp_type);

uint32_t topo_get_stack_size(topo_struct *topo_ptr);

uint32_t topo_bg_th_out_buff_num_unit_frames(topo_struct *topo_ptr);

uint32_t topo_main_th_out_buff_num_unit_frames(topo_struct *topo_ptr);

bool_t topo_is_port_or_stream_active(topo_struct *topo_ptr, uint32_t port_id);
bool_t topo_is_port_req_more_data(topo_struct *topo_ptr, uint32_t port_id);
bool_t topo_any_module_with_main_th_unit_frames(topo_struct *topo_ptr, uint32_t port_id);



uint32_t topo_get_search_module_id(uint32_t module_id);

ADSPResult topo_get_virtualizer_module_id(uint32_t *module_id);
ADSPResult topo_get_virtualizer_delay_param_id(uint32_t *param_id);
ADSPResult topo_get_virtualizer_volume_ramp_param_id(uint32_t *param_id);
void topo_print_module_buffers_samples(topo_struct *topo_ptr);

bool_t topo_is_ec_present(topo_struct *topo_ptr);

void topo_reset_port_buffers(topo_struct *topo_ptr, uint32_t port_id);

uint32_t topo_get_bytes_per_sample(uint32 bits_per_sample);


#endif // #ifndef AUDPROC_TOPOHANDLER_H


/**
@file

@brief

*/

/*========================================================================
$Header:

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/15/2014  ka       Created
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef VOICE_CAPI_UTILS_H
#define VOICE_CAPI_UTILS_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"
#include "Elite.h"
#include "Elite_lib_get_imc.h"
#include "Elite_fwk_extns_avc_rve_imc.h"
#include "Elite_intf_extns_vocoder.h"
#include "Elite_intf_extns_tty.h"
#include "Elite_fwk_extns_cng.h"



#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*----------------------------------------------------------------------------
 * Macro Defination
 * -------------------------------------------------------------------------*/
#define VOICE_SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))


/*----------------------------------------------------------------------------
 * Data structures
 * -------------------------------------------------------------------------*/
typedef enum voice_capi_media_format_t
{
   VOICE_CAPI_MEDIA_FMT_PRE_EC = 0, //prior to EC, multichannel input with single port
   VOICE_CAPI_MEDIA_FMT_EC, //EC itself, multichannel input on port 0, single channel on port 1
   VOICE_CAPI_MEDIA_FMT_POST_EC, //after EC, single channel, single port input
   VOICE_CAPI_MEDIA_FMT_AVC_RVE, //AVC/RVE, 3 input ports
   VOICE_CAPI_NUM_MEDIA_FMT
}voice_capi_media_format_t;

typedef enum voice_capi_io_stream_data_idx_t
{
   INPUT_STRM_BUF_1 = 0,
   INPUT_STRM_BUF_2,
   EC_INPUT_STRM_BUF,
   EC_OUTPUT_STRM_BUF,
   OUTPUT_STRM_BUF_1,
   OUTPUT_STRM_BUF_2,
   RESAMPLER_OUTPUT_STRM_BUF,
   NUM_STRM_BUF_IDX
}voice_capi_io_stream_data_idx_t;

//list of interface extensions the module supports
typedef struct voice_intf_ext_list_t
{
   bool_t is_voc_intf_supported;
   bool_t is_tty_intf_supported;
}voice_intf_ext_list_t;

typedef struct voice_fwk_extn_list_t
{
   bool_t is_service_state_fwk_extn_supported;
   bool_t is_cng_fwk_extn_supported;
} voice_fwk_extn_list_t;

typedef struct{
   capi_v2_set_get_media_format_t media_format;
   capi_v2_standard_data_format_t data_format;
} voice_capi_data_format_struct_t;

typedef struct voice_module_port_t
{
   uint32_t data_threshold;               // data threshold of the port per buffer in bytes
   voice_capi_data_format_struct_t media_fmt;
}voice_module_port_t;

typedef struct voice_capi_module_port_info_t
{
   capi_v2_port_num_info_t port_num_info;
   voice_module_port_t *input_port;
   voice_module_port_t *output_port;
}voice_capi_module_port_info_t;

typedef struct voice_capi_module_t
{
   uint32_t module_id;
   uint16_t instance_id;
   uint32_t module_index; //position in module array
   uint32_t is_capi;
   /**< Flag to indicate that the module is a CAPI module. this will be removed once all modules
        are transitioned */
   capi_v2_t* module_ptr;
   void* svc_ptr;
   uint32_t mem_size_allocated;
   uint32_t kpps;
   uint32_t delay;
   uint32_t is_enabled;
   uint32_t stack_size;
   bool_t output_media_fmt_prop_pending;
   voice_capi_io_stream_data_idx_t input_buf_index;      //input stream data index
   voice_capi_io_stream_data_idx_t output_buf_index;  //output stream data index
   bool_t is_in_place;
   /**< Is this module inplace */
   bool_t requires_data_buffering;
   /**< Is the algo requires buffering */
   capi_v2_num_needed_framework_extensions_t num_extensions;
   /**< Number of Custom frameworks supported by the module */
   capi_v2_framework_extension_id_t *fwk_extn_ptr;
   /**< Custom frameworks supported by the module */
   void* amdb_handle_ptr; //adsp_amdb_capi_v2_t
   /**< Opaque handle returned by AMDB
   You can access the get_static_properties and init APIs via this handle */
   voice_capi_media_format_t media_format;
   bool_t   is_virtual_stub;
   voice_intf_ext_list_t intf_extn_list; /**< list of interface extensions the module supports */
   voice_fwk_extn_list_t fwk_extn_list;  //  list of framework extension support module required.
   voice_capi_module_port_info_t    port_info;
} voice_capi_module_t;

/*------------------------------------------------------------------------------
 *  IMC source and destination info structure
 *----------------------------------------------------------------------------*/

typedef enum
{
   IMC_SOURCE = 0,
   IMC_DESTINATION
}imc_src_dst_t;

/**< State information of the source or destination */
typedef enum
{
   IMC_STATE_NONE = 0,
   IMC_STATE_REGISTERED,
   IMC_STATE_SOURCE_OPENED_COMMUNICATION,
   IMC_STATE_SOURCE_CLOSED_COMMUNICATION
}imc_src_dst_state_t;

/**< IMC source or destination information */
typedef struct imc_src_dst_info_t
{
   int8_t               key[16];            // Communication Id will be first 4 bytes of this array.
   uint32_t             key_length;         // length of the key.
   imc_src_dst_state_t  state;              // IMC state. Module knows if it is source or destination depending on the communication id.
   imc_dest_info_t      *dest_handle;       // destination handle for communication Id instance.
   imc_src_info_t       *source_handle;     // Source handle for communication Id instance. For a destination there can be multiple sources.
   elite_lib_get_imc_t  *imc_lib_handle;    // IMC library handle
   uint8_t              src_frame_num;      // Number of frames source tries to connect with destination
}imc_src_dst_info_t;

typedef enum voice_intf_extn_idx
{
   INTF_EXTN_VOCODER_IDX = 0,
   INTF_EXTN_TTY_IDX,
   INTF_EXTN_MAX
}voice_intf_extn_idx;

typedef struct voice_intf_extn_list
{
   capi_v2_interface_extns_list_t intf_extn_num;
   capi_v2_interface_extn_desc_t intf_extn_desc[INTF_EXTN_MAX];
}voice_intf_extn_list;

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
uint32_t voice_capi_get_static_prop(voice_capi_module_t* module_info_ptr);
ADSPResult voice_capi_query_intf_ext(voice_capi_module_t* module_info_ptr);
void voice_capi_populate_data_format_struct(voice_capi_data_format_struct_t *data_format_struct_ptr, elite_multi_channel_pcm_fmt_blk_t *media_type_ptr);

/**<This function registers the source or destination to inter module communication.
 * In addition it allocates and pushed the buffers to its own queue if the module is source.
 */
capi_v2_err_t voice_imc_register_src_dest(imc_src_dst_info_t *imc_src_dst_into,int8_t *key,uint32_t keylen,uint32_t max_bufq_len,uint32_t alloc_num_buf,uint32 buf_size,imc_src_dst_t src_dst);

ADSPResult voice_topo_get_module_info( voice_capi_module_t* modules_array, uint32_t module_id,uint16_t instance_id,uint32_t num_module, voice_capi_module_t **modules_info);
bool_t voice_check_voc_capi(uint32_t media_type);
ADSPResult voice_capi_service_state_fwk_extn_handler(voice_capi_module_t *module_info_ptr);
ADSPResult voice_capi_cng_fwk_extn_handler(voice_capi_module_t *module_info_ptr);
bool_t is_capi_media_fmt_same(voice_capi_data_format_struct_t *media_fmt1, voice_capi_data_format_struct_t *media_fmt2);
void voice_print_media_fmt(voice_capi_module_t* curr_module_ptr);

static __inline bool_t voice_is_channel_map_valid(uint8_t  channel_map[8])
{
   if( 0 == channel_map[0] )
   {
      return FALSE;
   }
   return TRUE;
}



#ifdef __cplusplus
}
#endif //__cplusplus
#endif //VOICE_CAPI_UTILS_H

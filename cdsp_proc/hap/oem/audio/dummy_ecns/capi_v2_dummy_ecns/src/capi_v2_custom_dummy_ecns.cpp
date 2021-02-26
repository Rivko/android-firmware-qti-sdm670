/**

 @brief CAPI V2 API wrapper for Dummy ECNS example
 Currently the example, supports single, dual and quad mic use case based on
 number of channels from Voice framwork.

 A single CAPIv2 example overloads SingleMic(SM), DualMic(DM) and QuadMic(QM) dummy ECNS modules.
 Each mic configuration will have separate module Id and hence will have separate
 exposed entry functions which distinguish the dummy ECNS modules for SM/DM/QM.

 For SM Dummy ECNS: Eg: VOICE_MODULE_DUMMY_SM_ECNS 0x10027060
   capi_v2_custom_dummy_sm_ecns_get_static_properties
   capi_v2_custom_dummy_sm_ecns_init

 For DM Dummy ECNS: Eg: VOICE_MODULE_DUMMY_DM_ECNS 0x10027061
   capi_v2_custom_dummy_dm_ecns_get_static_properties
   capi_v2_custom_dummy_dm_ecns_init

 For QM Dummy ECNS: Eg: VOICE_MODULE_DUMMY_QM_ECNS 0x10027062
   capi_v2_custom_dummy_qm_ecns_get_static_properties
   capi_v2_custom_dummy_qm_ecns_init

 As it is dummy example, definitions are same for all. So, we are internally calling single function.
 User can update as different functions based on thier need.

 */

/*-----------------------------------------------------------------------
 Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "capi_v2_custom_dummy_ecns.h"
#include "capi_v2_dummy_ecns_calib.h"
#include "adsp_vparams_internal_api.h"
#include "adsp_vpm_api.h"
#include "adsp_vcmn_api.h"
#include "shared_lib_api.h"
#include "Elite_lib_get_capi_v2_module.h"
#include "dummy_ecns_lib.h" // Core library header file


/*==========================================================================
  Globals
  ========================================================================== */
#define ROUNDTO8(x) ((((uint32_t)(x) + 7) >> 3) << 3)
#define NB_SAMPLING_RATE      (8000)
#define WB_SAMPLING_RATE      (16000)
#define FB_SAMPLING_RATE      (48000)

#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))
#define DUMMY_ECNS_CFG_SIZE 4000

#define OUT_BUF_SAMP_48K 960
/* KPPS values */
/* worst case profiled values */
#define DUMMY_ECNS_NB_KPPS (10000)
#define DUMMY_ECNS_WB_KPPS (15000)
#define DUMMY_ECNS_FB_KPPS (20000)

/* Delay values */
/* worst case profiled values */
#define DUMMY_ECNS_NB_DELAY   (10000)
#define DUMMY_ECNS_WB_DELAY   (15000)
#define DUMMY_ECNS_FB_DELAY   (20000)

#define SINGLE_MIC 1
#define DUAL_MIC 2
#define QUAD_MIC 4

#define SM_NEAR_PRI_NUM_CHANNELS (SINGLE_MIC)
#define DM_PRI_NUM_CHANNELS (DUAL_MIC)
#define QM_PRI_NUM_CHANNELS (QUAD_MIC)
#define FAR_NUM_CHANNELS (1)

#define NUM_INPUT_PORTS  (2)
#define NUM_OUTPUT_PORTS (1)

#define INVALID    ((int32_t)(-1))


#ifndef VOICE_LOG_TAP_POINT_VPTX_RTM
#define VOICE_LOG_TAP_POINT_VPTX_RTM   0x00010F67
#endif

typedef struct{
   capi_v2_set_get_media_format_t media_format;
   capi_v2_standard_data_format_t data_format;
} capi_data_format_struct_t;

typedef struct capi_v2_dummy_ecns_t {
   const capi_v2_vtbl_t *vtbl;
   /**< Pointer to the virtual table functions */
   void *static_buf_ptr;
   void *param_struct_ptr;
   int32_t enable;
   uint32_t module_id;
   uint32_t num_mic;
   uint32_t sampling_rate;
   uint32_t output_framesize;  //2msec frame size
   int8_t params_modified;             //flag to indicate if interface paramters are changed.
   int32_t *mem_ptr;         // memory fragment parameters
   capi_v2_event_cb_f event_cb; /**< Callback function to issue to framework to report delay and kpps */
   void* event_context; /**< Event context that must be issued in the callback */
   uint32_t delay;
   uint32_t kpps;
   capi_v2_port_num_info_t num_port_info;
   capi_data_format_struct_t input_media_type[NUM_INPUT_PORTS];
   QURT_ELITE_HEAP_ID heap_id;
   capi_data_format_struct_t output_media_type[NUM_OUTPUT_PORTS];
   int8_t primary_input_port_idx;
   int8_t primary_mic_idx;
   int8_t secondary_mic1_idx;
   int8_t secondary_mic2_idx;
   int8_t secondary_mic3_idx;
   int8_t far_input_port_idx;
   int8_t primary_output_port_idx;
   int16_t pri_num_channels;  // used to track number of input channels for primary port based on topology.
   uint32_t param_id_1;
} capi_v2_dummy_ecns_t;

static capi_v2_err_t capi_v2_custom_dummy_ecns_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr);
static capi_v2_err_t capi_v2_custom_dummy_ecns_init(capi_v2_t* _pif, capi_v2_proplist_t *init_set_properties);

static capi_v2_err_t capi_v2_dummy_ecns_process(capi_v2_t* _pif, capi_v2_stream_data_t* input[], capi_v2_stream_data_t* output[]);
static capi_v2_err_t capi_v2_dummy_ecns_end(capi_v2_t* _pif);
static capi_v2_err_t capi_v2_dummy_ecns_set_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr);
static capi_v2_err_t capi_v2_dummy_ecns_get_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr);
static capi_v2_err_t capi_v2_dummy_ecns_set_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr);
static capi_v2_err_t capi_v2_dummy_ecns_get_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr);


static void capi_v2_dummy_ecns_send_kpps(capi_v2_dummy_ecns_t* me);
static void capi_v2_dummy_ecns_send_delay(capi_v2_dummy_ecns_t* me);
static void capi_v2_dummy_ecns_get_kpps_delay(capi_v2_dummy_ecns_t* me, uint32_t *delay, uint32_t *kpps);
static capi_v2_err_t capi_v2_dummy_ecns_send_output_mediatype(capi_v2_dummy_ecns_t *me);
capi_v2_err_t capi_v2_voice_dummy_ecns_init(capi_v2_dummy_ecns_t *me, capi_v2_proplist_t *init_set_properties);
capi_v2_err_t capi_v2_voice_dummy_ecns_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr);

static capi_v2_vtbl_t vtbl =
{
      capi_v2_dummy_ecns_process,
      capi_v2_dummy_ecns_end,
      capi_v2_dummy_ecns_set_param,
      capi_v2_dummy_ecns_get_param,
      capi_v2_dummy_ecns_set_properties,
      capi_v2_dummy_ecns_get_properties,
};



/*----------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * This function is used to query the static properties to create the CAPIv2.
 *
 * @param[in] init_props_ptr, pointer to the initializing prop list
 * @param[in, out] out_props_ptr, pointer to the output pro list
 * @return capi_v2_err_t, result code
 *----------------------------------------------------------------------------*/


capi_v2_err_t capi_v2_custom_dummy_sm_ecns_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr)
{

   capi_v2_err_t result;

   result = capi_v2_custom_dummy_ecns_get_static_properties(init_props_ptr, out_props_ptr);

   return result;
}


capi_v2_err_t capi_v2_custom_dummy_dm_ecns_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr)
{

   capi_v2_err_t result;

   result = capi_v2_custom_dummy_ecns_get_static_properties(init_props_ptr, out_props_ptr);

   return result;
}



capi_v2_err_t capi_v2_custom_dummy_qm_ecns_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr)
{

   capi_v2_err_t result;

   result = capi_v2_custom_dummy_ecns_get_static_properties(init_props_ptr, out_props_ptr);

   return result;
}



capi_v2_err_t capi_v2_custom_dummy_ecns_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr)
{

   capi_v2_err_t result;

   // Implement different static properties for different mic use cases in the case.
   result = capi_v2_voice_dummy_ecns_get_static_properties(init_props_ptr, out_props_ptr);

   return result;
}


/*------------------------------------------------------------------------------
 * This function updates the static property information needed by module
 * Mandatory properties to be set
	- CAPI_V2_INIT_MEMORY_REQUIREMENT
    - CAPI_V2_IS_INPLACE:
	- CAPI_V2_REQUIRES_DATA_BUFFERING
	- CAPI_V2_STACK_SIZE
	- CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS
	- CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS
	- CAPI_V2_CUSTOM_PROPERTY

 *----------------------------------------------------------------------------*/
capi_v2_err_t capi_v2_voice_dummy_ecns_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr)
{
   // out_props_ptr cannot be NULL as we need to know the prop requested
   if (NULL == out_props_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: CAPI V2 get_static_properties() FAILED received bad property pointer !!, %p", out_props_ptr);
      return CAPI_V2_EFAILED;
   }

   uint16_t i;
   capi_v2_prop_t *prop_ptr = out_props_ptr->prop_ptr;

   for (i = 0; i < out_props_ptr->props_num; i++)
   {
      capi_v2_buf_t *payload = &prop_ptr[i].payload;

      switch (prop_ptr[i].id)
      {
         case CAPI_V2_INIT_MEMORY_REQUIREMENT:
            {
               if (payload->max_data_len >= sizeof(capi_v2_init_memory_requirement_t))
               {
                  capi_v2_init_memory_requirement_t *data_ptr = (capi_v2_init_memory_requirement_t*) payload->data_ptr;
                  data_ptr->size_in_bytes = sizeof(capi_v2_dummy_ecns_t);  // Specify the module memory requirement here. This memory is allocated by framework
                  payload->actual_data_len = sizeof(capi_v2_init_memory_requirement_t);
               } else
               {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: CAPI V2 get_static_properties() FAILED Get Property id 0x%lx Bad param size %lu", (uint32_t) prop_ptr[i].id,
                        payload->max_data_len);
                  return CAPI_V2_ENEEDMORE;
               }
               break;
            }
         case CAPI_V2_IS_INPLACE:
            {
               if (payload->max_data_len >= sizeof(capi_v2_is_inplace_t))
               {
                  capi_v2_is_inplace_t *data_ptr = (capi_v2_is_inplace_t*) payload->data_ptr;
                  data_ptr->is_inplace = FALSE;		// No need to change inplace computing for ECNS module because input and output buffers are different
                  payload->actual_data_len = sizeof(capi_v2_is_inplace_t);
               } else
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! CAPI V2 Get Property id 0x%lx Bad param size %lu", (uint32_t) prop_ptr[i].id, payload->max_data_len);
                  return CAPI_V2_ENEEDMORE;
               }
               break;
            }
         case CAPI_V2_REQUIRES_DATA_BUFFERING:
            {
               if (payload->max_data_len >= sizeof(capi_v2_requires_data_buffering_t))
               {
                  capi_v2_requires_data_buffering_t *data_ptr = (capi_v2_requires_data_buffering_t*) payload->data_ptr;
                  data_ptr->requires_data_buffering = FALSE; // Depending on whether the module needs data buffering to be handled by framework, set this to TRUE/FALSE
                  payload->actual_data_len = sizeof(capi_v2_requires_data_buffering_t);
               } else
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! CAPI V2 Get Property id 0x%lx Bad param size %lu", (uint32_t) prop_ptr[i].id, payload->max_data_len);
                  return CAPI_V2_ENEEDMORE;
               }
               break;
            }
         case CAPI_V2_STACK_SIZE:
            {
               if (payload->max_data_len >= sizeof(capi_v2_stack_size_t))
               {
                  capi_v2_stack_size_t *data_ptr = (capi_v2_stack_size_t*) payload->data_ptr;
                  data_ptr->size_in_bytes = 1024; // Worst case stack size
				  payload->actual_data_len = sizeof(capi_v2_stack_size_t);
               } else
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! CAPI V2 Get Property id 0x%lx Bad param size %lu", (uint32_t) prop_ptr[i].id, payload->max_data_len);
                  return CAPI_V2_ENEEDMORE;
               }

               break;
            }
         case CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS:
            {
               if (payload->max_data_len >= sizeof(capi_v2_num_needed_framework_extensions_t))
               {
                  capi_v2_num_needed_framework_extensions_t *data_ptr = (capi_v2_num_needed_framework_extensions_t *) payload->data_ptr;
                  data_ptr->num_extensions = 1; // Specify the number of framework extensions needed by ECNS
                  payload->actual_data_len = sizeof(capi_v2_num_needed_framework_extensions_t);
               } else
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! CAPI V2 Get Property id 0x%lx Bad param size %lu", (uint32_t) prop_ptr[i].id, payload->max_data_len);
                  return CAPI_V2_ENEEDMORE;
               }
               break;
            }

         case CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS:
            {
               if (payload->max_data_len >= (sizeof(capi_v2_framework_extension_id_t)))
               {
                  capi_v2_framework_extension_id_t *data_ptr = (capi_v2_framework_extension_id_t *) payload->data_ptr;
                  data_ptr[0].id = FWK_EXTN_ECNS; // Define the ID of framework extensions needed by module.
                  payload->actual_data_len = sizeof(capi_v2_framework_extension_id_t);
               } else
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! CAPI V2 Get Property id 0x%lx Bad param size %lu", (uint32_t) prop_ptr[i].id, payload->max_data_len);
                  return CAPI_V2_ENEEDMORE;
               }
               break;
            }
         case CAPI_V2_CUSTOM_PROPERTY:
            {
               capi_v2_get_ecns_property_t *ecns_custom_prop = (capi_v2_get_ecns_property_t *) payload->data_ptr;

               switch (ecns_custom_prop->sec_property_id)
               {
                  case CAPI_V2_PROPERTY_ID_ECNS_OUTPUT_CAPABILITIES:
                     {
                        ecns_output_capabilities_t *ecns_output_info = (ecns_output_capabilities_t*) ecns_custom_prop->ecns_info;
                        ecns_output_info->output_lec = FALSE;
                                      // Set this to TRUE to update framework that module provides Linear Echo Canceller output.
                                      // NUM_OUTPUT_PORTS need to be incremented by 1, if it is true.
                        ecns_output_info->output_nr = FALSE;
                                      // Set this to TRUE to update framework that module provides Noise Reference output.
                                      // NUM_OUTPUT_PORTS need to be incremented by 1 if it is true.
                        payload->actual_data_len = sizeof(capi_v2_get_ecns_property_t);
                        break;
                     }
                  case CAPI_V2_PROPERTY_ID_ECNS_MONITORING_CAPABILITIES:
                     {
                        ecns_monitoring_capabilities_t *ecns_monitoring_info =  (ecns_monitoring_capabilities_t*)ecns_custom_prop->ecns_info;
                        ecns_monitoring_info->is_rtm_supported=FALSE; // Set TRUE if module has real time monitoring capabilities
                        payload->actual_data_len =  sizeof(capi_v2_get_ecns_property_t);
                        break;
                     }
                  case CAPI_V2_PROPERTY_ID_ECNS_VP3_CAPABILITIES:
                     {
                        ecns_vp3_capabilities_t *ecns_vp3_info =  (ecns_vp3_capabilities_t*)ecns_custom_prop->ecns_info;
                        ecns_vp3_info->is_vp3_supported=FALSE; // Set to TRUE if module supports VP3
                        payload->actual_data_len =  sizeof(capi_v2_get_ecns_property_t);
                        break;
                     }
                  case CAPI_V2_PROPERTY_ID_ECNS_RATE_MATCHING_CAPABILITIES:
                     {
                        ecns_rate_matching_capabilities_t *ecns_rate_matching_info =  (ecns_rate_matching_capabilities_t*)ecns_custom_prop->ecns_info;
                        ecns_rate_matching_info->is_rate_matching_supported=FALSE; // Set to TRUE if module supports rate matching/sample slip

                        payload->actual_data_len =  sizeof(capi_v2_get_ecns_property_t);
                        break;
                     }
                  case CAPI_V2_PROPERTY_ID_ECNS_STT_CAPABILITIES:
				   {
					  ecns_stt_capabilities_t *ecns_stt_info =  (ecns_stt_capabilities_t*)ecns_custom_prop->ecns_info;
					  ecns_stt_info->is_stt_supported=FALSE;
					  payload->actual_data_len =  sizeof(capi_v2_get_ecns_property_t);
					  break;
				   }
                  default:
                     {
                        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: ERROR !! CAPI V2 Invalid CUSTOM property id %lx", ecns_custom_prop->sec_property_id);
                        break;
                     }
               }
               break;
            }

         default:
            {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: CAPI V2 get_static_properties() FAILED Get Property for 0x%x. Not supported.", prop_ptr[i].id);
               return CAPI_V2_EBADPARAM;
            }
      }
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: CAPI V2 get_static_properties() Get Property for 0x%x done", prop_ptr[i].id);
   }
   return CAPI_V2_EOK;
}

/*------------------------------------------------------------------------------
 * This function is used init the CAPIv2 lib.
 *
 * @param[in] _pif, pointer to the CAPIv2 lib.
 * @param[in] init_set_properties, pointer to the prop list that needs to be init'ed.
 *
 * @return capi_v2_err_t, result code
 *----------------------------------------------------------------------------*/

capi_v2_err_t capi_v2_custom_dummy_sm_ecns_init(capi_v2_t* _pif, capi_v2_proplist_t *init_set_properties)
{
   capi_v2_err_t result;

   if (NULL == _pif)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: CAPI V2 Init FAILED received bad pointer");
      return CAPI_V2_EFAILED;
   }
   capi_v2_dummy_ecns_t *me = (capi_v2_dummy_ecns_t *) _pif;

   // Clean the structure first
   memset(me, 0, sizeof(capi_v2_dummy_ecns_t));
   me->enable = 0 ; // Disable module by default
   //me->module_id = VOICE_MODULE_DUMMY_SM_ECNS;
   me->param_id_1 = 0;

   me->num_mic = SINGLE_MIC; // set the number of mics/channels required by the module.
                             // Here, for ECNS, number of channels are assumed as equal to number of mics.
                             // Later you can extract the number of channels from voice framework
                             // and compare with required channels by module.

   result=capi_v2_custom_dummy_ecns_init(_pif, init_set_properties);

   return result;

}

capi_v2_err_t capi_v2_custom_dummy_dm_ecns_init(capi_v2_t* _pif, capi_v2_proplist_t *init_set_properties)
{
   capi_v2_err_t result;

   if (NULL == _pif)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: CAPI V2 Init FAILED received bad pointer");
      return CAPI_V2_EFAILED;
   }
   capi_v2_dummy_ecns_t *me = (capi_v2_dummy_ecns_t *) _pif;

   // Clean the structure first
   memset(me, 0, sizeof(capi_v2_dummy_ecns_t));
   me->enable = 0 ; // Disable module by default
   //me->module_id = VOICE_MODULE_DUMMY_DM_ECNS;
   me->param_id_1 = 0;

   me->num_mic = DUAL_MIC;   // set the number of mics/channels required by the module.
                             // Here, for ECNS, number of channels are assumed as equal to number of mics.
                             // Later you can extract the number of channels from voice framework
                             // and compare with required channels by module.
   result=capi_v2_custom_dummy_ecns_init(_pif, init_set_properties);

   return result;

}

capi_v2_err_t capi_v2_custom_dummy_qm_ecns_init(capi_v2_t* _pif, capi_v2_proplist_t *init_set_properties)
{
   capi_v2_err_t result;

   if (NULL == _pif)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: CAPI V2 Init FAILED received bad pointer");
      return CAPI_V2_EFAILED;
   }
   capi_v2_dummy_ecns_t *me = (capi_v2_dummy_ecns_t *) _pif;

   // Clean the structure first
   memset(me, 0, sizeof(capi_v2_dummy_ecns_t));
   me->enable = 0 ; // Disable module by default
   //me->module_id = VOICE_MODULE_DUMMY_QM_ECNS;
   me->param_id_1 = 0;

   me->num_mic = QUAD_MIC;   // set the number of mics/channels required by the module.
                             // Here, for ECNS, number of channels are assumed as equal to number of mics.
                             // Later you can extract the number of channels from voice framework
                             // and compare with required channels by module.
   result=capi_v2_custom_dummy_ecns_init(_pif, init_set_properties);

   return result;

}




capi_v2_err_t capi_v2_custom_dummy_ecns_init(capi_v2_t* _pif, capi_v2_proplist_t *init_set_properties)
{
   capi_v2_err_t result;

   if (NULL == _pif)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: CAPI V2 Init FAILED received bad pointer");
      return CAPI_V2_EFAILED;
   }

   capi_v2_dummy_ecns_t *me = (capi_v2_dummy_ecns_t *) _pif;

   result=capi_v2_voice_dummy_ecns_init(me, init_set_properties);

   dummy_ecns_lib_init(); //initialize core library accordingly, here it is just single dummy call for all mic scenarios

   return result;

}


/*------------------------------------------------------------------------------
 * This function initializes the static property information for the module
 * Following properties are set by framework and should be handled by module.
	- CAPI_V2_INPUT_MEDIA_FORMAT
    - CAPI_V2_EVENT_CALLBACK_INFO
	- CAPI_V2_PORT_NUM_INFO
	- CAPI_V2_HEAP_ID
 *----------------------------------------------------------------------------*/
capi_v2_err_t capi_v2_voice_dummy_ecns_init(capi_v2_dummy_ecns_t *me, capi_v2_proplist_t *init_set_properties)
{

   uint32_t i, result = 0;
   uint32_t event_cb_idx = -1;
   capi_v2_event_cb_f cb_func =NULL;
   void* cb_context = NULL;

   if (NULL == me)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: CAPI V2 Init FAILED received bad pointer");
      return CAPI_V2_EFAILED;
   }

   if (NULL == init_set_properties || NULL == init_set_properties->prop_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: CAPI V2 Init FAILED received bad pointer for init props");
      return CAPI_V2_EFAILED;
   }

   /* Set the vtable to allow the processing */
   me->vtbl = &vtbl;

   capi_v2_prop_t *prop_ptr = init_set_properties->prop_ptr;

   for (i = 0; i < init_set_properties->props_num; i++)
   {
      capi_v2_buf_t *payload = &prop_ptr[i].payload;
      switch (uint32_t(prop_ptr[i].id))
      {
         case CAPI_V2_INPUT_MEDIA_FORMAT:
            {
               capi_data_format_struct_t *dummy_ecns_data_format_ptr = (capi_data_format_struct_t*) payload->data_ptr;
               for (uint32_t i = 0; i < me->num_port_info.num_input_ports; i++)
               {
                  // Payload can be no smaller than the header for media format
                  if (payload->actual_data_len != sizeof(capi_data_format_struct_t) * me->num_port_info.num_input_ports)
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! Size mismatch for input format property, got %lu", payload->actual_data_len);
                     //bail out
                     return CAPI_V2_EBADPARAM;
                  }

                  if (CAPI_V2_FIXED_POINT != dummy_ecns_data_format_ptr[i].media_format.format_header.data_format)
                  {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! unsupported media format %d",
                           (int) dummy_ecns_data_format_ptr[i].media_format.format_header.data_format);
                     return CAPI_V2_EBADPARAM;
                  }
                  capi_v2_standard_data_format_t *pcm_format_ptr = &dummy_ecns_data_format_ptr[i].data_format;
                  // We only care about number of channels and sampling rate
                  if (((NB_SAMPLING_RATE != pcm_format_ptr->sampling_rate) && (WB_SAMPLING_RATE != pcm_format_ptr->sampling_rate)
                           && (FB_SAMPLING_RATE != pcm_format_ptr->sampling_rate)) || (16 != pcm_format_ptr->bits_per_sample))
                  {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! invalid format chan %lu, SR %lu, bits per samp %lu", pcm_format_ptr->num_channels,
                           pcm_format_ptr->sampling_rate, pcm_format_ptr->bits_per_sample);
                     return CAPI_V2_EBADPARAM;
                  }
                  switch (dummy_ecns_data_format_ptr[i].data_format.channel_type[0])
                  {
                     case PCM_CHANNEL_PRI_MIC:
                     case PCM_CHANNEL_SEC_MIC_1:
                     case PCM_CHANNEL_SEC_MIC_2:
                     case PCM_CHANNEL_SEC_MIC_3:
                        {
                           me->primary_mic_idx = me->secondary_mic1_idx = INVALID;
                           me->secondary_mic2_idx = me->secondary_mic3_idx = INVALID;

                           // Extract the number of channels from voice framework and compare with required channels by module.
                           // Here, for ECNS, number of channels are assumed as equal to number of mics.
                           // Fail if, num channels supported by module is different from num channels sent by framework
                    	   if (me->num_mic != dummy_ecns_data_format_ptr[i].data_format.num_channels)
						   {
						         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! Invalid number of channels (%ld) for primary port required by the module (%ld)",
													 dummy_ecns_data_format_ptr[i].data_format.num_channels, me->num_mic);
					             return CAPI_V2_EBADPARAM;
							}

                           switch (me->num_mic)
                           {
                              case SINGLE_MIC:
                              	 {
										// Set the primary mic index to 0 for Single mic topology
										// Use index i for secondary mic
										me->primary_mic_idx = 0;
										me->pri_num_channels = SM_NEAR_PRI_NUM_CHANNELS;
										break;
                              	 }
                              case DUAL_MIC:
                                 {
                                    me->pri_num_channels = DM_PRI_NUM_CHANNELS;
                                    if (PCM_CHANNEL_PRI_MIC == dummy_ecns_data_format_ptr[i].data_format.channel_type[0])
                                    {
                                       me->primary_mic_idx = 0;
                                       me->secondary_mic1_idx =1;
                                    }
                                    else
                                    {
                                       me->primary_mic_idx = 1;
                                       me->secondary_mic1_idx =0;
                                    }
                                    break;
                                 }
                              case QUAD_MIC:
                                 {
                                    me->pri_num_channels = QM_PRI_NUM_CHANNELS;
                                    for ( uint32_t j = 0; j< QM_PRI_NUM_CHANNELS; j++)
                                    {
                                       switch (dummy_ecns_data_format_ptr[i].data_format.channel_type[j])
                                       {
                                          case PCM_CHANNEL_PRI_MIC:
                                             me->primary_mic_idx = j;
                                             break;
                                          case PCM_CHANNEL_SEC_MIC_1:
                                             me->secondary_mic1_idx = j;
                                             break;
                                          case PCM_CHANNEL_SEC_MIC_2:
                                             me->secondary_mic2_idx = j;
                                             break;
                                          case PCM_CHANNEL_SEC_MIC_3:
                                             me->secondary_mic3_idx = j;
                                             break;
                                          default:
                                             {
                                                MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: ERROR !!! wrong channel type for QM DUMMY ECNS Primary Port Config");
                                                return CAPI_V2_EBADPARAM;
                                             }

                                       }
                                    }
                                    break;
                                 }
                              default:
                                 {
                                	 MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: ERROR !!! unrecognized topology for Primary Port Config %ld", me->num_mic);
                                    return CAPI_V2_EBADPARAM;
                                 }
                           }
                           me->primary_input_port_idx = i;
                           MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: NEAR PORT at %d", i);
                           break;
                        }
                     case PCM_CHANNEL_FAR_REF:
                        {
                           // Fail if, num channels supported by module is different from num channels sent by framework
                           if (FAR_NUM_CHANNELS != dummy_ecns_data_format_ptr[i].data_format.num_channels)
                           {
                             MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! invalid number of channels (%ld) for secondary port ",
                                    dummy_ecns_data_format_ptr[i].data_format.num_channels);
                              return CAPI_V2_EBADPARAM;
                           }
						   // Set far input port index
                           me->far_input_port_idx = i;
                           MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: FAR PORT at %d", i);
                           break;
                        }
                     default:
                        {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: ERROR !!! unrecognized input channel type");
                           break;
                        }
                  }
                  me->input_media_type[i] = dummy_ecns_data_format_ptr[i]; // saving the port media type information
               }

               me->sampling_rate = dummy_ecns_data_format_ptr[0].data_format.sampling_rate;
               me->output_framesize = (me->sampling_rate/1000)*20; // 20 msec frame size
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: CAPI V2 Sampling rate (%d), Output Framesize=(%ld), number of channels (%ld) for primary port", me->sampling_rate,me->output_framesize, me->num_mic);

               // Here, ECNS might have different output media format than the input media format.
               // So, output media format is provided to the framework by the module
               // using an event call back during CAPI_V2_INPUT_MEDIA_FORMAT
               capi_v2_dummy_ecns_send_output_mediatype(me);

               break;
            }
         case CAPI_V2_EVENT_CALLBACK_INFO:
            {
			   // Set the event callback pointer
			   // This is required so that module can raise events to framework later.
			   capi_v2_event_callback_info_t *cb_info_ptr = (capi_v2_event_callback_info_t*) payload->data_ptr;
               event_cb_idx = i;
               cb_func = me->event_cb = cb_info_ptr->event_cb;
                              //function pointer for event callback
               cb_context = me->event_context = cb_info_ptr->event_context;
                              // state required for event callback

               break;
            }
         case CAPI_V2_PORT_NUM_INFO:
            {
               capi_v2_port_num_info_t *num_port_info = (capi_v2_port_num_info_t*) payload->data_ptr;
               if ((num_port_info->num_input_ports != NUM_INPUT_PORTS) || (num_port_info->num_output_ports != NUM_OUTPUT_PORTS))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! incorrect number of input ( %lu) or output ( %lu) ports", num_port_info->num_input_ports,
                        num_port_info->num_output_ports);
                  //bail out
                  return CAPI_V2_EBADPARAM;
               }
			   // Update module about port information
               me->num_port_info = *num_port_info;
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: CAPI V2 received port info");

               break;

            }
         case CAPI_V2_HEAP_ID:
            {
               if (sizeof(capi_v2_heap_id_t) != payload->actual_data_len)
               {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: CAPI V2 received bad size(%ld) for heap id property", payload->actual_data_len);
                  return CAPI_V2_EFAILED;
               }
			   // Update heap ID to module
               capi_v2_heap_id_t* heap_id_ptr = (capi_v2_heap_id_t*) payload->data_ptr;
               memscpy(&me->heap_id, sizeof(me->heap_id), heap_id_ptr, payload->actual_data_len);
               break;
            }
         default:
            {
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: Unsupported prop_id 0x%lx", (uint32_t) prop_ptr[i].id);
               return CAPI_V2_EOK;
            }
      }
   }
   return result;

}

/*------------------------------------------------------------------------------
 * This function is responsible for processing of data.
 *
 * @param[in] _pif, pointer to the CAPI lib.
 * @param[in] input, pointer to input data buffer.
 * @param[in] output, pointer to output data buffer.
 *
 * @return capi_v2_err_t, result code
 * It does passthrough of primary input port data to output port.
 *----------------------------------------------------------------------------*/

static capi_v2_err_t capi_v2_dummy_ecns_process(capi_v2_t* _pif, capi_v2_stream_data_t* input[], capi_v2_stream_data_t* output[])
{
   int16 *ec_out_ptr;
   capi_v2_dummy_ecns_t *me = (capi_v2_dummy_ecns_t *) _pif;


   int16 *ptr_Tx_Input[4] = {(int16*) (input[me->primary_input_port_idx]->buf_ptr[me->primary_mic_idx].data_ptr), NULL, NULL, NULL};
   int16 *ptr_Rx_Input[4] = {(int16*) (input[me->far_input_port_idx]->buf_ptr[0].data_ptr), NULL, NULL, NULL};
   uint32_t nSamples_near = input[me->primary_input_port_idx]->buf_ptr[me->primary_mic_idx].actual_data_len >> 1;
   uint32_t nSamples_far = input[me->far_input_port_idx]->buf_ptr[0].actual_data_len>>1;
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: Invoking Process Samples %d, max_size %d, actual_size %d",nSamples_near,input[0]->buf_ptr[0].max_data_len, input[0]->buf_ptr[0].actual_data_len);
   if ( SM_NEAR_PRI_NUM_CHANNELS != me->pri_num_channels)
   {
      ptr_Tx_Input[1] = (int16*) (input[me->primary_input_port_idx]->buf_ptr[me->secondary_mic1_idx].data_ptr);
      if(QM_PRI_NUM_CHANNELS == me->pri_num_channels)
      {
         ptr_Tx_Input[2] = (int16*) (input[me->primary_input_port_idx]->buf_ptr[me->secondary_mic2_idx].data_ptr);
         ptr_Tx_Input[3] = (int16*) (input[me->primary_input_port_idx]->buf_ptr[me->secondary_mic3_idx].data_ptr);
      }
   }

   ec_out_ptr = (int16 *) output[me->primary_output_port_idx]->buf_ptr[0].data_ptr;
   output[me->primary_output_port_idx]->buf_ptr[0].actual_data_len = me->output_framesize<<1;  //setting up primary output size
   uint32_t near_ch0_bufsize = input[me->primary_input_port_idx]->buf_ptr[0].max_data_len;

   if((me->enable))
   {
	   if(ec_out_ptr!=ptr_Tx_Input[0])
		{
		  dummy_ecns_lib_process(ec_out_ptr, near_ch0_bufsize, ptr_Tx_Input[0], nSamples_near<<1, ptr_Rx_Input[0], nSamples_far);
		}
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: End of process function");
   return CAPI_V2_EOK;
}

/*------------------------------------------------------------------------------
 * This function returns the module to the uninitialized state and can be used
 * to free the memory that was allocated by module.
 * This function also frees the virtual function table.
 *----------------------------------------------------------------------------*/
capi_v2_err_t capi_v2_dummy_ecns_end(capi_v2_t* _pif)
{
   if (NULL == _pif)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: FAILED received bad property pointer");
      return CAPI_V2_EFAILED;
   }

   capi_v2_dummy_ecns_t *me = (capi_v2_dummy_ecns_t*) _pif;

   me->static_buf_ptr = NULL;
   me->param_struct_ptr = NULL;
   me->vtbl = NULL;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: CAPI_V2 End done");
   return CAPI_V2_EOK;
}

/*------------------------------------------------------------------------------
 * This function is responsible for setting parameters in module.
 *
 * @param[in] _pif, pointer to the CAPIv2 lib.
 * @param[in] param_id, parameter id sent from client processor.
 * @param[in] port_info_ptr, pointer to port.
 * @param[in] params_ptr, data pointer to parameters.
 *
 * @return capi_v2_err_t, result code
 *----------------------------------------------------------------------------*/

capi_v2_err_t capi_v2_dummy_ecns_set_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr)
{
   if (NULL == _pif || NULL == params_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: FAILED received bad property pointer for param_id property, 0x%lx", param_id);
      return CAPI_V2_EFAILED;
   }

   capi_v2_dummy_ecns_t *me = (capi_v2_dummy_ecns_t*) _pif;
   capi_v2_err_t nResult = CAPI_V2_EOK;
   switch (param_id)
   {
      case VOICE_PARAM_MOD_ENABLE:
         {
            int16_t nEnableFlag = *((int16_t*) params_ptr->data_ptr);
            if (sizeof(int32_t) != params_ptr->actual_data_len)
            {
              MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: voice DUMMYECNS set_param :: Bad Param Size");
               nResult = CAPI_V2_EBADPARAM;
               break;
            }
            if (me->enable != nEnableFlag)
            {

               me->enable = (int32_t) nEnableFlag;
               if (TRUE == nEnableFlag)
               {
                  me->params_modified = TRUE;
               }
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: DUMMYECNS enable(%ld)",me->enable);
            }

            break;
         }
	  case DUMMY_ECNS_PARAM_ID_1:
	     {
			int16_t param_1 = *((int16_t*) params_ptr->data_ptr);
            if (sizeof(int32_t) != params_ptr->actual_data_len)
            {
              MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: voice DUMMY ECNS set_param :: Bad Param Size for param id = %x ", DUMMY_ECNS_PARAM_ID_1);
              nResult = CAPI_V2_EBADPARAM;
              break;
            }

			me->param_id_1 = (uint32_t) param_1;
		 }
      default:
         {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: voice DUMMY ECNS set_param :: Param ID not supported");
            nResult = CAPI_V2_EBADPARAM;
            break;
         }
   }
   uint32_t new_kpps = 0, new_delay = 0;

   if (me->enable)
   {
      capi_v2_dummy_ecns_get_kpps_delay(me,&new_delay,&new_kpps);
   }

   if (new_kpps != me->kpps)
   {
      me->kpps = new_kpps;
      capi_v2_dummy_ecns_send_kpps(me);
   }

   if (new_delay != me->delay)
   {
      me->delay = new_delay;
      capi_v2_dummy_ecns_send_delay(me);
   }

   return nResult;
}

/*------------------------------------------------------------------------------
 * This function is responsible for getting parameters of module.
 *
 * @param[in] _pif, pointer to the CAPIv2 lib.
 * @param[in] param_id, parameter id sent from client processor.
 * @param[in] port_info_ptr, pointer to port.
 * @param[in] params_ptr, data pointer to parameters to be written by module.
 *
 * @return capi_v2_err_t, result code
 *----------------------------------------------------------------------------*/

capi_v2_err_t capi_v2_dummy_ecns_get_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr)
{
   if (NULL == _pif || NULL == params_ptr || NULL == params_ptr->data_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: FAILED received bad property pointer for param_id property, 0x%lx", param_id);
      return CAPI_V2_EFAILED;
   }
   uint32_t nBufferSize = params_ptr->max_data_len;
   uint16_t* nParamSize = (uint16_t*) &(params_ptr->actual_data_len);
   int8_t* pParamsBuffer = params_ptr->data_ptr;
   capi_v2_err_t nResult = CAPI_V2_EOK;

   capi_v2_dummy_ecns_t *me = (capi_v2_dummy_ecns_t*) _pif;

   switch (param_id)
   {
      case VOICE_PARAM_MOD_ENABLE:
         {
            *nParamSize = sizeof(int32_t);
            if (*nParamSize > nBufferSize)
            {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: voice DUMMYECNS get_param :: on/off required size = %u, given size = %ld", *nParamSize, nBufferSize);
               return CAPI_V2_ENOMEMORY;
            }
            *((int32_t*) pParamsBuffer) = 0;  // Clearing the whole buffer
            *((int16_t*) pParamsBuffer) = (int16_t) me->enable;
            break;
         }
	  case  DUMMY_ECNS_PARAM_ID_1:
	     {
	     *nParamSize = sizeof(int32_t);
          if (*nParamSize > nBufferSize)
            {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: DUMMY_ECNS_PARAM_ID_1 get_param :: on/off required size = %u, given size = %ld", *nParamSize, nBufferSize);
               return CAPI_V2_ENOMEMORY;
            }
            *((int16_t*) pParamsBuffer) = (int16_t) me->param_id_1;
            break;
	     }
      default:
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: voice DUMMY ECNS get_param :: Param ID not supported");
            return CAPI_V2_EUNSUPPORTED;
            break;
         }
   }
   return nResult;
}

/*------------------------------------------------------------------------------
 * This function is responsible for setting properties to module.
 *
 * @param[in] _pif, pointer to the CAPIv2 lib
 * @param[in] props_ptr, pointer to various properties
 *
 * @return capi_v2_err_t, result code
 *----------------------------------------------------------------------------*/

static capi_v2_err_t capi_v2_dummy_ecns_set_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr)
{
   if (!_pif || !props_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! Received bad pointer in set_properties");
      return CAPI_V2_EFAILED;
   }

   capi_v2_dummy_ecns_t *me = (capi_v2_dummy_ecns_t*) _pif;
   uint32_t i;
   for (i = 0; i < props_ptr->props_num; i++)
   {
      capi_v2_prop_t* current_prop_ptr = &(props_ptr->prop_ptr[i]);
      capi_v2_buf_t* payload_ptr = &(current_prop_ptr->payload);
      switch (current_prop_ptr->id)
      {
         case CAPI_V2_EVENT_CALLBACK_INFO:
            {
              // Set the event callback pointer
              // This is required so that module can raise events to framework later.
               capi_v2_event_callback_info_t *cb_info_ptr = (capi_v2_event_callback_info_t*) payload_ptr->data_ptr;
               me->event_cb = cb_info_ptr->event_cb;
                              //function pointer for event callback
               me->event_context = cb_info_ptr->event_context;
                              // state required for event callback
               break;
            }
         case CAPI_V2_ALGORITHMIC_RESET:
            {
			   dummy_ecns_lib_reset(); // reset core processing library

               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DUMMY ECNS : Algorithm Reset done");
               break;
            }
         default:
            {
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: Unsupported prop ID %lx", current_prop_ptr->id);
               return CAPI_V2_EUNSUPPORTED;
            }
      }
   }
   return CAPI_V2_EOK;
}


static capi_v2_err_t capi_v2_dummy_ecns_get_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr)
{
   return CAPI_V2_EUNSUPPORTED;
}


/*------------------------------------------------------------------------------
 * This function is responsible for updating the kpps to caller service using
 * event callback functionality.
 *
 * @param[in] me, Module pointer.
 *
 *----------------------------------------------------------------------------*/

void capi_v2_dummy_ecns_send_kpps(capi_v2_dummy_ecns_t* me)
{
   capi_v2_err_t result = CAPI_V2_EOK;
   capi_v2_event_info_t kpps_event;
   capi_v2_event_KPPS_t kpps_payload;

   kpps_payload.KPPS = me->kpps;
   kpps_event.payload.data_ptr = (int8_t*) &kpps_payload;
   kpps_event.payload.actual_data_len = kpps_event.payload.max_data_len = sizeof(kpps_payload);

   kpps_event.port_info.is_valid = FALSE;

   result = me->event_cb(me->event_context, CAPI_V2_EVENT_KPPS, &kpps_event);

   if (result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! kpps reporting failed, error %ld", result);
   }

}

/*------------------------------------------------------------------------------
 * This function is responsible for updating the algorithm delay to caller
 * service using event callback functionality.
 *
 * @param[in] me, Module pointer.
 *
 *----------------------------------------------------------------------------*/

void capi_v2_dummy_ecns_send_delay(capi_v2_dummy_ecns_t* me)
{
   capi_v2_err_t result = CAPI_V2_EOK;
   capi_v2_event_info_t algorithmic_delay_event;
   capi_v2_event_algorithmic_delay_t algorithmic_delay_payload;

   algorithmic_delay_payload.delay_in_us = me->delay;
   algorithmic_delay_event.payload.data_ptr = (int8_t*) &algorithmic_delay_payload;
   algorithmic_delay_event.payload.actual_data_len = algorithmic_delay_event.payload.max_data_len = sizeof(algorithmic_delay_payload);

   algorithmic_delay_event.port_info.is_valid = FALSE;

   result = me->event_cb(me->event_context, CAPI_V2_EVENT_ALGORITHMIC_DELAY, &algorithmic_delay_event);

   if (result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Error! algorithmic_delay reporting failed, error %ld", result);
   }
}

/*------------------------------------------------------------------------------
 * This function is responsible for updating the module about algorithm delay
 * and kpps depending on topology id and sampling rate
 *
 * @param[in] me, Module pointer.
 * @param[in] delay_value, pointer to store delay.
 * @param[in] kpps_value, pointer to store kpps.
 *
 *----------------------------------------------------------------------------*/

void capi_v2_dummy_ecns_get_kpps_delay(capi_v2_dummy_ecns_t* me, uint32_t *delay_value, uint32_t *kpps_value)
{
   uint32_t kpps = 0,delay =0;
   if (me->enable)
   {
      switch (me->num_mic)
      {
         // Add proper kpps and delay values based on single, dual and quad mic requirements
		 // Here, it is clubbed in only one, as it is dummy ECNS example.
		 case SINGLE_MIC:
         case DUAL_MIC:
         case QUAD_MIC:
            {
               switch (me->sampling_rate)
               {
                  case NB_SAMPLING_RATE:
                     kpps = DUMMY_ECNS_NB_KPPS;
                     delay = DUMMY_ECNS_NB_DELAY;
                     break;
                  case WB_SAMPLING_RATE:
                     kpps = DUMMY_ECNS_WB_KPPS;
                     delay = DUMMY_ECNS_WB_DELAY;
                     break;
                  case FB_SAMPLING_RATE:
                     kpps = DUMMY_ECNS_FB_KPPS;
                     delay = DUMMY_ECNS_FB_DELAY;
                     break;
                  default:
                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: sampling rate  %ld not supported by DUMMY FV5", me->sampling_rate);
                     break;
               }
               break;
            }
      }
   }
   *delay_value = delay;
   *kpps_value = kpps;
   return;
}

/*------------------------------------------------------------------------------
 * This function is responsible for updating the output media format
 * The output media type is provided to the framework by the module using
 * an event callback which may be raised at any time and depends on the module.
 *
 * @param[in] me, Module pointer.
 *
 * @return capi_v2_err_t, result code
 *----------------------------------------------------------------------------*/
static capi_v2_err_t capi_v2_dummy_ecns_send_output_mediatype(capi_v2_dummy_ecns_t *me)
{
   capi_v2_err_t result = CAPI_V2_EOK;
   capi_v2_event_info_t process_event_info;

   if (NULL == me->event_context)
   {
      return CAPI_V2_EBADPARAM;
   }

   for (uint32_t i = 0; i < me->num_port_info.num_output_ports; i++)
   {
      me->output_media_type[i].data_format.bitstream_format = CAPI_V2_DATA_FORMAT_INVALID_VAL;
      me->output_media_type[i].data_format.num_channels = 1;
      me->output_media_type[i].data_format.bits_per_sample = 16;
      me->output_media_type[i].data_format.data_is_signed = TRUE;
      me->output_media_type[i].data_format.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
      me->output_media_type[i].data_format.q_factor = 15;
      me->output_media_type[i].media_format.format_header.data_format = CAPI_V2_FIXED_POINT;
   }

   me->output_media_type[0].data_format.channel_type[0] = PCM_CHANNEL_PRI_MIC;
   me->primary_output_port_idx = 0;

   me->output_media_type[0].data_format.sampling_rate = me->sampling_rate;

   process_event_info.port_info.is_valid = false;
   process_event_info.payload.actual_data_len = sizeof(me->output_media_type);
   process_event_info.payload.data_ptr = (int8_t *) &me->output_media_type;
   process_event_info.payload.max_data_len = sizeof(me->output_media_type);
   result = me->event_cb(me->event_context, CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED, &process_event_info);

   if (CAPI_V2_EOK != result)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DUMMY ECNS: Failed to raise event for output media type");
   } else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DUMMY ECNS: Raised event for output media type");
   }
   return result;
}

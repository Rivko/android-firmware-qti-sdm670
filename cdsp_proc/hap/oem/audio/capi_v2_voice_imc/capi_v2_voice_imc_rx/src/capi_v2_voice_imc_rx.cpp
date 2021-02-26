/*-----------------------------------------------------------------------
Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "capi_v2_voice_imc_calib.h"
#include "Elite_lib_get_imc.h"
#include "adsp_vparams_api.h"
#include "adsp_vparams_internal_api.h"
#include "shared_lib_api.h"

#include "capi_v2_voice_imc_rx.h"

// macros coming from VoiceCmnUtils.h
#define  VOICE_ROUNDTO8(x) ((((uint32_t)(x) + 7) >> 3) << 3)
#define VOICE_NB_SAMPLING_RATE      (8000)
#define VOICE_WB_SAMPLING_RATE      (16000)
#define VOICE_FB_SAMPLING_RATE      (48000)
#define VOICE_SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

// Enumeration for source and destination for IMC module
typedef enum
{
	IMC_SOURCE = 0,
	IMC_DESTINATION
}imc_src_dst_t;

/**< State information of the source or destination */
typedef enum imc_src_dst_state_t
{
	IMC_STATE_NONE = 0,
	IMC_STATE_REGISTERED,
	IMC_STATE_SOURCE_OPENED_COMMUNICATION,
	IMC_STATE_SOURCE_CLOSED_COMMUNICATION
}imc_src_dst_state_t;

/**< IMC source or destination information */
typedef struct imc_src_dst_info_t
{
	int32_t              key[4];             // Communication Id will be first 4 bytes of this array.
	uint32_t             key_length;         // length of the key.
	imc_src_dst_state_t  state;              // IMC state. Module knows if it is source or destination depending on the communication id.
	imc_dest_info_t      *dest_handle;       // destination handle for communication Id instance.
	imc_src_info_t       *source_handle;     // Source handle for communication Id instance. For a destination there can be multiple sources.
	elite_lib_get_imc_t  *imc_lib_handle;    // IMC library handle
	uint8_t              src_frame_num;      // Number of frames source tries to connect with destination
}imc_src_dst_info_t;

/*----------------------------------------------------------------------------
* Defines
* -------------------------------------------------------------------------*/
#define MAX_BUF_Q_ELEMENTS 2

/* KPPS values */
/* worst case profiled values */
#define VOICE_IMC_RX_NB_KPPS  100
#define VOICE_IMC_RX_WB_KPPS  200

/* Delay values*/
/* worst case profiled values */
#define VOICE_IMC_NB_DELAY_IN_US  (100)
#define VOICE_IMC_WB_DELAY_IN_US  (200)
#define VOICE_IMC_FB_DELAY_IN_US  (300)

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

// Structure for IMC RX module
typedef struct capi_v2_voice_imc_rx_t
{
	const capi_v2_vtbl_t *vtbl;
	uint32_t				enable;			  // Enable flag for module
	uint32_t				sampling_rate;    // Sampling rate
	uint32_t             kpps;             // kilo packets per second
	uint32_t             delay;            // delay
	QURT_ELITE_HEAP_ID   heap_id;          // heap_id
	capi_v2_event_cb_f   event_cb;         // Callback function to issue to framework to report delay and kpps
	void*                event_context;    // Event context that must be issued in the callback
	imc_src_dst_info_t   capi_v2_voice_imc_rx_imc_info; //only one instance of the communication is supported. Tx <-> Rx
	capi_v2_session_identifier_t session_identify; // For mutiple instances of TX/RX module, session identifier is used to identify correct instance.
}capi_v2_voice_imc_rx_t;

/*------------------------------------------------------------------------
* Static declarations
* -----------------------------------------------------------------------*/
static capi_v2_err_t capi_v2_voice_imc_rx_process(capi_v2_t* _pif, capi_v2_stream_data_t* input[], capi_v2_stream_data_t* output[]);
static capi_v2_err_t capi_v2_voice_imc_rx_end(capi_v2_t* _pif);
static capi_v2_err_t capi_v2_voice_imc_rx_set_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr);
static capi_v2_err_t capi_v2_voice_imc_rx_get_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr);
static capi_v2_err_t capi_v2_voice_imc_rx_set_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr);
static capi_v2_err_t capi_v2_voice_imc_rx_get_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr);

static uint32_t capi_v2_voice_imc_rx_get_kpps(capi_v2_voice_imc_rx_t* me);
static uint32_t capi_v2_voice_imc_rx_get_delay(capi_v2_voice_imc_rx_t* me);
static void capi_v2_voice_imc_rx_send_kpps(capi_v2_voice_imc_rx_t* me);
static void capi_v2_voice_imc_rx_send_delay(capi_v2_voice_imc_rx_t* me);
static void voice_imc_rx_recieve_buf(capi_v2_voice_imc_rx_t* me);
static void capi_v2_voice_imc_rx_get_imc_handle(capi_v2_voice_imc_rx_t* me);
static capi_v2_err_t capi_v2_voice_imc_register_src_dest(imc_src_dst_info_t *imc_src_dst_into, int8_t *key, uint32_t keylen,uint32_t max_bufq_len,uint32_t alloc_num_buf,uint32 buf_size, imc_src_dst_t src_dst);

static capi_v2_vtbl_t vtbl =
{
	capi_v2_voice_imc_rx_process,
	capi_v2_voice_imc_rx_end,
	capi_v2_voice_imc_rx_set_param,
	capi_v2_voice_imc_rx_get_param,
	capi_v2_voice_imc_rx_set_properties,
	capi_v2_voice_imc_rx_get_properties,
};

/*----------------------------------------------------------------------------
* Function Definitions
* -------------------------------------------------------------------------*/
/**
* This function is used to query the static properties to create the CAPIv2.
*
* @param[in] init_props_ptr, pointer to the initializing prop list
* @param[in, out] out_props_ptr, pointer to the output pro list
* @return capi_v2_err_t, result code
*/
capi_v2_err_t capi_v2_voice_imc_rx_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr)
{

	// Check if out_props_ptr is not null
	if( NULL == out_props_ptr )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_stat_prop(): Error! Received bad property pointer for get property");
		return CAPI_V2_EFAILED;
	}
	capi_v2_err_t result = CAPI_V2_EOK;
	uint32_t i;

	if(init_props_ptr)
	{
		for(i = 0; i < init_props_ptr->props_num; i++)
		{
			capi_v2_prop_t* current_prop_ptr = &(init_props_ptr->prop_ptr[i]);
			switch(current_prop_ptr->id)
			{
			default:
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_get_stat_prop(): Unsupported static property being set, %d", (int)current_prop_ptr->id);
					break;
				}
			}
		}
	}

	// now iterate over output properties that are being asked for by the framework
	for(i = 0; i < out_props_ptr->props_num; i++)
	{
		capi_v2_prop_t* current_prop_ptr = &(out_props_ptr->prop_ptr[i]);
		capi_v2_buf_t* payload_ptr = &(current_prop_ptr->payload);
		switch(current_prop_ptr->id)
		{
		case CAPI_V2_INIT_MEMORY_REQUIREMENT:
			{
				if(payload_ptr->max_data_len >= sizeof(capi_v2_init_memory_requirement_t))
				{
					capi_v2_init_memory_requirement_t *data_ptr = (capi_v2_init_memory_requirement_t *)payload_ptr->data_ptr;

					//Does not need a library
					data_ptr->size_in_bytes =  VOICE_ROUNDTO8(sizeof(capi_v2_voice_imc_rx_t)); // Specify the memory requirement for CAPIv2 Module only. 
					//For core processing library try to handle memory by internal memory management using heap ID sent from framework.
					payload_ptr->actual_data_len = sizeof(capi_v2_init_memory_requirement_t);
				}
				else
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
					return CAPI_V2_EBADPARAM;
				}
				break;
			}
		case CAPI_V2_IS_INPLACE:
			{
				if(payload_ptr->max_data_len >= sizeof(capi_v2_is_inplace_t))
				{
					capi_v2_is_inplace_t *data_ptr = (capi_v2_is_inplace_t*)payload_ptr->data_ptr;
					data_ptr->is_inplace = TRUE; // IMC modules have inplace computing.
					payload_ptr->actual_data_len = sizeof(capi_v2_is_inplace_t);
				}
				else
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
					return CAPI_V2_EBADPARAM;
				}
				break;
			}
		case CAPI_V2_REQUIRES_DATA_BUFFERING:
			{
				if(payload_ptr->max_data_len >= sizeof(capi_v2_requires_data_buffering_t))
				{
					capi_v2_requires_data_buffering_t *data_ptr = (capi_v2_requires_data_buffering_t*)payload_ptr->data_ptr;
					data_ptr->requires_data_buffering = FALSE; // Depending on whether module needs data buffering to be handled by framework, set this to TRUE/FALSE
					payload_ptr->actual_data_len = sizeof(capi_v2_requires_data_buffering_t);
				}
				else
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
					return CAPI_V2_EBADPARAM;
				}
				break;
			}
		case CAPI_V2_STACK_SIZE:
			{
				if(payload_ptr->max_data_len >= sizeof(capi_v2_stack_size_t))
				{
					capi_v2_stack_size_t *data_ptr = (capi_v2_stack_size_t*)payload_ptr->data_ptr;
					data_ptr->size_in_bytes = 1024; //Worst case profiled value. Can be profiled using hexagon-resourceanalyzer-gui.exe in hexagon-tools
					payload_ptr->actual_data_len = sizeof(capi_v2_stack_size_t);
				}
				else
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
					return CAPI_V2_EBADPARAM;
				}

				break;
			}
		case CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS:
			{
				if(payload_ptr->max_data_len >= sizeof(capi_v2_num_needed_framework_extensions_t))
				{
					capi_v2_num_needed_framework_extensions_t *data_ptr = (capi_v2_num_needed_framework_extensions_t *)payload_ptr->data_ptr;
					data_ptr->num_extensions = 0; // No framework extensions needed for IMC module
					payload_ptr->actual_data_len = sizeof(capi_v2_num_needed_framework_extensions_t);
				}
				else
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
					return CAPI_V2_EBADPARAM;
				}
				break;
			}
		case CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS:
			{
				if(payload_ptr->max_data_len >= sizeof(capi_v2_framework_extension_id_t))
				{
					payload_ptr->actual_data_len = 0;
				}
				else
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
					return CAPI_V2_EBADPARAM;
				}
				break;
			}
		default:
			{
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_stat_prop(): Unsupported prop_id 0x%lx", (uint32_t)current_prop_ptr->id);
				result=CAPI_V2_EUNSUPPORTED;
				break;
			}
		}
	}
	return result;
}

/**
* This function is used init the CAPIv2 lib.
*
* @param[in] _pif, pointer to the CAPIv2 lib.
* @param[in] init_set_properties, pointer to the prop list that needs to be init'ed.
*
* @return capi_v2_err_t, result code
*/

capi_v2_err_t capi_v2_voice_imc_rx_init(capi_v2_t* _pif, capi_v2_proplist_t *init_props_ptr)
{
	capi_v2_err_t result = CAPI_V2_EOK;

	if(NULL == _pif)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_init(): Error! received bad pointer");
		return CAPI_V2_EFAILED;
	}

	capi_v2_voice_imc_rx_t* me = (capi_v2_voice_imc_rx_t*)_pif;
	me->kpps = 0;
	me->delay = 0;
	me->session_identify.service_id = 0xFFFF;
	me->session_identify.session_id = 0xFFFF;  //initialize with 0xFFFF

	if(init_props_ptr)
	{
		// setting the properties sent by framework
		result=capi_v2_voice_imc_rx_set_properties(_pif,init_props_ptr);
		if(CAPI_V2_FAILED(result))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_init(): Error! Set properties failed");
			return result;
		}
	}

	// Point to vtbl
	me->vtbl = &vtbl;

	// Get the handle of IMC module. This also fetches the session identifier
	capi_v2_voice_imc_rx_get_imc_handle(me);

	
	// If session identifier is received and imc library is queried, register for IMC handle.
	if(me->session_identify.service_id != 0xFFFF &&
		me->session_identify.session_id != 0xFFFF &&
		(NULL != me->capi_v2_voice_imc_rx_imc_info.imc_lib_handle) )
	{
		//Key is COMMID | RX module ID | Tx module Id | sessionID
		
		me->capi_v2_voice_imc_rx_imc_info.key[0] = IMC_COMM_ID;
		me->capi_v2_voice_imc_rx_imc_info.key[1] = VOICE_IMC_RX; //IMC destination module ID
		me->capi_v2_voice_imc_rx_imc_info.key[2] = VOICE_IMC_TX; //IMC source module ID
		me->capi_v2_voice_imc_rx_imc_info.key[3] = (uint32_t)me->session_identify.session_id;
		imc_src_dst_info_t *comm_info_ptr;
		uint32_t param_size,max_payload_size,max_q_length,alloc_buffer;
		imc_src_dst_t src_dest;

		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_RX: imc_set_param(): key (%lx) (%lx) (%lx) (%lx)",
			me->capi_v2_voice_imc_rx_imc_info.key[0],
			me->capi_v2_voice_imc_rx_imc_info.key[1],
			me->capi_v2_voice_imc_rx_imc_info.key[2],
			me->capi_v2_voice_imc_rx_imc_info.key[3]);

		param_size = sizeof(me->capi_v2_voice_imc_rx_imc_info.key);
		comm_info_ptr = (imc_src_dst_info_t*)&me->capi_v2_voice_imc_rx_imc_info;
		src_dest = IMC_DESTINATION;
		max_q_length = alloc_buffer = MAX_BUF_Q_ELEMENTS;
		//Max size of all the payload + param Id
		max_payload_size = 0;

		// Register the soruce RX module
		capi_v2_err_t imc_result;
		imc_result = capi_v2_voice_imc_register_src_dest(comm_info_ptr,(int8_t*)me->capi_v2_voice_imc_rx_imc_info.key, param_size, max_q_length, alloc_buffer,max_payload_size,src_dest);
		if(CAPI_V2_FAILED(imc_result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_RX: diamondvoice_set_param(): failed to register to IMC. result(0x%lx)",imc_result);
			return imc_result;
		}
	}
	else
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_RX: ERROR in fetching IMC handle : service id = (%lx) \
											  session id  = (%lx)  IMC lib handle=(%lx) ",
											  me->session_identify.service_id,
											  me->session_identify.session_id,
											  me->capi_v2_voice_imc_rx_imc_info.imc_lib_handle);
	}

	return CAPI_V2_EOK;
}

/**
* This function is used to process the data. In case of IMC, it's recieving the data from IMC module
*
* @param[in] _pif, pointer to the CAPIv2 lib.
* @param[in] input, pointer to the input stream data.
* @param[in, out] output, pointer to the output stream data.
*
* @return capi_v2_err_t, result code
*/
static capi_v2_err_t capi_v2_voice_imc_rx_process(capi_v2_t* _pif, capi_v2_stream_data_t* input[], capi_v2_stream_data_t* output[])
{
	if((NULL == _pif) || (NULL== input) || (NULL== output) || (input[0]->bufs_num != 1))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_process(): Received NULL pointers in process");
		return CAPI_V2_EFAILED;
	}
	capi_v2_err_t result=CAPI_V2_EOK;

	capi_v2_voice_imc_rx_t* me = (capi_v2_voice_imc_rx_t*)_pif;

	// In case the module is enabled and supports non inplace computing, copy the data from input to output
	if(TRUE == me->enable)
	{
		if(input[0]->buf_ptr[0].data_ptr != output[0]->buf_ptr[0].data_ptr)
		{
			memscpy(output[0]->buf_ptr[0].data_ptr,output[0]->buf_ptr[0].max_data_len,input[0]->buf_ptr[0].data_ptr,input[0]->buf_ptr[0].actual_data_len);
			output[0]->buf_ptr[0].actual_data_len = input[0]->buf_ptr[0].actual_data_len;
		}
		// return result;
	}

	// If the state of RX module is registered, the recieve the buffer sent from TX module
	if(IMC_STATE_REGISTERED == me->capi_v2_voice_imc_rx_imc_info.state)
	{
		voice_imc_rx_recieve_buf(me);
	}
	return result;
}



/**
* This function is end of the CAPIv2.
*
* @param[in] _pif, pointer to the CAPIv2 lib.
*
*/
static capi_v2_err_t capi_v2_voice_imc_rx_end(capi_v2_t* _pif)
{
	if(NULL == _pif)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_end(): Error! Received bad pointer");
		return CAPI_V2_EFAILED;
	}
	capi_v2_voice_imc_rx_t* me = (capi_v2_voice_imc_rx_t*) _pif;

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_end(): end()");

	// De register IMC module
	if(IMC_STATE_NONE != me->capi_v2_voice_imc_rx_imc_info.state)
	{
		me->capi_v2_voice_imc_rx_imc_info.imc_lib_handle->vtable->dest_deregister(me->capi_v2_voice_imc_rx_imc_info.dest_handle);
		me->capi_v2_voice_imc_rx_imc_info.state = IMC_STATE_NONE;
	}

	me->capi_v2_voice_imc_rx_imc_info.imc_lib_handle->vtable->b.end(me->capi_v2_voice_imc_rx_imc_info.imc_lib_handle);

	return CAPI_V2_EOK;
}


/*
 * This function is responsible for setting parameters in module. 
 *
 * @param[in] _pif, pointer to the CAPI lib.
 * @param[in] param_id, parameter id sent from client processor.
 * @param[in] port_info_ptr, pointer to port.
 * @param[in] params_ptr, data pointer to parameters.
 *
 * @return capi_v2_err_t, result code
 */

static capi_v2_err_t capi_v2_voice_imc_rx_set_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr)
{
	if(NULL == _pif || NULL == params_ptr || NULL == params_ptr->data_ptr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_set_param(): Error! Received bad property pointer for set_param, 0x%lx", param_id);
		return CAPI_V2_EFAILED;
	}
	capi_v2_err_t result = CAPI_V2_EOK;
	capi_v2_voice_imc_rx_t* me = (capi_v2_voice_imc_rx_t*)_pif;

	int8* pParamsBuffer = params_ptr->data_ptr;

	switch(param_id)
	{

	case VOICE_PARAM_MOD_ENABLE:
		{
			int16_t *enable_ptr = (int16_t*) pParamsBuffer;

			if(TRUE == *enable_ptr)
			{
				me->enable = TRUE; // enabled
			}   

			break;
		}

	default:
		{
			return CAPI_V2_EBADPARAM;
		}
	}

	uint32_t new_kpps,new_delay;
	new_kpps = capi_v2_voice_imc_rx_get_kpps(me);

	if(new_kpps != me->kpps)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_set_param(): Voice voice_imc_rx kpps updated, old %lu, new %lu", me->kpps, new_kpps);
		me->kpps = new_kpps;
		capi_v2_voice_imc_rx_send_kpps(me);
	}

	new_delay = capi_v2_voice_imc_rx_get_delay(me);
	if(new_delay != me->delay)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_set_param(): Voice voice_imc_rx delay updated, old %lu, new %lu", me->kpps, new_kpps);
		me->delay = new_delay;
		capi_v2_voice_imc_rx_send_delay(me);
	}
	return result;
}

/*
 * This function is responsible for getting parameters in module. 
 *
 * @param[in] _pif, pointer to the CAPI lib.
 * @param[in] param_id, parameter id sent from client processor.
 * @param[in] port_info_ptr, pointer to port.
 * @param[in] params_ptr, data pointer to parameters to be written by module.
 *
 * @return capi_v2_err_t, result code
 */

static capi_v2_err_t capi_v2_voice_imc_rx_get_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr)
{
	if(NULL == _pif || NULL == params_ptr || NULL == params_ptr->data_ptr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_param(): Error! Received bad property pointer for set_param, 0x%lu", param_id);
		return CAPI_V2_EFAILED;
	}

	capi_v2_voice_imc_rx_t* me = (capi_v2_voice_imc_rx_t*)_pif;

	uint32_t pParamsBufferLen = params_ptr->max_data_len;
	int8_t *pParamsBuffer = params_ptr->data_ptr;

	switch(param_id)
	{
	case VOICE_PARAM_MOD_ENABLE:
		{
			if(pParamsBufferLen < sizeof(int32_t))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VOICE_IMC_RX: capi_v2_imc_tx_get_param(): structure required size = %d, given size = %ld",sizeof(int32_t), pParamsBufferLen);
				return CAPI_V2_ENOMEMORY;
			}
			params_ptr->actual_data_len = sizeof(int32_t);

			*((int32_t*)pParamsBuffer) = 0; // Clearing the whole buffer

			if (TRUE == me->enable)
			{
				*((int16_t*)pParamsBuffer) = TRUE;
		 }
			else
		 {
			 *((int16_t*)pParamsBuffer) = FALSE;
		 }
			break;
		}
	default:
		return CAPI_V2_EFAILED;
	}
	return CAPI_V2_EOK;
}


/**
* This function is used set properties of the CAPIv2.
*
* @param[in] _pif, pointer to the CAPIv2 lib.
* @param[in] props_ptr, pointer to the prop list that needs to be set.
*
* @return capi_v2_err_t, result code
*/
static capi_v2_err_t capi_v2_voice_imc_rx_set_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr)
{

	if(!_pif || !props_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_set_properties(): Error! Received bad pointer in set_property");
		return CAPI_V2_EFAILED;
	}

	capi_v2_err_t result = CAPI_V2_EOK;
	capi_v2_voice_imc_rx_t* me = (capi_v2_voice_imc_rx_t*) _pif;

	uint32_t i;
	for(i = 0; i < props_ptr->props_num; i++)
	{
		capi_v2_prop_t* current_prop_ptr = &(props_ptr->prop_ptr[i]);
		capi_v2_buf_t* payload_ptr = &(current_prop_ptr->payload);
		switch(current_prop_ptr->id)
		{
		case CAPI_V2_EVENT_CALLBACK_INFO:
			{
				capi_v2_event_callback_info_t *cb_info_ptr = (capi_v2_event_callback_info_t*)payload_ptr->data_ptr;
				me->event_cb = cb_info_ptr->event_cb;
				me->event_context = cb_info_ptr->event_context;

				break;
			}
		case CAPI_V2_HEAP_ID:
			{
				if (sizeof(capi_v2_heap_id_t) != payload_ptr->actual_data_len)
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_set_properties(): Error received bad size(%ld) for heap id property", payload_ptr->actual_data_len);
					return CAPI_V2_EBADPARAM;
				}
				capi_v2_heap_id_t* heap_id_ptr = (capi_v2_heap_id_t*) payload_ptr->data_ptr;
				memscpy(&me->heap_id, sizeof(me->heap_id), heap_id_ptr, payload_ptr->actual_data_len);
				break;
			}
		case CAPI_V2_PORT_NUM_INFO:
			{
				if (sizeof(capi_v2_port_num_info_t) != payload_ptr->actual_data_len)
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_set_properties: received bad size(%ld) for port_num_info property",
						payload_ptr->actual_data_len);
					return CAPI_V2_EFAILED;
				}
				capi_v2_port_num_info_t* port_num_info_ptr = (capi_v2_port_num_info_t*) payload_ptr->data_ptr;
				if ((1 != port_num_info_ptr->num_input_ports) || (1 != port_num_info_ptr->num_output_ports))
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_set_properties: received unsupported in_num_ports(%ld) or out_num_ports(%ld)",
						port_num_info_ptr->num_input_ports, port_num_info_ptr->num_output_ports);
					return CAPI_V2_EFAILED;
				}
				break;
			}
		case CAPI_V2_INPUT_MEDIA_FORMAT:
			{
				capi_v2_set_get_media_format_t *pFmt;
				capi_v2_standard_data_format_t *pStd;

				// Payload can be no smaller than the header for media format
				if(payload_ptr->actual_data_len != (sizeof(capi_v2_set_get_media_format_t) + sizeof(capi_v2_standard_data_format_t)))
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_set_properties(): Error! Size mismatch for input format property, got %lu", payload_ptr->actual_data_len);
					//bail out
					return CAPI_V2_EBADPARAM;
				}

				pFmt = (capi_v2_set_get_media_format_t *)payload_ptr->data_ptr;
				if(CAPI_V2_FIXED_POINT == pFmt->format_header.data_format)
				{
					pStd = (capi_v2_standard_data_format_t *)(payload_ptr->data_ptr + sizeof(capi_v2_set_get_media_format_t));
					/* Validate incoming media fmt */
					if( (1 != pStd->num_channels) || (16 != pStd->bits_per_sample ) || ( (VOICE_NB_SAMPLING_RATE != pStd->sampling_rate) && (VOICE_WB_SAMPLING_RATE != pStd->sampling_rate) &&
						(VOICE_FB_SAMPLING_RATE != pStd->sampling_rate) ) )
					{
						MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_set_properties(): Invalid bits_per_sample(%lu) or samping_rate(%lu) or num_channel(%lu)",pStd->bits_per_sample,pStd->sampling_rate,pStd->num_channels);
						return CAPI_V2_EBADPARAM;
					}
					me->sampling_rate = pStd->sampling_rate;
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_set_properties(): set_props bits_per_sample(%lu), samping_rate(%lu), num_channel(%lu)",pStd->bits_per_sample,pStd->sampling_rate,pStd->num_channels);
				}
				else
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_set_properties(): Invalid media fmt(%d)",(int)pFmt->format_header.data_format);
					return CAPI_V2_EBADPARAM;
				}
				break;
			}
		case CAPI_V2_ALGORITHMIC_RESET:
			{
				// Call module algorithmic reset
				break;
			}
		case CAPI_V2_SESSION_IDENTIFIER:
			{
				capi_v2_session_identifier_t *session_identify = (capi_v2_session_identifier_t *)payload_ptr->data_ptr;
				if( payload_ptr->actual_data_len != sizeof(capi_v2_session_identifier_t) )
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_RX: Error! Size mismatch for port info property, got %lu", payload_ptr->actual_data_len);
					//bail out
					return CAPI_V2_EBADPARAM;
				}

				me->session_identify.service_id = session_identify->service_id;
				me->session_identify.session_id = session_identify->session_id;

				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_set_properties(): session_identifies service_id(%lx), session_id(#lx)",
					me->session_identify.service_id,
					me->session_identify.session_id);
				break;
			}
		default:
			{
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_set_properties(): Unsupported property being set %d, ignoring", (int)current_prop_ptr->id);
				break;
			}
		}
	}
	return result;
}

/**
* This function is used get properties from the CAPIv2.
*
* @param[in] _pif, pointer to the CAPIv2 lib.
* @param[in] props_ptr, pointer to the prop list that is needed.
*
* @return capi_v2_err_t, result code
*/
static capi_v2_err_t capi_v2_voice_imc_rx_get_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr)
{
	if(!_pif || !props_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_properties(): Error! Received bad pointer in get_property");
		return CAPI_V2_EFAILED;
	}

	// This module does not expect any properties to be get, so just iterate over, set size to zero, print the prop id and return ok
	uint32_t i;
	for(i = 0; i < props_ptr->props_num; i++)
	{
		capi_v2_prop_t* current_prop_ptr = &(props_ptr->prop_ptr[i]);
		switch(current_prop_ptr->id)
		{
		default:
			{
				current_prop_ptr->payload.actual_data_len = 0;
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_get_properties(): Unsupported prop ID %d", (int)current_prop_ptr->id);
				break;
			}
		}
	}
	return CAPI_V2_EOK;
}

/**
* This function is used to recieve the IMC buffer sent from source.
*/

static void voice_imc_rx_recieve_buf(capi_v2_voice_imc_rx_t* me)
{
	capi_v2_err_t imc_result,result=CAPI_V2_EOK;

	int32_t *buf = NULL ;
	MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VOICE_IMC_RX: Begin voice_imc_rx_recieve_buf()");
	uint32_t payload_size,payload_max_size;
	bool_t is_queue_empty;
	imc_src_dst_info_t *imc_handle = &me->capi_v2_voice_imc_rx_imc_info;
	do
	{
		// Pop the data pointer for getting data sent from source
		imc_result = imc_handle->imc_lib_handle->vtable->dest_pop_buf(imc_handle->dest_handle,(void **)&buf,&payload_size,&payload_max_size,&is_queue_empty);
		if(is_queue_empty)
		{
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_rx_get_stat_prop(): Queue is empty");
			return;
		}
		else if(ADSP_FAILED(imc_result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: voice_imc_rx_recieve_buf(): error in popping imc payload, result(0x%lx)",imc_result);
			return;
		}
		else
		{
			MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VOICE_IMC_RX: voice_imc_rx_recieve_buf(): IMC RX recieved counter = 0x%X !!",(int32_t)*buf);
			imc_result = imc_handle->imc_lib_handle->vtable->dest_return_buf(imc_handle->dest_handle,buf,result);
			if(CAPI_V2_FAILED(imc_result))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: voice_imc_rx_recieve_buf(): error in pushing imc payload. result(%lx)",imc_result);
				return;
			}
		}
	}while(CAPI_V2_SUCCEEDED(result));

	return;
}

/**
* This function is used to update the kpps depending on sampling rate.
*/
static uint32_t capi_v2_voice_imc_rx_get_kpps(capi_v2_voice_imc_rx_t* me)
{
	uint32_t kpps = 0;

	if (VOICE_FB_SAMPLING_RATE == me->sampling_rate)
	{
		kpps = VOICE_IMC_RX_NB_KPPS;
	}
	else if (VOICE_WB_SAMPLING_RATE == me->sampling_rate)
	{
		kpps = VOICE_IMC_RX_WB_KPPS;
	}
	else
	{
		kpps = VOICE_IMC_RX_NB_KPPS;
	}

	return kpps;
}

/**
* This function is used to update the delay depending on sampling rate.
*/
static uint32_t capi_v2_voice_imc_rx_get_delay(capi_v2_voice_imc_rx_t* me)
{
	uint32_t delay=0;
	if(TRUE == me->enable)
	{
		if (VOICE_FB_SAMPLING_RATE == me->sampling_rate)
		{
			delay = VOICE_IMC_FB_DELAY_IN_US;
		}
		else if (VOICE_WB_SAMPLING_RATE == me->sampling_rate)
		{
			delay = VOICE_IMC_WB_DELAY_IN_US;
		}
		else
		{
			delay = VOICE_IMC_NB_DELAY_IN_US;
		}
	}
	else
	{
		delay = 0;
	}
	return delay;
}

/**
* This function is used to send an kpps of module to framework service 
* by using event callback functionality.
*/
static void capi_v2_voice_imc_rx_send_kpps(capi_v2_voice_imc_rx_t * me)
{
	capi_v2_err_t result = CAPI_V2_EOK;
	capi_v2_event_info_t kpps_event;
	capi_v2_event_KPPS_t kpps_payload;

	kpps_payload.KPPS = me->kpps;
	kpps_event.payload.data_ptr = (int8_t*)&kpps_payload;
	kpps_event.payload.actual_data_len = kpps_event.payload.max_data_len = sizeof(kpps_payload);

	if(me->event_cb)
	{
		result = me->event_cb(me->event_context, CAPI_V2_EVENT_KPPS, &kpps_event);
		if(result)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_send_kpps(): Error! Voice kpps reporting failed, error %ld", result);
		}
	}
	return;
}

/**
* This function is used to send an delay of module to framework service 
* by using event callback functionality.
*/
static void capi_v2_voice_imc_rx_send_delay(capi_v2_voice_imc_rx_t * me)
{
	capi_v2_err_t result = CAPI_V2_EOK;
	capi_v2_event_info_t delay_event;
	capi_v2_event_algorithmic_delay_t delay_payload;

	delay_payload.delay_in_us = me->delay;
	delay_event.payload.data_ptr = (int8_t*)&delay_payload;
	delay_event.payload.actual_data_len = delay_event.payload.max_data_len = sizeof(delay_payload);

	if(me->event_cb)
	{
		result = me->event_cb(me->event_context, CAPI_V2_EVENT_ALGORITHMIC_DELAY, &delay_event);
		if(result)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_RX: capi_v2_voice_imc_rx_send_delay(): Error! Voice delay reporting failed, error %ld", result);
		}
	}
	return;
}

/**
* This function is used to get the IMC module handle along with session ID for module
* by using event callback functionality.
*/
static void capi_v2_voice_imc_rx_get_imc_handle(capi_v2_voice_imc_rx_t* me)
{
	capi_v2_err_t result = CAPI_V2_EOK;
	capi_v2_event_get_library_instance_t lib_instance;
	capi_v2_event_info_t  event_info;

	lib_instance.id = ELITE_LIB_GET_IMC;
	lib_instance.ptr = NULL;

	event_info.port_info.is_valid = FALSE;
	event_info.payload.data_ptr = (int8_t *)&lib_instance;
	event_info.payload.actual_data_len =event_info.payload.max_data_len = sizeof(lib_instance);

	result = me->event_cb(me->event_context, CAPI_V2_EVENT_GET_LIBRARY_INSTANCE,&event_info);
	if(CAPI_V2_FAILED(result))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_RX: get_imc_handle(): failed to get IMC lib handle. result(0x%lx)",result);
		return;
	}
	else
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_RX: get_imc_handle(): IMC Rx received IMC lib handle");
	}

	me->capi_v2_voice_imc_rx_imc_info.imc_lib_handle = (elite_lib_get_imc_t*)lib_instance.ptr;

	return;
}

/*
* This function is used to register the module for intermodule communication
*/
static capi_v2_err_t capi_v2_voice_imc_register_src_dest(imc_src_dst_info_t *imc_src_dst_into, int8_t *key, uint32_t keylen,uint32_t max_bufq_len,uint32_t alloc_num_buf,uint32 buf_size, imc_src_dst_t src_dst)
{
	capi_v2_err_t imc_result = CAPI_V2_EOK;

	uint32_t comm_id = *(uint32_t*)key;

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_register_src_dest: module is trying to register for IMC CommID(%#lx), src_dst(%d)",comm_id,(int)src_dst);

	//Check if the key is already received
	if(IMC_STATE_NONE != imc_src_dst_into->state)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_register_src_dest: Error!! trying to set multiple Key values for the same CommID(%lx). state(%d)",
			comm_id,(int)imc_src_dst_into->state);
		return CAPI_V2_EBADPARAM;
	}

	//clear Key
	imc_src_dst_into->key_length = keylen;

	//Now register for the intermodule communication
	if(IMC_SOURCE == src_dst)
	{
		imc_result = imc_src_dst_into->imc_lib_handle->vtable->src_register(&imc_src_dst_into->source_handle,max_bufq_len);
		if(CAPI_V2_FAILED(imc_result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_register_src_dest: Error!! source failed to register, result(0x%lx)",imc_result);
			return imc_result;
		}

		//Now Fill the source queue with buffers
		imc_result = imc_src_dst_into->imc_lib_handle->vtable->src_allocate_push_self_buf(imc_src_dst_into->source_handle, buf_size, alloc_num_buf);
		if(CAPI_V2_FAILED(imc_result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_register_src_dest: Error!! source failed to push buffers, result(0x%lx)",imc_result);
			return imc_result;
		}
		else
		{
			imc_src_dst_into->state =  IMC_STATE_REGISTERED;
		}
	}
	else if(IMC_DESTINATION == src_dst)
	{
		imc_result = imc_src_dst_into->imc_lib_handle->vtable->dest_register(&imc_src_dst_into->dest_handle,(int8_t*)imc_src_dst_into->key,imc_src_dst_into->key_length,max_bufq_len);
		if(CAPI_V2_FAILED(imc_result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_register_src_dest: Error!! destination failed to register, result(0x%lx)",imc_result);
			return imc_result;
		}
		else
		{
			imc_src_dst_into->state =  IMC_STATE_REGISTERED;
		}
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_RX: capi_v2_voice_imc_register_src_dest: Error!! invalid source/destination param(%d)",(int)src_dst);
		imc_result = CAPI_V2_EBADPARAM;
	}

	return imc_result;
}

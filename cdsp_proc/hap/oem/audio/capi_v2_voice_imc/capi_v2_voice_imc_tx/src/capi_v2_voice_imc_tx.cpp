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
#include "adsp_vpm_api.h"
#include "shared_lib_api.h"

#include "capi_v2_voice_imc_tx.h"

// macros coming from VoiceCmnUtils.h
#define  VOICE_ROUNDTO8(x) ((((uint32_t)(x) + 7) >> 3) << 3)

// Sampling Rates
#define VOICE_NB_SAMPLING_RATE      (8000)
#define VOICE_WB_SAMPLING_RATE      (16000)
#define VOICE_INVALID_SAMPLING_RATE (0)
#define VOICE_FRAME_SIZE_NB_10MS  (80)
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

/* -------------------------------------------------------------*/
/* Constant / Define Declarations                               */
/* -------------------------------------------------------------*/
//Number of max input and output buffers
#define IMC_TX_MAX_INPUT_PORTS 3
#define IMC_TX_MAX_OUTPUT_PORTS 1

#define VOICE_IMC_RETRY_THRESHOLD 5
#define VOICE_IMC_TX_SRC_MAX_BUF_Q_ELEMENTS 2

//KPPS Values
#define VOICE_IMC_TX_NB_KPPS (150)  // worst case profiled 
#define VOICE_IMC_TX_WB_KPPS (250)  // worst case profiled

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
	IMC_TX_COMM_RX = 0,      //IMC instance for Tx <-> Rx
	IMC_TX_COMM_NUM              //Number of IMC instances
}voice_imc_tx_t;

typedef struct{
	capi_v2_set_get_media_format_t media_format;
	capi_v2_standard_data_format_t data_format;
} capi_data_format_struct_t;

typedef struct capi_v2_imc_tx_t
{
	const capi_v2_vtbl_t	*vtbl;
	uint32_t				enable;
	uint32_t				kpps;             // kilo packets per second
	uint32_t				delay;            // delay will always be zero as no processing in pri mic input
	QURT_ELITE_HEAP_ID		heap_id;          //heap_id
	capi_v2_event_cb_f		event_cb;         // Callback function to issue to framework to report delay and kpps
	void*					event_context;    // Event context that must be issued in the callback
	capi_v2_port_num_info_t			port_info;        // number of i/o ports
	capi_data_format_struct_t		input_output_media_format[IMC_TX_MAX_INPUT_PORTS];
	imc_src_dst_info_t				capi_v2_imc_tx_imc_info; //IMC supported. (Tx <-> Rx)
	uint32_t						sampling_rate; 
	capi_v2_session_identifier_t	session_identify;
	uint32_t						frame_samp_10ms;
	int32_t							counter_imc;
}capi_v2_imc_tx_t;

/*------------------------------------------------------------------------
* Static declarations
* -----------------------------------------------------------------------*/
static capi_v2_err_t capi_v2_imc_tx_process(capi_v2_t* _pif, capi_v2_stream_data_t* input[], capi_v2_stream_data_t* output[]);
static capi_v2_err_t capi_v2_imc_tx_end(capi_v2_t* _pif);
static capi_v2_err_t capi_v2_imc_tx_set_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr);
static capi_v2_err_t capi_v2_imc_tx_get_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr);
static capi_v2_err_t capi_v2_imc_tx_set_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr);
static capi_v2_err_t capi_v2_imc_tx_get_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr);

//static functions
static uint32_t capi_v2_imc_tx_get_kpps(capi_v2_imc_tx_t* me);
static void capi_v2_imc_tx_send_kpps(capi_v2_imc_tx_t* me);
static void capi_v2_imc_tx_get_imc_handle(capi_v2_imc_tx_t* me);
static capi_v2_err_t capi_v2_imc_tx_imc_register_src_dest(imc_src_dst_info_t *imc_src_dst_into, int8_t *key, uint32_t keylen,uint32_t max_bufq_len,uint32_t alloc_num_buf,uint32 buf_size, imc_src_dst_t src_dst);

static capi_v2_vtbl_t vtbl =
{
	capi_v2_imc_tx_process,
	capi_v2_imc_tx_end,
	capi_v2_imc_tx_set_param,
	capi_v2_imc_tx_get_param,
	capi_v2_imc_tx_set_properties,
	capi_v2_imc_tx_get_properties,
};

/**
* This function is used to query the static properties to create the CAPI.
*
* @param[in] init_props_ptr, pointer to the initializing prop list
* @param[in, out] out_props_ptr, pointer to the output pro list
* @return capi_v2_err_t, result code
*/
capi_v2_err_t capi_v2_voice_imc_tx_get_static_properties(capi_v2_proplist_t *init_props_ptr, capi_v2_proplist_t *out_props_ptr)
{

	if( NULL == out_props_ptr )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_get_stat_prop(): Error! Received bad property pointer for get property");
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
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_get_stat_prop(): Unsupported static property being set, %d", (int)current_prop_ptr->id);
					break;
				}
			}
		}
	}

	// now iterate over output properties that are being asked for
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
					data_ptr->size_in_bytes =  VOICE_ROUNDTO8(sizeof(capi_v2_imc_tx_t)); // Specify the memory requirement for CAPIv2 Module only. 
					//For core processing library try to handle memory by internal memory management using heap ID sent from framework.
					payload_ptr->actual_data_len = sizeof(capi_v2_init_memory_requirement_t);
				}
				else
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
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
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
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
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
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
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
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
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
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
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_get_stat_prop(): Error! Get Property id 0x%lx Bad param size %lu", (uint32_t)current_prop_ptr->id, payload_ptr->max_data_len);
					return CAPI_V2_EBADPARAM;
				}
				break;
			}
		default:
			{
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_get_stat_prop(): Unsupported prop_id 0x%lx", (uint32_t)current_prop_ptr->id);
				result=CAPI_V2_EUNSUPPORTED;
				break;
			}
		}
	}
	return result;
}

/**
* This function is used init the CAPI lib.
*
* @param[in] _pif, pointer to the CAPI lib.
* @param[in] init_set_properties, pointer to the prop list that needs to be init'ed.
*
* @return capi_v2_err_t, result code
*/

capi_v2_err_t capi_v2_voice_imc_tx_init(capi_v2_t* _pif, capi_v2_proplist_t *init_props_ptr)
{
	capi_v2_err_t result = CAPI_V2_EOK;

	if(NULL == _pif)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_init(): Error! received bad pointer");
		return CAPI_V2_EFAILED;
	}

	capi_v2_imc_tx_t* me = (capi_v2_imc_tx_t*)_pif;
	memset(me,0,sizeof(capi_v2_imc_tx_t));

	me->session_identify.service_id = 0xFFFF;
	me->session_identify.session_id = 0xFFFF;  //initialize with 0xFFFF

	if(init_props_ptr)
	{
		// setting the properties sent by framework
		result=capi_v2_imc_tx_set_properties(_pif,init_props_ptr);
		if(CAPI_V2_FAILED(result))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_init(): Error! Set properties failed");
			return CAPI_V2_EFAILED;
		}
	}

	// Point to vtbl
	me->vtbl = &vtbl;

	me->kpps = 100; // approximate value
	me->capi_v2_imc_tx_imc_info.state = IMC_STATE_NONE;
	me->frame_samp_10ms = VOICE_FRAME_SIZE_NB_10MS;
	me->counter_imc = 0; // Initialize counter to 0

	//Initialize state information.
	me->capi_v2_imc_tx_imc_info.state = IMC_STATE_NONE;

	// Get the handle of IMC module. This also fetches the session identifier
	capi_v2_imc_tx_get_imc_handle(me);

	/*
	* If session identifier is received and imc library is queried, register for IMC handle.
	*/
	if(me->session_identify.service_id != 0xFFFF &&
		me->session_identify.session_id != 0xFFFF &&
		(NULL != me->capi_v2_imc_tx_imc_info.imc_lib_handle) )
	{
		/*
		* Key is COMMID | RX module ID | Tx module Id | seesionID
		*/
		me->capi_v2_imc_tx_imc_info.key[0] = IMC_COMM_ID;
		me->capi_v2_imc_tx_imc_info.key[1] = VOICE_IMC_RX; //IMC destination module ID
		me->capi_v2_imc_tx_imc_info.key[2] = VOICE_IMC_TX; //IMC source module ID
		me->capi_v2_imc_tx_imc_info.key[3] = (uint32_t)me->session_identify.session_id;
		imc_src_dst_info_t *comm_info_ptr;
		uint32_t param_size,max_q_length,alloc_buffer;
		uint32_t max_payload_size = sizeof(int32_t);
		imc_src_dst_t src_dest;

		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_TX: key (%lx) (%lx) (%lx) (%lx)",
			me->capi_v2_imc_tx_imc_info.key[0],
			me->capi_v2_imc_tx_imc_info.key[1],
			me->capi_v2_imc_tx_imc_info.key[2],
			me->capi_v2_imc_tx_imc_info.key[3]);

		param_size = sizeof(me->capi_v2_imc_tx_imc_info.key);
		comm_info_ptr = (imc_src_dst_info_t*)&me->capi_v2_imc_tx_imc_info;
		src_dest = IMC_SOURCE;
		max_q_length = alloc_buffer = VOICE_IMC_TX_SRC_MAX_BUF_Q_ELEMENTS;

		// register for intermodule communication
		capi_v2_err_t imc_result;
		imc_result = capi_v2_imc_tx_imc_register_src_dest(comm_info_ptr,(int8_t*)me->capi_v2_imc_tx_imc_info.key, param_size, max_q_length, alloc_buffer,max_payload_size,src_dest);
		if(CAPI_V2_FAILED(imc_result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_TX: failed to register to IMC. result(0x%lx)",imc_result);
			return imc_result;
		}
	}
	capi_v2_imc_tx_send_kpps(me);

	return CAPI_V2_EOK;
}

/**
* This function is used set properties of the CAPI.
*
* @param[in] _pif, pointer to the CAPI lib.
* @param[in] input, pointer to the input stream data.
* @param[in, out] output, pointer to the output stream data.
*
* @return capi_v2_err_t, result code
*/
static capi_v2_err_t capi_v2_imc_tx_process(capi_v2_t* _pif, capi_v2_stream_data_t* input[], capi_v2_stream_data_t* output[])
{
	if((NULL == _pif) || (NULL== input) || (NULL== output) )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_process(): Received bad property in process");
		return CAPI_V2_EFAILED;
	}
	capi_v2_err_t result=CAPI_V2_EOK;
	void *payload_header_ptr = NULL;
	bool_t is_queue_empty;
	capi_v2_err_t resp_result;
	int16_t *in_samp_ptr;
	capi_v2_imc_tx_t* me = (capi_v2_imc_tx_t*)_pif;
	uint32_t i;

	//copy the input to output if the buffer pointers are not same. non-inplace computing
	for(i=0;i<input[0]->bufs_num;i++)
	{
		in_samp_ptr = (int16_t *)input[0]->buf_ptr[i].data_ptr;
		if(input[0]->buf_ptr[i].data_ptr != output[0]->buf_ptr[i].data_ptr)
		{
			memscpy(output[0]->buf_ptr[i].data_ptr,output[0]->buf_ptr[i].max_data_len,input[0]->buf_ptr[i].data_ptr,input[0]->buf_ptr[i].actual_data_len);
		}
		output[0]->buf_ptr[i].actual_data_len = input[0]->buf_ptr[i].actual_data_len;
	}
	//if disabled, return.
	if(FALSE == me->enable)
	{
		return result;
	}

	//Check if the Tx has opend the communication. If it has not then open it
	// Also this is checked only for VOICE_IMC_RETRY_THRESHOLD number of times. 
	if( ((IMC_STATE_SOURCE_CLOSED_COMMUNICATION == me->capi_v2_imc_tx_imc_info.state) ||
		(IMC_STATE_REGISTERED == me->capi_v2_imc_tx_imc_info.state)) &&
		(me->capi_v2_imc_tx_imc_info.src_frame_num < VOICE_IMC_RETRY_THRESHOLD))
	{
		imc_src_dst_info_t *imc_instc_ptr = &me->capi_v2_imc_tx_imc_info;
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_process(): opening communication for inter module communication");

		result = imc_instc_ptr->imc_lib_handle->vtable->src_open_comm( imc_instc_ptr->source_handle, &imc_instc_ptr->dest_handle,
			(const int8_t *)imc_instc_ptr->key,imc_instc_ptr->key_length);

		if(CAPI_V2_FAILED(result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_process(): Failed to open Communication. result(%lx)",result);
			imc_instc_ptr->src_frame_num++;
		}
		else
		{
			imc_instc_ptr->state = IMC_STATE_SOURCE_OPENED_COMMUNICATION;
		}
	}



	if(me->capi_v2_imc_tx_imc_info.state != IMC_STATE_SOURCE_OPENED_COMMUNICATION)
	{
		MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VOICE_IMC_TX:  VoiceIMCTx has not opened communication. Returning");
		return CAPI_V2_EOK;
	}
	/*
	* Send the data to destination module.
	* 1. pop the buffer from Source queue.
	* 2. fill the buffer with the data - payload header and data.
	* 3. send the buffer to destination.
	*/
	int32_t *buf = NULL ;
	MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "VOICE_IMC_TX:  Trying to pop buffers from the queue");
	result = me->capi_v2_imc_tx_imc_info.imc_lib_handle->vtable->src_pop_buf(me->capi_v2_imc_tx_imc_info.source_handle,(void**)&buf, &resp_result,&is_queue_empty);
	if(is_queue_empty == FALSE)
	{
		uint32_t payload_size;

		if( CAPI_V2_FAILED( result))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX:  FAILED to pop buffers from the queue. result(%lx),is_queue_empty(%d)!!",result,(int)is_queue_empty);
			return CAPI_V2_EFAILED;
		}

		*buf = (int32_t)me->counter_imc;
		payload_size = sizeof(buf);
		(me->counter_imc)++;
		MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "VOICE_IMC_TX:  IMC TX sending counter = 0x%X !!",*buf);
		result = me->capi_v2_imc_tx_imc_info.imc_lib_handle->vtable->src_push_buf(me->capi_v2_imc_tx_imc_info.source_handle,me->capi_v2_imc_tx_imc_info.dest_handle,buf,payload_size);
		if( CAPI_V2_FAILED(result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX:  FAILED to push buffers from the queue. result(%lx)!!",result);
			me->capi_v2_imc_tx_imc_info.imc_lib_handle->vtable->src_return_buf(me->capi_v2_imc_tx_imc_info.source_handle, payload_header_ptr,CAPI_V2_EOK);

			//IF the destination is not active then Close the connection and try again to open connection in next frame
			if(CAPI_V2_EUNSUPPORTED == result)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX:   Rx handle is not active. Closing the connection");
				me->capi_v2_imc_tx_imc_info.imc_lib_handle->vtable->src_close_connection(me->capi_v2_imc_tx_imc_info.source_handle,me->capi_v2_imc_tx_imc_info.dest_handle);
				me->capi_v2_imc_tx_imc_info.state = IMC_STATE_SOURCE_CLOSED_COMMUNICATION;
				me->capi_v2_imc_tx_imc_info.src_frame_num = 0;
			}
		}
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VOICE_IMC_TX:  queue is empty. result(%lx)!!",result);
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

static capi_v2_err_t capi_v2_imc_tx_set_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr)
{
	if(NULL == _pif || NULL == params_ptr || NULL == params_ptr->data_ptr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_set_param(): Error! Received bad property pointer for set_param, 0x%lx", param_id);
		return CAPI_V2_EFAILED;
	}
	capi_v2_err_t result = CAPI_V2_EOK;
	capi_v2_imc_tx_t* me = (capi_v2_imc_tx_t*)_pif;

	int8* pParamsBuffer = params_ptr->data_ptr;

	switch(param_id)
	{
	case VOICE_PARAM_MOD_ENABLE:
		{
			//typecast the buffer you got with structure of type ISOD definition
			int16_t *enable_ptr = (int16_t*) pParamsBuffer;

			if(TRUE == *enable_ptr)
			{
				me->enable = TRUE; //IMC enabled
			}
			break;
		}
		break;
	default:
		{
			return CAPI_V2_EUNSUPPORTED;
		}
	}

	uint32_t new_kpps;
	new_kpps = capi_v2_imc_tx_get_kpps(me);

	if(new_kpps != me->kpps)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_set_param(): Voice kpps updated, old %lu, new %lu", me->kpps, new_kpps);
		me->kpps = new_kpps;
		capi_v2_imc_tx_send_kpps(me);
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
static capi_v2_err_t capi_v2_imc_tx_get_param(capi_v2_t* _pif, uint32_t param_id, const capi_v2_port_info_t *port_info_ptr, capi_v2_buf_t *params_ptr)
{
	if(NULL == _pif || NULL == params_ptr || NULL == params_ptr->data_ptr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_get_param(): Error! Received bad property pointer for set_param, 0x%lx", param_id);
		return CAPI_V2_EFAILED;
	}
	capi_v2_imc_tx_t* me = (capi_v2_imc_tx_t*)_pif;

	uint32_t pParamsBufferLen = params_ptr->max_data_len;
	int8_t *pParamsBuffer = params_ptr->data_ptr;

	switch(param_id)
	{
	case VOICE_PARAM_MOD_ENABLE:
		{
			if(pParamsBufferLen < sizeof(int32_t))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_get_param(): IMC TX structure required size = %d, given size = %ld",sizeof(int32_t), pParamsBufferLen);
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
		{
			return CAPI_V2_EFAILED;
		}
	}
	return CAPI_V2_EOK;
}

/**
* This function is end of the CAPIv2.
*
* @param[in] _pif, pointer to the CAPIv2 lib.
*
*/
static capi_v2_err_t capi_v2_imc_tx_end(capi_v2_t* _pif)
{
	if(NULL == _pif)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_end(): received null pointer in end");
		return CAPI_V2_EFAILED;
	}

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_end(): end()");

	capi_v2_imc_tx_t* me = (capi_v2_imc_tx_t*)_pif;
	imc_src_dst_info_t *imc_src_inst = &me->capi_v2_imc_tx_imc_info;

	//Close communication if already opened
	if(IMC_STATE_SOURCE_OPENED_COMMUNICATION == imc_src_inst->state)
	{
		imc_src_inst->imc_lib_handle->vtable->src_close_connection(imc_src_inst->source_handle,imc_src_inst->dest_handle);
		imc_src_inst->state = IMC_STATE_SOURCE_CLOSED_COMMUNICATION;
	}

	// Deregister the module
	if(IMC_STATE_NONE != imc_src_inst->state)
	{
		imc_src_inst->imc_lib_handle->vtable->src_deregister(imc_src_inst->source_handle);
		imc_src_inst->state = IMC_STATE_NONE;
	}
	imc_src_inst->imc_lib_handle->vtable->b.end(me->capi_v2_imc_tx_imc_info.imc_lib_handle);
	me->counter_imc = 0;
	return CAPI_V2_EOK;
}


/**
* This function is used set properties of the CAPI.
*
* @param[in] _pif, pointer to the CAPI lib.
* @param[in] props_ptr, pointer to the prop list that needs to be set.
*
* @return capi_v2_err_t, result code
*/
static capi_v2_err_t capi_v2_imc_tx_set_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr)
{

	if(!_pif || !props_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_set_properties(): Error! Received bad pointer in set_property");
		return CAPI_V2_EFAILED;
	}

	capi_v2_err_t result = CAPI_V2_EOK;
	capi_v2_imc_tx_t* me = (capi_v2_imc_tx_t*) _pif;

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
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_set_properties(): Error received bad size(%ld) for heap id property", payload_ptr->actual_data_len);
					return CAPI_V2_EBADPARAM;
				}
				capi_v2_heap_id_t* heap_id_ptr = (capi_v2_heap_id_t*) payload_ptr->data_ptr;
				memscpy(&me->heap_id, sizeof(me->heap_id), heap_id_ptr, payload_ptr->actual_data_len);
				break;
			}
		case CAPI_V2_PORT_NUM_INFO:
			{
				capi_v2_port_num_info_t *port_info_ptr = (capi_v2_port_num_info_t *)payload_ptr->data_ptr;
				if( payload_ptr->actual_data_len != sizeof(capi_v2_port_num_info_t) )
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_set_properties(): Error! Size mismatch for port info property, got %lu", payload_ptr->actual_data_len);
					//bail out
					return CAPI_V2_EBADPARAM;
				}

				if( (port_info_ptr->num_input_ports > IMC_TX_MAX_INPUT_PORTS) ||
					(port_info_ptr->num_output_ports > IMC_TX_MAX_OUTPUT_PORTS) )
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_set_properties(): Error! invalid number of ports, got num_input_ports(%lu), num_output_ports(%lu)",
						port_info_ptr->num_input_ports,port_info_ptr->num_output_ports);
					//bail out
					return CAPI_V2_EBADPARAM;
				}
				me->port_info.num_input_ports = port_info_ptr->num_input_ports;
				me->port_info.num_output_ports = port_info_ptr->num_output_ports;

				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_set_properties(): port number config. num_input_ports(%lu), num_output_ports(%lu)",port_info_ptr->num_input_ports,port_info_ptr->num_output_ports);
				break;
			}
		case CAPI_V2_INPUT_MEDIA_FORMAT:
			{
				capi_data_format_struct_t *pFmt;
				uint32_t num_input_ports;

				if(CAPI_V2_DATA_FORMAT_INVALID_VAL == me->port_info.num_input_ports)
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_set_properties(): Error! input media format is being configured before port_num_info is configured");
					return CAPI_V2_EBADPARAM;
				}

				// Payload can be no smaller than the header for media format
				if(payload_ptr->actual_data_len != (sizeof(capi_data_format_struct_t)*me->port_info.num_input_ports ) )
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_set_properties(): Error! Size mismatch for input media format property, got %lu", payload_ptr->actual_data_len);
					//bail out
					return CAPI_V2_EBADPARAM;
				}

				pFmt = (capi_data_format_struct_t *)payload_ptr->data_ptr;
				for(num_input_ports=0; num_input_ports<me->port_info.num_input_ports; num_input_ports++)
				{
					if(CAPI_V2_FIXED_POINT == pFmt->media_format.format_header.data_format)
					{
						//check number of channels. 
						if((((pFmt->data_format.num_channels != 1) || (VOICE_WB_SAMPLING_RATE < pFmt->data_format.sampling_rate)))
							|| (pFmt->data_format.bits_per_sample != 16) )
						{
							MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_set_properties(): Invalid bits_per_sample(%d) or samping_rate(%lu) or num_channel(%d) for LEC_OUT or NREF output port",
								(int)pFmt->data_format.bits_per_sample,pFmt->data_format.sampling_rate,(int)pFmt->data_format.num_channels);
							return CAPI_V2_EBADPARAM;
						}

						//now populate media format
						me->input_output_media_format[num_input_ports].media_format.format_header = pFmt->media_format.format_header;

						me->input_output_media_format[num_input_ports].data_format.bitstream_format = pFmt->data_format.bitstream_format;
						me->input_output_media_format[num_input_ports].data_format.num_channels     = pFmt->data_format.num_channels;
						me->input_output_media_format[num_input_ports].data_format.bits_per_sample  = pFmt->data_format.bits_per_sample;
						me->input_output_media_format[num_input_ports].data_format.q_factor         = pFmt->data_format.q_factor;
						me->input_output_media_format[num_input_ports].data_format.sampling_rate    = pFmt->data_format.sampling_rate;
						me->input_output_media_format[num_input_ports].data_format.data_is_signed   = pFmt->data_format.data_is_signed;
						me->input_output_media_format[num_input_ports].data_format.data_interleaving= pFmt->data_format.data_interleaving;

						//copy channel_type
						memscpy(&me->input_output_media_format[num_input_ports].data_format.channel_type[0],sizeof(me->input_output_media_format[num_input_ports].data_format.channel_type),
							&pFmt->data_format.channel_type[0],sizeof(pFmt->data_format.channel_type[0])*(pFmt->data_format.num_channels));
					}
					else
					{
						MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_set_properties(): Invalid media fmt(%ul)",pFmt->media_format.format_header.data_format);
						return CAPI_V2_EBADPARAM;
					}

					pFmt = (capi_data_format_struct_t*)((int8_t*)pFmt+sizeof(capi_data_format_struct_t));
				}
				break;
			}
		case CAPI_V2_ALGORITHMIC_RESET:
			{
				// Call module reset function here.
				break;
			}
		case CAPI_V2_SESSION_IDENTIFIER:
			{
				capi_v2_session_identifier_t *session_identify = (capi_v2_session_identifier_t *)payload_ptr->data_ptr;
				if( payload_ptr->actual_data_len != sizeof(capi_v2_session_identifier_t) )
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX:  Error! Size mismatch for port info property, got %lu", payload_ptr->actual_data_len);
					//bail out
					return CAPI_V2_EBADPARAM;
				}

				me->session_identify.service_id = session_identify->service_id;
				me->session_identify.session_id = session_identify->session_id;

				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_TX:  session_identifies service_id(%lx), session_id(%lx)",
					me->session_identify.service_id,
					me->session_identify.session_id);
				break;
			}
		default:
			{
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_set_properties(): Unsupported property being set %d, ignoring", (int)current_prop_ptr->id);
				break;
			}
		}
	}
	return result;
}

/**
* This function is used get properties from the CAPI.
*
* @param[in] _pif, pointer to the CAPI lib.
* @param[in] props_ptr, pointer to the prop list that is needed.
*
* @return capi_v2_err_t, result code
*/
static capi_v2_err_t capi_v2_imc_tx_get_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr)
{
	if(!_pif || !props_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_get_properties(): Error! Received bad pointer in get_property");
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
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_get_properties(): Unsupported prop ID %d", (int)current_prop_ptr->id);
				break;
			}
		}
	}
	return CAPI_V2_EOK;
}


/*
* This function is used to register the module for intermodule communication
*/
static capi_v2_err_t capi_v2_imc_tx_imc_register_src_dest(imc_src_dst_info_t *imc_src_dst_into, int8_t *key, uint32_t keylen,uint32_t max_bufq_len,uint32_t alloc_num_buf,uint32 buf_size, imc_src_dst_t src_dst)
{
	capi_v2_err_t imc_result = CAPI_V2_EOK;

	uint32_t comm_id = *(uint32_t*)key;

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_imc_register_src_dest: module is trying to register for IMC CommID(%#lx), src_dst(%d)",comm_id,(int)src_dst);

	//Check if the key is already received
	if(IMC_STATE_NONE != imc_src_dst_into->state)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_imc_register_src_dest: Error!! trying to set multiple Key values for the same CommID(%lx). state(%d)",
			comm_id,(int)imc_src_dst_into->state);
		return CAPI_V2_EBADPARAM;
	}

	imc_src_dst_into->key_length = keylen;

	//Now register for the intermodule communication
	if(IMC_SOURCE == src_dst)
	{
		imc_result = imc_src_dst_into->imc_lib_handle->vtable->src_register(&imc_src_dst_into->source_handle,max_bufq_len);
		if(CAPI_V2_FAILED(imc_result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_imc_register_src_dest: Error!! source failed to register, result(0x%lx)",imc_result);
			return imc_result;
		}

		//Now Fill the source queue with buffers
		imc_result = imc_src_dst_into->imc_lib_handle->vtable->src_allocate_push_self_buf(imc_src_dst_into->source_handle, buf_size, alloc_num_buf);
		if(CAPI_V2_FAILED(imc_result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_imc_register_src_dest: Error!! source failed to push buffers, result(0x%lx)",imc_result);
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
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_imc_register_src_dest: Error!! destination failed to register, result(0x%lx)",imc_result);
			return imc_result;
		}
		else
		{
			imc_src_dst_into->state =  IMC_STATE_REGISTERED;
		}
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VOICE_IMC_TX: capi_v2_imc_tx_imc_register_src_dest: Error!! invalid source/destination param(%d)",(int)src_dst);
		imc_result = CAPI_V2_EBADPARAM;
	}

	return imc_result;
}

/**
* This function is used to update the kpps depending on sampling rate.
*/
static uint32_t capi_v2_imc_tx_get_kpps(capi_v2_imc_tx_t* me)
{
	uint32_t kpps = 0;

	if (TRUE == me->enable)
	{
		switch (me->sampling_rate)
		{
		case VOICE_NB_SAMPLING_RATE:
		{
			kpps=VOICE_IMC_TX_NB_KPPS;
			break;
		}
		case VOICE_WB_SAMPLING_RATE:
		{
			kpps=VOICE_IMC_TX_WB_KPPS;
			break;
		}
		default:
			break;
		}
	}

	return kpps;
}

/**
* This function is used to send an kpps of module to framework service 
* by using event callback functionality.
*/
static void capi_v2_imc_tx_send_kpps(capi_v2_imc_tx_t * me)
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
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX: capi_v2_imc_tx_send_kpps(): Error! Voice capi_v2_imc_tx kpps reporting failed, error %ld", result);
		}
	}
	return;
}

/**
* This function is used to get the IMC module handle along with session ID for module
* by using event callback functionality.
*/
static void capi_v2_imc_tx_get_imc_handle(capi_v2_imc_tx_t* me)
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
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VOICE_IMC_TX: get_imc_handle(): failed to get IMC lib handle. result(0x%lx)",result);
		return;
	}
	else
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VOICE_IMC_TX: get_imc_handle(): VOICE IMC Tx received IMC lib handle");
	}

	me->capi_v2_imc_tx_imc_info.imc_lib_handle = (elite_lib_get_imc_t*)lib_instance.ptr;

	return;
}

/* ======================================================================== */
/**
   @file capi_v2_decimate_utils.cpp

   C source file to implement the utility functions for
   CAPIv2 for DECIMATE example.
 */

/* =========================================================================
   Copyright (c) 2015 QUALCOMM Technologies Incorporated.
   All rights reserved. Qualcomm Technologies Proprietary and Confidential.
   ========================================================================= */
/*------------------------------------------------------------------------
 * Include files and Macro definitions
 * -----------------------------------------------------------------------*/

#include "Elite_CAPI_V2_properties.h"

#include "capi_v2_decimate_utils.h"

static inline uint32_t align_to_8_byte(const uint32_t num)
{
   return ((num + 7) & (0xFFFFFFF8));
}


/*===========================================================================
	FUNCTION : capi_v2_decimate_process_get_properties
	DESCRIPTION: Utility function responsible for getting the properties from Decimate
				 example.
===========================================================================*/
capi_v2_err_t capi_v2_decimate_process_get_properties(
		capi_v2_decimate_t *me_ptr, capi_v2_proplist_t *proplist_ptr)
{
	capi_v2_err_t  capi_v2_result = CAPI_V2_EOK;
	capi_v2_prop_t *prop_array = proplist_ptr->prop_ptr;
	uint32_t i;
	for (i = 0; i < proplist_ptr->props_num; i++)
	{
		capi_v2_buf_t *payload_ptr = &(prop_array[i].payload);

		switch(prop_array[i].id)
		{
		/*The amount of memory in bytes to be passed into the capi_v2_init function.
		    Payload structure: capi_v2_init_memory_requirement_t.
		 */
		case CAPI_V2_INIT_MEMORY_REQUIREMENT:
		{
			if (payload_ptr->max_data_len >= sizeof(capi_v2_init_memory_requirement_t))
			{
				uint32_t decimate_size = 0;
				uint32_t procblock_size = TEMP_BUF_SIZE_IN_SAMPLES ;

				// Each block must begin with 8-byte aligned memory.
				decimate_size = align_to_8_byte(sizeof(capi_v2_decimate_t))
										 + align_to_8_byte(procblock_size * sizeof(int16_t))   // For L-channel
										 + align_to_8_byte(procblock_size * sizeof(int16_t));  // For R-channel

				capi_v2_init_memory_requirement_t *data_ptr =
						(capi_v2_init_memory_requirement_t*)(payload_ptr->data_ptr);
				data_ptr->size_in_bytes = decimate_size;
				payload_ptr->actual_data_len = sizeof(capi_v2_init_memory_requirement_t);

				FARF(HIGH,"CAPIv2 Decimate: get property for initialization memory "
						"requirements %lu bytes",data_ptr->size_in_bytes);
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->max_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
			break;
		}

		/**< Indicates whether the module can perform in-place computation. If this value
		    is true, the caller may provide the same pointers for input and output (but this
		    is not guaranteed). This requires that the input and output data format be the
		    same and the requires_data_buffering property be false.
		    Payload Structure: capi_v2_is_inplace_t */

		case CAPI_V2_IS_INPLACE:
		{
			if (payload_ptr->max_data_len >= sizeof(capi_v2_is_inplace_t))
			{
				capi_v2_is_inplace_t *data_ptr = (capi_v2_is_inplace_t*)payload_ptr->data_ptr;
				data_ptr->is_inplace = FALSE;
				payload_ptr->actual_data_len = sizeof(capi_v2_is_inplace_t);
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->max_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
			break;
		}


		/**< Inform the caller service whether the module needs data buffering or not.
		    If this value is false, the module must behave as follows:
		    1. The number of output samples should always be the same as the number
		    of input samples on all output ports. The caller must ensure that the
		    number of input samples is the same on all input ports.
		    2. All the input must be consumed. The caller must ensure that the output
		    buffers have enough space.
		    3. The module should be able to handle any number of samples.

		    If this value is true, the module must behave as follows:
		    1. The module must define a threshold in terms of number of bytes for each
		    input port and each output port.
		    2. The module must consume data on its inputs and fill data on its outputs
		    till the amount of remaining data on each buffer of at least one input
		    port is less than its threshold or the amount of free space on each buffer
		    of at least one output port is less than its threshold.

		    Note: Setting this value to true adds significant overhead, so it should
		    only be used if:
		    1. The module performs encoding/decoding of data.
		    2. The module performs rate conversion between the input and output.

		    Payload Structure: capi_v2_requires_data_buffering_t
		 */

		case CAPI_V2_REQUIRES_DATA_BUFFERING:
		{
			if (payload_ptr->max_data_len >= sizeof(capi_v2_requires_data_buffering_t))
			{
				capi_v2_requires_data_buffering_t *data_ptr =
						(capi_v2_requires_data_buffering_t*)payload_ptr->data_ptr;
				data_ptr->requires_data_buffering = TRUE;
				payload_ptr->actual_data_len = sizeof(capi_v2_requires_data_buffering_t);
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->max_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
			break;
		}

		/**< The amount of stack size in bytes needed by this module
		    Payload Structure: capi_v2_stack_size_t */
		case CAPI_V2_STACK_SIZE:
		{
			if (payload_ptr->max_data_len >= sizeof(capi_v2_stack_size_t))
			{
				capi_v2_stack_size_t *data_ptr = (capi_v2_stack_size_t*)payload_ptr->data_ptr;
				data_ptr->size_in_bytes = CAPI_V2_DECIMATE_STACK_SIZE;
				payload_ptr->actual_data_len = sizeof(capi_v2_stack_size_t);
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->max_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
			break;
		}

		/**< The maximum size of metadata generated by this module after each call
		    to process(). If this value is zero, the module does not generate any
		    metadata. It includes size of different structures used to pack
		    metadata (See property CAPI_V2_METADATA).
		    Payload Structure: capi_v2_max_metadata_size_t */
		case CAPI_V2_MAX_METADATA_SIZE:
		{
			if (payload_ptr->max_data_len >= sizeof(capi_v2_max_metadata_size_t))
			{
				capi_v2_max_metadata_size_t *data_ptr =
						(capi_v2_max_metadata_size_t*)payload_ptr->data_ptr;
				data_ptr->size_in_bytes = 0;
				payload_ptr->actual_data_len = sizeof(capi_v2_max_metadata_size_t);
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->max_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
		}
		break;

		/**< The size of the media format payload for a particular output port.
		 *   This excludes the size of
		 * capi_v2_data_format_header_t.
		 * Payload Structure: capi_v2_output_media_format_size_t
		 */
		case CAPI_V2_OUTPUT_MEDIA_FORMAT_SIZE:
		{
			if (payload_ptr->max_data_len >= sizeof(capi_v2_output_media_format_size_t))
			{
				capi_v2_output_media_format_size_t *data_ptr =
						(capi_v2_output_media_format_size_t*)payload_ptr->data_ptr;
				data_ptr->size_in_bytes = sizeof(capi_v2_standard_data_format_t);
				payload_ptr->actual_data_len = sizeof(capi_v2_output_media_format_size_t);
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->max_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
		}
		break;

		/**< Can be used to query the media format for a particular output port.
		 * This property can also be used to set the output media format for modules at support control
		 * of the output media format. If a module only supports controlling some aspects like say the
		 * sample rate only, all other fields can be set to CAPI_V2_DATA_FORMAT_INVALID_VAL.
		 * The port id must be set in the payload by the caller.
		 */
		case CAPI_V2_OUTPUT_MEDIA_FORMAT:
		{
			if (payload_ptr->max_data_len >= sizeof(capi_v2_decimate_media_fmt_t))
			{
				capi_v2_decimate_media_fmt_t *data_ptr =
						(capi_v2_decimate_media_fmt_t*)payload_ptr->data_ptr;

				if ( (NULL == me_ptr) ||
						(prop_array[i].port_info.is_valid && prop_array[i].port_info.port_index != 0 ))
				{
					FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx due to invalid/unexpected values",
							(uint32_t)prop_array[i].id);
					CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_EFAILED);
				}
				else
				{
					*data_ptr = me_ptr->output_media_fmt[0];
					payload_ptr->actual_data_len = sizeof(capi_v2_decimate_media_fmt_t);
				}
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->max_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
		}
		break;

		/**< The number of framework extensions needed by this module.
		    Payload Structure: capi_v2_num_needed_framework_extensions_t */
		case CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS:
		{
			if (payload_ptr->max_data_len >= sizeof(capi_v2_num_needed_framework_extensions_t))
			{
				capi_v2_num_needed_framework_extensions_t *data_ptr =
						(capi_v2_num_needed_framework_extensions_t*)payload_ptr->data_ptr;

				data_ptr->num_extensions = 0;
				payload_ptr->actual_data_len = sizeof(capi_v2_num_needed_framework_extensions_t);
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->max_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
		}
		break;

		/**< The threshold in bytes of an input or output port.
		 * This property is only used for modules that
		 * require data buffering. Refer to the comments for
		 * the CAPI_V2_REQUIRES_DATA_BUFFERING property for usage.
		 * Payload Structure: capi_v2_port_data_threshold_t
		 */
		case CAPI_V2_PORT_DATA_THRESHOLD:
		{
			if (payload_ptr->max_data_len >= sizeof(capi_v2_port_data_threshold_t))
			{
				capi_v2_port_data_threshold_t *data_ptr =
						(capi_v2_port_data_threshold_t*)payload_ptr->data_ptr;
				if (!prop_array[i].port_info.is_valid)
				{
					FARF(ERROR,"CAPIv2 Decimate: get property id not valid");
					CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_EBADPARAM);
					break;
				}
				if (prop_array[i].port_info.is_input_port)
				{
					if (0 != prop_array[i].port_info.port_index)
					{
						FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Max Input Port: 1. "
								"Requesting for index %lu",
								(uint32_t)prop_array[i].id, prop_array[i].port_info.port_index);
						CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_EBADPARAM);
					}
					data_ptr->threshold_in_bytes = 1;
				}
				else
				{
					if (0 != prop_array[i].port_info.port_index)
					{
						FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Max Output Port: 1. "
								"Requesting for index %lu",
								(uint32_t)prop_array[i].id, prop_array[i].port_info.port_index);
						CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_EBADPARAM);
					}
					data_ptr->threshold_in_bytes = 1;
				}
				payload_ptr->actual_data_len = sizeof(capi_v2_port_data_threshold_t);
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: get property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->max_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
			break;
		}

		/**< Provides module-specific initialization data. This property is typically only
		    set at initialization time.
		    Payload Structure: Module-specific */
		case CAPI_V2_CUSTOM_INIT_DATA:
		{
			// Ignore this property.
			break;
		}
		default:
		{
			FARF(HIGH,"CAPIv2 Decimate: get property id for 0x%x is not supported.",prop_array[i].id);
			CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_EUNSUPPORTED);
		}
		break;
		}

		if (CAPI_V2_FAILED(capi_v2_result))
		{
			FARF(ERROR,"CAPIv2 Decimate: get property id for 0x%x failed with opcode %lu",
					prop_array[i].id, capi_v2_result);
		}
		else
		{

			FARF(HIGH,"CAPIv2 Decimate: get property id for 0x%x done",prop_array[i].id);
		}
	}
	return capi_v2_result;
}


/*===========================================================================
	Function name: capi_v2_decimate_init_media_fmt
	DESCRIPTION: Function to set the default media format for decimate
	during the initialization
===========================================================================*/
void capi_v2_decimate_init_media_fmt(capi_v2_decimate_t *me_ptr)
{
	capi_v2_decimate_media_fmt_t *media_fmt_ptr = &(me_ptr->input_media_fmt[0]);

	uint32_t j;

	/**< Set the media format to default state */
	media_fmt_ptr->main.format_header.data_format = CAPI_V2_FIXED_POINT;
	media_fmt_ptr->std_fmt.bits_per_sample = 16;
	media_fmt_ptr->std_fmt.bitstream_format = CAPI_V2_DATA_FORMAT_INVALID_VAL;
	media_fmt_ptr->std_fmt.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
	media_fmt_ptr->std_fmt.data_is_signed = 1;
	media_fmt_ptr->std_fmt.num_channels = CAPI_V2_DATA_FORMAT_INVALID_VAL;
	media_fmt_ptr->std_fmt.q_factor = PCM_16BIT_Q_FORMAT;
	media_fmt_ptr->std_fmt.sampling_rate = CAPI_V2_DATA_FORMAT_INVALID_VAL;

	for (j=0; (j<CAPI_V2_MAX_CHANNELS); j++)
	{
		media_fmt_ptr->std_fmt.channel_type[j] = (uint16_t)CAPI_V2_DATA_FORMAT_INVALID_VAL;
	}
}


/*===========================================================================
	FUNCTION : capi_v2_decimate_process_set_properties
	DESCRIPTION: Function to set properties for decimate example
===========================================================================*/
capi_v2_err_t capi_v2_decimate_process_set_properties(
		capi_v2_decimate_t* me_ptr, capi_v2_proplist_t *proplist_ptr)
{
	capi_v2_err_t  capi_v2_result = CAPI_V2_EOK;
	capi_v2_prop_t *prop_array= proplist_ptr->prop_ptr;
	uint8_t i;
	for ( i=0; i < proplist_ptr->props_num; i++)
	{
		capi_v2_buf_t *payload_ptr = &(prop_array[i].payload);

		switch(prop_array[i].id)
		{
		case CAPI_V2_PORT_NUM_INFO:
		{
			if (payload_ptr->actual_data_len >= sizeof(capi_v2_port_num_info_t))
			{
				capi_v2_port_num_info_t *data_ptr = (capi_v2_port_num_info_t*)payload_ptr->data_ptr;

				if ((data_ptr->num_input_ports > CAPI_V2_DECIMATE_MAX_IN_PORTS) ||
						(data_ptr->num_output_ports > CAPI_V2_DECIMATE_MAX_OUT_PORTS) )
				{
					FARF(ERROR,"CAPIv2 Decimate: Set property id 0x%lx number of input and output ports "
							"cannot be more than 1", (uint32_t)prop_array[i].id);
					CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_EBADPARAM);
				}
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: Set property id  0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->actual_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
		}
		break;

		case CAPI_V2_HEAP_ID:
		{
			if (payload_ptr->actual_data_len >= sizeof(capi_v2_heap_id_t))
			{
				//capi_v2_heap_id_t *data_ptr = (capi_v2_heap_id_t*)payload->data_ptr;
				FARF(HIGH,"CAPIv2 Decimate: Set property id for heap is ignored.");
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: Set property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->actual_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
		}
		break;

		case CAPI_V2_EVENT_CALLBACK_INFO:
		{
			if (payload_ptr->actual_data_len >= sizeof(capi_v2_event_callback_info_t))
			{
				capi_v2_event_callback_info_t *data_ptr =
						(capi_v2_event_callback_info_t*)payload_ptr->data_ptr;
				if (NULL == data_ptr)
				{
					CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_EBADPARAM);
				}
				else
				{
					me_ptr->cb_info = *data_ptr;
				}
				FARF(HIGH,"CAPIv2 Decimate: Set property id for Event CallBack done.");
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: Set property id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->actual_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
		}
		break;

		case CAPI_V2_ALGORITHMIC_RESET:
		{
			FARF(HIGH,"CAPIv2 Decimate: received RESET");
		}
		break;

		case CAPI_V2_INPUT_MEDIA_FORMAT:
		{
			if (payload_ptr->actual_data_len >= sizeof(capi_v2_decimate_media_fmt_t))
			{
				FARF(HIGH,"CAPIv2 Decimate: received input media fmt");

				capi_v2_decimate_media_fmt_t *in_data_ptr =
						(capi_v2_decimate_media_fmt_t *) (payload_ptr->data_ptr);

				//copy and save the input media fmt
				me_ptr->input_media_fmt[0].std_fmt = in_data_ptr->std_fmt;

				me_ptr->output_media_fmt[0].std_fmt = in_data_ptr->std_fmt;
				uint32_t output_sampling_rate = me_ptr->input_media_fmt[0].std_fmt.sampling_rate;
				me_ptr->output_media_fmt[0].std_fmt.sampling_rate = output_sampling_rate;
				capi_v2_decimate_raise_event(me_ptr);

			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: Set Param id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->actual_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
		}
		break;

		case CAPI_V2_OUTPUT_MEDIA_FORMAT:
		{
			if (payload_ptr->actual_data_len >= sizeof(capi_v2_decimate_media_fmt_t))
			{
				FARF(HIGH,"CAPIv2 Decimate: received output media fmt");

				capi_v2_decimate_media_fmt_t *data_ptr =
						(capi_v2_decimate_media_fmt_t *) (payload_ptr->data_ptr);
				if(me_ptr->output_media_fmt[0].std_fmt.sampling_rate != data_ptr->std_fmt.sampling_rate)
				{
					me_ptr->output_media_fmt[0].std_fmt.sampling_rate = data_ptr->std_fmt.sampling_rate;
					capi_v2_decimate_raise_event(me_ptr);
				}
			}
			else
			{
				FARF(ERROR,"CAPIv2 Decimate: Set Param id 0x%lx Bad param size %lu",
						(uint32_t)prop_array[i].id, payload_ptr->actual_data_len);
				CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_ENEEDMORE);
			}
		}
		break;

		case CAPI_V2_CUSTOM_INIT_DATA:
		{
			FARF(ERROR,"CAPIv2 Decimate: Set property id custom Init data unsupported! ");
			break;
		}
		default:
		{
			FARF(HIGH,"CAPIv2 Decimate: Set property for 0x%x. Not supported.",prop_array[i].id);
			CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_EUNSUPPORTED);
		}
		break;

		}

		if (CAPI_V2_FAILED(capi_v2_result))
		{
			FARF(HIGH,"CAPIv2 Decimate: Set property for 0x%x failed with opcode %lu",
					prop_array[i].id, capi_v2_result);
		}
		else
		{
			FARF(HIGH,"CAPIv2 Decimate: Set property for 0x%x done", prop_array[i].id);
		}
	}
	return capi_v2_result;
}

/*===========================================================================
FUNCTION : capi_v2_decimate_release_memory
DESCRIPTION: Function to release allocated memory
===========================================================================*/
void capi_v2_decimate_release_memory(capi_v2_decimate_t *me_ptr)
{
	memset(&me_ptr->decimate_mem ,0,sizeof(me_ptr->decimate_mem));
}

/*===========================================================================
	FUNCTION : capi_v2_decimate_update_event_states
	DESCRIPTION: Function to update the event states for software decimation
===========================================================================*/
static void capi_v2_decimate_update_event_states(capi_v2_decimate_t* me_ptr)
{
	int16_t input_delay = DECIMATE_DELAY_US;
	uint64_t delay_in_us_64 =
			(input_delay * (uint64_t)(1000000))/ me_ptr->lib_config.config_params[1];
	if( 1 == me_ptr->enable_flag )
	{
		me_ptr->events_config.enable = 1;
	}
	else
	{
		me_ptr->events_config.enable = 0;
	}

	me_ptr->events_config.KPPS = capi_v2_decimate_get_kpps(me_ptr);

	me_ptr->events_config.delay = (delay_in_us_64 > UINT32_MAX) ? UINT32_MAX : delay_in_us_64;
}


/*===========================================================================
	FUNCTION : capi_v2_decimate_get_kpps
	DESCRIPTION: Function to update KPPS
===========================================================================*/
int capi_v2_decimate_get_kpps(capi_v2_decimate_t* me_ptr)
{
	return 5000; // roughly profiled value
}


/*===========================================================================
	FUNCTION : capi_v2_decimate_raise_event
	DESCRIPTION: Function to raise all the events using the callback function
===========================================================================*/
void capi_v2_decimate_raise_event(capi_v2_decimate_t *me_ptr)
{
	if (NULL == me_ptr->cb_info.event_cb)
	{
		FARF(HIGH,"CAPIv2 Decimate: Event callback is not set."
				" Unable to raise output media format event!");
		return;
	}
	capi_v2_decimate_raise_output_media_format_event(me_ptr);
	capi_v2_decimate_update_event_states(me_ptr);
	capi_v2_decimate_raise_process_event(me_ptr);
}


/*===========================================================================
	FUNCTION : capi_v2_decimate_output_media_format_event
	DESCRIPTION: Function to send the output media format using the
	callback function
===========================================================================*/
void capi_v2_decimate_raise_output_media_format_event(capi_v2_decimate_t *me_ptr)
{
	capi_v2_err_t  capi_v2_result = CAPI_V2_EOK;

	// Raise an event
	capi_v2_event_info_t    event_info;

	event_info.port_info.is_valid = TRUE;
	event_info.port_info.is_input_port = FALSE;
	event_info.port_info.port_index = 0;
	event_info.payload.actual_data_len = event_info.payload.max_data_len =
			sizeof(me_ptr->output_media_fmt);
	event_info.payload.data_ptr =
			reinterpret_cast<int8_t*>(&me_ptr->output_media_fmt[0]);

	capi_v2_result = me_ptr->cb_info.event_cb(me_ptr->cb_info.event_context,
			CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED, &event_info);
	if (CAPI_V2_FAILED(capi_v2_result))
	{
		FARF(ERROR,"CAPIv2 Decimate: Failed to send output media format updated event with %lu",
				capi_v2_result);
	}
}

/*===========================================================================
	FUNCTION : capi_v2_decimate_raise_process_event
	DESCRIPTION: Function to send process check using the callback function
===========================================================================*/
void capi_v2_decimate_raise_process_event(capi_v2_decimate_t *me_ptr)
{
	if (NULL == me_ptr->cb_info.event_cb)
	{
		FARF(ERROR,"CAPIv2 Decimate: Event callback is not set. Unable to raise process event!");
		return;
	}
	capi_v2_err_t  capi_v2_result = CAPI_V2_EOK;

	capi_v2_event_process_state_t event;
	event.is_enabled = (bool_t)((me_ptr->events_config.enable ==0) ? 0 : 1);
	capi_v2_event_info_t event_info;
	event_info.port_info.is_valid = FALSE;
	event_info.payload.actual_data_len = sizeof(event);
	event_info.payload.max_data_len = sizeof(event);
	event_info.payload.data_ptr = (int8_t*)(&event);
	capi_v2_result = me_ptr->cb_info.event_cb(me_ptr->cb_info.event_context,CAPI_V2_EVENT_PROCESS_STATE,
			&event_info);
	if (CAPI_V2_FAILED(capi_v2_result))
	{
		FARF(ERROR,
				"CAPIv2 Decimate: Failed to send process_check update event with %lu",
				capi_v2_result);
	}
}

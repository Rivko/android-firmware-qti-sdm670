/* ======================================================================== */
/**
   @file capi_v2_decimate.cpp

   C source file to implement the Audio Post Processor Interface for
   DECIMATE example
 */

/* =========================================================================
   Copyright (c) 2015 QUALCOMM Technologies Incorporated.
   All rights reserved. Qualcomm Technologies Proprietary and Confidential.
   ========================================================================= */
/*------------------------------------------------------------------------
 * Include files and Macro definitions
 * -----------------------------------------------------------------------*/

#ifndef _DEBUG  // To enable FARF messages
#define _DEBUG
#endif
#include "capi_v2_decimate_calib.h"
#include "capi_v2_decimate.h"
#include "capi_v2_decimate_utils.h"
#include <assert.h>
#include <hexagon_protos.h>
#include "decimate_block.h"


/*------------------------------------------------------------------------
 * Static declarations
 * -----------------------------------------------------------------------*/

static inline uint32_t align_to_8_byte(const uint32_t num)
{
   return ((num + 7) & (0xFFFFFFF8));
}


static capi_v2_err_t capi_v2_decimate_process(capi_v2_t* _pif,
		capi_v2_stream_data_t* input[],
		capi_v2_stream_data_t* output[]);

static capi_v2_err_t capi_v2_decimate_end(capi_v2_t* _pif);

static capi_v2_err_t capi_v2_decimate_set_param(capi_v2_t* _pif,
		uint32_t param_id,
		const capi_v2_port_info_t *port_info_ptr,
		capi_v2_buf_t *params_ptr);

static capi_v2_err_t capi_v2_decimate_get_param(capi_v2_t* _pif,
		uint32_t param_id,
		const capi_v2_port_info_t *port_info_ptr,
		capi_v2_buf_t *params_ptr);

static capi_v2_err_t capi_v2_decimate_set_properties(capi_v2_t* _pif,
		capi_v2_proplist_t *props_ptr);

static capi_v2_err_t capi_v2_decimate_get_properties(capi_v2_t* _pif,
		capi_v2_proplist_t *props_ptr);

/** Virtual table for capi_v2_decimate
 *  */
static capi_v2_vtbl_t vtbl =
{
		capi_v2_decimate_process,
		capi_v2_decimate_end,
		capi_v2_decimate_set_param,
		capi_v2_decimate_get_param,
		capi_v2_decimate_set_properties,
		capi_v2_decimate_get_properties
};

/*------------------------------------------------------------------------
  Function name: capi_v2_decimate_get_static_properties
  DESCRIPTION: Function to get the static properties of DECIMATE module
  -----------------------------------------------------------------------*/
capi_v2_err_t capi_v2_decimate_get_static_properties (
		capi_v2_proplist_t *init_set_properties,
		capi_v2_proplist_t *static_properties)
{
	capi_v2_err_t capi_v2_result = CAPI_V2_EOK;

	if (NULL != static_properties)
	{
		capi_v2_result =
				capi_v2_decimate_process_get_properties(
						(capi_v2_decimate_t*)NULL, static_properties);
		if (CAPI_V2_FAILED(capi_v2_result))
		{
			return capi_v2_result;
		}
	}

	if (NULL != init_set_properties)
	{
		FARF(HIGH,"CAPIv2 Decimate: get static properties ignoring init_set_properties!");
	}

	return capi_v2_result;
}


/*------------------------------------------------------------------------
  Function name: capi_v2_decimate_init
  DESCRIPTION: Initialize the CAPIv2 Decimate module and library.
  This function can allocate memory.
  -----------------------------------------------------------------------*/
capi_v2_err_t capi_v2_decimate_init (capi_v2_t* _pif,
		capi_v2_proplist_t      *init_set_properties)
{
	capi_v2_err_t  capi_v2_result = CAPI_V2_EOK;

	if (NULL == _pif || NULL == init_set_properties )
	{
		FARF(ERROR,"CAPIv2 Decimate: "
				"Init received bad pointer, 0x%lx, 0x%lx",
				(uint32_t)_pif, (uint32_t)init_set_properties);

		return CAPI_V2_EBADPARAM;
	}

	int8_t* ptr = (int8_t*)_pif;
	capi_v2_decimate_t *me_ptr = (capi_v2_decimate_t *)ptr;
	uint32_t decimate_size = 0;

	// Each block must begin with 8-byte aligned memory.
	decimate_size = align_to_8_byte(sizeof(capi_v2_decimate_t))
					+ align_to_8_byte(TEMP_BUF_SIZE_IN_SAMPLES * sizeof(int16_t))   // For L-channel
					+ align_to_8_byte(TEMP_BUF_SIZE_IN_SAMPLES * sizeof(int16_t));  // For R-channel

	// Initialize the memory to 0
	memset((void *)me_ptr, 0, decimate_size);

	/* Shift the memory pointer by size of capi_v2_decimate_t
	   for other members of the structure.
	 */
	ptr += align_to_8_byte(sizeof(capi_v2_decimate_t));

	// Allocate memory to output samples after decimation
	me_ptr->out_samples = (uint32_t*)ptr;
	memset(me_ptr->out_samples, 0, sizeof(uint32_t));
	ptr += align_to_8_byte(sizeof(uint32_t));

	// Disable the module by default
	me_ptr->enable_flag = 0;
	me_ptr->lib_config.decimatest_frame= 1;

	capi_v2_decimate_init_media_fmt(me_ptr);
	me_ptr->vtbl.vtbl_ptr = &vtbl;

	me_ptr->decimation_factor = 1;

	//should contain  EVENT_CALLBACK_INFO, PORT_INFO
	capi_v2_result = capi_v2_decimate_process_set_properties(me_ptr, init_set_properties);

	//Ignoring non-fatal error code.
	capi_v2_result ^= (capi_v2_result & CAPI_V2_EUNSUPPORTED);
	if (CAPI_V2_FAILED(capi_v2_result))
	{
		return capi_v2_result;
	}

	FARF(HIGH, "CAPIv2 Decimate: Init done!");
	return capi_v2_result;
}

/*------------------------------------------------------------------------
  Function name: capi_v2_decimate_process
  DESCRIPTION: Processes an input buffer and generates an output buffer.
  -----------------------------------------------------------------------*/
static capi_v2_err_t capi_v2_decimate_process(capi_v2_t* _pif,
		capi_v2_stream_data_t* input[],
		capi_v2_stream_data_t* output[])
{

	capi_v2_err_t  capi_v2_result = CAPI_V2_EOK;
	capi_v2_decimate_t* me_ptr = (capi_v2_decimate_t*)(_pif);

	uint32_t decimation_factor = me_ptr->decimation_factor;
	assert(me_ptr);
	assert(input);
	assert(output);

	// Data pointers and sizes
	// Sample count from one channel should be sufficient
	int16_t* lin_ptr = NULL;
	int16_t* rin_ptr = NULL;
	int16_t* lout_ptr = NULL;
	int16_t* rout_ptr = NULL;
	uint32_t offset_into_inbuf = 0;  // Offset for input buffer
	uint32_t offset_into_outbuf = 0; // Offset for output buffer

	/* The processing block size should be a minimum of
	 *   1. input_actual_data_length/decimation_factor
	 *   2. output_max_data_length
	 *
	 *   Also, the input_actual_data_length and ooutput_max_data_length
	 *   is in bytes. Assuming bits per sample is 16 or 2 bytes per sample,
	 *   num of samples are calculated by bitwise shifting by 1 byte to the right.
	 * */
	uint32_t num_samples = Q6_R_min_RR(
			(input[0]->buf_ptr->actual_data_len>> 1)/decimation_factor,
			(output[0]->buf_ptr->max_data_len >> 1));

	// Input and output buffers for L channel processing
	lin_ptr  = (int16_t*)input[0]->buf_ptr[0].data_ptr;
	lout_ptr = (int16_t*)output[0]->buf_ptr[0].data_ptr;

	// Input and output buffers for R channel processing
	if (2 == output[0]->bufs_num) {
		rin_ptr  = (int16_t*)input[0]->buf_ptr[1].data_ptr;
		rout_ptr = (int16_t*)output[0]->buf_ptr[1].data_ptr;
	}

	if (1 == me_ptr->enable_flag) {
		// Primary processing
		uint32_t num_samples_for_this_iteration = 0;
		uint32_t rem_num_samples = decimation_factor*num_samples;

		do {
			num_samples_for_this_iteration = Q6_R_min_RR(rem_num_samples,
					TEMP_BUF_SIZE_IN_SAMPLES);

			decimate_block(lin_ptr + offset_into_inbuf,
					num_samples_for_this_iteration,
					me_ptr->out_samples,
					decimation_factor,
					lout_ptr + offset_into_outbuf);

			if(2 == output[0]->bufs_num) {
				decimate_block(rin_ptr + offset_into_inbuf,
						num_samples_for_this_iteration,
						me_ptr->out_samples,
						decimation_factor,
						rout_ptr + offset_into_outbuf);
			}

			// Check if any more input data is left to be processed
			// Update Offset into Input buffer, so we begin reading from that
			// point in the next iteration (if any).
			offset_into_inbuf += num_samples_for_this_iteration;
			offset_into_outbuf += *me_ptr->out_samples;
			rem_num_samples -= num_samples_for_this_iteration;
		}
		while (rem_num_samples);

		//Update actual data length for output buffer
		input[0]->buf_ptr[0].actual_data_len = ((decimation_factor*num_samples)<<1);
		output[0]->buf_ptr[0].actual_data_len = offset_into_outbuf<<1;
		if (2 == output[0]->bufs_num) {
			output[0]->buf_ptr[1].actual_data_len = offset_into_outbuf<<1;
			input[0]->buf_ptr[1].actual_data_len = ((decimation_factor*num_samples)<<1);
		}

	}
	else /* This case will never be hit because if enable_flag is not 1, module will
	 * raise an event to service to disable the module from PP chain.
	 * It's a dummy code for copying data from input to output.
	 */
	{
		memcpy(output[0]->buf_ptr[0].data_ptr, input[0]->buf_ptr[0].data_ptr,
				input[0]->buf_ptr->actual_data_len);
		if (2 == output[0]->bufs_num) {
			memcpy(output[0]->buf_ptr[1].data_ptr, input[0]->buf_ptr[1].data_ptr,
					input[0]->buf_ptr->actual_data_len);
		}
		//Update actual data length for output buffer
		output[0]->buf_ptr[0].actual_data_len = input[0]->buf_ptr->actual_data_len;
		if (2 == output[0]->bufs_num) {
			output[0]->buf_ptr[1].actual_data_len = input[0]->buf_ptr->actual_data_len;
		}

	}

	return capi_v2_result;
}

/*------------------------------------------------------------------------
  Function name: capi_v2_decimate_end
  DESCRIPTION: Returns the library to the uninitialized state and frees the
  memory that was allocated by init(). This function also frees the virtual
  function table.
  -----------------------------------------------------------------------*/
static capi_v2_err_t capi_v2_decimate_end(capi_v2_t* _pif)
{
	capi_v2_err_t  capi_v2_result = CAPI_V2_EOK;
	if (NULL == _pif)
	{
		FARF(ERROR,"CAPIv2 Decimate: End received bad pointer, 0x%lx",(uint32_t)_pif);
		return CAPI_V2_EBADPARAM;
	}

	capi_v2_decimate_t* me_ptr = (capi_v2_decimate_t*)(_pif);

	capi_v2_decimate_release_memory(me_ptr);

	me_ptr->vtbl.vtbl_ptr = NULL;

	FARF(HIGH,"CAPIv2 Decimate: End done");
	return capi_v2_result;
}


/*------------------------------------------------------------------------
  Function name: capi_v2_decimate_set_param
  DESCRIPTION: Sets either a parameter value or a parameter structure containing
  multiple parameters. In the event of a failure, the appropriate error code is
  returned.
  -----------------------------------------------------------------------*/
static capi_v2_err_t capi_v2_decimate_set_param(capi_v2_t* _pif,
		uint32_t param_id,
		const capi_v2_port_info_t *port_info_ptr,
		capi_v2_buf_t *params_ptr)

{
	if (NULL == _pif || NULL == params_ptr)
	{
		FARF(ERROR,"CAPIv2 Decimate: Set param received bad pointer, 0x%lx, 0x%lx",
				(uint32_t)_pif, (uint32_t)params_ptr);
		return CAPI_V2_EBADPARAM;
	}

	capi_v2_err_t  capi_v2_result  = CAPI_V2_EOK;
	capi_v2_decimate_t* me_ptr = (capi_v2_decimate_t*)(_pif);

	switch (param_id)
	{

	case CAPI_V2_PARAM_ID_DECIMATE_ENABLE:
	{
		if (params_ptr->actual_data_len >= sizeof(decimate_filter_enable_cfg_t)) {
			decimate_filter_enable_cfg_t* cfg_ptr =
					(decimate_filter_enable_cfg_t*)(params_ptr->data_ptr);
			//set DECIMATE enable flag
			me_ptr->enable_flag = cfg_ptr->enable_flag;

			/* After enabling/disabling the module, use callback functionality
			 * to update the caller service about the process state of module.
			 */
			capi_v2_decimate_raise_event(me_ptr);
			FARF(HIGH, "CAPIv2 Decimate : <<set_param>> Enable/Disable %lu   ", me_ptr->enable_flag);
		} else {
			FARF(ERROR, "CAPIv2 Decimate : <<set_param>> Bad param size %lu  ",
					params_ptr->actual_data_len);
			return CAPI_V2_ENEEDMORE ;
		}
		break;
	}


	case CAPI_V2_PARAM_ID_UPDATE_DECIMATION_FACTOR:
	{
		if (params_ptr->actual_data_len >= sizeof(decimate_factor_t)) {
			decimate_factor_t* cfg_ptr =
					(decimate_factor_t*)(params_ptr->data_ptr);

			if( 1 <= cfg_ptr->decimation_factor){
				me_ptr->decimation_factor = cfg_ptr->decimation_factor;
				FARF(HIGH, "CAPIv2 Decimate : <<set_param>> Decimation factor set to %12lu",
						me_ptr->decimation_factor);
			}
			else {
				FARF(ERROR,"CAPIv2 Decimate: <<set_param>> "
						"Decimation factor should be greater than 1 ");
				return CAPI_V2_EUNSUPPORTED ;
			}
		} else {
			FARF(ERROR,"CAPIv2 Decimate: <<set_param>> Bad param size %12lu",
					params_ptr->actual_data_len);
			return CAPI_V2_ENEEDMORE ;
		}
		break;
	}

	default:
		FARF(ERROR,"CAPIv2 Decimate: Set unsupported param ID 0x%x",(int)param_id);
		CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_EBADPARAM);
	}

	FARF(HIGH,"CAPIv2 Decimate: Set param done");
	return capi_v2_result;
}



/*------------------------------------------------------------------------
  Function name: capi_v2_decimate_get_param
  DESCRIPTION: Gets either a parameter value or a parameter structure
  containing multiple parameters. In the event of a failure, the appropriate
  error code is returned.
 * -----------------------------------------------------------------------*/
static capi_v2_err_t capi_v2_decimate_get_param(capi_v2_t* _pif,
		uint32_t param_id,
		const capi_v2_port_info_t *port_info_ptr,
		capi_v2_buf_t *params_ptr)
{
	if (NULL == _pif || NULL == params_ptr)
	{
		FARF(ERROR,"CAPIv2 Decimate: Get param received bad pointer, 0x%lx, 0x%lx",
				(uint32_t)_pif, (uint32_t)params_ptr);
		return CAPI_V2_EBADPARAM;
	}

	capi_v2_err_t  capi_v2_result  = CAPI_V2_EOK;
	capi_v2_decimate_t* me_ptr = (capi_v2_decimate_t*)(_pif);
	//void *param_payload_ptr  = (void *)(params_ptr->data_ptr);

	switch (param_id)
	{

	case CAPI_V2_PARAM_ID_DECIMATE_ENABLE:
	{
		if (params_ptr->max_data_len >= sizeof(decimate_filter_enable_cfg_t)) {
			decimate_filter_enable_cfg_t* cfg_ptr =
					(decimate_filter_enable_cfg_t*)(params_ptr->data_ptr);
			cfg_ptr->enable_flag = me_ptr->enable_flag;
			params_ptr->actual_data_len = sizeof(decimate_filter_enable_cfg_t);
			FARF(HIGH, "<<get_param>> Enable/Disable %lu                                    ",
					cfg_ptr->enable_flag);
		} else {
			FARF(ERROR, "<<get_param>> Bad param size %lu                                    ",
					params_ptr->max_data_len);
			return CAPI_V2_ENEEDMORE;
		}
		break;
	}

	case CAPI_V2_PARAM_ID_UPDATE_DECIMATION_FACTOR:
	{
		if (params_ptr->max_data_len >= sizeof(decimate_factor_t)) {
			decimate_factor_t* cfg_ptr =
					(decimate_factor_t*)(params_ptr->data_ptr);
			cfg_ptr->decimation_factor = me_ptr->decimation_factor;
			params_ptr->actual_data_len = sizeof(decimate_factor_t);
			FARF(HIGH, "<<get_param>> Decimation factor = %lu                                              ",
					cfg_ptr->decimation_factor);
		} else {
			FARF(ERROR, "<<get_param>> Bad param size %lu  Param id = %lu",
					params_ptr->max_data_len, param_id);
			return CAPI_V2_ENEEDMORE;
		}
		break;
	}


	default :
	{
		FARF(ERROR,"CAPIv2 Decimate: Get unsupported param ID 0x%x",(int)param_id);
		CAPI_V2_SET_ERROR(capi_v2_result,CAPI_V2_EBADPARAM);
	}
	}

	FARF(HIGH,"CAPIv2 Decimate: Get param done");
	return capi_v2_result;
}

/*------------------------------------------------------------------------
  Function name: capi_v2_decimate_set_properties
  DESCRIPTION: Function to set the properties for the Decimate module
 * -----------------------------------------------------------------------*/
static capi_v2_err_t capi_v2_decimate_set_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr)
{
	capi_v2_err_t  capi_v2_result = CAPI_V2_EOK;
	if (NULL == _pif)
	{
		FARF(ERROR,"CAPIv2 Decimate: Set properties received bad pointer, 0x%lx",(uint32_t)_pif);
		return CAPI_V2_EBADPARAM;
	}
	capi_v2_decimate_t *me_ptr = (capi_v2_decimate_t *)_pif;
	capi_v2_result = capi_v2_decimate_process_set_properties(me_ptr, props_ptr);
	return capi_v2_result;
}


/*------------------------------------------------------------------------
  Function name: capi_v2_decimate_get_properties
  DESCRIPTION: Function to get the properties for the Decimate module
 * -----------------------------------------------------------------------*/
static capi_v2_err_t capi_v2_decimate_get_properties(capi_v2_t* _pif, capi_v2_proplist_t *props_ptr)
{
	capi_v2_err_t  capi_v2_result = CAPI_V2_EOK;
	if (NULL == _pif)
	{
		FARF(ERROR,"CAPIv2 Decimate: Get properties received bad pointer, 0x%lx",(uint32_t)_pif);
		return CAPI_V2_EBADPARAM;
	}
	capi_v2_decimate_t *me_ptr = (capi_v2_decimate_t *)_pif;
	capi_v2_result = capi_v2_decimate_process_get_properties(me_ptr, props_ptr);
	return capi_v2_result;
}


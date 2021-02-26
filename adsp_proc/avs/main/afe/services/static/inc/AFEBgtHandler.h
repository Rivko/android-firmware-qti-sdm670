/*========================================================================
   This file contains AFE Background Task Handler implementation

  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: TBD
 ====================================================================== */
#ifndef __AFE_BGT_HANDLER_H__
#define __AFE_BGT_HANDLER_H__

/*==========================================================================
  Include files
  ========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg.h"
#include "EliteMsg_Custom.h"
#include "EliteMsg_Data.h"
#include "adsp_error_codes.h"
#include "AFEDevService.h"
#include "EliteMsg_AfeCustom.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================
  Macro defines
========================================================================== */
#define AFE_BGT_POOL_SIZE 			10

// TBD: Need to profile the stack requirements for SW MAD algo and then adjust this number
#define  SW_MAD_STACK_SIZE				2048

#define  AFE_FBSP_EXCURSION_BG_THREAD_STACK_SIZE				2048

//Mask for cmdQ
#define AFE_BGT_CMDQ_MASK 			0x00000001
//mask for dataQ
#define AFE_BGT_DATAQ_MASK 			0x00000002
//mask for signal
#define AFE_BGT_SIG_MASK 	                0x00000004
//Mask for RespQ
#define AFE_BGT_RESPQ_MASK 			0x00000008



//Denote the type of the algo module to be run for this background thread
typedef enum afe_algo_type
{
	AFE_SWMAD = 0,
}afe_algo_type;

typedef struct bgt_info
{
	elite_svc_handle_t   bgt_svc_handle;
	elite_svc_handle_t   *input_client_svc_h_ptr;
	qurt_elite_channel_t channel;                 //Channels used by each afe bgt handler
	qurt_elite_thread_t  threadID;                //ThreadID of each afe bgt handler thread.
	bool_t               is_usable;               //Denote if this bgt_info struct is usable or not, default value is true.
	void                 *bgt_algo_h;            // to track the algorithm data memory
	afe_algo_type        algo_type;               //Denote the type of the afe algo in this afe bgt handler.

	void                 *port_ptr;               //Pointer to the port structure associated with the bgt.
	qurt_elite_signal_t  *bgt_signal_ptr;         //pointer to signal on which BGT waits
	qurt_elite_queue_t   *resp_q_ptr;

	ADSPResult           cmd_result;              //Used to propagate the result of command received by BG processing
	qurt_elite_mutex_t   bgt_mutex;      //Denote the mutex for sync between bg thread and other thread for cmd_result.

	//Function pointers to each afe bgt handler, to be handled in afe_bgt_create()
	//As is suggested in the names, there are init, de-init, and process functions.
	ADSPResult (*init_func_ptr)(void *cfg_ptr,void **data_ptr);
	ADSPResult (*deinit_func_ptr)(void **data_ptr);
	ADSPResult (*process_func_ptr)(void *data_ptr);
} afe_bgt_info_t;

typedef struct afe_bgt_svc_info
{
	uint8_t 		bgt_pool_is_inited;
	afe_bgt_info_t  afe_bgt_pool[AFE_BGT_POOL_SIZE];
} afe_bgt_svc_info_t;

/*==========================================================================
  Function Declarations
========================================================================== */
/**
 * This function initializes afe bgt pool
 *
 * @param[in] none
 *
 * @return none
 */
void afe_bgt_pool_init(void);

/**
 * This function creates afe bgt thread and initializes parameters
 *
 * @param[in] bgt_info_handle: address of the pointer to the bgt_info_pool.
 * @param[in] algo: algo type of the created afe bgt handler
 *
 * @return ADSPResult
 */
ADSPResult afe_bgt_create(void **bgt_info_handle, afe_algo_type algo,void *port_ptr);

/**
 * This function destroys afe bgt thread
 *
 * @param[in] bgt_info_handle: ptr to the bgt_info struct created by afe_bgt_create
 * @return ADSPResult
 */
ADSPResult afe_bgt_destroy(void *bgt_info_handle);

/**
 * This function sends afe bgt commands to or from the background thread.
 *
 * @param[in] bgt_info_handle: ptr to the bgt_info struct created by afe_bgt_create
 * @param[in] algo_cfg_ptr: ptr to the payload of the cmd, generally algo cfg structs.
 * @param[in] opCode: opCode of the cmd to be sent.
 * @param[in] dir: Direction of the message, whether input or output of this BG task
 *
 * @return ADSPResult
 */
ADSPResult afe_bgt_send_cmd(void *bgt_info_handle, void *algo_cfg_ptr, uint32_t opCode);

/**
 * This function sends afe bgt data to or from the background thread.
 *
 * @param[in] bgt_info_handle: ptr to the bgt_info struct created by afe_bgt_create
 * @param[in] data_ptr: ptr to the payload of the data msg, generally data to be sent.
 * @param[in] size: size of the data payload to be sent.
 * @param[in] dir: Direction of the message, whether input or output of this BG task
 * @return ADSPResult
 */
ADSPResult afe_bgt_send_data(void *bgt_info_handle, void *data_ptr, uint32_t size);

ADSPResult afe_bgt_send_data_to_client(void *pDevPort, void *pClientInfo, int8_t* out_buf_ptr, uint16_t num_data_bytes);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // __AFE_BGT_HANDLER_H__

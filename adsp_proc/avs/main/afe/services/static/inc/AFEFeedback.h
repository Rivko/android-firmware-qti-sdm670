/** @file AFEFeedback.h

 @brief This file contains AFE service Feedback path utility
 functions

 */

/*========================================================================
 $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFEFeedback.h#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 4/8/2014 mangeshk   Initial version
 ==========================================================================*/


/*-----------------------------------------------------------------------
   Copyright (c) 2012-2013 Qualcomm  Technologies, Inc.  All rights reserved.
   Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

#ifndef _AFE_FEEDBACK_H_
#define _AFE_FEEDBACK_H_

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Data.h"
#include "adsp_error_codes.h"
#include "adsp_afe_service_commands.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


#define AFE_FB_DEV_BUFQ_MAX			32 
#define AFE_FB_DEV_DATQ_MAX			16


/** @brief AFE custom client, info shared by the clients */
typedef struct afe_custom_client_t
{
    elite_svc_handle_t*          client_handle;
                                 /**< Client's handle. */
    uint32_t                     num_channel;
                                 /**< Number of channels */
    uint32_t                     channel_mapping[AFE_PORT_MAX_CHANNEL_CNT]; 
                                 /**< channel mapping */
    uint32_t                     buf_size;
                                 /**< Output buffer size */
    uint32_t                     num_buffers;
                                 /**< Number of buffers in output queue */
	uint32_t					 data_type;
								 /**< data type */
} afe_custom_client_t;


/** @brief Used in the AFE to store information about the
  custom clients that connect to the AFE. 
 */
typedef struct afe_custom_client_info_t
{
	elite_svc_handle_t				this_client_manager;
                                    /**< Contains the AFE's handle provided
                                          to the client. */
	qurt_elite_queue_t             	*bufQ;
                                    /**< Tx path: buffer queue over which more
                                          buffers are extracted;
                                          Rx path: not applicable. */
	afe_custom_client_t             afe_custom_client;
                                    /**< Client information from the client.*/
	uint16_t                         client_id;
                                    /**< Client ID. */
	qurt_elite_channel_t       		qurt_elite_channel;
										/**< client-corresponding channel.*/
  
} afe_custom_client_info_t;


typedef struct afe_feedback_path_t
{
   elite_svc_handle_t		*dst_port_handle_ptr; 
							/**< Handle to the Dst port in a feedback for both thermal and excursion clients. */
   elite_svc_handle_t		*src_port_handle_ptr;
							/**< Handle of the Src port in a feedback for both thermal and excursion clients. */
   void						*src_port_client_ptr;
							/**< Client information of the Src port in a
                                  feedback for both thermal and excursion clients. */
}afe_feedback_path_t; 



/** @brief Multiple Feedback connection information; applicable for a Src port
 *  	   only. Each module can have 1 FB connection. */
typedef struct afe_port_multi_feedback_info_t
{
   afe_fbsp_feedback_path_cfg_v1_t feedback_cfg;
   /**< Feedback path configuration. */

   afe_feedback_path_t             *feedback_path[AFE_MAX_MODULES_PER_TOPOLOGY];
} afe_port_multi_feedback_info_t;


/**
  This function set the feedback path params

  @param[in] pDevPort Pointer to the instance of the port.
  @param[in] param_id  Parameter id  .
  @param[in] param_buffer_ptr Pointer from where the calibration
        data to fetch.
  @param[in] params_buffer_len_req_ptr  Actual Param size
   

  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_port_set_feedback_path_params(void *dev_port_ptr,uint32_t param_id,
                                                      int8_t* params_buffer_ptr,uint16_t param_size);

/**
  This function is to enable feedback path

  @param[in] dev_port_ptr Pointer to the instance of the port.
  @param[in] feedback_path_info_ptr  pointer to the feedback 
    	path info structure
   
  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_port_enable_feedback_path(uint32_t src_id,
                                         afe_fbsp_feedback_path_cfg_v1_t *feedback_path_cfg_ptr,
                                         uint32_t *feedback_handle_ptr);
/**
  This function is to disable feedback path

  @param[in] dev_port_ptr Pointer to the instance of the port.
  @param[in] feedback_path_info_ptr  pointer to the feedback 
    	path info structure
   
  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_port_disable_feedback_path(uint32_t src_id, uint32_t dest_id, uint32_t feedback_handle);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_AFE_FEEDBACK_H_




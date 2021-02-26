/*========================================================================
  This file contains AFE PCM logging related apis

  Copyright (c) 2009-2012, 2016 QUALCOMM Technologies, Inc. All Rights Reserved. 
  QUALCOMM Technologies, Inc Proprietary. Export of this technology or 
  software is regulated by the U.S. Government, Diversion contrary to 
  U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFELoggingUtils.h#1 $
 ====================================================================== */
#ifndef _AFE_LOGGING_UTILS_H_
#define _AFE_LOGGING_UTILS_H_



/*==========================================================================
  Include files
  ========================================================================== */
#include "qurt_elite.h"
#include "AFEInternal.h"
#include "log.h"
#include "diagpkt.h"
#include "EliteLoggingUtils_i.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/*==========================================================================
	  Macro Defines
========================================================================== */

/** Channel type  -- Also doubles as tap point id.
 * This is just offset for the tap point and used only for file name purpose due to legacy reasons.*/
#define AUDIOLOG_CHAN_PCM_RX_INTERNAL_AFE    0x0100

#define AUDIOLOG_GROUP_AFE                   0x0200 


/** buffer size in ms */
const uint32_t AFE_PCM_BUF_SIZE_1MS = 1;
const uint32_t AFE_PCM_BUF_SIZE_10MS = 10;

/**
 * This function initializes logging for afe ports
 *
 * @param[in] pDevPort Ptr to port struct
 *
 */
void afe_port_data_logging_init(afe_dev_port_t *pDevPort);
/**
 * This function deinitializes logging for afe ports
 *
 * @param[in] pDevPort Ptr to port struct
 *
 */

void afe_port_data_logging_deinit(afe_dev_port_t *pDevPort);
/**
 * This function does the logging for afe ports
 *
 * @param[in] pDevPort Ptr to port struct
 * @param[in] mem_start_addr Ptr to the buffer to be logged.
 * @param[in] is_data_interleaved is the data in the mem_start_addr interleaved (TRUE -> interleaved)
 *
 */
void afe_port_data_logging(afe_dev_port_t *pDevPort, int8_t *mem_start_addr, bool_t is_data_interleaved);

/**
 * This function is for checking log code status whether it is 
 * enable or not. 
 *
 * @param[in] log_code log code to be checked 
 * @param[out] ADSP_EOK if the code is enabled otherwise 
 *  	 ADSP_EFAILED
 * 
 */
ADSPResult afe_log_status(uint16_t log_code);


/**
 * This function initializes logging for afe group devices
 *
 * @param[in] group id for the group device
 * @param[in] group device state Ptr to the group device state structure
 *
 */
void afe_group_data_logging_init(uint16 group_id, afe_group_device_state_struct_t *pGroupDeviceState);

/**
 * This function deinitializes logging for afe group devices
 *
 * @param[in] group id for the group device
 * @param[in] group device state Ptr to the group device state structure
 *
 */

void afe_group_data_logging_deinit( uint16 group_id, afe_group_device_state_struct_t *pGroupDeviceState);

/**
 * This function does the logging for afe group devices
 *
 * @param[in] group id for the group device
 * @param[in] group device state Ptr to the group device state structure
 *
 */
void afe_group_data_logging(uint16 group_id, afe_group_device_state_struct_t *pGroupDeviceState);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _AFE_LOGGING_UTILS_H_ */


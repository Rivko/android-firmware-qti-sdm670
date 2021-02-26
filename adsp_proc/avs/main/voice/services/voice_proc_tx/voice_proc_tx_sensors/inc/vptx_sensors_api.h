#ifndef VPTX_SENSORS_API_H
#define VPTX_SENSORS_API_H

/*============================================================================
  FILE:          vptx_sensors_api.h

  OVERVIEW:      This file has the function declarations for sensors interface for fpro v2 algorithm.

  DEPENDENCIES:  None

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/voice_proc_tx_sensors/inc/vptx_sensors_api.h#2 $

  when       who     what, where, why
  ---------- ------- ---------------------------------------------------------
  2014-07-14 rishabht   Initial revision.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/


#include "AEEStdDef.h"
#include "comdef.h"
#include "qurt_elite.h"



/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

typedef enum
{
   REG_SIG = 0,
   IND_SIG
} reg_ind_signal_t;


/*
 * The below function gets invoked in vptx init to allocate memory and  initialize :
 *   1. Registration signal pointer in vptx qmi structure(signal which will be set when call back is recieved
 *      from sensors after successful registration)
 *   2. Indication signal pointer in vptx qmi structure (signal which will be set when call back is recieved
 *      from sensors after holding pattern change)
pQmi_object  -> pointer to qmi structure
pChannel     -> pointer to qurt elite channel to add registration and indication signals to channel
qmi_reg_mask -> qmi registration signal mask
qmi_ind_mask -> qmi indication signal mask
session_num  -> vptx session number
*/
ADSPResult vptx_sns_qmi_init(void *pQmi_object, qurt_elite_channel_t  *pChannel, uint32_t qmi_reg_mask, uint32_t qmi_ind_mask, uint32_t session_num);


/*
 * The below function gets invoked in vptx end to free memory allocated for :
 *   1. Registration signal pointer in vptx qmi structure(signal set when call back is recieved
 *      from sensors after successful registration)
 *   2. Indication signal pointer in vptx qmi structure (signal set when call back is recieved
 *      from sensors after holding pattern change)
pQmi_object  -> pointer to qmi structure
*/
void vptx_sns_qmi_destroy(void *pQmi_object);


/*
 * The below function gets invoked in vptx run cmnd to place request to QMI for registration
 * this function will:
 *   1. Get the service object to communicate with sensors code
 *   2. Check if qmi service is up, and get the list of services
 *   3. qmi client initialization
 *   4. place asynchronous message to request for indications from Facing Service
pQmi_object  -> pointer to qmi structure
*/
ADSPResult vptx_sns_qmi_register(void *pQmi_object);

/*
 * The below function gets invoked in vptx stop cmnd to deregister from  QMI
 * this function will:
 *   1. set registration flag to FALSE
 *   2. release the client (deregister from qmi)
 *   3. free memory allocated for response and request queues
pQmi_object  -> pointer to qmi structure
*/
void vptx_sns_qmi_deregister(void *pQmi_object);

/*
 * The below function gets invoked in vptx init  to get the size of
 * qmi structure
*/
uint32_t vptx_sns_qmi_get_mem_req(void);

/*
 * The below function gets invoked whenever holding pattern
 * information is required by framework.
 * It will return the current holding pattern value.
pQmi_object  -> pointer to qmi structure
*/
int32_t  vptx_sns_get_holding_pattern(void *pQmi_object);


/*
 * The below function will clear the registration and indication signals
pQmi_object  -> pointer to qmi structure
sig_flag     -> flag to decide whether to clear registration signal or indication signal
*/
void vptx_sns_clear_qmi_signal(void *pQmi_object, reg_ind_signal_t sig_flag);

#endif /* #ifndef VPTX_SENSORS_API_H */

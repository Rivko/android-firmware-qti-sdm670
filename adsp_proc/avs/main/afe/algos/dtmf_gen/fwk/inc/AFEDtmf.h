/*========================================================================
 This file contains DTMF generator high level apis

   Copyright (c) 2009-2015,2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.

 $Header: //components/rel/avs.adsp/2.8.5/afe/algos/dtmf_gen/fwk/inc/AFEDtmf.h#1 $

 Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/26/2014   rv       Created
 ====================================================================== */
#ifndef _AFE_DTMF_H_
#define _AFE_DTMF_H_

/*==========================================================================
 File includes
 ========================================================================== */
#include "AFEInternal.h"

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

/*==========================================================================
 Function Defines
 ========================================================================== */

/*
 * Function to generate the DTMF samples and add them to the port's buffer (resultBuf).
 * NOTE: this function modifies scratchBuf
 * the DTMF samples are added to samples in resultBuf
 *
 * data_t specifies whether resultBuf is actually 16 bit or 32 bit.
 *
 * aud_mixer_client_ctr is the pointer to mixer counter.
 * Value at aud_mixer_client_ctr indicates whether DTMF is to be mixed or just placed in resultBuf.
 *
 */
ADSPResult afe_port_gen_and_add_DTMF(afe_dev_port_t *pDevPort, int16_t *resultBuf,
                                       int16_t *scratchBuf, uint16_t *aud_mixer_client_ctr);

/**
 * This function is to deinit DTMF for afe ports.
 *
 * @param[in] pDevPort,  pointer to port instance
 * @return void
 */
void afe_port_dtmf_deinit(afe_dev_port_t *pDevPort);

/**
 * This function is to set the DTMF param for afe ports.
 * It also initializes the DTMF on those ports.
 *
 * @param[in] pAudStatAfeSvc: AFE Static service handle
 * @param[in] pMsg: Elite message pointer
 * @return ADSPResult
 */
ADSPResult afe_svc_set_dtmf_config(elite_msg_any_t* pMsg);


/*This function checks DTMF state and informs DTMF presence
 * pDevPort                 : Device port pointer, used to access dtmf info ptr
 * @return is_dtmf_enable   : returns boolean value TRUE, if DTMF is enabled, else FALSE
 * */
bool_t afe_port_check_dtmf_state(afe_dev_port_t *pDevPort);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // _AFE_DTMF_H_

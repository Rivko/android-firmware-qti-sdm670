/*========================================================================
 This file contains limiter high level apis

 Copyright (c) 2015-2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
 QUALCOMM Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.

 $Header: //components/rel/avs.adsp/2.8.5/afe/algos/limiter/fwk/inc/AFELimiter.h#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 8/7/2015   rv       Created
 ====================================================================== */
#ifndef _AFE_LIMITER_H_
#define _AFE_LIMITER_H_

/*==========================================================================
 File includes
 ========================================================================== */
#include "AFEInternal.h"

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

/* AFE limiter delay in seconds: 1ms in Q15 */
#define AFE_LIM_DELAY_1MS_Q15 33
/* AFE Limiter Threshold for 16bit output in Q15 format*/
#define AFE_LIM_16BIT_THRESHOLD_Q15 0x7FB5
/* AFE Limiter Threshold for 32bit output in Q27 format*/
#define AFE_LIM_32BIT_THRESHOLD_Q27 0x7FB4A2C
/* AFE Limiter Make up gain in Q8 format*/
#define AFE_LIM_MAKEUP_GAIN_Q8 0x0100
/* AFE Limiter Processing mode. 1 is for NORMAL MODE*/
#define AFE_LIM_MODE 0x1
/* AFE Limiter recovery const in Q15*/
#define AFE_LIM_GC_Q15 0x7EB8
/*==========================================================================
 Function Defines
 ========================================================================== */

/* Initializes the limiter for the port.
 */
ADSPResult afe_port_init_limiter(afe_dev_port_t *pDevPort);

/* De-initializes the limiter for the port.
 */
ADSPResult afe_port_deinit_limiter(afe_dev_port_t *pDevPort);

/* To send bypass mode enable/disable to the limiter.
 */
ADSPResult afe_port_limiter_set_mode(afe_dev_port_t *pDevPort, bool_t bypass_mode);

/* Sets the enable/disable on limiter
 */
ADSPResult afe_port_set_param_limiter(afe_dev_port_t *pDevPort, uint32_t param_id,
        								int8_t *param_buffer_ptr,
										uint32_t param_size);

/* Process the samples */
void afe_port_limiter_process(afe_dev_port_t* pDevPort, int8_t* in_buf_ptr, int8_t* out_buf_ptr,
                                     uint32_t client_counter, uint32_t num_samples_per_ch);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // _AFE_LIMITER_H_

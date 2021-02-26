/**
@file MixerSvc_Limiter.h
@brief This file declares Limiter utility functions that the audio 
       matrix mixer uses.
       
 */       

/*========================================================================
  Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_Limiter.h#1 $

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 5/8/2017   sn       Created
 ====================================================================== */
#ifndef _MIXER_SVC_LIMITER_H_
#define _MIXER_SVC_LIMITER_H_


#include "qurt_elite.h"
#include "MixerSvc.h"

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

/* Matrix limiter delay in seconds: 2ms in Q15 */
#define MATRIX_LIM_DELAY_Q15 66

/* Matrix Limiter Threshold for 16bit output in Q15 format*/
#define MATRIX_LIM_16BIT_THRESHOLD_Q15 0x7FB0  // -0.02db

/* Matrix Limiter Hard Threshold for 16bit output in Q15 format*/
#define MATRIX_LIM_16BIT_HARD_THRESHOLD_Q15 0x8000  // 0db

/* Matrix Limiter Threshold for 24bit output in Q27 format*/
#define MATRIX_LIM_24BIT_THRESHOLD_Q27 0x7FB4A2B  //-24db

/* Matrix Limiter Hard Threshold for 24bit output in Q27 format*/
#define MATRIX_LIM_24BIT_HARD_THRESHOLD_Q27 0x8000000 //0 db

/* Matrix Limiter Make up gain in Q8 format*/
#define MATRIX_LIM_MAKEUP_GAIN_Q8 0x0100
/* Matrix Limiter Processing mode. 1 is for NORMAL MODE*/
#define MATRIX_LIM_MODE 0x1
/* Matrix Limiter recovery const in Q15*/
#define MATRIX_LIM_GC_Q15 0x7EB8
/* Matrix Limiter Gain attack time const in Q31*/
#define MATRIX_LIM_GAIN_ATTACK_CONSTANT_Q31 0xB362C80
/* Matrix Limiter Gain release time const in Q31*/
#define MATRIX_LIM_GAIN_RELEASE_CONSTANT_Q31 0x1F0D180
/* Matrix Limiter Gain attack time coefficient value in Q15*/
#define MATRIX_LIM_GAIN_ATTACK_COEFFICIENT_Q15 0x8000
/* Matrix Limiter Gain release time coefficient value in Q15*/
#define MATRIX_LIM_GAIN_RELEASE_COEFFICIENT_Q15 0x8000
/* Matrix Limiter history window length in seconds:10ms in Q15*/
#define MATRIX_LIM_HISTORY_WINLEN_10MS_Q15 0x147

typedef struct This_t This_t;

typedef struct mt_mx_limiter_callback_obj_t mt_mx_limiter_callback_obj_t;
struct mt_mx_limiter_callback_obj_t
{
  This_t *me;
  uint32_t unOutPortID;
};

typedef struct mt_mx_struct_limiter_t mt_mx_struct_limiter_t;
struct mt_mx_struct_limiter_t
{
    capi_v2_t   *pLimiterCapiV2;  /* pointer to the limiter capi_v2 instance for audio
                                    required for Rx output ports; Tx is NULL. */
    bool_t      bIsLimiterEnable; /* flag to hold limiter enable/disable. */
    uint32_t    unLimiterKpps;    /* Var used by the LimiterInstance to store req KPPS */
    uint32_t    unLimiterBw;      /* Var used by the LimiterInstance to store req BW in Bps */
    uint32_t    unLimiterDelay;   /* Var used by the LimiterInstance to store delay */
    mt_mx_limiter_callback_obj_t structLimiterCbObj; /*callback structure */
};


/*==========================================================================
 Function Defines
 ========================================================================== */

/* Initializes the limiter for the matrix output port.
 */
ADSPResult MtMx_OutPortLimiterInit(This_t *me, uint32_t unOutPortID);

/* De-initializes the limiter for the matrix output port.
 */
ADSPResult MtMx_OutPortLimiterDeinit(This_t *me, uint32_t unOutPortID);

/* To send bypass mode enable/disable to the limiter.
 */
ADSPResult MtMx_OutPortLimiterSetMode(This_t *me, uint32_t unOutPortID, bool_t bypass_mode);

/* To send mediafmt to the matrix output port limiter instance.
 */
ADSPResult MtMx_OutPortLimiterSetMediafmt(This_t *me, uint32_t unOutPortID);

ADSPResult MtMx_OutPortLimiterSetParams(This_t *me, uint32_t unOutPortID, uint32_t  unParamID, void* param_data_ptr, uint32_t param_size);

ADSPResult MtMx_OutPortLimiterGetParams(This_t *me, uint32_t unOutPortID, uint32_t  unParamID, int8_t* param_buffer_ptr, int32_t param_buf_len, uint32_t* params_buffer_len_req_ptr);

ADSPResult MtMx_OutputPortLimiterDelayUpdate(This_t *me, uint32_t unOutPortID);

capi_v2_err_t MtMx_LimiterCapiv2Callback(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_ptr);

capi_v2_event_callback_info_t MtMx_LimiterGetCapiv2CallbackHandler(This_t *me, uint32_t unOutPortID);

/* Process the samples */
void MtMx_OutPortLimiterProcess(This_t *me, uint32_t unOutPortID, int8_t* in_buf_ptr, int8_t* out_buf_ptr, uint32_t num_samples_per_ch);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // _MIXER_SVC_LIMITER_H_

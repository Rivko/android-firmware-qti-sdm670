/**
@file MixerSvc_Pspd.h
@brief This file defines matrix mixer PSPD utilities.
*/

/*===========================================================================
Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_Pspd.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/10/2014  kr      Re-factoring, De-coupling PSPD and ChannelMixer.
02/04/2014 rkc     Created file.
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "capi_v2_chmixer.h"
#include "MixerSvc.h"
#include "capi_v2_adsp_error_code_converter.h"

#ifndef __PSPD_SVC_H__
#define __PSPD_SVC_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//PSPD KPPS requirement. (voting for clocks assuming 192kHz. this need to be finetuned)
#define PSPD_KPPS_NO_MODULE_ENABLED          (2000)
#define PSPD_KPPS_CHMIXER_MODULE_ENABLED_2CH (2000)
#define PSPD_KPPS_CHMIXER_MODULE_ENABLED_6CH (12000)
#define PSPD_KPPS_CHMIXER_MODULE_ENABLED_8CH (14000)

//PSPD BW requirements.
#define PSPD_BW_NO_MODULE_ENABLED          (0)
#define PSPD_BW_CHMIXER_MODULE_ENABLED_2CH (48000*2*2)
#define PSPD_BW_CHMIXER_MODULE_ENABLED_6CH (1024*1024*2) //2 MBps
#define PSPD_BW_CHMIXER_MODULE_ENABLED_8CH (1024*1024*3) //3 MBps

//Maximum number of channels supported in PSPD threads.
#define PSPD_MAX_CHAN      8

//Param for ELITEMSG_CUSTOM_PSPD_CFG_CHMIXER_INOUT
typedef struct pspd_chmixer_inout_t pspd_chmixer_inout_t;
struct pspd_chmixer_inout_t
{
   uint32_t      num_in_ch;                 //Num input channels
   uint8_t       in_ch_map[PSPD_MAX_CHAN];  //Input channel mappings
   uint32_t      num_out_ch;                //Num output channels
   uint8_t       out_ch_map[PSPD_MAX_CHAN]; //Output channel mapping
   uint32_t      bit_width;                 //Bits per sample for input/output
   eChMixerType  eChannelMixerType;         //Type of channel mixer
};

//Operation code for creating and initing the input/output channel config of the channel mixer
#define ELITEMSG_CUSTOM_PSPD_CREATE_CHMIXER           (ELITE_PSPD_SVCID + 1)

//Operation code for destroying the channel mixer
#define ELITEMSG_CUSTOM_PSPD_DESTROY_CHMIXER        (ELITE_PSPD_SVCID + 2)

//Operation code for setting the input/output channel config of the channel mixer
#define ELITEMSG_CUSTOM_PSPD_CFG_CHMIXER_INOUT      (ELITE_PSPD_SVCID + 3)

//Payload structure for ELITEMSG_CUSTOM_PSPD_CREATE_CHMIXER and
//                                  ELITEMSG_CUSTOM_PSPD_CFG_CHMIXER_INOUT
typedef struct EliteMsg_CustomPspdCfgChmixInout EliteMsg_CustomPspdCfgChmixInout;
struct EliteMsg_CustomPspdCfgChmixInout
{
    qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t              unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t              unResponseResult; /**< Response result that the server
                                            sends back to the client. */

    uint32_t              unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    pspd_chmixer_inout_t  param;            /**< Specifies the input and output 
                                            channel maps. */
};

//Operation code for setting the channel mixer coefficients
#define ELITEMSG_CUSTOM_PSPD_SET_CHMIXER_COEF      (ELITE_PSPD_SVCID + 4)

//Payload structure for  ELITEMSG_CUSTOM_PSPD_SET_CHMIXER_COEF
typedef struct EliteMsg_CustomPspdSetChmixCoef EliteMsg_CustomPspdSetChmixCoef;
struct EliteMsg_CustomPspdSetChmixCoef
{
    qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t              unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t              unResponseResult; /**< Response result that the server
                                            sends back to the client. */

    uint32_t              unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    audproc_chmixer_param_id_coeff_t  *coef;              /**< Specifies coefficient set. */
};



//Operation code for reinit of PSPD thread buffers
#define ELITEMSG_CUSTOM_PSPD_REINIT_BUFFERS      (ELITE_PSPD_SVCID + 9)

//Payload structure for  ELITEMSG_CUSTOM_PSPD_REINIT_BUFFERS
typedef struct EliteMsg_CustomPspdReInitBuffers EliteMsg_CustomPspdReInitBuffers;
struct EliteMsg_CustomPspdReInitBuffers
{
    qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t              unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t              unResponseResult; /**< Response result that the server
                                            sends back to the client. */

    uint32_t              unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t              num_in_ch;        /**< Num input channels. */
    uint32_t              num_out_ch;       /**< Num output channels. */
    uint32_t              unPspdBufSize;    /**< Buffer size of intermediate buffer. */
};

//PSPD Utility functions
ADSPResult MtMx_CreatePspdSvc(elite_svc_handle_t *handle, uint32_t unInPortID, uint32_t unOutPortID, volatile uint32_t* punPspdKpps, volatile uint32_t* punPspdBw);

#ifdef __cplusplus
}
#endif // __cplusplus
       
#endif //__PSPD_SVC_H__

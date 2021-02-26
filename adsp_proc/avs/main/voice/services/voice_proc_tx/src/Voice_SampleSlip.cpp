/*========================================================================

*//** @file Voice_SampleSlip.cpp

Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file provides interfaces to create and control multi channel circular buffers for voice threads.
*//*====================================================================== */

/*============================================================================
                             Edit History
$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/src/Voice_SampleSlip.cpp#1 $

when        who   what, where, why
---------   ---   -------------------------------------------------------
08/30/10   ss      Created file for new Sample Slip algorithm wrapper

============================================================================*/

/*============================================================================
 *                       INCLUDE FILES FOR MODULE
 *==========================================================================*/


#include "Voice_SampleSlip.h"
#include "adsp_vcmn_api.h"
#include "VoiceCmnUtils.h"

extern "C" {
#include "ss_api.h"
}


/* =============================================================*/
/*                                                              */
/*         DEFINITIONS AND DECLARATIONS FOR MODULE              */
/*                                                              */
/* This section contains definitions for constants,macros,types,*/
/* variables and other items needed by this module.             */
/*==============================================================*/

/* -------------------------------------------------------------*/
/* Constant / Define Declarations                               */
/* -------------------------------------------------------------*/
#define VOICE_SS_NB_KPPS (140)   // kpps for ss lib for input at nb sampling rate
#define VOICE_SS_DELAY_IN_SAMPLES  (4) //4 sample delay
#define ROUNDTO8(x) ((((uint32_t)(x) + 7) >> 3) << 3);
/*--------------------------------------------------------------*/
/* Local Object Definitions                                     */
/* -------------------------------------------------------------*/
void voice_ss_get_size(uint32_t *nSize)
{
   uint32_t nCfgSize, nDataSize;
   (void)ss_get_size(SAMP_SLIP_INTERP_16, &nCfgSize, &nDataSize);
   nCfgSize = ROUNDTO8(nCfgSize);
   nDataSize = ROUNDTO8(nDataSize);
   *nSize = nCfgSize + nDataSize;
   return;
}
void voice_ss_set_mem(ss_struct_type_t  *pSsStruct, int8_t *pMemAddr, uint32_t nSize)
{
   uint32_t nCfgSize, nDataSize;
   (void)ss_get_size(SAMP_SLIP_INTERP_16, &nCfgSize, &nDataSize);
   nCfgSize = ROUNDTO8(nCfgSize);
   nDataSize = ROUNDTO8(nDataSize);
   if((nCfgSize+nDataSize)<=nSize)
   {
      pSsStruct->ss_cfg_ptr = (void*)pMemAddr;
      pSsStruct->ss_data_ptr = (void*)(pMemAddr + nCfgSize);
   }
   else
   {
      pSsStruct->ss_cfg_ptr = NULL;
      pSsStruct->ss_data_ptr = NULL;
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Not enough memory for processing. reqd(%ld), got(%ld)\n",(nCfgSize+nDataSize),nSize );
   }
   return;
}

ADSPResult voice_ss_config(ss_struct_type_t  *ss_struct_ptr, uint32_t sampling_rate)
{
   ADSPResult result = ADSP_EOK;
   if (NULL != ss_struct_ptr)
   {
      ss_struct_ptr->sampling_rate = sampling_rate;
      ss_struct_ptr->nb_sampling_rate_factor = sampling_rate/VOICE_NB_SAMPLING_RATE;
   }
   else
   {
      result = ADSP_EBADPARAM;
   }
   return result;
}

ADSPResult voice_ss_get_kpps(ss_struct_type_t  *ss_struct_ptr,
				 uint32_t *kpps_ptr
             )
{
   ADSPResult result = ADSP_EOK;

   if ((NULL != ss_struct_ptr) && (NULL!= kpps_ptr))
   {  
      ss_struct_ptr->kpps = 0;
      ss_struct_ptr->kpps += (VOICE_SS_NB_KPPS * ss_struct_ptr->nb_sampling_rate_factor); // always enabled
      *kpps_ptr = ss_struct_ptr->kpps; 
   }
   else
   {
      result = ADSP_EBADPARAM;
   }
   return result;
}

ADSPResult voice_ss_get_delay(ss_struct_type_t  *ss_struct_ptr,
				 uint32_t *delay_ptr
             )
{
   ADSPResult result = ADSP_EOK;
   uint32_t sampling_rate_in_khz;

   if ((NULL != ss_struct_ptr) && (NULL!= delay_ptr))
   {
      sampling_rate_in_khz = 8 * ss_struct_ptr->nb_sampling_rate_factor;
      *delay_ptr = voice_cmn_int_div(1000 * VOICE_SS_DELAY_IN_SAMPLES, sampling_rate_in_khz);
   }
   else
   {
      result = ADSP_EBADPARAM;
   }
   return result;
}

ADSPResult voice_sample_slip_init(ss_struct_type_t  *pSsStruct, int16_t nSsFrameSize, uint8_t multi_frame)
{
   // Initialize the sample slipping configuration structure
   ss_cfg_init( (int8*)pSsStruct->ss_cfg_ptr, nSsFrameSize, multi_frame , SAMP_SLIP_INTERP_16);

   // Initialize the sample slipping data structure
   ss_init( (const int8*)pSsStruct->ss_cfg_ptr, (int8*)pSsStruct->ss_data_ptr );

   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: Sample Slipping init end reached");
   return ADSP_EOK;
}

ADSPResult voice_sample_slip_process (ss_struct_type_t  *pSsStruct,int16_t *pOut,int16_t *pIn, int16_t nInSize,int16_t nOutSize)
{
   if( (NULL == pSsStruct->ss_cfg_ptr) || (NULL == pSsStruct->ss_data_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: voice_sample_slip_process()::Memory NULL pointers \n");
   }
   else
   {
#if defined(VOICE_DBG_MSG)
      MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VOICE: sampleslip processing, nInSize(%#x), nOutSize(%#x), instance(%#x)",nInSize, nOutSize,pSsStruct);
#endif
      //dbg: MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: SampleSlipping Process Start");
      samp_slip_process((const int8*)pSsStruct->ss_cfg_ptr,
                        (int8*)pSsStruct->ss_data_ptr,
                        (int8*)pIn,
                        (int8*)pOut,
                        nInSize,
                        nOutSize);
      //dbg: MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: SampleSlipping Process End :: InSize(%d),OutSize(%d)\n",nInSize,nOutSize);
   }
   return ADSP_EOK;
}
void voice_ss_end(ss_struct_type_t  *pSsStruct)
{
   pSsStruct->ss_cfg_ptr = NULL;
   pSsStruct->ss_data_ptr = NULL;
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: SampleSlipping End Reached");
   return;
}

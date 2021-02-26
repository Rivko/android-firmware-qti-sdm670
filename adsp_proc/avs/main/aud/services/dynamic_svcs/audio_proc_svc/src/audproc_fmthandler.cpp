
/*==========================================================================
ELite Source File

This file implements the functions for converting a buffer into a different
media format.

Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/25/11   DG      Removed support for channel conversion, as this
                   is part of the topology now.
09/20/10   DG      Added support for the case of 5.1 input and output.
07/08/10   DG      Created file
=========================================================================== */

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "audproc_comdef.h"
#include "Interleaver.h"


/*---------------------------------------------------------------------------
* Definitions and Constants
* -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
* Globals
* -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
* Function Declarations
* -------------------------------------------------------------------------*/
static void AudPP_DeinterleaveData(int8_t* pInPcm, int8_t *pTmp, const uint32_t unInSamples, 
                                   const uint32_t numChannels, const uint32_t bytes_per_sample);

static ADSPResult AudPP_SetTmpBuf(AudPP_FormatConvType *pConv, const uint32_t bufferSizeInBytes);

/*---------------------------------------------------------------------------
* Function Definitions
* -------------------------------------------------------------------------*/

ADSPResult AudPP_InitFormatConv(AudPP_FormatConvType *pConv, const uint16_t instanceId)
{
   pConv->instanceId = instanceId;
   pConv->deinterleave = FALSE;
   pConv->pTmpBuffer = NULL;
   pConv->tmpBufSize = 0;

   return ADSP_EOK;
}

ADSPResult AudPP_SetupFormatConv(AudPP_FormatConvType *pConv, uint32_t inputChannels, uint32_t outputChannels, bool_t deinterleave, uint32_t samplingRate)
{
   pConv->deinterleave = deinterleave;
   pConv->numInputChannels = inputChannels;
   pConv->numOutputChannels = outputChannels;

   return ADSP_EOK;
}

void AudPP_DestroyFormatConv(AudPP_FormatConvType *pConv)
{
   if (NULL != pConv->pTmpBuffer)
   {
      qurt_elite_memory_free(pConv->pTmpBuffer);
      pConv->pTmpBuffer = NULL;
      pConv->tmpBufSize = 0;
   }
}

ADSPResult AudPP_ProcessFormatConv(AudPP_FormatConvType *pConv, int8_t *pBuffer, 
                                   uint32_t samplesPerChannel, uint32_t bytes_per_sample)
{
   ADSPResult result = ADSP_EOK;

   uint32_t num_channels = pConv->numInputChannels;

   if (samplesPerChannel == 0)
   {
      return ADSP_EOK;
   }

   if (pConv->deinterleave)
   {
      if (pConv->numInputChannels > 1)
      {
         result = AudPP_SetTmpBuf(pConv, samplesPerChannel * num_channels * bytes_per_sample);

         if (ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: Out of Memory for creating tmp buffer for deinterleaving!", pConv->instanceId);
         }
         else
         {
            AudPP_DeinterleaveData(pBuffer, pConv->pTmpBuffer, samplesPerChannel, num_channels, bytes_per_sample);
         }
      }
   }

   return result;
}

ADSPResult AudPP_ResetFormatConv(AudPP_FormatConvType *pConv)
{
   return ADSP_EOK;
}

/**
 * This function take the interleaved pcm data, and output deinterleaved
 * data in-place.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] pInPcm The buffer to convert to deinterleaved
 *       format.
 * @param[in] pTmp Scratch buffer. The size must be at least
 *       equal to the input.
 * @param[in] unInSamples Number of samples in the input buffer
 *       per channel.
 * @param[in] numChannels Number of channels in the input buffer
 *
 */
void AudPP_DeinterleaveData(int8_t* pInPcm, int8_t *pTmp, const uint32_t unInSamples,
                            const uint32_t numChannels, const uint32_t bytes_per_sample)
{
   ADSPResult result;
   int8_t *pOutBufs[MAX_INPUT_CHANNELS] = { NULL };

   int8_t *pOutChannel = pTmp;

   uint32_t num_input_bytes_per_ch = (unInSamples * bytes_per_sample);

   for (uint32_t i = 0; i < numChannels; i++)
   {
      pOutBufs[i] = pOutChannel;
      pOutChannel += num_input_bytes_per_ch;
   }

   if(BYTES_PER_SAMPLE_TWO == bytes_per_sample)
   {
      result = DeInterleave_Multich_16((int16_t *)pInPcm, (int16_t **)pOutBufs, numChannels, unInSamples);
   }
   else
   {
      result = DeInterleave_Multich_32((int32_t *)pInPcm, (int32_t **)pOutBufs, numChannels, unInSamples);
   }

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PP failed to deinterleave data!");
   }
   else
   {
      memscpy(pInPcm,unInSamples * numChannels * bytes_per_sample, pTmp, unInSamples * numChannels * bytes_per_sample);
   }
}

static ADSPResult AudPP_SetTmpBuf(AudPP_FormatConvType *pConv, const uint32_t bufferSizeInBytes)
{
   if (bufferSizeInBytes <= pConv->tmpBufSize)
   {
      return ADSP_EOK;
   }

   if (NULL != pConv->pTmpBuffer)
   {
      qurt_elite_memory_free(pConv->pTmpBuffer);
      pConv->pTmpBuffer = NULL;
      pConv->tmpBufSize = 0;
   }

   pConv->pTmpBuffer = (int8_t *)qurt_elite_memory_malloc(bufferSizeInBytes, PP_SVC_TMP_BUF);

   if (NULL == pConv->pTmpBuffer)
   {
      return ADSP_ENOMEMORY;
   }
   else
   {
      pConv->tmpBufSize = bufferSizeInBytes;
   }

#ifdef DBG_BUFFER_ADDRESSES
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR PP tmp buffer: 0x%8x, size %d",
         pConv->pTmpBuffer, bufferSizeInBytes);
#endif /* DBG_BUFFER_ADDRESSES */

   return ADSP_EOK;
}


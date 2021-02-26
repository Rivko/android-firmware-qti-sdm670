/**
@file AudStreamMgr_Utils.cpp

@brief This file contains the implementation of utility functions for AudioStreamMgr.

 */

/*========================================================================

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/23/16   HB		Create File.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/
#include "AudioStreamMgr_Util.h"
#include "EliteMsg_AdmCustom.h"

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

ADSPResult AudioStreamMgr_GetProcInitParamFromNonPcmEncConfig(
      PPOutputParams_t *pParam,
      uint32_t unFmtBlkId,
      void     *pFrmtBlk
)
{
   switch ( unFmtBlkId)
   {
   default:
   {
      asm_custom_enc_cfg_t *pEncConfig = (asm_custom_enc_cfg_t *)pFrmtBlk ;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = pEncConfig ->num_channels;
      memscpy(pParam->channelMap, sizeof(pParam->channelMap), pEncConfig->channel_mapping, 8*sizeof(uint8_t));
      return ADSP_EOK;
   }
   }
   return ADSP_EOK;
}


ADSPResult AudioStreamMgr_GetMtxConfigParamFromNonPcmEncConfig(
                                                void     *pParam,
                                                uint32_t unFmtBlkId,
                                                void     *pFrmtBlk
                                                )
{
	EliteMsg_CustomAdmStreamMtMxReConfig *config_ports_msg_ptr = (EliteMsg_CustomAdmStreamMtMxReConfig *)pParam;

    switch(unFmtBlkId)
    {
    default:
    {
       asm_custom_enc_cfg_t *pEncConfig = (asm_custom_enc_cfg_t *)pFrmtBlk ;
       config_ports_msg_ptr->num_channels = pEncConfig ->num_channels;
       memscpy(config_ports_msg_ptr->channel_mapping, sizeof(config_ports_msg_ptr->channel_mapping), pEncConfig->channel_mapping, 8*sizeof(uint8_t));
       break;
    }
    }

	return ADSP_EOK;
}

void AudioStreamMgr_GetASMNodeForEncDecParam(uint32_t paramId, ASM_GraphNodeIndex *pNodeIndex)
{
	switch (paramId)
	{
    default:
	{
		*pNodeIndex = ASM_NODE_MAX;
		break;
	}
	}

}

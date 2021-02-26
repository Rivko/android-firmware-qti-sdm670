/*========================================================================

 *//** @file AudioEncSvc_Public_Util.cpp
This file contains Elite Audio encoder service.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/28/16   HB		Create file.
========================================================================== */

#include "AudioEncSvc_Includes.h"
#include "AudioEncSvc_CapiV2Util.h"


bool_t AudioEncSvc_IsProcessB4Eos(AudioEncSvc_t* pMe)
{
	//return true if process should be called before EOS
	return false;
}

/*
 *Function Name:AudioEncSvc_UpdateEncConfig
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *            asm_enc_cfg_blk_param_v2_t *pEncCfgParam: Encoder config params
 *
 *Description: Updates encoder CAPI with encoder config sent by client

 * Encoder config is sent for encoders only (main capi). not for packetizers or depacketizers.
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_UpdateNonPcmEncCfg(AudioEncSvc_t *pMe, void* pConfig)
{
	return ADSP_EUNSUPPORTED;
}

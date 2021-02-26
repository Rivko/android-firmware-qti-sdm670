/*========================================================================
ELite

This file contain declares the tester for PP.

Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_proc_svc/tst/audproctst_svc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
4/12/10    AP      Created file.

========================================================================== */
#ifndef ELITEPPSVCTST_H
#define ELITEPPSVCTST_H


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
   /**
    * This function tests the PP service.
    */
   int AudioPPTstLauncher();
   int ELitePpSvcPlaybackTest(void*);
   int ELitePpSvcPlaybackTest32BitPcm(void*);
   int ELitePpSvcFlushTest(void*);
   int ELitePpSvcStereoToMonoTest(void* dummy);
   int ELitePpSvcAbsentMediaFormatTest(void* dummy);
   int ELitePpSvcPoPPKeepSameSamplingRateTest(void* dummy);
   int ELitePpHPFSetGetParam(void *dummy);
   int ELitePpSPASetGetParam(void *dummy);
   int ELitePpTxIIRSetGetParam(void *dummy);
   int ELitePpIIRSetGetParam(void *dummy);
   int ELitePpEansSetGetParam(void *dummy);
   int ELitePpVolumeSetGetParam(void *dummy);
   int ELitePpRxGainSetGetParam(void *dummy);
   int ELitePpTxGainSetGetParam(void *dummy);
   int ELiteCOPPTest(void* dummy);
   int ELitePOPrePPauseTest(void *dummy);
   int ELiteDownMix51to2Test(void* dummy);
   int ELitePpFatalErrorTest(void* dummy);
   int ELitePpRandomInputSizesTest(void* dummy);
   int ELitePpMultipleMediaTypeMsgTest(void* dummy);
   int ELitePpResamlerTest(void* dummy);
   int ELitePpSetGettsmParam(void* dummy);
   int ELitePpZeroOutputTest(void* dummy);
   int ELiteBypass51TestPOPP(void *dummy);
   int ELiteBypass51TestCOPP(void *dummy);
   int ELiteBypass51TestCOPreP(void *dummy);
   int ELiteBypass51TestPOPreP(void *dummy);
   int ELiteBypass51TestDefaultTopo(void *dummy);
   int ELiteBypass51FlushTest(void *dummy);
   int ELiteBypass71Test(void *dummy);
   int ELiteBypass71FlushTest(void *dummy);
   int ELiteBypassResampler3ChTest(void *dummy);
   int ELiteBypassResampler6ChTest(void *dummy);
   int ELiteBypassResamplerFlushTest(void *dummy);
   int ELiteBypassChangeMediaTest(void *dummy);
   int ELitePOPrePTest(void* dummy);
   int ELitePpPbeSetGetParam(void* dummy);
   int ELiteVerifyTopologiesTest(void* dummy);
   int ELiteInitMallocFailTest(void* dummy);
   int ELiteKCPSandBWChangeTest(void* dummy);
   int ELiteSoftPauseIntervalTest(void* dummy);
   int ELiteSoftPauseBeforeMediaFmtTest(void* dummy);
   int ELiteMultiplePauseRunTest(void* dummy);
   int ELiteMultichannelInputOutputChannelSettingTest(void* dummy);
   int ELiteMaxDataMessagesTest(void* dummy);
   int ELiteMultichannelTopoTest(void* dummy);
   int ELiteTopoBufferingTest(void* dummy);
   int ELiteDynamicOutputMediaChangeTest(void* dummy);
   int ELiteDownmixAutoDisableTest(void* dummy);
   int ELiteAlgorithmicDelayTest(void* dummy);
   int ELiteEOSBufferTest(void* dummy);
   int ELiteCOPPEOSResetTest(void* dummy);
   int ELiteDataSetParamMsgTest(void* dummy);
   int ELiteTopoSetParamFailuresTest(void* dummy);
   int ELiteCustomTopologiesTest(void* dummy);
   int ELiteCustomTopologyInitMallocFailTest(void* dummy);
   int ELiteDynamicLoadingTest(void* dummy);
   int EliteSetBitstreamFormatTest(void* dummy);
   int EliteCompressedMediaFmtTest(void* dummy);
   int EliteTopoInterleavedTest(void* dummy);
   int EliteAsyncEventsTest(void* dummy);
   int EliteBufferingDelayTest(void* dummy);
   int EliteNumOutputBuffersTest(void* dummy);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //ELITEPPSVCTST_H

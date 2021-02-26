/*-------------------------------------------------------------------
Copyright (c) 2013-2017 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential

Copyright (c) 2010 The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of The Linux Foundation nor
      the names of its contributors may be used to endorse or promote
      products derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------*/

#ifndef _VTEST_DECODER_H
#define _VTEST_DECODER_H

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_QCOMExtns.h"
#include "vtest_ComDef.h"
#include "vtest_SignalQueue.h"
#include "vtest_Thread.h"
#include "vtest_ISource.h"
#include "vtest_BufferManager.h"
#include "VideoCRCChecker.h"
#include "media/hardware/HardwareAPI.h"

namespace vtest {

/**
* @brief Delivers YUV data from the MDP for testing WFD.
*
*/
class Decoder : virtual public ISource {

public:
    Decoder(CodecConfigType *pConfig);
    ~Decoder();

    virtual PortBufferCapability GetBufferRequirements(OMX_U32 ePortIndex);
    virtual OMX_ERRORTYPE Start();
    virtual OMX_ERRORTYPE Stop();
    virtual OMX_ERRORTYPE Configure(CodecConfigType *pConfig,
            BufferManager *pBufManager, ISource *pSource,ISource *pSink);
    virtual OMX_ERRORTYPE SetBuffer(BufferInfo *pBufferInfo, ISource *pSource,
        OMX_U32 framenum = 0);
    virtual OMX_ERRORTYPE AllocateBuffers(BufferInfo **pBuffers,
        OMX_BUFFERHEADERTYPE **pExtraDataBuffers, OMX_U32 nWidth,
        OMX_U32 nHeight, OMX_U32 nBufferCount, OMX_U32 nBufferSize,
        OMX_U32 nExtraDataBufferCount, OMX_U32 nExtraDataBufferSize,
        OMX_U32 ePortIndex, OMX_U32 nBufferUsage = 0);
    virtual OMX_ERRORTYPE UseBuffers(BufferInfo **pBuffers,
            OMX_BUFFERHEADERTYPE **ppExtraDataBuffers, OMX_U32 nWidth,
            OMX_U32 nHeight, OMX_U32 nBufferCount, OMX_U32 nBufferSize,
            OMX_U32 nExtraDataBufferCount, OMX_U32 nExtraDataBufferSize,
            OMX_U32 ePortIndex);

private:
    Decoder();
    virtual OMX_ERRORTYPE ThreadRun(OMX_PTR pThreadData);
    virtual OMX_ERRORTYPE FreeBuffer(
            BufferInfo *pBuffer, OMX_U32 ePortIndex);
    virtual OMX_ERRORTYPE FreeExtraDataBuffer(
        OMX_BUFFERHEADERTYPE **pExtraDataBuffer, OMX_U32 ePortIndex);

    /**
     * @brief Set the encoder state
     *
     * This method can be asynchronous or synchronous. If asynchonous,
     * WaitState can be called to wait for the corresponding state
     * transition to complete.
     *
     * @param eState The state to enter
     * @param bSynchronous If OMX_TRUE, synchronously wait for
     *                     the state transition to complete
     */
    OMX_ERRORTYPE SetState(OMX_STATETYPE eState, OMX_BOOL bSynchronous);

    /**
     * @brief Wait for the corresponding state transition to complete
     *
     * @param eState The state to wait for
     */
    OMX_ERRORTYPE WaitState(OMX_STATETYPE eState);

    OMX_ERRORTYPE HandleOutputPortSettingsChange(OMX_U32 nData2);
    /**
     * @brief API to validate if the port setting event is appropriate or not.
     *
     * @param eType which is enum of ReconfigType  If CropSettingChange, validation of crop setting change event,
                     If PortDefinitionChange, validation of port definition change event.
     */
    OMX_ERRORTYPE ValidatePortSettingChange(ReconfigType eType);
    OMX_ERRORTYPE PortReconfigOutput();

    OMX_ERRORTYPE DumpCodecInfo();

    OMX_ERRORTYPE SetNativeWindowEnable();
    OMX_ERRORTYPE GetGraphicBufferUsage(OMX_U32 *nBufferUsage);
    OMX_ERRORTYPE ParseExtraData(BufferPool * pBufferPool,
            OMX_BUFFERHEADERTYPE *pHeader, int idx);
    OMX_ERRORTYPE ProcessColorAspects();
    OMX_ERRORTYPE ProcessColorAspectsinReconfig();

    static OMX_ERRORTYPE EventCallback(
            OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData,
            OMX_IN OMX_EVENTTYPE eEvent, OMX_IN OMX_U32 nData1,
            OMX_IN OMX_U32 nData2, OMX_IN OMX_PTR pEventData);
    static OMX_ERRORTYPE EmptyDoneCallback(OMX_IN OMX_HANDLETYPE hComponent,
            OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE *pBuffer);
    static OMX_ERRORTYPE FillDoneCallback(OMX_IN OMX_HANDLETYPE hComponent,
            OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE *pBuffer);

    OMX_ERRORTYPE GetComponentRole(OMX_BOOL bSecureSession,
            FileType *eFileType, OMX_STRING *role);
    OMX_ERRORTYPE Flush(OMX_U32 nPortIndex);
    OMX_BOOL CheckColorFormatSupported(OMX_COLOR_FORMATTYPE nColorFormat,
            OMX_VIDEO_PARAM_PORTFORMATTYPE *pVideoPortFmt);
    OMX_ERRORTYPE SetPictureTypeDecode(OMX_U32 nPictureTypeDecode);
    OMX_ERRORTYPE SetVUIColorAspectsExtraData(android::ColorAspects sColorAspects);
    void convert_color_space_info(OMX_U32 primaries, OMX_U32 range,
        OMX_U32 transfer, OMX_U32 matrix, android::ColorAspects *aspects);

private:
    OMX_BOOL m_bInputEOSReached;
    OMX_BOOL m_bSecureSession;
    OMX_BOOL m_bPortReconfig;
    OMX_BOOL m_bIsSeek;
    OMX_BOOL m_bThumbnailMode;
    OMX_BOOL m_nExtraDataOutputBufferSettoOMX;
    SignalQueue *m_pSignalQueue;
    SignalQueue *m_pFreeBufferQueue;
    OMX_U32 m_nFrames;
    OMX_U32 m_nFramerate;
    OMX_U32 m_nFrameWidth;
    OMX_U32 m_nFrameHeight;
    OMX_U32 m_nInputBufferCount;
    OMX_U32 m_nInputBufferSize;
    OMX_U32 m_nOutputBufferCount;
    OMX_U32 m_nOutputBufferSize;
    OMX_U32 m_nExtraDataOutputBufferCount;
    OMX_U32 m_nExtraDataOutputBufferSize;
    OMX_U8 * pExtraDatapBuffer;
    OMX_U32 m_nOriginalOutputBufferCount;
    OMX_U32 m_nOutputBufferUsage;
    OMX_U8 *m_pMetaBuffer;
    OMX_HANDLETYPE m_hDecoder;
    OMX_STATETYPE m_eState;
    OMX_STATETYPE m_eStatePending;
    OMX_VIDEO_CODINGTYPE m_eCodec;
    OMX_PORT_PARAM_TYPE m_sPortParam;
    OMX_PARAM_PORTDEFINITIONTYPE m_sPortFmt;
    PlaybackModeType m_ePlaybackMode;
    OMX_U32 m_nXMLColorFormat;
    OMX_U32 m_nConfiguredColorFormat;
    OMX_U32 m_nETBcount;
    OMX_U32 m_nFrameNumber;
    OMX_U32 m_nFrameDoneCount; /* FBD count */
    QOMX_VIDEO_PICTURE_ORDER m_eDecoderPictureOrder;
    VideoCRCChecker* m_pCRCChecker;
    VideoResolution m_sOutputRes;
    OMX_S32 m_nDynamicIndexCount;
    DynamicConfigType m_sDynamicConfig;
    struct DynamicConfig *pDecDynamicProperties;
    OMX_U32 m_nDynamicConfigArraySize;
    OMX_U32 m_nColorPrimaries;
    android::DescribeColorAspectsParams m_sVUIColorAspects;
    android::DescribeColorAspectsParams m_sPreviousRefColorAspects;
    OMX_U32 m_nPrevCropWidth;
    OMX_U32 m_nPrevCropHeight;
    LIST_NODE m_pCropSettingChangeValidationList;
    LIST_NODE m_pPortDefinitionChangeValidationList;

    FILE *m_fMISRFile;
    /* ExtraDataConfig */
    FILE *m_fInterlace;
    FILE *m_fInterlaceRef;
    FILE *m_fAspectRatio;
    FILE *m_fAspectRatioRef;
    FILE *m_fFrameRate;
    FILE *m_fFrameRateRef;
    FILE *m_fConcealMB;
    FILE *m_fConcealMBRef;
    FILE *m_fRecoveryPointSEI;
    FILE *m_fRecoveryPointSEIRef;
    FILE *m_fPanScan;
    FILE *m_fPanScanRef;
    FILE *m_fTimeStamp;
    FILE *m_fTimeStampRef;
    FILE *m_fS3D;
    FILE *m_fS3DRef;
    FILE *m_fUserData;
    FILE *m_fUserDataRef;
    FILE *m_fVUIColorAspects;
    FILE *m_fVUIColorAspectsRef;
    FILE *m_fFrameQP;
    FILE *m_fFrameQPRef;
};

} // namespace vtest

#endif // #ifndef _VTEST_DECODER_H

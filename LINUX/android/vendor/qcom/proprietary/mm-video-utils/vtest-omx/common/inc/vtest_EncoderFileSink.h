/*-------------------------------------------------------------------
Copyright (c) 2013-2017 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
--------------------------------------------------------------------*/

#ifndef _VTEST_ENCODER_FILE_SINK_H
#define _VTEST_ENCODER_FILE_SINK_H

#include "OMX_Core.h"
#include "vtest_Debug.h"
#include "vtest_ComDef.h"
#include "vtest_ISource.h"
#include "vtest_Thread.h"
#include "vtest_Queue.h"
#include "vtest_BufferManager.h"
#include "vtest_Crypto.h"

namespace vtest {
/**
 * @brief Writes encoded bitstream to file.
 *
 * Frames are written to file asynchronously as they are delivered.
 */
class EncoderFileSink : virtual public ISource {

public:
    EncoderFileSink(Crypto *pCrypto);
    ~EncoderFileSink();

    virtual PortBufferCapability GetBufferRequirements(OMX_U32 ePortIndex);
    virtual OMX_ERRORTYPE Configure(CodecConfigType *pConfig,
            BufferManager *pBufManager, ISource *pSource, ISource *pSink);
    virtual OMX_ERRORTYPE SetBuffer(BufferInfo *pBuffers, ISource *pSource,
        OMX_U32 framenum = 0);

private:
    EncoderFileSink();
    virtual OMX_ERRORTYPE ThreadRun(OMX_PTR pThreadData);
    OMX_ERRORTYPE writeFrame(OMX_BUFFERHEADERTYPE *pBuffer);
    OMX_BUFFERHEADERTYPE* makeFrameHeaderIvf(OMX_BUFFERHEADERTYPE *pBuffer);
    OMX_BUFFERHEADERTYPE* makeHeaderIvf(int nFrameCount, int nWidth, int nHeight, int nScale, int nRate);
    void add16Bits(OMX_U8 aArray[], OMX_U32 nIndex, OMX_U32 nValue);
    void add32Bits(OMX_U8 aArray[], OMX_U32 nIndex, OMX_U32 nValue);
    void add64Bits(OMX_U8 aArray[], OMX_U32 nIndex, OMX_U64 nValue);
    OMX_ERRORTYPE handleExtradata(OMX_BUFFERHEADERTYPE *pBuffer);

private:
    OMX_U32 m_nFrames;
    File *m_pFile;
    OMX_BOOL m_bIsProfileMode;
    OMX_BOOL m_bIsVTPath;
    OMX_BOOL m_bIsVPX;
    OMX_U32 m_nWidth;
    OMX_U32 m_nHeight;
    OMX_U32 m_nScale;
    OMX_U32 m_nRate;
    OMX_BOOL m_bSecureSession;
    OMX_U8 *m_pOutCopyBuf;
    OMX_U32 m_nOutCopyBufSize;
    Crypto *m_pCrypto;

};

} // namespace vtest

#endif // #ifndef _VTEST_ENCODER_FILE_SINK_H

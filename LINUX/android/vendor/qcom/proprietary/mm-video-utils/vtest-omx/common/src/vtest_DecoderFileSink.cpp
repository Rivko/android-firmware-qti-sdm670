/*-------------------------------------------------------------------
Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------*/

#include "venc/inc/omx_video_common.h"
#include "vtest_ComDef.h"
#include "vtest_Debug.h"
#include "vtest_Thread.h"
#include "vtest_SignalQueue.h"
#include "vtest_Sleeper.h"
#include "vtest_File.h"
#include "vtest_Time.h"
#include "vtest_Parser.h"
#include "vtest_DecoderFileSink.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <media/msm_media_info.h>
#include "VideoCRCChecker.h"

#undef LOG_TAG
#define LOG_TAG "VTEST_DECODER_FILE_SINK"

namespace vtest {

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DecoderFileSink::DecoderFileSink(Crypto *pCrypto)
    : ISource(),
      m_nFrames(0),
      m_nFramerate(0),
      m_pFile(NULL),
      m_nFileFd(-1),
      m_bSecureSession(OMX_FALSE),
      m_pCrypto(pCrypto),
      m_pCRCChecker(NULL),
      m_sOutputRes{} {

    snprintf(m_pName, MAX_NAME_LENGTH, "%s", "DecoderFileSink");
    VTEST_MSG_LOW("%s: created...", Name());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DecoderFileSink::~DecoderFileSink() {

    if (m_pFile != NULL) {
        m_pFile->Close();
        delete m_pFile;
        m_pFile = NULL;
    }
    if (m_nFileFd != -1) {
        close(m_nFileFd);
        m_nFileFd = -1;
    }
    if(m_pCRCChecker)
    {
        /* TODO: check pending CRC count and update error code */
        m_pCRCChecker->Destroy(m_pCRCChecker);
        free(m_pCRCChecker);
    }

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
PortBufferCapability DecoderFileSink::GetBufferRequirements(OMX_U32 ePortIndex) {

    PortBufferCapability sBufCap;

    Mutex::Autolock autoLock(m_pLock);
    memset(&sBufCap, 0, sizeof(PortBufferCapability));

    if (ePortIndex == PORT_INDEX_IN) {
        sBufCap.bAllocateBuffer = OMX_FALSE;
        sBufCap.bUseBuffer = OMX_TRUE;
        sBufCap.pSource = this;
        sBufCap.ePortIndex = ePortIndex;
        sBufCap.nMinBufferSize = 0x1000;
        sBufCap.nMinBufferCount = 1;
        sBufCap.nExtraDataBufferSize = 0;
        sBufCap.nExtraDataBufferCount = 0;
        sBufCap.nBufferUsage = 0;
        sBufCap.nExtraBufferCount = 0;
    } else {
        VTEST_MSG_ERROR("Error: invalid port selection");
    }
    return sBufCap;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE DecoderFileSink::Configure(CodecConfigType *pConfig,
        BufferManager *pBufManager, ISource *pSource, ISource *pSink) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    VidcStatus ret = VidcStatusSuccess;


    result = ISource::Configure(pConfig, pBufManager, pSource, pSink);
    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("DecoderFileSink configure failed");
        return result;
    }

    Mutex::Autolock autoLock(m_pLock);

    char sFileName[VTEST_MAX_STRING];
    OMX_S32 nTestNum = pConfig->nCurrentTestNum;
    m_bSecureSession = pConfig->bSecureSession;
    m_sOutputRes.nColorFmt = pConfig->nOutputColorFormat;

    SNPRINTF(sFileName,VTEST_MAX_STRING,"%s%s%s",sGlobalStaticVideoProp.sOutRoot,pConfig->cOutFileName,".yuv");

    if (pConfig->nFramerate > 0 &&
        pConfig->nFrameWidth > 0 &&
        pConfig->nFrameHeight > 0) {

        m_nFramerate = pConfig->nFramerate;
        m_sOutputRes.nWidth = pConfig->nFrameWidth;
        m_sOutputRes.nHeight = pConfig->nFrameHeight;
        m_sOutputRes.nStartX = 0;
        m_sOutputRes.nStartY = 0;
        m_sOutputRes.nCropWidth =  pConfig->nFrameWidth;
        m_sOutputRes.nCropHeight = pConfig->nFrameHeight;
        if (QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m == m_sOutputRes.nColorFmt )
        {
            m_sOutputRes.nStride = VENUS_Y_STRIDE(COLOR_FMT_NV12, pConfig->nFrameWidth);
            m_sOutputRes.nScanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12, pConfig->nFrameHeight);
        }
        else if(QOMX_COLOR_FORMATYUV420SemiPlanarP010Venus == m_sOutputRes.nColorFmt)
        {
            m_sOutputRes.nStride = VENUS_Y_STRIDE(COLOR_FMT_P010, pConfig->nFrameWidth);
            m_sOutputRes.nScanlines = VENUS_Y_SCANLINES(COLOR_FMT_P010, pConfig->nFrameHeight);
        }
        else
        {
            VTEST_MSG_HIGH("Color format is not NV12! StridexScanlines is set to same as widhthxheight");
            m_sOutputRes.nStride = pConfig->nFrameWidth;
            m_sOutputRes.nScanlines = pConfig->nFrameHeight;
        }
        if (!pConfig->bThumbnailMode)
            m_nFrames = pConfig->nFrames;
        else {
            if (pConfig->eCodec == OMX_VIDEO_CodingAVC ||
                pConfig->eCodec == OMX_VIDEO_CodingMPEG2 ||
                pConfig->eCodec == OMX_VIDEO_CodingWMV)
            {
                m_nFrames = 2; //incase of interlace frames
            }
            else
            {
                m_nFrames = 1;
            }
        }

        if (sFileName[0] != '\0') {
            m_pFile = new File();
            if (m_pFile != NULL) {
                VTEST_MSG_MEDIUM("Opening output file...");
                result = m_pFile->Open(sFileName, OMX_FALSE);
                if (result != OMX_ErrorNone) {
                    VTEST_MSG_ERROR("Failed to open file");
                }
            } else {
                VTEST_MSG_ERROR("Failed to allocate file");
                result = OMX_ErrorInsufficientResources;
                goto bailout;

            }
        } else {
            VTEST_MSG_MEDIUM("No output file");
        }
    }
    else {
        VTEST_MSG_ERROR("bad params in config");
        result = OMX_ErrorBadParameter;
    }
bailout:
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE DecoderFileSink::SetBuffer(
        BufferInfo *pBuffer, ISource *pSource, OMX_U32 framenum) {
    (void) framenum;
    OMX_ERRORTYPE result = OMX_ErrorNone;

    result = ISource::SetBuffer(pBuffer, pSource);
    if (result != OMX_ErrorNone) {
        return result;
    }

    VTEST_MSG_LOW("queue push (%p %p)", pBuffer->pHeaderIn, pBuffer->pHeaderOut);
    result = m_pBufferQueue->Push(&pBuffer, sizeof(BufferInfo**));
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE DecoderFileSink::PortReconfig(OMX_U32 ePortIndex,
        OMX_U32 nWidth, OMX_U32 nHeight, const OMX_CONFIG_RECTTYPE& sRect) {

    (void)sRect;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    VTEST_MSG_LOW("PortReconfig port %s", OMX_PORT_NAME(ePortIndex));
    m_sOutputRes.nStartX = sRect.nLeft;
    m_sOutputRes.nStartY= sRect.nTop;
    m_sOutputRes.nCropWidth = sRect.nWidth;
    m_sOutputRes.nCropHeight= sRect.nHeight;

    if (m_sOutputRes.nWidth != nWidth || m_sOutputRes.nHeight != nHeight) {
        VTEST_MSG_MEDIUM("PortReconfig WxH (%uX%u) ==> WxH (%uX%u)",
                (unsigned int)m_sOutputRes.nWidth,
                (unsigned int)m_sOutputRes.nHeight,
                (unsigned int)nWidth,
                (unsigned int)nHeight);
        m_sOutputRes.nWidth = nWidth;
        m_sOutputRes.nHeight = nHeight;

        if(m_sOutputRes.nColorFmt == QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m)
        {
           m_sOutputRes.nStride = VENUS_Y_STRIDE(COLOR_FMT_NV12, m_sOutputRes.nWidth);
           m_sOutputRes.nScanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12, m_sOutputRes.nHeight);
        }
        else if(m_sOutputRes.nColorFmt == QOMX_COLOR_FORMATYUV420SemiPlanarP010Venus)
        {
           m_sOutputRes.nStride = VENUS_Y_STRIDE(COLOR_FMT_P010, m_sOutputRes.nWidth);
           m_sOutputRes.nScanlines = VENUS_Y_SCANLINES(COLOR_FMT_P010, m_sOutputRes.nHeight);
        }
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE DecoderFileSink::ThreadRun(OMX_PTR pThreadData) {

    (void)pThreadData;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pHeader = NULL;
    BufferInfo *pBuffer = NULL;

    for (OMX_U32 i = 1; i <= m_nFrames && !m_bThreadStop; i++) {

        result = m_pBufferQueue->Pop(&pBuffer, sizeof(pBuffer), 0);
        VTEST_MSG_LOW("queue pop %u of %u (qsize %u)",
                (unsigned int)i, (unsigned int)m_nFrames, (unsigned int)m_pBufferQueue->GetSize());

        if ((pBuffer == NULL) || (result != OMX_ErrorNone)) {
            /* Can only happen if stop is called or someone else ran into an
             * error */
            VTEST_MSG_HIGH("Stopping thread");
            result = OMX_ErrorNone;
            continue;
        }

        pHeader = pBuffer->pHeaderIn;
        if (pHeader->nFlags & OMX_BUFFERFLAG_ENDOFFRAME) {
            VTEST_MSG_HIGH("frames written %u", (unsigned int)i);
        }

        if ((pHeader->nFlags & OMX_BUFFERFLAG_EOS) || (i >= m_nFrames)) {
            VTEST_MSG_HIGH("got EOS for frame : %u", (unsigned int)i);
            m_bThreadStop = OMX_TRUE;
        }

        if (m_pFile != NULL) {

            if (pHeader->nFilledLen > 0) {
                VTEST_MSG_MEDIUM("writing frame %u with %u bytes...",
                        (unsigned int)i, (unsigned int)pHeader->nFilledLen);


                OMX_S32 nBytes;

                if (m_bSecureSession) {

                    /* This non-sense is done for secure decoder down scalar
                     * feature where beyond a certain threshold, buffer can be
                     * taken out of CPZ by venus */
                    OMX_U8 *data = (OMX_U8 *)mmap(NULL, pHeader->nFilledLen,
                            PROT_READ | PROT_WRITE, MAP_SHARED, (unsigned long)pHeader->pBuffer, 0);
                    if (data == MAP_FAILED) {
                        data = NULL;
                        VTEST_MSG_HIGH("Mapping failed - Assume secure buffer!");
                        if (m_pCrypto != NULL) {
                            data = new OMX_U8[pHeader->nAllocLen];
                            result = m_pCrypto->Copy(SAMPLECLIENT_COPY_SECURE_TO_NONSECURE,
                                    data, (unsigned long)pHeader->pBuffer, pHeader->nFilledLen);
                        } else {
                            VTEST_MSG_ERROR("Crypto object null");
                            result = OMX_ErrorBadParameter;
                        }

                        if (result != OMX_ErrorNone) {
                            VTEST_MSG_ERROR("OEMCrypto_Copy failed, result is %d", result);
                        } else {
                            result = WriteToFile(data, pHeader->nFilledLen, nBytes);
                        }

                        if (data) {
                            delete[] data;
                            data = NULL;
                        }
                    } else {
                        VTEST_MSG_HIGH("Mapping passed! - Assume buffer out of CPZ");
                        result = WriteToFile(data, pHeader->nFilledLen, nBytes);
                        if (munmap(data, pHeader->nFilledLen)) {
                            VTEST_MSG_ERROR("munmap failed for data : %p", data);
                        }
                        data = NULL;
                    }
                } else {
                    result = WriteToFile(
                            (OMX_U8 *)pHeader->pBuffer, pHeader->nFilledLen, nBytes);
                }

                if (result != OMX_ErrorNone) {
                    VTEST_MSG_ERROR("Error writing to file...");
                    SetError();
                } else if ((OMX_U32)nBytes > pHeader->nFilledLen) {
                    VTEST_MSG_ERROR(
                            "Error mismatched number of bytes %d vs expected : %d in file write",
                            nBytes, (OMX_S32)pHeader->nFilledLen);
                    SetError();
                    result = OMX_ErrorUndefined;
                }
            } else {
                VTEST_MSG_HIGH("skipping frame %u, 0 length...", (unsigned int)i);
            }
        } else {
            VTEST_MSG_MEDIUM("received frame %u... but did not write as no file present", (unsigned int)i);
        }

        m_pSource->SetBuffer(pBuffer, this);
        VTEST_SIMPLE_MSG_CONSOLE("%d.", i);
    }

    VTEST_SIMPLE_MSG_CONSOLE("\n\n");
    //clean up
    while(m_pBufferQueue->GetSize() > 0) {
        VTEST_MSG_LOW("cleanup: q-wait (qsize %u)", (unsigned int)m_pBufferQueue->GetSize());
        m_pBufferQueue->Pop(&pBuffer, sizeof(BufferInfo **), 0);
        m_pSource->SetBuffer(pBuffer, this);
    }

    VTEST_MSG_HIGH("thread exiting...");
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE DecoderFileSink::WriteToFile(
        OMX_U8 *pData, OMX_U32 nFilledLen, OMX_S32 &nTotalBytes) {

    OMX_U32 i = 0;
    OMX_S32 nBytes = 0;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    nTotalBytes = 0;
    OMX_U8 *pBuffer = pData;
    VidcStatus ret = VidcStatusSuccess;
    BOOL bCurFrameCRC = FALSE;
    OMX_U32 bytePerPixel = 1;


    if (m_sOutputRes.nColorFmt == QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m ||
        m_sOutputRes.nColorFmt == QOMX_COLOR_FORMATYUV420SemiPlanarP010Venus) {
        if(m_pCRCChecker)
        {
            ret = m_pCRCChecker->CheckCRCofYUVframe(m_pCRCChecker, pData,
                    &m_sOutputRes, TRUE, &bCurFrameCRC);
            if(ret)
            {
                VTEST_MSG_HIGH("CRC failure %d  at Frame no: %d",ret, m_pCRCChecker->m_fCRCCounter);
                result = OMX_ErrorUndefined;
                FAILED1(result, "Failed CRC validation");
            }
        }

        bytePerPixel = m_sOutputRes.nColorFmt == QOMX_COLOR_FORMATYUV420SemiPlanarP010Venus ? 2 : 1;

        for (i = 0; i < m_sOutputRes.nHeight; i++) {
            result = m_pFile->Write(pData, m_sOutputRes.nWidth * bytePerPixel, &nBytes);
            pData += m_sOutputRes.nStride;
            nTotalBytes += nBytes;
            FAILED1(result, "Failed to write to file");
        }

        pData = pBuffer + m_sOutputRes.nStride * m_sOutputRes.nScanlines;
        for (i = 0; i < m_sOutputRes.nHeight/2; i++) {
            result = m_pFile->Write(pData, m_sOutputRes.nWidth * bytePerPixel, &nBytes);
            pData += m_sOutputRes.nStride;
            nTotalBytes += nBytes;
            FAILED1(result, "Failed to write to file");
        }
    } else {
        result = m_pFile->Write(pData, nFilledLen, &nTotalBytes);
    }
    return result;
}

} // namespace vtest

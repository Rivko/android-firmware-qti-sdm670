/*-------------------------------------------------------------------
Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------*/

#include "venc/inc/omx_video_common.h"
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#ifdef _ANDROID_
#include <cutils/properties.h>
#endif

#include "vtest_ComDef.h"
#include "vtest_Debug.h"
#include "vtest_Thread.h"
#include "vtest_SignalQueue.h"
#include "vtest_File.h"
#include "vtest_Time.h"
#include "vtest_EncoderFileSink.h"
#include "vtest_Parser.h"
#include "vtest_Sleeper.h"
#include <OMX_QCOMExtns.h>

#undef LOG_TAG
#define LOG_TAG "VTEST_ENCODER_FILE_SINK"
namespace vtest {

static const OMX_S32 MAX_BUFFER_ASSUME = 256;
static const OMX_U8 HEADER_END = 32;
static const OMX_U32 QCOM_FLAG_FREE_BUFFER = 0x00100000;
static const OMX_U32 QCOM_FLAG_SEEK_START = 0x00200000;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
EncoderFileSink::EncoderFileSink(Crypto *pCrypto)
    : ISource(),
      m_nFrames(0),
      m_pFile(),
      m_bIsProfileMode(OMX_FALSE),
      m_bIsVTPath(OMX_FALSE),
      m_bIsVPX(OMX_FALSE),
      m_nWidth(0),
      m_nHeight(0),
      m_nScale(0),
      m_nRate(0),
      m_bSecureSession(OMX_FALSE),
      m_pOutCopyBuf(NULL),
      m_nOutCopyBufSize(0),
      m_pCrypto(pCrypto) {

    snprintf(m_pName, MAX_NAME_LENGTH, "%s", "EncoderFileSink");
    VTEST_MSG_LOW("%s: created...", Name());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
EncoderFileSink::~EncoderFileSink() {

    VTEST_MSG_LOW("start");
    if (m_pFile != NULL) {
        (void)m_pFile->Close();
        delete m_pFile;
    }
    if (m_pOutCopyBuf != NULL) {
        delete m_pOutCopyBuf;
    }

    VTEST_MSG_LOW("done");
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
PortBufferCapability EncoderFileSink::GetBufferRequirements(OMX_U32 ePortIndex) {
    PortBufferCapability sBufCap;
    memset(&sBufCap, 0, sizeof(PortBufferCapability));

    if (ePortIndex == PORT_INDEX_IN) {
        sBufCap.bAllocateBuffer = OMX_FALSE;
        sBufCap.bUseBuffer = OMX_TRUE;
        sBufCap.pSource = this;
        sBufCap.ePortIndex = ePortIndex;
        sBufCap.nMinBufferSize = 0x1000;
        sBufCap.nMinBufferCount = 1;
        sBufCap.nExtraBufferCount = 0;
        sBufCap.nBufferUsage = 0;
    } else {
        VTEST_MSG_ERROR("Error: invalid port selection");
    }

    return sBufCap;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE EncoderFileSink::Configure(CodecConfigType *pConfig,
        BufferManager *pBufManager, ISource *pSource, ISource *pSink) {

    OMX_ERRORTYPE result = OMX_ErrorNone;

    result = ISource::Configure(pConfig, pBufManager, pSource, pSink);
    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("EncoderFileSink configure failed");
        return result;
    }

    Mutex::Autolock autoLock(m_pLock);

    char sFileName[VTEST_MAX_STRING];;
    OMX_S32 nTestNum = pConfig->nCurrentTestNum;

    m_nWidth = pConfig->nOutputFrameWidth;
    m_nHeight = pConfig->nOutputFrameHeight;
    m_nFrames = pConfig->nFrames;
    pConfig->bProfileMode = m_bIsProfileMode;
    m_bSecureSession = pConfig->bSecureSession;

    SNPRINTF(sFileName,VTEST_MAX_STRING,"%s%s",sGlobalStaticVideoProp.sOutRoot,pConfig->cOutFileName);

#if 0 // removed to simplify, will have to re-add if needed
    char value[PROPERTY_VALUE_MAX] = { 0 };
    property_get("vidc.venc.slicedeliverymode", value, "0");
    if (atoi(value)) {
        VTEST_MSG_HIGH("Slice delivery mode enabled on File Sink via setprop command.");
        m_bSliceDeliveryMode = OMX_TRUE;
    }
#endif

    if (pConfig->eCodec == OMX_VIDEO_CodingVP8) {
        m_bIsVPX = OMX_TRUE;
        m_nScale = 1; // Microsecond timebase for OMX
        m_nRate = 1000000;
    }

    if (sFileName[0] != '\0') {
        m_pFile = new File();
        if (m_pFile != NULL) {
            VTEST_MSG_MEDIUM("Opening output file: %s...", sFileName);
            result = m_pFile->Open(sFileName, OMX_FALSE);
            if (result != OMX_ErrorNone) {
                VTEST_MSG_ERROR("Failed to open file");
            }
            if (m_bIsVPX) {
                m_pFile->SeekStart(HEADER_END);  // Skip the header for now, as framecount is unknown
            }
        } else {
            VTEST_MSG_ERROR("Failed to allocate file");
            result = OMX_ErrorInsufficientResources;
        }
    } else {
        VTEST_MSG_MEDIUM("No output file");
    }

    for(OMX_U32 i=0; i<pConfig->nExtradataConfigArraySize; i++)
    {
        switch ((int)pConfig->pExtradataProperties[i].ExtradataType)
        {
            default:
            {
                VTEST_MSG_HIGH("Unsupported extradata type 0x%x\n",pConfig->pExtradataProperties[i].ExtradataType);
                break;
            }
        }
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE EncoderFileSink::SetBuffer(
        BufferInfo *pBuffer, ISource *pSource, OMX_U32 framenum) {
    (void) framenum;
    OMX_ERRORTYPE result = OMX_ErrorNone;

    result = ISource::SetBuffer(pBuffer, pSource);
    if (result != OMX_ErrorNone) {
        return result;
    }

    VTEST_MSG_LOW("queue push (%p %p)", pBuffer->pHeaderIn, pBuffer->pHeaderOut);
    result = m_pBufferQueue->Push(&pBuffer, sizeof(BufferInfo **));
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE EncoderFileSink::ThreadRun(OMX_PTR pThreadData) {

    (void)pThreadData;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pHeader = NULL;
    BufferInfo *pBuffer = NULL;

    VTEST_MSG_MEDIUM("thread started");

    if (m_bIsVPX && !m_bSecureSession) {
        OMX_BUFFERHEADERTYPE *pIvfBufferHdr = makeHeaderIvf(
                                                     m_nFrames,
                                                     m_nWidth,
                                                     m_nHeight,
                                                     m_nScale,
                                                     m_nRate);
        writeFrame(pIvfBufferHdr);

        if (pIvfBufferHdr) {
            if (pIvfBufferHdr->pBuffer) {
                free(pIvfBufferHdr->pBuffer);
            }
            free(pIvfBufferHdr);
        }
    }

    for (OMX_S32 i = 1; !m_bThreadStop; i++) {

        result = m_pBufferQueue->Pop(&pBuffer, sizeof(BufferInfo**), 0);
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

        if (m_bIsVPX && !m_bSecureSession && pHeader->nFilledLen) {
            OMX_BUFFERHEADERTYPE *pIvfBufferHdr = makeFrameHeaderIvf(pHeader);
            writeFrame(pIvfBufferHdr);

            if (pIvfBufferHdr) {
                if (pIvfBufferHdr->pBuffer) {
                    free(pIvfBufferHdr->pBuffer);
                }
                free(pIvfBufferHdr);
            }
        }

        writeFrame(pHeader);
        VTEST_MSG_LOW("wrote frame %u", (unsigned int)i);
        result = handleExtradata(pHeader);
        if (result != OMX_ErrorNone)
        {
            VTEST_MSG_ERROR("Error handling extra data");
            SetError();
        }
        if (pHeader->nFlags & OMX_BUFFERFLAG_EOS || m_pSource->m_EOS3) {
            // this is the last frame. note that we may get fewer frames
            // than expected if RC is enabled with frame skips
            VTEST_MSG_HIGH("got EOS, frame_ct %u", (unsigned int)i);
            m_bThreadStop = OMX_TRUE;
            m_pSource->m_EOS3 = OMX_FALSE;
        }

        m_pSource->SetBuffer(pBuffer, this);
    } // for loop end

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
OMX_ERRORTYPE EncoderFileSink::writeFrame(OMX_BUFFERHEADERTYPE *pHeader) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_S32 nBytes(0);

    Mutex::Autolock autoLock(m_pLock);

    if (!pHeader) {
        VTEST_MSG_ERROR("invalid pHeader");
        return OMX_ErrorBadParameter;
    }

    if (m_pFile == NULL || pHeader->nFilledLen <= 0) {
        VTEST_MSG_LOW("received bytes %u...", (unsigned int)pHeader->nFilledLen);
        return OMX_ErrorNone;
    }

    if (pHeader->nFlags & QCOM_FLAG_SEEK_START) {
        VTEST_MSG_MEDIUM("Seeking to file start before writing IVF file header");
        m_pFile->SeekStart(0);
    }
    VTEST_MSG_MEDIUM("Writing frame with %u bytes; nOffset = %u",
            (unsigned int)pHeader->nFilledLen, (unsigned int)pHeader->nOffset);

    if (m_bSecureSession)
    {
        MetaBuffer *buffer = (MetaBuffer *)pHeader->pBuffer;
        NativeHandle *handle = buffer->meta_handle;

        if (handle == NULL) {
            VTEST_MSG_ERROR("invalid NativeHandle");
            return OMX_ErrorBadParameter;
        }

        if (m_pOutCopyBuf == NULL)
        {
            m_nOutCopyBufSize = handle->data[3];
            m_pOutCopyBuf = new OMX_U8[m_nOutCopyBufSize];
        }

        if (m_nOutCopyBufSize >= (OMX_U32)handle->data[3])
        {
            if (m_pCrypto != NULL && handle->data[0] >= 0)
            {
                result = m_pCrypto->Copy(SAMPLECLIENT_COPY_SECURE_TO_NONSECURE,
                                                m_pOutCopyBuf, handle->data[0],
                                                              handle->data[2]);

                if (result != OMX_ErrorNone)
                {
                    VTEST_MSG_ERROR("OEMCrypto_Copy failed, result is %d", result);
                }
                else
                {
                    result = m_pFile->Write(m_pOutCopyBuf, handle->data[2],
                                                                  &nBytes);
                }
            }
            else
            {
                VTEST_MSG_ERROR("bad secure output handle.");
            }
        }
        else
        {
            VTEST_MSG_ERROR("variable output buffer size!");
        }
    }
    else
    {
        result = m_pFile->Write(pHeader->pBuffer + pHeader->nOffset,
                                      pHeader->nFilledLen, &nBytes);
    }

    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("error writing to file...");
    }


    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/**
 * Makes a 32 byte file header for IVF format.
 *
 * Timebase fraction is in format scale/rate, e.g. 1/1000
 *
 * @param nFrameCount total number of frames file contains
 * @param nWidth      frame width
 * @param bHeight     frame height
 * @param nScale      timebase scale (or numerator of the timebase fraction)
 * @param nRate       timebase rate (or denominator of the timebase fraction)
 */
OMX_BUFFERHEADERTYPE* EncoderFileSink::makeHeaderIvf(int nFrameCount,
                    int nWidth, int nHeight, int nScale, int nRate) {

    OMX_BUFFERHEADERTYPE *pIvfBufferHdr = NULL;
    OMX_U8 *pIvfBuffer = NULL;

    pIvfBufferHdr = (OMX_BUFFERHEADERTYPE *)calloc(1, sizeof(OMX_BUFFERHEADERTYPE));
    if (pIvfBufferHdr == NULL) return NULL;

    pIvfBuffer = (OMX_U8 *)calloc(32, sizeof(OMX_U8));
    if (pIvfBuffer == NULL) {
        free(pIvfBufferHdr);
        return NULL;
    }

    pIvfBufferHdr->pBuffer = pIvfBuffer;
    pIvfBufferHdr->nOffset = 0;
    pIvfBufferHdr->nAllocLen = 32 * sizeof(OMX_U8);
    pIvfBufferHdr->nFilledLen = pIvfBufferHdr->nAllocLen;
    pIvfBufferHdr->nFlags = QCOM_FLAG_FREE_BUFFER | OMX_BUFFERFLAG_CODECCONFIG | QCOM_FLAG_SEEK_START;

    pIvfBuffer[0] = 'D';
    pIvfBuffer[1] = 'K';
    pIvfBuffer[2] = 'I';
    pIvfBuffer[3] = 'F';
    add16Bits(pIvfBuffer, 4, 0);  // version
    add16Bits(pIvfBuffer, 6, 32);  // header size
    pIvfBuffer[8] = 'V';  // fourcc
    pIvfBuffer[9] = 'P';
    pIvfBuffer[10] = '8';
    pIvfBuffer[11] = '0';
    add16Bits(pIvfBuffer, 12, nWidth);
    add16Bits(pIvfBuffer, 14, nHeight);
    add32Bits(pIvfBuffer, 16, nRate);  // scale/rate
    add32Bits(pIvfBuffer, 20, nScale);
    add32Bits(pIvfBuffer, 24, nFrameCount);
    add32Bits(pIvfBuffer, 28, 0);  // unused
    VTEST_MSG_HIGH("VPX: ivf file, final nFrameCount = %d", nFrameCount);
    return pIvfBufferHdr;
}

/**
 * Makes a 12 byte header for an encoded frame.
 *
 * @param size      frame size
 * @param timestamp presentation timestamp of the frame
 */
OMX_BUFFERHEADERTYPE* EncoderFileSink::makeFrameHeaderIvf(OMX_BUFFERHEADERTYPE *pBuffer) {

    OMX_BUFFERHEADERTYPE *pIvfBufferHdr = NULL;
    OMX_U8 *pIvfBuffer = NULL;

    pIvfBufferHdr = (OMX_BUFFERHEADERTYPE *)calloc(1, sizeof(OMX_BUFFERHEADERTYPE));
    if (pIvfBufferHdr == NULL) return NULL;
    pIvfBuffer = (OMX_U8 *)calloc(12, sizeof(OMX_U8));
    if (pIvfBuffer == NULL) {
        free(pIvfBufferHdr);
        return NULL;
    }
    pIvfBufferHdr->pBuffer = pIvfBuffer;
    pIvfBufferHdr->nOffset = 0;
    pIvfBufferHdr->nAllocLen = 12 * sizeof(OMX_U8);
    pIvfBufferHdr->nFilledLen = pIvfBufferHdr->nAllocLen;
    pIvfBufferHdr->nFlags = QCOM_FLAG_FREE_BUFFER | OMX_BUFFERFLAG_CODECCONFIG;

    VTEST_MSG_LOW("nTimeStamp = %llu", pBuffer->nTimeStamp);

    add32Bits(pIvfBuffer, 0, pBuffer->nFilledLen);
    add64Bits(pIvfBuffer, 4, pBuffer->nTimeStamp);
    return pIvfBufferHdr;
}

/**
 * Lays least significant 16 bits of an int into 2 items of a byte array.
 *
 * Note that ordering is little-endian.
 *
 * @param aArray     the array to be modified
 * @param nIndex     index of the array to start laying down
 * @param nValue     the integer to use least significant 16 bits
 */
void EncoderFileSink::add16Bits(OMX_U8 aArray[], OMX_U32 nIndex, OMX_U32 nValue) {

    aArray[nIndex] = (OMX_U8)nValue;
    aArray[nIndex + 1] = (OMX_U8)(nValue >> 8);
}

/**
 * Lays an int into 4 items of a byte array.
 *
 * Note that ordering is little-endian.
 *
 * @param nArray     the array to be modified
 * @param nIndex     index of the array to start laying down
 * @param nValue     the integer to use
 */
void EncoderFileSink::add32Bits(OMX_U8 aArray[], OMX_U32 nIndex, OMX_U32 nValue) {

    for (int i = 0; i < 4; i++) {
        aArray[nIndex + i] = (OMX_U8)(nValue >> (i * 8));
    }
}

/**
 * Lays a long int into 8 items of a byte array.
 *
 * Note that ordering is little-endian.
 *
 * @param aArray     the array to be modified
 * @param nIndex     index of the array to start laying down
 * @param nValue     the integer to use
 */
void EncoderFileSink::add64Bits(OMX_U8 aArray[], OMX_U32 nIndex, OMX_U64 nValue) {

    for (int i = 0; i < 8; i++) {
        aArray[nIndex + i] = (OMX_U8)(nValue >> (i * 8));
    }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE EncoderFileSink::handleExtradata(OMX_BUFFERHEADERTYPE *pHeader) {

    OMX_OTHER_EXTRADATATYPE *pExtra;
    OMX_U8 *pTmp = pHeader->pBuffer + pHeader->nOffset + pHeader->nFilledLen;
    OMX_ERRORTYPE status = OMX_ErrorNone;

    if (!(pHeader->nFlags & OMX_BUFFERFLAG_EXTRADATA))
        return status;

    pExtra = (OMX_OTHER_EXTRADATATYPE *)(((unsigned long) pTmp + 3) & ~3);
    VTEST_MSG_LOW("pExtra(0x%p): nSize = %d, eType = 0x%x",
            pExtra, (int)pExtra->nSize, pExtra->eType);

    while ((pExtra->eType != OMX_ExtraDataNone) &&
          (status == OMX_ErrorNone))
    {
        switch ((OMX_U32)pExtra->eType)
        {
	    case OMX_ExtraDataEncoderFrameQp:
                VTEST_MSG_MEDIUM("OMX_ExtraDataEncoderFrameQp: %d", *pExtra->data);
                break;
            default:
                break;
        }

        pExtra = (OMX_OTHER_EXTRADATATYPE *)(((OMX_U8 *)pExtra) + pExtra->nSize);
        VTEST_MSG_MEDIUM("pExtra(0x%p): nSize = %d, eType = 0x%x",
                pExtra, (int)pExtra->nSize, pExtra->eType);
    }
return status;
}

} // namespace vtest

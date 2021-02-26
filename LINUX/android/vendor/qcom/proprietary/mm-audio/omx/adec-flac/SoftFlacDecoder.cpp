/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "SoftFlacDecoder"
#include <utils/Log.h>
#include <dlfcn.h>
#include <inttypes.h>
#include "SoftFlacDecoder.h"

#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/MediaDefs.h>

static int number_of_adec_flac_instance;
static pthread_mutex_t lock_dec = PTHREAD_MUTEX_INITIALIZER;
#define MAX_ADEC_FLAC_INSTANCE 10
namespace android {
static const char* FLAC_DECODER_LIB = "libFlacSwDec.so";

template<class T>
static void InitOMXParams(T *params) {
    params->nSize = sizeof(T);
    params->nVersion.s.nVersionMajor = 1;
    params->nVersion.s.nVersionMinor = 0;
    params->nVersion.s.nRevision = 0;
    params->nVersion.s.nStep = 0;
}

SoftFlacDecoder::SoftFlacDecoder(
    const char *name,
    const OMX_CALLBACKTYPE *callbacks,
    OMX_PTR appData,
    OMX_COMPONENTTYPE **component)
    : SimpleSoftOMXComponent(name, callbacks, appData, component),
      mInitStatus(false),
      mNumChannels(2),
      mSamplingRate(44100),
      mOutBytesPerSample(2),
      mAnchorTimeUs(0),
      mNumFramesOutput(0),
      mDecodedBlocks(0),
      mLibHandle(NULL),
      mOutBuffer(NULL),
      mDecoderInit(NULL),
      mProcessData(NULL),
      mDecoderEnd(NULL),
      mSignalledError(false) {
    ALOGV("SoftFlacDecoder::SoftFlacDecoder(name=%s)", name);
    mBuffer.i32BufferSize = 0;
    mBuffer.i32ReadPtr = 0;
    mBuffer.i32WritePtr = 0;
    mBuffer.ui8TempBuf = NULL;
    mBuffer.eos = 0;
    mBuffer.error = 0;
    mTmpBuf = NULL;
    initPorts();
    parserInfoToPass.i32NumChannels = 2;
    parserInfoToPass.i32SampleRate = 48000;
    parserInfoToPass.i32BitsPerSample = 16;
    parserInfoToPass.i32MinBlkSize = 16;
    parserInfoToPass.i32MaxBlkSize = 16;
    parserInfoToPass.i32MinFrmSize = 0;
    parserInfoToPass.i32MaxFrmSize = 0;

}

SoftFlacDecoder::~SoftFlacDecoder() {
    ALOGV("SoftFlacDecoder::~SoftFlacDecoder()");

    if (mOutBuffer) {
        free(mOutBuffer);
        mOutBuffer = NULL;
    }

    if (mTmpBuf) {
        free(mTmpBuf);
        mTmpBuf = NULL;
    }

    if (mDecoderEnd && mInitStatus) {
        (*mDecoderEnd)(&flacDecState);
        mDecoderEnd = NULL;
    }

    if (mLibHandle) {
        dlclose(mLibHandle);
        mLibHandle = NULL;
    }

    if (mBuffer.ui8TempBuf) {
        free(mBuffer.ui8TempBuf);
        mBuffer.ui8TempBuf = NULL;
    }
    mInitStatus = false;
    pthread_mutex_lock(&lock_dec);
    number_of_adec_flac_instance--;
    pthread_mutex_unlock(&lock_dec);
}

void SoftFlacDecoder::initPorts() {
    ALOGV("SoftFlacDecoder::initPorts() Enter");

    OMX_PARAM_PORTDEFINITIONTYPE def;
    InitOMXParams(&def);

    // configure input port of the Decoder
    def.nPortIndex = 0;
    def.eDir = OMX_DirInput;
    def.nBufferCountMin = kNumInputBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = kMaxInputBufferSize;
    def.bEnabled = OMX_TRUE;
    def.bPopulated = OMX_FALSE;
    def.eDomain = OMX_PortDomainAudio;
    def.bBuffersContiguous = OMX_FALSE;
    def.nBufferAlignment = 1;

    def.format.audio.cMIMEType = const_cast<char *>(MEDIA_MIMETYPE_AUDIO_FLAC);
    def.format.audio.pNativeRender = NULL;
    def.format.audio.bFlagErrorConcealment = OMX_FALSE;
    def.format.audio.eEncoding = OMX_AUDIO_CodingFLAC;

    addPort(def);

    // configure output port of the Decoder
    def.nPortIndex = 1;
    def.eDir = OMX_DirOutput;
    def.nBufferCountMin = kNumOutputBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = kMaxOutputBufferSize;
    def.bEnabled = OMX_TRUE;
    def.bPopulated = OMX_FALSE;
    def.eDomain = OMX_PortDomainAudio;
    def.bBuffersContiguous = OMX_FALSE;
    def.nBufferAlignment = 2;

    def.format.audio.cMIMEType = const_cast<char *>(MEDIA_MIMETYPE_AUDIO_RAW);
    def.format.audio.pNativeRender = NULL;
    def.format.audio.bFlagErrorConcealment = OMX_FALSE;
    def.format.audio.eEncoding = OMX_AUDIO_CodingPCM;

    addPort(def);
}

OMX_ERRORTYPE SoftFlacDecoder::init(FLACDec_ParserInfo* parserInfoToPass) {
    int result = 0;

    ALOGV("SoftFlacDecoder::init() Enter");
    mLibHandle = dlopen(FLAC_DECODER_LIB, RTLD_LAZY);
    if (!mLibHandle) {
        ALOGE("SoftFlacDecoder: CSIM decoder library open failed!");
        return OMX_ErrorComponentNotFound;
    }

    mDecoderInit = (DecoderInit) dlsym (mLibHandle, "CFlacDecoderLib_Meminit");
    mProcessData = (DecoderLib_Process) dlsym (mLibHandle, "CFlacDecoderLib_Process");
    mDecoderEnd = (DecoderEnd)dlsym(mLibHandle, "CFlacDecoderLib_End");

    if (!mDecoderInit || !mProcessData || !mDecoderEnd) {
        ALOGE("SoftFlacDecoder: Symbols not found in the CSIM decoder library!");
        return OMX_ErrorComponentNotFound;
    }

    mOutBuffer = (uint8_t *) malloc (FLAC_INSTANCE_SIZE);
    mTmpBuf = (uint8_t *) malloc (FLAC_INSTANCE_SIZE);

    if (!mOutBuffer || !mTmpBuf) {
        ALOGW("Failed to allocate ouput or scratch buffer\n");
        mInitStatus = false;
        return  OMX_ErrorInsufficientResources;
    }

    memset(&flacDecState,0,sizeof(CFlacDecState));

    (*mDecoderInit)(&flacDecState, &result, parserInfoToPass->i32BitsPerSample);

    if (result != DEC_SUCCESS) {
        ALOGE("SoftFlacDecoder: CSIM decoder init failed! Result %d", result);
        if (result == DEC_NOMEMORY_FAILURE) {
            return  OMX_ErrorInsufficientResources;
        }
        return OMX_ErrorUndefined;
    } else {
        mInitStatus = true;
    }

    stFLACDec* pstFLACDec = (stFLACDec*)(flacDecState.m_pFlacDecoder);
    memcpy(&pstFLACDec->MetaDataBlocks.MetaDataStrmInfo, parserInfoToPass, sizeof(FLACDec_ParserInfo));
    flacDecState.m_bIsStreamInfoPresent = 1;
    flacDecState.ui32MaxBlockSize = pstFLACDec->MetaDataBlocks.MetaDataStrmInfo.i32MaxBlkSize;
    memcpy(flacDecState.pFlacDecMetaDataStrmInfo, parserInfoToPass, sizeof(FLACDec_ParserInfo));

    if (flacDecState.ui32MaxBlockSize > SUPERSET_THRESHOLD) {
        ALOGV("Allocating bigger buffer for superset clips");
        mBuffer.i32BufferSize = 2*(BUFFERING_SIZE);
    } else {
        mBuffer.i32BufferSize = BUFFERING_SIZE;
    }
    mBuffer.ui8TempBuf = (uint8_t*)malloc(mBuffer.i32BufferSize);
    mBuffer.i32ReadPtr = 0;
    mBuffer.i32WritePtr = 0;
    mBuffer.eos = 0;
    mBuffer.error = 0;

    if (!mBuffer.ui8TempBuf) {
        ALOGW("Failed to allocate temp buffer\n");
        mInitStatus = false;
        return  OMX_ErrorInsufficientResources;
    }

    if (parserInfoToPass->i32BitsPerSample == 24) {
        //decoder output will be in 8_24 PCM format which is 4 bytes
        mOutBytesPerSample = sizeof(uint32_t);
    } else {
        mOutBytesPerSample = sizeof(uint16_t);
    }

    mIsDecodeSuccess = true;
    eos = false;
    mNotifyInputBufferEmpty = true;
    inInfo = NULL;
    outInfo = NULL;
    inHeader = NULL;
    outHeader = NULL;
    mAnchorTimeUs = 0;
    mNumFramesOutput = 0;

    return OMX_ErrorNone;
}

void SoftFlacDecoder::onPortFlushCompleted(OMX_U32 portIndex) {
    ALOGV("SoftFlacDecoder: onPortFlushCompleted(), portIndex=%d", portIndex);

    mBuffer.i32WritePtr = 0;
    mBuffer.i32ReadPtr = 0;
    mBuffer.eos = 0;
    eos = false;
    mNotifyInputBufferEmpty = true;
    mDecodedBlocks = 0;
    mIsDecodeSuccess = true;
    inInfo = NULL;
    outInfo = NULL;
    inHeader = NULL;
    outHeader = NULL;
    mAnchorTimeUs = 0;
    mNumFramesOutput = 0;

    return;
}

void SoftFlacDecoder::onReset() {
    ALOGV("SoftFlacDecoder: onReset()");

    onPortFlushCompleted(0);
}

int32_t SoftFlacDecoder::enoughDataAvailable(flacByteStreamBuffer *pByteBuffer) {
    uint32_t bytesRemain;

    /* Check if data from parser is needed or not */
    bytesRemain = pByteBuffer->i32WritePtr - pByteBuffer->i32ReadPtr;
    if (((bytesRemain >= THRESHOLD) || (pByteBuffer->eos)) && (!pByteBuffer->error)) {
        return 1;
    }

    return 0;
}

int32_t SoftFlacDecoder::updateInputBitstream(flacByteStreamBuffer *pByteBuffer, void * bitstream, int32_t inSize) {
    uint32_t i, bytesRemain;
    uint8_t *bitsbuffer = (uint8_t*) bitstream;

    bytesRemain = pByteBuffer->i32WritePtr - pByteBuffer->i32ReadPtr;
    ALOGV("updateInputBitstream: bytesRemain : %d , InData : %d, pByteBuffer->error : %d, pByteBuffer->eos: %d",
            bytesRemain, inSize, pByteBuffer->error, pByteBuffer->eos);
    if (((bytesRemain >= THRESHOLD) || (pByteBuffer->eos) || (inSize == 0)) && (!pByteBuffer->error)) {
        pByteBuffer->error = 0;
        return 0;
    }
    pByteBuffer->error = 0;

    /* First copy the un-decoded bitstream to start of buffer */
    if (bytesRemain) {
        for (i = 0; i < bytesRemain; i++) {
            pByteBuffer->ui8TempBuf[i] = pByteBuffer->ui8TempBuf[pByteBuffer->i32ReadPtr+i];
        }
        pByteBuffer->i32ReadPtr = 0;
        pByteBuffer->i32WritePtr = bytesRemain;
    }

    uint32_t sizeRemaining = pByteBuffer->i32BufferSize - pByteBuffer->i32WritePtr;
    if (inSize > (int32_t)sizeRemaining) {
        // report error for now?
        ALOGE("inSize (%d) > avail size (%d)", inSize, sizeRemaining);
        return -1;
    } else {
        memcpy(&pByteBuffer->ui8TempBuf[pByteBuffer->i32WritePtr], bitsbuffer, inSize);
        pByteBuffer->i32WritePtr += inSize;
    }

    return 1;
}

int32_t SoftFlacDecoder::updatePointers(flacByteStreamBuffer *pByteBuffer, uint32_t readBytes, int32_t result) {
    pByteBuffer->i32ReadPtr += readBytes;
    if ((result == FLACDEC_SUCCESS) || (result == FLACDEC_EOF)) {
        ALOGV("SoftFlacDecoder: Successful decode!");
        return 1;
    } else if ((result == FLACDEC_FAIL) || (result == FLACDEC_METADATA_NOT_FOUND)) {
        ALOGV("SoftFlacDecoder: Erroneous decode!");
    } else if (result == FLACDEC_ERROR_CODE_NEEDS_MORE_DATA) {
        ALOGV("SoftFlacDecoder: Not enough data to decode!");
    }

    if ((pByteBuffer->i32WritePtr - (pByteBuffer->i32ReadPtr)) >= pByteBuffer->i32BufferSize) {
    /* This can happen only if the entire data contains erroneous data
       and no sync word has been found
       Reset the internal buffering to indicate empty buffer and consume
       all stored bytes and request more data from parser. */
        pByteBuffer->i32WritePtr = 0;
        pByteBuffer->i32ReadPtr = 0;
    }
    pByteBuffer->error = 1;

    return 0;
}

bool SoftFlacDecoder::checkIfEnoughDecodedPCMSamples(int64_t decodedBlocks)
{
     if ((FLAC_OUTPUT_BUFFERING_THRESHOLD_IN_MSEC <= (uint64_t)((decodedBlocks * 1000LL)/mSamplingRate)) ||
          (decodedBlocks * mOutBytesPerSample * mNumChannels >= kMaxOutputBufferSize))
          return true;
     else {
          ALOGV("Not enough decoded data %" PRId64, decodedBlocks);
          return false;
     }
}

OMX_ERRORTYPE SoftFlacDecoder::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {

    switch ((int)index) {
         case OMX_IndexParamAudioPortFormat:
        {
            OMX_AUDIO_PARAM_PORTFORMATTYPE *formatParams =
                (OMX_AUDIO_PARAM_PORTFORMATTYPE *)params;

            if (!isValidOMXParam(formatParams)) {
                return OMX_ErrorBadParameter;
            }

            if (formatParams->nPortIndex > 1) {
                return OMX_ErrorUndefined;
            }

            if (formatParams->nIndex > 0) {
                return OMX_ErrorNoMore;
            }

            formatParams->eEncoding =
                (formatParams->nPortIndex == 0)
                    ? OMX_AUDIO_CodingFLAC : OMX_AUDIO_CodingPCM;

            return OMX_ErrorNone;
        }
        case OMX_IndexParamAudioFlac:
        {
            /* This case is needed to handle Google SW FLAC Encoder and QTI SW FLAC
               Decoder which share the mime type */
            ALOGV("SoftFlacDecoder::internalGetParameter(OMX_IndexParamAudioFlac)");
            OMX_AUDIO_PARAM_FLACTYPE *flacParams =
                (OMX_AUDIO_PARAM_FLACTYPE *)params;

            if (!isValidOMXParam(flacParams)) {
                ALOGE("internalGetParameter(OMX_IndexParamAudioFlac): invalid omx params");
                return OMX_ErrorBadParameter;
            }

            if (flacParams->nPortIndex != 0) {
                ALOGE("internalGetParameter(OMX_IndexParamAudioFlac): bad port index");
                return OMX_ErrorBadPortIndex;
            }
            flacParams->nCompressionLevel = 0;
            if (mInitStatus) {
                stFLACDec* pstFLACDec = (stFLACDec*)(flacDecState.m_pFlacDecoder);
                stMetaDataStrmInfo* pstMetaDataStrmInfo =
                    (stMetaDataStrmInfo*)(&pstFLACDec->MetaDataBlocks.MetaDataStrmInfo);
                flacParams->nChannels = pstMetaDataStrmInfo->i32NumChannels;
                flacParams->nSampleRate = pstMetaDataStrmInfo->i32SampleRate;
            } else {
                flacParams->nChannels = parserInfoToPass.i32NumChannels;
                flacParams->nSampleRate = parserInfoToPass.i32SampleRate;
            }
            return OMX_ErrorNone;
        }

        case (OMX_INDEXTYPE)QOMX_IndexParamAudioFlacDec:
        {
            ALOGV("SoftFlacDecoder::internalGetParameter(QOMX_IndexParamAudioFlacDec)");
            QOMX_AUDIO_PARAM_FLAC_DEC_TYPE *flacDecParam =
                (QOMX_AUDIO_PARAM_FLAC_DEC_TYPE *) params;

            if (0 == flacDecParam->nPortIndex) {
                if (!mInitStatus) {
                    flacDecParam->nChannels = parserInfoToPass.i32NumChannels;
                    flacDecParam->nSampleRate = parserInfoToPass.i32SampleRate;
                    flacDecParam->nBitsPerSample = parserInfoToPass.i32BitsPerSample;
                    flacDecParam->nMinBlkSize = parserInfoToPass.i32MinBlkSize;
                    flacDecParam->nMaxBlkSize = parserInfoToPass.i32MaxBlkSize;
                    flacDecParam->nMinFrmSize = parserInfoToPass.i32MinFrmSize;
                    flacDecParam->nMaxFrmSize = parserInfoToPass.i32MaxFrmSize;
                } else {
                    stFLACDec* pstFLACDec = (stFLACDec*)(flacDecState.m_pFlacDecoder);
                    stMetaDataStrmInfo* pstMetaDataStrmInfo =
                        (stMetaDataStrmInfo*)(&pstFLACDec->MetaDataBlocks.MetaDataStrmInfo);

                    flacDecParam->nChannels = pstMetaDataStrmInfo->i32NumChannels;
                    flacDecParam->nSampleRate = pstMetaDataStrmInfo->i32SampleRate;
                    flacDecParam->nBitsPerSample = pstMetaDataStrmInfo->i32BitsPerSample;
                    flacDecParam->nMinBlkSize = pstMetaDataStrmInfo->i32MinBlkSize;
                    flacDecParam->nMaxBlkSize = pstMetaDataStrmInfo->i32MaxBlkSize;
                    flacDecParam->nMinFrmSize = pstMetaDataStrmInfo->i32MinFrmSize;
                    flacDecParam->nMaxFrmSize = pstMetaDataStrmInfo->i32MaxFrmSize;
                }
            } else {
                ALOGE("get_parameter:QOMX_AUDIO_PARAM_FLAC_DEC_TYPE \
                                  OMX_ErrorBadPortIndex %d\n",
                                  (int)flacDecParam->nPortIndex);
                return OMX_ErrorBadPortIndex;
            }
            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioPcm:
        {
            ALOGV("SoftFlacDecoder::internalGetParameter(OMX_IndexParamAudioPcm)");
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                (OMX_AUDIO_PARAM_PCMMODETYPE *)params;
            stFLACDec* pstFLACDec = (stFLACDec*)(flacDecState.m_pFlacDecoder);
            stMetaDataStrmInfo* pstMetaDataStrmInfo =
                (stMetaDataStrmInfo*)(&pstFLACDec->MetaDataBlocks.MetaDataStrmInfo);

            if (1 == pcmParams->nPortIndex) {
                pcmParams->eNumData = OMX_NumericalDataSigned;
                pcmParams->eEndian = OMX_EndianBig;
                pcmParams->bInterleaved = OMX_TRUE;
                pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
                pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelLF;
                pcmParams->eChannelMapping[1] = OMX_AUDIO_ChannelRF;
                if (!mInitStatus) {
                    pcmParams->nChannels = parserInfoToPass.i32NumChannels;
                    pcmParams->nSamplingRate = parserInfoToPass.i32SampleRate;
                    pcmParams->nBitPerSample = parserInfoToPass.i32BitsPerSample;
                } else {
                    pcmParams->nChannels = pstMetaDataStrmInfo->i32NumChannels;
                    pcmParams->nSamplingRate = pstMetaDataStrmInfo->i32SampleRate;
                    pcmParams->nBitPerSample = pstMetaDataStrmInfo->i32BitsPerSample;
                }
            } else {
                ALOGE("get_parameter:OMX_IndexParamAudioPcm \
                                  OMX_ErrorBadPortIndex %u\n",
                                  (unsigned) pcmParams->nPortIndex);
                return OMX_ErrorBadPortIndex;
            }
            ALOGV("nChannels=%d, nSamplingRate=%d, nBitPerSample=%d",
                pcmParams->nChannels, pcmParams->nSamplingRate, pcmParams->nBitPerSample);

            return OMX_ErrorNone;
        }

        default:
            ALOGV("SoftFlacDecoder::internalGetParameter(default)");
            return SimpleSoftOMXComponent::internalGetParameter(index, params);
    }
}

OMX_ERRORTYPE SoftFlacDecoder::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
    switch ((int)index) {
        case OMX_IndexParamStandardComponentRole:
        {
            ALOGV("SoftFlacDecoder::internalSetParameter(OMX_IndexParamStandardComponentRole)");
            const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                (const OMX_PARAM_COMPONENTROLETYPE *)params;

            if (strncmp((const char *)roleParams->cRole,
                    "audio_decoder.flac",
                    OMX_MAX_STRINGNAME_SIZE - 1)) {
                ALOGE("unsupported role %s", (const char *)roleParams->cRole);
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioPortFormat:
        {
            const OMX_AUDIO_PARAM_PORTFORMATTYPE *formatParams =
                (const OMX_AUDIO_PARAM_PORTFORMATTYPE *)params;

            if (!isValidOMXParam(formatParams)) {
                return OMX_ErrorBadParameter;
            }

            if (formatParams->nPortIndex > 1) {
                return OMX_ErrorUndefined;
            }

            if ((formatParams->nPortIndex == 0
                        && formatParams->eEncoding != OMX_AUDIO_CodingFLAC)
                || (formatParams->nPortIndex == 1
                        && formatParams->eEncoding != OMX_AUDIO_CodingPCM)) {
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        case (OMX_INDEXTYPE)QOMX_IndexParamAudioFlacDec:
        {
            ALOGV("SoftFlacDecoder::internalSetParameter(QOMX_IndexParamAudioFlacDec)");
            QOMX_AUDIO_PARAM_FLAC_DEC_TYPE *flacDecParam =
                (QOMX_AUDIO_PARAM_FLAC_DEC_TYPE *) params;

            if (0 == flacDecParam->nPortIndex) {
                parserInfoToPass.i32NumChannels = flacDecParam->nChannels;
                parserInfoToPass.i32SampleRate = flacDecParam->nSampleRate;
                parserInfoToPass.i32BitsPerSample = flacDecParam->nBitsPerSample;
                parserInfoToPass.i32MinBlkSize = flacDecParam->nMinBlkSize;
                parserInfoToPass.i32MaxBlkSize = flacDecParam->nMaxBlkSize;
                parserInfoToPass.i32MinFrmSize = flacDecParam->nMinFrmSize;
                parserInfoToPass.i32MaxFrmSize = flacDecParam->nMaxFrmSize;

                ALOGD("CSD: nChannels=%d, nSampleRate=%d, nBitsPerSample=%d",
                    flacDecParam->nChannels, flacDecParam->nSampleRate, flacDecParam->nBitsPerSample);
                ALOGD("CSD: nMinBlkSize=%d, nMaxBlkSize=%d",
                    flacDecParam->nMinBlkSize, flacDecParam->nMaxBlkSize);
                ALOGD("CSD: nMinFrmSize=%d, nMaxFrmSize=%d",
                    flacDecParam->nMinFrmSize, flacDecParam->nMaxFrmSize);
            } else {
                ALOGE("set_parameter:QOMX_AUDIO_PARAM_FLAC_DEC_TYPE \
                                  OMX_ErrorBadPortIndex %d\n",
                                  (int)flacDecParam->nPortIndex);
                return OMX_ErrorBadPortIndex;
            }
            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioPcm:
        {
            ALOGV("SoftFlacDecoder::internalSetParameter(OMX_IndexParamAudioPcm)");
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                (OMX_AUDIO_PARAM_PCMMODETYPE *)params;

            if (1 == pcmParams->nPortIndex) {
                if (mInitStatus) {
                    stFLACDec* pstFLACDec = (stFLACDec*)(flacDecState.m_pFlacDecoder);
                    stMetaDataStrmInfo* pstMetaDataStrmInfo =
                        (stMetaDataStrmInfo*)(&pstFLACDec->MetaDataBlocks.MetaDataStrmInfo);

                    pstMetaDataStrmInfo->i32NumChannels = pcmParams->nChannels;
                    pstMetaDataStrmInfo->i32SampleRate = pcmParams->nSamplingRate;
                    pstMetaDataStrmInfo->i32BitsPerSample = pcmParams->nBitPerSample;
                } else {
                    parserInfoToPass.i32NumChannels = pcmParams->nChannels;
                    parserInfoToPass.i32SampleRate = pcmParams->nSamplingRate;
                    parserInfoToPass.i32BitsPerSample = pcmParams->nBitPerSample;
                }
            } else {
                ALOGE("set_parameter:OMX_IndexParamAudioPcm \
                                  OMX_ErrorBadPortIndex %u\n",
                                  (unsigned) pcmParams->nPortIndex);
                return OMX_ErrorBadPortIndex;
            }
            ALOGV("nChannels=%d, nSamplingRate=%d, nBitPerSample=%d",
                pcmParams->nChannels, pcmParams->nSamplingRate, pcmParams->nBitPerSample);

            return OMX_ErrorNone;
        }

        default:
            ALOGV("SoftFlacDecoder::internalSetParameter(default)");
            return SimpleSoftOMXComponent::internalSetParameter(index, params);
    }
}

inline bool SoftFlacDecoder::isBufferFull(uint32_t blockSize)
{
      return (((mDecodedBlocks + blockSize) * mNumChannels * mOutBytesPerSample)
               > kMaxOutputBufferSize ? true : false);
}

void SoftFlacDecoder::onQueueFilled(OMX_U32 portIndex) {
    ALOGV("SoftFlacDecoder::onQueueFilled(portIndex=%d)", portIndex);
    uint32 flacOutputBufSize = FLAC_OUTPUT_BUFFER_SIZE;
    uint32 blockSize = 0, usedBitstream = 0, availLength = 0;
    int status = 0;
    bool flagEOS = false;
    bool skipBuffering = false;

    if(!mInitStatus) {
       OMX_ERRORTYPE omx_err = init(&parserInfoToPass);
       if(omx_err != OMX_ErrorNone) {
          ALOGE("Failed to initialize Flac Decoder");
          return;
       }
    }

    stFLACDec* pstFLACDec = (stFLACDec*)(flacDecState.m_pFlacDecoder);
    stMetaDataStrmInfo* pstMetaDataStrmInfo =
    (stMetaDataStrmInfo*)(&pstFLACDec->MetaDataBlocks.MetaDataStrmInfo);

    List<BufferInfo *> &inQueue = getPortQueue(0);
    List<BufferInfo *> &outQueue = getPortQueue(1);

    if (mSignalledError) {
        return;
    }

    while (!inQueue.empty() && !outQueue.empty()) {
        // check if new input buffer from parser needs to be read
        if (mNotifyInputBufferEmpty) {
            inInfo = *inQueue.begin();
            inHeader = inInfo->mHeader;
            mNotifyInputBufferEmpty = false;

            availLength = inHeader->nFilledLen;

            if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
                eos = true;
            } else {
                mAnchorTimeUs = inHeader->nTimeStamp;
            }

            if (inHeader->pBuffer) {
                //availLength being passed is the size read from parser
                if (updateInputBitstream(&mBuffer,
                                         (uint8*)inHeader->pBuffer + inHeader->nOffset,
                                         availLength) < 0) {
                    notify(OMX_EventError, OMX_ErrorUndefined, -1, NULL);
                    mSignalledError = true;
                    return;
                }
            }
        }

        // check if new output buffer is needed
        if (mIsDecodeSuccess) {
            outInfo = *outQueue.begin();
            outHeader = outInfo->mHeader;
            mIsDecodeSuccess = false;
        }

        while (1) {
            if (enoughDataAvailable(&mBuffer) || eos) {
                ALOGV("SoftFlacDecoder: Decoder has enough data. Need not read from parser");
                availLength = 0;
                // EOS reported; see if there is any data left in the input buffer
                if ((mBuffer.i32WritePtr >= mBuffer.i32ReadPtr) && eos) {
                    ALOGV("Parser reported EOS");
                    mBuffer.eos = 1;
                    availLength = 0;
                }
                if ((((mBuffer.i32WritePtr - mBuffer.i32ReadPtr) <= 0) && mBuffer.eos) || flagEOS) {
                    // EOS reported and also all the data in input buffer is consumed
                    availLength = 0;
                    ALOGV("Report EOS as no more bitstream is left with the decoder");
                    outHeader->nFilledLen = 0;
                    outHeader->nFlags = OMX_BUFFERFLAG_EOS;

                    // release output buffer and notify FillBufferDone
                    outQueue.erase(outQueue.begin());
                    outInfo->mOwnedByUs = false;
                    notifyFillBufferDone(outHeader);

                    // release input buffer and notify EmptyBufferDone
                    inQueue.erase(inQueue.begin());
                    inInfo->mOwnedByUs = false;
                    notifyEmptyBufferDone(inHeader);

                    return;
                }
            } else if ((int32_t)(mBuffer.i32BufferSize - mBuffer.i32WritePtr) >= FLAC_PARSER_BUFFER_SIZE) {
                // Not enough data available in the input buffer, set flag to read more data
                ALOGV("set flag to read bitsream from parser");
                mNotifyInputBufferEmpty = true;
                break;
            } else {
                availLength = 0;
            }

            if (inHeader->pBuffer) {
                //availLength being passed is the size read from parser
                if (updateInputBitstream(&mBuffer,
                                         (uint8*)inHeader->pBuffer + inHeader->nOffset,
                                         availLength) < 0) {
                    notify(OMX_EventError, OMX_ErrorUndefined, -1, NULL);
                    mSignalledError = true;
                    return;
                }
            }

            availLength = mBuffer.i32WritePtr - mBuffer.i32ReadPtr;
            ALOGV("SoftFlacDecoder: Bytes left in internal buffer: %d", availLength);

            if (availLength) {
                ALOGV("Calling Decode Frame; writePtr=%d, readPtr=%d",
                        mBuffer.i32WritePtr, mBuffer.i32ReadPtr);

                usedBitstream = 0;
                flacOutputBufSize = FLAC_OUTPUT_BUFFER_SIZE;
                status = (*mProcessData)(&flacDecState,
                                         &mBuffer.ui8TempBuf[mBuffer.i32ReadPtr],
                                         availLength,
                                         mOutBuffer,
                                         &flacOutputBufSize,
                                         &usedBitstream,
                                         &blockSize);
                mNumChannels = pstMetaDataStrmInfo->i32NumChannels;
                mSamplingRate = pstMetaDataStrmInfo->i32SampleRate;

                ALOGV("Decode Frame done; status=%d, flacOutputBufSize=%d, usedBitstream=%d blockSize %d",
                        status, flacOutputBufSize, usedBitstream,blockSize);
            } else {
                break;
            }

            // Decoding is done; Update read and write pointers of the input buffer
            if (!updatePointers(&mBuffer, usedBitstream, status)) {
                if (mBuffer.eos && status != FLACDEC_SUCCESS) {
                    ALOGV("Error during decode after EOS is set. Report EOS");
                    flagEOS = true;
                } else {
                    // Some error or insufficient data - read again from parser
                    continue;
                }
            } else {
                   //Successful decode!
                   //check if we have enough space to accumulate if skipBuffering
                   // Two cases are possible here
                   // a) We are at the boundary hence need to write the earlier buffered data and then copy
                   //    back the new data
                   // b) Error case where in the blockSize from core decoder is corrupt
                   if ( isBufferFull(blockSize)){
                        ALOGI("Not enough space error scenario, mDecodedBlocks %" PRId64 " blockSize %d",
                                mDecodedBlocks, blockSize);
                        skipBuffering = true;
                        break;
                   }
                   //Interleave the output from decoder for multichannel clips.
                   if (flacDecState.m_bits_per_sample == 24) {
                       interleaveData<uint32_t>(blockSize);
                   } else {
                       interleaveData<uint16_t>(blockSize);
                   }
                   break;
            }
        }// End of while for decoding

        if (checkIfEnoughDecodedPCMSamples(mDecodedBlocks) || mBuffer.eos || skipBuffering) {
            if (flacDecState.m_bits_per_sample == 24) {
                //decoder gives output in 8_24 pcm format, convert it to 24 bit packed and copy
                convertFrom8_24ToPacked((uint8_t *) outHeader->pBuffer,
                                        (int32_t *) mTmpBuf, mDecodedBlocks * mNumChannels);
                outHeader->nFilledLen = mDecodedBlocks * mNumChannels * sizeof(uint8_t) * 3;
            } else {
                memcpy((uint16_t *)outHeader->pBuffer, mTmpBuf, mDecodedBlocks * mNumChannels * mOutBytesPerSample);
                outHeader->nFilledLen = mDecodedBlocks * mNumChannels * mOutBytesPerSample;
            }

            outHeader->nOffset = 0;
            outHeader->nTimeStamp = mAnchorTimeUs +
                (mNumFramesOutput * 1000000ll) / mSamplingRate;
            ALOGV("mAnchorTimeUs=%" PRId64 ", mNumFramesOutput=%" PRId64 ", output_timestamp=%lld",
                mAnchorTimeUs, mNumFramesOutput, outHeader->nTimeStamp);
            ALOGV("decoder output size = %d bytes", outHeader->nFilledLen);
            mNumFramesOutput += mDecodedBlocks;
            mDecodedBlocks = 0;
            // release output buffer and notify FillBufferDone
            outQueue.erase(outQueue.begin());
            outInfo->mOwnedByUs = false;
            notifyFillBufferDone(outHeader);
            mIsDecodeSuccess = true;
        }
        if (skipBuffering) {
            if (isBufferFull(blockSize)){
                ALOGW("This is error case, blocksize %d is not supported",blockSize);
            } else {
               // Copy the remaining data as it was skipped because buffer was full
               //Interleave the output from decoder for multichannel clips.
               if (flacDecState.m_bits_per_sample == 24) {
                   interleaveData<uint32_t>(blockSize);
               } else {
                   interleaveData<uint16_t>(blockSize);
               }
            }
            skipBuffering = false;
        }
        if (mNotifyInputBufferEmpty) {
            // release input buffer and notify EmptyBufferDone
            inQueue.erase(inQueue.begin());
            inInfo->mOwnedByUs = false;
            notifyEmptyBufferDone(inHeader);
        }
    }
}

void SoftFlacDecoder::convertFrom8_24ToPacked(uint8_t *dest, int32_t *src, uint32_t numSamples) {
    const int32_t maxPos = 0x7fffff;
    const int32_t minNeg = -0x800000;
    while (numSamples--) {
        int32_t val = *src++;
        if (val < minNeg) {
            val = minNeg;
        } else if (val > maxPos) {
            val = maxPos;
        }
        *dest++ = val;
        *dest++ = val >> 8;
        *dest++ = val >> 16;
    }
}

template<typename T>
void SoftFlacDecoder::interleaveData(uint32_t blockSize) {
    T *ptr = (T *) mOutBuffer;
    T *ptr1 = (T *) mTmpBuf + mDecodedBlocks * mNumChannels;

    mDecodedBlocks += blockSize;
    if (mNumChannels > 1) {
        for (uint32_t k = 0; k < blockSize; k++) {
            for (uint32_t i = k, j = mNumChannels * k;
                    i < blockSize * mNumChannels;
                    i += blockSize, j++) {
                ptr1[j] = ptr[i];
            }
        }
    } else {
        memcpy(ptr1, mOutBuffer, blockSize * mNumChannels * mOutBytesPerSample);
    }
}

}  // namespace android

android::SoftOMXComponent *createSoftOMXComponent(
        const char *name, const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData, OMX_COMPONENTTYPE **component) {
        pthread_mutex_lock(&lock_dec);
        if (number_of_adec_flac_instance + 1 > MAX_ADEC_FLAC_INSTANCE) {
            ALOGE("Exceeded max supported omx FLAC decoder concurrent instances");
            pthread_mutex_unlock(&lock_dec);
            return NULL;
        }
        number_of_adec_flac_instance++;
        pthread_mutex_unlock(&lock_dec);
        return new android::SoftFlacDecoder(name, callbacks, appData, component);
}


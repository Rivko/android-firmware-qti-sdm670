/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
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

#ifndef SOFT_FLAC_DEC_H_
#define SOFT_FLAC_DEC_H_

#include "SimpleSoftOMXComponent.h"
#include "FLACDec_Wrapper.h"
#include "FLACDec_BitStream.h"
#include "FLACDec_MetaData.h"
#include "FLACDec_Struct.h"
#include "QOMX_AudioExtensions.h"
#include "QOMX_AudioIndexExtensions.h"

#define FLAC_OUTPUT_BUFFER_SIZE     (8192*8)*4*8
#define FLAC_INPUT_BUFFER_SIZE      MAXINPBUFFER
#define FLAC_INSTANCE_SIZE 2048 + MAXINPBUFFER + 65536*8*4
#define SUPERSET_THRESHOLD 14000
#define THRESHOLD 8192*2
#define FLAC_PARSER_BUFFER_SIZE 8192*8
#define BUFFERING_SIZE (MAXINPBUFFER) > (FLAC_PARSER_BUFFER_SIZE*2) ? \
                        (MAXINPBUFFER) : (FLAC_PARSER_BUFFER_SIZE*2)
#define FLAC_OUTPUT_BUFFERING_THRESHOLD_IN_MSEC 20 // In miliseconds

namespace android {
struct SoftFlacDecoder : public SimpleSoftOMXComponent {
    SoftFlacDecoder(const char *name,
            const OMX_CALLBACKTYPE *callbacks,
            OMX_PTR appData,
            OMX_COMPONENTTYPE **component);
protected:
    virtual ~SoftFlacDecoder();

    virtual OMX_ERRORTYPE internalGetParameter(
            OMX_INDEXTYPE index, OMX_PTR params);

    virtual OMX_ERRORTYPE internalSetParameter(
            OMX_INDEXTYPE index, const OMX_PTR params);

    virtual void onQueueFilled(OMX_U32 portIndex);

    virtual void onPortFlushCompleted(OMX_U32 portIndex);
    virtual void onReset();
private:
    enum {
        kNumInputBuffers = 4,
        kNumOutputBuffers = 4,
        kMaxInputBufferSize = FLAC_INPUT_BUFFER_SIZE,
        kMaxOutputBufferSize = FLAC_OUTPUT_BUFFER_SIZE,
    };
    typedef struct {
        uint32_t i32BufferSize;
        uint32_t i32ReadPtr;
        uint32_t i32WritePtr;
        uint8_t *ui8TempBuf;
        int32_t eos;
        int32_t error;
    } flacByteStreamBuffer;
    flacByteStreamBuffer mBuffer;

    BufferInfo *inInfo, *outInfo;
    OMX_BUFFERHEADERTYPE *inHeader, *outHeader;
    bool mIsDecodeSuccess;
    bool eos;
    bool mNotifyInputBufferEmpty;
    bool mInitStatus;
    int32_t mNumChannels;
    int32_t mSamplingRate;
    int32_t mOutBytesPerSample;
    int64_t mAnchorTimeUs;
    int64_t mNumFramesOutput;
    int64_t mDecodedBlocks;

    CFlacDecState flacDecState;
    FLACDec_ParserInfo parserInfoToPass;

    void *mLibHandle;
    void *mOutBuffer;
    void *mTmpBuf;

    typedef void* (*DecoderInit) (CFlacDecState* pFlacDecState, int* nRes, int bitWidth);

    typedef int (*DecoderLib_Process) (CFlacDecState* pFlacDecState, uint8* pInBitStream,
                                        uint32 nActualDataLen, void *pOutSamples,
                                        uint32* uFlacOutputBufSize, uint32* usedBitstream,
                                        uint32* blockSize);

    typedef void (*DecoderEnd) (void* pFlacDec );

    DecoderInit mDecoderInit;
    DecoderLib_Process mProcessData;
    DecoderEnd mDecoderEnd;

    bool mSignalledError;

    void initPorts();
    OMX_ERRORTYPE init(FLACDec_ParserInfo* parserInfoToPass);

    int32_t enoughDataAvailable(flacByteStreamBuffer *pByteBuffer);

    int32_t updateInputBitstream(flacByteStreamBuffer *pByteBuffer, void * bitstream, int32_t inSize);

    int32_t updatePointers(flacByteStreamBuffer *pByteBuffer, uint32_t readBytes, int32_t result);

    bool checkIfEnoughDecodedPCMSamples(int64_t decodedPcmBlocks);

    inline bool isBufferFull(uint32_t blockSize);

    void convertFrom8_24ToPacked(uint8_t *dest, int32_t *src, uint32_t numSamples);

    template<typename T> void interleaveData(uint32_t blockSize);

    DISALLOW_EVIL_CONSTRUCTORS(SoftFlacDecoder);
};

}  // namespace android

#endif  // SOFT_FLAC_DEC_H_


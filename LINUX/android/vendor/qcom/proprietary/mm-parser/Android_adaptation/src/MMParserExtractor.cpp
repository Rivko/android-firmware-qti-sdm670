/*
 * Copyright (c) 2010 - 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2010 The Android Open Source Project
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
#define LOG_TAG "MMParserExtractor"
#include <utils/Log.h>
#include <vector>
#include <VideoAPI.h>
#include <cstring>

//! Number of sync samples required for thumbnail generation
#define NUM_SYNC_FRAMES (5)
//#define DUMP_TO_FILE
/* Max buffer size in byte stream mode */
#define MAX_PARSER_IN_BUF_SIZE (256*1024)
//! Flag to indicate whether local playback or streaming use case.
//! This value is used to check whether to use infinite or timed wait for the
//! response from FileSource thread.
static bool gbLocalPlayback = false;

#include <media/stagefright/MediaExtractor.h>
#include "MMParserExtractor.h"
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaBufferGroup.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/foundation/ABitReader.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ColorUtils.h>
#include <QCMediaDefs.h>
#include <QCMetaData.h>
#include <AVMetaData.h>
#include <media/stagefright/MediaErrors.h>
#include <byteswap.h>
#include "include/avc_utils.h"

#include <cutils/properties.h>
#include <media/stagefright/Utils.h>
#include <utils/Unicode.h>
#include "parserdatadef.h"
#include "MMDebugMsg.h"

namespace android {

#define BITSTREAM_LOG_LOC "/data/misc/media"

typedef enum {
    AOT_AAC_LC           = 2,   /**< Low Complexity object                     */
    AOT_SBR              = 5,
    AOT_ER_AAC_LC        = 17,   /**< Error Resilient(ER) AAC Low Complexity    */
    AOT_ER_AAC_SCAL      = 20,   /**< Error Resilient(ER) AAC Scalable object   */
    AOT_ER_BSAC          = 22,   /**< Error Resilient(ER) BSAC object           */
    AOT_ER_AAC_LD        = 23,   /**< Error Resilient(ER) AAC LowDelay object   */
    AOT_PS               = 29,   /**< PS, Parametric Stereo (includes SBR)      */
    AOT_ESCAPE           = 31,   /**< Signal AOT uses more than 5 bits          */
} AUDIO_OBJECT_TYPE;

FileSourceWrapper* FileSourceWrapper::New(iStreamPort& aStreamPort,
                                          FileSourceFileFormat eFileFormat) {
    FileSourceWrapper* self = new FileSourceWrapper;
    if(!self) {
        return NULL;
    }

    status_t err = self->Init(aStreamPort, eFileFormat);
    if(err!=OK){
        LOGV(" FileSourceWrapper::New Init returned %d \n", err);
        delete self;
        return NULL;
    }

    return self;
}

status_t FileSourceWrapper::Init(iStreamPort& aStreamPort,
                                 FileSourceFileFormat eFileFormat) {
    uint32 ulCount = 0;
    FileSourceStatus status = FILE_SOURCE_FAIL;
    //! Create synchronous FileSource
    m_pFileSource = new FileSource(FileSourceWrapper::cbFileSourceStatus,
                                   (void*)this, false);
    if(!m_pFileSource) {
        return ERROR_IO;
    }

    do {
        status = m_pFileSource->OpenFile(&aStreamPort, eFileFormat, TRUE);
        if(FILE_SOURCE_SUCCESS != status) {
            LOGE(" m_pFileSource->OpenFile error %d\n", status);
            return ERROR_IO;
        }
        mWaitSeamaphore.Wait();
        if(FILE_SOURCE_DATA_NOTAVAILABLE == mStatus) {
            LOGE(" m_pFileSource->OpenFile returned data UnderRun\n");
            usleep(100000); //! Sleep for 100ms
            ulCount++;
        }
        if(ulCount > 5) {
            LOGE(" m_pFileSource->OpenFile returned data UnderRun for 5 times\n");
            break;
        }
    } while(FILE_SOURCE_DATA_NOTAVAILABLE == mStatus);

    if(FILE_SOURCE_SUCCESS !=mStatus ) {
        LOGV(" m_pFileSource->OpenFile async error %d\n", mStatus);
        return ERROR_IO;
    }

    FileSourceFileFormat fileformat = FILE_SOURCE_UNKNOWN;
    status = m_pFileSource->GetFileFormat(fileformat);
    if(FILE_SOURCE_SUCCESS != status ) {
        LOGE(" m_pFileSource->GetFileFormat error %d\n", status);
        return ERROR_IO;
    }

    iDrmEncrypted = m_pFileSource->IsDRMProtection();

    if(iDrmEncrypted && fileformat != FILE_SOURCE_AVI) {
       LOGE("Encrypted clip but in unsupported container");
       return UNKNOWN_ERROR;
    }

    return OK;
}

FileSourceWrapper::FileSourceWrapper()
:   m_pFileSource(NULL),
    iDrmEncrypted(false),
    mStatus(FILE_SOURCE_FAIL),
    mFileFormat(FILE_SOURCE_UNKNOWN),
    mVideoMinorType(FILE_SOURCE_MN_TYPE_UNKNOWN){
}

FileSourceWrapper::~FileSourceWrapper() {
    LOGV(" FileSourceWrapper::~FileSourceWrapper\n");
    m_pFileSource->CloseFile();
    delete m_pFileSource;
}

uint32 FileSourceWrapper::GetWholeTracksIDList(FileSourceTrackIdInfoType *trackIdInfo ) {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->GetWholeTracksIDList(trackIdInfo);
}

FileSourceStatus FileSourceWrapper::GetMimeType(uint32 id, FileSourceMjMediaType& majorType, FileSourceMnMediaType& minorType) {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->GetMimeType(id, majorType, minorType);
}

FileSourceStatus FileSourceWrapper::GetMediaTrackInfo(uint32 id,MediaTrackInfo* info) {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->GetMediaTrackInfo(id, info);
}

uint64 FileSourceWrapper::GetTrackMediaDuration(uint32 id) {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->GetTrackMediaDuration(id);
}

uint64 FileSourceWrapper::GetClipDuration() {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->GetClipDuration();
}

FileSourceStatus FileSourceWrapper::GetFormatBlock(uint32 id, uint8* buf,
                                                   uint32 *pbufSize, bool bRawCodec) {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->GetFormatBlock(id, buf, pbufSize, bRawCodec);
}

FileSourceStatus FileSourceWrapper::GetFileFormat(FileSourceFileFormat& fileFormat)
{
   FileSourceStatus status = FILE_SOURCE_FAIL;
   fileFormat = FILE_SOURCE_UNKNOWN;
   if(m_pFileSource)
   {
     status = m_pFileSource->GetFileFormat(fileFormat);
   }
   return status;
}

bool FileSourceWrapper::GetWMACodecData(uint32 id,WmaCodecData* pCodecData) {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->GetWMACodecData(id, pCodecData);
}

uint8 FileSourceWrapper::IsSeekDenied() {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->IsSeekDenied();
}

FileSourceStatus FileSourceWrapper::SeekAbsolutePosition( int64 trackid ,
                                         const int tAbsoluteTime,
                                         bool bSeekToSync,
                                         int64 nCurrPlayTime,
                                         FS_SEEK_MODE eSeekMode) {
    Mutex::Autolock autoLock(mMutex);
    FileSourceStatus err = m_pFileSource->SeekAbsolutePosition(trackid, tAbsoluteTime,
                            bSeekToSync, nCurrPlayTime,eSeekMode);
    if(err!=FILE_SOURCE_SUCCESS) {
        return err;
    }
    mWaitSeamaphore.Wait();
    return mStatus;
}

FileSourceStatus FileSourceWrapper::SeekAbsolutePosition(const int tAbsoluteTime,
                                         bool bSeekToSync,
                                         int64 nCurrPlayTime,
                                         FS_SEEK_MODE eSeekMode) {
    Mutex::Autolock autoLock(mMutex);
    FileSourceStatus err = m_pFileSource->SeekAbsolutePosition(tAbsoluteTime,
                            bSeekToSync, nCurrPlayTime,eSeekMode);
    if(err!=FILE_SOURCE_SUCCESS) {
        return err;
    }
    mWaitSeamaphore.Wait();
    return mStatus;
}

FileSourceStatus FileSourceWrapper::SeekRelativeSyncPoint( int currentPlaybacktime, const int numSync) {
    Mutex::Autolock autoLock(mMutex);
    FileSourceStatus err = m_pFileSource->SeekRelativeSyncPoint(currentPlaybacktime, numSync);
    if(err!=FILE_SOURCE_SUCCESS) {
        return err;
    }
    mWaitSeamaphore.Wait();
    return mStatus;
}

int32 FileSourceWrapper::GetTrackMaxFrameBufferSize(uint32 id) {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->GetTrackMaxFrameBufferSize(id);
}

FileSourceMediaStatus FileSourceWrapper::GetNextMediaSample(uint32 id,
                    uint8 *buf, uint32 *size, FileSourceSampleInfo& pSampleInfo) {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->GetNextMediaSample(id, buf, size, pSampleInfo);
}

FileSourceStatus FileSourceWrapper::GetClipMetaData(wchar_t* pMetaData,
                    uint32* size, FileSourceMetaDataType pSampleInfo,
                    FS_TEXT_ENCODING_TYPE *pEncode) {
    Mutex::Autolock autoLock(mMutex);
    return m_pFileSource->GetClipMetaData(pMetaData, size, pSampleInfo, pEncode);
}

void FileSourceWrapper::cbFileSourceStatus(FileSourceCallBackStatus status, void* pCbData)
{
    LOGV("FileSourceWrapper::cbFileSourceStatus , pCbData = 0x%p, status = %d ===>\n", pCbData, status);
    FileSourceWrapper* pThis = reinterpret_cast<FileSourceWrapper*>(pCbData);

    //File Source event
    switch (status)
    {
    case FILE_SOURCE_OPEN_COMPLETE:
        LOGV("FileSourceWrapper::cbFileSourceStatus FILE_SOURCE_OPEN_COMPLETE\n");
        pThis->mStatus = FILE_SOURCE_SUCCESS;
        break;
    case FILE_SOURCE_OPEN_FAIL:
        LOGV("FileSourceWrapper::cbFileSourceStatus FILE_SOURCE_OPEN_FAIL\n");
        pThis->mStatus = FILE_SOURCE_FAIL;
        break;
    case FILE_SOURCE_OPEN_DATA_UNDERRUN:
        LOGV("FileSourceWrapper::cbFileSourceStatus FILE_SOURCE_OPEN_DATA_UNDERRUN\n");
        pThis->mStatus = FILE_SOURCE_DATA_NOTAVAILABLE;
        break;
    case FILE_SOURCE_SEEK_COMPLETE:
        LOGV("FileSourceWrapper::cbFileSourceStatus FILE_SOURCE_SEEK_COMPLETE\n");
        pThis->mStatus = FILE_SOURCE_SUCCESS;
        break;
    case FILE_SOURCE_SEEK_FAIL:
        LOGV("FileSourceWrapper::cbFileSourceStatus FILE_SOURCE_SEEK_FAIL\n");
        pThis->mStatus = FILE_SOURCE_FAIL;
        break;
    default:
        LOGV("FileSourceWrapper::cbFileSourceStatus **NOT SUPPORTED CALLBACK**");
        pThis->mStatus = FILE_SOURCE_FAIL;
        break;
    }
    /* Post the event to the filter thread */
    pThis->mWaitSeamaphore.Signal();
}

bool FileSourceWrapper::IsDrmProtection() {
    Mutex::Autolock autoLock(mMutex);
    return iDrmEncrypted;
}

status_t FileSourceWrapper::SetConfiguration(uint32 id,FileSourceConfigItem* pItem, FileSourceConfigItemEnum ienumData)
{
   return m_pFileSource->SetConfiguration(id,pItem,ienumData);
}

status_t FileSourceWrapper::GetConfiguration(uint32 id,FileSourceConfigItem* pItem, FileSourceConfigItemEnum ienumData)
{
   return m_pFileSource->GetConfiguration(id,pItem,ienumData);
}

bool FileSourceWrapper::GetWavCodecData(uint32 id,WavFormatData* pCodecData)
{
   return m_pFileSource->GetWavCodecData(id, pCodecData);
}

status_t FileSourceWrapper::GetStreamParameter(uint32 id, uint32 paramIndex, void* ptr)
{
   return m_pFileSource->GetStreamParameter(id, paramIndex, ptr);
}

status_t FileSourceWrapper::SetStreamParameter(uint32 id, uint32 paramIndex, void* ptr)
{
   return m_pFileSource->SetStreamParameter(id, paramIndex, ptr);
}

bool FileSourceWrapper::GetAACCodecData(uint32 id,AacCodecData* pCodecData)
{
   return m_pFileSource->GetAACCodecData(id,pCodecData);
}

////////////////////////////////////////////////////////////////////////////////////

Semaphore::Semaphore()
:   mCount(1) {}

void Semaphore::Signal() {
    mMutex.lock();
    mCount++;
    if(mCount<=1) { //Implies someone was waiting.
        mCondition.signal();
    }
    mMutex.unlock();
}

void Semaphore::Wait() {
//! Maximum time that Parser can wait for response from FileSource Thread
//! This value is kept as 15sec. This needs to be provided in nanosec units.
//! This timed wait is used only in case of local playback mode.
//! In streaming playback, infinite loop will be used to handle low bandwidth
//! use cases.
    mMutex.lock();
    mCount--;
    if(mCount<=0) {
        if(true == gbLocalPlayback) {
            const nsecs_t sllWaitTime = 15000000000LL;
            mCondition.waitRelative(mMutex, sllWaitTime);
        }
        else
            mCondition.wait(mMutex);
    }
    mMutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////
bool FileSourceWrapper::IsDivxSupportEnabled(
          FileSourceFileFormat  eFileFormat,
          FileSourceMnMediaType eCodecType) {
    /* By Default this flag will be set to true.
       If file format is MKV and codec type is H264/HEVC, check whether
       content is generated by DivxHD+ tool or not. Due to licensing
       issues, all the customers may not play content generated by
       DivxHD+ tool.
       Similarly for Divx codecs also, check whether feature is enabled or not.
       "mm.enable.qcom_parser" property is extended to enable/disable these
       features. */
    bool bIsDivxEnabled = true;
    char ucDivxPropertyValue[PROPERTY_VALUE_MAX] = {0};
    int slDivxFlags = 0;
    //! If this flag is not defined, then default value "0" will be used
    //! "0" means DivxHD Plus support is not enabled by default
    property_get("mm.enable.qcom_parser", ucDivxPropertyValue, "0");
    slDivxFlags = atoi(ucDivxPropertyValue);
    /*
       1) Check whether Divx property is enabled or not.
          This is required if codec type is Divx.
       2) Check whether DivxHD property is enabled or not.
          This is requierd if codec type is H264/HEVC and fileformat is
          MKV.
    */
    if((FILE_SOURCE_MN_TYPE_DIVX311   == eCodecType) ||
       (FILE_SOURCE_MN_TYPE_DIVX40    == eCodecType) ||
       (FILE_SOURCE_MN_TYPE_DIVX50_60 == eCodecType)) {
        //! Check whether Divx flag is enabled or not
        if (!(PARSER_DIVX & slDivxFlags)) {
            bIsDivxEnabled = false;
        }
    }
    else if(FILE_SOURCE_MKV == eFileFormat) {
        bool bisDivxHD = true;
        //!Check whether DivxHD flag is enabled or not
        if (!(PARSER_DIVXHD & slDivxFlags)) {
            bisDivxHD = false;
        }
        //! If DivxHD flag is not enabled, then check whether content is
        //! generated by DivxHD+ tool or not.
        if((false == bisDivxHD) &&
           (FILE_SOURCE_MN_TYPE_H264 == eCodecType ||
            FILE_SOURCE_MN_TYPE_HEVC == eCodecType)) {
            bool bRet = IsContentGeneratedbyDivxHDTool();
            //!Disable Divx support if content is generated by DivxHD tool
            bIsDivxEnabled = !bRet;
        }
    }
    LOGV("Divx enabled flag %d", bIsDivxEnabled);
    return bIsDivxEnabled;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSourceWrapper::IsContentGeneratedbyDivxHDTool() {
    /* This function will return true, if content is generated by
       DivxHD tool, otherwise returns false. */
    bool bIsDivxHDEnabled = false;

    char*  pStr = NULL;
    uint8  ucMetaDataStr[128];
    uint32 ulMetaDtaSize = 128;
    bool   bIsDivxPlus   = false;
    FileSourceStatus       eStatus   = FILE_SOURCE_FAIL;
    FileSourceMetaDataType eMetaData = FILE_SOURCE_MD_MUXING_APP;
    pStr = (char*)ucMetaDataStr;
    eStatus = GetClipMetaData((wchar_t*)pStr, &ulMetaDtaSize, eMetaData);
    if (!strncmp(pStr, DIVX_MUXING_APP, strlen(DIVX_MUXING_APP))) {
        bIsDivxPlus = true;
    }
    if(!bIsDivxPlus) {
        pStr[0]   = '\0';
        eMetaData = FILE_SOURCE_MD_WRITING_APP;
        eStatus   = GetClipMetaData((wchar_t*)pStr, &ulMetaDtaSize,
                                    eMetaData);
        if (!strncmp(pStr, DIVX_WRITING_APP, strlen(DIVX_WRITING_APP))) {
            bIsDivxPlus = true;
        }
    }
    if(!bIsDivxPlus) {
        pStr[0]   = '\0';
        eMetaData = FILE_SOURCE_MD_CODEC_NAME;
        eStatus   = GetClipMetaData((wchar_t*)pStr, &ulMetaDtaSize,
                                    eMetaData);
        if (!strncmp(pStr, DIVX_PLUS, strlen(DIVX_PLUS))) {
            bIsDivxPlus = true;
        }
    }
    if (bIsDivxPlus) {
      LOGV("Content generated with DivxHD+ tool");
      bIsDivxHDEnabled = true;
    }
    return bIsDivxHDEnabled;
}

////////////////////////////////////////////////////////////////////////////////////
const char *MediaType2MIME(FileSourceMnMediaType minorType){
    LOGV("static function MediaType2MIME");
    switch (minorType) {
        case FILE_SOURCE_MN_TYPE_MP3:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_MP3 \n");
            return MEDIA_MIMETYPE_AUDIO_MPEG;
        case FILE_SOURCE_MN_TYPE_AC3:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_AC3 \n");
            return MEDIA_MIMETYPE_AUDIO_AC3;
        case FILE_SOURCE_MN_TYPE_EAC3:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_EAC3 \n");
            return MEDIA_MIMETYPE_AUDIO_EAC3;
        case FILE_SOURCE_MN_TYPE_EAC3_JOC:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_EAC3_JOC \n");
            return "audio/eac3-joc";
        case FILE_SOURCE_MN_TYPE_AAC:
        case FILE_SOURCE_MN_TYPE_AAC_ADTS:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_AAC minor type %d\n", minorType);
            return MEDIA_MIMETYPE_AUDIO_AAC;
        case FILE_SOURCE_MN_TYPE_AAC_ADIF:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_AAC_ADIF minor type is not supported ");
            return NULL;
        case FILE_SOURCE_MN_TYPE_AAC_LOAS:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_AAC_LOAS minor type is not supported ");
            return NULL;
        case FILE_SOURCE_MN_TYPE_QCELP:
        case FILE_SOURCE_MN_TYPE_QCP:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_QCELP \n");
            return MEDIA_MIMETYPE_AUDIO_QCELP;
        case FILE_SOURCE_MN_TYPE_EVRC:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_EVRC \n");
            return MEDIA_MIMETYPE_AUDIO_EVRC;
        case FILE_SOURCE_MN_TYPE_WMA:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_WMA \n");
            return MEDIA_MIMETYPE_AUDIO_WMA;
        case FILE_SOURCE_MN_TYPE_WMA_PRO:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_WMA_PRO \n");
            return MEDIA_MIMETYPE_AUDIO_WMA;
        case FILE_SOURCE_MN_TYPE_WMA_LOSSLESS:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_WMA_LOSSLESS \n");
            return MEDIA_MIMETYPE_AUDIO_WMA;
        case FILE_SOURCE_MN_TYPE_GSM_AMR:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_GSM_AMR \n");
            return MEDIA_MIMETYPE_AUDIO_AMR_NB;
        case FILE_SOURCE_MN_TYPE_AMR_WB:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_AMR_WB \n");
            return MEDIA_MIMETYPE_AUDIO_AMR_WB;
        case FILE_SOURCE_MN_TYPE_AMR_WB_PLUS:
            LOGV(" MediaType2MIME MEDIA_MIMETYPE_AUDIO_AMR_WB_PLUS \n");
            return MEDIA_MIMETYPE_AUDIO_AMR_WB_PLUS;
        case FILE_SOURCE_MN_TYPE_DTS:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_DTS \n");
            return MEDIA_MIMETYPE_AUDIO_DTS;
        case FILE_SOURCE_MN_TYPE_DSD:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_DSD \n");
            return MEDIA_MIMETYPE_AUDIO_DSD;
        case FILE_SOURCE_MN_TYPE_AIFF_PCM:
        case FILE_SOURCE_MN_TYPE_PCM:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_PCM \n");
            return MEDIA_MIMETYPE_AUDIO_RAW;
        case FILE_SOURCE_MN_TYPE_G711_ALAW:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_G711_ALAW \n");
            return MEDIA_MIMETYPE_AUDIO_G711_ALAW;
        case FILE_SOURCE_MN_TYPE_G711_MULAW:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_G711_MULAW \n");
            return MEDIA_MIMETYPE_AUDIO_G711_MLAW;
        case FILE_SOURCE_MN_TYPE_GSM_FR:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_GSM_FR \n");
            return MEDIA_MIMETYPE_AUDIO_MSGSM;
        case FILE_SOURCE_MN_TYPE_ALAC:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_ALAC \n");
            return MEDIA_MIMETYPE_AUDIO_ALAC;
        case FILE_SOURCE_MN_TYPE_APE:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_APE \n");
            return MEDIA_MIMETYPE_AUDIO_APE;
        case FILE_SOURCE_MN_TYPE_SOWT:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_SOWT \n");
            return MEDIA_MIMETYPE_AUDIO_RAW;
        case FILE_SOURCE_MN_TYPE_FLAC:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_FLAC \n");
            return MEDIA_MIMETYPE_AUDIO_FLAC;
        case FILE_SOURCE_MN_TYPE_OPUS:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_OPUS \n");
            return MEDIA_MIMETYPE_AUDIO_OPUS;
        case FILE_SOURCE_MN_TYPE_VORBIS:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_VORBIS \n");
            return MEDIA_MIMETYPE_AUDIO_VORBIS;

        case FILE_SOURCE_MN_TYPE_DIVX311:
             LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_DIVX311\n");
             return MEDIA_MIMETYPE_VIDEO_DIVX311;
        case FILE_SOURCE_MN_TYPE_DIVX40:
             LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_DIVX40\n");
             return MEDIA_MIMETYPE_VIDEO_DIVX4;
        case FILE_SOURCE_MN_TYPE_DIVX50_60:
             LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_DIVX50_60\n");
             return MEDIA_MIMETYPE_VIDEO_DIVX;
        case FILE_SOURCE_MN_TYPE_WMV1:
        case FILE_SOURCE_MN_TYPE_WMV2:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_WMv 1/2 not supported \n");
            return NULL;
        case FILE_SOURCE_MN_TYPE_WMV3:
        case FILE_SOURCE_MN_TYPE_VC1:
            LOGV(" MediaType2MIME  FILE_SOURCE_MN_TYPE_WMV* \n");
            return MEDIA_MIMETYPE_VIDEO_WMV;
        case FILE_SOURCE_MN_TYPE_MPEG4:
        case FILE_SOURCE_MN_TYPE_STILL_IMAGE:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_MPEG4 \n");
            return MEDIA_MIMETYPE_VIDEO_MPEG4;
        case FILE_SOURCE_MN_TYPE_H263:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_H263\n");
            return MEDIA_MIMETYPE_VIDEO_H263;
        case FILE_SOURCE_MN_TYPE_H264:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_H264\n");
            return MEDIA_MIMETYPE_VIDEO_AVC;
       case FILE_SOURCE_MN_TYPE_VP8F:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_VP8F\n");
            return MEDIA_MIMETYPE_VIDEO_VPX;
       case FILE_SOURCE_MN_TYPE_VP9:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_VP9\n");
            return MEDIA_MIMETYPE_VIDEO_VP9;
        case FILE_SOURCE_MN_TYPE_HEVC:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_HEVC\n");
            return MEDIA_MIMETYPE_VIDEO_HEVC;
        case FILE_SOURCE_MN_TYPE_MPEG2:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_MPEG2 \n");
            return MEDIA_MIMETYPE_VIDEO_MPEG2;
        case FILE_SOURCE_MN_TYPE_MPEG1:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_MPEG1 \n");
            // returning mpeg2 as frameworks does not has support
            // for mpeg1, this enables playback of mpeg1 videos
            return MEDIA_MIMETYPE_VIDEO_MPEG2;

        case FILE_SOURCE_MN_TYPE_TIMED_TEXT:
            LOGV(" MediaType2MIME FILE_SOURCE_MN_TYPE_TIMED_TEXT \n");
            return MEDIA_MIMETYPE_TEXT_3GPP;
        case FILE_SOURCE_MN_TYPE_META_TEXT:
            LOGV("MediaType2MIME FILE_SOURCE_MN_TYPE_META_TEXT");
            return "text/plain";
        default:
            LOGE("MediaType2MIME  minor type not supported, returns NULL string, caller should handle minor type %d", minorType);
            return NULL;
    }
}

const char *fillMimeType(FileSourceFileFormat fileFormat, bool isVideoTrackPresent)
{
    switch(fileFormat) {
        case FILE_SOURCE_AC3:
            return "audio/ac3";

        case FILE_SOURCE_AAC:
            return "audio/aac";

        case FILE_SOURCE_AMR_NB:
            return "audio/amr";

        case FILE_SOURCE_AMR_WB:
            return "audio/amr-wb";

       case FILE_SOURCE_AIFF:
           return MEDIA_MIMETYPE_AUDIO_AIFF;

       case FILE_SOURCE_APE:
           return MEDIA_MIMETYPE_AUDIO_APE;

       case FILE_SOURCE_DSF:
       case FILE_SOURCE_DSDIFF:
           return MEDIA_MIMETYPE_AUDIO_DSD;

        case FILE_SOURCE_WAV:
            return "audio/x-wav";

        case FILE_SOURCE_FLV:
            return "video/flv";

        case FILE_SOURCE_ASF:
            if(isVideoTrackPresent) {
                return MEDIA_MIMETYPE_VIDEO_WMV;
            } else {
                return MEDIA_MIMETYPE_AUDIO_WMA;
            }

        case FILE_SOURCE_FLAC:
           return MEDIA_MIMETYPE_AUDIO_FLAC;

        case FILE_SOURCE_OGG:
           return MEDIA_MIMETYPE_CONTAINER_OGG;

        case FILE_SOURCE_AVI:
            return "video/avi";

        case FILE_SOURCE_EVRCB:
        case FILE_SOURCE_EVRC_WB:
            return "audio/evrc";

        case FILE_SOURCE_QCP:
            return "audio/qcelp";
        case FILE_SOURCE_WEBM:
            return "video/webm";

        case FILE_SOURCE_MKV:
            return MEDIA_MIMETYPE_CONTAINER_MATROSKA;

        case FILE_SOURCE_3G2:
        case FILE_SOURCE_MPEG4:
            if(isVideoTrackPresent) {
                return "video/mp4";
            }else {
                return "audio/mp4";
            }

        case FILE_SOURCE_MP3:
            return "audio/mpeg";

        case FILE_SOURCE_DTS:
            return "audio/dts";

        case FILE_SOURCE_MP2TS:
            return MEDIA_MIMETYPE_CONTAINER_MPEG2TS;

        case FILE_SOURCE_MP2PS:
            return MEDIA_MIMETYPE_CONTAINER_MPEG2PS;

        default:
            LOGW("fillMimeType:: NULL sending");
            return NULL;

    }
}

FileSourceFileFormat MapMediaMimeType2FileFormat(const char *MimeType)
{
    FileSourceFileFormat eFileFormatType = FILE_SOURCE_UNKNOWN;

    if (!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_AVI)){
        eFileFormatType = FILE_SOURCE_AVI;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_OGG)) {
        eFileFormatType = FILE_SOURCE_OGG;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_ASF)){
        eFileFormatType = FILE_SOURCE_ASF;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_3G2) ||
            !strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_MPEG4) ) {
        eFileFormatType = FILE_SOURCE_3G2;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_AAC) ||
            !strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_AAC_ADTS)) {
        eFileFormatType = FILE_SOURCE_AAC;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_AC3) ||
            !strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_EAC3)) {
        eFileFormatType = FILE_SOURCE_AC3;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_QCP)) {
        eFileFormatType = FILE_SOURCE_QCP;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_DTS) ||
            !strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_DTS_LBR)) {
        eFileFormatType = FILE_SOURCE_DTS;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_WAV)) {
        eFileFormatType = FILE_SOURCE_WAV;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_AIFF)) {
        eFileFormatType = FILE_SOURCE_AIFF;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_APE)) {
        eFileFormatType = FILE_SOURCE_APE;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_DSF)) {
        eFileFormatType = FILE_SOURCE_DSF;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_DFF)) {
        eFileFormatType = FILE_SOURCE_DSDIFF;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_QCFLV)) {
        eFileFormatType = FILE_SOURCE_FLV;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_AMR_WB)) {
        eFileFormatType = FILE_SOURCE_AMR_WB;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_AMR_NB)) {
        eFileFormatType = FILE_SOURCE_AMR_NB;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_MPEG)) {
        eFileFormatType = FILE_SOURCE_MP3;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_AUDIO_FLAC)) {
        eFileFormatType = FILE_SOURCE_FLAC;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_MATROSKA)) {
        eFileFormatType = FILE_SOURCE_MKV;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_MPEG2TS)) {
        eFileFormatType = FILE_SOURCE_MP2TS;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_MPEG2PS)) {
        eFileFormatType = FILE_SOURCE_MP2PS;
    }
    else if(!strcasecmp(MimeType, MEDIA_MIMETYPE_CONTAINER_MOV)) {
        eFileFormatType = FILE_SOURCE_MOV;
    }

    return eFileFormatType;
}

MMParserExtractor::MMParserExtractor(const sp<DataSource> &source,
                                     const char* MimeType)
    : mDataSource(source),
      mFileMetaData(new MetaData),
      mHaveMetadata(NO_INIT),
      mUseBigBuffers(false),
      mTracks(0),
      mIsAacformatAdif(false),
      mVideoTrackPresent(false),
      mFirstTrack(NULL),
      mLastTrack(NULL),
      m_pFileSourceWrapper(NULL),
      m_pSourcePort(NULL),
      m_eFileFormat(FILE_SOURCE_UNKNOWN){
    LOGV("MMParserExtractor::MMParserExtractor ====> \n");
    m_pSourcePort = new SourcePort(source);
    if(MimeType) {
        m_eFileFormat = MapMediaMimeType2FileFormat(MimeType);
    }
    video::iStreamPort::DataSourceType eSourceType;
    m_pSourcePort->GetSourceType(&eSourceType);
    if(video::iStreamPort::DS_STREAMING_SOURCE != eSourceType)
        gbLocalPlayback = true;
    else
        gbLocalPlayback = false;

    mSetBuffers = false;
    mFormatBlock = new ABuffer(1024);
    mFormatBlock->setRange(0, 0);

    char property_value[PROPERTY_VALUE_MAX] = {0};
    memset(&m_sDebug, 0, sizeof(DebugInfo));
    property_get("vendor.qcom_parser.debug.meta", property_value, "0");
    m_sDebug.bDumpMetaToLog = atoi(property_value);
    property_value[0] = '\0';

    property_get("vendor.qcom_parser.debug.out", property_value, "0");
    m_sDebug.bDumpToFile = atoi(property_value);
    property_value[0] = '\0';
    if (m_sDebug.bDumpToFile) {
        snprintf(m_sDebug.pucFileName, PROPERTY_VALUE_MAX, "%s", BITSTREAM_LOG_LOC);
        property_get("vendor.qcom_parser.debug.loc", property_value, "");
        if (*property_value)
            strlcpy(m_sDebug.pucFileName, property_value, PROPERTY_VALUE_MAX);
        snprintf(m_sDebug.pucFileName, PROPERTY_VALUE_MAX + 36, "%s/bitstream_%p",
                 m_sDebug.pucFileName, this);
        property_value[0] = '\0';
    }
}

MMParserExtractor::~MMParserExtractor() {
    LOGV("MMParserExtractor::~MMParserExtractor ====> \n");
    Track *track = mFirstTrack;
    while (track) {
        Track *next = track->next;
        delete track;
        track = next;
    }
    mFirstTrack = mLastTrack = NULL;

    if (m_pFileSourceWrapper) {
        delete m_pFileSourceWrapper;
    }
    if (m_pSourcePort) {
        delete m_pSourcePort;
    }
    m_pFileSourceWrapper = NULL;
    m_pSourcePort = NULL;
}

size_t MMParserExtractor::countTracks() {
    status_t err;
    LOGV("MMParserExtractor::countTracks ====> \n");

    if ((err = readMetaData()) != OK) {
        LOGE("MMParserExtractor::countTracks readMetaData failed \n  ");
        return 0;
    }

    LOGV("MMParserExtractor::countTracks NumTracks = %d \n  <====", mTracks);
    return mTracks;
}

sp<MetaData> MMParserExtractor::getTrackMetaData(size_t index,uint32_t flags) {
    status_t err;
    LOGV("MMParserExtractor::getTrackMetaData \n  ");
    if ((err = readMetaData()) != OK) {
        return NULL;
    }

    Track *track = mFirstTrack;
    while (index > 0) {
        if (track == NULL) {
            return NULL;
        }
        track = track->next;
        --index;
    }

    return track->meta;
}

sp<MetaData> MMParserExtractor::getMetaData() {

    status_t err;
    LOGV("MMParserExtractor::getTrackMetaData \n  ");
    if ((err = readMetaData()) != OK)  {
       return new MetaData;
    }

    return mFileMetaData;
}


int convertWideCharToChar( const wchar_t * src, int nSrc,
                  char *dest, int nDest) {
    int count = 0;

    if (nDest==0 || dest == NULL) {
        //return size of required output buffer
        count = (nSrc/sizeof(wchar_t))+1;
    } else if(src != NULL) {
        for(count =0; count<nSrc && count<(nDest-1) && src[count] != '\0'; ++count) {
            dest[count] = src[count];
        }
      dest[count]='\0';
    }
    return count;
}



void MMParserExtractor::setAlbumMetaElement(FileSourceMetaDataType element, uint32_t key) {

    wchar_t* albBuffer = NULL;
    uint32   nMetaBuff = 0;
    FS_TEXT_ENCODING_TYPE eEncode = FS_ENCODING_TYPE_UNKNOWN;
    m_pFileSourceWrapper->GetClipMetaData(NULL, &nMetaBuff, element);
    if(nMetaBuff >0) {
        albBuffer = new wchar_t[nMetaBuff];
    }

    if(albBuffer != NULL) {
        FileSourceStatus albStatus = m_pFileSourceWrapper->GetClipMetaData(albBuffer,
                                                                           &nMetaBuff,
                                                                           element,
                                                                           &eEncode);

        if(albStatus==FILE_SOURCE_SUCCESS && nMetaBuff > 0 ) {

            //! If metadata is in UTF8 or ISO-8859 string format, set it directly
            if(FS_TEXT_ENC_UTF8 == eEncode || FS_TEXT_ENC_ISO8859_1 == eEncode) {
                mFileMetaData->setCString(key, (const char*)albBuffer);
            //! If metadata is in UTF16 string format, set it as char16 string
            } else if(FS_TEXT_ENC_UTF16_BE == eEncode) {
                String8 temp;
                const char16_t *pMetaData = (const char16_t *) (albBuffer);
                char16_t *pucTemp = NULL;
                char16_t *pFrameData;
                uint32 nMetaSize = nMetaBuff / 2;

                // Convert endianness
                pucTemp = new char16_t[nMetaBuff];
                if (pucTemp == NULL) {
                    LOGE("Failed to allocate memory\n");
                    return;
                }
                for (int i = 0; i < nMetaBuff/2; i++) {
                  pucTemp[i] = bswap_16(pMetaData[i]);
                }
                pFrameData = pucTemp;
                // If the string starts with an endianness marker, skip it
                // Since the byte order is already indicated for UTF16BE, BOM is not permitted
                // Treat it as a ZERO WIDTH NON-BREAKING SPACE - ZWNBSP and skip it
                if (0xfeff == *pFrameData) {
                    pFrameData++;
                    nMetaSize--;
                }
                temp.setTo((const char16_t*)pFrameData, nMetaSize);
                mFileMetaData->setCString(key, temp);
                delete[] pucTemp;
            //! If metadata is in UTF16 string format with BOM,
            //! then check the string is in BE or LE format first.
            } else if(FS_TEXT_ENC_UTF16 == eEncode) {
                const char16_t *pMetaData = (const char16_t *) (albBuffer);
                char16_t *pucTemp = NULL;
                char16_t *pFrameData = (char16_t *) albBuffer;
                uint32 nMetaSize = nMetaBuff / 2;
                // endianness marker doesn't match host endianness, convert
                if(0xfffe == *pMetaData) {
                    pucTemp = new char16_t[nMetaBuff];
                    if (pucTemp == NULL) {
                        LOGE("Failed to allocate memory\n");
                        return;
                    }
                    for (int i = 0; i < nMetaBuff/2; i++) {
                        pucTemp[i] = bswap_16(pMetaData[i]);
                    }
                    pFrameData = pucTemp;
                }

                // If the string starts with an endianness marker, skip it
                if (0xfeff == *pFrameData) {
                    pFrameData++;
                    nMetaSize--;
                }
                // check if the resulting data consists entirely of 8-bit values
                bool bEightBitData = true;
                for (int i = 0; i < nMetaSize; i++) {
                    if (pFrameData[i] > 0xff) {
                        bEightBitData = false;
                        break;
                    }
                }
                if (bEightBitData) {
                    // convert to 8 bit format, let the client figure out the real encoding
                    char *pFrameDatain8Bit = new char[nMetaBuff];
                    if (pFrameDatain8Bit == NULL) {
                        LOGE("Failed to allocate memory");
                        return;
                    }
                    for (int i = 0; i <= nMetaSize; i++) {
                        pFrameDatain8Bit[i] = pFrameData[i];
                    }
                    mFileMetaData->setCString(key, pFrameDatain8Bit);
                    delete [] pFrameDatain8Bit;
                } else {
                    String8 temp;
                    temp.setTo((const char16_t*)pFrameData, nMetaSize);
                    mFileMetaData->setCString(key, temp);
                }
                if(pucTemp) {
                    delete[] pucTemp;
                }
            } else if(FS_TEXT_ENC_UTF32 == eEncode) {
                int len = nMetaBuff / sizeof(wchar_t);
                const ssize_t bytes = utf32_to_utf8_length((char32_t *)albBuffer, len);

                char *buffer = new char[bytes + 1];

                if(buffer != NULL && len > 0) {
                    utf32_to_utf8((char32_t *)albBuffer, len, buffer, bytes+1);
                    mFileMetaData->setCString(key, buffer);
                }

                if(buffer != NULL) {
                    delete[] buffer;
                }
            }

        }else {
            LOGE(" get metadata failed for meta type %d, size %u\n", element, nMetaBuff);
        }
        delete[] albBuffer;
    }

}



void MMParserExtractor::setAlbumArtRelatedMeta() {

    //------- Set ALBUM -------------
    setAlbumMetaElement(FILE_SOURCE_MD_ALBUM,kKeyAlbum);

    //------- Set ARTIST -----------
    setAlbumMetaElement(FILE_SOURCE_MD_ARTIST,kKeyArtist);

    //------- Set Title -----------
    setAlbumMetaElement(FILE_SOURCE_MD_TITLE,kKeyTitle);

    //------- Set COMPOSER -----------
    setAlbumMetaElement(FILE_SOURCE_MD_COMPOSER,kKeyComposer);

    //------- Set TRACK -----------
    setAlbumMetaElement(FILE_SOURCE_MD_TRACK_NUM,kKeyCDTrackNumber);

    //------- Set YEAR -----------
    setAlbumMetaElement(FILE_SOURCE_MD_REC_YEAR,kKeyYear);

    //------- Set GENRE -----------
    setAlbumMetaElement(FILE_SOURCE_MD_GENRE,kKeyGenre);

    //------- Set AUTHOR -----------
    setAlbumMetaElement(FILE_SOURCE_MD_AUTHOR,kKeyAuthor);

    //------- Set ALBUMARTIST -----------
    setAlbumMetaElement(FILE_SOURCE_MD_ALBUM_ARTIST,kKeyAlbumArtist);

    //------- Set DISCNUMBER -----------
    setAlbumMetaElement(FILE_SOURCE_MD_DISK_NUM,kKeyDiscNumber);

    //------- Set COMPILATION -----------
    setAlbumMetaElement(FILE_SOURCE_MD_COMPILATION,kKeyCompilation);

    //------- Set WRITER -----------
    setAlbumMetaElement(FILE_SOURCE_MD_WRITER,kKeyWriter);

    //------- Set CREATION DATE -----------
    setAlbumMetaElement(FILE_SOURCE_MD_CREATION_DATE,kKeyDate);

    //------- Set Location Info -----------
    setAlbumMetaElement(FILE_SOURCE_MD_GEOTAG,kKeyLocation);
}

void MMParserExtractor::setAlbumArt() {

    //------- Set Album Art ----------------------
    uint32 nMetaBuff = 0;
    wchar_t* albBuffer = NULL;
    m_pFileSourceWrapper->GetClipMetaData(NULL, &nMetaBuff, FILE_SOURCE_MD_ALBUM_ART);

    LOGV("ClipMetaData size = %lu \n", nMetaBuff);
    if(nMetaBuff > sizeof(FS_ALBUM_ART_METADATA)) {
        albBuffer = new wchar_t[nMetaBuff];
    } else {
        LOGV("AlbumArtData data not present, ignoring it");
    }

    if(albBuffer != NULL) {
        FileSourceStatus albStatus = m_pFileSourceWrapper->GetClipMetaData(albBuffer, &nMetaBuff, FILE_SOURCE_MD_ALBUM_ART);

        if(albStatus == FILE_SOURCE_SUCCESS) {

            FS_ALBUM_ART_METADATA *tmpMetaData = (FS_ALBUM_ART_METADATA*)albBuffer;

            //copy album art data
            mFileMetaData->setData(kKeyAlbumArt, MetaData::TYPE_NONE, tmpMetaData->pucPicData, tmpMetaData->ulPicDataLen);

            //copy Mime type
            mFileMetaData->setCString(kKeyAlbumArtMIME, (char*)tmpMetaData->ucImgFormatStr);
            LOGV("kKeyAlbumArtMIME = %s", (char*)tmpMetaData->ucImgFormatStr);
        }else {
            LOGE(" getAlbumArt failed \n");
        }
        delete[] albBuffer;
    }

    albBuffer = NULL;

}

static inline bool isValidInt32ColourValue(long long value) {
  return value != MAX_INT64
      && value >= MIN_INT32
      && value <= MAX_INT32;
}
static inline bool isValidUint16ColourValue(long long value) {
    return value != MAX_INT64
        && value >= 0 && value <= MAX_UINT16;
}
static inline bool isValidPrimary(double primary) {
    return primary != NULL && primary >= 0 && primary <= 1;
}

bool FileSourceWrapper::IsCodecSupportAvailable(
          FileSourceFileFormat  eFileFormat,
          FileSourceMnMediaType eCodecType,
          MediaTrackInfo mediaTrackInfo) {

    bool bCodecAllowed = true;
    FileSourceAudioInfo sAudioInfo = mediaTrackInfo.audioTrackInfo;
    switch (eFileFormat) {
        case FILE_SOURCE_APE:
            if (FILE_SOURCE_MN_TYPE_APE == eCodecType &&
                8 == sAudioInfo.nBitsPerSample) {
                bCodecAllowed = false;
                MM_MSG_PRIO1(MM_FILE_OPS, MM_PRIO_HIGH,
                    "MM-Parser: AAL: APE- %d bit support is disabled",
                    sAudioInfo.nBitsPerSample);
            }
            break;
        case FILE_SOURCE_FLAC:
            if (FILE_SOURCE_MN_TYPE_FLAC == eCodecType &&
                8 == sAudioInfo.nBitsPerSample) {
                bCodecAllowed = false;
                MM_MSG_PRIO1(MM_FILE_OPS, MM_PRIO_HIGH,
                    "MM-Parser: AAL: FLAC- %d bit support is disabled",
                    sAudioInfo.nBitsPerSample);
            }
            break;

        default:
            break;
    }
    return bCodecAllowed;
}

status_t MMParserExtractor::updateAudioTrackInfoFromCSD(
        const uint8_t* csd, size_t csd_size) {
    LOGV("%s Enter:", __func__);
    int32_t sampleRate = 0;
    int32_t numChannels = 0;
    uint32_t objectType = 0;
    if (csd_size == 0) {
       return OK;
    }

    if (csd_size < 2) {
        LOGE("Codec specific data was malformed");
        return ERROR_MALFORMED;
    }

    static uint32_t kSamplingRate[] = {
        96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
        16000, 12000, 11025, 8000, 7350
    };

    ABitReader br(csd, csd_size);
    objectType = br.getBits(5);

    if (objectType == 31) {  //AAC-ELD => additional 6 bits
        objectType = 32 + br.getBits(6);
    }

    if (mLastTrack == NULL) {
        return ERROR_MALFORMED;
    }

    mLastTrack->meta->setInt32(kKeyAACAOT, objectType);
    uint32_t freqIndex = br.getBits(4);
    if (freqIndex == 15) {
        if (br.numBitsLeft() < 28) {
            LOGE("Codec specific data was malformed");
            return ERROR_MALFORMED;
        }
        sampleRate = br.getBits(24);
        numChannels = br.getBits(4);
    } else {
        if (br.numBitsLeft() < 4) {
            LOGE("Codec specific data was malformed");
            return ERROR_MALFORMED;
        }
        numChannels = br.getBits(4);
        if (freqIndex == 13 || freqIndex == 14) {
            LOGE("Codec specific data was malformed");
            return ERROR_MALFORMED;
        }
        sampleRate = kSamplingRate[freqIndex];
    }
    if (objectType == AOT_SBR || objectType == AOT_PS) {//SBR specific config per 14496-3 table 1.13
        if (br.numBitsLeft() < 4) return ERROR_MALFORMED;
        uint32_t extFreqIndex = br.getBits(4);
        int32_t extSampleRate __unused;
        if (extFreqIndex == 15) {
            if (csd_size < 8) {
                return ERROR_MALFORMED;
            }
            if (br.numBitsLeft() < 24) return ERROR_MALFORMED;
            extSampleRate = br.getBits(24);
        } else {
            if (extFreqIndex == 13 || extFreqIndex == 14) {
                return ERROR_MALFORMED;
            }
            extSampleRate = kSamplingRate[extFreqIndex];
        }
    }
    switch (numChannels) {
         case 0:
         case 1: //FC
         case 2: //FL FR
         case 3: //FC, FL, FR
         case 4: //FC, FL, FR, RC
         case 5: //FC, FL, FR, SL SR
         case 6: //FC, FL, FR, SL, SR, LFE
            break;
         case 11: //FC, FL, FR, SL, SR, RC, LFE
            numChannels = 7;
            break;
         case 7:
         case 12:
         case 14:
            numChannels = 8;
            break;
         default:
            return ERROR_UNSUPPORTED;
    }//switch(numChannels)
    if (objectType == AOT_SBR || objectType == AOT_PS) {
        if (br.numBitsLeft() < 5) return ERROR_MALFORMED;
        objectType = br.getBits(5);

        if (objectType == AOT_ESCAPE) {
            if (br.numBitsLeft() < 6) return ERROR_MALFORMED;
            objectType = 32 + br.getBits(6);
        }
    }
    if (objectType == AOT_AAC_LC || objectType == AOT_ER_AAC_LC ||
            objectType == AOT_ER_AAC_LD || objectType == AOT_ER_AAC_SCAL ||
            objectType == AOT_ER_BSAC) {
        if (br.numBitsLeft() < 2) return ERROR_MALFORMED;

        const int32_t frameLengthFlag __unused = br.getBits(1);
        const int32_t dependsOnCoreCoder = br.getBits(1);

        if (dependsOnCoreCoder ) {
            if (br.numBitsLeft() < 14) return ERROR_MALFORMED;
            const int32_t coreCoderDelay __unused = br.getBits(14);
        }

        int32_t extensionFlag = -1;
        if (br.numBitsLeft() > 0) {
            extensionFlag = br.getBits(1);
        }else {
           switch (objectType) {
           case AOT_AAC_LC:
              extensionFlag =0;
              break;
           case AOT_ER_AAC_LC:
           case AOT_ER_AAC_SCAL:
           case AOT_ER_BSAC:
           case AOT_ER_AAC_LD:
              extensionFlag = 1;
              break;
           default:
              return ERROR_MALFORMED;
              break;
           }
        }
        if (numChannels == 0 && (br.numBitsLeft() > 0)) {
            int32_t channelsEffectiveNum = 0;
            int32_t channelsNum = 0;
            if (br.numBitsLeft() < 32) {
                return ERROR_MALFORMED;
            }
            const int32_t ElementInstanceTag __unused = br.getBits(4);
            const int32_t Profile __unused = br.getBits(2);
            const int32_t SamplingFrequencyIndex __unused = br.getBits(4);
            const int32_t NumFrontChannelElements = br.getBits(4);
            const int32_t NumSideChannelElements = br.getBits(4);
            const int32_t NumBackChannelElements = br.getBits(4);
            const int32_t NumLfeChannelElements = br.getBits(2);
            const int32_t NumAssocDataElements __unused = br.getBits(3);
            const int32_t NumValidCcElements __unused = br.getBits(4);

            const int32_t MonoMixdownPresent = br.getBits(1);

            if (MonoMixdownPresent != 0) {
                if (br.numBitsLeft() < 4) return ERROR_MALFORMED;
                const int32_t MonoMixdownElementNumber __unused = br.getBits(4);
            }

            if (br.numBitsLeft() < 1) return ERROR_MALFORMED;
            const int32_t StereoMixdownPresent = br.getBits(1);
            if (StereoMixdownPresent != 0) {
                if (br.numBitsLeft() < 4) return ERROR_MALFORMED;
                const int32_t StereoMixdownElementNumber __unused = br.getBits(4);
            }

            if (br.numBitsLeft() < 1) return ERROR_MALFORMED;
            const int32_t MatrixMixdownIndexPresent = br.getBits(1);
            if (MatrixMixdownIndexPresent != 0) {
                if (br.numBitsLeft() < 3) return ERROR_MALFORMED;
                const int32_t MatrixMixdownIndex __unused = br.getBits(2);
                const int32_t PseudoSurroundEnable __unused = br.getBits(1);
            }

            int i;
            for (i=0; i < NumFrontChannelElements; i++) {
                if (br.numBitsLeft() < 5) return ERROR_MALFORMED;
                const int32_t FrontElementIsCpe = br.getBits(1);
                const int32_t FrontElementTagSelect __unused = br.getBits(4);
                channelsNum += FrontElementIsCpe ? 2 : 1;
            }

            for (i=0; i < NumSideChannelElements; i++) {
                if (br.numBitsLeft() < 5) return ERROR_MALFORMED;
                const int32_t SideElementIsCpe = br.getBits(1);
                const int32_t SideElementTagSelect __unused = br.getBits(4);
                channelsNum += SideElementIsCpe ? 2 : 1;
            }

            for (i=0; i < NumBackChannelElements; i++) {
                if (br.numBitsLeft() < 5) return ERROR_MALFORMED;
                const int32_t BackElementIsCpe = br.getBits(1);
                const int32_t BackElementTagSelect __unused = br.getBits(4);
                channelsNum += BackElementIsCpe ? 2 : 1;
            }
            channelsEffectiveNum = channelsNum;

            for (i=0; i < NumLfeChannelElements; i++) {
                if (br.numBitsLeft() < 4) return ERROR_MALFORMED;
                const int32_t LfeElementTagSelect __unused = br.getBits(4);
                channelsNum += 1;
            }
            LOGV("mpeg4 audio channelsNum = %d", channelsNum);
            LOGV("mpeg4 audio channelsEffectiveNum = %d", channelsEffectiveNum);
            numChannels = channelsNum;
        }
    }
    if (numChannels ==0 ) {
       return ERROR_UNSUPPORTED;
    }
    if (mLastTrack == NULL) {
       return ERROR_MALFORMED;
    }
    int32_t prevSampleRate;
    mLastTrack->meta->findInt32(kKeySampleRate, &prevSampleRate);
    if (prevSampleRate != sampleRate) {
       LOGE("mpeg4 audio sample rate different from previous setting"
            "was: %d, now: %d",prevSampleRate,sampleRate);
    }
    mLastTrack->meta->setInt32(kKeySampleRate, sampleRate);
    LOGV("sampleRate: %d",sampleRate);

    int32_t prevChannelCount;
    mLastTrack->meta->findInt32(kKeyChannelCount, &prevChannelCount);
    if (prevChannelCount != numChannels) {
       LOGE("mpeg4 audio channel count different from previous setting"
            "was: %d, now: %d",prevChannelCount,numChannels);
    }
    mLastTrack->meta->setInt32(kKeyChannelCount, numChannels);
    LOGV("numChannels: %d",numChannels);

    return OK;
}
status_t MMParserExtractor::FillColorInformation(Track *track,
                                                 FileSourceFileFormat eFileFmt)
{
  status_t configStatus;
  int32_t primaries = 2;
  int32_t coeffs = 2;
  int32_t transfer = 2;
  bool fullRange = false;
  bool rangeSpecified = false;

  FS_VIDEO_PARAM_COLOR_INFOTYPE sColor_params = {0};
  FS_VIDEO_PARAM_MASTER_METADATA_INFOTYPE sMaster_meta = {0};
  configStatus = m_pFileSourceWrapper->GetStreamParameter(track->trackId,
                                  FS_IndexParamVideoColorInfo, &sColor_params);
  if(configStatus != FILE_SOURCE_SUCCESS)
  {
    return FILE_SOURCE_SUCCESS;
  }
  ALOGV("llPrimaries %llu,llTransferCharacteristics %llu,llMatrixCoefficients %llu,llRange %llu",
      sColor_params.llPrimaries,sColor_params.llTransferCharacteristics,
      sColor_params.llMatrixCoefficients,sColor_params.llRange);

  if (isValidInt32ColourValue(sColor_params.llPrimaries))
    primaries = sColor_params.llPrimaries;
  if (isValidInt32ColourValue(sColor_params.llTransferCharacteristics))
    transfer = sColor_params.llTransferCharacteristics;
  if (isValidInt32ColourValue(sColor_params.llMatrixCoefficients))
    coeffs = sColor_params.llMatrixCoefficients;
  if ( ( FILE_SOURCE_MKV == eFileFmt) &&
       ( sColor_params.llRange != 0x7fffffffffffffffLL ) &&
       ( sColor_params.llRange != 0 ) ) {
    fullRange = sColor_params.llRange == 2;
    rangeSpecified = true;
  }
  ColorAspects aspects;
  ColorUtils::convertIsoColorAspectsToCodecAspects(
          primaries, transfer, coeffs, fullRange, aspects);
  track->meta->setInt32(kKeyColorPrimaries, aspects.mPrimaries);
  track->meta->setInt32(kKeyTransferFunction, aspects.mTransfer);
  track->meta->setInt32(kKeyColorMatrix, aspects.mMatrixCoeffs);
  if (FILE_SOURCE_MKV == eFileFmt) {
    track->meta->setInt32(kKeyColorRange, rangeSpecified ?
                          aspects.mRange : ColorAspects::RangeUnspecified);
  } else if ( ( FILE_SOURCE_MPEG4 == eFileFmt ) ||
              ( FILE_SOURCE_3G2 == eFileFmt ) ) {
    track->meta->setInt32(kKeyColorRange, aspects.mRange);
  }

  HDRStaticInfo info;
  memset(&info, 0, sizeof(info));
  configStatus = m_pFileSourceWrapper->GetStreamParameter(track->trackId,
                          FS_IndexParamVideoMasterMetaDataInfo, &sMaster_meta);
  if(configStatus != FILE_SOURCE_SUCCESS)
  {
    return FILE_SOURCE_SUCCESS;
  }
  ALOGV("Chrom_R_X %llf,Chrom_R_y %llf,chrom_g_x %llf,chrom_g_y %llf,chrom_b_x %llf,chrom_b_y %llf,llMaxCLL %llu,llMaxFALL %llu",
   sMaster_meta.fPrimChromaticityR_XY[0],sMaster_meta.fPrimChromaticityR_XY[1],
   sMaster_meta.fPrimChromaticityG_XY[0],sMaster_meta.fPrimChromaticityG_XY[1],
   sMaster_meta.fPrimChromaticityB_XY[0],sMaster_meta.fPrimChromaticityB_XY[1],
   sColor_params.llMaxCLL,sColor_params.llMaxFALL);
  if( configStatus != FILE_SOURCE_SUCCESS )
    LOGV("Getting colour master metadata StreamParameter %d", configStatus );
  if (isValidUint16ColourValue(sColor_params.llMaxCLL))
    info.sType1.mMaxContentLightLevel = sColor_params.llMaxCLL;
  if (isValidUint16ColourValue(sColor_params.llMaxFALL))
    info.sType1.mMaxFrameAverageLightLevel = sColor_params.llMaxFALL;
  if (isValidPrimary(sMaster_meta.fPrimChromaticityR_XY[0])) {
    info.sType1.mR.x = \
      (uint16_t)(50000 * sMaster_meta.fPrimChromaticityR_XY[0] + 0.5);
  }
  if (isValidPrimary(sMaster_meta.fPrimChromaticityR_XY[1])) {
    info.sType1.mR.y = \
      (uint16_t)(50000 * sMaster_meta.fPrimChromaticityR_XY[1] + 0.5);
  }
  if (isValidPrimary(sMaster_meta.fPrimChromaticityG_XY[0])) {
    info.sType1.mG.x = \
      (uint16_t)(50000 * sMaster_meta.fPrimChromaticityG_XY[0] + 0.5);
  }
  if (isValidPrimary(sMaster_meta.fPrimChromaticityG_XY[1])) {
    info.sType1.mG.y = \
      (uint16_t)(50000 * sMaster_meta.fPrimChromaticityG_XY[1] + 0.5);
  }
  if (isValidPrimary(sMaster_meta.fPrimChromaticityB_XY[0])) {
    info.sType1.mB.x = \
      (uint16_t)(50000 * sMaster_meta.fPrimChromaticityB_XY[0] + 0.5);
  }
  if (isValidPrimary(sMaster_meta.fPrimChromaticityB_XY[1])) {
    info.sType1.mB.y = \
      (uint16_t)(50000 * sMaster_meta.fPrimChromaticityB_XY[1] + 0.5);
  }
  if (isValidPrimary(sMaster_meta.fPrimChromaticityWhile_XY[0])) {
    info.sType1.mW.x = \
      (uint16_t)(50000 * sMaster_meta.fPrimChromaticityWhile_XY[0] + 0.5);
  }
  if (isValidPrimary(sMaster_meta.fPrimChromaticityWhile_XY[1])) {
    info.sType1.mW.y = \
      (uint16_t)(50000 * sMaster_meta.fPrimChromaticityWhile_XY[1] + 0.5);
  }
  if ((sMaster_meta.fLuminance_Max >= 0.5) &&
      (sMaster_meta.fLuminance_Max < 65535.5)) {
    info.sType1.mMaxDisplayLuminance = \
      (uint16_t)(sMaster_meta.fLuminance_Max + 0.5);
  }
  if ((sMaster_meta.fLuminance_Min >= 0.00005) &&
      (sMaster_meta.fLuminance_Min < 6.55355)) {
    info.sType1.mMinDisplayLuminance = \
      (uint16_t)(10000 * sMaster_meta.fLuminance_Min + 0.5);
  }
  track->meta->setData(kKeyHdrStaticInfo, 'hdrS', &info, sizeof(info));
  return FILE_SOURCE_SUCCESS;
}

//  Parse metadata in this function
status_t MMParserExtractor::readMetaData() {
    LOGV("MMParserExtractor::readMetaData \n  ");
    //! Check if metadata is already parsed or not
    if (NO_INIT != mHaveMetadata) {
        if (OK == mHaveMetadata) {
            LOGV("readMetaData : already has metadata \n");
            return OK;
        } else {
            LOGW("readMetaData : metadata parsing is failed \n");
            return ERROR_IO;
        }
    }

    //! Init with error, Update to "OK" if parsing is successful
    mHaveMetadata = UNKNOWN_ERROR;

    m_pFileSourceWrapper = FileSourceWrapper::New(*m_pSourcePort, m_eFileFormat);
    if(!m_pFileSourceWrapper){
        LOGE(" FileSourceWrapper::New returned NULL \n");
        return ERROR_IO;
    }

    FileSourceFileFormat fileFormat;
    FileSourceStatus status = m_pFileSourceWrapper->GetFileFormat(fileFormat);

    // if big buffer mode enabled, override default buffer size
    char audioParserBuf[PROPERTY_VALUE_MAX];
    property_get("vendor.audio.parser.ip.buffer.size", audioParserBuf, "0");
    uint32_t audParserInpBufSize = atoi(audioParserBuf);

    LOGV("readMetaData - Get track list  \n");
    FileSourceTrackIdInfoType trackList[FILE_SOURCE_MAX_NUM_TRACKS];
    uint32_t numTracks = m_pFileSourceWrapper->GetWholeTracksIDList(trackList);
    LOGV("readMetaData - Num tracks = %d  \n", numTracks);
    //save the number of tracks
    FileSourceMjMediaType majorType;
    FileSourceMnMediaType minorType = FILE_SOURCE_MN_TYPE_UNKNOWN;
    //! If more than one Video track is present, set thumbnail time only for
    //! first video track.
    bool bThumbNail = false;
    for (uint32_t i=0; i<numTracks; i++){
        FileSourceTrackIdInfoType trackInfo = trackList[i];
        MediaTrackInfo mediaTrackInfo;
        LOGV("iterating trackList[i], i = %d",i);
        // get mime type
        const char* pMimeType =NULL;
        status = m_pFileSourceWrapper->GetMimeType(trackInfo.id, majorType, minorType);
        LOGV("MMParserExtractor::readMetaData, after getting Mime type,  trackInfo.id = %u, majorType = %d, minorType = %d\n ",(unsigned int)trackInfo.id,majorType, minorType);

        if (status != FILE_SOURCE_SUCCESS) {
            LOGV("MMParserExtractor::readMetaData failed in GetMimeType %d", status);
            continue;
        }
        else {
            bool bIsDivxEnabled =
             m_pFileSourceWrapper->IsDivxSupportEnabled(fileFormat, minorType);
           // get mediatrackinfo
           status = m_pFileSourceWrapper->GetMediaTrackInfo(trackInfo.id, &mediaTrackInfo);
            //! Some codec types have constraints with respect to bps, sampling freq etc
            bool bIsMIMETypeAllowed =
             m_pFileSourceWrapper->IsCodecSupportAvailable(fileFormat, minorType,
                                                           mediaTrackInfo);
            if(bIsDivxEnabled && bIsMIMETypeAllowed) {
                pMimeType = MediaType2MIME(minorType);
            } else {
                MM_MSG_PRIO1(MM_FILE_OPS, MM_PRIO_HIGH,
                    "MM-Parser: AAL: Codec %d support is disabled",
                    minorType);
            }
            if (NULL!= pMimeType) {
                LOGV("MMParserExtractor::readMetaData Valid Mime type =%s \n ",pMimeType);
            } else {
                if ((i == (numTracks - 1)) && !mTracks) {
                    LOGE("Error - No valid tracks - Un supported Minor Type = %d  \n",minorType);
                    return BAD_TYPE;
                } else {
                    LOGV("Un supported Minor Type = %d - Iterate through tracklist \n",minorType);
                    continue;
                }
            }
        }
        // Create Track object and set metadata
        Track *track = new Track;
        track->next = NULL;
        if (mLastTrack) {
            mLastTrack->next = track;
        } else {
            mFirstTrack = track;
        }
        mLastTrack = track;

        track->meta = new MetaData;
        track->timescale = 0; // update it after querying GetMediaTrackInfo() on FileSource
        track->trackId = trackInfo.id;
        track->meta->setCString(kKeyMIMEType, pMimeType);
        mTracks++; //increment track count
        if(minorType==FILE_SOURCE_MN_TYPE_WMA_PRO) {
            track->meta->setInt32(kKeyWMAVersion, kTypeWMAPro);
        } else if(minorType==FILE_SOURCE_MN_TYPE_WMA_LOSSLESS) {
            track->meta->setInt32(kKeyWMAVersion, kTypeWMALossLess);
        } else if(minorType==FILE_SOURCE_MN_TYPE_WMA) {
            track->meta->setInt32(kKeyWMAVersion, kTypeWMA);
        }

        if(minorType==FILE_SOURCE_MN_TYPE_DIVX311) {
            track->meta->setInt32(kKeyDivXVersion, kTypeDivXVer_3_11);
        } else if (minorType==FILE_SOURCE_MN_TYPE_DIVX40) {
            track->meta->setInt32(kKeyDivXVersion, kTypeDivXVer_4);
        } else if (minorType==FILE_SOURCE_MN_TYPE_DIVX50_60) {
            track->meta->setInt32(kKeyDivXVersion, kTypeDivXVer_5);
        }
        const char* pFileFormat = fillMimeType(fileFormat,true);

        if(NULL != pFileFormat) {
            track->meta->setCString(kKeyFileFormat, pFileFormat);
        }

        //Set decoder arbitrary mode based on file format & codec type
        if ((fileFormat == FILE_SOURCE_AVI) ||
            (fileFormat == FILE_SOURCE_MKV)){
            //XVID/MPEG4 can contain B frames, use arbitrary mode
            if (minorType==FILE_SOURCE_MN_TYPE_MPEG4 ||
                minorType==FILE_SOURCE_MN_TYPE_DIVX40 ||
                minorType==FILE_SOURCE_MN_TYPE_DIVX50_60) {
                track->meta->setInt32(kKeyUseArbitraryMode, 1);
            } else {
                track->meta->setInt32(kKeyUseArbitraryMode, 0);
            }
        }
        // To avoid issues with multiple frames in
        // one PES case.
        if ((fileFormat == FILE_SOURCE_MP2PS) ||
              (fileFormat == FILE_SOURCE_MP2TS)){
          track->meta->setInt32(kKeyUseArbitraryMode, 1);
        }

        if (fileFormat == FILE_SOURCE_ASF) {
            //VC1 is advance profile, use arbitrary mode
            if (minorType==FILE_SOURCE_MN_TYPE_VC1) {
                track->meta->setInt32(kKeyUseArbitraryMode, 1);
            } else {
                track->meta->setInt32(kKeyUseArbitraryMode, 0);
            }
        }

        if (status == FILE_SOURCE_SUCCESS){
            if (majorType == FILE_SOURCE_MJ_TYPE_AUDIO){
                size_t max_size = m_pFileSourceWrapper->GetTrackMaxFrameBufferSize(trackInfo.id);
                track->meta->setInt32(kKeyMaxInputSize, max_size);
                // audio
                track->meta->setInt32(kKeyChannelCount, mediaTrackInfo.audioTrackInfo.numChannels);
                track->meta->setInt32(kKeySampleRate, mediaTrackInfo.audioTrackInfo.samplingRate);
                if (mediaTrackInfo.audioTrackInfo.bitRate) {
                    track->meta->setInt32(kKeyBitRate, mediaTrackInfo.audioTrackInfo.bitRate);
                }
                if (mediaTrackInfo.audioTrackInfo.maxBitRate) {
                    track->meta->setInt32(kKeyMaxBitRate, mediaTrackInfo.audioTrackInfo.maxBitRate);
                }
                LOGV("MMParserExtractor::readMetaData audio bitRate %u maxBitRate %u", mediaTrackInfo.audioTrackInfo.bitRate,mediaTrackInfo.audioTrackInfo.maxBitRate);
                track->meta->setCString(kKeyMediaLanguage,(const char*)mediaTrackInfo.audioTrackInfo.ucLanguageCode);
                track->timescale = mediaTrackInfo.audioTrackInfo.timeScale;

                LOGV("MMParserExtractor::readMetaData Audio track metadata numChannels = %lu samplingRate = %lu timeScale = %lu duration = %llu",
                        mediaTrackInfo.audioTrackInfo.numChannels, mediaTrackInfo.audioTrackInfo.samplingRate,
                        mediaTrackInfo.audioTrackInfo.timeScale, mediaTrackInfo.audioTrackInfo.duration);

                int64 AudioDuration = mediaTrackInfo.audioTrackInfo.duration;
                uint32 ulTimeScale  = mediaTrackInfo.audioTrackInfo.timeScale;
                FileSourceFileFormat format = FILE_SOURCE_UNKNOWN;
                m_pFileSourceWrapper->GetFileFormat(format);
                if (format == FILE_SOURCE_3G2) { //File is fragmented
                    AudioDuration = m_pFileSourceWrapper->GetClipDuration();
                    if (AudioDuration <= 0)
                        AudioDuration = mediaTrackInfo.audioTrackInfo.duration;
                }

                LOGV("MMParserExtractor::readMetaData Audio trackAudioDuration = %u %u",
                        (unsigned int)(AudioDuration>>32), (unsigned int)(AudioDuration&0xFFFFFFFF));

                int64 kKeyDurationValue = AudioDuration;
                if(ulTimeScale && ulTimeScale != 1000000) {
                    kKeyDurationValue = AudioDuration * (1000000 / ulTimeScale);
                }
                track->meta->setInt64(kKeyDuration,kKeyDurationValue);
                LOGV("MMParserExtractor::readMetaData, kKeyDuration = %u %u",
                        (unsigned int)(kKeyDurationValue>>32), (unsigned int)(kKeyDurationValue&0xFFFFFFFF));

                if(mediaTrackInfo.audioTrackInfo.nEncoderDelay) {
                    track->meta->setInt32(kKeyEncoderDelay, mediaTrackInfo.audioTrackInfo.nEncoderDelay);
                    LOGV("MMParserExtractor::readMetaData, kKeyEncoderDelay = %lu",
                         mediaTrackInfo.audioTrackInfo.nEncoderDelay);
                }
                if(mediaTrackInfo.audioTrackInfo.nPaddingDelay) {
                    track->meta->setInt32(kKeyEncoderPadding, mediaTrackInfo.audioTrackInfo.nPaddingDelay);
                    LOGV("MMParserExtractor::readMetaData, kKeyEncoderPadding = %lu",
                         mediaTrackInfo.audioTrackInfo.nPaddingDelay);
                }
                if(mediaTrackInfo.audioTrackInfo.nBitsPerSample) {
                    track->meta->setInt32(kKeySampleBits, mediaTrackInfo.audioTrackInfo.nBitsPerSample);
                    LOGV("MMParserExtractor::readMetaData, kKeySampleBits = %lu",
                         mediaTrackInfo.audioTrackInfo.nBitsPerSample);
                }

                if( pMimeType == MEDIA_MIMETYPE_AUDIO_AAC ) {
                    LOGV("Inside AAC block ");
                    uint32 nFormatBlockSize = 0;
                    LOGV("MMParserExtractor::kKeyBitRate for AAC is not required");
                    status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, NULL, &nFormatBlockSize);
                    LOGV(" getFormatBlock size = %u \n", nFormatBlockSize);
                    uint8_t *buffer = new uint8_t[nFormatBlockSize];
                    if(buffer!=NULL) {
                        status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, buffer, &nFormatBlockSize);
                        if(status==FILE_SOURCE_SUCCESS) {
                           track->meta->setData(kKeyAacCodecSpecificData, MetaData::TYPE_NONE, buffer, nFormatBlockSize);
                           status_t CSDStatus;
                           CSDStatus = updateAudioTrackInfoFromCSD(buffer, nFormatBlockSize);
                           if (CSDStatus != OK) {
                               LOGE(" updateAudioTrackInfoFromCSD failed!");
                           }
                        } else {
                           LOGE(" getFormatBlock failed decoder config can fail, continue \n");
                        }
                        delete[] buffer;
                        buffer = NULL;
                    }

                    FileSourceConfigItem pitem;
                    status_t configStatus;
                    AacCodecData pCodecData;
                    bool codecDataStatus = m_pFileSourceWrapper->GetAACCodecData(trackInfo.id,&pCodecData);

                    if(codecDataStatus && pCodecData.ucAACProfile == 4)
                    {
                         LOGV("setting LTP profile to meta data");
                         track->meta->setInt32(kkeyAacFormatLtp, true);
                    }

                    if( minorType == FILE_SOURCE_MN_TYPE_AAC_ADIF ) {
                         track->meta->setInt32(kkeyAacFormatAdif, true);
                         mIsAacformatAdif = true;
                         configStatus = m_pFileSourceWrapper->SetConfiguration(trackInfo.id, &pitem, FILE_SOURCE_MEDIA_STRIP_AUDIO_HEADER);
                    }
                    else {
                         track->meta->setInt32(kkeyAacFormatAdif, false);
                         if ( codecDataStatus ) {
                             LOGV("set profile info to meta data");
                             track->meta->setInt32(kKeyAACProfile,pCodecData.ucAACProfile);
                             track->meta->setInt32(kKeyAACAOT,pCodecData.ucAACProfile);
                             // Set strip audio header config only if audio object type is
                             // FILE_SOURCE_AAC_PROFILE_LC or FILE_SOURCE_AAC_PROFILE_LTP
                             if( pCodecData.ucAACProfile == 2 || pCodecData.ucAACProfile == 4 )
                                 configStatus = m_pFileSourceWrapper->SetConfiguration(trackInfo.id, &pitem, FILE_SOURCE_MEDIA_STRIP_AUDIO_HEADER );
                         } else {
                             LOGE("Error in determining Audio Object Type");
                         }

                         // for audio only local playback usecase if big buffer size feature is enabled set Byte stream mode
                         if ((true == gbLocalPlayback) && mUseBigBuffers &&
                                  ((audParserInpBufSize > 0) && (audParserInpBufSize <= MAX_PARSER_IN_BUF_SIZE)) &&
                                  (FILE_SOURCE_AAC == fileFormat)) {
                             configStatus = m_pFileSourceWrapper->SetConfiguration(trackInfo.id, &pitem, FILE_SOURCE_MEDIA_OUTPUT_BYTES_STREAM);
                             track->meta->setInt32(kKeyIsADTS, 1);
                             track->meta->setInt32(kKeyMaxInputSize, audParserInpBufSize);
                             track->meta->setInt32(kKeyIsByteMode, 1);
                         } else {
                             configStatus = m_pFileSourceWrapper->SetConfiguration(trackInfo.id, &pitem, FILE_SOURCE_MEDIA_OUTPUT_SINGLE_AUDIO_FRAME);
                         }
                    }

                    if( configStatus != FILE_SOURCE_SUCCESS ) {
                         LOGV("Error in setting AAC config %d", configStatus );
                    }
                } // end of if AAC

                if( minorType == FILE_SOURCE_MN_TYPE_EVRC || minorType == FILE_SOURCE_MN_TYPE_QCELP ) {
                    FileSourceConfigItem pitem;
                    status_t configStatus;
                    configStatus = m_pFileSourceWrapper->SetConfiguration(trackInfo.id, &pitem, FILE_SOURCE_MEDIA_OUTPUT_SINGLE_AUDIO_FRAME);
                    if( configStatus != FILE_SOURCE_SUCCESS ) {
                        LOGE("Setting EVRC/QCELP config %d", configStatus );
                    }
                }//end of if QCELP/EVRC

                //Get WMA codec specfic data
                if( minorType == FILE_SOURCE_MN_TYPE_WMA || minorType == FILE_SOURCE_MN_TYPE_WMA_PRO || minorType == FILE_SOURCE_MN_TYPE_WMA_LOSSLESS) {
                    WmaCodecData codecData;

                    LOGV("MMParserExtractor::readMetaData WMA audo format detected");
                    if( m_pFileSourceWrapper->GetWMACodecData(trackInfo.id, &codecData) != true) {
                        LOGE("MMParserExtractor::readMetaData GetWMACodecData() failed");
                        continue;
                    }
                    track->meta->setInt32(kKeyWMAEncodeOpt, codecData.nEncodeOpt);
                    LOGV("MMParserExtractor::readMetaData WMA audo Encode options %lu", codecData.nEncodeOpt);
                    track->meta->setInt32(kKeyWMABlockAlign, codecData.nBlockAlign);
                    LOGV("MMParserExtractor::readMetaData WMA audo Block Align %u", codecData.nBlockAlign);
                    if (max_size < codecData.nASFPacketSize) {
                       track->meta->setInt32(kKeyMaxInputSize, codecData.nASFPacketSize);
                       LOGV("MMParserExtractor::readMetaData WMA audio nASFPacketSize %lu", codecData.nASFPacketSize);
                    }
                    track->meta->setInt32(kKeyWMABitspersample, codecData.nBitsPerSample);
                    track->meta->setInt32(kKeySampleBits, codecData.nBitsPerSample);
                    LOGV("MMParserExtractor::readMetaData WMA audo bits per sample %lu", codecData.nBitsPerSample);
                    track->meta->setInt32(kKeyWMAChannelMask, codecData.nChannelMask);
                    LOGV("MMParserExtractor::readMetaData WMA channel mask %u", codecData.nChannelMask);
                    track->meta->setInt32(kKeyWMAFormatTag, codecData.nFormatTag);
                    LOGV("MMParserExtractor::readMetaData WMA audo format tag %lu", codecData.nFormatTag);
                    if( (minorType == FILE_SOURCE_MN_TYPE_WMA_PRO) || (minorType == FILE_SOURCE_MN_TYPE_WMA_LOSSLESS) ) {
                        LOGV("MMParserExtractor::readMetaData WMA PRO audo format detected");
                        codecData.nVirtualPktSize = (((codecData.nVirtualPktSize & 0xff) << 8) | ((codecData.nVirtualPktSize & 0xff00) >> 8));
                        LOGV("codecData.nVirtualPktSize %lu", codecData.nVirtualPktSize);
                        track->meta->setInt32(kKeyWMAVirPktSize, codecData.nVirtualPktSize);
                        LOGV("MMParserExtractor::readMetaData WMA PRO virtual packet size %lu", codecData.nVirtualPktSize);
                        track->meta->setInt32(kKeyWMAAdvEncOpt1, codecData.nAdvEncodeOpt);
                        LOGV("MMParserExtractor::readMetaData WMA PRO audo advance encode opt %d", codecData.nAdvEncodeOpt);
                        track->meta->setInt32(kKeyWMAAdvEncOpt2, codecData.nAdvEncodeOpt2);
                        LOGV("MMParserExtractor::readMetaData WMA PRO audo advance encode opt2 %u", codecData.nAdvEncodeOpt2);
                    } //if( (minorType == FILE_SOURCE_MN_TYPE_WMA_PRO)..
                }//end of if WMA*

                if( (minorType == FILE_SOURCE_MN_TYPE_AC3) ||
                    (minorType == FILE_SOURCE_MN_TYPE_EAC3) ||
                    (minorType == FILE_SOURCE_MN_TYPE_EAC3_JOC) ) {
                    FileSourceConfigItem pitem;
                    status_t configStatus;
                    configStatus = m_pFileSourceWrapper->SetConfiguration(trackInfo.id, &pitem, FILE_SOURCE_MEDIA_OUTPUT_SINGLE_AUDIO_FRAME);

                    if( configStatus != FILE_SOURCE_SUCCESS ) {
                        LOGV("Setting AC3 config %d", configStatus );
                    }

                } else if((minorType == FILE_SOURCE_MN_TYPE_MP3) ||
                          (minorType == FILE_SOURCE_MN_TYPE_MP2) ||
                          (minorType == FILE_SOURCE_MN_TYPE_MP1)) {
                    FileSourceConfigItem pitem;
                    status_t configStatus;

                    // for audio only local playback usecase if big buffer size feature is enabled set Byte stream mode
                    if ((true == gbLocalPlayback) && mUseBigBuffers &&
                        ((audParserInpBufSize > 0) && (audParserInpBufSize <= MAX_PARSER_IN_BUF_SIZE)) &&
                        (FILE_SOURCE_MP3 == fileFormat)) {
                        configStatus = m_pFileSourceWrapper->SetConfiguration(trackInfo.id, &pitem, FILE_SOURCE_MEDIA_OUTPUT_BYTES_STREAM);
                        track->meta->setInt32(kKeyMaxInputSize, audParserInpBufSize);
                        track->meta->setInt32(kKeyIsByteMode, 1);
                    } else {
                        configStatus = m_pFileSourceWrapper->SetConfiguration(trackInfo.id, &pitem, FILE_SOURCE_MEDIA_OUTPUT_SINGLE_AUDIO_FRAME);
                    }

                    if( configStatus != FILE_SOURCE_SUCCESS ) {
                        LOGV("Setting MP3 config %d", configStatus );
                    }
                } else if(minorType == FILE_SOURCE_MN_TYPE_DTS) {
                    FileSourceConfigItem pitem;
                    status_t configStatus;

                    FS_AUDIO_PARAM_DTSTYPE dts_params;
                    pitem.nresult = (unsigned long long )&dts_params;
                    configStatus = m_pFileSourceWrapper->GetStreamParameter(trackInfo.id, FS_IndexParamAudioDTS, &dts_params);

                    if( configStatus != FILE_SOURCE_SUCCESS ) {
                            LOGE("Getting DTS StreamParameter %d", configStatus );
                    }
                    track->meta->setInt32(kKeyChannelCount, dts_params.usNumChannels);
                    track->meta->setInt32(kKeySampleRate, dts_params.ulSamplingRate);
                    if(dts_params.ulBitRate) {
                        track->meta->setInt32(kKeyBitRate, dts_params.ulBitRate);
                    }
#if 0 //! Disabled this as Audio team expecting same MIME type for all DTS Variants
                    if( dts_params.eSubType == FILE_SOURCE_SUB_MN_TYPE_DTS_LBR ) {
                            LOGE("reassign mimeType to  DTS-LBR esubtype=%d", dts_params.eSubType );
                            track->meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_DTS_LBR);
                        }
#endif
                    LOGV("MMParserExtractor::readMetaData DTS num_channels=%d,SR=%lu,BitRate=%lu,subType=%d\n",
                         dts_params.usNumChannels,dts_params.ulSamplingRate,
                        dts_params.ulBitRate,dts_params.eSubType);
                    configStatus = m_pFileSourceWrapper->SetConfiguration
                        (trackInfo.id, &pitem, FILE_SOURCE_MEDIA_OUTPUT_SINGLE_AUDIO_FRAME);
                    if( configStatus != FILE_SOURCE_SUCCESS ) {
                         LOGV("Erorr Setting DTS in frame mode config: %d", configStatus);
                    }

                } else if(FILE_SOURCE_MN_TYPE_G711_ALAW == minorType ||
                          FILE_SOURCE_MN_TYPE_PCM       == minorType ||
                          FILE_SOURCE_MN_TYPE_AIFF_PCM  == minorType ||
                          FILE_SOURCE_MN_TYPE_G711_MULAW== minorType ||
                          FILE_SOURCE_MN_TYPE_G721      == minorType ||
                          FILE_SOURCE_MN_TYPE_G723      == minorType ||
                          FILE_SOURCE_MN_TYPE_GSM_FR    == minorType ) {

                    WavFormatData wav_params;
                    bool status = m_pFileSourceWrapper->GetWavCodecData(trackInfo.id, &wav_params);
                    FileSourceConfigItem pitem;
                    status_t configStatus;
                    if ((24 == mediaTrackInfo.audioTrackInfo.nBitsPerSample) &&
                        (FILE_SOURCE_MN_TYPE_AIFF_PCM == minorType)) {
                        LOGV("AIFF-PCM at native bit width %d",24);
                        track->meta->setInt32(kKeySampleBits, 24);
                        track->meta->setInt32(kKeyPcmEncoding, getPcmEncoding((uint32)24));
                        configStatus = m_pFileSourceWrapper->SetConfiguration(
                                            trackInfo.id, &pitem,
                                            FILE_SOURCE_MEDIA_DISABLE_PCM_SAMPLE_UPGRADE);
                        if( configStatus != FILE_SOURCE_SUCCESS ) {
                            LOGE("Setting AIFF config failed %d", configStatus );
                        }
                    } else if((8 == mediaTrackInfo.audioTrackInfo.nBitsPerSample) &&
                       ((FILE_SOURCE_MN_TYPE_PCM == minorType)
                        || (FILE_SOURCE_MN_TYPE_AIFF_PCM == minorType))) {
                        LOGV("PCM codec Upgrade to 16bitsper sample");
                        track->meta->setInt32(kKeySampleBits, 16);
                        track->meta->setInt32(kKeyPcmEncoding, kAudioEncodingPcm16bit);
                        LOGV("MMParserExtractor::readMetaData, kKeySampleBits = %lu",
                         16);
                        configStatus = m_pFileSourceWrapper->SetConfiguration(
                                            trackInfo.id, &pitem,
                                            FILE_SOURCE_MEDIA_ENABLE_PCM_SAMPLE_UPGRADE);
                        if( configStatus != FILE_SOURCE_SUCCESS ) {
                            LOGV("Setting WAV config failed %d", configStatus );
                        }
                        size_t max_size =
                            m_pFileSourceWrapper->GetTrackMaxFrameBufferSize(trackInfo.id);
                        track->meta->setInt32(kKeyMaxInputSize, max_size);
                    }
//! Parser needs to provide data as it is to driver.
//! If any customer needs downgrade, then on need basis decision to be taken
#if 0
                    else if((24 == mediaTrackInfo.audioTrackInfo.nBitsPerSample) &&
                            (FILE_SOURCE_MN_TYPE_PCM == minorType)) {
                        LOGV("PCM codec Downgrade to 16bitsper sample");
                        configStatus = m_pFileSourceWrapper->SetConfiguration(
                                            trackInfo.id, &pitem,
                                            FILE_SOURCE_MEDIA_DISABLE_PCM_SAMPLE_UPGRADE);
                        if( configStatus != FILE_SOURCE_SUCCESS ) {
                            LOGV("Setting WAV config failed %d", configStatus );
                        }
                        //! Hard code to 16bits per sample
                        track->meta->setInt32(kKeySampleBits, 16);
                        track->meta->setInt32(kKeyPcmEncoding, getPcmEncoding((uint32)16));
                    }
#endif
                    else if((24 == mediaTrackInfo.audioTrackInfo.nBitsPerSample) &&
                            (FILE_SOURCE_MN_TYPE_PCM == minorType)) {
                        LOGV("AUDIO_FORMAT_PCM_24_BIT_PACKED is set for PCM codec");
                        track->meta->setInt32(kKeySampleBits, 24);
                        track->meta->setInt32(kKeyPcmEncoding, getPcmEncoding((uint32)24));
                    } else {
                        track->meta->setInt32(kKeyPcmEncoding, getPcmEncoding(mediaTrackInfo.audioTrackInfo.nBitsPerSample));
                    }

                    if( true == status ) {
                            track->meta->setInt32(kKeyChannelMask, wav_params.channel_mask);
                        }

                    if(FILE_SOURCE_MN_TYPE_G711_ALAW == minorType ||
                       FILE_SOURCE_MN_TYPE_G711_MULAW == minorType) {
                            track->meta->setInt32(kKeyMaxInputSize, 16384);
                    }

                } else if(FILE_SOURCE_MN_TYPE_GSM_AMR == minorType ||
                          FILE_SOURCE_MN_TYPE_AMR_WB  == minorType) {
                    FileSourceConfigItem pitem;
                    status_t configStatus;
                    configStatus = m_pFileSourceWrapper->SetConfiguration(trackInfo.id, &pitem, FILE_SOURCE_MEDIA_OUTPUT_SINGLE_AUDIO_FRAME);
                    if( configStatus != FILE_SOURCE_SUCCESS ) {
                        LOGV("Setting AMR config failed %d", configStatus );
                    }

                } else if(FILE_SOURCE_MN_TYPE_VORBIS == minorType) {
                    uint32 nFormatBlockSize = 0;
                    //Get the size of the format block.
                    status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, NULL, &nFormatBlockSize);
                    LOGV(" ogg getFormatBlock size = %u \n", nFormatBlockSize);

                    if((status==FILE_SOURCE_SUCCESS) && nFormatBlockSize!=0) {
                        uint8_t *buffer = new uint8_t[nFormatBlockSize];
                        uint32 ulIndex = 1;
                        while(buffer!=NULL && ulIndex < 4) {
                            uint32 ulBufSize = nFormatBlockSize;
                            FileSourceSampleInfo sampleInfo;
                            memset(&sampleInfo, 0, sizeof(FileSourceSampleInfo));
                            FileSourceMediaStatus mstatus = m_pFileSourceWrapper->GetNextMediaSample(trackInfo.id, buffer, &ulBufSize, sampleInfo);
                            if(mstatus==FILE_SOURCE_DATA_OK) {
                                if(1 == ulIndex) {
                                    track->meta->setData(kKeyVorbisInfo, 0, buffer, ulBufSize);
                                    LOGV(" kKeyVorbisInfo set, size %u \n", ulBufSize);
                                    if(ulBufSize > 12)
                                    {
                                      mediaTrackInfo.audioTrackInfo.numChannels = buffer[11];
                                      LOGV(" set vorbis channel from IdentificationHDR %d",  buffer[11]);
                                      track->meta->setInt32(kKeyChannelCount, mediaTrackInfo.audioTrackInfo.numChannels);
                                    }
                                } else if (2 == ulIndex) {
                                    track->meta->setData(kKeyVorbisData, 0, buffer, ulBufSize);
                                    LOGV(" kKeyVorbisData set, size %u \n", ulBufSize);
                                } else if (3 == ulIndex) {
                                    track->meta->setData(kKeyVorbisBooks, 0, buffer, ulBufSize);
                                    LOGV(" kKeyVorbisBooks set, size %u \n", ulBufSize);
                                }
                            } else {
                                LOGE(" getFormatBlock failed \n");
                            }
                            ulIndex++;
                        }
                        delete[] buffer;
                    }
                    FileSourceConfigItem pitem;
                    status_t configStatus =
                           m_pFileSourceWrapper->SetConfiguration(trackInfo.id,
                                                                        &pitem,
                                  FILE_SOURCE_MEDIA_OUTPUT_SINGLE_AUDIO_FRAME);
                    if( configStatus != FILE_SOURCE_SUCCESS ) {
                        LOGV("Setting OGG config failed %d", configStatus );
                    }
                    uint8 pBuf[12];
                    uint32 ulSize = 12;
                    FileSourceStatus eRet = m_pFileSourceWrapper->GetClipMetaData
                                            ((wchar_t*)pBuf, &ulSize,
                                             FILE_SOURCE_MD_ANDROID_LOOP);
                    if(FILE_SOURCE_SUCCESS == eRet) {
                        LOGV("android loop data is present, %s", pBuf);
                        if(!strncmp("true", (char*)pBuf, 4)) {
                          mFileMetaData->setInt32(kKeyAutoLoop, true);
                          LOGV("kkeyauto loop set to true");
                        }
                    }
                } else if(FILE_SOURCE_MN_TYPE_FLAC == minorType) {
                    LOGV(" Codec type is FLAC");
                    status_t configStatus;
                    FlacFormatData flacParams;
                    FileSourceConfigItem pitem;
                    if(true == gbLocalPlayback)
                      configStatus =
                           m_pFileSourceWrapper->SetConfiguration(trackInfo.id,
                                                                        &pitem,
                                        FILE_SOURCE_MEDIA_OUTPUT_BYTES_STREAM);
                    else
                      configStatus =
                           m_pFileSourceWrapper->SetConfiguration(trackInfo.id,
                                                                        &pitem,
                                  FILE_SOURCE_MEDIA_OUTPUT_SINGLE_AUDIO_FRAME);

                    if( configStatus != FILE_SOURCE_SUCCESS ) {
                        LOGV("Setting FLAC config failed %d", configStatus );
                    }
                    configStatus = m_pFileSourceWrapper->GetStreamParameter(trackInfo.id,
                                                                            FS_IndexParamAudioFlac,
                                                                            &flacParams);
                    if(FILE_SOURCE_SUCCESS == configStatus) {
                        track->meta->setInt32(kKeyMinBlkSize, flacParams.nMinBlockSize);
                        track->meta->setInt32(kKeyMaxBlkSize, flacParams.nMaxBlockSize);
                        track->meta->setInt32(kKeyMinFrmSize, flacParams.nMinFrameSize);
                        track->meta->setInt32(kKeyMaxFrmSize, flacParams.nMaxFrameSize);
                    }
                    else {
                        LOGE("FLAC Codec data read failed");
                    }
                }
                else if(FILE_SOURCE_MN_TYPE_OPUS == minorType)
                {
                  uint32 nFormatBlockSize = 0;
                  status_t configStatus;
                  FS_AUDIO_PARAM_OPUSTYPE sOpus_params = {0};
                  status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, NULL, &nFormatBlockSize, 1);
                  LOGV(" OPUS getFormatBlock size = %u \n", nFormatBlockSize);
                  if((status==FILE_SOURCE_SUCCESS) && nFormatBlockSize!=0) {
                    uint8_t *buffer = new uint8_t[nFormatBlockSize];
                    if(buffer!=NULL) {
                      status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, buffer, &nFormatBlockSize, 1);
                      if(status==FILE_SOURCE_SUCCESS)
                        track->meta->setData(kKeyOpusHeader, /*MetaData::TYPE_NONE*/0, buffer, nFormatBlockSize);
                    }
                  }
                  //! Get OPUS audio specific stream parameter.
                  configStatus = m_pFileSourceWrapper->GetStreamParameter(trackInfo.id, FS_IndexParamAudioOPUS, &sOpus_params);
                  if( configStatus != FILE_SOURCE_SUCCESS ) {
                     LOGE("Getting OPUS StreamParameter %d", status );
                  }
                  track->meta->setInt64(kKeyOpusCodecDelay, sOpus_params.ullCodecDelay);
                  track->meta->setInt64(kKeyOpusSeekPreRoll, sOpus_params.ullSeekPreRoll);
                  LOGV("OPUS nCodecDelay %lu,nSeekPrerollDelay %llu",sOpus_params.ullCodecDelay,sOpus_params.ullSeekPreRoll);
                }
                else if(FILE_SOURCE_MN_TYPE_ALAC == minorType) {
                    uint32 nFormatBlockSize = 0;
                    //Get the size of the format block.
                    status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, NULL, &nFormatBlockSize);
                    LOGV(" ALAC getFormatBlock size = %u \n", nFormatBlockSize);
                    if((status==FILE_SOURCE_SUCCESS) && nFormatBlockSize!=0) {
                        uint8_t *buffer = new uint8_t[nFormatBlockSize];
                        if(buffer!=NULL) {
                            status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, buffer, &nFormatBlockSize);
                            if(status==FILE_SOURCE_SUCCESS) {
                                track->meta->setData(kKeyRawCodecSpecificData, MetaData::TYPE_NONE, buffer, nFormatBlockSize);
                            }
                        }
                    }
                } else if(FILE_SOURCE_MN_TYPE_APE == minorType) {
                    uint32 nFormatBlockSize = 0;
                    //Get the size of the format block.
                    status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, NULL, &nFormatBlockSize);
                    LOGE("APE getFormatBlock size = %lu \n", nFormatBlockSize);
                    if((status==FILE_SOURCE_SUCCESS) && nFormatBlockSize!=0) {
                        uint8_t *buffer = new uint8_t[nFormatBlockSize];
                        if(buffer!=NULL) {
                            status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, buffer, &nFormatBlockSize);
                            if(status==FILE_SOURCE_SUCCESS) {
                                track->meta->setData(kKeyRawCodecSpecificData, MetaData::TYPE_NONE, buffer, nFormatBlockSize);
                            }
                        }
                    }
                }else if(FILE_SOURCE_MN_TYPE_DSD == minorType ) {
                    FileSourceConfigItem pitem;
                    status_t configStatus;
                    configStatus = m_pFileSourceWrapper->SetConfiguration(trackInfo.id, &pitem, FILE_SOURCE_MEDIA_OUTPUT_BYTES_STREAM);
                    if( configStatus != FILE_SOURCE_SUCCESS ) {
                        LOGV("Setting DSD config failed %d", configStatus );
                    }

                }
                //! Call Seek to ZERO API. Parser will set
                //! data pointers to start if it's not in proper state
                m_pFileSourceWrapper->SeekAbsolutePosition(0);

            } else if (majorType == FILE_SOURCE_MJ_TYPE_VIDEO) {
                FileSourceVideoInfo sVideoMediaInfo;
                memcpy(&sVideoMediaInfo, &(mediaTrackInfo.videoTrackInfo), \
                       sizeof(sVideoMediaInfo));
                char ucPropValue[PROPERTY_VALUE_MAX] = {(char)0};
                int32 nBufSize;
                property_get("vendor.video.buffer.size", ucPropValue, "0");
                nBufSize = atoi(ucPropValue);
                if(nBufSize > 0)
                {
                 sVideoMediaInfo.ulMaxBufSize = \
                     (uint32)(sVideoMediaInfo.frameWidth * \
                              sVideoMediaInfo.frameHeight * 1.5);
                  m_pFileSourceWrapper->SetStreamParameter( trackInfo.id, (uint32)FS_IndexParamOtherMediaTrackInfo, &sVideoMediaInfo);
                }
                size_t max_size = m_pFileSourceWrapper->GetTrackMaxFrameBufferSize(trackInfo.id);
                track->meta->setInt32(kKeyMaxInputSize, max_size);
                mVideoTrackPresent = true;
                // video
                track->meta->setInt32(kKeyWidth, mediaTrackInfo.videoTrackInfo.frameWidth);
                track->meta->setInt32(kKeyHeight, mediaTrackInfo.videoTrackInfo.frameHeight);
                if (mediaTrackInfo.videoTrackInfo.ulDisplayWidth &&
                    mediaTrackInfo.videoTrackInfo.ulDisplayHeight)
                {
                  track->meta->setInt32(kKeyDisplayWidth,
                                        mediaTrackInfo.videoTrackInfo.ulDisplayWidth);
                  track->meta->setInt32(kKeyDisplayHeight,
                                        mediaTrackInfo.videoTrackInfo.ulDisplayHeight);
                }
                // set rotation degree if container provide information
                track->meta->setInt32(kKeyRotation, mediaTrackInfo.videoTrackInfo.ulRotationDegrees);
                track->meta->setCString(kKeyMediaLanguage,(const char*)mediaTrackInfo.videoTrackInfo.ucLanguageCode);
                track->timescale = mediaTrackInfo.videoTrackInfo.timeScale;
                LOGV("MMParserExtractor::readMetaData  Video track metadata frameWidth = %lu frameHeight = %lu timeScale = %lu duration = %llu\n",
                        mediaTrackInfo.videoTrackInfo.frameWidth, mediaTrackInfo.videoTrackInfo.frameHeight,
                        mediaTrackInfo.videoTrackInfo.timeScale,  mediaTrackInfo.videoTrackInfo.duration);

                int64 VideoDuration = mediaTrackInfo.videoTrackInfo.duration;
                uint32 ulTimeScale  = mediaTrackInfo.videoTrackInfo.timeScale;
                FileSourceFileFormat format = FILE_SOURCE_UNKNOWN;
                m_pFileSourceWrapper->GetFileFormat(format);
                if (format == FILE_SOURCE_3G2) { //File is fragmented
                    VideoDuration = m_pFileSourceWrapper->GetClipDuration();
                    if (VideoDuration <= 0)
                        VideoDuration = mediaTrackInfo.videoTrackInfo.duration;
                }
                MM_MSG_PRIO2(MM_FILE_OPS, MM_PRIO_MEDIUM,"captured frame rate is %f and calc frame rate is %f",
                    mediaTrackInfo.videoTrackInfo.capturedFrameRate, mediaTrackInfo.videoTrackInfo.frameRate);
                mFileMetaData->setFloat(kKeyCaptureFramerate,
                                        mediaTrackInfo.videoTrackInfo.capturedFrameRate);

                track->meta->setInt32(kKeyFrameRate,
                                       (int32_t)(mediaTrackInfo.videoTrackInfo.frameRate));

                LOGV("MMParserExtractor::readMetaData Video track videoDuration = %u %u",
                        (unsigned int)(VideoDuration>>32), (unsigned int)(VideoDuration&0xFFFFFFFF));

                int64 kKeyDurationValue = VideoDuration;
                if(ulTimeScale && ulTimeScale != 1000000) {
                    kKeyDurationValue = VideoDuration * (1000000 / ulTimeScale);
                }
                track->meta->setInt64(kKeyDuration,kKeyDurationValue);

                LOGV("MMParserExtractor::readMetaData, KkeyDuration =  %u %u  \n",
                        (unsigned int)(kKeyDurationValue>>32), (unsigned int)(kKeyDurationValue&0xFFFFFFFF));

                if (mSetBuffers) {
                    if (FILE_SOURCE_MN_TYPE_H264 == minorType ||
                        FILE_SOURCE_MN_TYPE_HEVC == minorType) {
                        MM_MSG_PRIO(MM_FILE_OPS, MM_PRIO_HIGH,
                        "MM-Parser: AAL: found h264/hevc format, enable setBuffers mode");
                        track->meta->setInt32(kKeyEnableSetBuffers, 1);
                    } else {
                        MM_MSG_PRIO(MM_FILE_OPS, MM_PRIO_HIGH,
                        "MM-Parser: AAL: disable setBuffers mode");
                        mSetBuffers = false;
                        mFileMetaData->setInt32(kKeyEnableSetBuffers, 0);
                    }
                }

                uint32 nFormatBlockSize = 0;
                bool bRawCodecData = false;
                //! AVCC and HVCC is available only in MKV and MP4 file formats.
                //! Provide that data as it is to upper layer.
                if((FILE_SOURCE_3G2 == format || FILE_SOURCE_MPEG4 == format
                    || FILE_SOURCE_MKV == format) &&
                   ((FILE_SOURCE_MN_TYPE_H264 == minorType) ||
                    (FILE_SOURCE_MN_TYPE_HEVC == minorType)) ){
                    bRawCodecData = true;
                }
                //Get the size of the format block.
                status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, NULL, &nFormatBlockSize, bRawCodecData);
                LOGV(" getFormatBlock size = %u \n", nFormatBlockSize);

                if((status==FILE_SOURCE_SUCCESS) && nFormatBlockSize!=0) {
                    uint8_t *buffer = new uint8_t[nFormatBlockSize];
                    if(buffer!=NULL) {
                        status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, buffer, &nFormatBlockSize, bRawCodecData);
                        if(status==FILE_SOURCE_SUCCESS) {
                            sp<MetaData> meta;
                            if (!mSetBuffers) {
                                meta = track->meta;
                            } else{
                                meta = new MetaData(*(track->meta.get()));
                            }

                            if (false == bRawCodecData) {
                                meta->setData(kKeyRawCodecSpecificData, MetaData::TYPE_NONE, buffer, nFormatBlockSize);
                            } else if(FILE_SOURCE_MN_TYPE_H264 == minorType) {
                                meta->setData(kKeyAVCC, kTypeAVCC, buffer, nFormatBlockSize);
                            } else if(FILE_SOURCE_MN_TYPE_HEVC == minorType) {
                                meta->setData(kKeyHVCC, kTypeHVCC, buffer, nFormatBlockSize);
                            } else if(FILE_SOURCE_MN_TYPE_VP9 == minorType) {
                                LOGV("getFormatBlock size VP9 = %u \n", nFormatBlockSize);
                                meta->setData(kKeyVp9CodecPrivate, 0, buffer, nFormatBlockSize);
                            }
                            if (mSetBuffers) {
                                sp<AMessage> msg = new AMessage();
                                if(convertMetaDataToMessage(meta, &msg) == OK) {
                                    size_t i = 0;
                                    for (;;) {
                                        sp<ABuffer> csd;
                                        if (!msg->findBuffer(AStringPrintf("csd-%u", i).c_str(), &csd)) {
                                            break;
                                        }
                                        if ((mFormatBlock->size() + csd->size()) > mFormatBlock->capacity()) {
                                            sp<ABuffer> tmpBuffer = new ABuffer(mFormatBlock->capacity() + 1024);
                                            memcpy(tmpBuffer->data(), mFormatBlock->data(), mFormatBlock->size());
                                            tmpBuffer->setRange(0, mFormatBlock->size());
                                            mFormatBlock = tmpBuffer;
                                        }
                                        memcpy(mFormatBlock->data() + mFormatBlock->size(), csd->data(), csd->size());
                                        mFormatBlock->setRange(0, mFormatBlock->size() + csd->size());
                                        ++i;
                                    }
                                    sp<ABuffer> extendedCSD;
                                    msg->findBuffer("raw-codec-specific-data", &extendedCSD);
                                    if (extendedCSD != NULL) {
                                        if ((mFormatBlock->size() + extendedCSD->size()) > mFormatBlock->capacity()) {
                                            sp<ABuffer> tmpBuffer = new ABuffer(mFormatBlock->capacity() + 1024);
                                            memcpy(tmpBuffer->data(), mFormatBlock->data(), mFormatBlock->size());
                                            tmpBuffer->setRange(0, mFormatBlock->size());
                                            mFormatBlock = tmpBuffer;
                                        }
                                        memcpy(mFormatBlock->data() + mFormatBlock->size(), extendedCSD->data(), extendedCSD->size());
                                        mFormatBlock->setRange(0, mFormatBlock->size() + extendedCSD->size());
                                    }
                                }
                                LOGV("Total size of csd is %zu", mFormatBlock->size());
                            }
                        } else {
                            LOGE(" getFormatBlock failed \n");
                        }
                    }
                    delete[] buffer;
                }
                if ((FILE_SOURCE_MKV == format) ||
                    (FILE_SOURCE_MPEG4 == format) ||
                    (FILE_SOURCE_3G2 == format)) {
                  status_t configStatus = FillColorInformation(track, format);
                }

                video::iStreamPort::DataSourceType eSourceType;
                m_pSourcePort->GetSourceType(&eSourceType);

                //Pick a suitable timestamp for thumbnail
                //but we shouldn't be showing thumbnail for DRMed
                //clips so just set flag and return
                if (m_pFileSourceWrapper->IsDrmProtection()) {
                    track->meta->setInt32(kKeyIsDRM, true);
                } else if(video::iStreamPort::DS_STREAMING_SOURCE != eSourceType &&
                          !bThumbNail){
                    uint64 DurationinMS = (mediaTrackInfo.videoTrackInfo.duration * 1000) /
                                          mediaTrackInfo.videoTrackInfo.timeScale;
                    //! Find sync frames within 40% of duration
                    //! Calculate delta based on total num of sync frames required
                    //! In this case, that value is 5
                    uint64 Delta = (uint64)(DurationinMS * 10) /
                                           (NUM_SYNC_FRAMES * 100);
                    LOGV("Delta value is : %llu", Delta);
                    //We need to pick a suitable iframe for thumbnail generation
                    int64_t thumbnailTimestampUs = 0;
                    uint32 maxSyncSampleSize = 0;
                    if (!m_pFileSourceWrapper->IsSeekDenied()) {
                        uint32 buffLen = m_pFileSourceWrapper->GetTrackMaxFrameBufferSize(trackInfo.id);
                        void *buff = malloc(buffLen);
                        FileSourceSampleInfo sampleInfo;
                        memset(&sampleInfo, 0, sizeof(FileSourceSampleInfo));
                        bThumbNail = true;
                        uint64 lastSyncSampleTime_real = 0;

                        for (uint64 lastSyncSampleTime = 0, syncSample = 0;
                             syncSample < NUM_SYNC_FRAMES && buff != NULL; ++syncSample) {
                            if( ((fileFormat == FILE_SOURCE_MP2TS)||
                                  (fileFormat == FILE_SOURCE_MKV)||
                                  (fileFormat == FILE_SOURCE_ASF)) && syncSample)
                            {
                              thumbnailTimestampUs = lastSyncSampleTime_real * 1000;
                              break;
                            }
                            //! Asf, find 1 sync frame at 0 ts
                            //! MKV, find 1 sync frame at 5% duration
                            if(fileFormat == FILE_SOURCE_ASF){
                                 lastSyncSampleTime = 0;
                            }
                            else if(fileFormat == FILE_SOURCE_MKV){
                                lastSyncSampleTime = DurationinMS/20;
                            }
                            uint32 bytesRead = 0;
                            FileSourceMediaStatus status;

                            //seek to next sync sample
                            FileSourceStatus stat = m_pFileSourceWrapper->SeekAbsolutePosition
                                    (trackInfo.id, lastSyncSampleTime, true,
                                     lastSyncSampleTime);
                            if (FILE_SOURCE_SUCCESS != stat){
                                LOGE("SeekAbsolutePosition returns err = %d\n", stat);
                                break;
                            }
                            lastSyncSampleTime_real = lastSyncSampleTime;
                            bytesRead = buffLen; //GetNextMediaSample will set the actual bytes read
                            status = m_pFileSourceWrapper->GetNextMediaSample(trackInfo.id,
                                                                              (uint8 *)buff,
                                                                              &bytesRead,
                                                                              sampleInfo);
                            /* Increase Sync Sample time by 1ms,
                               This is required to do seek for next sync frame.
                               Sample time is in micro-sec untis, convert to milli-sec*/
                           lastSyncSampleTime = (sampleInfo.startTime)/1000 + Delta;

                            if (FILE_SOURCE_DATA_OK != status || bytesRead <= 0){
                                //Some error in reading, file is probably screwed up
                                //lets not bother parsing further
                                break;
                            } else if (bytesRead >= maxSyncSampleSize
                                    && sampleInfo.bStartTsValid) {
                                thumbnailTimestampUs = sampleInfo.startTime;
                                maxSyncSampleSize = bytesRead;
                            }
                        }//for(lastSyncSampleTime)
                        if(buff){
                          free(buff);
                        }

                    }
                    m_pFileSourceWrapper->SeekAbsolutePosition(0); //reposition back to 0
                    track->meta->setInt64(kKeyThumbnailTime, thumbnailTimestampUs);
                    LOGV("thumbnail time is %llu, max sync sample size %lu",
                         thumbnailTimestampUs, maxSyncSampleSize);
                }
            }
            else if (majorType == FILE_SOURCE_MJ_TYPE_TEXT) {
                size_t max_size = m_pFileSourceWrapper->GetTrackMaxFrameBufferSize(trackInfo.id);
                track->meta->setInt32(kKeyMaxInputSize, max_size);
                // Update Duration and timescale values
                track->timescale = mediaTrackInfo.textTrackInfo.timeScale;
                int64 TextDuration =
                    mediaTrackInfo.textTrackInfo.duration;
                FileSourceFileFormat format = FILE_SOURCE_UNKNOWN;
                m_pFileSourceWrapper->GetFileFormat(format);
                if (format == FILE_SOURCE_3G2) { //File is fragmented
                    int64 TotalDur = m_pFileSourceWrapper->GetClipDuration();
                    if (TotalDur > TextDuration)
                        TextDuration = TotalDur;
                }

                LOGV("MMParserExtractor::readMetaData text track Duration = %u %u",
                     (unsigned int)(TextDuration>>32),
                     (unsigned int)(TextDuration&0xFFFFFFFF));

                int64 kKeyDurationValue = TextDuration;
                if(track->timescale && track->timescale != 1000000) {
                   kKeyDurationValue = (TextDuration * 1000000) /
                                        track->timescale;
                }
                track->meta->setInt64(kKeyDuration,kKeyDurationValue);
                track->meta->setCString(kKeyMediaLanguage,(const char*)mediaTrackInfo.textTrackInfo.ucLanguageCode);
                LOGV("MMParserExtractor::readMetaData, KkeyDuration =  %u %u  \n",
                     (unsigned int)(kKeyDurationValue>>32),
                     (unsigned int)(kKeyDurationValue&0xFFFFFFFF));

                if (FILE_SOURCE_MN_TYPE_TIMED_TEXT == minorType) {
                    track->meta->setCString(kKeyMIMEType,
                                            MEDIA_MIMETYPE_TEXT_3GPP);
                }
                uint32 nFormatBlockSize = 0;
                //Get the size of the format block.
                status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, NULL, &nFormatBlockSize);
                LOGV(" TextTrack: getFormatBlock size = %u \n", nFormatBlockSize);

                if((status==FILE_SOURCE_SUCCESS) && nFormatBlockSize!=0) {
                    // Check if already format block provided or not
                    // For text track, it is possible to have multiple codecData
                    uint32_t type;
                    const void *data;
                    size_t nPrevMetaDataSize = 0;
                    if (!track->meta->findData(kKeyTextFormatData, &type,
                                               &data, &nPrevMetaDataSize)) {
                        nPrevMetaDataSize = 0;
                    }

                    uint8_t *buffer = new uint8_t[nPrevMetaDataSize +
                                                  nFormatBlockSize];

                    if(buffer!=NULL) {
                        if (nPrevMetaDataSize > 0) {
                            memcpy(buffer, data, nPrevMetaDataSize);
                        }
                        status = m_pFileSourceWrapper->GetFormatBlock(
                                                trackInfo.id,
                                                buffer + nPrevMetaDataSize,
                                                &nFormatBlockSize);
                        if(status==FILE_SOURCE_SUCCESS) {
                            track->meta->setData(kKeyTextFormatData,
                                                 MetaData::TYPE_NONE,
                                                 buffer,
                                                 nFormatBlockSize+ nPrevMetaDataSize);
                        } else {
                            LOGE(" getFormatBlock failed \n");
                        }
                    }
                    delete[] buffer;
                }
            }
            else if (majorType == FILE_SOURCE_MJ_TYPE_META)
            {
                if (minorType == FILE_SOURCE_MN_TYPE_META_TEXT) {
                    uint32 nFormatBlockSize = 0;
                    status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id, NULL,
                                                                  &nFormatBlockSize);
                    if (status == FILE_SOURCE_SUCCESS && nFormatBlockSize != 0) {
                        uint8_t *buffer = new uint8_t[nFormatBlockSize];
                        if (buffer != NULL) {
                            status = m_pFileSourceWrapper->GetFormatBlock(trackInfo.id,
                                                                          buffer,
                                                                          &nFormatBlockSize);
                            if (status==FILE_SOURCE_SUCCESS) {
                                track->meta->setCString(kKeyMIMEType, (const char *)buffer);
                                track->timescale = mediaTrackInfo.sMetaTrackInfo.ulTimeScale;
                                int64 kKeyDurationValue = mediaTrackInfo.sMetaTrackInfo.ullDuration;
                                if (track->timescale && track->timescale != 1000000) {
                                    kKeyDurationValue = kKeyDurationValue * \
                                                        (1000000 / track->timescale);
                                }
                                track->meta->setInt64(kKeyDuration,kKeyDurationValue);
                            }
                            delete[] buffer;
                        }
                    }
                }
            }
        }
    }//for loop

    const char* pFileType = fillMimeType(fileFormat,mVideoTrackPresent);

    if(NULL != pFileType) {
        mFileMetaData->setCString(kKeyMIMEType, pFileType);
        LOGV("fillMimeType:: %s",(char*)pFileType);
    }

    setAlbumArtRelatedMeta();
    //! Check for AlbumArt info for all audio only clips
    if(!mVideoTrackPresent) {
        setAlbumArt();
    }

    dumpMetadataToLog();

    mHaveMetadata = OK;
    return OK;
}

void MMParserExtractor::dumpMetadataToLog() {
    if(m_sDebug.bDumpMetaToLog) {
        LOGI("[METADUMP] Start");
        LOGI("[METADUMP] FileMeta");
        if (mFileMetaData != NULL)
            mFileMetaData->dumpToLog();

        int count = 0;
        Track* currentTrack = mFirstTrack;
        while (currentTrack != NULL && currentTrack->meta != NULL) {
            LOGI("[METADUMP] Track %d",++count);
            currentTrack->meta->dumpToLog();
            currentTrack = currentTrack->next;
        }
        LOGI("[METADUMP] End");
    }
}
sp<IMediaSource> MMParserExtractor::getTrack(size_t index) {
    LOGV(" MMParserExtractor::getTrack : index = %zu\n", index);
    status_t err;
    if ((err = readMetaData()) != OK) {
        return NULL;
    }

    Track *track = mFirstTrack;
    while (index > 0) {
        if (track == NULL) {
            return NULL;
        }
        track = track->next;
        --index;
    }

    return new MMParserSource(track->meta,
                         this,
                         m_pFileSourceWrapper,
                         track->trackId,track->timescale);
}

uint32_t MMParserExtractor::flags() const {
    LOGV("MMParserExtractor::flags");
    if(m_pFileSourceWrapper==NULL) {
        LOGE("MMParserExtractor::flags problem m_pFileSourceWrapper not yet inited - say we support nothing");
        return 0;
    }
    const uint8 seekDenied = m_pFileSourceWrapper->IsSeekDenied();
    if(seekDenied) {
        LOGE("Note - seek not supported for this clip");
        return CAN_PAUSE;
    }

    if (mIsAacformatAdif == true) {
        return (CAN_SEEK_TO_ZERO | CAN_PAUSE);
    } else {
        return (CAN_SEEK | CAN_SEEK_BACKWARD | CAN_SEEK_FORWARD | CAN_PAUSE);
    }
}

//set flag before calling getMetaData/getTrack
void MMParserExtractor::setExtraFlags(uint32_t flag) {
    LOGD("setExtraFlags called with flags %x for paser instance %x"
        , flag, this);
    if (flag & ENABLE_SET_BUFFERS) {
        mSetBuffers = true;
        char disbleSetBuffers[PROPERTY_VALUE_MAX] = {0};
        property_get("parser.disable.set.buffers", disbleSetBuffers, "1");
        if (!strncasecmp(disbleSetBuffers, "true", 4) || atoi(disbleSetBuffers)) {
            MM_MSG_PRIO(MM_FILE_OPS, MM_PRIO_HIGH,
                        "Set buffers is disabled by property");
            mSetBuffers = false;
        }

        if (mSetBuffers) {
            ALOGI("Enable set buffers mode");
            mFileMetaData->setInt32(kKeyEnableSetBuffers, 1);
        }
    }

    if (flag & ENABLE_AUDIO_BIG_BUFFERS) {
        mUseBigBuffers = true;
        LOGD("using bigger parser buffers");
    }
    return;
}
////////////////////////////////////////////////////////////////////////////////

MMParserSource::MMParserSource(
        const sp<MetaData> &format,
        const sp<MMParserExtractor> &extractor,
        FileSourceWrapper *fileSourceWrapper,
        uint32_t trackId,int32_t timeScale)
    : mFormat(format),
      mMMParserExtractor(extractor),
      mTimescale(timeScale),
      mTrackId(trackId),
      m_pFileSourceWrapper(fileSourceWrapper),
      mCurrentSampleIndex(0),
      mCurrentTimeMS(0),
      mStarted(false),
      mGroup(NULL),
      mBuffer(NULL){

    LOGV(" MMParserSource constructor ===>\n");
    const char *mime;
    bool success = mFormat->findCString(kKeyMIMEType, &mime);
    CHECK(success);
    LOGV(" mime = %s Timescale = %d\n", mime, mTimescale);

    mSetBuffers = false;
    int32_t setBuffers = 0;
    if (mFormat->findInt32(kKeyEnableSetBuffers, &setBuffers) && setBuffers) {
        ALOGI("MMParserSource parser supports set buffers");
        mSetBuffers = true;
    }

    mFirstSample = true;
    mFile = NULL;
}

MMParserSource::~MMParserSource() {
    if (mStarted) {
        stop();
    }
    if (mFile != NULL) {
        fclose(mFile);
        mFile = NULL;
    }
}

status_t MMParserSource::start(MetaData *params) {
    CHECK(!mStarted);

    LOGV(" MMParserSource::start ===>  \n");

    mGroup = new MediaBufferGroup;
    FileSourceMjMediaType majorType = FILE_SOURCE_MJ_TYPE_UNKNOWN;
    FileSourceMnMediaType minorType = FILE_SOURCE_MN_TYPE_UNKNOWN;
    m_pFileSourceWrapper->GetMimeType(mTrackId, majorType, minorType);
    if(majorType == FILE_SOURCE_MJ_TYPE_VIDEO)
      m_pFileSourceWrapper->mVideoMinorType = minorType;

    // Get max buffer size
    size_t max_size = m_pFileSourceWrapper->GetTrackMaxFrameBufferSize(mTrackId);
    LOGV(" Max frame size found = %zu \n", max_size);
    if( minorType == FILE_SOURCE_MN_TYPE_AAC_ADIF ) {
        //Parser does know frame boundaries for ADIF
        // using twice of default size 1536 to accomodate incomplete frames
        max_size = 1536*2;
    }
    if(FILE_SOURCE_MN_TYPE_G711_ALAW == minorType ||
       FILE_SOURCE_MN_TYPE_G711_MULAW == minorType) {
      int32_t ulSize = 0;
      if (mFormat->findInt32(kKeyMaxInputSize, &ulSize) && ulSize) {
        LOGV("MMParserSource updated max size to %d", ulSize);
        max_size = ulSize;
      }
    }

    // in audio only local playback usecase use big buffer, only if big buffer is > buffer size calculated
     // only for few formats
    FileSourceFileFormat fileFormat = FILE_SOURCE_UNKNOWN;
    m_pFileSourceWrapper->GetFileFormat(fileFormat);
    m_pFileSourceWrapper->mFileFormat = fileFormat;
    if(((true == gbLocalPlayback) &&
        (fileFormat == FILE_SOURCE_AAC) && (minorType != FILE_SOURCE_MN_TYPE_AAC_ADIF)) ||
        (fileFormat == FILE_SOURCE_MP3)) {
        // if big buffer mode enabled, override default buffer size
        char audioParserBuf[PROPERTY_VALUE_MAX];
        property_get("vendor.audio.parser.ip.buffer.size", audioParserBuf, "0");
        uint32_t audParserInpBufSize = atoi(audioParserBuf);
        if(mMMParserExtractor->mUseBigBuffers &&
            (audParserInpBufSize > 0) && (audParserInpBufSize <= MAX_PARSER_IN_BUF_SIZE)) {
            max_size = audParserInpBufSize;
            LOGD("Overiding Parser out buffer size  = %zu \n", max_size);
        } else {
            LOGD("Using default parser out buffer size  = %zu \n", max_size);
        }
    }

    // Assume that a given buffer only contains at most 10 fragments,
    // each fragment originally prefixed with a 2 byte length will
    // have a 4 byte header (0x00 0x00 0x00 0x01) after conversion,
    // and thus will grow by 2 bytes per fragment.
    // TODO we may only need max_size
    //mGroup->add_buffer(new MediaBuffer(max_size + 10 * 2));

    //mGroup->add_buffer(new MediaBuffer(max_size+4));
    const char *mime;
    int32_t decodedWidth =0;
    int32_t decodedHeight =0;
    mFormat->findCString(kKeyMIMEType, &mime);
    if (!strncasecmp(mime, "video/", 6)) {
        mFormat->findInt32(kKeyWidth, &decodedWidth);
        mFormat->findInt32(kKeyHeight, &decodedHeight);
        if (!mSetBuffers) {
            mGroup->add_buffer(new MediaBuffer(max_size + 10 *2 ));
        }
        LOGV("MMParserSource::start decodedWidth = %d, decodedHeight =%d",decodedWidth,decodedHeight);
        LOGV(" MMParserSource::start Allocation buffer  for video tracksize =max_size + 10*2\n" );
    } else {
        mGroup->add_buffer(new MediaBuffer(max_size));
        LOGV("MMParserSource::start Allocation buffer size for audio track= %zu \n", max_size);
    }

    mStarted = true;

    LOGV(" MMParserSource::start <===  \n");
    return OK;
}

status_t MMParserSource::stop() {
    CHECK(mStarted);

    LOGV(" MMParserSource::stop\n");

    if (mBuffer != NULL) {
        mBuffer->release();
        mBuffer = NULL;
    }

    delete mGroup;
    mGroup = NULL;

    mStarted = false;
    mCurrentSampleIndex = 0;

    return OK;
}

sp<MetaData> MMParserSource::getFormat() {
    LOGV(" MMParserSource::getFormat\n");
    return mFormat;
}

status_t MMParserSource::read(
        MediaBuffer **out, const ReadOptions *options) {
    CHECK(mStarted);
    //LOGE(" MMParserSource::read ===>  \n");
    *out = NULL;
    int64_t seekTimeUs;
    ReadOptions::SeekMode seekMode;
    FS_SEEK_MODE m_eSeekMode = FS_SEEK_DEFAULT;
    FS_SEEK_INFOTYPE seekInfo = {0};
    seekInfo.targetTimeUs = -1;

    if (options && options->getSeekTo(&seekTimeUs, &seekMode)) {
        uint32_t seekTimems = 0;
        LOGV("MMParserSource::read Seek seekTimeUs = %ld, seekMode %d",seekTimeUs,seekMode);
        //! Seektime needs to be in milli-sec units, Convert from Us to Ms.
        seekTimems = (seekTimeUs / 1000);
        switch(seekMode)
        {
            case ReadOptions::SEEK_PREVIOUS_SYNC:
                m_eSeekMode = FS_SEEK_PREV;
                break;
            case ReadOptions::SEEK_NEXT_SYNC:
                m_eSeekMode = FS_SEEK_NEXT;
                break;
            case ReadOptions::SEEK_CLOSEST_SYNC:
                m_eSeekMode = FS_SEEK_CLOSEST;
                break;
            case ReadOptions::SEEK_CLOSEST:
                m_eSeekMode = FS_SEEK_CLOSEST_NOSYNC;
                break;
            default:
                break;
        }
        LOGV("MMParserSource::read Seek seekTimems = %u, mTrackId = %d", seekTimems,mTrackId);
        FileSourceStatus stat = m_pFileSourceWrapper->SeekAbsolutePosition((int64)mTrackId,
                                        seekTimems, true, mCurrentTimeMS,m_eSeekMode);
        if (mBuffer != NULL) {
            mBuffer->release();
            mBuffer = NULL;
        }

        if (FILE_SOURCE_SUCCESS != stat) {
            LOGE("SeekAbsolutePosition returns err = %d", stat);
            return ERROR_END_OF_STREAM;
        }

        LOGV("FileSource SeekAbsolutePosition success");
        if (mSetBuffers) {
            mFirstSample = true;
        }

        if (m_eSeekMode == FS_SEEK_CLOSEST_NOSYNC) {
          if (FILE_SOURCE_SUCCESS != m_pFileSourceWrapper->GetStreamParameter((int64)mTrackId,
                                                                   FS_IndexParamOtherSeekInfo,
                                                                   &seekInfo)) {
              seekInfo.targetTimeUs = -1;
          }
        }
        // fall through
    }

    bool newBuffer = false;
    if (mBuffer == NULL) {
        newBuffer = true;
        status_t err = mGroup->acquire_buffer(&mBuffer,
                              options != NULL && options->getNonBlocking());
        if (err != OK) {
            CHECK(mBuffer == NULL);
            return err;
        }
    }

    if (newBuffer && mBuffer) {
        size_t size = mBuffer->size();
        uint8_t *data = (uint8_t *)mBuffer->data();
        const char *mime;
        FileSourceSampleInfo sInfo = {0};
        sp<ABuffer> setBuffSEI;
        int32 nSeiCount = 0;

        mFormat->findCString(kKeyMIMEType, &mime);
        bool writeCSD = mSetBuffers && mFirstSample && (!strncasecmp(mime, "video/", 6));
        if (writeCSD) {
            memcpy(data, mMMParserExtractor->mFormatBlock->data(), mMMParserExtractor->mFormatBlock->size());
            mFirstSample = false;
            size -= mMMParserExtractor->mFormatBlock->size();
            data += mMMParserExtractor->mFormatBlock->size();
        }

        FileSourceMediaStatus status = m_pFileSourceWrapper->GetNextMediaSample(mTrackId, data, (uint32*)&size, sInfo);

        if (FILE_SOURCE_DATA_OK != status || 0 == size) {
            if (mBuffer != NULL) {
                mBuffer->release();
                mBuffer = NULL;
            }
            //! Sample Size ZERO is special case to indicate NAL corruption
            if (FILE_SOURCE_DATA_END == status || 0 == size) {    //Grab this as a special case
                LOGV("FileSource::FILE_SOURCE_DATA_END");
                return ERROR_END_OF_STREAM;
            }
            else if (FILE_SOURCE_DATA_INSUFFICIENT == status) {   //Grab this as a special case
                LOGV("FileSource::FILE_SOURCE_DATA_INSUFFICIENT");
                return -EAGAIN;
            }
            return ERROR_IO;
        }

        if ((m_pFileSourceWrapper->mFileFormat == FILE_SOURCE_MP2TS ) &&
                   (m_pFileSourceWrapper->mVideoMinorType == FILE_SOURCE_MN_TYPE_H264)) {
            uint32 ulIdx = 0;
            uint8_t *tempPtr = data;
            size_t dataSize = size;
            const uint8_t *nalStart;
            size_t nalSize;
            NALPosition max_sei[6];

            while ((ulIdx < size) &&
              getNextNALUnit((const uint8_t **)&(tempPtr),&dataSize,&nalStart,&nalSize,true) == OK) {
                // For SEI naltype is 0x06
                // currently kept support of  max number of sei's per fram is 6
                if ((nalStart[0] & 0x1f) == 0x06 && nSeiCount < 6) {
                    ulIdx = (uint32)(nalStart - data);
                    ulIdx += (uint32)nalSize;
                    max_sei[nSeiCount].nalSize = nalSize;
                    max_sei[nSeiCount].nalOffset = (size_t)(nalStart - data);
                    tempPtr = (data + ulIdx);
                    if (ulIdx < size)
                        dataSize = size - ulIdx;
                    nSeiCount++;
                }
            }//while(ulIdx<size)

            // Allocate and update SEI buffer if SEI NAL found
            if (nSeiCount > 0) {
                setBuffSEI = new ABuffer(nSeiCount * sizeof(NALPosition));
                for (int i = 0; i < nSeiCount; i++) {
                    NALPosition &seiPos = ((NALPosition *)setBuffSEI->data())[i];
                    seiPos.nalOffset = max_sei[i].nalOffset;
                    seiPos.nalSize = max_sei[i].nalSize;
                }//for(nSeiCount)
            }//if(nSeiCount)
        }

        if (writeCSD) {
            size += mMMParserExtractor->mFormatBlock->size();
        }

        //dumpToFile
        dumpToFile(mBuffer->data(), size);

        mBuffer->set_range(0, size);
        mBuffer->meta_data()->clear();

        // set buffer on kKeySEI if SEI NAL found
        if (nSeiCount > 0)
          mBuffer->meta_data()->setData(kKeySEI, 0, setBuffSEI->data(), setBuffSEI->size());

        // here we are sending composition time instead of decode time
        // timestamp is in microsecond units
        uint64 ullTime = sInfo.startTime;
        mBuffer->meta_data()->setInt64(kKeyTime, ullTime);
        ++mCurrentSampleIndex;
        //! Timestamp is availale in micro-sec units
        mCurrentTimeMS = (sInfo.startTime / 1000);
        //! Set target time
        if (seekInfo.targetTimeUs >= 0) {
            mBuffer->meta_data()->setInt64(kKeyTargetTime, seekInfo.targetTimeUs);
        }
        //! This field is available only for OGG Files
        if (sInfo.nGranule) {
            mBuffer->meta_data()->setInt32(kKeyValidSamples, sInfo.nGranule);
            LOGV("Granule value is set as %llu", sInfo.nGranule);
        }
        LOGV("mCurrentSampleIndex = %d startTime = %lld Timescale = %d size = %zu",
             mCurrentSampleIndex, sInfo.startTime, mTimescale, size);
        if (sInfo.sync) {
            mBuffer->meta_data()->setInt32(kKeyIsSyncFrame, 1);
        }
    }

    *out = mBuffer;
    mBuffer = NULL;
    //LOGE(" MMParserSource::read <==== \n");
    return OK;
}
status_t MMParserSource::setBuffers(const Vector<MediaBuffer *> &pBuffers) {
   LOGI("Using codec-supplied buffers, size is %zu", pBuffers.size());
   CHECK(mSetBuffers);

   delete mGroup;
   mGroup = new MediaBufferGroup;
   for (size_t i = 0; i < pBuffers.size(); ++i) {
       mGroup->add_buffer(pBuffers.itemAt(i));
   }
   return OK;
}

bool MMParserSource::canReadMultiple() {
    bool ret = true;
    int32_t byteMode = 0;
    int32_t maxInputSize = 0;
    const char *mime = NULL;

    do {
        if (mFormat == NULL) {
            LOGI("canReadMultiple: mFormat is null!");
            break;
        }

        //Prevent readMultiple for byteMode
        if (mFormat->findInt32(kKeyIsByteMode, &byteMode) && byteMode == 1) {
            ret = false;
            break;
        }

        //Prevent readMultiple for large frame sizes in case of APE
        if (mFormat->findCString(kKeyMIMEType, &mime) && mime != NULL &&
            strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_APE) == 0 &&
            mFormat->findInt32(kKeyMaxInputSize, &maxInputSize) &&
            maxInputSize >= MediaBuffer::kSharedMemThreshold) {
            ret = false;
            break;
        }
    } while(false);

    LOGD("canReadMultiple byteMode(%d), mime(%s), maxInputSize(%d), ret(%d)",
                   byteMode, mime == NULL ? "null" : mime, maxInputSize, ret);
    return ret;
}

void MMParserSource::dumpToFile(void *data, size_t size)
{
    sp<MMParserExtractor> extractor = mMMParserExtractor;
    if(!extractor->m_sDebug.bDumpToFile)
        return;
    if(mFile == NULL) {
        char pucFileName[PROPERTY_VALUE_MAX + 36] = {0};
        snprintf(pucFileName, PROPERTY_VALUE_MAX + 36, "%s_track_%d",
                 extractor->m_sDebug.pucFileName, mTrackId);
        mFile = fopen(pucFileName, "ab");
        if (mFile == NULL) {
            LOGE("ERROR - o/p file %s could NOT be opened", pucFileName);
            return;
        }
    }
    fwrite(data, size, 1, mFile);
}

}  // namespace android

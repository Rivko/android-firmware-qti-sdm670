/*
 * Copyright (c) 2010 - 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef MMPARSER_EXTRACTOR_H_

#define MMPARSER_EXTRACTOR_H_

#include <media/stagefright/MediaExtractor.h>
#include "SourcePort.h"
#include <utils/threads.h>
#include <utils/String8.h>
#include "filesource.h"
#include "filesourcetypes.h"
#include "common_log.h"

#include <media/stagefright/MediaSource.h>
#include <media/stagefright/foundation/ABuffer.h>

/**
 * Parser masks bitwise map
 * These mask will be used to enable or disable individual mm-parser
 * dynamically based on property value defined in "mm.enable.qcom_parser".
 */
#define PARSER_AAC    0x00000001
#define PARSER_AC3    0x00000002
#define PARSER_AMR_NB 0x00000004
#define PARSER_AMR_WB 0x00000008
#define PARSER_ASF    0x00000010
#define PARSER_AVI    0x00000020
#define PARSER_DTS    0x00000040
#define PARSER_FLV    0x00000080
#define PARSER_3GP    0x00000100
#define PARSER_3G2    0x00000200
#define PARSER_MKV    0x00000400
#define PARSER_MP2PS  0x00000800
#define PARSER_MP2TS  0x00001000
#define PARSER_MP3    0x00002000
#define PARSER_OGG    0x00004000
#define PARSER_QCP    0x00008000
#define PARSER_WAV    0x00010000
#define PARSER_FLAC   0x00020000
#define PARSER_AIFF   0x00040000
#define PARSER_APE    0x00080000

//! These two flags are used to enable Divx and DivxHD+ support
#define PARSER_DIVX   0x00100000
#define PARSER_DIVXHD 0x00200000
#define PARSER_DSF    0x00400000
#define PARSER_DSDIFF 0x00800000
#define PARSER_MOV    0x01000000   //.mov,.mp4,.3gp
#define PARSER_XVID   0x02000000

//! These are required to check tool used to generate some MKV content
#define DIVX_PLUS        "DIVXPLUS"
#define DIVX_MUXING_APP  "LIBDIVXMEDIAFORMAT"
#define DIVX_WRITING_APP "DIVXMKVMUX"
using namespace video;

namespace android {

    class DataSource;
    class SampleTable;
    class String8;
    class MediaBufferGroup;

typedef struct {
    bool  bDumpMetaToLog;
    bool  bDumpToFile;
    char  pucFileName[PROPERTY_VALUE_MAX + 36];
} DebugInfo;

//Simple semaphore class.
class Semaphore
{
public:
    Semaphore();
    void Signal();
    void Wait();
private:
    Mutex mMutex;
    Condition mCondition;
    int mCount;
};

class FileSourceWrapper
{
public:
    static FileSourceWrapper* New(iStreamPort& aStreamPort, FileSourceFileFormat eFileFormat);
    virtual ~FileSourceWrapper();
    uint32 GetWholeTracksIDList(FileSourceTrackIdInfoType *trackIdInfo );
    FileSourceStatus GetMimeType(uint32 id, FileSourceMjMediaType& majorType, FileSourceMnMediaType& minorType);
    FileSourceStatus GetMediaTrackInfo(uint32 id,MediaTrackInfo* info);
    uint64  GetTrackMediaDuration(uint32 id);
    uint64  GetClipDuration();
    FileSourceStatus GetFormatBlock(uint32 id, uint8* buf, uint32 *pbufSize, bool bRawCodec = false);
    bool GetWMACodecData(uint32 id,WmaCodecData* pCodecData);
    uint8 IsSeekDenied();
    FileSourceStatus SeekAbsolutePosition( int64 trackid ,
                                         const int tAbsoluteTime,
                                         bool bSeekToSync=true,
                                         int64 nCurrPlayTime=-1,
                                         FS_SEEK_MODE eSeekMode=FS_SEEK_DEFAULT);
    FileSourceStatus SeekAbsolutePosition( const int tAbsoluteTime,
                                         bool bSeekToSync=true,
                                         int64 nCurrPlayTime=-1,
                                         FS_SEEK_MODE eSeekMode=FS_SEEK_DEFAULT);
    FileSourceStatus SeekRelativeSyncPoint( int currentPlaybacktime, const int numSync);
    int32 GetTrackMaxFrameBufferSize(uint32 id);
    FileSourceMediaStatus GetNextMediaSample(uint32 id, uint8 *buf, uint32 *size, FileSourceSampleInfo& pSampleInfo);
    FileSourceStatus GetFileFormat(FileSourceFileFormat& fileFormat);
    bool IsDrmProtection();
    status_t SetConfiguration(uint32 id,FileSourceConfigItem* pItem, FileSourceConfigItemEnum ienumData);
    status_t GetConfiguration(uint32 id,FileSourceConfigItem* pItem, FileSourceConfigItemEnum ienumData);
    status_t GetStreamParameter(uint32 id, uint32 paramIndex, void* ptr);
    status_t SetStreamParameter(uint32 id, uint32 paramIndex, void* ptr);
    bool GetAACCodecData(uint32 id,AacCodecData* pCodecData);
    bool GetWavCodecData(uint32 id,WavFormatData* pCodecData);
    FileSourceStatus GetClipMetaData(wchar_t* pMetaData,uint32* size,
                                     FileSourceMetaDataType pSampleInfo,
                                     FS_TEXT_ENCODING_TYPE *pEncode = NULL);
    bool IsContentGeneratedbyDivxHDTool();
    bool IsDivxSupportEnabled(FileSourceFileFormat  eFileFormat,
                              FileSourceMnMediaType eCodecType);
    bool IsCodecSupportAvailable(FileSourceFileFormat  eFileFormat,
                                 FileSourceMnMediaType eCodecType,
                                 MediaTrackInfo mediaTrackInfo);
private:
    FileSourceWrapper();
    static void cbFileSourceStatus(FileSourceCallBackStatus status, void* pCbData);
    status_t Init(iStreamPort& aStreamPort, FileSourceFileFormat eFileFormat);
private:
    FileSource *m_pFileSource;
    bool   iDrmEncrypted; //Indicates if clip is DRM encrypted
    Mutex mMutex;
    FileSourceStatus mStatus;
    Semaphore mWaitSeamaphore;
public:
    FileSourceFileFormat mFileFormat;
    FileSourceMnMediaType mVideoMinorType;
};

class MMParserExtractor : public MediaExtractor {
public:
    // Extractor assumes ownership of "source".
    MMParserExtractor(const sp<DataSource> &source, const char* mime);

    size_t countTracks();
    sp<IMediaSource> getTrack(size_t index);
    sp<MetaData> getTrackMetaData(size_t index,uint32_t flags );

    virtual uint32_t flags() const;
    sp<MetaData> getMetaData();
    void setExtraFlags(uint32_t flag);

protected:
    virtual ~MMParserExtractor();

private:
    friend class MMParserSource;
    struct Track {
        Track *next;
        sp<MetaData> meta;
        uint32_t timescale;
        uint32_t trackId;
    };

    sp<DataSource> mDataSource;
    sp<MetaData> mFileMetaData;

    status_t mHaveMetadata;
    uint32_t mTracks;
    bool mIsAacformatAdif;
    bool mVideoTrackPresent;
    bool mUseBigBuffers;

    Track *mFirstTrack, *mLastTrack;
    bool mSetBuffers;
    sp<ABuffer> mFormatBlock;

    status_t readMetaData();
    status_t updateAudioTrackInfoFromCSD(const uint8_t* csd, size_t csd_size);
    void setAlbumArtRelatedMeta();
    void setAlbumArt();
    void setAlbumMetaElement(FileSourceMetaDataType element, uint32_t key);
    status_t FillColorInformation(Track *track, FileSourceFileFormat eFileFmt);
    void dumpMetadataToLog();

    MMParserExtractor(const MMParserExtractor &);
    MMParserExtractor &operator=(const MMParserExtractor &);

    FileSourceWrapper *m_pFileSourceWrapper;
    SourcePort *m_pSourcePort;
    FileSourceFileFormat m_eFileFormat;
    DebugInfo m_sDebug;
};

class MMParserSource : public MediaSource {
public:
    // Caller retains ownership of both "dataSource" and "sampleTable".
    MMParserSource(const sp<MetaData> &format,
                const sp<MMParserExtractor> &extractor,
                FileSourceWrapper *fileSource,
                uint32_t trackId,int32_t timeScale);

    virtual status_t start(MetaData *params = NULL);
    virtual status_t stop();

    virtual sp<MetaData> getFormat();

    virtual status_t read(
            MediaBuffer **buffer, const ReadOptions *options = NULL);
    virtual status_t setBuffers(const Vector<MediaBuffer *> &pBuffers);
    virtual bool canReadMultiple();

protected:
    virtual ~MMParserSource();

private:
    void dumpToFile(void *data, size_t size);
    FILE* mFile;

    sp<MetaData> mFormat;
    //We need the mMMParserExtractor handle because it's referrence counted - if we don't keep it
    //it gets closed too early
    sp<MMParserExtractor> mMMParserExtractor;
    int32_t mTimescale;
    uint32_t mTrackId;
    FileSourceWrapper *m_pFileSourceWrapper;
    uint32_t mCurrentSampleIndex;
    uint64_t mCurrentTimeMS;

    bool mStarted;

    MediaBufferGroup *mGroup;

    MediaBuffer *mBuffer;

    bool mSetBuffers;
    bool mFirstSample;

    MMParserSource(const MMParserSource &);
    MMParserSource &operator=(const MMParserSource &);
};

}

#endif  // MMPARSER_EXTRACTOR_H_

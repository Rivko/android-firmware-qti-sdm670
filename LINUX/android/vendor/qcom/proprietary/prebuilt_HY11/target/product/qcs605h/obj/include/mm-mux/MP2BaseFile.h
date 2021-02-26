#ifndef __MUX_MP2BASEFILE_H__
#define __MUX_MP2BASEFILE_H__

/* =======================================================================
                              MP2BaseFile.h
DESCRIPTION

  Copyright (c) 2011 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */


/* =======================================================================
                             Edit History
$Source: //source/qcom/qct/multimedia2/Video/Source/FileMux/Sink/FileMux/MP2BaseFileLib/main/latest/inc/MP2BaseFile.h

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "sys/uio.h"
#include "MMTimer.h"
#include "oscl_file_io.h"
#include "filemuxtypes.h"
#include "muxbase.h"
#include "MMCriticalSection.h"
#include "MP2StreamMuxConstants.h"
#include "DataSourcePort.h"

/* =======================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define FS_FILE_EXTENSION_DOT              '.'
#define ASYNC_WRITER_FRAGMENT_BUF_SIZE(x)     (1024 * 1024)
#define ASYNC_WRITER_BUF_SIZE(x)              (256 * 1024)
#define MUX_MEM_MARGIN                        65536
#define EFS_MARGIN                            65536
#define CHUNK_TRANSFER_SIZE                   1000
#define TS_RTP_PAYLOAD_SIZE                   (7 * 188)
#define TS_HDR_POOL_SIZE                      7
#define IOV_VEC_POOL_SIZE                     (TS_HDR_POOL_SIZE * 2 + 1)

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

//! MEM file object type
typedef struct MP2_MUX_mem_object_struct {
    uint8     *buf_ptr;
    uint32    buf_size;
    uint32    length;
    uint32    pos;
    boolean   free_buf;
} MP2_MUX_mem_object_type;


#ifdef FILEMUX_WRITE_ASYNC
typedef struct mp2_stream_handler_info
{
    uint8     *pBuffer;        //!Buffer pointer
    uint32     pHead;          //!Head of the buf
    uint32     pTail;          //!Tail of the buf
    uint32     nSize;          //!Total Size of the buf
    uint32     nFlushThreshold;//!Threshold to start flushing the buffer
    boolean    bOverflow;      //!Queue overflow
    uint32     nBytesOverflow; //!Number of bytes that is overflown
    boolean    bFlushing;      //!Buffer is being drained
}Mux_mp2_stream_handler_info_type;
#endif /*FILEMUX_WRITE_ASYNC*/

typedef struct MP2_MUX_file_struct
{
  MP2_MUX_mem_object_type *mem_file;
  OSCL_FILE *efs_file;
}MP2_MUX_file;

typedef struct MP2OutputParamStruct
{
    uint8         *pMP2TSHdr;          //!Pointer to TS header
    uint32         nMP2TSHdrLen;       //!TS header length
    uint8         *pPESHdr;            //!Pointer to PES header
    uint32         nPESHdrLen;         //!PES header length
    uint8          *pData;             //!Pointer to data
    uint32         nDataLen;           //!Data length
    bool           bEOF;               //!End of stream flag
}MP2OutputParam;

typedef enum Mux_AAC_Format
{
    AAC_FORMAT_ADTS,
    AAC_FORMAT_LOAS
}Mux_AAC_Format_Type;

/**
 * MP2BufferPool is a class to store buffers associated with a
 * frame on the heap instead of the stack.
 */
class MP2BufferPool
{
public:
    MP2BufferPool(bool& bResult, uint8 nVideoStreamNum, video::iMultipleIovDataAllocator& rMultipleIovAllocator);
    ~MP2BufferPool();

    class MP2BufferPoolElement
    {
    public:
        MP2BufferPoolElement(bool& bOk, video::iMultipleIovData *pMultipleIovData, uint32 maxCount);
        ~MP2BufferPoolElement();

        void ResetMP2BufferPoolElement();

        uint8 *GetNextPESPktBuf();
        uint8 *GetNextTSHDrBufFromPool();

        video::iMultipleIovData *m_pMultipleIovData;

    private:
        MP2BufferPoolElement();
        MP2BufferPoolElement(const MP2BufferPoolElement&);
        MP2BufferPoolElement& operator=(const MP2BufferPoolElement&);

        static const uint32 MAX_PES_PKT_BUF_IN_POOL = 2;
        uint8 m_pMP2PESPacket[MAX_PES_PKT_BUF_IN_POOL][PES_PKT_SIZE];
        uint32 m_pMP2PESPacketIdx;

        // big enough ts header pool to accomodate MAX_RTP_PKTS_LIMIT packets limit
        typedef struct { uint8 tsbuffer[TS_PKT_SIZE]; } TSBufferType;
        TSBufferType *m_pTSHdrBufPool;
        uint32 m_pTSHdrBufPoolIdx;
        const uint32 m_MaxTSPoolEntries;
    };

    MP2BufferPoolElement* GetMP2BufferPoolElementForStream(uint32 nStreamNumber);

private:
    MP2BufferPool();
    MP2BufferPool(const MP2BufferPool&);
    MP2BufferPool& operator=(const MP2BufferPool&);

    void DeleteMP2BufferPool();

    MP2BufferPoolElement *m_PerStreamMP2BufferPoolElement[MUX_MAX_MEDIA_STREAMS];

    video::iMultipleIovDataAllocator& m_rMultipleIovAllocator;
};

/*!
  @brief   MP2BaseFile Module.

  @detail This module is to record the MP2 streams
*/
class MP2BaseFile : public MUXBase
{
public:
    //! constructor
    MP2BaseFile( MUX_create_params_type *,
                 MUX_fmt_type file_format,
                 MUX_brand_type file_brand,
                 MUX_handle_type *output_handle
                 );
    //! destructor
    virtual ~MP2BaseFile();


/* =======================================================================
**                          Function Declarations
** ======================================================================= */
virtual MUX_STATUS MUX_Process_Sample( uint32 stream_number,
                                        uint32 num_samples,
                                        const MUX_sample_info_type  *sample_info,
                                        const uint8  *sample_data);
virtual MUX_STATUS MUX_write_header (uint32 stream_id, uint32 header_size, const uint8 *header);

virtual MUX_STATUS MUX_update_AVC_Timing_HRD_Params
                                    (MUX_AVC_TimingHRD_params_type *pHRDParams);
virtual MUX_STATUS MUX_update_HEVC_Timing_HRD_Params
                                    (MUX_HEVC_TimingHRD_params_type *pHRDParams);

virtual MUX_STATUS MUX_update_streamport(uint64_t port);

virtual MUX_STATUS MUX_update_stream_params(MUX_create_params_type *pParams);

virtual MUX_STATUS MUX_end_Processing();

virtual MUX_STATUS MUX_get_current_PTS(uint64 *pnPTS);

#if 0 //def FILEMUX_WRITE_ASYNC
virtual int FlushStream(void);
#endif /* FILEMUX_WRITE_ASYNC */
protected:
void MUX_Modify_AVC_Start_Code(
                            const uint8 * pBuffer,
                            uint32*  nSize,
                            bool bHeader);

uint32 MUX_FindNextPattern(const uint8 *streamBuf,
                           uint32 bufSize,
                           uint32 startcode);

uint32 FindCheckSum(uint8 *pData,uint32 nSize);


void InitData ();
void OpenFiles();
uint32 GenerateTablesAndPCR(uint32   nStreamNumber,
                            MUX_sample_info_type  *pSampleInfo);
void GeneratePATPacket(uint8    *pMP2TSPacket);
void GeneratePMTPacket(uint8    *pMP2TSPacket);
uint16 GeneratePESPacket( uint32   stream_number,
                         const MUX_sample_info_type   *pSampleInfo,
                         uint8  *PES_Header );
uint32 GenerateMP2TSPacket(uint32                nStreamNumber,
                           MUX_sample_info_type  *pSampleInfo,
                           const  uint16         nPESHeaderSize,
                           const  uint32         nSampleDataOffset,
                           uint8                 *pMP2TSPacket,
                           uint32                *pTSHdrSz);

bool Destination_Mmc(const WCHAR *);
boolean OutputTS(uint32 nStreamNum,
                 MP2OutputParam         *pOpParam,
                 struct iovec           *pIOV,
                 uint32                 *pIOVnIndex,
                 uint32                 *pTSStreamBufSz,
                 uint32                 nOffset,
                 MUX_sample_info_type&  rSampleInfo,
                 video::iMultipleIovData&  rMultipleIovData);

bool EnqueueFrameDataForTransmit(uint32 nStreamNumber,
                                 MP2BufferPool::MP2BufferPoolElement& rElem);

MUX_STATUS packetizeStream(uint32 nStreamNumber,
                                MUX_sample_info_type  *pCurrentSampleInfo,
                                const uint8           *pSampleData);

#ifdef FILEMUX_WRITE_ASYNC
    bool   OpenFileHandler();
    void   CloseFileHandler();
    uint32 GetStreamBufferOccupancy(Mux_mp2_stream_handler_info_type  *m_sStreamHandler);
    uint32 PushStream(const uint8  *data, uint32 offset, uint32 len);
#endif /*  FILEMUX_WRITE_ASYNC */

uint32 Space_Check(void);
void handle_close ();
void close_writer (void);
static void timerCallback(void *pMp2File);

bool GenerateAVCHRDTimingDescriptor
                        (MUX_AVC_TimingHRD_params_type *pAVCHrdParams);

bool GenerateHEVCHRDTimingDescriptor
                        (MUX_HEVC_TimingHRD_params_type *pHEVCHrdParams);

MUX_STATUS MUX_toggle_PMT_version();

#ifdef FILEMUX_WRITE_ASYNC
    mux_write_file_cb_func_ptr_type               pFnFileWrite;
    void*                                         pClientData;
    uint32                                        output_unit_size;
    Mux_mp2_stream_handler_info_type              m_sStreamHandler;
    bool                                          closeissued;
    uint8                                         mdatAtomSize[4];
    MM_HANDLE                                     MP2BaseFileMux_CS;
    MM_HANDLE                                     pBuffAvailSig;
    MM_HANDLE                                     pBuffManSigQ;
    static const uint32                           BUF_AVAIL_EVENT;
#endif /* FILEMUX_WRITE_ASYNC  */
MUX_create_params_type                        *mpParams;
MUX_brand_type                                meFileBrand;
boolean                                       mbAudioPresent;
boolean                                       mbVideoPresent;
boolean                                       mbGenericPresent;
bool                                          mbToggleVersionChange;
bool                                          mbAdjustTSforCompliance;
uint8                                         mnAudioStreamNum;
Mux_AAC_Format_Type                           meAACFormat;
uint32                                        mnAudioFrameRate;
uint32                                        mnAudioFrameDuration90Khz;
uint64                                        mnCurrAudioTime;
uint8                                         mnVideoStreamNum;
uint32                                        mnVideoFrameRate;
uint64                                        mnCurrVideoTime;
bool                                          mbSendFillerNalu;
bool                                          mbLookForIframe;
uint8*                                        mpAVCHrdDescr;
uint8                                         mnAVCHrdDescrLen;
MUX_AVC_TimingHRD_params_type                 msHRDParams;
uint8*                                        mpHEVCHrdDescr;
uint8                                         mnHEVCHrdDescrLen;
MUX_HEVC_TimingHRD_params_type                msHEVCHRDParams;
uint8                                         mnGenericStreamNum;
uint64                                        mnPrevTimeStamp;
bool                                          mbBaseTimeStampTaken;
uint64                                        mnBaseTimestamp;
uint64                                        mllVideoDelayCorrection;
uint64                                        mnCurrPCR;
uint8                                         mpMP2TSContinutyCounter[MUX_MAX_MEDIA_STREAMS];
uint8                                         mMP2TSTableContinutyCounter[2];
boolean                                       mDataAlignmentIndicator;
uint8*                                        mpHeader[MUX_MAX_MEDIA_STREAMS];
uint32                                        mnHeaderSize[MUX_MAX_MEDIA_STREAMS];
bool                                          mbHeaderSent[MUX_MAX_MEDIA_STREAMS];
MM_HANDLE                                     mhVideoFrameTimer;
MP2_MUX_file                                  msFilePtr;
MUX_handle_type                               msOutputHandle;
boolean                                       mbOutputOpen;
MM_HANDLE                                     mhCritSect;
MM_HANDLE                                     mpStatTimer;
uint32                                        mnDuration;
bool                                          mbSaveToMmc;
uint32                                        mnOutputOffset;
uint32                                        mnOutputSize;
uint32                                        mnMmcFreeSpace;
uint32                                        mnFlashFreeSpace;
uint64                                        mllTimeBase;
MP2BufferPool *m_pMP2BufferPool;

private:

#ifdef ENABLE_MUX_STATS
typedef struct mux_stats_struct
{
    uint32  nStatCount;
    unsigned long nStartTime;
    unsigned long nEndTime;
    unsigned long  nMaxTime;
    bool    bEnableMuxStat;
}mux_stats;
mux_stats msMuxStats;

static void readStatTimerHandler(void * ptr);
#endif

};
#endif  //__MUX_MP2BASEFILE_H__


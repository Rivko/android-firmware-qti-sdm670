
/*==============================================================================
*        @file RTPParser.h
*
*  @par DESCRIPTION:
*       This is RTPParser class header file.
*
*
*  Copyright (c) 2012-2014, 2016-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/
#ifndef RTP_PARSER_H
#define RTP_PARSER_H
#ifdef _LINUX_
#include <errno.h>
#include <queue>
#include <list>

#if !defined(status_t)
typedef int32_t status_t;
#endif

#define NO_MEMORY -ENOMEM
#endif
#define PAYLOAD_NOT_SUPPORT -EPERM
/*
  *with recvmsg RTPPackets payload will be directly stored to circular buffer of RTPParser
  */
//#define RTP_USE_RECVMSG

//for wfd, maximum 7 Ts packets can be transmitted..hence max rtp size is 7*188+12(fixed header length)
#define RTP_PACKET_MAXIMUM_PAYLOAD_SIZE 7*188

#define RTP_PACKET_HEADER_LENGTH 12

#define TS_PKT_LEN               188
#define MTU_SIZE                 1500

#define RTP_MPEG2TS_PACKET_MAXIMUM_SIZE (RTP_PACKET_HEADER_LENGTH + RTP_PACKET_MAXIMUM_PAYLOAD_SIZE)

#define RTP_H264_PACKET_MAXIMUM_SIZE MTU_SIZE
#define RTP_H264_PAYLOAD_MAXIMUM_SIZE 1450

//maximum time that socket recv call can block in milli seconds
#define RTP_SOCKET_RECEIVE_TIME_OUT 50

//setting the socket receive buffer size
#define RTP_SOCKET_RECEIVE_BUFFER_SIZE 256*1024

//maximum Buffer size of 10MB
//ToDo:identify the correct value
#define RTP_PAYLOAD_DEFAULT_BUFFER_SIZE 30*1024*1024

//maximun time  in micro seconds that the caller can be bolcked to receive the data
//ToDo:identify the correct value
#define RTP_PAYLOAD_MAX_WAIT_TIME 50000

#define RTP_REORDER_PACKETS_QUEUE_LENGTH 20

#define ARRAY_NTOHS(data,i) ((0x00FF & data[i])<<8) | (0x00FF & data[i+1])
#define ARRAY_NTOHL(data,i) ((0x000000FF & data[i])<<24) | ((0x000000FF & data[i+1])<<16) \
                                                     | ((0x000000FF & data[i+2])<<8) | (0x000000FF & data[i+3])
#include <sys/socket.h>
#include <utils/List.h>
#include <media/stagefright/foundation/ALooper.h>
#include "MMMemory.h"
#include "AEEstd.h"
#include <queue>
#include "RTPEncoder.h"

class RTCPTransmitHandler;

#ifdef _ANDROID_
namespace android
{
#endif

  enum RTPParserStatus
  {
    RTP_PARSER_SUCCESS = 0,
    RTP_PARSER_ERROR_OOO_PACKET = -1, //out of order packet
    RTP_PARSER_ERROR_INVALID_PACKET = -2,
    RTP_PARSER_ERROR_NO_MEMORY = -3
  };

  /*
     *Contains the RTP packet info
     */
  typedef struct rtp_packet
  {
    uint16_t payloadType;  //payload type of packet
    uint16_t seqNum;       //sequence number of RTP packet
    uint32_t timeStamp;    //timestamp of RTP packet
    uint32_t ssrcID;       //SSRC ID

    uint8_t* pPayloadBuffer; //rtp payload buffer pointer
    size_t   nPayloadSize;   //payload buffer size
  }RTP_Packet_struct;
  typedef struct nal_buffer_hander_info_type
  {
     uint8_t   *pBuffer;            //buffer pointer
     size_t    nBufsize;            //buffer size in bytes
     size_t    nBuflen;             //buffer filled len in bytes
     bool      bStart;              //mark nal start
     bool      bEnd;                //mark nal end
  }nal_buffer_hander_info_type;
  /*
     *Class RTPParser
     */
  class RTPParser
  {
    public:

      virtual ~RTPParser();

      static RTPParser* create(::RTCPTransmitHandler* pRTCPHandler, int32 payloadType, bool tcp);

      virtual RTPParserStatus parseRTPHeader(uint8_t* pRTPPacket, size_t packetLength, RTP_Packet_struct* pRTPPacketStruct);

      virtual RTPParserStatus processRTPPacket(uint8_t* pRTPPacket, size_t packetLength);

      virtual ssize_t read(int64 offset, void *data, size_t size);
      virtual RTPParserStatus processRTPH264Packet(uint8_t* pRTPPacket,size_t packetLength);

      size_t readNalPackage(uint8_t *pData, size_t nOffset, bool * bCompleted);

      RTPParserStatus createNalBuffer(nal_buffer_hander_info_type *sNalBufHandle, uint8_t *pPayloadBuffer, size_t nPayloadSize);
      RTPParserStatus createNal( uint8_t *pPayloadBuffer, size_t nPayloadSize);
      status_t allocatePayloadBuffer(size_t nBufSize);

      int64 getNumBytesAvailable();

      int64 getHeadOffset();

      void updateRTPPortVars();

      void printStatistics();

      status_t getRecvDataPositions(uint8_t** recvVector_1, int32* recvVector_length_1,
                                                                 uint8_t** recvVector_2, int32* recvVector_length_2);

      status_t processRecvDataVector(size_t totalLengthRecvd);

      int64_t getRTPBaseTimeUs();

      bool   IsBaseTimeAdjusted(bool *bDiscontinuity, uint64 *pnTSBefore, uint64* pnTSAfter);

      RTPParserStatus flushRTPPacketsQueue();

      static void clearDebugLogs();

    private:
      RTPParser(RTCPTransmitHandler *pRTCPHandler, RTPPayloadType payloadType, bool tcp);

      RTPParser();
      RTPParser(const RTPParser&);
      RTPParser& operator=(const RTPParser &);

      void ParsePMTPacket(uint8* pRtpPacket);
      void ParsePATPacket(uint8* pRtpPacket);
      RTCPTransmitHandler* m_pRTCPHandler;
      /*
           * Circular buffer to hold payload.
           * if any new data received,after buffer is full, oldest data will be overridden.
           * Here head refers to the oldest byte in the buffer and the tail latest byte
           */
      typedef struct buffer_handler_info
      {
         uint8_t   *pBuffer;            //Buffer pointer
         size_t    nBufsize;            //buffer size in bytes
         volatile int64_t   headOffset; //offset of the ring buffer start  i.e. oldest byte offset
         volatile int64_t   tailOffset; //offset of the ring buffer end i.e. latest byte offset
      }TS_buffer_handler_info_type;

      TS_buffer_handler_info_type m_sTSBufferHandler;


      //WFD:debug variable.Set to true when recvmsg is being used
      bool mIsReorderingEnabled;

      //stores the payload of reorder packets
      uint8_t* mReorderPackets[RTP_REORDER_PACKETS_QUEUE_LENGTH];

      RTPPayloadType mPayloadType;

      //maintains rtp packets list sorted by sequence number
#ifdef _LINUX_
      std::list<RTP_Packet_struct> mRTPPackets;
#else
       List<RTP_Packet_struct> mRTPPackets;
#endif
      //queue of buffers which are filled with nal data and pending for processing
      std::queue<nal_buffer_hander_info_type> mNalPendingBuffers;
      /*maintains the last received sequence number*/
      uint16_t mPrevSeqNum;
      /*Synchronzation src Id*/
      uint32_t  mSSRCId;

      //captures the system time when first valid rtp packet comes
      int64_t mFirstPacketBaseTimeUs;

      uint64_t m_nPCRJumpValue;

      //statistics variables
      bool isStatisticsEnabled;
      int64_t mTotalPackets;
      int64_t mOutOfOrderPackets;
      int64_t mInvalidPackets;
      int64_t mSequenceNumMismatches;
      bool  m_bIsTCP;

      //mutex for RTP time stamp variables updating
      MM_HANDLE m_TimeStampCS;    //! handle for critical section
      MM_HANDLE m_pSignalRtpParserQ;//handle for signal queue
      MM_HANDLE m_pDataAvailableSignal;//handle for signal to be set
      MM_HANDLE m_NalQueueCS;
      bool m_bCompletedNal;
      static const uint32 DATA_AVAILABLE_SIGNAL; //user data for m_pDataAvailableSignal



      /*
            *offset in shared buffer where data can be received.
            *data at these places cannot be used for writing.
            *If continuous data is available,only one buffer is considered,otherwise two offsets need to be considered
            */
      int64_t mRecvBuffer_offset_1;
      int64_t mRecvBuffer_offset_2;
      size_t mRecvBuffer_offset_size_1;
      size_t mRecvBuffer_offset_size_2;

      uint32 m_nPrevPCRMs;
      uint32 m_nPCRBaseMs;
      unsigned long m_nLocalBaseTime;
      uint32 m_nPCRConsistency;
      uint32 m_nDelayedPCR;
      bool m_bPATParsed;
      bool m_bPMTParsed;
      bool m_bPCRBaseTaken;
      bool m_bLocalBaseTimeTaken;
      bool m_bBaseTimeAdjusted;
      uint32 m_nLastPCRPrevBase;
      uint32 m_nStartPCRNewBase;
      bool   m_bDiscontinuity;
      uint32 m_nPCRPID;
      uint32 m_nPMTPID;
      uint32 m_nLocalPrevTime;

      FILE *m_pDumpFile;
      MM_HANDLE m_pDumpTimer;
      uint32 m_nDuration;
      static void checkForRTPDumpStatus(void * ptr);

      int64 m_nMaxOffsetRead;
      boolean isUnReadDataOverWritten(RTP_Packet_struct* pRTPPacketStruct);
      RTPParserStatus pushPayload(RTP_Packet_struct* pRTPPacketStruct);
      RTPParserStatus enquePacket(RTP_Packet_struct* pRTPPacketStruct);
      uint16_t getMaxSequenceNumber(uint16_t seqNum1,uint16_t seqNum2);
      void init();
      void PCRCheckAndTimeUpdate(RTP_Packet_struct &rtpPacketStruct);
  };
#ifdef _ANDROID_
}//android namespace
#endif

#endif /*RTP_PARSER_H*/



#ifndef _RTP_PACKETIZER_H
#define _RTP_PACKETIZER_H
/*==============================================================================
*        @file RTPPacketizer.h
*
*  @par DESCRIPTION:
*       This is the RTPPacketizer class. This packetizes the incoming data
*       and transmit as TS over the port.
*
*
*  Copyright (c) 2011-2013,2015-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  $Header:

================================================================================
*/
#include "AEEStdDef.h"
#include "MMThread.h"
#ifdef _LINUX_
#include <stdio.h>
#endif
#include "RTPH264.h"
#include "RTPParser.h"
class CRTPPacketTransmit;
class MultipleIovData;

typedef struct networkInfo
{
    uint32 PortNum0;
    uint32 nIPAddress;
    uint8  bRtpPortTypeUdp;
    int32  nSocket;
    uint8 nIPAddressv6[16];
    bool bIPv6;
}networkInfoType;

class CRTPPacketizer
{
public:
    CRTPPacketizer( uint32 nPortNum,
                    uint32 nIPAddress,
                    uint32 nStreamBitrate,
                    uint8  bRtpPortTypeUdp,
                    RTPPayloadType payload_type,
                    bool isIPv6=false,
                    uint8 nIPv6Address[]={0},
                    uint32 IPv6Len=0);
    CRTPPacketizer( networkInfoType *pNetworkInfo,
                    uint32 nStreamBitrate, uint32 *pnSsrc,
                    RTPPayloadType payload_type);
    uint32 Encode(const uint8* pData,
                  uint32 len, bool bEOF);

    uint32 Encode(struct iovec* pIOV,
                  uint32 nNumVectors, bool bEOF);

    int TransmitMultipleIovData(const MultipleIovData& rMultipleIovData);

    bool   IsOK();
    void   Close (void);
    ~CRTPPacketizer(void);
    static void clearDebugLogs();
    boolean m_bIPv6;
private:

    uint32 m_nOutputUnitSize;

    typedef struct stream_handler_info
    {
        uint8    *pBuffer;           //!Buffer pointer
        uint8    *pOrigBuffer;       //!Caching the original pointer
        uint32   pHead;              //!Head of the buf
        uint32   pTail;              //!Tail of the buf
        uint32   nSize;              //!Total Size of the buf
        uint32   nFlushThreshold;    //!Threshold to start flushing the buffer
        boolean  bOverflow;          //!Queue overflow
        uint32   nBytesOverflow;     //!Number of bytes that is overflown
        boolean  bFlushing;          //!Buffer is being drained
    }RTP_ENC_stream_handler_info_type;

    RTP_ENC_stream_handler_info_type m_sStreamHandler;

    bool _success;

    MM_HANDLE m_pRTPSignalQ;        //! handle for signalQ
    MM_HANDLE m_pRTPThread;         //! handle for RTPThread
    MM_HANDLE m_RTPEncodeSignal;    //! handle for RTPEncodeSignal
    MM_HANDLE m_RTPCloseSignal;     //! handle for RTP close signal
    MM_HANDLE m_RTPPacketizerCS;    //! handle for critical section

    MM_HANDLE m_pBuffManSigQ;       //! handle for buffer manager signal queue
    MM_HANDLE m_pBuffAvailSig;      //! handle for signaling buffer availability

    static const uint32 m_RTPEncodeEvent;   //! Event-process incomingdata
    static const uint32 m_RTPCloseEvent;    //! Event-close the thred
    static const uint32 m_RTPBufAvailEvent; //! Event-buffer is available
    static const uint32 m_RTPStackSize; //! Stack size for rtp Thread

    CRTPPacketTransmit *m_pTransmitter;//! Instance of the transmitter class
    bool               m_bTCP;

    uint8 *m_RTPPacketBuf;           //! temporary Packet Buffer
    uint32 m_nRTPRefClock;
    bool   m_bFirstTimeStampCaptured;

    struct RTPPacketParams
    {
        uint32 txSsrc;               //! identified of the source
        uint32 payloadType;          //! payload type in RTP
        uint16 sequenceNo;           //! Sequence number
    }m_sRTPPacketParams;
    uint32 MakeRTPPacketH264(const uint8* pData, uint32 nLen,\
                    RTPH264_MODE mode, uint8 nFuPackNum, uint8 nCuPackNum);
    uint32 MakeRTPPacket(const uint8* pData, uint32 nLen);
    uint32 MakeRTPPacket(const uint8* pData, uint32 nLen,\
                         uint8* pRTPPacketBuf,\
                         unsigned long nCurrTimestamp);
    uint32 GetStreamBufferOccupancy();
    uint32 PushStream(const uint8  *data, uint32 len);
    uint32 PushStreamSync(const uint8  *data, uint32 len);
    uint32 PushStreamSync(struct iovec *pIOV, uint32 nNumVectors,
                          unsigned long nCurrTimestamp);

    void   InitRTPPacketParameters(RTPPayloadType payload_type, uint32 *pnSsrc);
    bool   OpenStreamHandler(void);
    int    FlushStream(void);
    void   CloseStreamHandler();
    static int RTPThreadEntry(void*);
    void RTPThread(void );

    unsigned long  m_nCurrTimestamp;
    unsigned long  m_nBaseTimestamp;
    RTPPayloadType  m_ePayloadType; //! payload type of RTP packge
    bool m_bFrameStart;
    bool m_bEOF;
    #ifdef ENABLE_RTP_STATS
    typedef struct rtp_stats_struct
    {
        uint32  nStatCount;
        unsigned long  nStartTime;
        unsigned long  nEndTime;
        unsigned long  nMaxTime;
        bool    bRTPStat;
    }rtp_stats;
    rtp_stats rtpStats;
    MM_HANDLE m_pStatTimer;
    uint32 m_nDuration;
    static void readStatTimerHandler(void * ptr);
    #endif /*ENABLE_RTP_STATS*/
    FILE *m_dumpfp;
};

#endif /*_RTP_PACKETIZER_H*/

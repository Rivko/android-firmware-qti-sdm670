/*==============================================================================
*        @file RTPDataSource.h
*
*  @par DESCRIPTION:
*       This is the RTPDataSource class header file
*
*  Copyright (c) 2012-2014, 2016-2018 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/

#ifndef RTP_DATA_SOURCE_H

#define RTP_DATA_SOURCE_H

#define RTP_THREAD_STACK_SIZE 16384

#include "MMMemory.h"
#include "AEEstd.h"
#include <sys/types.h>
#include <errno.h>
#ifdef _ANDROID_
#include <utils/Errors.h>
#endif
#include "RTPEncoder.h"
#include "RTPH264.h"
#ifdef _LINUX_
#include "Errors.h"
typedef int32_t status_t;
#define OK 0
#define ERROR_END_OF_STREAM -11
#define NO_INIT -ENODEV
#endif
class RTCPTransmitHandler;
#ifdef _ANDROID_
namespace android
{
#endif
  class RTPParser;

  class RtpTcpDataReader
  {
  public:
    /**
     * c'tor
     */
    RtpTcpDataReader();

    /**
     * d'tor
     */
    virtual ~RtpTcpDataReader();

    /**
     * Set the socket fd
     *
     * @param fd
     */
    void SetSockFd(int fd);

    enum ReturnType
    {
      SUCCESS,
      EWAIT,
      EFAILED,
    };

    enum ReadState
    {
      RTP_PKT_READ_SZ,
      RTP_PKT_READ_DATA,
      RTP_PKT_DATA_READY,
      RTP_PKT_CLOSED,
    };

    /**
     * Get the current rtp packet from the buffer
     *
     * @return ReturnType
     *  SUCCESS rtp packet fetched
     *  EWAIT   complete rtp packet not yet available
     *  EFAILED connection closed by peer or socket error
     */
    ReturnType GetRtpPacket(uint8*& pPkt, int& pktSize);

    /**
     * Current rtp packet is read and no longer needed.
     */
    void GetRtpPacketDone();

  private:
    RtpTcpDataReader(const RtpTcpDataReader&);
    RtpTcpDataReader& operator=(const RtpTcpDataReader&);

    /**
     * Move the state machine to try to have a complete packet
     * available in buffer.
     */
    void MoveStateMachine();

    /**
     * Moves unread data to start of buffer to maximize available
     * buffer space to read. For efficiency, this should be called
     * only when there is an incomplete packet left to read from
     * buffer.
     */
    void MovePendingDataToStartOfBuffer();

    /**
     * Update the state and reset variables for state if needed.
     *
     * @param newState
     */
    void SetState(ReadState newState);

  public:
    /**
     * Read data from socket.
     * (Overriden by unit-test).
     *
     * @return int
     *  > 0 number of bytes read from network
     *  0 no bytes were read, try again
     *  -1 connection closed or socket error
     */
    virtual int ReadFromSocket();

  protected:
    uint8 m_RtpTcpBuffer[100 * 1024];
    int m_WriteIdx;
    int m_ReadIdx;
    ReadState m_ReadState;

    /**
     * Size of current rtp packet within the state machine. This
     * value is correct when state is RTP_PKT_READ_DATA or
     * RTP_PKT_DATA_READY.
     */
    uint16 m_RtpPacketSize;

private:
    int m_SockFd;
  };

  class RTPDataSource
  {
   public:
    RTPDataSource(int32 port, RTPPayloadType payloadType, boolean isTCPConnection, int rtpSock, // RTP
                  uint16 rtcpRemotePort, uint32 destIP, int rtcpSock, int rtcpIntervalMs, bool isIPv6=false, uint8 destIPv6[]={0}, uint32 IPv6Len=0, // RTCP Stuff
                  bool isMulticast=false, uint32 multicastIP=0, uint8 multicastIPv6[]={0}, uint32 multicastIPv6Len=0);

    virtual ~RTPDataSource();

    virtual ssize_t readAt(int64 offset, void *data, size_t size);
    virtual status_t initCheck() const;
    status_t setCacheSize(size_t cacheSize);
    status_t start();
    status_t stop();
    status_t pause();
    status_t resume();
    void updateRTPPortVars();
    ssize_t readH264Nal(uint8_t *data);
    virtual status_t getSize(int64 *size);
    virtual status_t getHeadOffset(int64 *size);
    status_t getRTPBaseTime(int64 *baseTimeUs);
    bool IsBaseTimeAdjusted(bool *bDiscontinuity, uint64 *pnTSBefore, uint64* pnTSAfter);
    static void clearDebugLogs();
   private:

    enum RTPState
    {
      RTP_NO_INIT = 0,     //Component is not yet initialized
      RTP_INITIALIZED,      //Component is initialized
      RTP_FETCHING_DATA,   //Component starts fetching data
      RTP_PAUSED,          //Component starts fetching data//
      RTP_STOPPED,         //Component stopped fetching data
    };

    RTPState m_eRTPState;

    //holds RTP packet data. Points to only header if recvmsg is being used
    uint8* m_pRTPPacket;

    RTPParser* m_pRTPParser;

    MM_HANDLE m_pRTPThread;

    MM_HANDLE m_pSignalQ;
    MM_HANDLE m_pFetchDataSignal;
    MM_HANDLE m_pExitSignal;
    MM_HANDLE m_pPauseSignal;
    MM_HANDLE m_pResumeSignal;


    /*
         *Following values are the user data corresponding to m_pFetchDataSignal, m_pExitSignal respectively
         */
    static const uint32 FETCH_DATA_SIGNAL; //user data for m_pFetchDataSignal
    static const uint32 THREAD_EXIT_SIGNAL;
    static const uint32 THREAD_PAUSE_SIGNAL;
    static const uint32 THREAD_RESUME_SIGNAL;



    //True indicates connection is over TCP else UDP
    boolean m_bTCPConnection;
    int32 m_nRTPSocket;
    int32 m_nTCPDataSocket;
    RTPPayloadType m_ePayloadType;

    //Cache size in bytes
    int32 m_nCacheSize;

    //As part of Debug feature RTP_READ_FROM_FILE
    FILE *m_pLocalFile;
    boolean m_bIPv6;
    boolean m_isMulticast;
    uint32 m_MulticastIP;
    uint8 m_MulticastIPv6[16];
    boolean m_isFirstcheck;

    static int rtpThreadEntry( void* ptr );
    void rtpThreadWorker();
    int32 fetchData();
    int32 receiveRTPPacket(uint8* rtpPacket);
    void validateAndProcessRTPPacket(int32 nBytes);

    void init(int32 port, int32 payloadType, int rtpSock, bool tcp,
              uint16 rtcpRemotePort, uint32 destIP, int rtcpSock, int rtcpIntervalMs, bool isIPv6=false, uint8 destIPv6[]={0}, uint32 IPv6Len=0);
    int32 createSocket(int32 port);
    int32 setSocketOptions(int32 socket);
    int32 acceptTCPConnection(int32 tcpSocket);

    void closeDataSource();

    RTCPTransmitHandler *m_pRTCPHandler;

    RtpTcpDataReader m_RtpTcpDataReader;
  };

#ifdef _ANDROID_
}
#endif
#endif /*RTP_DATA_SOURCE_H*/


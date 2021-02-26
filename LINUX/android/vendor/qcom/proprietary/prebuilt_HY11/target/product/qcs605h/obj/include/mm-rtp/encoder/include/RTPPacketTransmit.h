/*==============================================================================
*        @file RTPPacketTransmit.h
*
*  @par DESCRIPTION:
*       This is the implementation of the RTPPacketizer class.
*
*
*  Copyright (c) 2011-2012,2015-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  $Header:

================================================================================
*/

//#undef UNICODE
#include "AEEStdDef.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include "dlfcn.h"

typedef void* MM_HANDLE;
// Fetches the requested parameter as configured in config utils library
typedef void (*ConfigParamFuncType)(const char*, void*);


#define FN_GET_CONFIG_PARAM      "getParameter"

class IovData;
class MultipleIovData;

class CRTPPacketTransmit
{
public:
    int SendPacket(const uint8 *pData, uint32 nLen);
    int SendPacket(uint8 *pRTPHeader, uint32 nHeaderLen, struct iovec *pIOV, uint32 nNumVectors);
    CRTPPacketTransmit(uint32 portNum, int32 nSocket, uint32 destIP, uint8 bRtpPortTypeUdp,
    bool isIPv6=false, uint8 destIPv6[]={0}, uint32 IPv6Len=0);

    ~CRTPPacketTransmit();

    int TransmitMultipleIovData(const MultipleIovData& rMultipleIovData);
    int TransmitMultipleIovDataHelper(const MultipleIovData& rMultipleIovData);
    int RetryTcpSend();
    bool m_bIPv6;
private:
    CRTPPacketTransmit();
    CRTPPacketTransmit(const CRTPPacketTransmit&);
    CRTPPacketTransmit& operator=(const CRTPPacketTransmit&);

    bool TransportConnect();
    void AllocateOrReallocateMemPool(uint32 sz);
    int TransmitPacketsInBatch(const MultipleIovData& rMultipleIovData);
    int TransmitPacketsOneAtTime(const MultipleIovData& rMultipleIovData);
    int TransmitSinglePacket(struct mmsghdr& rMmsgHdr, uint32 nTotalBytesToSend);
    int TransmitPacketsOneAtTimeRetry(int nRetryIdx);
    void PrintMmsgArray(uint32_t nStartIndex) const;

    bool _success;
    int XmitSocket;
    sockaddr_in faraddr;
    sockaddr_in6 faraddr6;
#ifdef RTCPONHOSTTEST
    int tempSock;
    sockaddr_in faraddr_rtcp;
    sockaddr_in6 faraddr_rtcp6;
#endif
    bool m_bRtpPortTypeUdp;
    bool m_bSockAssigned;

    struct iovec *m_ScratchIovBuffer;
    size_t m_ScratchIovBufferNumEntriesAllocated;
    static const size_t scratchIovBufferInitNumEntries = 32;

    struct mmsghdr *m_mmsghdr_array;
    uint32 m_mmsghdr_array_size;

    // Pointer to the MultipleIovData object being transmitted. The scope of
    // this is within the scope of TransmitMultipleIov as rtp module does not
    // have its own thread context. But this need not be the case if and when
    // rtp module (and its client - mux) is enhanced so that it has its own
    // thread context.
    const MultipleIovData *m_pMultipleIovData;

    // Number of packets completely sent from m_pMultipleIovData.
    uint32 m_nNumPktsCompletelySentTCP;

    // Number of bytes transitted for the first partially sent packet from
    // m_pMultipleIovData
    uint32 m_nNumBytesSentForLastTransmittedPktTCP;
    uint8 m_nIPTtl;
    int m_nUDPBuffersize;
    int m_nTCPBuffersize;
    int m_nTID;
};

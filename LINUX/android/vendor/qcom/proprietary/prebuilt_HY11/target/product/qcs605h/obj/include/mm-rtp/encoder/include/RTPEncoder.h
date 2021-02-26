#ifndef _RTPENCODER_H
#define _RTPENCODER_H
/* =======================================================================
                              SourcePortTest.h
DESCRIPTION
  Test class definition of IStreamPort based output interface


  Copyright (c) 2011-2012,2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */


/* =======================================================================
                             Edit History

$Header:

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "sys/uio.h"
#include <stdlib.h>
#include "DataSourcePort.h"
#include <stdio.h>

#ifdef _LINUX_
#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif
#else
#ifndef UNUSED(x)
#define UNUSED(x) ((void)(x))
#endif
#endif
using namespace video;
class CRTPPacketizer;

typedef struct data
{
    char *pData;
    unsigned int nLen;
}rtpGenericDatatype;

typedef enum RTPEvent
{
    RTP_SERVER_PORT_EVENT,
    RTP_RTCP_RR_EVENT
}RTPEventType;

typedef enum RTPStatus
{
    RTP_READY,
    RTP_SUCCESS,
    RTP_FAIL
}RTPStatusType;

typedef struct RTPNetworkInfo
{
    uint32 Port0;
    uint32 DestIp;
    uint8  bRtpPortTypeUdp;
    int32  nSocket;
    uint8 DestIp6[16];
    bool bIPv6;
}RTPNetworkInfoType;

typedef enum RTPPayload
{
  /*RTP Payload types specified in tables 4,5 of RFC 3551(RTP Profile for Audio and Video Conferences with Minimal Control)*/
   RTP_PAYLOAD_MPEG2TS = 33,
   RTP_PAYLOAD_H264 = 96,
}RTPPayloadType;

typedef void (*RTPCallbackType)(RTPEventType eEvent, RTPStatusType eStatus,
                             void* nEvtData, void* pUserData);
class CRTPEncoder:public video::iStreamPort
{
public:
    CRTPEncoder(uint32 nDestPortNum, uint32 nDestIP, uint32 nStreamBitrate,
            uint8 bRtpPortTypeUdp, RTPPayloadType payload_type, bool isIPv6=false, uint8 nDestIPv6[]={0}, uint32 IPv6Len=0);
    CRTPEncoder(RTPNetworkInfoType *pNetWorkInfo, uint32 nStreamBitrate,
            uint32 *pnSSRC = NULL, RTPPayloadType payload_type = RTP_PAYLOAD_MPEG2TS);
    void* QueryInterface(const AEEIID iid) {UNUSED(iid);return NULL;};

    uint32 AddRef() {return 0;};
    uint32 Release() {return 0;};

    /**
      Read data into the specified buffer (of given size).
      return DS_SUCCESS - *pnRead indicates the amount of data read
                      (*pnRead = 0 and nBufSize > 0 signifies EOS)
         DS_WAIT    - no data available at the moment call Readable() to wait
         DS_FAILURE - generic read failure
    */

    //virtual void iStreamPort::iReadNotificationHandler::Notify(){return;};


    virtual DataSourceReturnCode Read(/*rout*/ unsigned char* pBuf,
                                      /*in*/   ssize_t nBufSize,
                                      /*rout*/ ssize_t* pnRead)
    {
        UNUSED(pBuf);
        UNUSED(nBufSize);
        UNUSED(pnRead);
        return DS_FAILURE;
    };


  /**
  Read data into the specified buffer (of given size) from given Offset.
  return DS_SUCCESS - *pnRead indicates the amount of data read
                      (*pnRead = 0 and nBufSize > 0 signifies EOS)
         DS_WAIT    - no data available at the moment call Readable() to wait
         DS_FAILURE - generic read failure
  Repositions the read/write point in an open port to the specified offset.
  */
  virtual DataSourceReturnCode ReadAt(/*rout*/ unsigned char* pBuf,
                                      /*in*/ ssize_t nBufSize,
                                      /*in*/ const int64 nOffset,
                                      /*in*/ const int nWhence,
                                      /*rout*/ ssize_t* pnRead)
  {
        UNUSED(pBuf);
        UNUSED(nBufSize);
        UNUSED(nOffset);
        UNUSED(nWhence);
        UNUSED(pnRead);
        return DS_FAILURE;
  }

    /**
    Register a callback interface to be invoked when data is
     available to be read
    (i.e. Read() would return something other than DS_WAIT).
    */
    virtual DataSourceReturnCode Readable(
        /*in*/ iReadNotificationHandler const* pNotificationHandler)
    {
        UNUSED(pNotificationHandler);
        return DS_FAILURE;
    };

  /**
  Write data from the specified buffer (of given size).
  return DS_SUCCESS - *pnWritten indicates the amount of data written
         DS_FAILURE - generic write failure
  */
  /**
  Write data from the specified buffer (of given size).
  return DS_SUCCESS - *pnWritten indicates the amount of data written
         DS_FAILURE - generic write failure
  */
    DataSourceReturnCode Write(/*in*/   const unsigned char* pBuf,
                               /*in*/   ssize_t nBufSize,
                               /*rout*/ ssize_t* pnWritten);

  /**
  Write data from the specified buffer (of given size). This API
   provides additional information if the current frame carries
   and end of a frame.This is particularly used in RTP where
   RTP provides framing for bitstreams
   return DS_SUCCESS
         - pnWritten indicates the amount of data written
           DS_FAILURE
         - generic write failure
  */
  virtual DataSourceReturnCode WriteBlockData(/*in*/ const unsigned char* pBuf,
                                     /*in*/ int nBufSize,
                                     /*in*/ int nTimeStamp,
                                     /*in*/ bool bEOF,
                                     /*rout*/ int64* pnWritten);

/**
  Write data from the specified vector (of given size).
   This API provides additional information if the current frame
   carries and end of a frame.This is particularly used in RTP
   where RTP provides framing for bitstreams
   return DS_SUCCESS
         - pnWritten indicates the amount of data written
           DS_FAILURE
         - generic write failure
  */
  virtual DataSourceReturnCode WriteBlockDataScattered(/*in*/ struct iovec* pIOV,
                                     /*in*/ int nNumVectors,
                                     /*in*/ int nTimeStamp,
                                     /*in*/ bool bEOF,
                                     /*rout*/ int* pnWritten);

   /**
   * Send an instance of iMultipleIovData.
   * This is useful in transporting multiple RTP packets wherein
   * each RTP packet is an array of struct iovec entries.
   * Return DS_SUCCESS  entire data successfully transmitted
   *        DS_FAILURE  otherwise
   */
  virtual DataSourceReturnCode EnqueueForTransmit(
     /*in*/uint32 nStreamNumber, /*in*/ const iMultipleIovData *pMultipleIovData);

   /**
  Repositions the read/write point in an open port to the specified offset.
  *pnOutOffset gives the offset after a successful seek.
  */
    DataSourceReturnCode Seek(/*in*/   const int64 nOffset,
                              /*in*/   const int nWhence,
                              /*rout*/ int64* pnOutOffset)
    {
        UNUSED(nOffset);
        UNUSED(nWhence);
        UNUSED(pnOutOffset);
        return DS_FAILURE;
    };
  /**
  Close the data source port - port becomes unusable after this call.
  */
    virtual DataSourceReturnCode Close();

  /**
  Get the content length (in bytes).
  */
    virtual DataSourceReturnCode GetContentLength(
                          /*rout*/ int64* pContentLength)
    {
        UNUSED(pContentLength);
        return DS_FAILURE;
    };
  /**
  Get start offset.
  */
   virtual DataSourceReturnCode GetStartOffset(int64* pStartOffset)
   {
        UNUSED(pStartOffset);
        return DS_FAILURE;
   }

  /**
  Get the underlying data source type.
  */
    virtual DataSourceReturnCode GetSourceType(
                      /*rout*/ DataSourceType* pSourceType)
    {
        UNUSED(pSourceType);
        return DS_FAILURE;
    };

  /**
  Get the current available data size (in bytes).
  e.g. number of bytes downloaded for a network source.
  */
    virtual DataSourceReturnCode GetNumBytesAvailable(
                                 /*rout*/ int64* pNumBytesAvailable)
    {
        UNUSED(pNumBytesAvailable);
        return DS_FAILURE;
    };

  /**
  Get the max available offset to read (in bytes).
  e.g. This is the same as number of bytes downloaded if data download is
  sequential (e.g. Progressive Download) else (e.g. FastTrack) it is the
  farthest download point in the buffer.
  *pbEOS will tell whether to expect any more data, avoiding a Read() call
  to learn EOS.
  */
    virtual DataSourceReturnCode GetAvailableOffset(
                                         /*rout*/ int64* pAvailableOffset,
                                         /*rout*/ bool* pbEOS)
    {
        UNUSED(pAvailableOffset);
        UNUSED(pbEOS);
        return DS_FAILURE;
    };

  /**
  Get the Lower Bound avaialble offset to read (in bytes).
  e.g. This is useful in case of Live streaming scenarios, where
  Download component will overwrite data into the buffer. It gives
  lower bound valid offset value.
   pbEOS will tell whether to expect any more data, avoiding a
  Read() call to learn EOS.
  */
    virtual DataSourceReturnCode GetBufferLowerBound(/*rout*/ int64* pAvailableOffset,
                                                  /*rout*/ bool* pbEOS)
    {
        UNUSED(pAvailableOffset);
        UNUSED(pbEOS);
        return DS_FAILURE;
    };

    ~CRTPEncoder();

    static void clearDebugLogs();
    boolean m_bIPv6;
private:

    CRTPPacketizer *m_pPacketizer;
    typedef enum RTP_Encoder_State
    {
        RTP_ENC_INIT,
        RTP_ENC_READY,
        RTP_ENC_CLOSED,
        RTP_ENC_ERROR
    }RTP_Encoder_State_t;

    RTP_Encoder_State_t m_eRTPEncState;
};

#endif //_RTPENCODER_H

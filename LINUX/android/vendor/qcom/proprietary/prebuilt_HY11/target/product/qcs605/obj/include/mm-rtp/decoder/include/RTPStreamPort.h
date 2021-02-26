#ifndef __RTP_STREAM_PORT_H__
#define __RTP_STREAM_PORT_H__
/*==============================================================================
  *       RTPStreamPort.h
  *
  *  DESCRIPTION:
  *
  *
  *
  *  Copyright (c) 2012 - 2017 by Qualcomm Technologies, Inc. All Rights Reserved.
  *  Qualcomm Technologies Proprietary and Confidential.
  *==============================================================================*/
/* =======================================================================
                             Edit History
========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "MMDebugMsg.h"
#include "AEEstd.h"

#include "DataSourcePort.h"
#include "RTPDataSource.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
using namespace video;
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

#define UNUSED(x) ((void)x)

/* =======================================================================
**                        Class Declarations
** ======================================================================= */
class RTPStreamPort : public iStreamPort
{
  public:
     RTPStreamPort(uint16 rtpPort, bool bTCPConnection, int rtpsock,
                   uint16 rtcpRemotePort=0,uint32 destIP=0, int rtcpSock=-1,int rtcpIntervalMs=-1, bool isIv6=false, uint8 destIPv6[]={0}, uint32 IPv6Len=0);

     virtual ~RTPStreamPort();
     virtual DataSourceReturnCode Read(/*rout*/ unsigned char* pBuf,
                                    /*in*/ ssize_t nBufSize,
                                    /*rout*/ ssize_t* pnRead);

     virtual DataSourceReturnCode ReadAt(/*rout*/ unsigned char* pBuf,
                                      /*in*/ ssize_t nBufSize,
                                      /*in*/ const int64 nOffset,
                                      /*in*/ const int nWhence,
                                      /*rout*/ ssize_t* pnRead);

     DataSourceReturnCode GetRTPBaseTimeUs(/*rout*/ int64* pBaseTimeUs);
     bool IsBaseTimeAdjusted(bool *bDiscontinuity, uint64 *pnTSBefore, uint64* pnTSAfter);
     virtual DataSourceReturnCode GetContentLength(/*rout*/ int64* pContentLength);
     virtual DataSourceReturnCode GetNumBytesAvailable(/*rout*/ int64* pNumBytesAvailable);
     virtual DataSourceReturnCode GetAvailableOffset(/*rout*/ int64* pAvailableOffset,
                                                /*rout*/ bool* pbEOS);
     virtual DataSourceReturnCode GetBufferLowerBound(/*rout*/ int64* pAvailableOffset,
                                                /*rout*/ bool* pbEOS);

     virtual DataSourceReturnCode GetStartOffset(int64* pStartOffset);
     virtual DataSourceReturnCode GetSourceType(
                       /*rout*/ DataSourceType* pSourceType);
     virtual DataSourceReturnCode Close();
     virtual DataSourceReturnCode Seek(/*in*/ const int64 nOffset,
                                    /*in*/ const int nWhence,
                                    /*rout*/ int64* pnOutOffset);



     void* QueryInterface(const AEEIID iid) {UNUSED(iid);return NULL;};
     uint32 AddRef() {return 0;};
     uint32 Release() {return 0;};

     virtual DataSourceReturnCode Readable(
      /*in*/ iReadNotificationHandler const* pNotificationHandler)
     {
      UNUSED(pNotificationHandler);
      return DS_FAILURE;
     };

     virtual DataSourceReturnCode Write(/*in*/   const unsigned char* pBuf,
                             /*in*/   ssize_t nBufSize,
                             /*rout*/ ssize_t* pnWritten)
     {
       UNUSED(pBuf);
       UNUSED(nBufSize);
       UNUSED(pnWritten);
       return DS_FAILURE;
     };

     DataSourceReturnCode Start();
     DataSourceReturnCode Pause();
     DataSourceReturnCode Resume();
     void updateRTPPortVars();
     static void clearDebugLogs();

  private:
#ifdef _ANDROID_
     android::RTPDataSource* m_pRTPDataSource;
#else
     RTPDataSource* m_pRTPDataSource;
#endif
     int64 mOffset;
     void CreateRTPDataSource(
        int32 rtpPort, bool bTCPConnection, int rtpSock,
        uint16 rtcpRemotePort, uint32 destIP, int rtcpSock, int rtcpIntervalMs, bool isIv6=false, uint8 destIPv6[]={0}, uint32 IPv6Len=0);
};
#endif /*__RTP_STREAM_PORT_H__*/

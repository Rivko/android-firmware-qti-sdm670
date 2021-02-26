/*==============================================================================
*       RTCPMessage.h
*
*  DESCRIPTION:
*       Class declaration RTCPMessage
*
*  Copyright (c) 2016 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/

#ifndef _RTCP_MESSAGE_H
#define _RTCP_MESSAGE_H

/*==============================================================================
**               Includes and Public Data Declarations
**==============================================================================
*/

/* =============================================================================

                     INCLUDE FILES FOR MODULE

================================================================================*/
#include "AEEStdDef.h"
#include <string>

using namespace std;

//------------------------------------------------------------------------------
// Calculated based on 1500 MTU size
//------------------------------------------------------------------------------
#define  MAX_RR_MSGS      64

//------------------------------------------------------------------------------
//! breif   RTCPMessage structure. This holds all identified RTCP Params
//! @detail RTCPMessage class can take a payload of bytes received in the RTCP
//!         port and parse it a populate the RTCP params that are identified
//!         from within the message.
//!         The Usage of the RTCPMessage is as given below.
//!         RTCPMessage sMsg(pMessage, nLen);
//!         or
//!         new RTCPMessage(pMessage, nLen);
//------------------------------------------------------------------------------
struct RTCPMessage
{
    struct RTCPHeader
    {
        RTCPHeader(uint8 *pMsg, uint32 nLen){parseGenericHdr(pMsg, nLen);};

        void parseGenericHdr(uint8 *pMsg, uint32 nLen);

        uint32 ssrc;
        uint16 length;
        uint16 nPadLength;
        uint8  flags;
        uint8  type;

    private:
        RTCPHeader();
        RTCPHeader(const RTCPHeader&);
        RTCPHeader& operator=(const RTCPHeader&);
    };

    struct RRMsg: public RTCPHeader
    {
        RRMsg(uint8* pMsg, uint32 nLen);

        void parseRR(uint8* pMsg, uint32 nLen);

        struct ReportBlock
        {
            ReportBlock(uint8 *pMsg, uint32 nLen);

            void initDefaults();

            ~ReportBlock(){;};

            uint32 nSrcSSRC;
            int32 nCumNumOfPktsLost;
            uint32 nHighestSeqRcvd;
            uint32 nInterArvlJttr;
            uint32 nLastSR;
            uint32 nDlaySinceLastSR;
            uint8  nFractionLost;
            bool   valid;

        private:
            ReportBlock();
            ReportBlock(const ReportBlock&);
            ReportBlock& operator=(const ReportBlock&);

        };

        ~RRMsg();

        //!  Number of report blocks in RR
        uint16        nReportBlks;

        //!  Pointer to the array of report blocks
        ReportBlock **pReportBlks;
        uint8         nFmt;

    private:
        RRMsg();
        RRMsg(const RRMsg&);
        RRMsg& operator=(const RRMsg&);
    };

    RTCPMessage(uint8 *pMsg, uint32 nLen){ParseRTCPMsg(pMsg, nLen);};

    ~RTCPMessage();

    void ParseRTCPMsg(uint8 *pMsg, uint32 nLen);

    void getXml(string& xmlStr) const;

    //! Number of identified Receiver Reports in the message
    uint32       m_nRRs;

    //! Pointer to the array of receiver reports
    RRMsg       *m_pRR[MAX_RR_MSGS];

private:
    RTCPMessage();
    RTCPMessage(const RTCPMessage&);
    RTCPMessage& operator=(const RTCPMessage&);

};
#endif //_RTCP_MESSAGE_H

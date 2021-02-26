/*
===========================================================================

FILE:         DalSlimBusTest.c

DESCRIPTION:  This file implements tests for the SLIMbus Device Access Library.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/test/dal/src/DalSlimBusTest.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
10/20/16   MJS     Test support for PGD port index query API.
05/10/16   MJS     Test support for clock gear callback and max endpoint APIs.
10/14/15   MJS     Enhance test with multi-codec support and benchmarking.
06/11/15   NSD     Add wait calls to wait for both the source and sink port
                   disconnection events.
04/23/14   MJS     Support for ultrasound high-concurrency use case.
07/31/13   MJS     Support for codec repeat change value element message.
01/20/12   MJS     Updated for 8974.
09/13/11   MJS     Add second loopback test.
08/18/11   MJS     Temporarily skip data channel control after define channel.
05/27/11   MJS     Add QNX suport.
05/10/11   MJS     Update for on-target testing.
05/05/11   MJS     Fix pipe configuration options for multi-channel test.
04/14/11   MJS     Add IE tests.
08/20/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010-2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "ddislimbus.h"
#include "ddislimbus_codec.h"
#include "DALDeviceId.h"
#include "DALSys.h"
//#include "err.h"
#include "msg.h"
#ifdef IMAGE_QDSP6_PROC
#include "qurt.h"
#elif !defined(FEATURE_QNX)
#include "rex.h"
#endif


uint32 slimbus_test_err_cnt = 0;
uint32 slimbus_test_case_cnt = 0;
uint32 slimbus_test_err_line = 0;
DALBOOL slimbus_test_done = FALSE;

volatile uint32 slimbus_test_ready = 1;

#define MSG_BUILD_MASK_8500 MSG_BUILD_MASK_MSG_SSID_QDSP6

void slimbus_test_error_handler( uint32 line )
{
  slimbus_test_err_cnt++;
  slimbus_test_err_line = line;
  MSG_2(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "Slimbus test assert fail on line %d, error count %d", line, slimbus_test_err_cnt);
}

#define SB_ASSERT( exp )       \
  if( !(exp) )                 \
  {                            \
    slimbus_test_error_handler(__LINE__); \
  } \
  else \
  { \
    slimbus_test_case_cnt++; \
  }

//extern void memory_barrier(void);

#if !defined(IMAGE_QDSP6_PROC) && !defined(FEATURE_QNX)
__weak
#endif
extern struct
{
  DALBOOL ignore_replies;
  DALBOOL force_nack;
} _g_sb_debug;

uint32 g_sb_test_case = 0;

volatile uint32 xxxzzz = 0;

static DALSYSEventHandle hNormalEvent;

typedef struct
{
  DalDeviceHandle *hSB;
  SlimBusResourceHandle *phPort;
  SlimBusResourceHandle hLoopPort;
  SlimBusResourceHandle hChannel;
  SlimBusResourceHandle hCounter;
  DALSYSEventHandle hBamEvent;
  DALSYSEventHandle hPortEvent;
  uint32 uNumPorts;
  uint32 uNumWords;
  uint32 uBamCount;
  uint32 uPhysOffset;
  DALBOOL bConnected;
} CallbackDataType;

static uint8 ucDummyBuf[256];
static uint32 hDummyBuf;
    
#ifdef FEATURE_QNX
#define SBTEST_SLEEP()  SlimBusTestSleep()
#elif defined(IMAGE_QDSP6_PROC)
#define SBTEST_SLEEP()  qurt_timer_sleep(125)
#else
#define SBTEST_SLEEP()  rex_sleep(10)
#endif
uint32 uSlimBusTestSleepRef = 1000;
volatile uint32 uSlimBusTestSleepCount;
void SlimBusTestSleep(void)
{
   uint32 n;
   uSlimBusTestSleepCount = 0;
   for (n = uSlimBusTestSleepRef; n > 0; n--)
   {
      uSlimBusTestSleepCount++;
   }
}

void SlimBusTestProcessBamEvents(CallbackDataType *pCbData)
{
  SlimBusBamIOVecType IOVec;
  DALResult result;
  uint32 i;
  
  while (TRUE)
  {
    DALSYS_EventWait(pCbData->hBamEvent);
    
    for (i = 0; i < pCbData->uNumPorts; i++)
    {
      result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->phPort[i], SLIMBUS_BAM_DEFAULT, &IOVec);
      while (DAL_SUCCESS == result &&
             NULL != IOVec.uAddr)
      {
//        IOVec.uAddr = (uint32) ucDummyBuf;
//        IOVec.uSize = sizeof(ucDummyBuf);
////        if (i == pCbData->uNumPorts-1)
////        {
//          IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
////        }
////        else
////        {
////          IOVec.uFlags = NULL;
////        }
        result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->phPort[i], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
        SB_ASSERT(result == DAL_SUCCESS);
    
        if ( i == pCbData->uNumPorts-1 )
        {
          MSG(MSG_SSID_QDSP6, MSG_LEGACY_MED, "SlimBusTestProcessBamEvents: BAM descriptor done");

          if ( --pCbData->uBamCount == 0 )
          {
            uint32 uDMA = 0, uFifo = 0, j;
            
            result = DalSlimBus_ReadProgressCounter(pCbData->hSB, pCbData->hCounter, 
                                                    SLIMBUS_COUNTER_FREE_RUN, &uDMA, &uFifo);
            SB_ASSERT(result == DAL_SUCCESS);
            SB_ASSERT(uDMA >= pCbData->uNumWords &&
                      uDMA < pCbData->uNumWords+IOVec.uSize);
  
            IOVec.uAddr = hDummyBuf;
            IOVec.uSize = sizeof(ucDummyBuf);
            IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
            for (j = 0; j < pCbData->uNumPorts; j++)
            {
              result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->phPort[j], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
              SB_ASSERT(result == DAL_SUCCESS);
            }
  
            return;
          }
        }
    
        result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->phPort[i], SLIMBUS_BAM_DEFAULT, &IOVec);
      }
    }
  }
}

void SlimBusTestProcessBamLoopbackEvents(CallbackDataType *pCbData)
{
  SlimBusBamIOVecType IOVecRx;
  DALResult result;
  
  while (TRUE)
  {
    DALSYS_EventWait(pCbData->hBamEvent);
    
    result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecRx);
    while (DAL_SUCCESS == result &&
           NULL != IOVecRx.uAddr)
    {
      SlimBusBamIOVecType IOVecTx;
      
      MSG(MSG_SSID_QDSP6, MSG_LEGACY_MED, "SlimBusTestProcessBamLoopbackEvents: BAM descriptor done");

      result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecTx);
      if (DAL_SUCCESS == result &&
          NULL != IOVecTx.uAddr)
      {
        uint32 uMemCmpVal;
        uMemCmpVal = memcmp((void*)(IOVecTx.uAddr+pCbData->uPhysOffset),
                            (void*)(IOVecRx.uAddr+pCbData->uPhysOffset),
                            IOVecTx.uSize);
        if (0 != uMemCmpVal && IOVecTx.uSize > 4)
        {
          uMemCmpVal = memcmp((void*)(IOVecTx.uAddr+pCbData->uPhysOffset),
                              (void*)(IOVecRx.uAddr+pCbData->uPhysOffset+4),
                              IOVecTx.uSize-4);
        }
        SB_ASSERT(uMemCmpVal == 0);

        IOVecTx.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
        result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecTx, NULL);
        SB_ASSERT(result == DAL_SUCCESS);

        if (--pCbData->uBamCount == 0)
        {
          uint32 uDMA = 0, uFifo = 0;

          result = DalSlimBus_ReadProgressCounter(pCbData->hSB, pCbData->hCounter, 
                                                  SLIMBUS_COUNTER_FREE_RUN, &uDMA, &uFifo);
          SB_ASSERT(result == DAL_SUCCESS);
          SB_ASSERT(uDMA >= pCbData->uNumWords &&
                    uDMA < pCbData->uNumWords+IOVecRx.uSize);

//////////
#if 0
          IOVecTx.uAddr = hDummyBuf;
          IOVecTx.uSize = sizeof(ucDummyBuf);
          result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecTx, NULL);
          SB_ASSERT(result == DAL_SUCCESS);
          result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecTx, NULL);
          SB_ASSERT(result == DAL_SUCCESS);
#endif
          return;
        }
      }
      //IOVecRx.uAddr += 2*IOVecRx.uSize;
      IOVecRx.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecRx, NULL);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecRx);
    }
  }
}

uint32 uLoopZeroCnt = 0, uLoopCmpCnt = 0;

void SlimBusTestProcessBamLoopback2Events(CallbackDataType *pCbData)
{
  SlimBusBamIOVecType IOVecRx;
  SlimBusBamIOVecType IOVecTx;
  DALResult result;
  
  while (TRUE)
  {
    DALSYS_EventWait(pCbData->hBamEvent);
    
    result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecRx);
    while (DAL_SUCCESS == result &&
           NULL != IOVecRx.uAddr)
    {
      if ((*(uint32*)(IOVecRx.uAddr+pCbData->uPhysOffset)) != 0)
      {
        MSG(MSG_SSID_QDSP6, MSG_LEGACY_MED, "SlimBusTestProcessBamLoopback2Events: BAM descriptor done (non-zero data)");
        
        if (--pCbData->uBamCount == 0)
        {
//////////
#if 0
          IOVecRx.uAddr = hDummyBuf;
          IOVecRx.uSize = sizeof(ucDummyBuf);
          result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecRx, NULL);
          SB_ASSERT(result == DAL_SUCCESS);
          result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecRx, NULL);
          SB_ASSERT(result == DAL_SUCCESS);
          result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecRx, NULL);
          SB_ASSERT(result == DAL_SUCCESS);
          result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecRx, NULL);
          SB_ASSERT(result == DAL_SUCCESS);
#endif
          return;
        }
        else
        {
          uLoopZeroCnt++;
        }
      }
      else
      {
        MSG(MSG_SSID_QDSP6, MSG_LEGACY_MED, "SlimBusTestProcessBamLoopback2Events: BAM descriptor done (received zeros)");
      }
      IOVecRx.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecRx, NULL);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecRx);
    }
      
    result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecTx);
    while (DAL_SUCCESS == result &&
           NULL != IOVecTx.uAddr)
    {
      IOVecTx.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecTx, NULL);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecTx);
    }
  }
}

void SlimBusTestProcessBamLoopback3Events(CallbackDataType *pCbData)
{
  SlimBusBamIOVecType IOVecRx;
  DALResult result;
  
  while (TRUE)
  {
    DALSYS_EventWait(pCbData->hBamEvent);
    
    result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecRx);
    while (DAL_SUCCESS == result &&
           NULL != IOVecRx.uAddr)
    {
      SlimBusBamIOVecType IOVecTx;
      
      result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecTx);
      if (DAL_SUCCESS == result &&
          NULL != IOVecTx.uAddr)
      {
        IOVecTx.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
        result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecTx, NULL);
        SB_ASSERT(result == DAL_SUCCESS);

        if( memcmp((void*)(IOVecTx.uAddr+pCbData->uPhysOffset),
                   (void*)(IOVecRx.uAddr+pCbData->uPhysOffset),
                   IOVecTx.uSize) == 0 )
        {
          uLoopCmpCnt++;
          if (--pCbData->uBamCount == 0)
          {
//////////////
#if 0
            IOVecTx.uAddr = hDummyBuf;
            IOVecTx.uSize = sizeof(ucDummyBuf);
            result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->phPort[0], SLIMBUS_BAM_DEFAULT, &IOVecTx, NULL);
            SB_ASSERT(result == DAL_SUCCESS);
            result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecTx, NULL);
            SB_ASSERT(result == DAL_SUCCESS);
#endif  
            return;
          }
        }
      }
      IOVecRx.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecRx, NULL);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->hLoopPort, SLIMBUS_BAM_DEFAULT, &IOVecRx);
    }
  }
}

void SlimBusTestProcessUnderflowEvents(CallbackDataType *pCbData)
{
  SlimBusBamIOVecType IOVec;
  DALResult result;
  int i = 0;
  
  while (TRUE)
  {
    DALSYS_EventWait(pCbData->hBamEvent);
    
//    SlimBusTestSleep();
    for (i = 0; i < 1000000; i++)
    {
      xxxzzz++;
    }
        
    result = DalSlimBus_GetBamIOVec(pCbData->hSB, pCbData->phPort[0],
                                    SLIMBUS_BAM_DEFAULT, &IOVec);
    if (DAL_SUCCESS == result &&
        NULL != IOVec.uAddr)
    {
      if (--pCbData->uBamCount == 0)
      {
        return;
      }
      pCbData->bConnected = TRUE;
      result = DalSlimBus_SubmitBamTransfer(pCbData->hSB, pCbData->phPort[0],
                                            SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
    }
  }
}

void* SlimBusPortCb( void *pBuf1, uint32 dwParam, void *pBuf2, uint32 uSize2 )
{
  SlimBusEventNotifyType Notify;
  CallbackDataType *pCbData = (CallbackDataType*)pBuf1;
  DALResult result;
  uint32 i;

  
  /* Could have race condition where port is deallocated before we receive 
     all event callbacks, so don't get events if we've already started 
     disconnecting */  
  if ( pCbData->bConnected )
  {
    for (i = 0; i < pCbData->uNumPorts; i++)
    {
      result = DalSlimBus_GetPortEvent(pCbData->hSB, pCbData->phPort[i], &Notify);
      SB_ASSERT(result == DAL_SUCCESS);
      
      if (Notify.eEvent & SLIMBUS_EVENT_O_PORT_DISCONNECT)
      {
        if (i == pCbData->uNumPorts-1)
        {
          pCbData->bConnected = FALSE;
          DALSYS_EventCtrl(pCbData->hPortEvent, DALSYS_EVENT_CTRL_TRIGGER);
        }
        else
        {
          SB_ASSERT(FALSE);
        }
      }
      if (Notify.eEvent & ~SLIMBUS_EVENT_O_PORT_DISCONNECT)
      {
        SB_ASSERT(FALSE);
      }
    }
  }
  
  return NULL;
}
void* SlimBusSimplePortCb( void *pBuf1, uint32 dwParam, void *pBuf2, uint32 uSize2 )
{
  CallbackDataType *pCbData = (CallbackDataType*)pBuf1;
  SlimBusEventNotifyType Notify;
  DALResult result;

  result = DalSlimBus_GetPortEvent(pCbData->hSB, pCbData->phPort[0], &Notify);
  SB_ASSERT(result == DAL_SUCCESS);
  if (Notify.eEvent & (SLIMBUS_EVENT_O_FIFO_OVERRUN|SLIMBUS_EVENT_O_FIFO_UNDERRUN))
  {
    SB_ASSERT(pCbData->bConnected);
    pCbData->bConnected = FALSE;
    DALSYS_EventCtrl(pCbData->hBamEvent, DALSYS_EVENT_CTRL_TRIGGER);
  }
  if (Notify.eEvent & SLIMBUS_EVENT_O_PORT_DISCONNECT)
  {
    pCbData->bConnected = FALSE;
    DALSYS_EventCtrl(pCbData->hPortEvent, DALSYS_EVENT_CTRL_TRIGGER);
  }
  
  return NULL;
}

void* SlimBusClockCb( void *pBuf1, uint32 dwParam, void *pBuf2, uint32 uSize2 )
{
  uint32 *puCount = (uint32*)pBuf1;
  SlimBusSchedParamsType *pSchedParams = (SlimBusSchedParamsType*)dwParam;

  (*puCount)++;
  
  if (pSchedParams->UpdateType == SLIMBUS_SCHED_PARAM_BEFORE_UPDATE)
  {
    MSG_4(MSG_SSID_QDSP6, MSG_LEGACY_MED, "Got clock gear callback #%d: (before) "
                                          "gear=%d, prevgear=%d, subframe=0x%x",
          *puCount, pSchedParams->uClkGear, pSchedParams->uPrevClkGear,
          pSchedParams->uSubFrmCode);
  }
  else
  {
    MSG_4(MSG_SSID_QDSP6, MSG_LEGACY_MED, "Got clock gear callback #%d: (after)  "
                                          "gear=%d, prevgear=%d, subframe=0x%x",
          *puCount, pSchedParams->uClkGear, pSchedParams->uPrevClkGear,
          pSchedParams->uSubFrmCode);
  }
 
  return NULL;
}

#define NUM_LOOPBACK_PORTS 2
static DALResult SlimBusTestWorkLoopHandler(DALSYSEventHandle hEvent, void *pVoid)
{
  DalDeviceHandle *hSB;
  SlimBusResourceHandle hPort[NUM_LOOPBACK_PORTS];
  SlimBusPortConfigType Config;
  SlimBusPipeConfigType Pipe;
  SlimBusChannelReqType ChanReq;
  SlimBusBamRegisterEventType Reg;
  SlimBusBamIOVecType IOVec;
  DALBOOL bWatermark = FALSE;
  SlimBusResourceHandle hRxSharedChannel, hTxSharedChannel;
  CallbackDataType CbData;
  SlimBusDataChannelStatusType ChanStatus;
  uint32 uFifoWord;
  DALSYSEventHandle /*hEvent,*/ hBamEvent, hPortEvent;
  static uint8 ucBuf1[96];
  uint8 *pucBuf3, *pucBuf4, *pucBuf5, *pucBuf6;
  uint32 phBuf3, phBuf4, phBuf5, phBuf6;
  uint32 i;

  DALSYSMemHandle hPhysMem;
  DALSYSMemInfo MemInfo;
  DALResult result;
  
  result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                                 DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                 DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                 4096, &hPhysMem, NULL);    
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_MemInfo(hPhysMem, &MemInfo);
  SB_ASSERT(result == DAL_SUCCESS);

  pucBuf3 = (uint8*) MemInfo.VirtualAddr;
  phBuf3 = (uint32)pucBuf3 - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;

  pucBuf4 = pucBuf3 + sizeof(ucBuf1);
  phBuf4 = (uint32)pucBuf4 - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;

  pucBuf5 = pucBuf4 + sizeof(ucBuf1);
  phBuf5 = (uint32)pucBuf5 - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;

  pucBuf6 = pucBuf5 + sizeof(ucBuf1);
  phBuf6 = (uint32)pucBuf6 - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;

  result = DAL_SlimBusDeviceAttach( DALDEVICEID_SLIMBUS_1, &hSB );
  SB_ASSERT(result == DAL_SUCCESS);
  
  result = DalDevice_Open( hSB, DAL_OPEN_SHARED );
  SB_ASSERT(result == DAL_SUCCESS);
  
  for (i=0; i < 2; i++)
  {
    /* Note: multi-channel groups should be allocated with a single call */
    result = DalSlimBus_AllocMasterPorts(hSB, SLIMBUS_PORT_REQ_O_DEFAULT, 4, &hPort[i], sizeof(hPort[0]));
    SB_ASSERT(result == DAL_SUCCESS);
  }

  result = DalSlimBus_AllocSharedDataChannel(hSB, 128, &hRxSharedChannel);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DalSlimBus_AllocSharedDataChannel(hSB, 129, &hTxSharedChannel);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEvent, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hBamEvent, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &hPortEvent, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_SetupCallbackEvent(hPortEvent, SlimBusPortCb, (void*)&CbData);
  SB_ASSERT(result == DAL_SUCCESS);

  CbData.hSB = hSB;
  CbData.phPort = &hPort[0];
  CbData.hLoopPort = hPort[1];
  CbData.uNumPorts = 1;
  CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
  CbData.hCounter = NULL;
  CbData.hChannel = hRxSharedChannel;
  CbData.hBamEvent = hBamEvent;
  CbData.hPortEvent = hEvent;
  CbData.uBamCount = 12;
  CbData.uNumWords = CbData.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
  CbData.bConnected = TRUE;

  Config.eOptions = SLIMBUS_PORT_O_PACKED;
  Config.uBlockSize = 0;
  Config.uTransSize = 0;
  Config.uWaterMark = 8;

  memset(&Pipe, 0, sizeof(Pipe));
  Pipe.DescFifo.uMinSize = 64;
  Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
  Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);

  Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
  Reg.hEvent = hBamEvent;
  Reg.pUser = NULL;

  result = DalSlimBus_ConfigMasterPort(hSB, hPort[0], &Config);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], hPortEvent, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_ConfigMasterPipe(hSB, hPort[0], SLIMBUS_PORT_SINK_FLOW, &Pipe);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_RegisterBamEvent(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &Reg);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_ConnectPortToChannel(hSB, hTxSharedChannel, SLIMBUS_PORT_SINK_FLOW, hPort[0]);
  SB_ASSERT(result == DAL_SUCCESS);

//  result = DalSlimBus_InitProgressCounter(hSB, hCounter[0], 0);
//  SB_ASSERT(result == DAL_SUCCESS);

  memset(pucBuf3, 0, sizeof(ucBuf1));
  IOVec.uAddr = phBuf3;
  IOVec.uSize = sizeof(ucBuf1);
  IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
  result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  memset(pucBuf4, 0, sizeof(ucBuf1));
  IOVec.uAddr = phBuf4;
  IOVec.uSize = sizeof(ucBuf1);
  IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
  result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DalSlimBus_ConfigMasterPort(hSB, hPort[1], &Config);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], hPortEvent, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_ConfigMasterPipe(hSB, hPort[1], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_RegisterBamEvent(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &Reg);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_ConnectPortToChannel(hSB, hRxSharedChannel, SLIMBUS_PORT_SOURCE_FLOW, hPort[1]);
  SB_ASSERT(result == DAL_SUCCESS);

  memset(pucBuf5, 0, sizeof(ucBuf1));
  IOVec.uAddr = phBuf5;
  IOVec.uSize = sizeof(ucBuf1);
  IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
  result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  memset(pucBuf6, 0, sizeof(ucBuf1));
  IOVec.uAddr = phBuf6;
  IOVec.uSize = sizeof(ucBuf1);
  IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
  result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
  ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
  ChanReq.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;
  ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
  ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;

  ChanReq.uRateMultiplier = 6;
  ChanReq.uSampleSize_bits = 8;
  result = DalSlimBus_NextDefineDataChannel(hSB, hRxSharedChannel, &ChanReq);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_NextDefineDataChannel(hSB, hTxSharedChannel, &ChanReq);
  SB_ASSERT(result == DAL_SUCCESS);

  //result = DalSlimBus_NextDataChannelControl(hSB,  hChannel[0], SLIMBUS_CHAN_CTRL_ACTIVATE);
  //SB_ASSERT(result == DAL_ERROR);

  bWatermark = FALSE;
  for (i = 0; !bWatermark && i < 1000; i++)
  {
    result = DalSlimBus_GetPortFifoStatus(hSB, hPort[1], SLIMBUS_BAM_DEFAULT,
                                          &uFifoWord, &bWatermark);
  }
  SB_ASSERT(result == DAL_SUCCESS);
  //SB_ASSERT(uFifoWord > 0);

  result = DalSlimBus_DoReconfigureNow(hSB);
  SB_ASSERT(result == DAL_SUCCESS);

  DALSYS_EventCtrl(hNormalEvent, DALSYS_EVENT_CTRL_TRIGGER);

  SlimBusTestProcessBamLoopback2Events(&CbData);

  result = DalSlimBus_GetDataChannelStatus(hSB, hTxSharedChannel, &ChanStatus);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE);
  result = DalSlimBus_GetDataChannelStatus(hSB, hRxSharedChannel, &ChanStatus);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE);

  DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET);

  result = DalSlimBus_NextDataChannelControl(hSB, hTxSharedChannel, SLIMBUS_CHAN_CTRL_REMOVE);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_NextDataChannelControl(hSB, hRxSharedChannel, SLIMBUS_CHAN_CTRL_REMOVE);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_DoReconfigureNow(hSB);
  SB_ASSERT(result == DAL_SUCCESS);

  DALSYS_EventWait(hEvent);
  result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], NULL, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], NULL, NULL);
  SB_ASSERT(result == DAL_SUCCESS);


  for (i=0; i < NUM_LOOPBACK_PORTS; i++)
  {
    result = DalSlimBus_DeallocMasterPorts(hSB, &hPort[i], sizeof(hPort[0]));
    SB_ASSERT(result == DAL_SUCCESS);
  }

  result = DalDevice_Close( hSB );
  SB_ASSERT(result == DAL_SUCCESS);
  
  result = DAL_DeviceDetach( hSB );
  SB_ASSERT(result == DAL_SUCCESS);
  
  DALSYS_EventCtrl(hNormalEvent, DALSYS_EVENT_CTRL_TRIGGER);

  return DAL_WORK_LOOP_EXIT;
}

typedef struct
{
  uint8 aMsmEA[SLIMBUS_ENUM_ADDR_BUF_SIZE+2];
  uint32 uMaxEndPoints;
  SlimBusHwCapType eHwCap;
  uint32 uPortBase;
} SlimBusMsmDataType;

void SLIMbus_Test( void )
{
  DALResult result = DAL_SUCCESS;
//  uint32 addr = 0x800;
//  uint32 data = 0xabcd1234;
//  uint32 data2 = 0xdeadbeef;
//  uint16 data3 = 0x5531;
//  uint32 data4 = 0xef4b23a8;
  uint32 datain = 0;
//  uint32 datain2 = 0;
  uint32 datalen = 0;
  uint8 aaIntEA[][SLIMBUS_ENUM_ADDR_BUF_SIZE+2] = {
    { 0x00, 0x00, 0x30, 0x01, 0x17, 0x02, /*start padding*/0x0, 0x0 },  // TomTom
    { 0x00, 0x00, 0xa0, 0x01, 0x17, 0x02, /*start padding*/0x0, 0x0 },  // Tasha
    { 0x00, 0x00, 0x90, 0x01, 0x17, 0x02, /*start padding*/0x0, 0x0 },  // Taphon
    { 0x00, 0x00, 0x50, 0x02, 0x17, 0x02, /*start padding*/0x0, 0x0 },  // Tavil
  };
  uint8 aaGenEA[][SLIMBUS_ENUM_ADDR_BUF_SIZE+2] = {
    { 0x00, 0x01, 0x30, 0x01, 0x17, 0x02, /*start padding*/0x0, 0x0 },  // TomTom
    { 0x00, 0x01, 0xa0, 0x01, 0x17, 0x02, /*start padding*/0x0, 0x0 },  // Tasha
    { 0x00, 0x01, 0x90, 0x01, 0x17, 0x02, /*start padding*/0x0, 0x0 },  // Taphon
    { 0x00, 0x01, 0x50, 0x02, 0x17, 0x02, /*start padding*/0x0, 0x0 },  // Tavil
  };
  SlimBusMsmDataType MsmData[] = {
    { { 0x00, 0x05, 0x60, 0x01, 0x17, 0x02 },  // Istari
      1,
      (SLIMBUS_HW_CAP_COMBINED_PORT_DMA_INT|
       SLIMBUS_HW_CAP_TIMESTAMP_SHADOW_COUNTER|
       SLIMBUS_HW_CAP_PORT_UPPER_WATERMARK),
      0
    },
    { { 0x00, 0x05, 0x10, 0x02, 0x17, 0x02 },  // Nazgul
      4,
      (SLIMBUS_HW_CAP_COMBINED_PORT_DMA_INT|
       SLIMBUS_HW_CAP_TIMESTAMP_SHADOW_COUNTER|
       SLIMBUS_HW_CAP_PORT_UPPER_WATERMARK|
       SLIMBUS_HW_CAP_PORT_MULTI_ENDPOINT),
      6
    },
    { { 0x00, 0x05, 0x70, 0x02, 0x17, 0x02 },  // Napali
      4,
      (SLIMBUS_HW_CAP_COMBINED_PORT_DMA_INT|
       SLIMBUS_HW_CAP_TIMESTAMP_SHADOW_COUNTER|
       SLIMBUS_HW_CAP_PORT_UPPER_WATERMARK|
       SLIMBUS_HW_CAP_PORT_MULTI_ENDPOINT),
      0
    },
    { { 0x00, 0x05, 0x60, 0x02, 0x17, 0x02 },  // Starlord
      4,
      (SLIMBUS_HW_CAP_COMBINED_PORT_DMA_INT|
       SLIMBUS_HW_CAP_TIMESTAMP_SHADOW_COUNTER|
       SLIMBUS_HW_CAP_PORT_UPPER_WATERMARK|
       SLIMBUS_HW_CAP_PORT_MULTI_ENDPOINT),
      0
    }
  };
  SlimBusLogicalAddrType genLA = 0, intLA = 0;
  DalDeviceHandle *hSB;
  DALSYSEventHandle hEvent, hBamEvent, hPortEvent, hSimplePortEvent, hEvent1, hPortEvent1, hSimplePortEvent1, hClockEvent;
  uint32 uIdx, uIdx2;
  uint32 uMaxEP = 0, uClkCount = 0;
  SlimBusHwCapType eHwCap;
  uint32 uMsmIdx;
//  SlimBusErrorType eError;
//  int i;
  

  slimbus_test_err_cnt = 0;

  //while (slimbus_test_ready == 0)
  {
  }
  
  DALSYS_InitMod(NULL);
  
  MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "Slimbus test start");

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEvent, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEvent1, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hBamEvent, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &hPortEvent, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &hPortEvent1, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &hSimplePortEvent, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &hSimplePortEvent1, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &hClockEvent, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_SetupCallbackEvent(hClockEvent, SlimBusClockCb, (void*)&uClkCount);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DAL_SlimBusDeviceAttach( DALDEVICEID_SLIMBUS_1, &hSB );
  SB_ASSERT(result == DAL_SUCCESS);
  
  result = DalDevice_Open( hSB, DAL_OPEN_SHARED );
  SB_ASSERT(result == DAL_SUCCESS);
  
  for ( uIdx = 0, result = DAL_ERROR; uIdx < 100 && result != DAL_SUCCESS; uIdx++)
  {
    for (uIdx2 = 0; uIdx2 < sizeof(aaIntEA)/sizeof(aaIntEA[0]); uIdx2++)
    {
      result = DalSlimBus_GetDeviceLogicalAddr( hSB, aaIntEA[uIdx2], SLIMBUS_ENUM_ADDR_BUF_SIZE, &intLA);
      if (result == DAL_SUCCESS)
      {
        result = DalSlimBus_GetDeviceLogicalAddr( hSB, aaGenEA[uIdx2], SLIMBUS_ENUM_ADDR_BUF_SIZE, &genLA);
        if (result == DAL_SUCCESS)
        {
          break;
        }
      }
    }
    SBTEST_SLEEP();
  }
  SB_ASSERT(result == DAL_SUCCESS);

  for (uMsmIdx = 0; uMsmIdx < sizeof(MsmData)/sizeof(MsmData[0]); uMsmIdx++)
  {
    SlimBusLogicalAddrType msmLA = 0;
    
    result = DalSlimBus_GetDeviceLogicalAddr(hSB, MsmData[uMsmIdx].aMsmEA, SLIMBUS_ENUM_ADDR_BUF_SIZE, &msmLA); 
    if (result == DAL_SUCCESS)
    {
      break;
    }
  }
  SB_ASSERT(result == DAL_SUCCESS);
  if (uMsmIdx >= sizeof(MsmData)/sizeof(MsmData[0]))
  {
    uMsmIdx = 0;
  }

  result = DalSlimBus_RegisterClockGearEvent(hSB, hClockEvent);
  SB_ASSERT(result == DAL_SUCCESS);

  datain = 0x35;
  result = DalSlimBus_WriteValueElement(hSB, intLA, 0x810, 1, (uint8*)&datain, 1, NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  datain = 0;
  result = DalSlimBus_ReadValueElement(hSB, intLA, 0x820, (uint8*)&datain, 1, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0x50);
  SB_ASSERT(datalen == 1);

  datain = 0;
  result = DalSlimBus_ReadValueElement(hSB, intLA, 0x821, (uint8*)&datain, 1, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0x48);
  SB_ASSERT(datalen == 1);

  datain = 0;
  result = DalSlimBus_ReadValueElement(hSB, intLA, 0x810, (uint8*)&datain, 1, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0x35);
  SB_ASSERT(datalen == 1);

  result = DalSlimBus_ReadValueElement(hSB, intLA, 0x820, (uint8*)&datain, 4, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0x00004850);
  SB_ASSERT(datalen == 4);

  result = DalSlimBus_ReadValueElement(hSB, intLA, 0x80f, (uint8*)&datain, 4, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0x00003500);
  SB_ASSERT(datalen == 4);

  {
    uint32 dataout, datain2, i, j;
    uint8 ucBuf[] =
    {
      0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
    };
    
    result = DalSlimBus_CodecRepeatWriteValueElement(hSB, intLA, 0x810, ucBuf+3, 1, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    
    datain = 0;
    result = DalSlimBus_ReadValueElement(hSB, intLA, 0x810, (uint8*)&datain, 1, &datalen, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(datain == 0x3);
    SB_ASSERT(datalen == 1);

    result = DalSlimBus_CodecRepeatWriteValueElement(hSB, intLA, 0x810, ucBuf+2, 3, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    datain = 0;
    result = DalSlimBus_ReadValueElement(hSB, intLA, 0x810, (uint8*)&datain, 1, &datalen, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(datain == 0x4);
    SB_ASSERT(datalen == 1);

    result = DalSlimBus_CodecRepeatWriteValueElement(hSB, intLA, 0x810, ucBuf+4, 8, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    datain = 0;
    result = DalSlimBus_ReadValueElement(hSB, intLA, 0x810, (uint8*)&datain, 1, &datalen, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(datain == 0xb);
    SB_ASSERT(datalen == 1);

    result = DalSlimBus_CodecRepeatWriteValueElement(hSB, intLA, 0x810, ucBuf, 16, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    datain = 0;
    result = DalSlimBus_ReadValueElement(hSB, intLA, 0x810, (uint8*)&datain, 1, &datalen, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(datain == 0xf);
    SB_ASSERT(datalen == 1);

    datain = 0x35;
    result = DalSlimBus_WriteValueElement(hSB, intLA, 0x810, 1, (uint8*)&datain, 1, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    
    /******/    
    
    result = DalSlimBus_ReadValueElement(hSB, intLA, 0x830, (uint8*)&datain, 4, &datalen, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    dataout = 0x36;
    
    result = DalSlimBus_WriteValueElement(hSB, intLA, 0x830, 1, (uint8*)&dataout, 1, SLIMBUS_EVENT_HANDLE_BATCHED_XFER);
    SB_ASSERT(result == DAL_SUCCESS);

    dataout = 0xfa1b;
    
    result = DalSlimBus_WriteValueElement(hSB, intLA, 0x832, 2, (uint8*)&dataout, 2, SLIMBUS_EVENT_HANDLE_BATCHED_XFER);
    SB_ASSERT(result == DAL_SUCCESS);

    dataout = 0xc5;
    
    result = DalSlimBus_WriteValueElement(hSB, intLA, 0x831, 1, (uint8*)&dataout, 1, SLIMBUS_EVENT_HANDLE_BATCHED_XFER);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_ReadValueElement(hSB, intLA, 0x830, (uint8*)&datain2, 4, &datalen, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(datain == datain2);
    SB_ASSERT(datalen == 4);
    
    DalSlimBus_SendBatchedMessages(hSB);
    SB_ASSERT(result == DAL_SUCCESS);
    
    result = DalSlimBus_ReadValueElement(hSB, intLA, 0x830, (uint8*)&datain2, 4, &datalen, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(datain2 == 0xfa1bc536);
    SB_ASSERT(datalen == 4);

    dataout = 0xd4;

    result = DalSlimBus_WriteValueElement(hSB, intLA, 0x833, 1, (uint8*)&dataout, 1, SLIMBUS_EVENT_HANDLE_BATCHED_XFER);
    SB_ASSERT(result == DAL_SUCCESS);

    dataout = 0x7f18;

    result = DalSlimBus_WriteValueElement(hSB, intLA, 0x831, 2, (uint8*)&dataout, 2, SLIMBUS_EVENT_HANDLE_BATCHED_XFER);
    SB_ASSERT(result == DAL_SUCCESS);

    dataout = 0xeb;

    result = DalSlimBus_WriteValueElement(hSB, intLA, 0x830, 1, (uint8*)&dataout, 1, SLIMBUS_EVENT_HANDLE_BATCHED_XFER);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_ReadValueElement(hSB, intLA, 0x830, (uint8*)&datain2, 4, &datalen, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(datain2 == 0xfa1bc536);
    SB_ASSERT(datalen == 4);

    DalSlimBus_SendBatchedMessages(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_ReadValueElement(hSB, intLA, 0x830, (uint8*)&datain2, 4, &datalen, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(datain2 == 0xd47f18eb);
    SB_ASSERT(datalen == 4);

    result = DalSlimBus_WriteValueElement(hSB, intLA, 0x830, 4, (uint8*)&datain, 4, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    
    /*******/
    
    result = DalSlimBus_SetBatchBufferSize(hSB, 1024*64);
    SB_ASSERT(result == DAL_SUCCESS);
    
    for (i = 0; i < 10; i++)
    {
      for (j = 0; j < 2000; j++)
      {
        result = DalSlimBus_CodecRepeatWriteValueElement(hSB, intLA, 0x810, ucBuf, 16, SLIMBUS_EVENT_HANDLE_BATCHED_XFER);
        SB_ASSERT(result == DAL_SUCCESS);
      }

      DalSlimBus_SendBatchedMessages(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }

    for (i = 8; i <= 10; i++)
    {
      if (i==9)
      {
        result = DalSlimBus_NextReserveMsgBandwidth(hSB, 8000000);
        SB_ASSERT(result == DAL_SUCCESS);
        result = DalSlimBus_DoReconfigureNow(hSB);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      else if (i==10)
      {
        result = DalSlimBus_NextReserveMsgBandwidth(hSB, 16000000);
        SB_ASSERT(result == DAL_SUCCESS);
        result = DalSlimBus_DoReconfigureNow(hSB);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "Start blocking transfer n=2000, CG=%d", i);
      for (j = 0; j < 2000; j++)
      {
        result = DalSlimBus_CodecRepeatWriteValueElement(hSB, intLA, 0x810, ucBuf, 16, NULL);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "End blocking transfer n=2000, CG=%d", i);

      MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "Start non-blocking transfer n=2000, CG=%d", i);
      for (j = 0; j < 2000-1; j++)
      {
        result = DalSlimBus_CodecRepeatWriteValueElement(hSB, intLA, 0x810, ucBuf, 16, SLIMBUS_EVENT_HANDLE_NON_BLOCKING);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_CodecRepeatWriteValueElement(hSB, intLA, 0x810, ucBuf, 16, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "End non-blocking transfer n=2000, CG=%d", i);

      MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "Start batch transfer n=2000, CG=%d", i);
      for (j = 0; j < 2000; j++)
      {
        result = DalSlimBus_CodecRepeatWriteValueElement(hSB, intLA, 0x810, ucBuf, 16, SLIMBUS_EVENT_HANDLE_BATCHED_XFER);
        SB_ASSERT(result == DAL_SUCCESS);
      }
  
      DalSlimBus_SendBatchedMessages(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
      MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "End batch transfer n=2000, CG=%d", i);
    }
    result = DalSlimBus_NextReserveMsgBandwidth(hSB, 0);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);
    
    datain = 0x35;
    result = DalSlimBus_WriteValueElement(hSB, intLA, 0x810, 1, (uint8*)&datain, 1, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
  }

#if 0 // mjs
  result = DalSlimBus_WriteValueElement(hSB, intLA, addr, sizeof(data), (uint8*)&data, sizeof(data), NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  
  result = DalSlimBus_ReadValueElement(hSB, intLA, addr, (uint8*)&datain, sizeof(datain), &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == data);
  SB_ASSERT(datalen == 4);

  _g_sb_debug.force_nack = TRUE;
  result = DalSlimBus_WriteValueElement(hSB, intLA, addr+0x4, sizeof(data2), (uint8*)&data2, sizeof(data), NULL);
  SB_ASSERT(result == SLIMBUS_ERR_MSG_NACKED);
  _g_sb_debug.force_nack = FALSE;

  result = DalSlimBus_WriteValueElement(hSB, intLA, addr+0x4, sizeof(data2), (uint8*)&data2, sizeof(data), NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DalSlimBus_WriteValueElement(hSB, intLA, addr+0x8, 7, (uint8*)&data, sizeof(data), NULL);
  SB_ASSERT(result == DAL_ERROR);

//  result = DalSlimBus_ReadValueElement(hSB, intLA, addr, (uint8*)&datain, sizeof(datain), &datalen, NULL);
//  SB_ASSERT(result == DAL_SUCCESS);
//  SB_ASSERT(datain == data);
//  SB_ASSERT(datalen == 4);
  
  result = DalSlimBus_ReadValueElement(hSB, intLA, addr+0x4, (uint8*)&datain, sizeof(datain), &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == data2);
  SB_ASSERT(datalen == 4);

  _g_sb_debug.force_nack = TRUE;
  result = DalSlimBus_ReadValueElement(hSB, intLA, addr+0x4, (uint8*)&datain, sizeof(datain), &datalen, NULL);
  SB_ASSERT(result == SLIMBUS_ERR_MSG_NACKED);
  _g_sb_debug.force_nack = FALSE;

  result = DalSlimBus_ReadValueElement(hSB, intLA, addr+0x3, (uint8*)&datain, 1, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT((uint8)datain == 0xab);
  SB_ASSERT(datalen == 1);

  _g_sb_debug.ignore_replies = TRUE;
  result = DalSlimBus_ReadValueElement(hSB, intLA, addr+0x3, (uint8*)&datain, 1, &datalen, NULL);
  SB_ASSERT(result == SLIMBUS_ERR_MSG_RESP_TIMEOUT);
  _g_sb_debug.ignore_replies = FALSE;

  result = DalSlimBus_ReadValueElement(hSB, intLA, addr+0x4, (uint8*)&datain, 2, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT((uint16)datain == 0xbeef);
  SB_ASSERT(datalen == 2);

  result = DalSlimBus_ReadValueElement(hSB, intLA, addr, (uint8*)aIntEA, 5, &datalen, NULL);
  SB_ASSERT(result == DAL_ERROR);

  result = DalSlimBus_WriteValueElement(hSB, intLA, addr+1, sizeof(data3), (uint8*)&data3, sizeof(data3), NULL);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DalSlimBus_ReadValueElement(hSB, intLA, addr, (uint8*)&datain, 4, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0xab553134);
  SB_ASSERT(datalen == 4);

//#if 0
  result = DalSlimBus_WriteValueElement(hSB, intLA, addr+3, 1, (uint8*)&data2, 1,
                                        SLIMBUS_EVENT_HANDLE_NON_BLOCKING);
  SB_ASSERT(result == DAL_SUCCESS);

  _g_sb_debug.force_nack = TRUE;
  result = DalSlimBus_WriteValueElement(hSB, intLA, addr, 1, (uint8*)&data, sizeof(data), hEvent);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventMultipleWait(&hEvent, 1, 10000, &uIdx);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DalSlimBus_GetLastError(hSB, &eError);
  SB_ASSERT(eError == SLIMBUS_ERR_MSG_NACKED);
  _g_sb_debug.force_nack = FALSE;

  result = DalSlimBus_WriteValueElement(hSB, intLA, addr, 3, (uint8*)&data, sizeof(data), hEvent);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventMultipleWait(&hEvent, 1, 1000, &uIdx);
  SB_ASSERT(result == DAL_SUCCESS);
  
  result = DalSlimBus_GetLastError(hSB, &eError);
  SB_ASSERT(eError == SLIMBUS_ERR_NONE);
  
  datain = 0;
  result = DalSlimBus_ReadValueElement(hSB, intLA, addr+1, (uint8*)&datain, 3, &datalen, hEvent);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventMultipleWait(&hEvent, 1, 1000, &uIdx);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DalSlimBus_GetLastError(hSB, &eError);
  SB_ASSERT(eError == SLIMBUS_ERR_NONE);
  SB_ASSERT(datain == 0xefcd12);
  SB_ASSERT(datalen == 3);

  _g_sb_debug.ignore_replies = TRUE;
  result = DalSlimBus_ReadValueElement(hSB, intLA, addr+1, (uint8*)&datain, 3, &datalen, hEvent);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DALSYS_EventMultipleWait(&hEvent, 1, 10000, &uIdx);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DalSlimBus_GetLastError(hSB, &eError);
  SB_ASSERT(eError == SLIMBUS_ERR_MSG_RESP_TIMEOUT);

  /* Temp test for avail transaction blocking */
  result = DalSlimBus_ReadValueElement(hSB, intLA, addr, (uint8*)&datain, 1, &datalen, hEvent);
  SB_ASSERT(result == DAL_SUCCESS);

  result = DalSlimBus_ReadValueElement(hSB, intLA, addr+2, (uint8*)&datain2, 1, &datalen, NULL);
  SB_ASSERT(result == SLIMBUS_ERR_MSG_RESP_TIMEOUT);

  result = DALSYS_EventMultipleWait(&hEvent, 1, 10000, &uIdx);
  SB_ASSERT(result == DAL_SUCCESS);
  _g_sb_debug.ignore_replies = FALSE;
//#endif
  
  datain = 0;
  datalen = 0;
  result = DalSlimBus_ExchangeValueElement(hSB, intLA, addr, (uint8*)&data4, sizeof(data4), (uint8*)&datain, sizeof(datain), &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0xefcd1234);
  SB_ASSERT(datalen == sizeof(datain));
  
  result = DalSlimBus_ExchangeValueElement(hSB, intLA, addr, (uint8*)&datain, sizeof(datain), (uint8*)&datain, sizeof(datain), &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == data4);
  SB_ASSERT(datalen == sizeof(datain));

  result = DalSlimBus_ReadValueElement(hSB, intLA, addr, (uint8*)&datain, sizeof(datain), &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0xefcd1234);
  SB_ASSERT(datalen == sizeof(datain));
#endif // mjs
  
//  datain = 0;
//  result = DalSlimBus_ReadInfoElement(hSB, intLA, 0x8, (uint8*)&datain, 2, &datalen, NULL);
//  SB_ASSERT(result == DAL_SUCCESS);
//  SB_ASSERT(datain == 0xfd01);
//  SB_ASSERT(datalen == 2);
#if 0
  result = DalSlimBus_ReadInfoElement(hSB, genLA, 0x8, (uint8*)&datain, 2, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0x0001);
  SB_ASSERT(datalen == 2);
#endif
//  result = DalSlimBus_ReadInfoElement(hSB, 0xff, 0x8, (uint8*)&datain, 2, &datalen, NULL);
//  SB_ASSERT(result == DAL_SUCCESS);
//  SB_ASSERT(datain == 0xff01);
//  SB_ASSERT(datalen == 2);

  {
    #define NUM_CHANS 24
    #define NUM_PORTS 16
    #define NUM_MULTI_CHANS 3
    #define NUM_PC 6
    #define NUM_GROUPS 8
    SlimBusResourceHandle hPort[NUM_PORTS], hSlPort[NUM_PORTS], hCounter[NUM_PC];
    SlimBusResourceHandle hChannel[NUM_CHANS], hGroup[NUM_GROUPS];
    SlimBusResourceHandle hRxSharedChannel, hTxSharedChannel;
    SlimBusResourceHandle hSharedChannel;
    SlimBusResourceHandle hMultiPort[NUM_MULTI_CHANS];
    SlimBusPortConfigType Config;
    SlimBusPipeConfigType Pipe;
    SlimBusChannelReqType ChanReq;
    SlimBusBamRegisterEventType Reg;
    SlimBusBamIOVecType IOVec;
    //int i, j;
    static uint8 ucBuf1[96] =
    {
      0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
      0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0,
      0x0, 0x0, 0x1, 0x1, 0x2, 0x2, 0x3, 0x3, 0x4, 0x4, 0x5, 0x5, 0x6, 0x6, 0x7, 0x7,
      0x8, 0x8, 0x9, 0x9, 0xa, 0xa, 0xb, 0xb, 0xc, 0xc, 0xd, 0xd, 0xe, 0xe, 0xf, 0xf,
      0x0, 0x1, 0x2, 0x3, 0x1, 0x2, 0x3, 0x4, 0x2, 0x3, 0x4, 0x5, 0x3, 0x4, 0x5, 0x6,
      0x4, 0x5, 0x6, 0x7, 0x5, 0x6, 0x7, 0x8, 0x6, 0x7, 0x8, 0x9, 0x7, 0x8, 0x9, 0xa
    };
    static uint8 ucBuf2[96] =
    {
      0xf0, 0xe0, 0xd0, 0xc0, 0xb0, 0xa0, 0x90, 0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00,
      0x1f, 0x1e, 0x2d, 0x2c, 0x3b, 0x3a, 0x49, 0x48, 0x57, 0x56, 0x65, 0x64, 0x73, 0x72, 0x81, 0x80,
      0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07,
      0x18, 0x18, 0x29, 0x29, 0x3a, 0x3a, 0x4b, 0x4b, 0x5c, 0x5c, 0x6d, 0x6d, 0xae, 0xbe, 0xcf, 0xdf,
      0x1f, 0x1e, 0x2d, 0x2c, 0x3b, 0x3a, 0x49, 0x48, 0x57, 0x56, 0x65, 0x64, 0x73, 0x72, 0x81, 0x80,
      0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07,
    };
    uint8 *pucBuf1, *pucBuf2, *pucBuf3, *pucBuf4, *pucBuf5, *pucBuf6, *pucRxBuf;
    uint32 phBuf1, phBuf2, phBuf3, phBuf4, phBuf5, phBuf6, phRxBuf;
    CallbackDataType CbData, CbData1;
    SlimBusDataChannelStatusType ChanStatus;
    uint32 uFifoWord;
    uint32 i;
    DALBOOL bWatermark = FALSE;
    DALSYSMemHandle hPhysMem;
    DALSYSMemInfo MemInfo;
    
    result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                                   DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                   DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                   4096, &hPhysMem, NULL);    
    SB_ASSERT(result == DAL_SUCCESS);

    result = DALSYS_MemInfo(hPhysMem, &MemInfo);
    SB_ASSERT(result == DAL_SUCCESS);
    
    hDummyBuf = MemInfo.PhysicalAddr;
    memset((void*)MemInfo.VirtualAddr, 0, sizeof(ucDummyBuf));

    result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                                   DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                   DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                   4096, &hPhysMem, NULL);    
    SB_ASSERT(result == DAL_SUCCESS);
    
    result = DALSYS_MemInfo(hPhysMem, &MemInfo);
    SB_ASSERT(result == DAL_SUCCESS);

    pucBuf1 = (uint8*) MemInfo.VirtualAddr;
    phBuf1 = (uint32)pucBuf1 - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;
    memcpy(pucBuf1, ucBuf1, sizeof(ucBuf1));
    
    pucBuf2 = pucBuf1 + sizeof(ucBuf1);
    phBuf2 = (uint32)pucBuf2 - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;
    memcpy(pucBuf2, ucBuf2, sizeof(ucBuf2));
    
    pucBuf3 = pucBuf2 + sizeof(ucBuf2);
    phBuf3 = (uint32)pucBuf3 - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;
    
    pucBuf4 = pucBuf3 + 96;
    phBuf4 = (uint32)pucBuf4 - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;

    pucBuf5 = pucBuf4 + sizeof(ucBuf1);
    phBuf5 = (uint32)pucBuf5 - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;

    pucBuf6 = pucBuf5 + sizeof(ucBuf1);
    phBuf6 = (uint32)pucBuf6 - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;

    pucRxBuf = pucBuf4 + 96;
    phRxBuf = (uint32)pucRxBuf - MemInfo.VirtualAddr + MemInfo.PhysicalAddr;
    
    result = DALSYS_SetupCallbackEvent(hPortEvent, SlimBusPortCb, (void*)&CbData);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DALSYS_SetupCallbackEvent(hSimplePortEvent, SlimBusSimplePortCb, (void*)&CbData);
    SB_ASSERT(result == DAL_SUCCESS);
        
    result = DALSYS_SetupCallbackEvent(hPortEvent1, SlimBusPortCb, (void*)&CbData1);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DALSYS_SetupCallbackEvent(hSimplePortEvent1, SlimBusSimplePortCb, (void*)&CbData1);
    SB_ASSERT(result == DAL_SUCCESS);
    
    result = DalSlimBus_AllocMasterPortWithEndPoints(hSB, 4, 4, &hPort[0]);
    if (MsmData[uMsmIdx].uMaxEndPoints >= 4)
    {
      SB_ASSERT(result == DAL_SUCCESS);
    }
    else
    {
      SB_ASSERT(result == SLIMBUS_ERR_NOT_ENOUGH_ENDPOINTS);
    }
    /*ignore retval*/DalSlimBus_DeallocMasterPorts(hSB, &hPort[0], sizeof(hPort[0]));

    result = DalSlimBus_GetHardwareCapability(hSB, &eHwCap);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(eHwCap == MsmData[uMsmIdx].eHwCap);

    result = DalSlimBus_GetMaxEndPointsSupported(hSB, &uMaxEP);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(uMaxEP == MsmData[uMsmIdx].uMaxEndPoints);

    for (i=0; i < NUM_CHANS; i++)
    {
      result = DalSlimBus_AllocDataChannel(hSB, &hChannel[i]);
      SB_ASSERT(result == DAL_SUCCESS);
    }
    for (i=0; i < NUM_PORTS - NUM_MULTI_CHANS - NUM_LOOPBACK_PORTS; i++)
    {
      uint32 uPgdIdx = 0;
      
      /* Note: multi-channel groups should be allocated with a single call */
      result = DalSlimBus_AllocMasterPorts(hSB, SLIMBUS_PORT_REQ_O_DEFAULT, 4, &hPort[i], sizeof(hPort[0]));
      SB_ASSERT(result == DAL_SUCCESS);
      
      result = DalSlimBus_GetPortIndex(hSB, hPort[i], NULL, &uPgdIdx);
      SB_ASSERT(result == DAL_SUCCESS);
      SB_ASSERT(uPgdIdx == ((hPort[i] & 0x1f) + MsmData[uMsmIdx].uPortBase));
      MSG_2(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "Port handle 0x%x has index %d", hPort[i], uPgdIdx);
    }
    
    result = DalSlimBus_AllocMasterPorts(hSB, SLIMBUS_PORT_REQ_O_MULTI_CHAN_GROUP,
                                         4, hMultiPort, sizeof(hMultiPort));
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_AllocProgressCounter(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &hCounter[0]);
    SB_ASSERT(result == DAL_SUCCESS);
    
    result = DalSlimBus_InitProgressCounter(hSB, hCounter[0], 0);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_GetDataChannelStatus(hSB, hChannel[0], &ChanStatus);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_REMOVED);
    
    bWatermark = FALSE;
    result = DalSlimBus_GetPortFifoStatus(hSB, hPort[0], SLIMBUS_BAM_DEFAULT,
                                           &uFifoWord, &bWatermark);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(uFifoWord == 0);
    SB_ASSERT(bWatermark == FALSE);

    CbData.hSB = hSB;
    CbData.phPort = &hPort[0];
    CbData.hLoopPort = hPort[1];
    CbData.uNumPorts = 1;
    CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData.hCounter = hCounter[0];
    CbData.hChannel = hChannel[0];
    CbData.hBamEvent = hBamEvent;
    CbData.hPortEvent = hEvent;
    CbData.uBamCount = 10;
    CbData.uNumWords = CbData.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData.bConnected = TRUE;

    CbData1.hSB = hSB;
    CbData1.phPort = &hPort[1];
    CbData1.hLoopPort = hPort[0];
    CbData1.uNumPorts = 1;
    CbData1.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData1.hCounter = hCounter[0];
    CbData1.hChannel = hChannel[0];
    CbData1.hBamEvent = hBamEvent;
    CbData1.hPortEvent = hEvent1;
    CbData1.uBamCount = 10;
    CbData1.uNumWords = CbData1.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData1.bConnected = TRUE;

    MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "expected BAM descriptor rate: 4ms");

    Config.eOptions = SLIMBUS_PORT_O_PACKED;
    Config.uBlockSize = 0;
    Config.uTransSize = 0;
    Config.uWaterMark = 4;
    Config.uUpperWaterMark = 8;
    Config.bUpperWaterMarkValid = TRUE;
    //for (i=0; i < NUM_CHANS; i++)
    {
      memset(&Pipe, 0, sizeof(Pipe));
//      for (j = 0; j < sizeof(ucBuf1)/sizeof(ucBuf1[0]); j++)
//      {
//        ucBuf1[j] = i&0x3;
//      }
      //for (j = 0; j < sizeof(ucBuf2)/sizeof(ucBuf2[0]); j++)
      //{
      //  ucBuf2[j] = i&0xC;
      //}
      //memory_barrier();
      //Pipe.Data.uSize = 0x1000;
      Pipe.DescFifo.uMinSize = 64;
      //DALSYS_Malloc(Pipe.Data.uSize, &Pipe.Data.pBase);
      //Pipe.Data.uPhysBase = (uint32)Pipe.Data.pBase;
      Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
      Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);
      Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
      Reg.hEvent = hBamEvent;
      Reg.pUser = NULL;
      result = DalSlimBus_ConfigMasterPort(hSB, hPort[0], &Config);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], hPortEvent, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_ConfigMasterPipe(hSB, hPort[0], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_RegisterBamEvent(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &Reg);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[0], SLIMBUS_PORT_SOURCE_FLOW, hPort[0]);
      SB_ASSERT(result == DAL_SUCCESS);
      
      IOVec.uAddr = phBuf1;
      IOVec.uSize = sizeof(ucBuf1);
      IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      IOVec.uAddr = phBuf2;
      IOVec.uSize = sizeof(ucBuf2);
      IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      
      result = DalSlimBus_ConfigMasterPort(hSB, hPort[1], &Config);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], hPortEvent1, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_ConfigMasterPipe(hSB, hPort[1], SLIMBUS_PORT_SINK_FLOW, &Pipe);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_RegisterBamEvent(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &Reg);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[0], SLIMBUS_PORT_SINK_FLOW, hPort[1]);
      SB_ASSERT(result == DAL_SUCCESS);

      IOVec.uAddr = phRxBuf;
      IOVec.uSize = sizeof(ucBuf1);
      IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      IOVec.uAddr = phRxBuf + sizeof(ucBuf1);
      IOVec.uSize = sizeof(ucBuf2);
      IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
    }
    
    ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
    ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
    ChanReq.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;
    ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
    ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;

    ChanReq.uRateMultiplier = 6;
    ChanReq.uSampleSize_bits = 8;
    result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[0], &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);
        
    //result = DalSlimBus_NextDataChannelControl(hSB,  hChannel[0], SLIMBUS_CHAN_CTRL_ACTIVATE);
    //SB_ASSERT(result == DAL_ERROR);
    
    bWatermark = FALSE;
    for (i = 0; !bWatermark && i < 1000; i++)
    {
      result = DalSlimBus_GetPortFifoStatus(hSB, hPort[0], SLIMBUS_BAM_DEFAULT,
                                            &uFifoWord, &bWatermark);
    }
    SB_ASSERT(result == DAL_SUCCESS);
    //SB_ASSERT(uFifoWord > 0);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);
    
    SlimBusTestProcessBamLoopbackEvents(&CbData);

    result = DalSlimBus_GetDataChannelStatus(hSB, hChannel[0], &ChanStatus);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE);

    DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET);
    DALSYS_EventCtrl(hEvent1, DALSYS_EVENT_CTRL_RESET);
    
    result = DalSlimBus_NextDataChannelControl(hSB, hChannel[0], SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);
    DALSYS_EventWait(hEvent1);
    
    
    result = DalSlimBus_NextReserveMsgBandwidth(hSB, 16000000);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_NextReserveMsgBandwidth(hSB, 0);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    
    result = DalSlimBus_DeallocMasterPorts(hSB, &hPort[0], sizeof(hPort[0]));
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DeallocMasterPorts(hSB, &hPort[1], sizeof(hPort[0]));
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DeallocProgressCounter(hSB, hCounter[0]);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_AllocMasterPorts(hSB, SLIMBUS_PORT_REQ_O_DEFAULT, 4, &hPort[0], sizeof(hPort[0]));
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_AllocMasterPorts(hSB, SLIMBUS_PORT_REQ_O_DEFAULT, 4, &hPort[1], sizeof(hPort[0]));
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_AllocProgressCounter(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &hCounter[0]);
    SB_ASSERT(result == DAL_SUCCESS);


    result = DalSlimBus_InitProgressCounter(hSB, hCounter[0], 0);
    SB_ASSERT(result == DAL_SUCCESS);

    CbData.hSB = hSB;
    CbData.phPort = &hPort[1];
    CbData.hLoopPort = hPort[0];
    CbData.uNumPorts = 1;
    CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData.hCounter = hCounter[0];
    CbData.hChannel = hChannel[0];
    CbData.hBamEvent = hBamEvent;
    CbData.hPortEvent = hEvent;
    CbData.uBamCount = 10;
    CbData.uNumWords = CbData.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData.bConnected = TRUE;

    CbData1.hSB = hSB;
    CbData1.phPort = &hPort[0];
    CbData1.hLoopPort = hPort[1];
    CbData1.uNumPorts = 1;
    CbData1.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData1.hCounter = hCounter[0];
    CbData1.hChannel = hChannel[0];
    CbData1.hBamEvent = hBamEvent;
    CbData1.hPortEvent = hEvent1;
    CbData1.uBamCount = 10;
    CbData1.uNumWords = CbData1.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData1.bConnected = TRUE;

    Config.eOptions = SLIMBUS_PORT_O_PACKED;
    Config.uBlockSize = 0;
    Config.uTransSize = 0;
    Config.uWaterMark = 4;
    Config.bUpperWaterMarkValid = FALSE;
    memset(&Pipe, 0, sizeof(Pipe));
        
    MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "expected BAM descriptor rate: 2ms");

    Pipe.DescFifo.uMinSize = 64;
    Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
    Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);
    Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
    Reg.hEvent = hBamEvent;
    Reg.pUser = NULL;
    result = DalSlimBus_ConfigMasterPort(hSB, hPort[1], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], hPortEvent, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[1], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[0], SLIMBUS_PORT_SOURCE_FLOW, hPort[1]);
    SB_ASSERT(result == DAL_SUCCESS);
            
    IOVec.uAddr = phBuf1;
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phBuf2;
    IOVec.uSize = sizeof(ucBuf2);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
            
    result = DalSlimBus_ConfigMasterPort(hSB, hPort[0], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], hPortEvent1, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[0], SLIMBUS_PORT_SINK_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[0], SLIMBUS_PORT_SINK_FLOW, hPort[0]);
    SB_ASSERT(result == DAL_SUCCESS);
                
    memset(pucRxBuf, 0, sizeof(ucBuf1)+sizeof(ucBuf2));
    IOVec.uAddr = phRxBuf;
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phRxBuf + sizeof(ucBuf1);
    IOVec.uSize = sizeof(ucBuf2);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    
    ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_1_HZ;
    ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
    ChanReq.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;
    ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
    ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;

    ChanReq.uRateMultiplier = 48000;
    ChanReq.uSampleSize_bits = 8;
    result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[0], &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);

    bWatermark = FALSE;
    for (i = 0; !bWatermark && i < 1000; i++)
    {
      result = DalSlimBus_GetPortFifoStatus(hSB, hPort[1], SLIMBUS_BAM_DEFAULT,
                                            &uFifoWord, &bWatermark);
    }
    SB_ASSERT(result == DAL_SUCCESS);
    //SB_ASSERT(uFifoWord > 0);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    SlimBusTestProcessBamLoopbackEvents(&CbData);

    result = DalSlimBus_GetDataChannelStatus(hSB, hChannel[0], &ChanStatus);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE);

    DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET);
    DALSYS_EventCtrl(hEvent1, DALSYS_EVENT_CTRL_RESET);

    result = DalSlimBus_NextDataChannelControl(hSB, hChannel[0], SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);
    DALSYS_EventWait(hEvent1);

        
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ////
    result = DalSlimBus_AllocSharedDataChannel(hSB, 129, &hSharedChannel);
    SB_ASSERT(result == DAL_SUCCESS);

    CbData.hSB = hSB;
    CbData.phPort = &hPort[0];
    CbData.hLoopPort = hPort[1];
    CbData.uNumPorts = 1;
    CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData.hCounter = hCounter[0];
    CbData.hChannel = hSharedChannel;
    CbData.hBamEvent = hBamEvent;
    CbData.hPortEvent = hEvent;
    CbData.uBamCount = 10;
    CbData.uNumWords = CbData.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData.bConnected = TRUE;

    CbData1.hSB = hSB;
    CbData1.phPort = &hPort[1];
    CbData1.hLoopPort = hPort[0];
    CbData1.uNumPorts = 1;
    CbData1.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData1.hCounter = hCounter[0];
    CbData1.hChannel = hSharedChannel;
    CbData1.hBamEvent = hBamEvent;
    CbData1.hPortEvent = hEvent1;
    CbData1.uBamCount = 10;
    CbData1.uNumWords = CbData1.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData1.bConnected = TRUE;

    Config.eOptions = SLIMBUS_PORT_O_PACKED;
    Config.uBlockSize = 0;
    Config.uTransSize = 0;
    Config.uWaterMark = 4;

    memset(&Pipe, 0, sizeof(Pipe));
    Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
    Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);

    Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
    Reg.hEvent = hBamEvent;
    Reg.pUser = NULL;
    
    MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "expected BAM descriptor rate: 4ms");

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[0], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], hPortEvent, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[0], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hSharedChannel, SLIMBUS_PORT_SOURCE_FLOW, hPort[0]);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_InitProgressCounter(hSB, hCounter[0], 0);
    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phBuf1;
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phBuf2;
    IOVec.uSize = sizeof(ucBuf2);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[1], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], hPortEvent1, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[1], SLIMBUS_PORT_SINK_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hSharedChannel, SLIMBUS_PORT_SINK_FLOW, hPort[1]);
    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phRxBuf;
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phRxBuf + sizeof(ucBuf1);
    IOVec.uSize = sizeof(ucBuf2);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
    ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
    ChanReq.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;
    ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
    ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;

    ChanReq.uRateMultiplier = 6;
    ChanReq.uSampleSize_bits = 8;
    result = DalSlimBus_NextDefineDataChannel(hSB, hSharedChannel, &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);

    //result = DalSlimBus_NextDataChannelControl(hSB,  hChannel[0], SLIMBUS_CHAN_CTRL_ACTIVATE);
    //SB_ASSERT(result == DAL_ERROR);

    bWatermark = FALSE;
    for (i = 0; !bWatermark && i < 1000; i++)
    {
      result = DalSlimBus_GetPortFifoStatus(hSB, hPort[0], SLIMBUS_BAM_DEFAULT,
                                            &uFifoWord, &bWatermark);
    }
    SB_ASSERT(result == DAL_SUCCESS);
    //SB_ASSERT(uFifoWord > 0);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    SlimBusTestProcessBamLoopbackEvents(&CbData);

    result = DalSlimBus_GetDataChannelStatus(hSB, hSharedChannel, &ChanStatus);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE);

    DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET);
    DALSYS_EventCtrl(hEvent1, DALSYS_EVENT_CTRL_RESET);

    result = DalSlimBus_NextDataChannelControl(hSB, hSharedChannel, SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);
    DALSYS_EventWait(hEvent1);
    
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ////
    
    CbData.hSB = hSB;
    CbData.phPort = &hPort[0];
    CbData.hLoopPort = hPort[1];
    CbData.uNumPorts = 1;
    CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData.hCounter = hCounter[0];
    CbData.hChannel = hSharedChannel;
    CbData.hBamEvent = hBamEvent;
    CbData.hPortEvent = hEvent;
    CbData.uBamCount = 10;
    CbData.uNumWords = CbData.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData.bConnected = TRUE;

    CbData1.hSB = hSB;
    CbData1.phPort = &hPort[1];
    CbData1.hLoopPort = hPort[0];
    CbData1.uNumPorts = 1;
    CbData1.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData1.hCounter = hCounter[0];
    CbData1.hChannel = hSharedChannel;
    CbData1.hBamEvent = hBamEvent;
    CbData1.hPortEvent = hEvent1;
    CbData1.uBamCount = 10;
    CbData1.uNumWords = CbData1.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData1.bConnected = TRUE;

    Config.eOptions = SLIMBUS_PORT_O_PACKED;
    Config.uBlockSize = 0;
    Config.uTransSize = 0;
    Config.uWaterMark = 4;

    memset(&Pipe, 0, sizeof(Pipe));
    Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
    Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);

    Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
    Reg.hEvent = hBamEvent;
    Reg.pUser = NULL;

    MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "expected BAM descriptor rate: 5ms");

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[0], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], hPortEvent, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[0], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hSharedChannel, SLIMBUS_PORT_SOURCE_FLOW, hPort[0]);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_InitProgressCounter(hSB, hCounter[0], 0);
    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phBuf1;
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phBuf2;
    IOVec.uSize = sizeof(ucBuf2);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[1], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], hPortEvent1, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[1], SLIMBUS_PORT_SINK_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hSharedChannel, SLIMBUS_PORT_SINK_FLOW, hPort[1]);
    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phRxBuf;
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phRxBuf + sizeof(ucBuf1);
    IOVec.uSize = sizeof(ucBuf2);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_1_HZ;
    ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
    ChanReq.eDesiredProtocol = SLIMBUS_PROTO_PUSHED;
    ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
    ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;

    ChanReq.uRateMultiplier = 19200;
    ChanReq.uSampleSize_bits = 8;
    result = DalSlimBus_NextDefineDataChannel(hSB, hSharedChannel, &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);

    //result = DalSlimBus_NextDataChannelControl(hSB,  hChannel[0], SLIMBUS_CHAN_CTRL_ACTIVATE);
    //SB_ASSERT(result == DAL_ERROR);

    bWatermark = FALSE;
    for (i = 0; !bWatermark && i < 1000; i++)
    {
      result = DalSlimBus_GetPortFifoStatus(hSB, hPort[0], SLIMBUS_BAM_DEFAULT,
                                            &uFifoWord, &bWatermark);
    }
    SB_ASSERT(result == DAL_SUCCESS);
    //SB_ASSERT(uFifoWord > 0);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    SlimBusTestProcessBamLoopbackEvents(&CbData);

    result = DalSlimBus_GetDataChannelStatus(hSB, hSharedChannel, &ChanStatus);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE);

    DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET);
    DALSYS_EventCtrl(hEvent1, DALSYS_EVENT_CTRL_RESET);

    result = DalSlimBus_NextDataChannelControl(hSB, hSharedChannel, SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);
    DALSYS_EventWait(hEvent1);
    
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ////


    CbData.hSB = hSB;
    CbData.phPort = &hPort[0];
    CbData.hLoopPort = hPort[1];
    CbData.uNumPorts = 1;
    CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData.hCounter = hCounter[0];
    CbData.hChannel = hSharedChannel;
    CbData.hBamEvent = hBamEvent;
    CbData.hPortEvent = hEvent;
    CbData.uBamCount = 10;
    CbData.uNumWords = CbData.uBamCount*84/sizeof(uint32);
    CbData.bConnected = TRUE;

    CbData1.hSB = hSB;
    CbData1.phPort = &hPort[1];
    CbData1.hLoopPort = hPort[0];
    CbData1.uNumPorts = 1;
    CbData1.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData1.hCounter = hCounter[0];
    CbData1.hChannel = hSharedChannel;
    CbData1.hBamEvent = hBamEvent;
    CbData1.hPortEvent = hEvent1;
    CbData1.uBamCount = 10;
    CbData1.uNumWords = CbData1.uBamCount*84/sizeof(uint32);
    CbData1.bConnected = TRUE;

    Config.eOptions = SLIMBUS_PORT_O_PACKED;
    Config.uBlockSize = 0;
    Config.uTransSize = 0;
    Config.uWaterMark = 4;

    memset(&Pipe, 0, sizeof(Pipe));
    Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
    Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);

    Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
    Reg.hEvent = hBamEvent;
    Reg.pUser = NULL;

    MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "expected BAM descriptor rate: 6ms");

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[0], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], hPortEvent, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[0], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hSharedChannel, SLIMBUS_PORT_SOURCE_FLOW, hPort[0]);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_InitProgressCounter(hSB, hCounter[0], 0);
    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phBuf1;
    IOVec.uSize = 84; /* 84 = 14000 * 6 / 1000 */
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phBuf2;
    IOVec.uSize = 84;
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[1], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], hPortEvent1, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[1], SLIMBUS_PORT_SINK_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hSharedChannel, SLIMBUS_PORT_SINK_FLOW, hPort[1]);
    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phRxBuf;
    IOVec.uSize = 84;
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phRxBuf + sizeof(ucBuf1);
    IOVec.uSize = 84;
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_1_HZ;
    ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
    ChanReq.eDesiredProtocol = SLIMBUS_PROTO_PULLED;
    ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
    ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;

    ChanReq.uRateMultiplier = 14000;
    ChanReq.uSampleSize_bits = 8;
    result = DalSlimBus_NextDefineDataChannel(hSB, hSharedChannel, &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);

    //result = DalSlimBus_NextDataChannelControl(hSB,  hChannel[0], SLIMBUS_CHAN_CTRL_ACTIVATE);
    //SB_ASSERT(result == DAL_ERROR);

    bWatermark = FALSE;
    for (i = 0; !bWatermark && i < 1000; i++)
    {
      result = DalSlimBus_GetPortFifoStatus(hSB, hPort[0], SLIMBUS_BAM_DEFAULT,
                                            &uFifoWord, &bWatermark);
    }
    SB_ASSERT(result == DAL_SUCCESS);
    //SB_ASSERT(uFifoWord > 0);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    SlimBusTestProcessBamLoopbackEvents(&CbData);

    result = DalSlimBus_GetDataChannelStatus(hSB, hSharedChannel, &ChanStatus);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE);

    DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET);
    DALSYS_EventCtrl(hEvent1, DALSYS_EVENT_CTRL_RESET);

    result = DalSlimBus_NextDataChannelControl(hSB, hSharedChannel, SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);
    DALSYS_EventWait(hEvent1);
    
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ////

    result = DalSlimBus_DeallocDataChannel(hSB, hSharedChannel);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_AllocSharedDataChannel(hSB, 128, &hSharedChannel);
    SB_ASSERT(result == DAL_SUCCESS);

    CbData.hSB = hSB;
    CbData.phPort = &hPort[0];
    CbData.hLoopPort = hPort[1];
    CbData.uNumPorts = 1;
    CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData.hCounter = hCounter[0];
    CbData.hChannel = hSharedChannel;
    CbData.hBamEvent = hBamEvent;
    CbData.hPortEvent = hEvent;
    CbData.uBamCount = 2;
    CbData.uNumWords = CbData.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData.bConnected = TRUE;

    Config.eOptions = SLIMBUS_PORT_O_PACKED;
    Config.uBlockSize = 0;
    Config.uTransSize = 0;
    Config.uWaterMark = 4;
    
    memset(&Pipe, 0, sizeof(Pipe));
    Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
    Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);
    
    Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
    Reg.hEvent = hBamEvent;
    Reg.pUser = NULL;
    
    MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "expected BAM descriptor rate: 2ms");

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[0], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], hPortEvent, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[0], SLIMBUS_PORT_SINK_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hSharedChannel, SLIMBUS_PORT_SINK_FLOW, hPort[0]);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_InitProgressCounter(hSB, hCounter[0], 0);
    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phRxBuf;
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phRxBuf + sizeof(ucBuf1);
    IOVec.uSize = sizeof(ucBuf2);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
    ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
    ChanReq.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;
    ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
    ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;

    ChanReq.uRateMultiplier = 12;
    ChanReq.uSampleSize_bits = 24;
    result = DalSlimBus_NextDefineDataChannel(hSB, hSharedChannel, &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);

    //result = DalSlimBus_NextDataChannelControl(hSB,  hChannel[0], SLIMBUS_CHAN_CTRL_ACTIVATE);
    //SB_ASSERT(result == DAL_ERROR);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    SlimBusTestProcessBamEvents(&CbData);

    result = DalSlimBus_GetDataChannelStatus(hSB, hSharedChannel, &ChanStatus);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE);

    DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET);

    result = DalSlimBus_NextDataChannelControl(hSB, hSharedChannel, SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ////
    
    result = DalSlimBus_DeallocDataChannel(hSB, hSharedChannel);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_AllocSharedDataChannel(hSB, 128, &hRxSharedChannel);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_AllocSharedDataChannel(hSB, 129, &hTxSharedChannel);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYSWorkLoopHandle hWorkLoop;
    DALSYSEventHandle hWorkLoopEvent;

    DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hNormalEvent, NULL);

  //  pszWorkLoopName = "slimbus_test";
    DALSYS_RegisterWorkLoop(0, 4, &hWorkLoop, NULL);
    DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT, &hWorkLoopEvent, NULL);
    DALSYS_AddEventToWorkLoop(hWorkLoop, SlimBusTestWorkLoopHandler,
                              NULL, hWorkLoopEvent, NULL);
    DALSYS_EventCtrl(hWorkLoopEvent, DALSYS_EVENT_CTRL_TRIGGER);

    DALSYS_EventWait(hNormalEvent);
    
    CbData.hSB = hSB;
    CbData.phPort = &hPort[0];
    CbData.hLoopPort = hPort[1];
    CbData.uNumPorts = 1;
    CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData.hCounter = hCounter[0];
    CbData.hChannel = hTxSharedChannel;
    CbData.hBamEvent = hBamEvent;
    CbData.hPortEvent = hEvent;
    CbData.uBamCount = 10;
    CbData.uNumWords = CbData.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData.bConnected = TRUE;

    CbData1.hSB = hSB;
    CbData1.phPort = &hPort[1];
    CbData1.hLoopPort = hPort[0];
    CbData1.uNumPorts = 1;
    CbData1.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData1.hCounter = hCounter[0];
    CbData1.hChannel = hRxSharedChannel;
    CbData1.hBamEvent = hBamEvent;
    CbData1.hPortEvent = hEvent1;
    CbData1.uBamCount = 10;
    CbData1.uNumWords = CbData1.uBamCount*sizeof(ucBuf1)/sizeof(uint32);
    CbData1.bConnected = TRUE;

    Config.eOptions = SLIMBUS_PORT_O_PACKED;
    Config.uBlockSize = 0;
    Config.uTransSize = 0;
    Config.uWaterMark = 4;
    
    memset(&Pipe, 0, sizeof(Pipe));
    Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
    Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);
    
    Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
    Reg.hEvent = hBamEvent;
    Reg.pUser = NULL;
    
    MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "expected BAM descriptor rate: 3ms");

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[0], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], hPortEvent, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[0], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hTxSharedChannel, SLIMBUS_PORT_SOURCE_FLOW, hPort[0]);
    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phBuf1;
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phBuf1;
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[1], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], hPortEvent1, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[1], SLIMBUS_PORT_SINK_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hRxSharedChannel, SLIMBUS_PORT_SINK_FLOW, hPort[1]);
    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phRxBuf;
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phRxBuf + sizeof(ucBuf1);
    IOVec.uSize = sizeof(ucBuf1);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
    ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
    ChanReq.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;
    ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
    ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;

    ChanReq.uRateMultiplier = 6;
    ChanReq.uSampleSize_bits = 8;
    result = DalSlimBus_NextDefineDataChannel(hSB, hRxSharedChannel, &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_NextDefineDataChannel(hSB, hTxSharedChannel, &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);

    //result = DalSlimBus_NextDataChannelControl(hSB,  hChannel[0], SLIMBUS_CHAN_CTRL_ACTIVATE);
    //SB_ASSERT(result == DAL_ERROR);

    bWatermark = FALSE;
    for (i = 0; !bWatermark && i < 1000; i++)
    {
      result = DalSlimBus_GetPortFifoStatus(hSB, hPort[0], SLIMBUS_BAM_DEFAULT,
                                            &uFifoWord, &bWatermark);
    }
    SB_ASSERT(result == DAL_SUCCESS);
    //SB_ASSERT(uFifoWord > 0);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    SlimBusTestProcessBamLoopback3Events(&CbData);

    result = DalSlimBus_GetDataChannelStatus(hSB, hTxSharedChannel, &ChanStatus);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE);
    result = DalSlimBus_GetDataChannelStatus(hSB, hRxSharedChannel, &ChanStatus);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE);

    result = DalSlimBus_NextDataChannelControl(hSB, hTxSharedChannel, SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_NextDataChannelControl(hSB, hRxSharedChannel, SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);
    DALSYS_EventWait(hEvent1);
    
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[0], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], NULL, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hNormalEvent);

#if 0
    _g_sb_debug.ignore_replies = 1;
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == SLIMBUS_ERR_MSG_RESP_TIMEOUT);
    _g_sb_debug.ignore_replies = 0;
    
    DALSYS_EventWait(hEvent);
#endif

    result = DalSlimBus_DeallocDataChannel(hSB, hTxSharedChannel);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DeallocDataChannel(hSB, hRxSharedChannel);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_GetDataChannelStatus(hSB, hChannel[0], &ChanStatus);
    SB_ASSERT(result == DAL_SUCCESS);
    SB_ASSERT(ChanStatus == SLIMBUS_CHANNEL_STATUS_REMOVED);

    DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET);
    DALSYS_EventCtrl(hEvent1, DALSYS_EVENT_CTRL_RESET);

#if 0
    /* Test port connection/disconnection */
    result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[0], SLIMBUS_PORT_SOURCE_FLOW, hPort[0]);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEventEx(hSB, hPort[0], hEvent, NULL, SLIMBUS_EVENT_O_PORT_DISCONNECT);
    result = DalSlimBus_DisconnectPortFromChannel(hSB, hPort[0]);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);
#endif

    result = DalSlimBus_AllocProgressCounter(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &hCounter[1]);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_InitProgressCounter(hSB, hCounter[1], 0);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_AllocResourceGroup(hSB, &hGroup[0]);
    SB_ASSERT(result == DAL_SUCCESS);
    
    result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[1]);
    SB_ASSERT(result == DAL_SUCCESS);
    
    CbData.hSB = hSB;
    CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData.hBamEvent = hBamEvent;
    CbData.hPortEvent = hEvent;
    
    CbData.phPort = &hPort[1];
    CbData.uNumPorts = 1;
    CbData.hChannel = hGroup[0];//hChannel[1];
    CbData.hCounter = hCounter[1];
    CbData.uBamCount = 20;
    CbData.uNumWords = CbData.uBamCount*sizeof(ucBuf2)/sizeof(uint32);
    CbData.bConnected = TRUE;
    
    memset(&Pipe, 0, sizeof(Pipe));
    Pipe.DescFifo.uMinSize = 64;
    Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
    Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);

    Config.eOptions = SLIMBUS_PORT_O_PACKED;
    Config.uBlockSize = 0;
    Config.uTransSize = 0;
    Config.uWaterMark = 4;
        
    Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
    Reg.hEvent = hBamEvent;
    Reg.pUser = NULL;

    MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "expected BAM descriptor rate: 3ms");

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[1], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[1], hPortEvent, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[1], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[1], SLIMBUS_PORT_SOURCE_FLOW, hPort[1]);
    SB_ASSERT(result == DAL_SUCCESS);
//    result = DalSlimBus_GetSlavePortHandle(hSB, genLA, 1, &hSlPort[1]);
//    SB_ASSERT(result == DAL_SUCCESS);
//    result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[1], SLIMBUS_PORT_SINK_FLOW, hSlPort[1]);
//    SB_ASSERT(result == DAL_SUCCESS);
    
    IOVec.uAddr = phBuf2;
    IOVec.uSize = sizeof(ucBuf2);
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
    ChanReq.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;
    ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
    ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;
            
    ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
    ChanReq.uRateMultiplier = 8;
    ChanReq.uSampleSize_bits = 8;
    result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[0], &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);
                    
    bWatermark = FALSE;
    for (i = 0; !bWatermark && i < 1000; i++)
    {
      result = DalSlimBus_GetPortFifoStatus(hSB, hPort[1], SLIMBUS_BAM_DEFAULT,
                                            &uFifoWord, &bWatermark);
    }
    SB_ASSERT(result == DAL_SUCCESS);
    //SB_ASSERT(uFifoWord > 0);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    SlimBusTestProcessBamEvents(&CbData);

    result = DalSlimBus_NextDataChannelControl(hSB, hGroup[0], SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);
                        
    result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[0]);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_AllocProgressCounter(hSB, hPort[2], SLIMBUS_BAM_DEFAULT, &hCounter[2]);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_InitProgressCounter(hSB, hCounter[2], 0);
    SB_ASSERT(result == DAL_SUCCESS);

    CbData.hSB = hSB;
    CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
    CbData.hBamEvent = hBamEvent;
    CbData.hPortEvent = hEvent;
    
    CbData.phPort = &hPort[2];
    CbData.uNumPorts = 1;
    CbData.hChannel = hChannel[2];
    CbData.hCounter = hCounter[2];
    CbData.uBamCount = 20;
    CbData.uNumWords = CbData.uBamCount*96/sizeof(uint32);
    CbData.bConnected = TRUE;

    memset(&Pipe, 0, sizeof(Pipe));
    Pipe.DescFifo.uMinSize = 64;
    Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
    Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);

    Config.eOptions = SLIMBUS_PORT_O_PACKED;
    Config.uBlockSize = 0;
    Config.uTransSize = 0;
    Config.uWaterMark = 8;

    Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
    Reg.hEvent = hBamEvent;
    Reg.pUser = NULL;

    MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "expected BAM descriptor rate: 8ms");

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[2], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[2], hPortEvent, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[2], SLIMBUS_PORT_SINK_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterBamEvent(hSB, hPort[2], SLIMBUS_BAM_DEFAULT, &Reg);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[2], SLIMBUS_PORT_SINK_FLOW, hPort[2]);
    SB_ASSERT(result == DAL_SUCCESS);
//    result = DalSlimBus_GetSlavePortHandle(hSB, genLA, 2, &hSlPort[2]);
//    SB_ASSERT(result == DAL_SUCCESS);
//    result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[2], SLIMBUS_PORT_SOURCE_FLOW, hSlPort[2]);
//    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phBuf3;
    IOVec.uSize = 96;
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[2], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    IOVec.uAddr = phBuf4;
    IOVec.uSize = 96;
    IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[2], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
    ChanReq.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;
    ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
    ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;

    ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
    ChanReq.uRateMultiplier = 3;
    ChanReq.uSampleSize_bits = 8;
    result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[2], &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);

    result = DalSlimBus_NextReserveMsgBandwidth(hSB, 1000000);
    SB_ASSERT(result == DAL_SUCCESS);
    
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    SlimBusTestProcessBamEvents(&CbData);

    result = DalSlimBus_NextDataChannelControl(hSB, hChannel[2], SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_NextReserveMsgBandwidth(hSB, 0);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);

#if 0
    CbData.phPort = &hPort[3];
    CbData.uNumPorts = 1;
    CbData.hChannel = hChannel[3];
    CbData.hCounter = NULL;
    CbData.uBamCount = 2;
    CbData.uNumWords = CbData.uBamCount*sizeof(ucBuf2)/sizeof(uint32);
    CbData.bConnected = TRUE;

    Config.eOptions = SLIMBUS_PORT_O_PACKED;
    Config.uBlockSize = 0;
    Config.uTransSize = 0;
    Config.uWaterMark = 8;
    Pipe.eOptions = 0;

    result = DalSlimBus_ConfigMasterPort(hSB, hPort[3], &Config);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_RegisterPortEvent(hSB, hPort[3], hSimplePortEvent, NULL);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConfigMasterPipe(hSB, hPort[3], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[3], SLIMBUS_PORT_SOURCE_FLOW, hPort[3]);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_GetSlavePortHandle(hSB, genLA, 3, &hSlPort[3]);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[3], SLIMBUS_PORT_SINK_FLOW, hSlPort[3]);
    SB_ASSERT(result == DAL_SUCCESS);

    IOVec.uAddr = phBuf2;
    IOVec.uSize = sizeof(ucBuf2);
    IOVec.uFlags = NULL;
    result = DalSlimBus_SubmitBamTransfer(hSB, hPort[3], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
    SB_ASSERT(result == DAL_SUCCESS);

    ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
    ChanReq.uRateMultiplier = 8;
    ChanReq.uSampleSize_bits = 8;
    result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[3], &ChanReq);
    SB_ASSERT(result == DAL_SUCCESS);

    bWatermark = FALSE;
    for (i = 0; !bWatermark && i < 1000; i++)
    {
      result = DalSlimBus_GetPortFifoStatus(hSB, hPort[3], SLIMBUS_BAM_DEFAULT,
                                            &uFifoWord, &bWatermark);
    }
    SB_ASSERT(result == DAL_SUCCESS);
    //SB_ASSERT(uFifoWord > 0);

    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    SlimBusTestProcessUnderflowEvents(&CbData);

    result = DalSlimBus_NextDataChannelControl(hSB, hChannel[3], SLIMBUS_CHAN_CTRL_REMOVE);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DoReconfigureNow(hSB);
    SB_ASSERT(result == DAL_SUCCESS);

    DALSYS_EventWait(hEvent);
#endif

    Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);
    /* Just test channel scheduling algorithm, no port/bam configuration */
    
//    if (g_sb_test_case == 0)
    {
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 6;
      ChanReq.uSampleSize_bits = 4;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[0], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      ChanReq.uRateMultiplier = 4;//24;
      ChanReq.uSampleSize_bits = 8;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[1], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[2], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[3], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[4], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      for (i = 0; i < 5; i++)
      {
        result = DalSlimBus_NextDataChannelControl(hSB, hChannel[i], SLIMBUS_CHAN_CTRL_REMOVE);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }

//    else if (g_sb_test_case == 1)
    {
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 6;
      ChanReq.uSampleSize_bits = 4;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[0], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      ChanReq.uRateMultiplier = 8;//24;
      ChanReq.uSampleSize_bits = 12;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[1], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[2], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[3], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[4], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      for (i = 0; i < 5; i++)
      {
        result = DalSlimBus_NextDataChannelControl(hSB, hChannel[i], SLIMBUS_CHAN_CTRL_REMOVE);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }
//    else if (g_sb_test_case == 2)
    {
      /* Audio UC #1 */
      
      for (i = 0; i < 6; i++ )
      {
        result = DalSlimBus_AllocResourceGroup(hSB, &hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }

      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 480000);
      SB_ASSERT(result == DAL_SUCCESS);
      
      /* acoustic touch screen */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      ChanReq.eOptions = (SlimBusChannelOptionsType)(SLIMBUS_CHAN_O_AUTO_ACTIVATE|SLIMBUS_CHAN_O_PRESERVE_ORDERING);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[5], hChannel[13]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[5], hChannel[14]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[5], &ChanReq);
      
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* alert */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 24;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[4]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[5]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[1], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
      
      /* voice */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 8;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[6]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[7]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[8]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[2], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* echo cancel feedback */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[9]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[10]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[3], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* ultrasound 3D */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 48;
      ChanReq.uSampleSize_bits = 12;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[0]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[1]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[2]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[3]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[0], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* vibra */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[4], hChannel[11]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[4], hChannel[12]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[4], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* invalid extra channels */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 96;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[15], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[16], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDataChannelControl(hSB, hChannel[6], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_ERROR);

      /* valid extra channels on second data line */
      DalSlimBus_ConnectPortToChannel(hSB, hChannel[15], SLIMBUS_PORT_SOURCE_FLOW, hPort[0]);
      DalSlimBus_ConnectPortToChannel(hSB, hChannel[16], SLIMBUS_PORT_SOURCE_FLOW, hPort[1]);
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 96;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[15], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[16], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* Remove echo cancel feedback */
      result = DalSlimBus_NextDataChannelControl(hSB, hGroup[3], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[3]);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* Remove voice */
      result = DalSlimBus_NextDataChannelControl(hSB, hGroup[2], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[2]);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* Remove acoustic touch screen */
      result = DalSlimBus_NextDataChannelControl(hSB, hGroup[5], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[5]);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* Remove ultrasound 3D */
      result = DalSlimBus_NextDataChannelControl(hSB, hGroup[0], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[0]);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* Remove vibra */
      result = DalSlimBus_NextDataChannelControl(hSB, hGroup[4], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[4]);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* Remove alert */
      result = DalSlimBus_NextDataChannelControl(hSB, hGroup[1], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[1]);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      /* Remove extra channels */
      result = DalSlimBus_NextDataChannelControl(hSB, hChannel[15], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDataChannelControl(hSB, hChannel[16], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 0);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }
//    else if (g_sb_test_case == 3)
    {
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 8;
      ChanReq.uSampleSize_bits = 20;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[0], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[1], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[2], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      for (i = 0; i < 3; i++)
      {
        result = DalSlimBus_NextDataChannelControl(hSB, hChannel[i], SLIMBUS_CHAN_CTRL_REMOVE);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }
//    else if (g_sb_test_case == 4)
    {
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[0], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 8000000);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_NextDataChannelControl(hSB, hChannel[0], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }
//    else if (g_sb_test_case == 5)
    {
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 6;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[0], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[1], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[2], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[3], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 24;
      ChanReq.uSampleSize_bits = 8;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[4], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 0);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      for (i = 0; i < 5; i++)
      {
        result = DalSlimBus_NextDataChannelControl(hSB, hChannel[i], SLIMBUS_CHAN_CTRL_REMOVE);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }
//    else if (g_sb_test_case == 6)
    {
      /* Audio UC #2 */

      for (i = 0; i < 5; i++ )
      {
        result = DalSlimBus_AllocResourceGroup(hSB, &hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }

      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 480000);
      SB_ASSERT(result == DAL_SUCCESS);

      /* ultrasound free form gestures */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 48;
      ChanReq.uSampleSize_bits = 12;
      ChanReq.eOptions = (SlimBusChannelOptionsType)(SLIMBUS_CHAN_O_AUTO_ACTIVATE|SLIMBUS_CHAN_O_PRESERVE_ORDERING);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[0]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[1]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[2]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[3]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[0], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* alert */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 24;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[4]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[5]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[1], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* voice */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 8;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[6]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[7]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[8]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[9]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[10]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[2], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* vibra */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[4], hChannel[11]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[4], hChannel[12]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[4], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* echo cancel feedback */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[14]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[15]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[3], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* FM radio RX (mono) */
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[13], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
      
      for (i = 0; i < 5; i++)
      {
        result = DalSlimBus_NextDataChannelControl(hSB, hGroup[i], SLIMBUS_CHAN_CTRL_REMOVE);
        SB_ASSERT(result == DAL_SUCCESS);
        result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_NextDataChannelControl(hSB, hChannel[13], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 0);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }
//    else if (g_sb_test_case == 7)
    {
      /* Audio UC #3 */

      for (i = 0; i < 4; i++ )
      {
        result = DalSlimBus_AllocResourceGroup(hSB, &hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }

      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 480000);
      SB_ASSERT(result == DAL_SUCCESS);

      /* 5.1 Audio */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 24;
      ChanReq.eOptions = (SlimBusChannelOptionsType)(SLIMBUS_CHAN_O_AUTO_ACTIVATE|SLIMBUS_CHAN_O_PRESERVE_ORDERING);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[0]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[1]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[2]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[3]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[4]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[5]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[0], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* voice */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 8;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[6]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[7]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[8]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[9]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[10]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[1], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* echo cancel feedback */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[11]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[12]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[2], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* acoustic touch screen */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[13]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[14]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[3], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      for (i = 0; i < 4; i++)
      {
        result = DalSlimBus_NextDataChannelControl(hSB, hGroup[i], SLIMBUS_CHAN_CTRL_REMOVE);
        SB_ASSERT(result == DAL_SUCCESS);
        result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 0);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }
//    else if (g_sb_test_case == 8)
    {
      /* Audio UC #4 */

      for (i = 0; i < 3; i++ )
      {
        result = DalSlimBus_AllocResourceGroup(hSB, &hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }

      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 480000);
      SB_ASSERT(result == DAL_SUCCESS);

      /* Gaming 5.1 Audio */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 24;
      ChanReq.uSampleSize_bits = 24;
      ChanReq.eOptions = (SlimBusChannelOptionsType)(SLIMBUS_CHAN_O_AUTO_ACTIVATE|SLIMBUS_CHAN_O_PRESERVE_ORDERING);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[0]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[1]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[2]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[3]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[4]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[5]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[0], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* acoustic touch screen */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[6]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[7]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[1], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* FM radio RX (stereo) */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[8]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[9]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[2], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      for (i = 0; i < 3; i++)
      {
        result = DalSlimBus_NextDataChannelControl(hSB, hGroup[i], SLIMBUS_CHAN_CTRL_REMOVE);
        SB_ASSERT(result == DAL_SUCCESS);
        result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 0);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }

    /* 6 US TX + 2 RX + 1 TX */
    {
      for (i = 0; i < 3; i++ )
      {
        result = DalSlimBus_AllocResourceGroup(hSB, &hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      
      /* Ultrasound */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_1_HZ;
      ChanReq.uRateMultiplier = 192000;
      ChanReq.uSampleSize_bits = 16;
      ChanReq.eOptions = (SlimBusChannelOptionsType)(SLIMBUS_CHAN_O_AUTO_ACTIVATE|SLIMBUS_CHAN_O_PRESERVE_ORDERING);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[0]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[1]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[2]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[3]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[4]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[5]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[0], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_1_HZ;
      ChanReq.uRateMultiplier = 48000;
      ChanReq.uSampleSize_bits = 16;
      /* Playback (stereo) */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[6]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[7]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[1], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
      /* Record (mono) */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[8]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[2], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      for (i = 0; i < 3; i++)
      {
        result = DalSlimBus_NextDataChannelControl(hSB, hGroup[i], SLIMBUS_CHAN_CTRL_REMOVE);
        SB_ASSERT(result == DAL_SUCCESS);
        result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }
//    else if (g_sb_test_case == 9)
    {
      /* Audio UC #5 */

      for (i = 0; i < 5; i++ )
      {
        result = DalSlimBus_AllocResourceGroup(hSB, &hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }

      /* ultrasound free form gestures */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 48;
      ChanReq.uSampleSize_bits = 12;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[0]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[1]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[2]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[3]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[0], &ChanReq);
      /* alert */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 24;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[4]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[5]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[1], &ChanReq);
      /* voice */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 8;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[6]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[7]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[8]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[9]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[10]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[2], &ChanReq);
      /* echo cancel feedback */
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[11]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[12]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[3], &ChanReq);
      /* vibra */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[4], hChannel[13]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[4], hChannel[14]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[4], &ChanReq);
      /* FM radio RX (mono) */
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[15], &ChanReq);
      /* control */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_1_HZ;
      ChanReq.eDesiredProtocol = SLIMBUS_PROTO_ASYNC_SIMPLEX;
      ChanReq.uRateMultiplier = 800;
      ChanReq.uSampleSize_bits = 8;
      //result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[16], &ChanReq);
      ChanReq.uRateMultiplier = 16000;
      ChanReq.uSampleSize_bits = 8;
      //result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[17], &ChanReq);
      //result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[18], &ChanReq);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      for (i = 0; i < 5; i++)
      {
        result = DalSlimBus_NextDataChannelControl(hSB, hGroup[i], SLIMBUS_CHAN_CTRL_REMOVE);
        SB_ASSERT(result == DAL_SUCCESS);
        result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_NextDataChannelControl(hSB, hChannel[15], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }
#if 0
//    else if (g_sb_test_case == 10)
    {
      /* Audio UC #B1 */

      for (i = 0; i < 7; i++ )
      {
        result = DalSlimBus_AllocResourceGroup(hSB, &hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }

      /* alert */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 24;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[0]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[1]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[1], &ChanReq);
      /* voice */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 8;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[2]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[3]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[4]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[5]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[6]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[2], &ChanReq);
      /* ultrasound 3D */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 48;
      ChanReq.uSampleSize_bits = 12;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[7]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[8]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[9]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[10]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[0], &ChanReq);
      /* echo cancel feedback */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[11]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[12]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[3], &ChanReq);
      /* vibra */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[4], hChannel[13]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[4], hChannel[14]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[4], &ChanReq);
      /* FM radio RX (stereo) */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[5], hChannel[15]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[5], hChannel[16]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[5], &ChanReq);
      /* BT stereo A2DP */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[6], hChannel[17]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[6], hChannel[18]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[6], &ChanReq);
      /* BT HCI */
      //ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_1_HZ;
      ChanReq.eDesiredProtocol = SLIMBUS_PROTO_ASYNC_SIMPLEX;
      ChanReq.uRateMultiplier = 24;
      ChanReq.uSampleSize_bits = 24;
      //result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[19], &ChanReq);
      /* MDM to APQ vocoder link */
      ChanReq.uRateMultiplier = 2;
      ChanReq.uSampleSize_bits = 16;
//      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[7], hChannel[20]);
//      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[7], hChannel[21]);
//      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[7], &ChanReq);

      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);

      for (i = 0; i < 7; i++)
      {
        result = DalSlimBus_NextDataChannelControl(hSB, hGroup[i], SLIMBUS_CHAN_CTRL_REMOVE);
        SB_ASSERT(result == DAL_SUCCESS);
        result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[i]);
        SB_ASSERT(result == DAL_SUCCESS);
      }
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
    }
    else if (g_sb_test_case == 11)
    {
      /* Audio UC #B2 */

      result = DalSlimBus_NextReserveMsgBandwidth(hSB, 480000);

      /* alert */
      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 24;
      result = DalSlimBus_NextDefineDataChannel(hSB, hChannel[0], &ChanReq);
      /* voice */
      ChanReq.uRateMultiplier = 8;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[1]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[0], hChannel[2]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[0], &ChanReq);
      /* ultrasound 3D */
      ChanReq.uRateMultiplier = 48;
      ChanReq.uSampleSize_bits = 12;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[3]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[4]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[5]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[6]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[1], &ChanReq);
      /* echo cancel feedback */
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 16;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[7]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[3], hChannel[8]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[3], &ChanReq);
      /* vibra */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[4], hChannel[9]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[4], hChannel[10]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[4], &ChanReq);
      /* FM radio RX (stereo) */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[5], hChannel[11]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[5], hChannel[12]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[5], &ChanReq);
      /* BT stereo A2DP */
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[6], hChannel[13]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[6], hChannel[14]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[6], &ChanReq);
      /* BT HCI */
      //ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_1_HZ;
      ChanReq.eDesiredProtocol = SLIMBUS_PROTO_ASYNC_SIMPLEX;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 32;
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[15]);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[2], hChannel[16]);
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[2], &ChanReq);
      /* MDM to APQ vocoder link */
      ChanReq.uRateMultiplier = 2;
      ChanReq.uSampleSize_bits = 16;
//      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[7], hChannel[20]);
//      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[7], hChannel[21]);
//      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[7], &ChanReq);
    }
#endif /* 0 */
    
#if 1
    /* Multi-channel data Tx test */
  
    {
      result = DalSlimBus_AllocProgressCounter(hSB, hMultiPort[2],
                                               SLIMBUS_BAM_DEFAULT, &hCounter[3]);
      SB_ASSERT(result == DAL_SUCCESS);
  
      result = DalSlimBus_InitProgressCounter(hSB, hCounter[3], 0);
      SB_ASSERT(result == DAL_SUCCESS);
  
      result = DalSlimBus_AllocResourceGroup(hSB, &hGroup[1]);
      SB_ASSERT(result == DAL_SUCCESS);
      
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[3]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[4]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_AddResourceToGroup(hSB, hGroup[1], hChannel[5]);
      SB_ASSERT(result == DAL_SUCCESS);
      
      CbData.hSB = hSB;
      CbData.uPhysOffset = MemInfo.VirtualAddr-MemInfo.PhysicalAddr;
      CbData.hBamEvent = hBamEvent;
      CbData.hPortEvent = hEvent;

      CbData.phPort = hMultiPort;
      CbData.uNumPorts = 3;
      CbData.hChannel = hGroup[1];
      CbData.hCounter = hCounter[3];
      CbData.uBamCount = 20;
      CbData.uNumWords = CbData.uBamCount*sizeof(ucBuf2)/sizeof(uint32);
      CbData.bConnected = TRUE;
      
      memset(&Pipe, 0, sizeof(Pipe));
      Pipe.DescFifo.uMinSize = 64;
      Pipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;

      Config.eOptions = SLIMBUS_PORT_O_PACKED;
      Config.uBlockSize = 0;
      Config.uTransSize = 0;
      Config.uWaterMark = 8;
          
      Reg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
      Reg.hEvent = hBamEvent;
      Reg.pUser = NULL;

      MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "expected BAM descriptor rate: 2ms");

      result = DalSlimBus_ConfigMasterPort(hSB, hMultiPort[0], &Config);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_ConfigMasterPort(hSB, hMultiPort[1], &Config);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_ConfigMasterPort(hSB, hMultiPort[2], &Config);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_RegisterPortEventEx(hSB, hMultiPort[0], hPortEvent, NULL, SLIMBUS_EVENT_O_FIFO_TRANSMIT_UNDERRUN);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_RegisterPortEventEx(hSB, hMultiPort[1], hPortEvent, NULL, SLIMBUS_EVENT_O_FIFO_TRANSMIT_UNDERRUN);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_RegisterPortEvent(hSB, hMultiPort[2], hPortEvent, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      Pipe.eOptions = 0;
      result = DalSlimBus_ConfigMasterPipe(hSB, hMultiPort[0], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_ConfigMasterPipe(hSB, hMultiPort[1], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
      SB_ASSERT(result == DAL_SUCCESS);
      Pipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);
      result = DalSlimBus_ConfigMasterPipe(hSB, hMultiPort[2], SLIMBUS_PORT_SOURCE_FLOW, &Pipe);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_RegisterBamEvent(hSB, hMultiPort[2], SLIMBUS_BAM_DEFAULT, &Reg);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[3], SLIMBUS_PORT_SOURCE_FLOW, hMultiPort[0]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[4], SLIMBUS_PORT_SOURCE_FLOW, hMultiPort[1]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[5], SLIMBUS_PORT_SOURCE_FLOW, hMultiPort[2]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_GetSlavePortHandle(hSB, genLA, 3, &hSlPort[3]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_GetSlavePortHandle(hSB, genLA, 4, &hSlPort[4]);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_GetSlavePortHandle(hSB, genLA, 5, &hSlPort[5]);
//      SB_ASSERT(result == DAL_SUCCESS);
//      result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[3], SLIMBUS_PORT_SINK_FLOW, hSlPort[3]);
//      SB_ASSERT(result == DAL_SUCCESS);
//      result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[4], SLIMBUS_PORT_SINK_FLOW, hSlPort[4]);
//      SB_ASSERT(result == DAL_SUCCESS);
//      result = DalSlimBus_ConnectPortToChannel(hSB, hChannel[5], SLIMBUS_PORT_SINK_FLOW, hSlPort[5]);
//      SB_ASSERT(result == DAL_SUCCESS);
      
      IOVec.uAddr = phBuf2;
      IOVec.uSize = sizeof(ucBuf2);
      IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(hSB, hMultiPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_SubmitBamTransfer(hSB, hMultiPort[0], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      IOVec.uAddr = phBuf3;
      IOVec.uSize = 96;
      IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(hSB, hMultiPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_SubmitBamTransfer(hSB, hMultiPort[1], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      IOVec.uAddr = phBuf4;
      IOVec.uSize = 96;
      IOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      result = DalSlimBus_SubmitBamTransfer(hSB, hMultiPort[2], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_SubmitBamTransfer(hSB, hMultiPort[2], SLIMBUS_BAM_DEFAULT, &IOVec, NULL);
      SB_ASSERT(result == DAL_SUCCESS);
              
      ChanReq.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
      ChanReq.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;
      ChanReq.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
      ChanReq.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;

      ChanReq.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
      ChanReq.uRateMultiplier = 12;
      ChanReq.uSampleSize_bits = 8;
      result = DalSlimBus_NextDefineDataChannel(hSB, hGroup[1], &ChanReq);
      SB_ASSERT(result == DAL_SUCCESS);
        
      for (i = 0; i < 3; i++)
      {
        uint32 j;
        
        bWatermark = FALSE;
        for (j = 0; !bWatermark && j < 1000; j++)
        {
          result = DalSlimBus_GetPortFifoStatus(hSB, hMultiPort[i], SLIMBUS_BAM_DEFAULT,
                                                &uFifoWord, &bWatermark);
        }
        SB_ASSERT(result == DAL_SUCCESS);
        //SB_ASSERT(uFifoWord > 0);
      }
  
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
  
      SlimBusTestProcessBamEvents(&CbData);
  
      result = DalSlimBus_NextDataChannelControl(hSB, hGroup[1], SLIMBUS_CHAN_CTRL_REMOVE);
      SB_ASSERT(result == DAL_SUCCESS);
      result = DalSlimBus_DoReconfigureNow(hSB);
      SB_ASSERT(result == DAL_SUCCESS);
  
      DALSYS_EventWait(hEvent);
                          
      result = DalSlimBus_DeallocResourceGroup(hSB, hGroup[1]);
      SB_ASSERT(result == DAL_SUCCESS);
    }
#endif

  
    /* Deallocation */  
    
    result = DalSlimBus_DeallocMasterPorts(hSB, hMultiPort, sizeof(hMultiPort));
    SB_ASSERT(result == DAL_SUCCESS);

    for (i=0; i < NUM_CHANS; i++)
    {
      result = DalSlimBus_DeallocDataChannel(hSB, hChannel[i]);
      SB_ASSERT(result == DAL_SUCCESS);
    }
    for (i=0; i < NUM_PORTS-NUM_MULTI_CHANS-NUM_LOOPBACK_PORTS; i++)
    {
      result = DalSlimBus_DeallocMasterPorts(hSB, &hPort[i], sizeof(hPort[0]));
      SB_ASSERT(result == DAL_SUCCESS);
    }

    result = DalSlimBus_DeallocProgressCounter(hSB, hCounter[0]);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DeallocProgressCounter(hSB, hCounter[1]);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DeallocProgressCounter(hSB, hCounter[2]);
    SB_ASSERT(result == DAL_SUCCESS);
    result = DalSlimBus_DeallocProgressCounter(hSB, hCounter[3]);
    SB_ASSERT(result == DAL_SUCCESS);

    for (i=0; i < 100; i++)
    {
      SlimBusResourceHandle hPort;

      result = DalSlimBus_AllocMasterPorts(hSB, SLIMBUS_PORT_REQ_O_HALF_DUPLEX,
                                           4, &hPort, sizeof(hPort));
      SB_ASSERT(result == DAL_SUCCESS);
      SB_ASSERT(NULL != hPort);
      SB_ASSERT(0xffffffff != hPort);

      result = DalSlimBus_DeallocMasterPorts(hSB, &hPort, sizeof(hPort));
      SB_ASSERT(result == DAL_SUCCESS);
    }
  }

  datain = 0x67;
  datalen = 0;
  result = DalSlimBus_ExchangeValueElement(hSB, intLA, 0x810, (uint8*)&datain, 1, (uint8*)&datain, 1, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0x35);
  SB_ASSERT(datalen == 1);

  result = DalSlimBus_ExchangeValueElement(hSB, intLA, 0x810, (uint8*)&datain, 1, (uint8*)&datain, 1, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0x67);
  SB_ASSERT(datalen == 1);

  result = DalSlimBus_ReadValueElement(hSB, intLA, 0x810, (uint8*)&datain, 1, &datalen, NULL);
  SB_ASSERT(result == DAL_SUCCESS);
  SB_ASSERT(datain == 0x35);
  SB_ASSERT(datalen == 1);

  SB_ASSERT(uClkCount > 0);
  
  slimbus_test_done = TRUE;
  MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "Slimbus test done (#err: %d)", slimbus_test_err_cnt);

  DALSYS_DestroyObject(hEvent);
  DALSYS_DestroyObject(hBamEvent);
  DALSYS_DestroyObject(hPortEvent);
  DALSYS_DestroyObject(hSimplePortEvent);
  
  DalDevice_Close(hSB);  
  DAL_DeviceDetach(hSB);
}


#ifdef IMAGE_QDSP6_PROC
#define SLIMBUS_TEST_SERVICE_STACK_SIZE  (4096)
static qurt_anysignal_t     slimbus_test_service_sig;
static qurt_thread_t        slimbus_test_service_tcb;
static qurt_thread_attr_t   slimbus_test_service_attr;

static void slimbus_test_service_thread(void *unused)
{
  qurt_timer_sleep(30000000);
  SLIMbus_Test();    
}

void slimbus_test_init(void)
{
  static char slimbus_test_service_stack[SLIMBUS_TEST_SERVICE_STACK_SIZE];

  qurt_anysignal_init(&slimbus_test_service_sig);
  qurt_thread_attr_init(&slimbus_test_service_attr);
  qurt_thread_attr_set_name(&slimbus_test_service_attr, "SlimBusTest");
  qurt_thread_attr_set_priority(&slimbus_test_service_attr, 10);
  qurt_thread_attr_set_stack_size(&slimbus_test_service_attr,
                                  SLIMBUS_TEST_SERVICE_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&slimbus_test_service_attr, slimbus_test_service_stack);
  qurt_thread_create(&slimbus_test_service_tcb, &slimbus_test_service_attr, 
                     slimbus_test_service_thread, NULL);
}
#endif

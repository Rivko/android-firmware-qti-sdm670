/*
===========================================================================

FILE:         SlimBusLogger.c

DESCRIPTION:  This file implements the SLIMbus hardware logger functionality.

===========================================================================

                             Edit History
when       who     what, where, why
--------   ---     --------------------------------------------------------
10/07/15   NSD     Initial revision. 
===========================================================================
             Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/
#include "SlimBus.h"
#include "SlimBusMaster.h"
#include "SlimBusDal.h"
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

#define PHYSICAL_MEMORY_SIZE 16384
#define LOG_BUFFER_SIZE      8192

SlimBusBamIOVecType GlobalLogIOVec;
SlimBusBamIOVecType GlobalLogIOVec2;
uint8* val = NULL;


typedef struct
{
  DalDeviceHandle *hSB;
  SlimBusResourceHandle hLoggerPort;
  DALSYSEventHandle hBamEvent;
} CallbackDataType;


void* LogPortCb(void *pBuf1, uint32 uSize1, void *pBuf2, uint32 uSize2)
{
  CallbackDataType *pCbData = (CallbackDataType*)pBuf1;
  DALResult result;

  result = SlimBus_GetBamIOVec((SlimBusHandle)pCbData->hSB, pCbData->hLoggerPort, SLIMBUS_BAM_DEFAULT, &GlobalLogIOVec);

  if (result != DAL_SUCCESS)
    return NULL;

  GlobalLogIOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
  result = SlimBus_SubmitBamTransfer((SlimBusHandle)pCbData->hSB, pCbData->hLoggerPort, SLIMBUS_BAM_DEFAULT, &GlobalLogIOVec, NULL);
  
  return NULL;
}

void* LogPortCb2(void *pBuf1, uint32 uSize1, void *pBuf2, uint32 uSize2)
{
  CallbackDataType *pCbData = (CallbackDataType*)pBuf1;

  DALResult result;

  result = SlimBus_GetBamIOVec((SlimBusHandle)pCbData->hSB, pCbData->hLoggerPort, SLIMBUS_BAM_DEFAULT, &GlobalLogIOVec2);

  if (result != DAL_SUCCESS)
    return NULL;

  GlobalLogIOVec2.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
  result = SlimBus_SubmitBamTransfer((SlimBusHandle)pCbData->hSB, pCbData->hLoggerPort, SLIMBUS_BAM_DEFAULT, &GlobalLogIOVec2, NULL);
  

  return NULL;
}

void SLIMbus_Init_Logger()
{
  
  MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "Starting the logger");
  
  
  DalDeviceHandle *hSB;
  DALResult result;
  uint32 x = 0;
  
  result = DAL_SlimBusDeviceAttach( DALDEVICEID_SLIMBUS_1, &hSB );
  
  result = DalDevice_Open( hSB, DAL_OPEN_SHARED );
  
  /* 
   * Primary logger resources
   *  */
  DALSYSEventHandle hBamLogEvent;
  
  SlimBusResourceHandle hLogPort;
  SlimBusPortConfigType LogConfig;
  SlimBusPipeConfigType LogPipe;
  SlimBusBamIOVecType LogIOVec;
  SlimBusBamRegisterEventType LogReg;
  DALSYSMemHandle hLogPhysMem;
  DALSYSMemInfo LogMemInfo;
  
  CallbackDataType CbLogData;

  uint8 *pucLogBuf;
  uint32 phLogBuf;
  
  result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                                DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                PHYSICAL_MEMORY_SIZE, &hLogPhysMem, NULL);
                                

  
  result = DALSYS_MemInfo(hLogPhysMem, &LogMemInfo);
  
  pucLogBuf = (uint8*) LogMemInfo.VirtualAddr;
  phLogBuf = LogMemInfo.PhysicalAddr;
  memset((void*)LogMemInfo.VirtualAddr, 0, PHYSICAL_MEMORY_SIZE);  


  /* 
   * Secondary logger resources 
   * */
  SlimBusResourceHandle hLogChannel;
  SlimBusResourceHandle hLogPort2;
  SlimBusPortConfigType LogConfig2;
  SlimBusPipeConfigType LogPipe2;
  SlimBusBamRegisterEventType LogReg2;
  SlimBusChannelReqType ChanReq2;
  SlimBusBamIOVecType LogIOVec2;
  CallbackDataType CbLogData2;
  SlimBusChannelType *pLogChannel;

  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT((SlimBusHandle)hSB);
  
  DALSYSMemHandle hLogPhysMem2;
  DALSYSMemInfo LogMemInfo2;
  
  DALSYSEventHandle hBamLogEvent2;
  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &hBamLogEvent2, NULL);

  


  uint8 *pucLogBuf2;
  uint32 phLogBuf2;


  result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                                DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                PHYSICAL_MEMORY_SIZE, &hLogPhysMem2, NULL);
                                

  
  result = DALSYS_MemInfo(hLogPhysMem2, &LogMemInfo2);
  
  pucLogBuf2 = (uint8*) LogMemInfo2.VirtualAddr;
  phLogBuf2 = LogMemInfo2.PhysicalAddr;
  memset((void*)LogMemInfo2.VirtualAddr, 0, PHYSICAL_MEMORY_SIZE);  

  result = DALSYS_SetupCallbackEvent(hBamLogEvent2, LogPortCb2, (void*)&CbLogData2);
  

  /* 
   * Primary logger setup 
   * */
  
  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &hBamLogEvent, NULL);
  
  result = DALSYS_SetupCallbackEvent(hBamLogEvent, LogPortCb, (void*)&CbLogData);
  
  result = SlimBus_AllocMasterPorts((SlimBusHandle)hSB, SLIMBUS_PORT_REQ_O_DEFAULT, 4, &hLogPort, sizeof(hLogPort));

  CbLogData.hSB = hSB;
  CbLogData.hLoggerPort = hLogPort;
  CbLogData.hBamEvent = hBamLogEvent;
  
  memset((void *)&LogPipe, 0, sizeof(LogPipe));
  LogPipe.DescFifo.uMinSize = 64;
  LogPipe.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
  LogPipe.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);
    
  LogReg.eOptions = SLIMBUS_BAM_O_DESC_DONE;
  LogReg.hEvent = hBamLogEvent;
  LogReg.pUser = NULL;  
  
  result = SlimBus_ConfigMasterPipe((SlimBusHandle)hSB, hLogPort, SLIMBUS_PORT_SINK_FLOW, &LogPipe);
  
  result = SlimBus_RegisterBamEvent((SlimBusHandle)hSB, hLogPort, SLIMBUS_BAM_DEFAULT, &LogReg);
  

  for(x = 0; x < 2; x++)
  {  
    LogIOVec.uAddr = phLogBuf + (LOG_BUFFER_SIZE * x);
    LogIOVec.uSize = LOG_BUFFER_SIZE;
    LogIOVec.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = SlimBus_SubmitBamTransfer((SlimBusHandle)hSB, hLogPort, SLIMBUS_BAM_DEFAULT, &LogIOVec, NULL);
    
  }
  SlimBus_StartLogger((SlimBusHandle)hSB, hLogPort);
  
  LogConfig.eOptions = SLIMBUS_PORT_O_PACKED;
  LogConfig.uBlockSize = 0;
  LogConfig.uTransSize = 0;
  LogConfig.uWaterMark = 4;
  LogConfig.bUpperWaterMarkValid = FALSE;

  result = SlimBus_ConfigMasterPort((SlimBusHandle)hSB, hLogPort, &LogConfig);
   
  /* 
   * Secondary logger setup
   * */
   
  result = SlimBus_AllocDataChannel((SlimBusHandle)hSB, &hLogChannel);
  
  
  result = SlimBus_AllocMasterPorts((SlimBusHandle)hSB, SLIMBUS_PORT_REQ_O_DEFAULT, 4, &hLogPort2, sizeof(hLogPort2));
  
  pLogChannel = GetChannelHandle(pClientCtxt, hLogChannel, NULL);
  
  SlimBus_SetDataLineFlag((SlimBusHandle)hSB, pLogChannel->channelNum);  // So that secondary data line will be assigned to the channel      
    
  CbLogData2.hSB = hSB;
  CbLogData2.hLoggerPort = hLogPort2;
  CbLogData2.hBamEvent = hBamLogEvent2;
  
  LogConfig2.eOptions = SLIMBUS_PORT_O_PACKED;
  LogConfig2.uBlockSize = 0;
  LogConfig2.uTransSize = 0;
  LogConfig2.uWaterMark = 4;
  LogConfig2.bUpperWaterMarkValid = FALSE;
    
  memset((void *)&LogPipe2, 0, sizeof(LogPipe2));
  LogPipe2.DescFifo.uMinSize = 64;
  LogPipe2.eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
  LogPipe2.eOptions = (SlimBusBamOptionType)(SLIMBUS_BAM_O_DESC_DONE);
    
  LogReg2.eOptions = SLIMBUS_BAM_O_DESC_DONE;
  LogReg2.hEvent = hBamLogEvent2;
  LogReg2.pUser = NULL;
  
  result = SlimBus_ConfigMasterPort((SlimBusHandle)hSB, hLogPort2, &LogConfig2);
  
  result = SlimBus_ConfigMasterPipe((SlimBusHandle)hSB, hLogPort2, SLIMBUS_PORT_SINK_FLOW, &LogPipe2);
  
  result = SlimBus_RegisterBamEvent((SlimBusHandle)hSB, hLogPort2, SLIMBUS_BAM_DEFAULT, &LogReg2);
  
  result = SlimBus_ConnectPortToChannel((SlimBusHandle)hSB, hLogChannel, SLIMBUS_PORT_SINK_FLOW, hLogPort2);
     
  
  for(x = 0; x < 2; x++)
  {  
    LogIOVec2.uAddr = phLogBuf2 + (LOG_BUFFER_SIZE * x);
    LogIOVec2.uSize = LOG_BUFFER_SIZE;
    LogIOVec2.uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
    result = SlimBus_SubmitBamTransfer((SlimBusHandle)hSB, hLogPort2, SLIMBUS_BAM_DEFAULT, &LogIOVec2, NULL);
    
  }
  
  ChanReq2.eBaseSampleRate = SLIMBUS_RATE_FAM_4_KHZ;
  ChanReq2.eDataFormat = SLIMBUS_DATA_FORMAT_NOT_INDICATED;// LPCM_AUDIO;
  ChanReq2.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;
  ChanReq2.eOptions = SLIMBUS_CHAN_O_AUTO_ACTIVATE;
  ChanReq2.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;
  ChanReq2.uRateMultiplier = 6;
  ChanReq2.uSampleSize_bits = 8;  
    
  SlimBus_StartLogger((SlimBusHandle)hSB, hLogPort2);

  result = SlimBus_NextDefineDataChannel((SlimBusHandle)hSB, hLogChannel, &ChanReq2);
  
  result = SlimBus_DoReconfigureNow((SlimBusHandle)hSB);
}


#ifdef IMAGE_QDSP6_PROC
#define SLIMBUS_LOGGER_SERVICE_STACK_SIZE  (40960)
static qurt_anysignal_t     slimbus_logger_service_sig;
static qurt_thread_t        slimbus_logger_service_tcb;
static qurt_thread_attr_t   slimbus_logger_service_attr;

static void slimbus_logger_service_thread(void *unused)
{
  qurt_timer_sleep(30000000);
  SLIMbus_Init_Logger();
}

void slimbus_logger_init(void)
{
  static char slimbus_logger_service_stack[SLIMBUS_LOGGER_SERVICE_STACK_SIZE];

  qurt_anysignal_init(&slimbus_logger_service_sig);
  qurt_thread_attr_init(&slimbus_logger_service_attr);
  qurt_thread_attr_set_name(&slimbus_logger_service_attr, "SlimBuslogger");
  qurt_thread_attr_set_priority(&slimbus_logger_service_attr, 10);
  qurt_thread_attr_set_stack_size(&slimbus_logger_service_attr,
                                  SLIMBUS_LOGGER_SERVICE_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&slimbus_logger_service_attr, slimbus_logger_service_stack);
  qurt_thread_create(&slimbus_logger_service_tcb, &slimbus_logger_service_attr, 
                     slimbus_logger_service_thread, NULL);
}
#endif

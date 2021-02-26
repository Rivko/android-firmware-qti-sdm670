#ifndef __DDIDMOV_H__
#define __DDIDMOV_H__

/*
===============================================================================

 FILE:         DDIDmov.h

 DESCRIPTION:  
   DMOV Device Interface

===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/27/09   MK      Added clock management 
01/18/08   ah      Created

===============================================================================
                   Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===============================================================================
*/

#include "DalDevice.h"
#include "DALStdErr.h"

#define DALDMOV_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

typedef uint32 DMOV_CMD_OPTIONS;

/* DMOV channel operation mode */
#define DMOV_CHAN_MODE_INTERRUPT            0
#define DMOV_CHAN_MODE_POLLING              1

/* DMOV command address mode */
#define DMOV_CMD_ADDR_MODE_SINGLE           0
#define DMOV_CMD_ADDR_MODE_SCATTER_GATHER   1
#define DMOV_CMD_ADDR_MODE_BOX              3

/* DMOV command endianness control */
#define DMOV_CMD_END_CTL_SWAP_BYTES         1
#define DMOV_CMD_END_CTL_SWAP_HALF_WORDS    2
#define DMOV_CMD_END_CTL_SWAP_WORDS         4

#define DMOV_MAX_NUM_FLUSH_STATE_REGS       6

/* Command specification - Data Mover command type */
typedef enum DmovCommandType DmovCommandType;
enum DmovCommandType
{
  DAL_DMOV_CMD_LIST = 0,
  DAL_DMOV_CMD_PTR_LIST = 1,
  DAL_DMOV_MPU_IN = 2,
  DAL_DMOV_MPU_OUT = 3,

  DAL_DMOV_CMD_SPEC_FORCE_32_BIT = 0x7FFFFFFF
};

/* Data Mover command options */
typedef struct DmovCommandOptions  DmovCommandOptions;
struct DmovCommandOptions
{
  uint32 dwAddrMode;
  uint32 dwSrcCRCI;
  uint32 dwDstCRCI;
  uint32 dwSrcEndianness;
  uint32 dwDstEndianness;
  uint32 bBlockThisChannel;
  uint32 bBlockOtherChannel;
  uint32 bUnblockOtherChannel;
  uint32 bLastCommand;
};

/* Data Mover single command 
 *
 * For the length, bits 0-15 specify the length of the transfer.
 * Therefore the max transfer size is 65535 bytes.
 */
typedef struct DmovSingleCommand  DmovSingleCommand;
struct DmovSingleCommand
{
  DMOV_CMD_OPTIONS CmdOptions;
  uint32           dwSrcAddr;
  uint32           dwDstAddr;
  uint32           dwLength;
};
#define  DMOV_NUM_SINGLE_CMD_WORDS  4

/* Data Mover scatter/gather command 
 *
 * Each descriptor list entry is represented by 2 uint32, one for the 
 * address of the segment and one for its length. Max length of 65535 
 * bytes to transfer per descriptor.
 */
typedef struct DmovSGCommand  DmovSGCommand;
struct DmovSGCommand
{
  DMOV_CMD_OPTIONS  CmdOptions;
  uint32  dwSrcDescListAddr;
  uint32  dwDstDescListAddr;
  uint32  dwReserved; 
};
#define DMOV_NUM_SG_CMD_WORDS  4

/* Data Mover box mode command */
typedef struct DmovBoxCommand  DmovBoxCommand;
struct DmovBoxCommand
{
  DMOV_CMD_OPTIONS  CmdOptions;
  uint32  dwFristSrcRowAddr;
  uint32  dwFristDstRowAddr;
  uint32  dwRowLength; /* bits 0-15 for dst, 16-31 for src */
  uint32  dwNumRows;   /* bits 0-15 for dst, 16-31 for src */
  uint32  dwRowOffset; /* bits 0-15 for dst, 16-31 for src */
};
#define DMOV_NUM_BOX_CMD_WORDS  6

/* Data Mover transfer type 
 *
 * DMOV_TRANSFER_LEGACY:
 *   Legacy transfer issued from the dmov legacy shim layer
 * DMOV_TRANSFER_SYNCHRONOUS:
 *   User pre-allocated command lists, should be kept until the transfer
 *   is done. can be of type DAL_DMOV_CMD_LIST or DAL_DMOV_CMD_PTR_LIST.
 * DMOV_TRANSFER_ASYNCHRONOUS:
 *   User can re-use the memory that holds the command lists immediately 
 *   upon return. Only for commands of type DAL_DMOV_CMD_LIST.
 */
typedef enum DmovTransferType  DmovTransferType;
enum DmovTransferType
{
  DMOV_TRANSFER_LEGACY,
  DMOV_TRANSFER_SYNCHRONOUS,
  DMOV_TRANSFER_ASYNCHRONOUS,

  _QIDL_PLACEHOLDER_DmovTransferType = 0x7fffffff
};

/* Data Mover transfer request
 *
 * Options for the whole transfer
 */
typedef struct DmovTransferRequest  DmovTransferRequest;
struct DmovTransferRequest
{
  uint32             dwDevice;
  uint32             dwChan;
  uint32             dwPriority;
  uint32             pUserData;
  DALSYSEventHandle  hEvent;
  uint32             dwNumCmds;
  DmovTransferType   TransferType;  
  DmovCommandType    CommandType;       
  void             * cmd_ptr;           /* legacy */
  uint32             pCallbackFunction; /* legacy */
  uint32             options;           /* legacy */
};

/* 
 *
 * ADM clock performance levels
 *
 *  DMOV_PERFLVL_LOW  :-   AXI / 4 and 9.6 MHz during ARM halt
 *  DMOV_PERFLVL_MID  :-   AXI / 4
 *  DMOV_PERFLVL_HIGH :-   AXI / 2
 *  DMOV_PERFLVL_MAX  :-   AXI / 1
 */

typedef enum DmovPerfLvlType DmovPerfLvlType;
enum DmovPerfLvlType
{
  DMOV_PERFLVL_MAX  = 0x0,
  DMOV_PERFLVL_HIGH = 0x1,
  DMOV_PERFLVL_MID  = 0x2,
  DMOV_PERFLVL_LOW  = 0x3,
  DMOV_NUM_PERFLVLS  
};


/* Data Mover transfer result information
 * Part of the result returned to the user of DMOV
 *
 * DMOV_TRANSFER_WAITING_TO_BE_ISSUED:
 *    Transfer queued in dmov and waiting to be issued
 * DMOV_TRANSFER_IN_PROGRESS:
 *    Transfer issued and waiting for its completion
 * DMOV_TRANSFER_SUCCESSFULL:
 *    Transfer successful
 * DMOV_TRANSFER_ERROR:
 *    Transfer resulted in an error
 * DMOV_TRANSFER_FLUSH:
 *    Transfer caused a flush
 * DMOV_TRANSFER_NON_EXISTENT:
 *    DMOV is not aware of this transfer
 */
typedef enum DmovResultState  DmovResultState;
enum DmovResultState
{
  DMOV_TRANSFER_WAITING_TO_BE_ISSUED,
  DMOV_TRANSFER_IN_PROGRESS,
  DMOV_TRANSFER_SUCCESSFULL,
  DMOV_TRANSFER_ERROR,
  DMOV_TRANSFER_FLUSH,
  DMOV_TRANSFER_NON_EXISTENT,

  _QIDL_PLACEHOLDER_DmovResultState = 0x7fffffff
};

/* Data Mover channel error */
typedef enum Dmov_ChanError Dmov_ChanError;
enum Dmov_ChanError
{
  DMOV_CHAN_NO_ERROR = 0,       
  DMOV_CHAN_INVALID_STATE = 1,
  DMOV_CHAN_CPH_BUS_ERROR = 2,
  DMOV_CHAN_DPH_BUS_ERROR = 3,
  DMOV_CHAN_MPU_ERROR = 4,
  DMOV_CHAN_LEN_ERROR = 5,
  DMOV_CHAN_CMD_ERROR = 6,

  DMOV_CHAN_ERROR_FORCE_32_BIT = 0x7FFFFFFF
};

/* Data Mover transfer result returned to the user of DMOV */
typedef struct DmovResult  DmovResult;
struct DmovResult
{
  DmovResultState  eState;
  uint32           dwResultValue;
  DALBOOL          bMoreResultsAvailable;
  uint32           FlushState[6];
  Dmov_ChanError   eChanError;
  uint32           pUserData;
};

/* State of the Data Mover transfer request issued by the user */
typedef enum DmovRequestState  DmovRequestState;
enum DmovRequestState
{
  DMOV_REQUEST_INITIAL,
  DMOV_REQUEST_OK,
  DMOV_REQUEST_CMD_LIST_NOT_ALIGNED,
  DMOV_REQUEST_BOOT_SHOULD_BE_SYNCHRONOUS,
  DMOV_REQUEST_INVALID_DEVICE,
  DMOV_REQUEST_INVALID_INTERRUPT_MODE,
  DMOV_REQUEST_INVALID_NUM_CMDS,
  DMOV_REQUEST_INVALID_DEVICE_STATE,
  DMOV_REQUEST_INVALID_CHAN_NUMBER,
  DMOV_REQUEST_INVALID_CHAN_STATE,
  DMOV_MAX_REQUESTS_ISSUED,

  _QIDL_PLACEHOLDER_DmovRequestState = 0x7fffffff
};

#define DMOV_INVALID_TOKEN 0xffffffff

/* Data Mover transfer request issued by the user */
typedef struct DmovReqStatus  DmovReqStatus;
struct DmovReqStatus
{
  DmovRequestState eState;
  uint32 dwToken;
};

/* Non-remotable API's */
DALResult 
Transfer (DalDeviceHandle           * _h, 
          const DmovTransferRequest * pTransferRequest, 
          uint32                    * pCommandList, 
          DmovReqStatus             * pRequestStatus);

DALResult 
GetTransferResult(DalDeviceHandle * _h, 
                  uint32            dwToken, 
                  DmovResult      * pResult); 

DALResult 
SetChanOperationMode (DalDeviceHandle * _h, 
                      uint32            dwChan, 
                      uint32            dwChanOpMode); 

DALResult
PrepareSGDescList(DalDeviceHandle * _h, 
                  uint32            dwNumDescriptors,
                  uint32          * pDescList);

DALResult
PrepareCommand(DalDeviceHandle    * _h, 
               DmovCommandOptions * pCommandOptions, 
               uint32             * pCommand);

DALResult
PrepareCommandListPtr(DalDeviceHandle * _h,
                      uint32          * pCommandList,
                      uint32            bLastPtr,
                      uint32          * pCommandListPtr);

/* Remotable API's */
DALResult 
FlushChannel (DalDeviceHandle * _h, 
              uint32            dwChan, 
              uint32            bFlushQueues,
              uint32            bDiscardCurrentCmd);

DALResult
FlushDomain (DalDeviceHandle * _h, 
             uint32            dwDomain);

/* DMOV function table */
typedef struct DalDmov DalDmov;
struct DalDmov
{
   DalDevice DalDevice;
   DALResult (*Transfer)( DalDeviceHandle * h,  const DmovTransferRequest*  pTransferRequest, uint32*  pCommandList, DmovReqStatus*  pRequestStatus);
   DALResult (*GetTransferResult)( DalDeviceHandle * h, uint32  dwToken,  DmovResult*  pResult);
   DALResult (*SetChanOperationMode)(DalDeviceHandle * _h, uint32  dwChan, uint32  dwChanOpMode);
   DALResult (*FlushChannel)(DalDeviceHandle * _h, uint32  dwChan, uint32  bFlushQueues, uint32  bDiscardCurrentCmd);
   DALResult (*FlushDomain)(DalDeviceHandle * _h, uint32  dwDomain);
   DALResult (*PrepareSGDescList)(DalDeviceHandle * _h, uint32  dwNumDescriptors, uint32*  pDescList);
   DALResult (*PrepareCommand)(DalDeviceHandle * _h, DmovCommandOptions* pCommandOptions, uint32*  pCommand);
   DALResult (*PrepareCommandListPtr)(DalDeviceHandle * _h, uint32*  pCommandList, uint32  bLastPtr, uint32*  pCommandListPtr);
};

typedef struct DalDmovHandle DalDmovHandle; 
struct DalDmovHandle 
{
   uint32 dwDalHandleId;
   const DalDmov * pVtbl;
   void * pClientCtxt;
};

/* 
 * Temporary support for Debug Info - till all clients start using
 * DAL_DmovDeviceAttachEx
 */

#define _QUOTES(x) #x
#define QUOTES(x) _QUOTES(x)

#define DAL_DmovDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(__FILE__ ":" QUOTES(__LINE__),DevId,DALDMOV_INTERFACE_VERSION,hDalDevice)

#define DAL_DmovDeviceAttachEx(pszArg,DevId,hDalDevice)\
        DAL_DeviceAttachEx(pszArg,DevId,DALDMOV_INTERFACE_VERSION,hDalDevice)


/* Non-remotable API's */
static __inline DALResult
DalDmov_Transfer(DalDeviceHandle * _h, const DmovTransferRequest* pTransferRequest, uint32* pCommandList, DmovReqStatus* pRequestStatus)
{
   if(DALISREMOTEHANDLE(_h))
   {
     /* No remoting for this API */
      return DAL_ERROR;
   }
   return ((DalDmovHandle *)_h)->pVtbl->Transfer( _h, pTransferRequest, pCommandList, pRequestStatus);
}

static __inline DALResult
DalDmov_PrepareSGDescList(DalDeviceHandle * _h, uint32  dwNumDescriptors, uint32*  pDescList)
{
   if(DALISREMOTEHANDLE(_h))
   {
     /* No remoting for this API */
     return DAL_ERROR;
   }
   return ((DalDmovHandle *)_h)->pVtbl->PrepareSGDescList( _h, dwNumDescriptors, pDescList);
}

static __inline DALResult
DalDmov_PrepareCommand(DalDeviceHandle * _h, DmovCommandOptions * pCommandOptions, uint32*  pCommand)
{
   if(DALISREMOTEHANDLE(_h))
   {
     /* No remoting for this API */
     return DAL_ERROR;
   }
   return ((DalDmovHandle *)_h)->pVtbl->PrepareCommand( _h, pCommandOptions, pCommand);
}

static __inline DALResult
DalDmov_PrepareCommandListPtr(DalDeviceHandle * _h, uint32 *  pCommandList, uint32  bLastPtr, uint32 *  pCommandListPtr)
{
   if(DALISREMOTEHANDLE(_h))
   {
     /* No remoting for this API */
     return DAL_ERROR;
   }
   return ((DalDmovHandle *)_h)->pVtbl->PrepareCommandListPtr( _h, pCommandList, bLastPtr, pCommandListPtr);
}

static __inline DALResult
DalDmov_GetTransferResult(DalDeviceHandle * _h, uint32  dwToken,  DmovResult*  pResult)
{
   if(DALISREMOTEHANDLE(_h))
   {
     /* No remoting for this API */
     return DAL_ERROR;
   }
   return ((DalDmovHandle *)_h)->pVtbl->GetTransferResult( _h, dwToken, pResult);
}

static __inline DALResult
DalDmov_SetChanOperationMode(DalDeviceHandle * _h, uint32  dwChan, uint32  dwChanOpMode)
{
   if(DALISREMOTEHANDLE(_h))
   {
     /* No remoting for this API */
     return DAL_ERROR;
   }
   return ((DalDmovHandle *)_h)->pVtbl->SetChanOperationMode( _h, dwChan, dwChanOpMode);
}

/* Remotable API's */
static __inline DALResult
DalDmov_FlushChannel(DalDeviceHandle * _h, uint32  dwChan, uint32  bFlushQueues, uint32  bDiscardCurrentCmd)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_3(DALVTBLIDX(((DalDmovHandle *)_h)->pVtbl, FlushChannel ), _h, dwChan, bFlushQueues, bDiscardCurrentCmd);
   }
   return ((DalDmovHandle *)_h)->pVtbl->FlushChannel( _h, dwChan, bFlushQueues, bDiscardCurrentCmd);
}

static __inline DALResult
DalDmov_FlushDomain(DalDeviceHandle * _h, uint32  dwDomain)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((DalDmovHandle *)_h)->pVtbl, FlushDomain ), _h, dwDomain);
   }
   return ((DalDmovHandle *)_h)->pVtbl->FlushDomain( _h, dwDomain);
}

#endif /* __DDIDMOV_H__ */

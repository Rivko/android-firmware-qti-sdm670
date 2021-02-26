/*==============================================================================
  Copyright (c) 2008,2010 Qualcomm Technologies Incorporated.
           All Rights Reserved.
  Qualcomm Confidential and Proprietary

==============================================================================*/

/*======================================================
FILE: DALFramework.h

SERVICES:
  DAL Framework API

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:
  None.

=====================================================*/

#ifndef DALFRAMEWORK_H
#define DALFRAMEWORK_H
#include "DALStdDef.h"
#include "DALSys.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

typedef struct DALDrvCtxt DALDrvCtxt;
typedef struct DALDevCtxt DALDevCtxt;
typedef struct DALClientCtxt DALClientCtxt;
typedef struct DALDrvVtbl DALDrvVtbl;

struct DALDrvVtbl
{
   int (*DAL_DriverInit)(DALDrvCtxt *);
   int (*DAL_DriverDeInit)(DALDrvCtxt *);
};

struct DALDevCtxt
{
    uint32   dwRefs;
    DALDEVICEID DevId;
    uint32      dwDevCtxtRefIdx;
    DALDrvCtxt  *pDALDrvCtxt;
    DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    DalDeviceHandle *hDALSystem;
    DalDeviceHandle *hDALTimer;
    DalDeviceHandle *hDALInterrupt;
    void ** pSystemTbl;
    DALSYSWorkLoopHandle * pDefaultWorkLoop;
    const char *strDeviceName;
    uint32 reserve[16-6];
};

struct DALDrvCtxt
{
   DALDrvVtbl DALDrvVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   DALDevCtxt DALDevCtxt[1];
};

struct DALClientCtxt
{
    uint32  dwRefs;
    uint32  dwAccessMode;
    void *  pPortCtxt;
    DALDevCtxt *pDALDevCtxt;
};

typedef struct DALInheritSrcPram DALInheritSrcPram;
struct DALInheritSrcPram
{
   const byte * pBuf;
   int dwBufLen;
   uint32 dwSeqIdx;
};

typedef struct DALInheritDestPram DALInheritDestPram;
struct DALInheritDestPram
{
   byte * pBuf;
   uint32 dwBufLen;
   int *pdwObjLen;
   uint32 *dwSeqIdx;
};

typedef struct DALInterface DALInterface;
struct DALInterface
{
    uint32 dwDalHandleId;
    void *pVtbl;
    DALClientCtxt *pclientCtxt;
};
/*------------------------------------------------------------------------------
Place the System Workloops within this table.
Timer driver acts as this environment's System Driver
and creates all System Workloops
the following indicates the max number of the shared system workloops
and other system wide pointers.

------------------------------------------------------------------------------*/
#define DAL_MAX_NUM_SYSTEM_WORKLOOPS   5
#define DAL_SYSTEM_TABLE_SIZE          (DAL_MAX_NUM_SYSTEM_WORKLOOPS+6)

//Indexs into the System Table
#define DAL_SYSTEM_TABLE_NUM_SYSTEM_WORKLOOPS   0
#define DAL_SYSTEM_TABLE_SYSTEM_WORKLOOPS       1

//Workloop priority levels
#define SYSTEM_WORKLOOP_PRIORITY_LOW            1
#define SYSTEM_WORKLOOP_PRIORITY_MEDIUM         3
#define SYSTEM_WORKLOOP_PRIORITY_HIGH           5

//MMU Schemes supported (default or undefined is HLOS)
#define DAL_CONFIG_MMU_SCHEME_MULTITHREADED_OS  0x1
#define DAL_CONFIG_MMU_SCHEME_HLOS              0x1
#define DAL_CONFIG_MMU_SCHEME_NONE              0x8
#define DAL_CONFIG_MMU_SCHEME_QCOM_BOOT         0x4
#define DAL_CONFIG_MMU_SCHEME_L4_PMEM           0x3

// Ops which can be ORed into marhal flags
#define DALFW_REMOTE_ENVIRONMENT                0x00000001
#define DALFW_REMOTE_PROCESS                    0x00000003
#define DALFW_REMOTE_CALL                       0x00000003

// Ops which can be Ored into MemDescCopy
#define DALFW_MEMDESC_OP_IN                     0x10000000
#define DALFW_MEMDESC_OP_OUT                    0x20000000


#define DAL_SYSTEM_NOTIFY_RDAL_DOWN              1
#define DAL_SYSTEM_NOTIFY_RDAL_RESET             0

#define DAL_GET_DEVICE_ATTACH_COUNT(hDevice)\
    (((DALInterface*)hDevice)->pclientCtxt->pDALDevCtxt->dwRefs)

#define DAL_GET_DEVICE_ID(hDevice)\
    (((DALInterface*)hDevice)->pclientCtxt->pDALDevCtxt->DevId)

#define DALFW_GET_CLIENT_CTXT(hDevice)\
    (((DALInterface*)hDevice)->pclientCtxt)

#define DALFW_SET_REMOTE_ENV_FLAG(hDevice)\
   (((DALInterface*)hDevice)->pclientCtxt->dwAccessMode |= DALFW_REMOTE_ENVIRONMENT)

#define DALFW_SET_REMOTE_CHANNEL_ID(hDevice,channel_id)\
   (((DALInterface*)hDevice)->pclientCtxt->dwAccessMode |= (channel_id<<2))

#define DALFW_GET_REMOTE_CHANNEL_ID(hDevice)\
   ((((DALInterface*)hDevice)->pclientCtxt->dwAccessMode & 0x000000C0)>>2)

#define DALFW_SET_CLIENT_PORT_CTXT(hDevice,pPortCtxt)\
   (((DALInterface*)hDevice)->pclientCtxt->pPortCtxt = pPortCtxt)

#define DALFW_SET_TO_HANDLE(hDevice,to_handle)\
    (((DALInterface*)hDevice)->pclientCtxt->pDALDevCtxt->reserve[0] = (uint32)to_handle)

#define DALFW_GET_TO_HANDLE(pclientCtxt)\
    (pclientCtxt->pDALDevCtxt->reserve[0])

#define DALFW_MarshalCaller(pClientCtx, dwMarshalFlags) \
   (dwMarshalFlags = pClientCtx->dwAccessMode & DALFW_REMOTE_CALL)

#define DALFW_GET_SYSTEM_NOTIFY_EVENT(pclientCtxt)\
    (pclientCtxt && ((DALClientCtxt*)pclientCtxt)->pDALDevCtxt)?((DALSYSEventHandle)(((DALClientCtxt*)pclientCtxt)->pDALDevCtxt->reserve[1])):NULL

DALResult 
DALFW_AttachToStringDevice(const char *pszDeviceName, DALDrvCtxt *pdalDrvCtxt, 
                           DALClientCtxt *pClientCtxt);

DALResult
DALFW_AttachToDevice(DALDEVICEID devId,DALDrvCtxt *pdalDrvCtxt,
                     DALClientCtxt *pdalClientCtxt);

/** \details
 * This function can be used from DAL DeviceInit entry point to mark device as
 * static.  This will prevent detach from calling DeviceDeInit.
 *
 * @param[in] pdalDevCtxt
 *    pointer to the DAL device context
 */
void
DALFW_MarkDeviceStatic(DALDevCtxt *pdalDevCtxt);

uint32
DALFW_AddRef(DALClientCtxt *pdalClientCtxt);

uint32
DALFW_Release(DALClientCtxt *pdalClientCtxt);

//The following should be used by drivers which use WorkLoops & timers
void
DALFW_SystemAttach(DALDevCtxt *pDevCtxt);

void
DALFW_SystemDetach(DALDevCtxt *pDevCtxt);

DALSYSWorkLoopHandle *
DALFW_GetWorkLoop(DALDevCtxt *pDevCtxt, uint32 dwWorkLoopPriority);


/*----------------------------------------------------------------------------------*/
/*----------------------------- DDI Helper APIs ------------------------------------*/
/*----------------------------------------------------------------------------------*/

DALMemObject *
DALFW_CopyMemObjectToDDI(DALSYSMemHandle hMem, DALMemObject * pDestMemObject);

DALSYSMemHandle
DALFW_CreateMemObjectFromDDI(uint32 dwMarshalFlags, DALMemObject * pInMemObject);

DALSYSEventHandle
DALFW_CreateEventFromDDI(DALClientCtxt * pClientCtxt, uint32 dwMarshalFlags,
                DALSYSEventHandle inHandle, DALEventObject * pPrealloc);

DALSYSEventHandle
DALFW_CreatePayloadEventFromDDI(DALClientCtxt * pClientCtxt, uint32 dwMarshalFlags,
                DALSYSEventHandle inHandle, DALEventObject * pPrealloc);

uint32
DALFW_InitDDIMemDescListFromSys(uint32 dwFlags,
                             DALSysMemDescList *pInMemDescList,
                             DALDDIMemDescList * pDestDDIMemDescList);

DALSysMemDescList *
DALFW_InitSysMemDescListFromDDI(DALSYSMemHandle hMemHandle,
                             DALDDIMemDescList *pInDDIMemDescList,
                             DALSysMemDescList *pDestMemDescList);
void
DALFW_RegisterSystemNotificationEvent(DalDeviceHandle * h, DALSYSEventHandle hEvent);

void memory_barrier(void);

#define DALFW_MemoryBarrier(addr) memory_barrier()

// void DALFW_memset(void * pDest, int iVal, uint32 iLen);

/*----------------------------------------------------------------------------------*/
/*----------------------------- Memory Descriptor Lists ----------------------------*/
/*----------------------------------------------------------------------------------*/

#define ALLOC_PHYS_MEMDESC_OP_READ   0
#define ALLOC_PHYS_MEMDESC_OP_WRITE  1

DALSysMemDescBuf *
DALFW_MemDescBufPtr(DALSysMemDescList * pMemDescList, uint32 idx);

/*
  @brief Intialize the memory decriptor list

  This API must be used by clients to initialize memory descriptors for a given
  DDI IO  Memory region. Client can access the buffers within the DDI IO Memory
  region through these memory descriptors.

  @param hMem          : Handle to the DALSYS Mem Object
         pMemDescList   : Pointer to the Memory Descriptor
         dwNumBufs     : Number of buffers to be intialized

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALFW_MemDescInit(DALSYSMemHandle hMem, DALSysMemDescList * pMemDescList,
                  uint32 dwNumBufs);

/*
  @brief Adds a buffer to the Memory Descriptor List

  Memory Descriptors must be setup by clients to point to desired DDI IO buffers
  . Client must use this API to associate the Memory Descriptors buffers to the
  DDI IO Mem region buffers, this must be done before they start using them.

  @param hMem            : Handle to the DALSYS Mem Object
         dwMemDescBufIdx : Memory Descriptor Buffer Index
         dwOffset        : Offset from base of the mem region
         dwSize          : Size of this Buffer

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALFW_MemDescAddBuf(DALSysMemDescList * pMemDescList, uint32 bufIdx,
                    uint32 offset, uint32 size, uint32 userInfo);


/*
   Prepares and/or allocates memory friendly for DMA.
   hMem - is a memory handle returned by a previous call to this function.
   operation -
         ALLOC_PHYS_MEMDESC_OP_READ   0
         ALLOC_PHYS_MEMDESC_OP_WRITE  1
   pInDescList - pointer to input descriptor list (user supplied)
   pOutDescList - In/Out, caller must provide a list which
      may be populated if needed.
   NOTE: Dont forget to reset what pOutDescList points to between consecutive
      calls to DALDW_AllocPhysForMemDescBufs
   If this function fails it returns DALSYS_MEM_INVALID_ADDR.
*/
DALSYSMemHandle
DALFW_AllocPhysForMemDescBufs(DALSYSMemHandle hMem,
                              uint32 operation,
                              DALSysMemDescList *pInDescList,
                              DALSysMemDescList ** pOutDescList);


/*
   Based on the lists this function makes sure that the
   destination list buffer data is the source list buffer data.
*/
DALResult
DALFW_MemDescListCopyBufs(DALSysMemDescList * pDestDescList,
                          DALSysMemDescList * pSrcDescList);


#define DALFW_MemDescGetPhysAddr(memdescList, idx)\
    ((DALSYSMemAddr)((DALSysMemDescList*)memdescList)->BufInfo[(idx)].PhysicalAddr)

#define DALFW_MemDescGetVirtAddr(memdescList, idx) \
    ((DALSYSMemAddr)((DALSysMemDescList*)memdescList)->BufInfo[(idx)].VirtualAddr)



/*----------------------------------------------------------------------------------*/
/*------------------------------ Atomic Operations ---------------------------------*/
/*----------------------------------------------------------------------------------*/


/*
   NOTE: Only LockSpinLockExt and UnLockSpinLockExt may be used external Core.
*/


/*
   LockedExchange
   Performs a word exchange at address pTarget
*/
uint32 DALFW_LockedExchangeW(volatile uint32 *pTarget, uint32 value);

/*
   LockedIncrement
   increments and returns the new value
   NOTE: Cant be used inter-processor from ARM9
*/
uint32 DALFW_LockedIncrementW(volatile uint32 *pTarget);

/*
   LockedDecrement
   increments and returns the new value
   NOTE: Cant be used inter-processor from ARM9
*/
uint32 DALFW_LockedDecrementW(volatile uint32 *pTarget);


/*
   LockedGetModifySetW
   performs *pTarget = (*pTarget & mask ) | value
   NOTE: Cant be used inter-processor from ARM9
*/
uint32 DALFW_LockedGetModifySetW(volatile uint32 *pTarget, uint32 AND_value, uint32 OR_value);


/*
   LockedCompareExchangeW
   performs
      if(*pTarget == comparand)
         *pTarget = value
         rerurn value
      else
         return *pTarget
   NOTE: Cant be used inter-processor from ARM9
*/
uint32 DALFW_LockedCompareExchangeW(volatile uint32 *pTarget, uint32 comparand, uint32 value);

/*----------------------------------------------------------------------------------*/
/*----------- Raw non-shared spinlocks (i.e. internal within a Core) ---------------*/
/*----------------------------------------------------------------------------------*/

/*
   DALFW_LockSpinLock
   raw spin lock, implementation uses DAL_LockedExchangeW
   NOTE: pid must not be 0
   NOTE: spinlock address should be "InterCore-Non-Shared"
            typically cached.
*/
void DALFW_LockSpinLock(volatile uint32 *spinLock, uint32 pid);

/*
   DALFW_UnLockSpinLock
   raw spin lock unlock
   NOTE: pid must not be 0
   NOTE: spinlock address should be "InterCore-Non-Shared"
            typically cached.
*/
void DALFW_UnLockSpinLock(volatile uint32 *spinLock);


/*----------------------------------------------------------------------------------*/
/*--------------- Raw Externally shared spinlocks (i.e. within Core) ---------------*/
/*----------------------------------------------------------------------------------*/

/*
   DALFW_LockSpinLock
   raw spin lock, implementation uses DAL_LockedExchangeW
   NOTE: pid must not be 0
   NOTE: spinlock address must be Uncached and "InterCore-Shared"
*/
void DALFW_LockSpinLockExt(volatile uint32 *spinLock, uint32 pid);

/*
   DALFW_UnLockSpinLock
   raw spin lock unlock
   NOTE: pid must not be 0
   NOTE: spinlock address must be Uncached and "InterCore-Shared"
*/
void DALFW_UnLockSpinLockExt(volatile uint32 *spinLock);

/*
   DALFW_UnLockSpinLock
   raw spin lock, try, if not available return DAL_ERROR
   NOTE: pid must not be 0
   NOTE: spinlock address must be Uncached and "InterCore-Shared"
*/

DALResult DALFW_TryLockSpinLockExt(volatile uint32 *spinLock, uint32 pid);


/*----------------------------------------------------------------------------------*/
/*------------------------- Inter-Core Locks with token ------------------------*/
/*----------------------------------------------------------------------------------*/

typedef struct _DAFFW_MPLOCK
{
 	volatile uint32 spin_lock;
 	volatile uint32 owner;
 	volatile uint32 waiting;
   volatile uint32 size;
}
DALFW_MPLOCK;

/*
   DALFW_LockSpinLock
   raw spin lock, implementation uses DAL_LockedExchangeW
   NOTE: pid must not be 0
*/
void DALFW_MPLock(DALFW_MPLOCK *pLock, uint32 pid, uint32 delayParam);

/*
   DALFW_UnLockSpinLock
   raw spin lock unlock
   NOTE: pid must not be 0
*/
void DALFW_MPUnLock(DALFW_MPLOCK *pLock);

/*
   DALFW_TryLockSpinLock
   raw try spin lock, implementation uses DAL_LockedExchangeW
   NOTE: pid must not be 0
*/
uint32 DALFW_TrySpinLockEx(DALFW_MPLOCK *pLock, uint32 pid);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* DALFRAMEWORK_H */

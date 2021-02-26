#ifndef DALSYSTYPES_H
#define DALSYSTYPES_H
/*==============================================================================
@file DALSysTypes.h


        Copyright (c) 2008,2010 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/api/dal/DALSysTypes.h#1 $

==============================================================================*/

/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "devcfgTypes.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALReg.h"

/*------------------------------------------------------------------------------
Defines for DALSYS Object sizes
------------------------------------------------------------------------------*/
#define DALSYS_BASE_OBJ_SIZE_BYTES           12
#define DALSYS_EVENT_OBJ_SIZE_BYTES          80
#define DALSYS_WORKLOOP_EVENT_OBJ_SIZE_BYTES 32
#define DALSYS_WORKLOOP_OBJ_SIZE_BYTES       64
#define DALSYS_SYNC_OBJ_SIZE_BYTES           40
#define DALSYS_MEM_OBJ_SIZE_BYTES            40
#define DALSYS_MEM_DESC_BASE_OBJ_SIZE_BYTES  20
#define DALSYS_MEM_DESC_INFO_OBJ_SIZE_BYTES  8

/*------------------------------------------------------------------------------
* Type declaration
*-----------------------------------------------------------------------------*/
typedef void  * DALSYSObjHandle;
typedef void  * DALSYSSyncHandle;
typedef void  * DALSYSMemHandle;
typedef void  * DALSYSWorkLoopHandle;

typedef struct DALSYSBaseObj DALSYSBaseObj;
struct DALSYSBaseObj
{
   uint32 dwObjInfo;
   uint32 hOwnerProc;
   DALSYSMemAddr thisVirtualAddr;
};

#define DALSYS_MEM_DESC_LIST_SIZE(numBufs) \
    (sizeof(DALSysMemDescList) + ((numBufs-1)*sizeof(DALSysMemDescBufInfo))

#define DALDDI_MEM_DESC_LIST_SIZE(numBufs) \
    (sizeof(DALDDIMemDescList) + ((numBufs-1)*sizeof(DALDDIMemBufDesc)))

typedef struct DALSYSEventObj  DALSYSEventObj;
struct DALSYSEventObj
{
   unsigned long long _bSpace[DALSYS_EVENT_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
};

typedef struct DALSYSSyncObj  DALSYSSyncObj;
struct DALSYSSyncObj
{
   unsigned long long _bSpace[DALSYS_SYNC_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
};

typedef struct DALSYSMemObj  DALSYSMemObj;
struct DALSYSMemObj
{
   unsigned long long _bSpace[DALSYS_MEM_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
};

typedef struct DALSYSWorkLoopEventObj DALSYSWorkLoopEventObj;
struct DALSYSWorkLoopEventObj
{
   unsigned long long _bSpace[DALSYS_WORKLOOP_EVENT_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
};

typedef struct DALSYSWorkLoopObj DALSYSWorkLoopObj;
struct DALSYSWorkLoopObj
{
   unsigned long long _bSpace[DALSYS_WORKLOOP_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
};

typedef void * DALSYSCallbackFuncCtx;
typedef void * (*DALSYSCallbackFunc)(void*,uint32,void*,uint32);


typedef struct DALSYSMemInfo DALSYSMemInfo;
struct DALSYSMemInfo
{
    DALSYSMemAddr VirtualAddr;
    DALSYSMemAddr PhysicalAddr;
    uint32        dwLen;
    uint32        dwMappedLen;
    uint32        dwProps;
};

typedef enum
{
   DALSYS_MEM_CACHE_DEVICE,
   DALSYS_MEM_CACHE_NONE,
   DALSYS_MEM_CACHE_WRITEBACK,
   DALSYS_MEM_CACHE_WRITETHROUGH,
   DALSYS_MEM_CACHE_INVALID
} 
DALSYSMemCacheType;

typedef enum
{
   DALSYS_MEM_PERM_NONE=0,
   DALSYS_MEM_PERM_R=0x1,
   DALSYS_MEM_PERM_W=0x2,
   DALSYS_MEM_PERM_RW=0x3,
   DALSYS_MEM_PERM_X=0x4,
   DALSYS_MEM_PERM_RX=0x5,
   DALSYS_MEM_PERM_WX=0x6,
   DALSYS_MEM_PERM_RWX=0x7,
   DALSYS_MEM_PERM_INVALID=0x8
} 
DALSYSMemPermType;

typedef enum
{
   DALSYS_MEM_SHARE_ALLOWED,
   DALSYS_MEM_SHARE_NOT_ALLOWED,
   DALSYS_MEM_SHARE_INVALID
}
DALSYSMemShareType;

// extended memory information structure
typedef struct DALSYSMemInfoEx DALSYSMemInfoEx;
struct DALSYSMemInfoEx
{
    DALSYSPhyAddr          physicalAddr;
    DALSYSMemAddr          virtualAddr;
    DALSYSMemAddr          size;
    DALSYSMemCacheType     cacheType;
    DALSYSMemPermType      permission;
    DALSYSMemShareType     shareType;
};

// memory region request structure
// DALSYS_MEM_REQ_OBJECT() must be used to define and initialize
// this request structure.  In case new structure members are added,
// using this macro will ensure the new members 
// will be initialized with default values.
typedef struct DALSYSMemReq DALSYSMemReq;
struct DALSYSMemReq
{
    DALSYSMemInfoEx        memInfo;
    // memory for memory region object (optional and may be NULL)
    DALSYSMemObj           *pObj;
    // true if memory region is preallocated with native OS API, false otherwise
    DALBOOL                prealloc;
};

/*------------------------------------------------------------------------------
* MACROS for declaring DALSys objects without requiring memory heap
*-----------------------------------------------------------------------------*/

#define DALSYS_EVENT_OBJECT(varName) \
   DALSYSEventObj varName

#define DALSYS_SYNC_OBJECT(varName) \
   DALSYSSyncObj varName

#define DALSYS_MEM_OBJECT(varName) \
    DALSYSMemObj varName

#define DALSYS_MEM_REQ_OBJECT(varName) \
   DALSYSMemReq varName = {{DALSYS_MEM_ADDR_NOT_SPECIFIED, \
      DALSYS_MEM_ADDR_NOT_SPECIFIED, 0, DALSYS_MEM_CACHE_NONE, \
      DALSYS_MEM_PERM_RW, DALSYS_MEM_SHARE_NOT_ALLOWED}, NULL, FALSE}

#define DALSYS_WORKLOOP_OBJECT(varName) \
   DALSYSWorkLoopObj varName

#define DALDDI_MEM_DESC_LIST_OBJECT(varName,numBufs) \
struct {\
    uint32          dwFlags;\
    uint32          dwNumBufs;\
    DALDDIMemBufDesc   bufList[numBufs];\
} varName






typedef DALResult
(*DALSYSWorkLoopExecute)
(
    DALSYSEventHandle,
    void *
);

typedef void
(*DALSYS_InitSystemHandleFncPtr)
(
    DalDeviceHandle * hDalDevice
);

typedef DALResult
(*DALSYS_DestroyObjectFncPtr)
(
    DALSYSObjHandle
);

typedef DALResult
(*DALSYS_CopyObjectFncPtr)
(
    DALSYSObjHandle,
    DALSYSObjHandle *
);

typedef DALResult
(*DALSYS_RegisterWorkLoopFncPtr)
(
    uint32,
    uint32,
    DALSYSWorkLoopHandle *,
    DALSYSWorkLoopObj *
);

typedef DALResult
(*DALSYS_RegisterWorkLoopExFncPtr)
(
    char *,
    uint32,
    uint32,
    uint32,
    DALSYSWorkLoopHandle *,
    DALSYSWorkLoopObj *
);

typedef DALResult
(*DALSYS_AddEventToWorkLoopFncPtr)
(
    DALSYSWorkLoopHandle,
    DALSYSWorkLoopExecute,
    void *,
    DALSYSEventHandle,
    DALSYSSyncHandle
);

typedef DALResult
(*DALSYS_DeleteEventFromWorkLoopFncPtr)
(
    DALSYSWorkLoopHandle,
    DALSYSEventHandle
);

typedef DALResult
(*DALSYS_EventCreateFncPtr)
(
    uint32 ,
    DALSYSEventHandle *,
    DALSYSEventObj *
);

typedef DALResult
(*DALSYS_EventCtrlFncPtr)
(
    DALSYSEventHandle,
    uint32,
   uint32,
   void *,
   uint32
);

typedef DALResult
(*DALSYS_EventWaitFncPtr)
(
    DALSYSEventHandle
);

typedef DALResult
(*DALSYS_EventMultipleWaitFncPtr)
(
    DALSYSEventHandle*,
    int,
    uint32,
    uint32 *
);
typedef DALResult
(*DALSYS_SetupCallbackEventFncPtr)
(
    DALSYSEventHandle,
    DALSYSCallbackFunc,
    DALSYSCallbackFuncCtx
);

typedef DALResult
(*DALSYS_SyncCreateFncPtr)
(
    uint32,
    DALSYSSyncHandle *,
    DALSYSSyncObj *
);

typedef void
(*DALSYS_SyncEnterFncPtr)
(
    DALSYSSyncHandle
);

typedef DALResult
(*DALSYS_SyncTryEnterFncPtr)
(
    DALSYSSyncHandle
);

typedef void
(*DALSYS_SyncLeaveFncPtr)
(
    DALSYSSyncHandle
);

typedef DALResult
(*DALSYS_MemRegionAllocFncPtr)
(
    uint32,
    DALSYSMemAddr,
    DALSYSMemAddr,
    uint32,
    DALSYSMemHandle *,
    DALSYSMemObj *
);

typedef DALResult
(*DALSYS_MemRegionMapPhysFncPtr)
(
    DALSYSMemHandle,
    uint32,
    DALSYSMemAddr,
    uint32
);

typedef DALResult
(*DALSYS_MemInfoFncPtr)
(
    DALSYSMemHandle,
    DALSYSMemInfo *
);

typedef DALResult
(*DALSYS_CacheCommandFncPtr)
(
    uint32 ,
    DALSYSMemAddr,
    uint32
);

typedef DALResult
(*DALSYS_MallocFncPtr)
(
    uint32 ,
    void **
);

typedef DALResult
(*DALSYS_FreeFncPtr)
(
    void *
);

typedef void
(*DALSYS_BusyWaitFncPtr)
(
   uint32
);

typedef DALResult
(*DALSYS_MemDescAddBufFncPtr)
(
    DALSysMemDescList *,
    uint32,
    uint32,
    uint32
);

typedef DALResult
(*DALSYS_MemDescPrepareFncPtr)
(
    DALSysMemDescList *,
    uint32
);

typedef DALResult
(*DALSYS_MemDescValidateFncPtr)
(
    DALSysMemDescList *,
    uint32
);

typedef DALResult
(*DALSYS_GetDALPropertyHandleFncPtr)
(
    DALDEVICEID,
    DALSYSPropertyHandle
);

typedef DALResult
(*DAL_DeviceAttachFncPtr)
(
    const char *pszArg,
    DALDEVICEID,
    DalDeviceHandle **
);

typedef DALResult
(*DAL_DeviceDetachFncPtr)
(
    DalDeviceHandle *
);

typedef DALResult
(*DAL_DeviceAttachExFncPtr)
(
	const char *pszArg,
    DALDEVICEID DevId,
    DALInterfaceVersion ClientVersion,
    DalDeviceHandle **phDalDevice
);

typedef DALResult
(*DALSYS_GetPropertyValueFncPtr)
(
    DALSYSPropertyHandle,
    const char *,
    uint32,
    DALSYSPropertyVar *
);

typedef void
(*DALSYS_LogEventFncPtr)
(
    DALDEVICEID,
    uint32,
    const char *
);

typedef struct DALSYSFncPtrTbl DALSYSFncPtrTbl;
struct DALSYSFncPtrTbl
{
    DALSYS_InitSystemHandleFncPtr DALSYS_InitSystemHandleFnc;
    DALSYS_DestroyObjectFncPtr DALSYS_DestroyObjectFnc;
    DALSYS_CopyObjectFncPtr DALSYS_CopyObjectFnc;
    DALSYS_RegisterWorkLoopFncPtr DALSYS_RegisterWorkLoopFnc;
    DALSYS_RegisterWorkLoopExFncPtr DALSYS_RegisterWorkLoopExFnc;
    DALSYS_AddEventToWorkLoopFncPtr DALSYS_AddEventToWorkLoopFnc;
    DALSYS_DeleteEventFromWorkLoopFncPtr DALSYS_DeleteEventFromWorkLoopFnc;
    DALSYS_EventCreateFncPtr DALSYS_EventCreateFnc;
    DALSYS_EventCtrlFncPtr DALSYS_EventCtrlFnc;
    DALSYS_EventWaitFncPtr DALSYS_EventWaitFnc;
    DALSYS_EventMultipleWaitFncPtr DALSYS_EventMultipleWaitFnc;
    DALSYS_SetupCallbackEventFncPtr DALSYS_SetupCallbackEventFnc;
    DALSYS_SyncCreateFncPtr DALSYS_SyncCreateFnc;
    DALSYS_SyncEnterFncPtr DALSYS_SyncEnterFnc;
    DALSYS_SyncTryEnterFncPtr DALSYS_SyncTryEnterFnc;
    DALSYS_SyncLeaveFncPtr DALSYS_SyncLeaveFnc;
    DALSYS_MemRegionAllocFncPtr DALSYS_MemRegionAllocFnc;
    DALSYS_MemRegionMapPhysFncPtr DALSYS_MemRegionMapPhysFnc;
    DALSYS_MemInfoFncPtr DALSYS_MemInfoFnc;
    DALSYS_CacheCommandFncPtr DALSYS_CacheCommandFnc;
    DALSYS_MallocFncPtr DALSYS_MallocFnc;
    DALSYS_FreeFncPtr DALSYS_FreeFnc;
    DALSYS_BusyWaitFncPtr DALSYS_BusyWaitFnc;
    DALSYS_MemDescAddBufFncPtr DALSYS_MemDescAddBufFnc;
    DALSYS_MemDescPrepareFncPtr DALSYS_MemDescPrepareFnc;
    DALSYS_MemDescValidateFncPtr DALSYS_MemDescValidateFnc;
    DALSYS_GetDALPropertyHandleFncPtr DALSYS_GetDALPropertyHandleFnc;
    DAL_DeviceAttachFncPtr DALSYS_DeviceAttachFnc;
    DAL_DeviceAttachExFncPtr DALSYS_DeviceAttachExFnc;
    DAL_DeviceAttachExFncPtr DALSYS_DeviceAttachRemoteFnc;
    DAL_DeviceDetachFncPtr DALSYS_DeviceDetachFnc;
    DALSYS_GetPropertyValueFncPtr DALSYS_GetPropertyValueFnc;
    DALSYS_LogEventFncPtr DALSYS_LogEventFnc;
};

//typedefs of external functions needed by Remoting
typedef DALResult
(*DALRemote_NewFncPtr)(const char *, DALDEVICEID, DalDeviceHandle **);
typedef int
(*DALRemote_CommonInitFncPtr)(void);
typedef void
(*DALRemote_IPCInitFcnPtr)(uint32);
typedef int
(*DALRemote_InitFncPtr)(void);
typedef DALSYSEventHandle
(*DALRemote_CreateEventFncPtr)(void *pClientCtxt, DALSYSEventHandle hRemote);
typedef uint32
(*DALRemoteInterProcessCallFncPtr)(void *in_buf, void *out_buf);
typedef int
(*DALRemote_DeinitFncPtr)(void);

typedef struct DALRemoteVtbl DALRemoteVtbl;
struct DALRemoteVtbl
{
   DALRemote_NewFncPtr DALRemote_NewFnc;
   DALRemote_CommonInitFncPtr DALRemote_CommonInitFnc;
   DALRemote_InitFncPtr DALRemote_InitFnc;
   DALRemote_DeinitFncPtr DALRemote_DeinitFnc;
   DALRemote_CreateEventFncPtr DALRemote_CreateEventFnc;
   DALRemote_CreateEventFncPtr DALRemote_CreatePayloadEventFnc;
   DALRemoteInterProcessCallFncPtr DALRemoteInterProcessCallFnc;
   DALRemote_IPCInitFcnPtr DALRemote_IPCInitFcn;
};

typedef struct DALSYSConfig DALSYSConfig;
struct DALSYSConfig
{
    void *pNativeEnv;
    uint32 dwConfig;
   DALRemoteVtbl *pRemoteVtbl;
};

#endif /* DALSYSTYPES_H */

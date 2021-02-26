#ifndef DALSYSTYPES_H
#define DALSYSTYPES_H
/*==============================================================================
@file DALSysTypes.h


        Copyright (c) 2008,2010,2014 Qualcomm Technologies, Inc.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

==============================================================================*/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/19/14   sho    Include device list in DALSYS configuration 
 08/14/14   aa     Added Function declarations for new API to support String 
                   based devices, updated function table
 08/14/14   aa     Added history 

=============================================================================*/
/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALReg.h"

/*------------------------------------------------------------------------------
Defines for DALSYS Object sizes
------------------------------------------------------------------------------*/
#define DALSYS_BASE_OBJ_SIZE_BYTES           12
#define DALSYS_EVENT_OBJ_SIZE_BYTES          64
#define DALSYS_WORKLOOP_EVENT_OBJ_SIZE_BYTES 32
#define DALSYS_WORKLOOP_OBJ_SIZE_BYTES       40
#define DALSYS_SYNC_OBJ_SIZE_BYTES           40
#define DALSYS_MEM_OBJ_SIZE_BYTES            36
#define DALSYS_MEM_DESC_BASE_OBJ_SIZE_BYTES  20
#define DALSYS_MEM_DESC_INFO_OBJ_SIZE_BYTES  8

/*------------------------------------------------------------------------------
* Type declaration
*-----------------------------------------------------------------------------*/
typedef void  * DALSYSObjHandle;
typedef void  * DALSYSSyncHandle;
typedef void  * DALSYSMemHandle;
typedef void  * DALSYSWorkLoopHandle;
typedef DALSYSMemAddr   * DALSYSPropertyHandle;

typedef struct  DALSYSPropertyVar DALSYSPropertyVar;
struct DALSYSPropertyVar
{
    uint32 dwType;
    uint32 dwLen;
    union
    {
        byte *pbVal;
        char *pszVal;
        unsigned long dwVal;
        uint32 *pdwVal;
        const void *pStruct;
    }Val;
};

// Struct to hold array of all Drivers which has String names
typedef struct  StringDevice StringDevice;
 struct StringDevice{
   const char *pszName; // Name of Device
   uint32 dwHash; // Hash Value
   uint32 dwOffset; // Offset
   DALREG_DriverInfo *pFunctionName; // Function Pointer
   uint32 dwNumCollision; // Number of Collisions
   uint32 *pdwCollisions; // List of Collisions offset in the array
};

typedef struct DALProps DALProps;
struct DALProps
{
   const byte *pDALPROP_PropBin;
   const void **pDALPROP_StructPtrs;
   uint32 dwDeviceSize; //Size of Devices array
   const StringDevice * pDevices;     //String Device array 
};

typedef struct DALREG_StringDeviceInfoList DALREG_StringDeviceInfoList;
struct DALREG_StringDeviceInfoList
{
   uint32 dwLen;                    // size of devices array
   const StringDevice *pDevices;    // string device array
};

typedef struct DALSYSBaseObj DALSYSBaseObj;
struct DALSYSBaseObj
{
   uint32 dwObjInfo;
   uint32 hOwnerProc;
   DALSYSMemAddr thisVirtualAddr;
};

//Base DALSYS Mem Object
typedef struct DALSYSMemObjBase DALSYSMemObjBase;
struct DALSYSMemObjBase
{
   INHERIT_DALSYS_BASE_OBJ();
   uint32        dwLen;
   DALSYSMemAddr OwnerVirtualAddr;
   DALSYSMemAddr VirtualAddr;
   DALSYSMemAddr PhysicalAddr;
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

/*------------------------------------------------------------------------------
* MACROS for declaring DALSys objects without requiring memory heap
*-----------------------------------------------------------------------------*/

#define DALSYS_EVENT_OBJECT(varName) \
   DALSYSEventObj varName

#define DALSYS_SYNC_OBJECT(varName) \
   DALSYSSyncObj varName

#define DALSYS_MEM_OBJECT(varName) \
    DALSYSMemObj varName

#define DALSYS_WORKLOOP_OBJECT(varName) \
   DALSYSWorkLoopObj varName

#define DALSYS_PROPERTY_HANDLE_DECLARE(varName) DALSYSMemAddr varName[2]

#define DALSYS_MEM_DESC_LIST_OBJECT(varName,numBufs) \
struct {\
    INHERIT_DALSYS_BASE_OBJ();\
    DALSYSMemAddr PhysicalAddr;\
    DALSYSMemAddr VirtualAddr;\
    uint32        dwCurBufIdx;\
    uint32        dwNumDescBufs;\
    DALSysMemDescBuf BufInfo[numBufs];\
} varName

#define DALDDI_MEM_DESC_LIST_OBJECT(varName,numBufs) \
struct {\
    uint32          dwFlags;\
    uint32          dwNumBufs;\
    DALDDIMemBufDesc   bufList[numBufs];\
} varName

/*------------------------------------------------------------------------------
* Function Declarations
*-----------------------------------------------------------------------------*/
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
(*DALSYS_GetDALPropertyHandleStrFncPtr)
(
    const char *,
    DALSYSPropertyHandle
);

typedef DALResult
(*DAL_DeviceAttachFncPtr)
(
    DALDEVICEID,
    DalDeviceHandle **
);

typedef DALResult
(*DAL_DeviceAttachStrFncPtr)
(
    const char *,
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
    const char *,
    DALDEVICEID ,
    DALInterfaceVersion ,
    DalDeviceHandle **
);
    
typedef DALResult
(*DAL_DeviceAttachStrExFncPtr)
(
    const char *,
    const char *,
    DALInterfaceVersion ,
    DalDeviceHandle **
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
    const char *,
    ...
);

typedef DALResult
(*DALGLBCTXT_AllocCtxtFncPtr)
(
   const char * name, 
   UINT32 size_req, 
   UINT32 lock_type, 
   VOID ** ctxt
);

typedef DALResult
(*DALGLBCTXT_FindCtxtFncPtr)
(const char * name, VOID ** ctxt);


typedef struct DALSYSFncPtrTbl DALSYSFncPtrTbl;
struct DALSYSFncPtrTbl
{
    DALSYS_InitSystemHandleFncPtr DALSYS_InitSystemHandleFnc;
    DALSYS_DestroyObjectFncPtr DALSYS_DestroyObjectFnc;
    DALSYS_CopyObjectFncPtr DALSYS_CopyObjectFnc;
    DALSYS_RegisterWorkLoopFncPtr DALSYS_RegisterWorkLoopFnc;
    DALSYS_AddEventToWorkLoopFncPtr DALSYS_AddEventToWorkLoopFnc;
    DALSYS_DeleteEventFromWorkLoopFncPtr DALSYS_DeleteEventFromWorkLoopFnc;
    DALSYS_EventCreateFncPtr DALSYS_EventCreateFnc;
    DALSYS_EventCtrlFncPtr DALSYS_EventCtrlFnc;
    DALSYS_EventWaitFncPtr DALSYS_EventWaitFnc;
    DALSYS_EventMultipleWaitFncPtr DALSYS_EventMultipleWaitFnc;
    DALSYS_SetupCallbackEventFncPtr DALSYS_SetupCallbackEventFnc;
    DALSYS_SyncCreateFncPtr DALSYS_SyncCreateFnc;
    DALSYS_SyncEnterFncPtr DALSYS_SyncEnterFnc;
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
    DALSYS_GetDALPropertyHandleStrFncPtr DALSYS_GetDALPropertyHandleStrFnc;
    DAL_DeviceAttachFncPtr DALSYS_DeviceAttachFnc;
    DAL_DeviceAttachExFncPtr DALSYS_DeviceAttachExFnc;
    DAL_DeviceAttachStrFncPtr DALSYS_StringDeviceAttachFnc;
    DAL_DeviceAttachStrExFncPtr DALSYS_StringDeviceAttachExFnc;
    DAL_DeviceAttachExFncPtr DALSYS_DeviceAttachRemoteFnc;
    DAL_DeviceDetachFncPtr DALSYS_DeviceDetachFnc;
    DALSYS_GetPropertyValueFncPtr DALSYS_GetPropertyValueFnc;
    DALSYS_LogEventFncPtr DALSYS_LogEventFnc;
    DALGLBCTXT_AllocCtxtFncPtr DALGLBCTXT_AllocCtxtFnc;
    DALGLBCTXT_FindCtxtFncPtr DALGLBCTXT_FindCtxtFnc;
};

typedef struct DALSYSConfig DALSYSConfig;
struct DALSYSConfig
{
   DALREG_StringDeviceInfoList stringDeviceInfoList;
   DALREG_DriverInfoList numericDriverInfoList;
};

#endif /* DALSYSTYPES_H */

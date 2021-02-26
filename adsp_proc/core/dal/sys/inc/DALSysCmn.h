/*==============================================================================
@file  DALSYSCmn.h

DALSYS common declarations and marcos

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
#ifndef DALSYSCMN_H
#define DALSYSCMN_H
#include "DALStdDef.h"
#include "DALSys.h"
#include "qurt.h"
#include <stdarg.h>

#define DALSYSCMN_MAX_NUM_MODS_PER_PROCESS 32

#define DALSYSCMN_HANDLE_SET_COPY( handle ) ( (void *)((uint32)handle | 0x00000001) )
#define DALSYSCMN_HANDLE_IS_COPY( handle ) ( (uint32)handle & 0x00000001 )
#define DALSYSCMN_HANDLE_TO_OBJECT( handle ) ( (uint32)handle & ~0x00000001 )

#define DALSYSCMN_MAX_NUM_SHARED_MEM_REGIONS  128


#define DALSYSCMN_OBJECT_ID_FIELD_MASK       0x0000000F
#define DALSYSCMN_OBJECT_ATTRIB_FIELD_MASK   0x000FFFF0
#define DALSYSCMN_DYNAMIC_ALLOC_FIELD_MASK   0x80000000

#define DALSYSCMN_DESTROY_OBJECT(dwObjInfo)\
   (dwObjInfo = 0xDEAD0000)

/* ptr  = pointer to a dalsys object, 
   size = size of the dalsys object p is pointing to. if size is specified,
        indexing starts from the bottom of the object struct
   idx  = index of the member in the dalsys object  
*/
#define DALSYSCMN_MOVEPTR(ptr, size, idx) (size) ? \
          (*(uint32*)ptr += size - (idx*4)) : \
          (*(uint32*)ptr += (idx-1)*4)

/*------------------------------------------------------------------------------
DALSYS Event Object specific bit fields
------------------------------------------------------------------------------*/
#define DALSYSCMN_EVENT_COPY_FIELD_MASK            0x10000000
#define DALSYSCMN_EVENT_ATTR_TIMER_MASK            (DALSYS_EVENT_ATTR_TIMEOUT_EVENT|DALSYS_EVENT_ATTR_TIMER_EVENT)

/*------------------------------------------------------------------------------
DALSYS Event Types
------------------------------------------------------------------------------*/
#define DALSYSCMN_WORKLOOP_EVENT             0x1
#define DALSYSCMN_TIMEOUT_EVENT              0x2
#define DALSYSCMN_CALLBACK_EVENT             0x3
#define EVENT_DEFAULT_SIGNAL                 0x01
#define EVENT_GROUP_SIGNAL                   0x02

/*------------------------------------------------------------------------------
DALSYS Mem Object specific bit fields
------------------------------------------------------------------------------*/
#define DALSYSCMN_MEM_COPY_FIELD_MASK      0x40000000

/*------------------------------------------------------------------------------
DALSYS Object Ids
------------------------------------------------------------------------------*/
#define  DALSYSCMN_EVENT_OBJECT_ID         0x1
#define  DALSYSCMN_SYNC_OBJECT_ID          0x2
#define  DALSYSCMN_MEM_OBJECT_ID           0x3
#define  DALSYSCMN_WORKLOOP_OBJECT_ID      0x4
#define  DALSYSCMN_MEM_DESC_OBJECT_ID      0x5

#define DALSYSCMN_RESET_ALL_BITS(dwObjInfo)\
   (dwObjInfo &= 0)

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query Object Id Field
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_OBJECT_ID(dwObjInfo, dwId)\
   (dwObjInfo |= dwId)

#define DALSYSCMN_RESET_OBJECT_ID(dwObjInfo)\
   (dwObjInfo &= ~DALSYSCMN_OBJECT_ID_FIELD_MASK)

#define DALSYSCMN_GET_OBJECT_ID(dwObjInfo)\
   (dwObjInfo&DALSYSCMN_OBJECT_ID_FIELD_MASK)

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query Attrib Alloc Bit Fields
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_ATTRIB(dwObjInfo, dwAttrib)\
   (dwObjInfo |= (dwAttrib & DALSYSCMN_OBJECT_ATTRIB_FIELD_MASK ))

#define DALSYSCMN_RESET_ATTRIB(dwObjInfo,dwAttrib)\
   (dwObjInfo &= ~(dwAttrib & DALSYSCMN_OBJECT_ATTRIB_FIELD_MASK))

#define DALSYSCMN_GET_ATTRIB(dwObjInfo,dwAttrib)\
   (dwAttrib = (dwObjInfo&DALSYSCMN_OBJECT_ATTRIB_FIELD_MASK))

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query Dynamic Alloc Bit Field
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_DYNAMIC_ALLOC_BIT(dwObjInfo)\
   (dwObjInfo |= DALSYSCMN_DYNAMIC_ALLOC_FIELD_MASK)

#define DALSYSCMN_RESET_DYNAMIC_ALLOC_BIT(dwObjInfo)\
   (dwObjInfo &= ~DALSYSCMN_DYNAMIC_ALLOC_FIELD_MASK)

#define DALSYSCMN_IS_OBJ_DYNAMICALLY_ALLOCED(dwObjInfo)\
   ((dwObjInfo&DALSYSCMN_DYNAMIC_ALLOC_FIELD_MASK) != 0)

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query Event Cilent Default Alloc Bit Field
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_CLIENT_DEFAULT_EVENT_BIT(dwObjInfo)\
   (dwObjInfo |= DALSYS_EVENT_ATTR_CLIENT_DEFAULT)

#define DALSYSCMN_RESET_CLIENT_DEFAULT_EVENT_BIT(dwObjInfo)\
   (dwObjInfo &= ~DALSYS_EVENT_ATTR_CLIENT_DEFAULT)

#define DALSYSCMN_IS_EVENT_A_CLIENT_DEFAULT(dwObjInfo)\
   ((dwObjInfo&DALSYS_EVENT_ATTR_CLIENT_DEFAULT) != 0)

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query Event Timeout Alloc Bit Field
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_TIMEOUT_EVENT_BIT(dwObjInfo)\
   (dwObjInfo |= DALSYS_EVENT_ATTR_TIMEOUT_EVENT)

#define DALSYSCMN_RESET_TIMEOUT_EVENT_BIT(dwObjInfo)\
   (dwObjInfo &= ~DALSYS_EVENT_ATTR_TIMEOUT_EVENT)

#define DALSYSCMN_IS_EVENT_A_TIMEOUT(dwObjInfo)\
   ((dwObjInfo&DALSYS_EVENT_ATTR_TIMEOUT_EVENT) != 0)

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query Event Periodic Timer Alloc Bit Field
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_TIMER_EVENT_BIT(dwObjInfo)\
   (dwObjInfo |= DALSYS_EVENT_ATTR_TIMER_EVENT)

#define DALSYSCMN_RESET_TIMER_EVENT_BIT(dwObjInfo)\
   (dwObjInfo &= ~DALSYS_EVENT_ATTR_TIMER_EVENT)

#define DALSYSCMN_IS_EVENT_A_TIMER(dwObjInfo)\
   ((dwObjInfo&DALSYS_EVENT_ATTR_TIMER_EVENT) != 0)

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query Event Non deferred Bit Field
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_EVENT_NON_DEFERRED_BIT(dwObjInfo)\
   (dwObjInfo |= DALSYS_EVENT_ATTR_NON_DEFERRED)

#define DALSYSCMN_RESET_EVENT_NON_DEFERRED_BIT(dwObjInfo)\
   (dwObjInfo &= ~DALSYS_EVENT_ATTR_NON_DEFERRED)

#define DALSYSCMN_IS_EVENT_NON_DEFERRED(dwObjInfo)\
   ((dwObjInfo&DALSYS_EVENT_ATTR_NON_DEFERRED) != 0)

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query Event Callback Alloc Bit Field
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_CALLBACK_EVENT_BIT(dwObjInfo)\
   (dwObjInfo |= DALSYS_EVENT_ATTR_CALLBACK_EVENT)

#define DALSYSCMN_RESET_CALLBACK_EVENT_BIT(dwObjInfo)\
   (dwObjInfo &= ~DALSYS_EVENT_ATTR_CALLBACK_EVENT)

#define DALSYSCMN_IS_EVENT_A_CALLBACK(dwObjInfo)\
   ((dwObjInfo&DALSYS_EVENT_ATTR_CALLBACK_EVENT) != 0)

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query Event Copy Alloc Bit Field
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_EVENT_COPY_BIT(dwObjInfo)\
   (dwObjInfo |= DALSYSCMN_EVENT_COPY_FIELD_MASK)

#define DALSYSCMN_RESET_EVENT_COPY_BIT(dwObjInfo)\
   (dwObjInfo &= ~DALSYSCMN_EVENT_COPY_FIELD_MASK)

#define DALSYSCMN_IS_EVENT_A_COPY(dwObjInfo)\
   ((dwObjInfo&DALSYSCMN_EVENT_COPY_FIELD_MASK) != 0)

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query WorkLoop Event Alloc Bit Field
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_WORKLOOP_EVENT_BIT(dwObjInfo)\
   (dwObjInfo |= DALSYS_EVENT_ATTR_WORKLOOP_EVENT)

#define DALSYSCMN_RESET_WORKLOOP_EVENT_BIT(dwObjInfo)\
   (dwObjInfo &= ~DALSYS_EVENT_ATTR_WORKLOOP_EVENT)

#define DALSYSCMN_IS_WORKLOOP_EVENT(dwObjInfo)\
   ((dwObjInfo&DALSYS_EVENT_ATTR_WORKLOOP_EVENT) != 0)

/*------------------------------------------------------------------------------
Macros to Set/Reset/Query Mem Obj Copy Bit Field
------------------------------------------------------------------------------*/
#define DALSYSCMN_SET_MEM_COPY_BIT(dwObjInfo)\
   (dwObjInfo |= DALSYSCMN_MEM_COPY_FIELD_MASK)

#define DALSYSCMN_RESET_MEM_COPY_BIT(dwObjInfo)\
   (dwObjInfo &= ~DALSYSCMN_MEM_COPY_FIELD_MASK)

#define DALSYSCMN_IS_MEM_A_COPY(dwObjInfo)\
   ((dwObjInfo&DALSYSCMN_MEM_COPY_FIELD_MASK) != 0)


/*------------------------------------------------------------------------------
Compiler asserts for DALSYS object size
------------------------------------------------------------------------------*/
#define DALSYSCMN_COMPILER_ASSERT(var)\
   switch(0){case 1:case var:;}

#define DALSYSCMN_CONTAINEROF(ptr, type, member)  \
   ({const typeof( ((type *)0)->member ) *__mptr = (ptr);  (type *)( (char *)__mptr - offsetof(type,member) );}) 

// helper macros to invoke error event with function name
#define DALSYS_LOG_INFO_EVENT(FORMAT, ... ) \
   DALSYS_LogEvent(0, DALSYS_LOGEVENT_INFO, "%s: " FORMAT, __FUNCTION__, ##__VA_ARGS__)

#define DALSYS_LOG_WARNING_EVENT(FORMAT, ... ) \
   DALSYS_LogEvent(0, DALSYS_LOGEVENT_WARNING, "%s: " FORMAT, __FUNCTION__, ##__VA_ARGS__)

#define DALSYS_LOG_ERROR_EVENT(FORMAT, ... ) \
   DALSYS_LogEvent(0, DALSYS_LOGEVENT_ERROR, "%s: " FORMAT, __FUNCTION__, ##__VA_ARGS__)

#define DALSYS_LOG_FATAL_EVENT(FORMAT, ... ) \
   DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR, "%s: " FORMAT, __FUNCTION__, ##__VA_ARGS__)

// macros to disambiguate symbols across PDs
#ifdef PD_SUFFIX
#define PD_VAR( name )        PD_VAR_1 (name, PD_SUFFIX)
#define PD_VAR_1( name, pd )  PD_VAR_2 (name, pd)
#define PD_VAR_2( name, pd )  name ## pd 
#else
#define PD_VAR(name)          name
#endif

typedef struct DALEnvInfo DALEnvInfo;
struct DALEnvInfo
{
    DalDeviceHandle *hTimer;
};

typedef struct _DALSYSWorkLoopEvent _DALSYSWorkLoopEvent;
struct _DALSYSWorkLoopEvent
{
   DALSYSWorkLoopHandle hWorkLoop;
   DALSYSWorkLoopExecute pWorkLoopFunc;
   void *pArg;
   DALSYSSyncHandle hSync;
   int dwState;
   _DALSYSWorkLoopEvent *nextEvent;
};

typedef struct _DALSYSSignalEventObj
{
   qurt_anysignal_t    anySignal;
   qurt_mutex_t        linkMutex;
   qurt_anysignal_t    *pLinkSignal;
}
_DALSYSSignalEventObj;

typedef struct _DALSYSCallBackEventObj
{
   DALSYSCallbackFunc    cbFunc;
   DALSYSCallbackFuncCtx cbFuncCtx;
}
_DALSYSCallBackEventObj;

typedef struct _DALSYSEventObj
{
   INHERIT_DALSYS_BASE_OBJ();
   int      threadId;
   void *   pTimerObj;
   union 
   {
      _DALSYSSignalEventObj   sig;
      _DALSYSCallBackEventObj cb;
      _DALSYSWorkLoopEvent    wl;
   } p;
} _DALSYSEventObj;

typedef struct _DALSYSWorkLoopObj _DALSYSWorkLoopObj;
struct _DALSYSWorkLoopObj
{
   INHERIT_DALSYS_BASE_OBJ();
   int                  ownerThreadId;
   void                 *ownerRetAddr;
   DALSYSSyncHandle     hSync;
   qurt_anysignal_t     signal;
   void                 *hThread;
   void                 *pStackBuffer;
   _DALSYSWorkLoopEvent *hWorkLoopEventHead;
   _DALSYSWorkLoopEvent *hWorkLoopEventTail;
};

/**
* @brief Cleanup and free the DALSYS Event Obj -- this is NOT 
*        used by clients but an internal API invoked by
*        DestroyObject.

  @param: hEvent Event Handle
*
* @return DAL_SUCCESS on success, DAL_ERROR otherwise 
*/
DALResult
DALSYS_DestroyEventObject(DALSYSEventHandle hEvent);

/**
* @brief Cleanup and free the timer object within 
*        DALSYS Event Obj -- this is NOT 
*        used by clients but an internal API invoked by
*        DestroyObject.
* @param hEvent Event Handle
*/
void DALSYS_DestroyEventTimerObject(DALSYSEventHandle hEvent);

/**
* @brief Cleanup and free the DALSYS Sync Obj -- this is NOT 
*        used by clients but an internal API invoked by
*        DestroyObject.

  @param: hSync Sync Handle
*
* @return DAL_SUCCESS on success, DAL_ERROR otherwise 
*/

DALResult
DALSYS_DestroySyncObject(DALSYSSyncHandle hSync);

/**
* @brief Cleanup and free the DALSYS MemObj, also unmap any 
*        memory mapped by this MemObj. This is NOT used by
*        clients but an internal API invoked by DestroyObject.

  @param: hMem Memory Obj Handle
*
* @return DAL_SUCCESS on success, DAL_ERROR otherwise 
*/

DALResult
DALSYS_DestroyMemObject(DALSYSMemHandle hMem);

/**
* @brief Cleanup and free the DALSYS WorkLoop Obj, deletes the 
*        thread that was create by RegisterWorkloop API.
*        -- this is NOT used by clients but an internal API
*        invoked by DestroyObject.

  @param: hWorkLoop Workloop Handle
*
* @return DAL_SUCCESS on success, DAL_ERROR otherwise 
*/

DALResult
DALSYS_DestroyWorkLoopObject(DALSYSWorkLoopHandle hWorkLoop);

/**
* @brief Initialize the DALSYSEvent to make it usable by the 
*        current thread. In L4, since only the thread that
*        creates the event can wait on it, we use this API to
*        grant ownership of this event to the current thread

  @param: hEvent Event Handle
*
* @return DAL_SUCCESS on success, DAL_ERROR otherwise 
*/
DALResult
DALSYS_EventInit(DALSYSEventHandle hEvent);

/**
  @brief Get the pointer to the "current" DAL Environment

  User(s)/client(s) of DAL drivers must use this API to get a pointer to the
  "current" DAL operating environment.

  @param: None
*
* @return Pointer to the current DAL Environment
*/
DALEnvInfo * DALSYSCMN_GetEnv(void );

/*
* Deprecated functions -- IGNORE.
*/

DALSYSMemHandle
DALSYSCMN_MemObjCopy(DALSYSMemObj * pInObj,DALSYSMemObj * pOutObj);

#endif

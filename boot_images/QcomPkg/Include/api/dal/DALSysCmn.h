/*==============================================================================
  Copyright (c) 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

==============================================================================*/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/19/14   sho    Remove references to the DALEnvInfo and use DALSYSConfig 
                   for device configurations
 08/15/14   aa     Added Function declarations for new API to support String 
                   based devices
 08/12/14   aa     Added history 

=============================================================================*/
#ifndef DALSYSCMN_H
#define DALSYSCMN_H
#include "DALStdDef.h"
#include "DALSys.h"
#include "DalDevice.h"

#define DALSYSCMN_MAX_NUM_MODS_PER_PROCESS 32

#define DALSYSCMN_HANDLE_SET_COPY( handle ) ( (void *)((DALSYSMemAddr)handle | 0x00000001) )
#define DALSYSCMN_HANDLE_IS_COPY( handle ) ( (DALSYSMemAddr)handle & 0x00000001 )
#define DALSYSCMN_HANDLE_TO_OBJECT( handle ) ( (DALSYSMemAddr)handle & ~0x00000001 )

#define DALSYSCMN_MAX_NUM_SHARED_MEM_REGIONS  128


#define DALSYSCMN_OBJECT_ID_FIELD_MASK       0x0000000F
#define DALSYSCMN_OBJECT_ATTRIB_FIELD_MASK   0x000FFFF0
#define DALSYSCMN_DYNAMIC_ALLOC_FIELD_MASK   0x80000000

#define DALSYSCMN_DESTROY_OBJECT(dwObjInfo)\
   (dwObjInfo = 0xDEAD0000)

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
   (dwObjInfo |= dwAttrib)

#define DALSYSCMN_RESET_ATTRIB(dwObjInfo,dwAttrib)\
   (dwObjInfo &= ~dwAttrib)

#define DALSYSCMN_IS_ATTRIB_SET(dwObjInfo,dwAttrib)\
   ((dwObjInfo&dwAttrib) != 0)

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

/*------------------------------------------------------------------------------
Work Loop Event
------------------------------------------------------------------------------*/
typedef struct _DALSYSWorkLoopObj _DALSYSWorkLoopObj;
typedef struct _DALSYSWorkLoopEvent _DALSYSWorkLoopEvent;
struct _DALSYSWorkLoopEvent
{
   _DALSYSWorkLoopObj *pWorkLoopObj;
   DALSYSWorkLoopExecute pWorkLoopFunc;
   void *pArg;
   DALSYSEventHandle hEvent;
   DALSYSSyncHandle hSync;
   _DALSYSWorkLoopEvent * nextEvent;
   int dwState;
};

/*------------------------------------------------------------------------------
Work Loop states.
------------------------------------------------------------------------------*/
#define DALSYS_WORKLOOP_BUSY  0xFFFFFFFF
#define DALSYS_WORKLOOP_READY 0x00

/*------------------------------------------------------------------------------
Work Loop event states.
------------------------------------------------------------------------------*/
#define WL_EVENT_DEQUEUED 0
#define WL_EVENT_ENQUEUED 1

/*------------------------------------------------------------------------------
Work Loop system object.
User events are queued to the hWorkLoop
------------------------------------------------------------------------------*/
struct _DALSYSWorkLoopObj
{
   INHERIT_DALSYS_BASE_OBJ();
   DALSYSEventHandle hSysEvent;
   DALSYSSyncHandle hSync;
   _DALSYSWorkLoopEvent *hWorkLoopEventHead;
   _DALSYSWorkLoopEvent *hWorkLoopEventTail;
   volatile uint32 dwPendingEvents;
   uint32 hThread;
};

typedef struct DALPropsDir DALPropsDir;
struct DALPropsDir
{
   uint32 dwLen;
   uint32 dwPropsNameSectionOffset;
   uint32 dwPropsStringSectionOffset;
   uint32 dwPropsByteSectionOffset;
   uint32 dwPropsUint32SectionOffset;
   uint32 dwNumDevices;
   uint32 dwPropsDeviceOffset[1][2]; // structure like this will follow...
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
* @bried Initialize and REgister the module with DALSYS. 
*        Exchange function pointers for subsequent DALSYS usage


* @param: pModDriverList Driver list of the invoking module 
*         pDALModStringDevDrvInfoList Driver list for String based devices 
*         pFncTblPtr output parameter populated with the DALSYS
*         function pointer table
*
* @return void
*/
void      
_DALSYS_Init(DALREG_DriverInfoList *pModDriverList, 
      DALProps *pDALModStringDevDrvInfoList, DALSYSFncPtrTbl **pFncTblPtr);

/**
* @bried Register the Timer/System Device handle with DALSYS. 
*        Used to enforce timeouts for DALSYS_EventMultipleWait


* @param: hDalDevice System/Timer device handle
*  
* @return void
*/
void     
_DALSYS_InitSystemHandle(DalDeviceHandle * hDalDevice);

/**
* @bried DeInitialize the module with DALSYS. After this, 
*        drivers that existed in the module will no longer be
*        accessible.


* @param: pModDriverList Driver list of the invoking module 
*
* @return void
*/
void     
 _DALSYS_DeInit(DALREG_DriverInfoList *pModDriverList, 
                DALProps *pDALModStringDevDrvInfoList);

/*
  @brief Destructor for DALSYS objects

  Users must invoke this API to clean-up/destroy "ANY" DALSYS object.

  @param hObj  : Any DALSYS Object Handle

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise

  Use of this function is not recommeded for WorkLoop destruction. Instead
  use an event to instruct the workloop to terminate.
*/
DALResult 
_DALSYS_DestroyObject(DALSYSObjHandle hObj);


DALResult 
_DALSYS_CopyObject(DALSYSObjHandle, DALSYSObjHandle *);
/*
  @brief Register a Work Loop Function (to run code in new execution context)

  Users must invoke this API to register a work loop function (executed in the
  context of a new thread).

  @param dwPriority        : Priority of this execution context
         dwMaxNumEvents    : Maximum number of events that can be added
         phWorkLoop        : Return parameter to this Work Loop Handle
         pWorkLoopObj      : Pointer to Work Loop Object (if statically alloced)

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_RegisterWorkLoop(uint32 dwPriority,uint32 dwMaxNumEvents,
                                   DALSYSWorkLoopHandle *phWorkLoop,
                                   DALSYSWorkLoopObj *pWorkLoopObj);
/*
  @brief Add an event to the Work Loop

  Users must invoke this API to add events to the work loop.

  @param hWorkLoop         : Handle to the registered workloop
         pfnWorkLoopExecute: Function to be invoked when the associated event
                             is triggered.
         pArg              : Argument to be passed to the work loop function
         hEvent            : Handle to desired add event
         hSync             : Handle to sychronization object to be associated
                             with this event

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_AddEventToWorkLoop(DALSYSWorkLoopHandle hWorkLoop,
                                     DALSYSWorkLoopExecute pfnWorkLoopExecute,
                                     void *pArg, DALSYSEventHandle hEvent, DALSYSSyncHandle hSync);
/*
  @brief Delete an event from the Work Loop

  Users must invoke this API to delete events to the work loop.

  @param hWorkLoop         : Handle to the registered workloop
         hEvent            : Handle to delete event

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_DeleteEventFromWorkLoop(DALSYSWorkLoopHandle hWorkLoop,DALSYSEventHandle hEvent);

/*
  @brief Construct a DALSYS Event Object

  Users must invoke this API to construct Event Object. Users have the option
  of pre-allocating memory required for this object, or rely upon dynamic
  memory allocation.

  @param dwAttribs:  Event Attribs
         phEvent   : Return parameter, upon successful invocation, points to
                     the DALSYS Event Handle
         pEventObj : Pointer to the Event Object if user doesnt desire
                     dynamic memory allocation for the Event Object.
                     NULL otherwise

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_EventCreate(uint32 dwAttribs, DALSYSEventHandle * phEvent, DALSYSEventObj *pEventObj);

/*
  @brief Make a DALSYS Event Object copy from an existing event

  Users must invoke this API to copy an Event Object. Users have the option
  of pre-allocating memory required for this object, or rely upon dynamic
  memory allocation.

  @param hEvent        : Handle to the original event
         phEventCopy   : Return parameter, upon successful invocation, points to
                         the copied DALSYS Event Handle
         pEventObjCopy : Pointer to the copied Event Object if user doesnt desire
                         dynamic memory allocation for the copied Event Object.
                         NULL otherwise
         dwMarshalFlags: Flags for marshalling -- TBD

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_EventCopy(DALSYSEventHandle hEvent, DALSYSEventHandle *phEventCopy,
                            DALSYSEventObj *pEventObjCopy,uint32 dwMarshalFlags);
/*
  @brief DALSYS Event control API

  This API can be used to pass control commands to events. Users can
  Trigger events by passing in the desired control command

  @param hEvent       : DALSYS Event Handle
         dwCtrl       : DALSYS Event control command (see common defs)
         dwParam      : parameter to be passed to callback
         pPayload     : payload message
         dwPayloadSize: payload size

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_EventCtrl(DALSYSEventHandle hEvent, uint32 dwCtrl,uint32 dwParam, 
                            void* pPayload,uint32 dwPayloadSize);
/*
  @brief Wait on a DALSYS Event

  @param hEvent      : DALSYS Event Handle

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult _DALSYS_EventWait(DALSYSEventHandle hEvent);

/*
  @brief Wait on a Multiple DALSYS Events

  @param phEvent     : DALSYS Event Handle Array
         nEvents     : Length of the Event Handle Array
         dwTimeoutUs : Timeout value in us
         pdwEventIdx : Id of the event that was trigger, return parameter,
                       Filled by calle, upon successful completion

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_EventMultipleWait(DALSYSEventHandle* phEvent, int nEvents,
                                    uint32 dwTimeoutUs, uint32 *pdwEventIdx);
/*
  @brief Setup an Event to call a callback when triggered

  Alternate method for Events where a callback in the same context is
  desired. This mechanism is more appropriate in single threaded
  environemnts as well as when stacking drivers which certain 3POS
  OS constructs such as NDIS (network drivers)

  @param hEvent    : Event handle
         cbFunc    : Callback function to call.
         cbFuncCtx : User context pointer to be supplied to cbFunc as param

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_SetupCallbackEvent(DALSYSEventHandle hEvent, DALSYSCallbackFunc cbFunc,
                                     DALSYSCallbackFuncCtx cbFuncCtx);
/*
  @brief Construct a DALSYS Sync Object

  Users must invoke this API to construct a Syncronization Object.
  Users have the option of pre-allocating memory required for this object,
  or rely upon dynamic memory allocation.

  Sync object usage must follow common mutex deadlock prevention guidelines
  for multi-threaded environments.

  An execution context may acquire ownership of a Sync object more than once
  without releasing ownership first; but for the ownership to be completely
  released the user must release ownership the same number of times.
  i.e Each DALSYS_SyncEnter(obj) should be matched by a DALSYS_SyncLeave(obj)

  @param dwAttribs : Desired type of Sync object :
                        -Resource Synchronization
                        -Resource Interrupt Synchronization
                           Between IST priority levels and normal threads
                        -Resource IRQ Synchronization ( Not implemented yet)
                           Between IRQ Handler and IST
                           Note: the bottom 12-bits must be the interrupt-ID
                        -Disable pre-emption
                        -Disable (hard) all pre-emptions
         phSync    : Return parameter, upon successful invocation, points to
                     the DALSYS Sync Handle
         pSyncObj  : Pointer to the Sync Object if user doesnt desire
                     dynamic memory allocation for the Sync Object.
                     NULL otherwise

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_SyncCreate(uint32 dwAttribs,DALSYSSyncHandle *phSync,DALSYSSyncObj *pSyncObj);

/*
  @brief Enter a DALSYS Sync section

  A User calls this API in order to get ownership of this synchronization.

  @param hSync : DALSYS Sync Handle

  @see DALYS_SyncLeave

  @return void
*/
void      
_DALSYS_SyncEnter(DALSYSSyncHandle hSync);

/*
  @brief Leave a DALSYS Sync section

  Users must invoke this API if they wish to relinquish ownership of this
  synchronization.

  @param hSync : DALSYS Sync Handle

  @see DALYS_SyncEnter

  @return void
*/
void      
_DALSYS_SyncLeave(DALSYSSyncHandle hSync);
/*
  @brief Construct a DALSYS Mem Object

  Users must invoke this API to construct a Mem Object. Users have the option
  of pre-allocating memory required for this object, or rely upon dynamic
  memory allocation.

  This DALSys Mem object accomplishes virtual memory allocation,
  physical memory allocation and HWIO memory mapping.

  All lengths and offsets must be aligned to the architectures natural
  page boundaries.

  - Use DALSYS_MEM_PROPS_MAP if it is desired that physical memory be
  allocated at this time.
  - When specifying DALSYS_MEM_PROPS_HWIO, this function requires that
  the physical address is specified and this API will map it
  (i.e. DALSYS_MEM_PROPS_MAP is ignored when used with DALSYS_MEM_PROPS_HWIO).

  These memory regions may be used in conjunction with DDI IO Mem Regions.

  @param dwAttribs   : Memory Attributes i.e Cached | UnCached etc..
                       (see common defs)
         VirtualAddr : Desired Virtual Addr, if no preference use
                       DALSYS_MEM_ADDR_NOT_SPECIFIED

         PhysicalAddr: Desired Physical Addr, if no preference use
                       DALSYS_MEM_ADDR_NOT_SPECIFIED

         dwLen       : Size of virtual memory
         phMem       : Return parameter, upon successful invocation, points to
                       the DALSYS Mem Handle

         pMemObj    :  Pointer to the Memory Object if user doesnt desire
                     dynamic memory allocation for this Object.
                     NULL otherwise

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_MemRegionAlloc(uint32 dwAttribs, DALSYSMemAddr VirtualAddr, DALSYSMemAddr PhysicalAddr, 
                                 uint32 dwLen, DALSYSMemHandle *phMem,DALSYSMemObj *pMemObj);
/*
  @brief Map/UnMap Physical Memory address to a Virtual Memory address

  Users must invoke this API in order to Map/UnMap a physical address to a
  virtual address for this process. To UnMap the previous VM range use
  DALSYS_MEM_INVALID_ADDR as the Physical Address parameter

  @param hMem           : DALSYS Mem Handle
         virtBaseOffset : Address offset from virtual base address of this region.
         PhysicalAddr   : Physical Address that needs to be mapped
         dwLen          : Length of the mem region

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_MemRegionMapPhys(DALSYSMemHandle hMem, uint32 virtBaseOffset,
                                   DALSYSMemAddr PhysicalAddr, uint32 dwLen);
/*
  @brief Return Information regarding the Memory Section

  @param hMem         : DALSYS Mem Handle
         pMemInfo     : Pointer to Mem Info structure, filled up by calle,
                        Valid only if return code is DAL_SUCCESS
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_MemInfo(DALSYSMemHandle hMem, DALSYSMemInfo *pMemInfo);

/*
  @brief Useful to Invalidate/flush/clean the cache

  @param CacheCmd     : Invalidate, Flush or clean ( see cache macro defs)
         VirtualAddr  : Virtual Address
         dwLen        : Length of the mem region

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_CacheCommand(uint32 CacheCmd, DALSYSMemAddr VirtualAddr, uint32 dwLen);
/*
  @brief Allocate memory from local heap

  This is similar to Malloc from the C library, but this will allocate memory
  within the DAL Environment Context. Memory alloced through this call should be
  freed by calling DALSYS_Free

  @param dwSize   : Requested size of memory to be alloced
         ppMem    : Return parameter, calle will populate this to point to the
                    block of alloced memory, valid only upon DAL_SUCCESS return
                    code
  @see DALSYS_Free

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_Malloc(uint32 dwSize, void **ppMem);

/*
  @brief Free allocated memory from local heap

  This is similar to free from the C library, but this will free memory alloced
  within the DAL Environment Context.

  @param pMem     : Pointer to block of memory alloced through DALSYS_Malloc

  @see DALSYS_Malloc
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_Free(void *pMem);

/*
  @brief Pause the execution of the calling thread for a specified amount of time
  
  Pause the execution of the calling thread for a specified amount of time.
  Intended for microsecond range delays internally implemented by looping.

  @param pause_time_us : Minimum amount of time in microseconds to delay/pause

  @return void
*/
void      
_DALSYS_BusyWait(uint32 pause_time_us);

/**
  @brief Get the DAL Properties handle

  User(s)/client(s) of DAL drivers must use this API to get the DAL Properties
  Handle

  @param DeviceId   : Desired Device Id
       phDALProps : Pointer to DALPropertyHandle, only valid if return code is
                 DAL_SUCCESS
*
* @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId, DALSYSPropertyHandle phDALProps);

/**
  @brief Get the DAL Properties handle

  User(s)/client(s) of DAL drivers must use this API to get the DAL Properties
  Handle for string based devices

  @param pszDevName   : Device Name
         phDALProps   : Pointer to DALPropertyHandle, only valid if return code is
                        DAL_SUCCESS
*
* @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_GetDALPropertyHandleStr(const char *pszDevName, DALSYSPropertyHandle hDALProps);

/*
  @brief Attach to a LOCAL DAL Driver. This function will not attempt to remote attach calls to 
         remote core or processor

  @param pszArg        : string argument to be passed to the driver instance creation function
         DeviceId      : Desired Device Id
         phDalDevice       : Output parameter that contains the device handle

  @return  DAL_SUCCESS on successful completion
           DAL_ERROR_DEVICE_NOT_FOUND if handle cannot be create locally
                        
*/
DALResult 
_DAL_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,DalDeviceHandle **phDalDevice);

/*
  @brief Attach to a DAL Driver. This function will attempt to remote attach calls to 
         remote core or processor if desired device id cannot be created locally. In 
         case of remote attach, this functon performs version checking between clients and servers

  @param pszArg        : string argument to be passed to the driver instance creation function
         ClientVersion : Client API revision #, use macro defined in the driver DDIxx.h interface file
         DeviceId      : Desired Device Id
         phDalDevice   : Output parameter that contains the device handle

  @return  DAL_SUCCESS on successful completion
           DAL_ERROR_PORT_CONN_BROKEN if remote connection timed out 
           DAL_ERROR if handle cannot be created
                        
*/
DALResult 
_DAL_DeviceAttachEx(const char *pszArg, DALDEVICEID DevId, DALInterfaceVersion ClientVersion,
                              DalDeviceHandle **phDalDevice);

/*
  @brief Attach Remotely to a DAL Driver, NO LOCAL SEARCHES. This function will attempt to remote attach calls to 
         remote core or processorthis functon performs version checking between clients and servers

  @param pszArg        : string argument to be passed to the driver instance creation function
         ClientVersion : Client API revision #, use macro defined in the driver DDIxx.h interface file
         DeviceId      : Desired Device Id
         phDalDevice   : Output parameter that contains the device handle

  @return  DAL_SUCCESS on successful completion
           DAL_ERROR_PORT_CONN_BROKEN if remote connection timed out
           DAL_ERROR if handle cannot be created
                        
*/
DALResult 
_DAL_DeviceAttachRemote(const char *pszArg, DALDEVICEID DevId, DALInterfaceVersion ClientVersion,
                                  DalDeviceHandle **phDalDevice);

/*
  @brief DeTach/Close a DAL Driver. 

  @param hDalDevice       : Output parameter that contains the device handle

  @return  DAL_SUCCESS on successful completion
           DAL_ERROR on error                        
*/
DALResult 
_DAL_DeviceDetach(DalDeviceHandle *hDalDevice);

/*
  @brief Get the Property Value. User must pass a ptr to the PropVariable.
  The User must also initialize the name field, this API will populate the
  name and value. The value is a "union" and must be used depending upon
  the appropriate type.

  @param hDALProps   : DAL Propery Handle
       pszName     : Prop Name ( if searching for uint32 name, this MUST be
                                 set to NULL)
        dwId        : Prop Id   ( integer prop name)
        pDALProp    : Ptr to the Property variable

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult 
_DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps, const char * pszName,
                                   uint32 dwId,DALSYSPropertyVar *pDALProp);

/*
* @brief Do string device object lookup for object reference

* @param pszDevName       : Desired Device Name.
         ppStringDeviceObj: DeviceObject pointer.
         DALProps         : DalProperty pointer.
*
* @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/ 
DALResult
_DAL_StringDeviceObjectLookup(const char *pszDevName, const StringDevice **ppStringDeviceObj, 
                             DALProps *pDALPtr);
/*
  @brief Loging and error reporting

  @param DeviceId      : Desired Device Id
         dwLogEventType: See above defines for log event type
         pszMsg        : Log message pointer

  @return DALSYS_LOGEVENT_FATAL_ERROR will not return
*/
void      
_DALSYS_LogEvent(DALDEVICEID DeviceId, uint32 dwLogEventType, const char *pszMsg);

/*
* @brief Get the DALConfig Property Dir -- internal API used by 
*        DALPROPS.c 

* @param: pDALPropsDir output parameter gets populate with 
*       property directory ptr
*
* @return void
*/
void
DALSYS_GetPropDir(DALPropsDir ** pDALPropsDir);

/*
* @brief Get the DALConfig Property Structure

* @param: void
*
* @return DAL Prop Structure ptr
*/
DALProps *
DALSYS_GetDAL_Prop(void);

/*
* @brief Get string device object for string device

* @param pszDevName       : Desired Device Name.
         pszArg           : string argument to be passed to the driver
         hDalDevice       : Output parameter that contains the device handle
*
* @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/ 
DALResult
_DALSYSCMN_GetStringDeviceObject(const char *pszDevName, 
                                 const char *pszArg, 
                                 DalDeviceHandle **phDevice);

/*
* @brief Get the DALConfig Property Structure array -- internal 
*        API used by DALPROPS.c

* @param: void
*
* @return DAL Prop Structure ptr
*/

const void ** 
DALSYS_GetPropStructPtrArray(void);

/*
* @brief Get the Target ID and Process Architecture type from 
*        DAL Props

* @param: pdwTargetId REturn parameter will be set to the Target 
*       id ( Hex value e.g. 0x763
*        pdwProcArch set to the arm arch ver number
* @return DAL Prop Structure ptr
*/

void 
DALSYSCMN_GetTargetInfo(uint32 * pdwTargetId, uint32 *pdwProcArch);

/*
* @brief Register the DAL Driver list with framework 

* @param: pModDriverList : Pointer to a driver list data structure.
*                          
* @return Return Code, DAL_SUCCESS on successful completion, 
          error code otherwise
*/
DALResult
DALSYSCMN_RegisterModDriverList(DALREG_DriverInfoList *pModDriverList);

/*
* @brief DeRegister DAL Driver list with framework 

* @param: pModDriverList : Pointer to a driver list data structure.
*                          
* @return Return Code, DAL_SUCCESS on successful completion, 
          error code otherwise
*/
DALResult
DALSYSCMN_DeRegisterModDriverList(DALREG_DriverInfoList *pModDriverList);

/*
* @brief Register the DAL Driver list with framework 

* @param: pDALModStringDevDrvInfoList : Pointer to a driver list 
          data structure.
*                          
* @return Return Code, DAL_SUCCESS on successful completion, 
          error code otherwise
*/
DALResult
DALSYSCMN_RegisterStrModDriverList(DALProps *pDALModStringDevDrvInfoList);

/*
* @brief DeRegister DAL Driver list with framework 

* @param: pDALModStringDevDrvInfoList : Pointer to a driver list 
          data structure.
*                          
* @return Return Code, DAL_SUCCESS on successful completion, 
          error code otherwise
*/
DALResult
DALSYSCMN_DeRegisterStrModDriverList(DALProps *pDALModStringDevDrvInfoList);

/*
  @brief workloop execution function.

  @param: pWorkLoopObj Pointer to the workloop object
*
* @return Return Code, DAL_SUCCESS on successful completion, 
*         error code otherwise 
*/
DALResult 
_WorkLoopExecute(void *pArg);

/*
* Deprecated functions -- IGNORE.
*/
DALSYSMemHandle
DALSYSCMN_MemObjCopy(DALSYSMemObj * pInObj,DALSYSMemObj * pOutObj);

DALResult
DALSYSCMN_ShareMemoryRegion(DALSYSMemObjBase *pMemObj);

DALSYSMemObjBase*
DALSYSCMN_GetMemRegionPtr(uint32 hProc, DALSYSMemAddr VirtualAddrStart);

#endif

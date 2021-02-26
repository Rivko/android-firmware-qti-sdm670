#ifndef DALSYS_H
#define DALSYS_H
/*==============================================================================
@file DALSys.h

                  DAL System lib API

        Copyright (c) 2008,2010 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/api/dal/DALSys.h#2 $
=============================================================================*/

/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "devcfg.h"
#include "DALSysTypes.h"
#include "string.h"

/*------------------------------------------------------------------------------
Defines for DALSYS Config
------------------------------------------------------------------------------*/
#define DALSYS_ABORT_ON_FATAL_ERROR          0x1

/*------------------------------------------------------------------------------
* Defines for DALSYS Event attributes
* NOTE: TIMEOUT and TIMER are mutually exclusive
        NORMAL  and WORKLOOP are mutually exclusive.

   In order for software to be common with all OSes, please observe the
   following usage rules. These are also described in DeviceAccessLibrary.doc.
   ----------------------------------------------------------------------
   - DALSYS_EVENT_ATTR_WORKLOOP_EVENT events can only be assigned to a
     workloop.

   - DALSYS_EventMultipleWait can only be used with 2 events one being
     DALSYS_EVENT_ATTR_TIMEOUT_EVENT

   - An event object must only be "Waited on" by only one thread at a time.

   - When an event is allocated to be used for Waiting by any arbitrary thread,
     then this event must be of type: DALSYS_EVENT_ATTR_CLIENT_DEFAULT.
     In the thread context which will call "Wait" the thread must first call
     DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET); this will associate
     or claim the event object with running thread and reset the event state if
     signaled. External synchronization may be necessary so that only one
     thread claims and waits on the event object. E.g.
         Synchronize - Begin
         ....
         DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET)
         ....
         Schedule Some request and wait for event
         ....
         DALSYS_EventWait(hEvent)
         ....
         Synchronize - End

   - The DALSYS_EVENT_ATTR_NORMAL events must be allocated and waited on by
     the same thread context.

   ----------------------------------------------------------------------

   The DALSYS_EVENT_ATTR_NORMAL has applicability when a DAL (common software)
   interoperates with some specific environments such as L4 & WM. As an
   example a DAL (which is common software) can trigger any event type, but
   the allocation of the event(s) was accomplished within native or not-common
   software. Then within this not-common native software DALSYS_EventMultipleWait
   may be used even with more than 2 objects, if the native OS environment supports
   this construct. This is the case with WM and L4 platforms but NOT Symbian.

*-----------------------------------------------------------------------------*/
#define DALSYS_EVENT_ATTR_NORMAL            0x00000000
#define DALSYS_EVENT_ATTR_WORKLOOP_EVENT    0x00000010
#define DALSYS_EVENT_ATTR_CALLBACK_EVENT    0x00000020
#define DALSYS_EVENT_ATTR_TIMEOUT_EVENT     0x00000040
#define DALSYS_EVENT_ATTR_TIMER_EVENT       0x00000080
#define DALSYS_EVENT_ATTR_CLIENT_DEFAULT    0x00000100
#define DALSYS_EVENT_ATTR_NON_DEFERRED      0x00000200

/*------------------------------------------------------------------------------
* Defines for DALSYS Event control
*-----------------------------------------------------------------------------*/
#define DALSYS_EVENT_CTRL_TRIGGER            0x00000100
#define DALSYS_EVENT_CTRL_RESET              0x00000200
#define DALSYS_EVENT_CTRL_ACCQUIRE_OWNERSHIP 0x00000400
#define DALSYS_EVENT_TIMEOUT_INFINITE        0xFFFFFFFF


/*------------------------------------------------------------------------------
* Define for DALSYS Sync object types
*-----------------------------------------------------------------------------*/
#define DALSYS_SYNC_ATTR_RESOURCE           0x00000010
#define DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT 0x00000020
#define DALSYS_SYNC_ATTR_IRQLEVEL           0x80000000
#define DALSYS_SYNC_ATTR_NO_PREEMPTION      0x00000040
#define DALSYS_SYNC_ATTR_HARD_NO_PREEMPTION 0x00000080

/*------------------------------------------------------------------------------
* Defines for DALSYS Mem props
*-----------------------------------------------------------------------------*/
#define DALSYS_MEM_INVALID_ADDR             0xFFFFFFFF
#define DALSYS_MEM_ADDR_NOT_SPECIFIED       0xFFFFFFFF

#define DALSYS_MEM_PROPS_ARCH_MASK_SHIFT    4
#define DALSYS_MEM_PROPS_NORMAL             0x00000000
#define DALSYS_MEM_PROPS_ARCH_SPECIFIC      0x00100000
#define DALSYS_MEM_PROPS_ARCH_MASK          0x000FFFF0
#define DALSYS_MEM_PROPS_PHYS_CONT          0x00200000
#define DALSYS_MEM_PROPS_PHYS_PREALLOC      0x00400000
#define DALSYS_MEM_PROPS_NO_PHYS_COMMIT     0x00800000
#define DALSYS_MEM_PROPS_SHARE              0x10000000

#define DALSYS_MEM_PROPS_EXT                          0x00080000
#define DALSYS_MEM_PROPS_EXT_MASK                     0x0007FFF0
#define DALSYS_MEM_PROPS_EXT_ATTRIBS_MASK             0x000001F0

#define DALSYS_MEM_PROPS_EXT_STRONGLY_ORDERED         0x00000000
#define DALSYS_MEM_PROPS_EXT_SHARED_DEVICE            0x00000010
#define DALSYS_MEM_PROPS_EXT_OUTER_INNER_WT_NO_ALLOC  0x00000020
#define DALSYS_MEM_PROPS_EXT_OUTER_INNER_WB_NO_ALLOC  0x00000030
#define DALSYS_MEM_PROPS_EXT_OUTER_INNER_UNCACHED     0x00000040
#define DALSYS_MEM_PROPS_EXT_OUTER_INNER_WB_ALLOC     0x00000070
#define DALSYS_MEM_PROPS_EXT_NON_SHARED_DEVICE        0x00000080

#define DALSYS_MEM_PROPS_EXT_NORM_MEM                 0x00000100

#define DALSYS_MEM_PROPS_EXT_OUTER_MEM_NONCACHED      0x00000000
#define DALSYS_MEM_PROPS_EXT_OUTER_MEM_WB_ALLOC       0x00000040
#define DALSYS_MEM_PROPS_EXT_OUTER_MEM_WT_NO_ALLOC    0x00000080
#define DALSYS_MEM_PROPS_EXT_OUTER_MEM_WB_NO_ALLOC    0x000000C0

#define DALSYS_MEM_PROPS_EXT_INNER_MEM_NONCACHED      0x00000000
#define DALSYS_MEM_PROPS_EXT_INNER_MEM_WB_ALLOC       0x00000010
#define DALSYS_MEM_PROPS_EXT_INNER_MEM_WT_NO_ALLOC    0x00000020
#define DALSYS_MEM_PROPS_EXT_INNER_MEM_WB_NO_ALLOC    0x00000030


#define DALSYS_MEM_PROPS_ARCH_HW_NORM_MEM_SHARE      0x00000200

#define DALSYS_MEM_PROPS_EXT_PERM_NONE                0x00000000
#define DALSYS_MEM_PROPS_EXT_PERM_RD                  0x00000800
#define DALSYS_MEM_PROPS_EXT_PERM_FULL                0x00000C00


#define DALSYS_MEM_PROPS_CACHED_MASK                 (DALSYS_MEM_PROPS_EXT|DALSYS_MEM_PROPS_EXT_MASK)
#define DALSYS_MEM_PROPS_HWIO                        (DALSYS_MEM_PROPS_EXT|DALSYS_MEM_PROPS_EXT_NON_SHARED_DEVICE|DALSYS_MEM_PROPS_EXT_PERM_FULL)
#define DALSYS_MEM_PROPS_UNCACHED                    (DALSYS_MEM_PROPS_EXT|DALSYS_MEM_PROPS_EXT_OUTER_INNER_UNCACHED|DALSYS_MEM_PROPS_EXT_PERM_FULL)
#define DALSYS_MEM_PROPS_CACHED_WB                   (DALSYS_MEM_PROPS_EXT|DALSYS_MEM_PROPS_EXT_OUTER_INNER_WB_NO_ALLOC|DALSYS_MEM_PROPS_EXT_PERM_FULL)
#define DALSYS_MEM_PROPS_CACHED_WT                   (DALSYS_MEM_PROPS_EXT|DALSYS_MEM_PROPS_EXT_OUTER_INNER_WT_NO_ALLOC|DALSYS_MEM_PROPS_EXT_PERM_FULL)


#define DALSYS_MEM_PROPS_EXT_CB_MASK_SHIFT                  4
#define DALSYS_MEM_PROPS_EXT_TEX_MASK_SHIFT                 6
#define DALSYS_MEM_PROPS_EXT_NORM_MEM_SHARE_MASK_SHIFT      9
#define DALSYS_MEM_PROPS_EXT_PERM_MASK_SHIFT                10

#define DALSYS_MEM_PROPS_EXT_CB_MASK                  0x00000030
#define DALSYS_MEM_PROPS_EXT_TEX_MASK                 0x000001C0
#define DALSYS_MEM_PROPS_EXT_NORM_MEM_SHARE_MASK      0x00000200
#define DALSYS_MEM_PROPS_EXT_PERM_MASK                0x00000C00


/*------------------------------------------------------------------------------
Can be passed to validate/prepare in order to operate on all bufs in the desc
------------------------------------------------------------------------------*/
#define DALSYS_MEM_DESC_OPERATE_ON_ALL_BUFS 0xFFFFFFFF

/*------------------------------------------------------------------------------
* Defines for DATA Cache commands
*-----------------------------------------------------------------------------*/
#define DALSYS_CACHE_CMD_INVALIDATE         0x00000001
#define DALSYS_CACHE_CMD_CLEAN              0x00000002
#define DALSYS_CACHE_CMD_FLUSH              0x00000003

/*------------------------------------------------------------------------------
* Defines for EVENT logging types
*-----------------------------------------------------------------------------*/
#define DALSYS_LOGEVENT_FATAL_ERROR    1
#define DALSYS_LOGEVENT_ERROR          2
#define DALSYS_LOGEVENT_WARNING        3
#define DALSYS_LOGEVENT_INFO           4

/*------------------------------------------------------------------------------
// Mask for global RDAL configuration
*-----------------------------------------------------------------------------*/
#define DALREMOTING_NONE                     0
#define DALREMOTING_CLIENTS                  1
#define DALREMOTING_SERVERS                  2
#define DALREMOTING_CLIENTS_SERVERS          3

/*------------------------------------------------------------------------------
General Utility macros
------------------------------------------------------------------------------*/
#define DALSYS_memset( pDest, val, size ) \
    ((void)memset((void *)pDest,val,size))

#define DALSYS_memscpy( pDest, iDestSz, pSrc, iSrcSize ) \
   _DALSYS_memscpy((void *)pDest,(uint32)iDestSz,(const void*)pSrc,(uint32)iSrcSize)

// below macro is deprecated and superceded by DALSYS_memscpy
#define DALSYS_memcpy( pDest, pSrc, len ) \
   _DALSYS_memscpy((void *)pDest,(uint32)len,(const void*)pSrc,(uint32)len)

/*------------------------------------------------------------------------------
* Function declaration and documentation
*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/*
  @brief Initialize/Register Module with the DALSYS library

  DAL Modules must invoke this API prior to any DALSYS usage.

  @param pCfg: DALSYS config struct pointer

  @return None
*/
void
DALSYS_InitMod(DALSYSConfig * pCfg);

/*
  @brief De-Initialize/De-Register Module with the DALSYS library

  DAL Modules can use this API once done using DALSYS.

  @param None
  @return None
*/
void
DALSYS_DeInitMod(void);

/*
  @brief Initialize System Handle with the DALSYS library

  @param  hDALDevice: DAL System Device Driver handle
  @return None
*/

void
DALSYS_InitSystemHandle(DalDeviceHandle *hDalDevice);

/*
  @brief Set log severity level

  Previously this function can set user-defined logging function, 
  however this feature has been disabled with the availability of 
  native DALSYS logging.

  @param  dwMaxLogLevel cut-off log msg level, msgs above this wont be logged
          DALSysLogFcn  unused

  @return Returns tbe previous Log Function ptr
*/

DALSYS_LogEventFncPtr
DALSYS_SetLogCfg(uint32 dwMaxLogLevel, DALSYS_LogEventFncPtr DALSysLogFcn);

/*
  @brief Destructor for DALSYS objects

  Users must invoke this API to clean-up/destroy "ANY" DALSYS object.

  @param hObj  : Any DALSYS Object Handle

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise

  Use of this function is not recommeded for WorkLoop destruction. Instead
  use an event to instruct the workloop to terminate.
*/
DALResult
DALSYS_DestroyObject(DALSYSObjHandle hObj);

/*
  @brief Copy function used for remoting DALSYS objects

  Users must invoke this API to create local copy of "ANY" DALSYS object.

  @param hObj  : Any DALSYS Object Handle

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/

DALResult
DALSYS_CopyObject(DALSYSObjHandle hObjOrig, DALSYSObjHandle *phObjCopy );
/*
  @brief Register a Work Loop Function (to run code in new execution context)

  Users must invoke this API to register a work loop function (executed in the
  context of a new thread).

  @param dwPriority      : Priority of this execution context between 1 to 255
                           1 for lowest priority
                           255 for highest priority
                           0 for default priority
       dwMaxNumEvents    : Maximum number of events that can be added
         phWorkLoop      : Return parameter to this Work Loop Handle
       pWorkLoopObj      : Pointer to Work Loop Object (if statically alloced)

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALSYS_RegisterWorkLoop(uint32 dwPriority,
                  uint32 dwMaxNumEvents,
                  DALSYSWorkLoopHandle *phWorkLoop,
                  DALSYSWorkLoopObj *pWorkLoopObj);

/*
  @brief Register a Work Loop Function (to run code in new execution context)

  Users must invoke this API to register a work loop function (executed in the
  context of a new thread).

  @param pszname        : workloop name
         dwStackSize    : workloop stack size
         dwPriority     : Priority of this execution context between 1 to 255
                          1 for lowest priority
                          255 for highest priority
                          0 for default priority
         dwMaxNumEvents : Maximum number of events that can be added 
         phWorkLoop     : Return parameter to this Work Loop Handle
         pWorkLoopObj   : Pointer to Work Loop Object (if statically alloced)

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALSYS_RegisterWorkLoopEx(
                  char * pszname,
                  uint32 dwStackSize,
                  uint32 dwPriority,
                  uint32 dwMaxNumEvents,
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
DALSYS_AddEventToWorkLoop(DALSYSWorkLoopHandle hWorkLoop,
                    DALSYSWorkLoopExecute pfnWorkLoopExecute,
                    void * pArg,
                    DALSYSEventHandle hEvent,
                    DALSYSSyncHandle hSync);

/*
  @brief Delete an event from the Work Loop

  Users must invoke this API to delete events to the work loop.

  @param hWorkLoop         : Handle to the registered workloop
         hEvent            : Handle to delete event

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALSYS_DeleteEventFromWorkLoop(DALSYSWorkLoopHandle hWorkLoop,
                         DALSYSEventHandle hEvent);
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
DALSYS_EventCreate(uint32 dwAttribs, DALSYSEventHandle *phEvent,
               DALSYSEventObj *pEventObj);

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
DALSYS_EventCopy(DALSYSEventHandle hEvent,
             DALSYSEventHandle *phEventCopy,DALSYSEventObj *pEventObjCopy,
                 uint32 dwMarshalFlags);

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
DALSYS_EventCtrlEx(DALSYSEventHandle hEvent, uint32 dwCtrl, uint32 dwParam,
                   void *pPayload, uint32 dwPayloadSize);


/*
  @brief DALSYS Event Ctrl macro to be backward compliant with non-payload
         events

  @param hEvent       : DALSYS Event Handle
         dwCtrl       : DALSYS Event control command (see common defs)

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise

*/
#define DALSYS_EventCtrl(hEvent,dwCtrl) DALSYS_EventCtrlEx(hEvent,dwCtrl,0,NULL,0)

/*
  @brief Wait on a DALSYS Event

  @param hEvent      : DALSYS Event Handle

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALSYS_EventWait(DALSYSEventHandle hEvent);

/*
  @brief Wait on a Multiple DALSYS Events

  @param phEvent     : DALSYS Event Handle Array
         nEvents     : Length of the Event Handle Array
         dwTimeoutUs : Timeout value in us
         pdwEventIdx : Id of the event that was trigger, return parameter,
                       Filled by callee, upon successful completion

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/

DALResult
DALSYS_EventMultipleWait(DALSYSEventHandle* phEvent, int nEvents,
                         uint32 dwTimeoutUs,uint32 *pdwEventIdx);


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
DALSYS_SetupCallbackEvent(DALSYSEventHandle hEvent, DALSYSCallbackFunc cbFunc,
                          DALSYSCallbackFuncCtx cbFuncCtx);

/*
  @brief Start a timer for a timer event

  On timer expiry the timer event is triggered.  
  This replaces the Register function in timer device.
  This can be used for both one-shot timeout or periodic timer.

  @param hEvent    : Event handle
         time      : time in microseconds, the timer is stopped for value of 0

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult DALSYS_TimerStart( DALSYSEventHandle hEvent, uint32 time);

/*
  @brief Stop a timer for a timer event

  This replaces the Unregister function in timer device.

  @param hEvent    : Event handle

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult DALSYS_TimerStop( DALSYSEventHandle hEvent );

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
DALSYS_SyncCreate(uint32 dwAttribs,
                  DALSYSSyncHandle *phSync,
                  DALSYSSyncObj *pSyncObj);

/*
  @brief Enter a DALSYS Sync section

  A User calls this API in order to get ownership of this synchronization.

  @param hSync : DALSYS Sync Handle

  @see DALYS_SyncLeave

  @return void
*/

void
DALSYS_SyncEnter(DALSYSSyncHandle hSync);

/*
  @brief Attempt to enter a DALSYS Sync section

  A User calls this API to try entering a sync section.
  If such attempt fails, the function returns immediately without blocking

  @param hSync : DALSYS Sync Handle

  @see DALYS_SyncEnter

  @return Return code, DAL_SUCCESS if sync section is entered successfully
      DAL_SYNC_ENTER_FAILED if failed to enter sync section
      DAL_ERROR for other errors
*/
DALResult
DALSYS_SyncTryEnter(DALSYSSyncHandle hSync);

/*
  @brief Leave a DALSYS Sync section

  Users must invoke this API if they wish to relinquish ownership of this
  synchronization.

  @param hSync : DALSYS Sync Handle

  @see DALYS_SyncEnter

  @return void
*/

void
DALSYS_SyncLeave(DALSYSSyncHandle hSync);


/*
  @brief Construct a DALSYS Memory Object

  This API is deprecated and does not support features such as 64-bit physical addressing.
  DALSYS_MemRegionAllocEx should be used instead.

  @param dwAttribs   : Memory Attributes such as cacheability, permission
                       preallocation

         VirtualAddr : Desired Virtual Addr, if no preference use
                       DALSYS_MEM_ADDR_NOT_SPECIFIED

         PhysicalAddr: Desired Physical Addr, if no preference use
                       DALSYS_MEM_ADDR_NOT_SPECIFIED

         dwLen       : Size of memory region

         phMem       : created DALSYS Mem Handle upon successful allocation,
                     NULL otherwise

         pMemObj     : pointer to user-provided static memory object, if NULL
                       the memory object is allocated dynamically

  @return DAL_SUCCESS on successful allocation, error code otherwise
*/
DALResult
DALSYS_MemRegionAlloc(uint32 dwAttribs, DALSYSMemAddr VirtualAddr,
                      DALSYSMemAddr PhysicalAddr, uint32 dwLen, 
                      DALSYSMemHandle *phMem, DALSYSMemObj *pMemObj);

/*
  @brief Construct a DALSYS Memory Object

  A DALSYS memory region is a MMU mapping between virtual and 
  physical memory, as well as the memory attributes 
  such as cacheability, permission, and shareability.

  This memory region may be created or reused from an exisitng region.  

  Addresses, sizes, and attributes requested by users may be adjusted to 
  conform to the underlying architecture.
  
  Users have the option of pre-allocating memory region created with the native
  OS APIs.  This enables users to create the types of memory regions 
  supported by the underlying OS but not supported by DAL.
  
  @param phMem       : created DALSYS Mem Handle upon successful allocation,
                       NULL otherwise
         pMemReq     : memory region request structure.  This must be defined
                       with DALSYS_MEM_REQ_OBJECT macro.
         pMemInfo    : optional argument for receiving information on
                       the created memory region.  This pointer may be set to 
                       NULL if user does not need the aforementioned 
                       information.

  @return DAL_SUCCESS on successful allocation, error code otherwise
*/
DALResult
DALSYS_MemRegionAllocEx( DALSYSMemHandle *phMem, const DALSYSMemReq *pMemReq, 
      DALSYSMemInfoEx *pMemInfo );
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
DALSYS_MemRegionMapPhys(DALSYSMemHandle hMem, uint32 dwVirtualBaseOffset,
                        DALSYSMemAddr PhysicalAddr, uint32 dwLen);

/*
  @brief Return Information on a DAL memory region

  This uses 32-bit physical address.

  @param hMem         : DALSYS Mem Handle
         pMemInfo     : Pointer to Mem Info structure, filled up by callee,
                        Valid only if return code is DAL_SUCCESS
  @return Return Code, DAL_SUCCESS on successful completion, 
          error code otherwise
*/
DALResult
DALSYS_MemInfo(DALSYSMemHandle hMem, DALSYSMemInfo *pMemInfo);

/*
  @brief Return Information on a DAL memory region

  This uses 64-bit physical address.

  @param hMem         : DALSYS Mem Handle
         pMemInfo     : Pointer to Mem Info structure, filled up by callee,
                        Valid only if return code is DAL_SUCCESS
  @return Return Code, DAL_SUCCESS on successful completion, 
          error code otherwise
*/
DALResult
DALSYS_MemInfoEx(DALSYSMemHandle hMem, DALSYSMemInfoEx *pMemInfo);


/*
  @brief Useful to Invalidate/flush/clean the cache

  @param CacheCmd     : Invalidate, Flush or clean ( see cache macro defs)
         VirtualAddr  : Virtual Address
         dwLen        : Length of the mem region

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/

DALResult
DALSYS_CacheCommand(uint32 CacheCmd, DALSYSMemAddr VirtualAddr, uint32 dwLen);

/*
  @brief Allocate memory from local heap

  This is similar to Malloc from the C library, but this will allocate memory
  within the DAL Environment Context. Memory alloced through this call should be
  freed by calling DALSYS_Free

  @param dwSize   : Requested size of memory to be alloced
         ppMem    : Return parameter, callee will populate this to point to the
                    block of alloced memory, valid only upon DAL_SUCCESS return
                    code
  @see DALSYS_Free

  @return Return Code, DAL_SUCCESS on successful completion, assert err_fatal otherwise
*/
DALResult
DALSYS_Malloc(uint32 dwSize, void **ppMem);

/*
  @brief Free allocated memory from local heap

  This is similar to free from the C library, but this will free memory alloced
  within the DAL Environment Context.

  @param pMem     : Pointer to block of memory alloced through DALSYS_Malloc

  @see DALSYS_Malloc
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALSYS_Free(void *pmem);

/*
  @brief Pause the execution of the calling thread for a specified amount of time
  
  Pause the execution of the calling thread for a specified amount of time.
  Intended for microsecond range delays internally implemented by looping.

  @param pause_time_us : Minimum amount of time in microseconds to delay/pause

  @return void
*/
void
DALSYS_BusyWait(uint32 pause_time_us);
/*
  @brief Logging and error reporting

  Log can have up to 5 32-bit arguments and message size less than 
  1024 characters.
  A script is provided to extract DALSYS log from a live T32 session or 
  T32 post-mortem dump. 
  
  @param DeviceId      : Desired Device Id
         dwLogEventType: See above defines for log event type
         pszMsg        : Log message pointer

  @return DALSYS_LOGEVENT_FATAL_ERROR will not return
*/
void
DALSYS_LogEvent(DALDEVICEID DeviceId, uint32 dwLogEventType, 
      const char * pszFmt, ...);

/*
  @brief Retreive the Remote Vtbl function pointer.

  @param None

  @return Pointer to remote vtbl if remoting is permitted/enabled
          NULL otherwise
*/

DALRemoteVtbl *
DALSYS_GetRemoteInterfaceVtbl(void);

/*
  @brief Set the Priority of the current thread to the desired priority. 1 is
         lowest priority and 255 being highest priority. This API returns the 
         original priority. To just obtain the original priority without changing
         to new level pass the argument priority to 0.

  @param priority   : desired priority level, 
                      to simply obtain current priority pass 0

  @return Return Code, previous thread priority 
*/
uint32 DALSYS_SetThreadPriority(uint32 priority);

/*
  @brief copy memory from source to destination buffer

  The number of bytes copied is the lesser of the destination and source
  buffer sizes.
  DALSYS_memscpy macro may be used to explicitly type-cast the parameters.

  @param pDest    : destination buffer
         iDestSz  : destination buffer size
         pSrc     : source buffer
         iSrcSize : source buffer length

  @return number of bytes copied 
 */
uint32 _DALSYS_memscpy(void * pDest, uint32 iDestSz, 
      const void * pSrc, uint32 iSrcSize);

/*
  @brief Safe copy memory from source (User process client) to destination buffer(GuestOS)

  @param client_handle  : client_handle obtained from a user client over a QDI call
         dest           : destination buffer
         src            : source buffer
         len            : source buffer length

  @return 0 on success, negative value on failure.
 */
int DalSys_CopyFromUser(int client_handle, void *dest, const void *src, unsigned len);

/*
  @brief Safe copy memory from source (GuestOs) to destination buffer(User process client)

  @param client_handle  : client_handle obtained from a user client over a QDI call
         dest           : destination buffer
         src            : source buffer
         len            : source buffer length

  @return 0 on success, negative value on failure.
 */
int DalSys_CopyToUser(int client_handle, void *dest, const void *src, unsigned len);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* DALSYS_H */

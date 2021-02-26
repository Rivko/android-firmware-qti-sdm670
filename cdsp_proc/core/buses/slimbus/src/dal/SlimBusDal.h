#ifndef SLIMBUSDAL_H
#define SLIMBUSDAL_H
/*
===========================================================================

FILE:         SlimBusDal.h

DESCRIPTION:  This file defines OS functionality used by the SlimBus driver
              in the DAL environment.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/src/dal/SlimBusDal.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/20/17   MJS     Changes for moving devcfg to settings directory.
11/15/16   MJS     Add additional MEMTAG definitions.
07/07/16   MJS     Interrupt enablement changes for HLOS compatibility.
02/05/16   MJS     Add support for multiple endpoints per port.
02/04/15   MJS     Fix compiler warnings.
09/09/14   mq      Multi-dataline support.
04/17/14   MJS     Ensure to reset timeout event for timed wait API.
04/09/14   MJS     Add timed event wait API.
02/28/14   MJS     Streamline ISR and don't lock device mutex in ISR.
01/31/14   MJS     Use interrupt-based clock toggle detection not polling.
08/16/13   MJS     Avoid acquiring the device mutex when already acquired.
07/25/13   MJS     Support for GPIO interrupt wakeup.
07/25/13   MJS     Check for external GPIO toggle during hardware init.
05/14/13   MJS     Add SlimBus_Memcpy definition.
03/21/13   MJS     Suppor for inactive framer.
02/21/13   MJS     Add port synchronization lock.
02/15/13   MJS     Update DALSYS_LogEvent fatal error routine.
01/09/13   MJS     Support more than 32 channels using dynamic allocation.
11/29/12   MJS     Export BAM context lock/unlock interface.
11/13/12   MJS     Support for non-standalone satellite clock management.
10/03/12   MJS     Add SlimBus_GetBamIOVecEx.
04/16/12   MJS     Updated malloc and DestroyEvent APIs.
12/11/11   MJS     Initial revision.

===========================================================================
             Copyright (c) 2011-2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include <stringl/stringl.h>
#include <DALDeviceId.h>
#include <DDIInterruptController.h>
#include <DDITimer.h>
#include <DDIHWIO.h>
#include <DDITlmm.h>
#include "ddislimbus.h"
#include "SlimBus.h"

#define SB_MEMTAG_0   0
#define SB_MEMTAG_1   0
#define SB_MEMTAG_2   0
#define SB_MEMTAG_3   0
#define SB_MEMTAG_4   0
#define SB_MEMTAG_5   0
#define SB_MEMTAG_6   0
#define SB_MEMTAG_7   0
#define SB_MEMTAG_8   0
#define SB_MEMTAG_9   0
#define SB_MEMTAG_10  0
#define SB_MEMTAG_11  0
#define SB_MEMTAG_12  0 
#define SB_MEMTAG_13  0
#define SB_MEMTAG_14  0
#define SB_MEMTAG_15  0
#define SB_MEMTAG_16  0
#define SB_MEMTAG_17  0
#define SB_MEMTAG_18  0
#define SB_MEMTAG_19  0
#define SB_MEMTAG_20  0
#define SB_MEMTAG_21  0
#define SB_MEMTAG_22  0
#define SB_MEMTAG_23  0
#define SB_MEMTAG_24  0
#define SB_MEMTAG_25  0
#define SB_MEMTAG_26  0
#define SB_MEMTAG_27  0

/**
 * @brief Return the SlimBus device handle
 *
 * This function returns the OS-invariant SlimBus device handle
 * 
 * @param pDevCtxt Pointer to the Slimbus device context
 * 
 * @return Pointer to the device handle
 */
SLIMBUS_INLINE void * SlimBus_GetDevHandle(SlimBusDevCtxt *pDevCtxt)
{
  return pDevCtxt;
}

/**
 * @brief Initialize the driver type
 *
 * This function initializes the driver type, either master 
 * driver of satellite driver 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on sucess, error code on error
 */
extern SBResult SlimBus_InitDriverType(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Initialize the Slimbus driver parameters
 *
 * This function initializes the Slimbus driver parameters
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_InitParams(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Initialize the master-specific Slimbus parameters
 *
 * This function initializes the master-specific Slimbus 
 * parameters 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_InitMasterParams(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Initialize the satellite-specific Slimbus parameters
 *
 * This function initializes the satellite-specific Slimbus 
 * parameters 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_InitSatParams(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Initialize the Slimbus services
 *
 * This function initializes the services used by the Slimbus 
 * driver 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_InitServices(SlimBusDevCtxt *pDevCtxt);


/**
 * @brief Initialize the master-specific Slimbus services
 *
 * This function initializes the master-specific services use by 
 * the Slimbus driver 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_InitMasterServices(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Initialize the Slimbus GPIOs
 *
 * This function initializes the Slimbus GPIOs
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_InitGpios(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief De-initialize the Slimbus GPIOs
 *
 * This function de-initializes the Slimbus GPIOs
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_DeinitGpios(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Determine whether the driver in satellite mode 
 *        should use clock control functionality
 *
 * This function determines whether the driver in satellite mode 
 * should use clock control functionality 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return TRUE if satellite clock control should be used, FALSE 
 *         otherwise
 */
extern SBBOOL SlimBus_UseSatelliteClockControl(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Inform power management that the Slimbus device is no
 *        longer idle
 *
 * This function informs power management that the SLimbus 
 * device is no longer idle 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_StopIdle(SlimBusDevCtxt *pDevCtxt)
{
  return SB_SUCCESS;
}

/**
 * @brief Inform power management that the Slimbus device is 
 *        idle
 *
 * This function informs power management that the Slimbus 
 * device is idle 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_ResumeIdle(SlimBusDevCtxt *pDevCtxt)
{
  return SB_SUCCESS;
}

/**
 * @brief Allocate memory
 *
 * This function allocates memory
 * 
 * @param[in] dwSize  Size of memory to allocoate
 * @param[out] ppVoid  Pointer to location to store address of 
 *       allocated memory
 * @param[in] uTag  Tag data to associate with this allocation 
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_Malloc(uint32 dwSize, void **ppVoid, uint32 uTag)
{
  return DALSYS_Malloc(dwSize, ppVoid);
}

/**
 * @brief Reallocate memory
 *
 * This function reallocates memory
 * 
 * @param[in] dwNewSize  Size of memory to allocoate
 * @param[in] dwOldSize  Previous size of allocated memory 
 * @param[out] ppVoid  Pointer to location to store address of 
 *       allocated memory
 * @param[in] uTag  Tag data to associate with this allocation 
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_Realloc(uint32 dwNewSize, uint32 dwOldSize, void **ppVoid, uint32 uTag)
{
  void *pData;
  if (DAL_SUCCESS != DALSYS_Malloc(dwNewSize, &pData))
  {
    return SB_ERROR;
  }
  memset(pData, 0, dwNewSize);
  if (NULL != *ppVoid)
  {
    memscpy(pData, dwNewSize, *ppVoid, dwOldSize);
    DALSYS_Free(*ppVoid);
  }
  *ppVoid = pData;
  return SB_SUCCESS;
}

/**
 * @brief Free memory
 *
 * This function frees previously allocated memory
 * 
 * @param[in] pMem  Pointer to the memory to free
 */
SLIMBUS_INLINE void SlimBus_Free(void *pMem)
{
  (void) DALSYS_Free(pMem);
}

/**
 * @brief Copy memory contents
 *
 * This function copies memory contents
 * 
 * @param[out] pDest  Pointer to copy destination
 * @param[in] pSrc  Pointer to copy source
 * @param[in] uSize  Size of memory to copy
 * 
 * @return Pointer to destination memory 
 */
#define SlimBus_Memcpy(pDest, pSrc, uSize) \
   memcpy(pDest, pSrc, uSize)

/**
 * @brief Copy memory contents
 *
 * This function copies memory contents
 * 
 * @param[out] pDest  Pointer to copy destination
 * @param[in] uSrcSize  Size of destination memory
 * @param[in] pSrc  Pointer to copy source
 * @param[in] uDestSize  Size of source memory to copy
 * 
 * @return Pointer to destination memory 
 */
#define SlimBus_Memscpy(pDest, uDestSize, pSrc, uSrcSize) \
   memscpy(pDest, uDestSize, pSrc, uSrcSize)

/**
 * @brief Mark the Slimbus device as a static service
 *
 * This function marks the Slimbus device as a static service so 
 * that it does not get uninitialized 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 */
SLIMBUS_INLINE void SlimBus_MarkDeviceStatic(SlimBusDevCtxt *pDevCtxt)
{
  DALFW_MarkDeviceStatic((DALDevCtxt*)pDevCtxt);
}

/**
 * @brief Lock the Slimbus device
 *
 * This function locks the Slimbus device mutex
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 */
SLIMBUS_INLINE void SlimBus_LockDevice(SlimBusDevCtxt *pDevCtxt)
{
  if (NULL != pDevCtxt &&
      NULL != pDevCtxt->hSync)
  {
    DALSYS_SyncEnter(pDevCtxt->hSync);
  }
}

/**
 * @brief Unlock the Slimbus device
 *
 * This function unlocks the Slimbus device mutex
 * 
 * @param[in] pDevCtxt  Pointer to the SLimbus device
 */
SLIMBUS_INLINE void SlimBus_UnlockDevice(SlimBusDevCtxt *pDevCtxt)
{
  if (NULL != pDevCtxt &&
      NULL != pDevCtxt->hSync)
  {
    DALSYS_SyncLeave(pDevCtxt->hSync);
  }
}

/**
 * @brief Lock the Slimbus device
 *
 * This function locks the Slimbus device mutex
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 */
SLIMBUS_INLINE void SlimBus_LockExtClkToggle(SlimBusDevCtxt *pDevCtxt)
{
  if (NULL != pDevCtxt &&
      NULL != pDevCtxt->hExtClkSync)
  {
    DALSYS_SyncEnter(pDevCtxt->hExtClkSync);
  }
}

/**
 * @brief Unlock the Slimbus device
 *
 * This function unlocks the Slimbus device mutex
 * 
 * @param[in] pDevCtxt  Pointer to the SLimbus device
 */
SLIMBUS_INLINE void SlimBus_UnlockExtClkToggle(SlimBusDevCtxt *pDevCtxt)
{
  if (NULL != pDevCtxt &&
      NULL != pDevCtxt->hExtClkSync)
  {
    DALSYS_SyncLeave(pDevCtxt->hExtClkSync);
  }
}

/**
 * @brief Enable the clock GPIO wakeup interrupt
 *
 * This function enables the clock GPIO wakeup interrupt
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
extern SBResult SlimBus_ClkGpioIntEnable(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Disable the clock GPIO wakeup interrupt
 *
 * This function disables the clock GPIO wakeup interrupt
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
extern SBResult SlimBus_ClkGpioIntDisable(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Create a client-level mutex lock
 *
 * This function creates a client-level mutex lock
 * 
 * @param[in] pCtxt  Pointer to the client context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_CreateClientLock(SlimBusClientCtxt *pCtxt)
{
  return DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT, &pCtxt->hSync, NULL);
}

/**
 * @brief Lock the client context
 *
 * This function locks the client context mutex
 * 
 * @param[in] pCtxt  Pointer to the client context
 */
SLIMBUS_INLINE void SlimBus_LockClient(SlimBusClientCtxt *pCtxt)
{
  if (NULL != pCtxt &&
      NULL != pCtxt->hSync)
  {
    DALSYS_SyncEnter(pCtxt->hSync);
  }
}

/**
 * @brief Unlock the client context
 *
 * This function unlocks the client context
 * 
 * @param[in] pCtxt  Pointer to the client context
 */
SLIMBUS_INLINE void SlimBus_UnlockClient(SlimBusClientCtxt *pCtxt)
{
  if (NULL != pCtxt &&
      NULL != pCtxt->hSync)
  {
    DALSYS_SyncLeave(pCtxt->hSync);
  }
}

/**
 * @brief Reset an event object
 *
 * This function resets an event object
 * 
 * @param[in] hEvent  Event handle
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_EventReset(SlimBusEventHandle hEvent)
{
  return DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET);
}

/**
 * @brief Wait on an event object
 *
 * This function waits on an event object
 * 
 * @param[in] hEvent  Event handle
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_EventWait(SlimBusEventHandle hEvent)
{
  return DALSYS_EventWait(hEvent);
}

/**
 * @brief Wait on an event object with timeout
 *
 * This function performs a timed wait on an event object
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] hEvent  Event handle
 * @param[in] uTimeoutUsec  Timeout in microseconds
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_EventTimedWait
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusEventHandle hEvent,
  uint32 uTimeoutUsec
)
{
  SBResult result = SB_ERROR;
  
  if (NULL != pClientCtxt)
  {
    uint32 uIdx = 1;
    DALSYSEventHandle hEvents[] = { hEvent, pClientCtxt->hTimeoutEvent };
    
    DALSYS_EventCtrl(pClientCtxt->hTimeoutEvent, DALSYS_EVENT_CTRL_RESET);

    result = DALSYS_EventMultipleWait(hEvents,
                                      sizeof(hEvents)/sizeof(hEvents[0]),
                                      uTimeoutUsec,
                                      &uIdx);
    if (SB_SUCCESS == result &&
        uIdx > 0)
    {
      result = SLIMBUS_ERR_MSG_RESP_TIMEOUT;
    }
  }
  
  return result;
}

/**
 * @brief Signal an event object
 *
 * This function signals (triggers) an event object
 * 
 * @param[in] hEvent  Event handle
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_EventTrigger(SlimBusEventHandle hEvent)
{
  return DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_TRIGGER);
}

/**
 * @brief Signal an event object with extended options
 *
 * This function signals (triggers) an event object and provides 
 * extended configuration options 
 * 
 * @param[in] hEvent  Event handle
 * @param[in] uUserData  User-specified data associated with the 
 *       event signal
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_EventTriggerEx(SlimBusEventHandle hEvent, void *puUserData)
{
  return DALSYS_EventCtrlEx(hEvent, DALSYS_EVENT_CTRL_TRIGGER, (uint32)puUserData, NULL, 0);
}


/**
 * @brief Create a synchronous event object
 *
 * This function creates a sychronous event object
 * 
 * @param[out] phEvent  Pointer to the location to store the 
 *                 event object
 * @param[in] pEventObj  Pointer to the pre-allocated storage 
 *                   for the event object
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_CreateSyncEvent(SlimBusEventHandle *phEvent, SlimBusEventObj *pEventObj)
{
  return DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT, phEvent, pEventObj);
}

/**
 * @brief Create a callback event object
 *
 * This function creates a callback event object
 * 
 * @param[out] phEvent  Pointer to the location to store the 
 *                 event object
 * @param[in] pEventObj  Pointer to the pre-allocated storage 
 *                   for the object
 * @param[in] cbFunc  Callback function to be associated with 
 *       the event
 * @param[in] cbData  Callback data to be associated with the 
 *       event
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_CreateCallbackEvent(SlimBusEventHandle *phEvent, SlimBusEventObj *pEventObj, SlimBusCallbackFunc cbFunc, void *cbData)
{
  SBResult result;
  
  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, phEvent, pEventObj);
  if (SB_SUCCESS == result)
  {
    result = DALSYS_SetupCallbackEvent(*phEvent, cbFunc, cbData);
  }
  return result;
}

/**
 * @brief Create a timeout event object
 *
 * This function creates a timeout event object
 * 
 * @param[out] phEvent  Pointer to the location to store the 
 *                 timeout event object
 * @param[in] pEventObj  Pointer to pre-allocated storage for 
 *                   the event object
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_CreateTimeoutEvent(SlimBusEventHandle *phEvent, SlimBusEventObj *pEventObj)
{
  return DALSYS_EventCreate( (DALSYS_EVENT_ATTR_TIMEOUT_EVENT|
                              DALSYS_EVENT_ATTR_NON_DEFERRED|
                              DALSYS_EVENT_ATTR_NORMAL),
                             phEvent,
                             pEventObj );
}

/**
 * @brief Get the Slimbus device structure associated with the 
 *        device handle
 *
 * This function gets the Slimbus device structure associated 
 * with the device handle 
 * 
 * @param[in] hDevCtxt  Handle to the Slimbus device
 * 
 * @return Pointer to the Slimbus device structure
 */
SLIMBUS_INLINE struct SlimBusDevCtxt *SB_GET_PDEVCTXT(void *hDevCtxt) 
{
  return (struct SlimBusDevCtxt*)(hDevCtxt);
}

/**
 * @brief Get the Slimbus client context associated with the 
 *        client handle
 *
 * This function gets the Slimbus client context associated with 
 * the client handle 
 * 
 * @param[in] hCtxt  Client handle
 * 
 * @return Pointer to the client context
 */
SLIMBUS_INLINE struct SlimBusClientCtxt *SB_GET_PCLIENTCTXT(void *hCtxt) 
{
  return ((DalSlimBusHandle *)hCtxt)->pClientCtxt;
}

/**
 * @brief Acquire an ISR level lock
 *
 * This function acquires an ISR level mutex lock
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device 
 */
SLIMBUS_INLINE void SlimBus_InterruptAcquireLock(SlimBusDevCtxt *pDevCtxt, uint32 *puToken)
{
  if (NULL != pDevCtxt->hIsrSync)
  {
    DALSYS_SyncEnter(pDevCtxt->hIsrSync);
  }
}

/**
 * @brief Release an ISR level lock
 *
 * This function releases an ISR level mutex lock
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 */
SLIMBUS_INLINE void SlimBus_InterruptReleaseLock(SlimBusDevCtxt *pDevCtxt, uint32 *puToken)
{
  if (NULL != pDevCtxt->hIsrSync)
  {
    DALSYS_SyncLeave(pDevCtxt->hIsrSync);
  }
}

/**
 * @brief Acquire a BAM synchronization lock
 *
 * This function acquires a BAM synchronization lock
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device 
 */
SLIMBUS_INLINE void SlimBus_BamAcquireLock(SlimBusDevCtxt *pDevCtxt)
{
  if (NULL != pDevCtxt->Plat.hBamSync)
  {
    DALSYS_SyncEnter(pDevCtxt->Plat.hBamSync);
  }
}

/**
 * @brief Release a BAM synchronization lock
 *
 * This function releases a BAM synchronization lock
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 */
SLIMBUS_INLINE void SlimBus_BamReleaseLock(SlimBusDevCtxt *pDevCtxt)
{
  if (NULL != pDevCtxt->Plat.hBamSync)
  {
    DALSYS_SyncLeave(pDevCtxt->Plat.hBamSync);
  }
}

/**
 * @brief Acquire a port synchronization lock
 *
 * This function acquires a port synchronization lock
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device 
 * @param[in] pPort  Pointer to a master port 
 */
SLIMBUS_INLINE void SlimBus_PortAcquireLock(SlimBusDevCtxt *pDevCtxt, SlimBusMasterPortType *pPort)
{
  UNREFERENCED_PARAMETER(pPort);

  if (NULL != pDevCtxt->hPortSync)
  {
    DALSYS_SyncEnter(pDevCtxt->hPortSync);
  }
}

/**
 * @brief Release a port synchronization lock
 *
 * This function releases a port synchronization lock
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device 
 * @param[in] pPort  Pointer to a master port 
 */
SLIMBUS_INLINE void SlimBus_PortReleaseLock(SlimBusDevCtxt *pDevCtxt, SlimBusMasterPortType *pPort)
{
  UNREFERENCED_PARAMETER(pPort);
  
  if (NULL != pDevCtxt->hPortSync)
  {
    DALSYS_SyncLeave(pDevCtxt->hPortSync);
  }
}

/**
 * @brief Create a port synchronization mutex lock
 *
 * This function creates a port synchronization mutex lock
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device 
 * @param[in] pPort  Pointer to a master port 
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_PortCreateLock(SlimBusDevCtxt *pDevCtxt, SlimBusMasterPortType *pPort)
{
  UNREFERENCED_PARAMETER(pPort);

  if (NULL == pDevCtxt->hPortSync)
  {
    return DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT, &pDevCtxt->hPortSync, NULL);
  }
  else
  {
    return SB_SUCCESS;
  }
}

/**
 * @brief Start a timer
 *
 * This function starts a timer
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * @param[in] hTimer  Timer handle
 * @param[in] hEvent  Event handle
 * @param[in] uTimeoutUsec  Microsecond timeout
 */
void SlimBus_TimerStart(SlimBusDevCtxt *pDevCtxt, SlimBusTimerHandle hTimer,
                        SlimBusEventHandle hEvent, uint32 uTimeoutUsec);

/**
 * @brief Stop a timer
 *
 * This function stops a timer
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * @param[in] hTimer  Timer handle
 * @param[in] hEvent  Event handle
 */
void SlimBus_TimerStop(SlimBusDevCtxt *pDevCtxt, SlimBusTimerHandle hTimer,
                       SlimBusEventHandle hEvent);

/**
 * @brief Disable Slimbus GPIOs
 *
 * This function disables the Slimbus GPIOs
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_GpioIntDisable(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Enable the Slimbus GPIO interrupt
 *
 * This function enables the Slimbus GPIO interrupt
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_GpioIntEnable(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Check for external GPIO toggle
 *
 * This function checks for an external GPIO toggle on the clock 
 * line. 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return TRUE if external GPIO toggle was detected, FALSE 
 *         otherwise
 */
extern SBBOOL SlimBus_CheckGpioClockToggle(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Allocate physical memory
 *
 * This function allocates physical memory
 * 
   @param[in] uLen  Length of the physical memory to allocate in
                bytes
 * @param[out] phMem  Location to store the physical memory 
 *       handle
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_PhysMemAlloc(uint32 uLen, SlimBusMemHandle *phMem)
{
  return DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                               DALSYS_MEM_ADDR_NOT_SPECIFIED,
                               DALSYS_MEM_ADDR_NOT_SPECIFIED,
                               uLen, phMem, NULL);
}

/**
 * @brief Retrive information about physical memory
 *
 * This function retrieves information about previously 
 * allocated physical memory 
 * 
 * @param[in] hMem  Physical memory handle
 * @param[out] puVirtAddr  Pointer to the location to store the 
 *       virtual memory address
 * @param[out] puPhysAddr  Pointer to the location to store the 
 *       physical memory address
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_PhysMemInfo(SlimBusMemHandle hMem, size_t *puVirtAddr, size_t *puPhysAddr)
{
  DALSYSMemInfo MemInfo;
  SBResult result;
  
  result = DALSYS_MemInfo(hMem, &MemInfo);
  if (SB_SUCCESS == result)
  {
    if (NULL != puVirtAddr)
    {
      *puVirtAddr = MemInfo.VirtualAddr;
    }
    if (NULL != puPhysAddr)
    {
      *puPhysAddr = MemInfo.PhysicalAddr;
    }
  }
  
  return result;  
}

/**
 * @brief Destroy an object
 *
 * This function destroys an object
 * 
 * @param[in] hObj  Object handle
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_DestroyObject(DALSYSObjHandle hObj)
{
  return DALSYS_DestroyObject(hObj);
}

/**
 * @brief Destroy an event object
 *
 * This function destroys an event object
 * 
 * @param[in] hEvent  Event handle
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SLIMBUS_INLINE SBResult SlimBus_DestroyEvent(SlimBusEventHandle hEvent)
{
  return SlimBus_DestroyObject(hEvent);
}

/**
 * @brief Perform a memory barrier instruction
 *
 * This function performs a memory barrier instruction
 * 
 * @param[in] pMem  Pointer to the memory to be accessed
 */
SLIMBUS_INLINE void SlimBus_MemoryBarrier(void *pMem)
{
  DALFW_MemoryBarrier(pMem);
}

/**
 * @brief Handle a fatal error
 *
 * This function handles a fatal error
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * @param[in] pszStr  Error message string
 */
SLIMBUS_INLINE void SlimBus_ErrorFatal(SlimBusDevCtxt *pDevCtxt, const char *pszStr)
{
  DALSYS_LogEvent(pDevCtxt->Base.DevId, DALSYS_LOGEVENT_FATAL_ERROR, "%s", pszStr);
}

/**
 * @brief Initialize the logging level variable
 *
 * This function initializes the logging level variable
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
SLIMBUS_INLINE void SlimBus_LogLevelInit(SlimBusDevCtxt *pDevCtxt)
{
  DALSYSPropertyVar PropVar;
  
  if (SB_LOG_LEVEL_UNDEFINED != g_iSlimBusLogLevelOverride &&
      g_iSlimBusLogLevelOverride >= 0)
  {
    pDevCtxt->uLogLevel = (uint32)g_iSlimBusLogLevelOverride;
  }
  else if ( SB_SUCCESS == DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, 
                                                    "log_level", 0, &PropVar ) &&
            DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->uLogLevel = PropVar.Val.dwVal;
  }
  else
  {
    pDevCtxt->uLogLevel = SB_LOG_LEVEL_DEFAULT;
  }
}

/**
  @brief Configure a BAM data pipe for a SLIMbus master port
  
  This function configures the BAM pipe for a SLIMbus master port to 
  source or sink data.  Half-duplex ports are treated logically 
  as a single port with both a source and sink pipe.  All other 
  ports are unidirectional and have either a source or sink 
  pipe. 

  @param[in] h  Client handle to the SLIMbus driver
  @param[in] hPort  Client handle to the master port 
  @param[in] eFlow  The pipe flow to configure, either source or 
        sink
  @param[in] pConfig  Pointer to the configuration to set for the BAM pipe

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult 
SlimBus_ConfigMasterPipe(uint32 ClientId, DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusPortFlowType  eFlow, const SlimBusPipeConfigType * pConfig);

/**
  @brief Register for asynchronous BAM event(s)

  This function registers for asynchronous BAM event(s).  If the 
  registered event is a callback event, a 
  SlimBusBamEventNotifyType structure will be passed back as the
  user data in the callback.  If the event is not a callback 
  event, the SlimBusBamEventNotifyType structure can be 
  retrieved by the DalSlimBus_GetBamEvent() function after the 
  event object has been signaled. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle corresponding to the BAM 
        pipe
  @param[in] eTransferDir  Direction of data flow for the BAM 
        pipe to register for.  For bi-directional ports, there
        is one BAM pipe for each of the transmit and receive
        directions.
  @param[in] pReg  Pointer to location of BAM registration 
        structure
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult 
SlimBus_RegisterBamEvent(uint32 ClientId, DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusBamRegisterEventType * pReg);

/**
  @brief Submit a new data transfer to the BAM descriptor FIFO 

  This function submits a new data transfer to the BAM 
  descriptor FIFO. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle corresponding to the BAM 
        pipe
  @param[in] eTransferDir  Direction of data flow for the BAM 
        pipe to submit the transfer for.  For bi-directional
        ports, there is one BAM pipe for each of the transmit
        and receive directions.
  @param[in] pIOVec  Pointer to the IO-vector for the data 
        transfer
  @param[in] pUser   User pointer that will be passed back 
        during an asynchronous BAM callback event for the
        descriptor
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult 
SlimBus_SubmitBamTransfer(uint32 ClientId, DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusBamIOVecType * pIOVec, void * pUser);

/**
  @brief Get a processed BAM IO-vector 

  This function fetches the next processed BAM IO-vector.  The 
  IO-vector structure will be zeroed if there are no more 
  processed IO-vectors. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle corresponding to the BAM 
        pipe
  @param[in] eTransferDir  Direction of data flow for the BAM
        pipe to get the IO-vector for.  For bi-directional
        ports, there is one BAM pipe for each of the transmit
        and receive directions.
  @param[out] pIOVec  Pointer to the location to store a 
        processed IO-vector.  The structure will be zeroed if
        there are no more processed IO-vectors.
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult 
SlimBus_GetBamIOVec(uint32 ClientId, DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusBamIOVecType * pIOVec);


/**
  @brief Get a processed BAM IO-vector with user data

  This function fetches the next processed BAM IO-vector.  The 
  IO-vector structure will be zeroed if there are no more 
  processed IO-vectors.  The user data pointer is optional and 
  if supplied will be filled with the user data pointer passed 
  when the descriptor was submitted. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle corresponding to the BAM 
        pipe
  @param[in] eTransferDir  Direction of data flow for the BAM
        pipe to get the IO-vector for.  For bi-directional
        ports, there is one BAM pipe for each of the transmit
        and receive directions.
  @param[out] pIOVec  Pointer to the location to store a 
        processed IO-vector.  The structure will be zeroed if
        there are no more processed IO-vectors.
  @param[out] ppUser  Pointer to location to store the user data 
        pointer associated with a processed IO-vector.  The
        pointer will be NULL if there are no more processed
        IO-vectors.
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_GetBamIOVecEx(uint32 ClientId, DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusBamIOVecType * pIOVec, void **ppUser);


#endif /* SLIMBUSDAL_H */


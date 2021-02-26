#ifndef __DALIINTERRUPTCONTROLLER_H__
#define __DALIINTERRUPTCONTROLLER_H__

#include "DalDevice.h"

#define DALINTERRUPTCONTROLLER_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

typedef uint32  DALInterruptID;
typedef void * DALISRCtx;
typedef void * (*DALISR)(DALISRCtx);
typedef void * DALIRQCtx;
typedef void * (*DALIRQ)(DALIRQCtx);

//
// Use the following for extended (non-default) controls along with
// with RegisterEvent, RegisterISR( h, intrID, enableCtrl )
// high nibble i.e. 0xFFxxxxxx is used for level
// NOTE: These extended controls are not available for all SW platforms
//       please check for availability prior to use.
#define DALINTRCTRL_ENABLE_DEFAULT_SETTINGS        0x00000001
#define DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER      0x00000002
#define DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER       0x00000003
#define DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER     0x00000004
#define DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER    0x00000005

/* Note : this is the legacy support for priority */
#define DALINTRCTRL_ENABLE_CUSTOM_SETTINGS(edge,level,priority) \
                                          (edge|level|(priority<<24))

/* when trying to set the trigger of an interrupt along with its priority 
please use the below macro */
#define DALINTRCTRL_ENABLE_CONFIG(trigger,priority) \
                                          (trigger|(priority<<24))

typedef struct DalInterruptController DalInterruptController;
struct DalInterruptController
{
   struct DalDevice DalDevice;
   DALResult (*RegisterISR)(DalDeviceHandle * _h, DALInterruptID  intrID,
                           const DALISR  isr, const DALISRCtx  ctx, uint32  bEnable);
   DALResult (*RegisterEvent)(DalDeviceHandle * _h, DALInterruptID  intrID,
                             const DALSYSEventHandle  hEvent, uint32  bEnable);
   DALResult (*Unregister)(DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptDone)(DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptEnable)(DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptDisable)(DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptTrigger)(DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptClear)(DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptStatus)(DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*RegisterIRQHandler)(DalDeviceHandle * _h, DALInterruptID  intrID,
                           const DALIRQ  irq, const DALIRQCtx  ctx, uint32  bEnable);
   DALResult (*SetInterruptTrigger)(DalDeviceHandle * _h, DALInterruptID intrID, uint32 nTrigger);
   DALResult (*IsInterruptPending)(DalDeviceHandle * _h, DALInterruptID intrID,void* bState,uint32 size);
   DALResult (*IsInterruptEnabled)(DalDeviceHandle * _h, DALInterruptID intrID,void* bState,uint32 size);
};

typedef struct DalInterruptControllerHandle DalInterruptControllerHandle;
struct DalInterruptControllerHandle
{
   uint32 dwDalHandleId;
   const DalInterruptController * pVtbl;
   void * pClientCtxt;
};

#define DAL_InterruptControllerDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALINTERRUPTCONTROLLER_INTERFACE_VERSION,hDalDevice)

static __inline DALResult
DalInterruptController_RegisterISR(DalDeviceHandle * _h, DALInterruptID  intrID,
                           const DALISR  isr, const DALISRCtx  ctx, uint32  IntrFlags)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->RegisterISR( _h, intrID,
                                                                   isr, ctx, IntrFlags);
}
static __inline DALResult
DalInterruptController_RegisterEvent(DalDeviceHandle * _h, DALInterruptID  intrID,
                            const DALSYSEventHandle  hEvent, uint32  IntrFlags)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->RegisterEvent( _h, intrID,
                                                                     hEvent, IntrFlags);
}
static __inline DALResult
DalInterruptController_Unregister(DalDeviceHandle * _h, DALInterruptID  intrID)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->Unregister( _h, intrID);
}
static __inline DALResult
DalInterruptController_InterruptDone(DalDeviceHandle * _h, DALInterruptID  intrID)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptDone( _h, intrID);
}
static __inline DALResult
DalInterruptController_InterruptEnable(DalDeviceHandle * _h, DALInterruptID  intrID)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptEnable( _h, intrID);
}
static __inline DALResult
DalInterruptController_InterruptDisable(DalDeviceHandle * _h, DALInterruptID  intrID)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptDisable( _h, intrID);
}
static __inline DALResult
DalInterruptController_InterruptTrigger(DalDeviceHandle * _h, DALInterruptID  intrID)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptTrigger( _h, intrID);
}
static __inline DALResult
DalInterruptController_InterruptClear(DalDeviceHandle * _h, DALInterruptID  intrID)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptClear( _h, intrID);
}
static __inline DALResult
DalInterruptController_InterruptStatus(DalDeviceHandle * _h, DALInterruptID  intrID)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptStatus( _h, intrID);
}
static __inline DALResult
DalInterruptController_RegisterIRQHandler(DalDeviceHandle * _h, DALInterruptID  intrID,
                           const DALIRQ  irq, const DALIRQCtx  ctx, uint32  bEnable)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->RegisterIRQHandler( _h, intrID,
                                                                   irq, ctx, bEnable);
}
static __inline DALResult
DalInterruptController_SetTrigger(DalDeviceHandle * _h, DALInterruptID  intrID,
                                  uint32 nTrigger)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->SetInterruptTrigger( _h, intrID, nTrigger);
}
static __inline DALResult
DalInterruptController_IsInterruptPending(DalDeviceHandle * _h, DALInterruptID  intrID,uint32* bState)
{
  return ((DalInterruptControllerHandle *)_h)->pVtbl->IsInterruptPending( _h, intrID, (void*)bState,1);
}
static __inline DALResult
DalInterruptController_IsInterruptEnabled(DalDeviceHandle * _h, DALInterruptID  intrID,uint32* bState)
{
  return ((DalInterruptControllerHandle *)_h)->pVtbl->IsInterruptEnabled( _h, intrID, (void*)bState,1);
}
#endif

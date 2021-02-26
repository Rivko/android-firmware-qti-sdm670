/*==============================================================================
  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
           All Rights Reserved.
  Qualcomm Confidential and Proprietary

==============================================================================*/
#ifndef __DALSYSTEMCALL_H__
#define __DALSYSTEMCALL_H__

#include "DalDevice.h"

#define DALSYSTEMCALL_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

typedef struct DalSystemCall
{
   DalDevice DalDevice;
   DALResult (*SoftwareWaitForInterrupt)(DalDeviceHandle * _h);
   DALResult (*PowerCollapse)(DalDeviceHandle * _h);
   DALResult (*SetAVSEnabled)(DalDeviceHandle * _h, int AVSIsEnabled);
   DALResult (*SetAVSDelaySynthesizerRAW)(DalDeviceHandle * _h, 
         uint32 AVSDelaySynthesizerRAW);
   DALResult (*SyncSysTimeTick)(DalDeviceHandle * _h, uint32 timeDelta );
} DalSystemCall;


typedef struct DalSystemCallHandle
{
   uint32 dwDalHandleId;
   const DalSystemCall * pVtbl;
   void * pClientCtxt;
} DalSystemCallHandle;

#define DAL_SystemCallDeviceAttach(hDalDevice)\
        DAL_DeviceAttachEx(NULL, DALDEVICEID_SYSTEMCALL, \
              DALSYSTEMCALL_INTERFACE_VERSION, hDalDevice)

static __inline DALResult
DalSystemCall_SoftwareWaitForInterrupt(DalDeviceHandle * _h)
{
   return ((DalSystemCallHandle *)_h)->pVtbl->SoftwareWaitForInterrupt(_h);
}

static __inline DALResult
DalSystemCall_PowerCollapse(DalDeviceHandle * _h)
{
   return ((DalSystemCallHandle *)_h)->pVtbl->PowerCollapse(_h);
}

static __inline DALResult
DalSystemCall_SetAVSEnabled(DalDeviceHandle * _h, int AVSIsEnabled)
{
   return ((DalSystemCallHandle *)_h)->pVtbl->SetAVSEnabled(_h, AVSIsEnabled);
}

static __inline DALResult
DalSystemCall_SetAVSDelaySynthesizerRAW(DalDeviceHandle * _h, 
      uint32 AVSDelaySynthesizerRAW)
{
   return ((DalSystemCallHandle *)_h)->pVtbl->SetAVSDelaySynthesizerRAW(_h, 
         AVSDelaySynthesizerRAW);
}

static __inline DALResult
DalSystemCall_SyncSysTimeTick(DalDeviceHandle * _h, 
      uint32 timeDelta)
{
   return ((DalSystemCallHandle *)_h)->pVtbl->SyncSysTimeTick(_h, 
         timeDelta);
}
#endif

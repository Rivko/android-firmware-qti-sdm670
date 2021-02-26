/*==============================================================================
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
           All Rights Reserved.
  Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/api/dal/DDISystemCall.h#1 $
==============================================================================*/
#ifndef __DALSYSTEMCALL_H__
#define __DALSYSTEMCALL_H__

#include "DalDevice.h"

#define DALSYSTEMCALL_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

typedef struct DalSystemCall
{
   DalDevice DalDevice;
   DALResult (*SoftwareWaitForInterrupt)(uint32 ClientHandle, DalDeviceHandle * _h);
   DALResult (*PowerCollapse)(uint32 ClientHandle, DalDeviceHandle * _h);
   DALResult (*SetAVSEnabled)(uint32 ClientHandle, DalDeviceHandle * _h, int AVSIsEnabled);
   DALResult (*SetAVSDelaySynthesizerRAW)(uint32 ClientHandle, DalDeviceHandle * _h, 
         uint32 AVSDelaySynthesizerRAW);
   DALResult (*SyncSysTimeTick)(uint32 ClientHandle, DalDeviceHandle * _h, uint32 timeDelta );
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
   return ((DalSystemCallHandle *)_h)->pVtbl->SoftwareWaitForInterrupt(DALDEVICE_LOCAL_CLIENTID, _h);
}

static __inline DALResult
DalSystemCall_PowerCollapse(DalDeviceHandle * _h)
{
   return ((DalSystemCallHandle *)_h)->pVtbl->PowerCollapse(DALDEVICE_LOCAL_CLIENTID, _h);
}

static __inline DALResult
DalSystemCall_SetAVSEnabled(DalDeviceHandle * _h, int AVSIsEnabled)
{
   return ((DalSystemCallHandle *)_h)->pVtbl->SetAVSEnabled(DALDEVICE_LOCAL_CLIENTID, _h, AVSIsEnabled);
}

static __inline DALResult
DalSystemCall_SetAVSDelaySynthesizerRAW(DalDeviceHandle * _h, 
      uint32 AVSDelaySynthesizerRAW)
{
   return ((DalSystemCallHandle *)_h)->pVtbl->SetAVSDelaySynthesizerRAW(DALDEVICE_LOCAL_CLIENTID, _h, 
         AVSDelaySynthesizerRAW);
}

static __inline DALResult
DalSystemCall_SyncSysTimeTick(DalDeviceHandle * _h, 
      uint32 timeDelta)
{
   return ((DalSystemCallHandle *)_h)->pVtbl->SyncSysTimeTick(DALDEVICE_LOCAL_CLIENTID, _h, 
         timeDelta);
}
#endif

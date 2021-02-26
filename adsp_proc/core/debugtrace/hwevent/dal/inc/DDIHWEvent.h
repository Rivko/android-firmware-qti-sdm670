#ifndef DDIHWEvent_H
#define DDIHWEvent_H

//========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.qdsp6/2.1/debugtrace/hwevent/dal/inc/DDIHWEvent.h#1 $
//
//========================================================================
//             Copyright (c) 2011 QUALCOMM Technologies Incorporated.
//                    All Rights Reserved.
//                    QUALCOMM Proprietary
//========================================================================

#include "DalDevice.h"

#define DALHWEVENT_STRING_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

#define QDSS_HWEVENT_CONFIG_SUCCESS         0x0
#define QDSS_HWEVENT_UNKNOWN_ERR            -1
#define QDSS_HWEVENT_FUSE_CHECK_ERR         0x2
#define QDSS_HWEVENT_ADDR_CHECK_ERR         0x3



                     
typedef struct DalHWEvent DalHWEvent;
struct DalHWEvent
{
   struct DalDevice DalDevice;
   DALResult (*SetRegister)(uint32 ClientId, DalDeviceHandle* h,uint32 phys_addr,uint32 value,uint8 *pret_code);
   DALResult (*ConfigurePreset)(uint32 ClientId, DalDeviceHandle* h);
};

typedef struct DalHWEventHandle DalHWEventHandle; 
struct DalHWEventHandle 
{
   uint32 dwDalHandleId;
   const DalHWEvent *pVtbl;
   void * pClientCtxt;
};

#define DAL_HWEventDeviceAttach(DevId,hDalDevice)\
        DAL_StringDeviceAttachEx(NULL,DevId,DALHWEVENT_STRING_INTERFACE_VERSION,hDalDevice)


static __inline DALResult
DAL_HWEvent_SetRegister(DalDeviceHandle *phandle,uint32 uPhysAddr,uint32 uValue,uint8 *pret_code)
{
   return ((DalHWEventHandle *)phandle)->pVtbl->SetRegister(DALDEVICE_LOCAL_CLIENTID, phandle,uPhysAddr,uValue,pret_code);
}

static __inline DALResult
DAL_HWEvent_ConfigurePreset(DalDeviceHandle *phandle)
{
   return ((DalHWEventHandle *)phandle)->pVtbl->ConfigurePreset(DALDEVICE_LOCAL_CLIENTID, phandle);
}


#endif

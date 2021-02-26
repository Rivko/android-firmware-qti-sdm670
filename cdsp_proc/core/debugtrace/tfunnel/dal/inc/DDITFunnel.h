#ifndef DDITFUNNEL_H
#define DDITFUNNEL_H

/*=============================================================================
   Copyright (c) 2011 Qualcomm Technologies Incorporated.
   All Rights Reserved.
=============================================================================*/

//========================================================================
//
//                   STM Configuration Driver Interface
//
// @file dditfunnel.ddi
//
//   This file contains the definitions of the constants, types, and
//   interfaces that comprise the interface to the Trace Funnel
//   DAL driver.
//
//========================================================================
//
//                             Edit History
//
//
// when       who     what, where, why
// --------   ---     ----------------------------------------------------
// 07/22/11   ps      Initial revision.
//
//========================================================================
//             Copyright © 2011 Qualcomm Technologies Incorporated.
//                    All Rights Reserved.
//                    QUALCOMM Proprietary
//========================================================================


#include "DalDevice.h"
#include "DALStdDef.h"



#define DALTFUNNEL_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

typedef enum
{
   TFUNNEL_PORT_ENABLE,
   TFUNNEL_PORT_PRIORITY,
   TFUNNEL_PORT_STATUS,
   _PLACEHOLDER_TFunnelControlType = 0x7fffffff
}TFunnelControlType;


typedef struct {
   const char *pPortStr;
   union {
      DALBOOL bEnable;
      uint32 nPrio;
   }Val;
} TFunnelControlParam;


typedef struct DalTFunnel DalTFunnel;
struct DalTFunnel
{
   struct DalDevice DalDevice;
   DALResult (*Control)(uint32 ClientID, DalDeviceHandle * _h, TFunnelControlType  eCtrl,
    TFunnelControlParam *pCtrlParam);
   DALResult (*DisableAll)(uint32 ClientID, DalDeviceHandle * _h);

};

typedef struct DalTFunnelHandle DalTFunnelHandle;
struct DalTFunnelHandle
{
   uint32 dwDalHandleId;
   const DalTFunnel * pVtbl;
   void * pClientCtxt;
};

#define DAL_TFunnelDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTFUNNEL_INTERFACE_VERSION,hDalDevice)

static __inline DALResult
DalTFunnel_Control(DalDeviceHandle * _h, TFunnelControlType  eCtrl,
 TFunnelControlParam *pCtrlParam)
{
   return ((DalTFunnelHandle *)_h)->pVtbl->Control(DALDEVICE_LOCAL_CLIENTID, _h, eCtrl,pCtrlParam);
}

static __inline DALResult
DalTFunnel_DisableAll(DalDeviceHandle * _h)
{
   return ((DalTFunnelHandle *)_h)->pVtbl->DisableAll(DALDEVICE_LOCAL_CLIENTID, _h);
}



#endif

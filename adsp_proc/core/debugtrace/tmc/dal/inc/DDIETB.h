#ifndef DDIETB_H
#define DDIETB_H

//========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.qdsp6/2.1/debugtrace/tmc/dal/inc/DDIETB.h#1 $
//
//========================================================================
//             Copyright (c) 2011 Qualcomm Technologies Incorporated.
//                    All Rights Reserved.
//                    QUALCOMM Proprietary
//========================================================================

#include "DalDevice.h"

#define DALETB_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

typedef enum
{
        ETB_ID,
        ETB_RAM_DEPTH,
        ETB_RAM_WIDTH,
        ETB_STATUS,
        ETB_READ_PTR,
        ETB_WRITE_PTR,
        ETB_TRIGGER_CTR,
   _PLACEHOLDER_ETBParamType = 0x7fffffff
} ETBParamType;



                     
typedef struct DalETB DalETB;
struct DalETB
{
   struct DalDevice DalDevice;
   DALResult (*EnableTrace)(DalDeviceHandle * _h);
   DALResult (*DisableTrace)(DalDeviceHandle * _h);
   DALResult (*ReadTrace)(DalDeviceHandle * _h, uint32  addr, byte * buf, int  nBufLen, int * pnLenRead);
   DALResult (*Flush)(DalDeviceHandle * _h, int  flags);
   DALResult (*GetParam)(DalDeviceHandle * _h, ETBParamType  param, uint32  uVal);
   DALResult (*SetParam)(DalDeviceHandle * _h, ETBParamType  param, uint32  uVal);
};

typedef struct DalETBHandle DalETBHandle; 
struct DalETBHandle 
{
   uint32 dwDalHandleId;
   const DalETB * pVtbl;
   void * pClientCtxt;
};

#define DAL_ETBDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALETB_INTERFACE_VERSION,hDalDevice)

static __inline DALResult
DalETB_EnableTrace(DalDeviceHandle * _h)
{
   return ((DalETBHandle *)_h)->pVtbl->EnableTrace( _h);
}


static __inline DALResult
DalETB_DisableTrace(DalDeviceHandle * _h)
{
   return ((DalETBHandle *)_h)->pVtbl->DisableTrace( _h);
}


static __inline DALResult
DalETB_ReadTrace(DalDeviceHandle * _h, uint32  addr, byte * buf, int  nBufLen, int * pnLenRead)
{
   return ((DalETBHandle *)_h)->pVtbl->ReadTrace( _h, addr, buf, nBufLen, pnLenRead);
}

#define ETB_FLUSH_NOBLOCK 0
#define ETB_FLUSH_BLOCK   1

static __inline DALResult
DalETB_Flush(DalDeviceHandle * _h, int  flags)
{
   return ((DalETBHandle *)_h)->pVtbl->Flush( _h, flags);
}


static __inline DALResult
DalETB_GetParam(DalDeviceHandle * _h, ETBParamType  param, uint32  uVal)
{
   return ((DalETBHandle *)_h)->pVtbl->GetParam( _h, param, uVal);
}


static __inline DALResult
DalETB_SetParam(DalDeviceHandle * _h, ETBParamType  param, uint32  uVal)
{
   return ((DalETBHandle *)_h)->pVtbl->SetParam( _h, param, uVal);
}


#endif

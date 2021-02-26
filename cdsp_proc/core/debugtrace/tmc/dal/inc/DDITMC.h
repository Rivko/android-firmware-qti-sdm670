#ifndef DDITMC_H
#define DDITMC_H

/*=============================================================================

FILE:         DDITMC.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/


#include "DalDevice.h"

#define DALTMC_STRING_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

#define TMC_MODE_CIRCULAR_BUFFER  0x0
#define TMC_MODE_SW_FIFO          0x1
#define TMC_MODE_HW_FIFO          0x2


typedef struct DalTMC DalTMC;
struct DalTMC
{
   struct DalDevice DalDevice;
   DALResult (*GetMode)(uint32 ClientHandle, DalDeviceHandle *_h,uint8 *pMode);
   DALResult (*SetMode)(uint32 ClientHandle, DalDeviceHandle *_h,uint8 mode);
   DALResult (*GetTraceBuffer)(uint32 ClientHandle, DalDeviceHandle *_h,uint32 *pdwAddr, uint32 *pdwBufLen);
   DALResult (*SetTraceBuffer)(uint32 ClientHandle, DalDeviceHandle *_h,uint32 dwAddr, uint32 dwBufLen);

   DALResult (*EnableTrace)(uint32 ClientHandle, DalDeviceHandle *_h);
   DALResult (*DisableTrace)(uint32 ClientHandle, DalDeviceHandle *_h);
   DALResult (*ReadTrace)(uint32 ClientHandle, DalDeviceHandle *_h, byte * buf, uint32  nBufLen, uint32 * pnLenRead);
   DALResult (*Flush)(uint32 ClientHandle, DalDeviceHandle *_h, int  flags);
   DALResult (*GetParam)(uint32 ClientHandle, DalDeviceHandle *_h, uint32  param, uint32  *uVal);
   DALResult (*SetParam)(uint32 ClientHandle, DalDeviceHandle *_h, uint32  param, uint32  uVal);
   DALResult (*SaveRegisters)(uint32 ClientHandle, DalDeviceHandle *_h, byte *buf, uint32 nBufLen);
};

typedef struct DalTMCHandle DalTMCHandle;
struct DalTMCHandle
{
   uint32 dwDalHandleId;
   const DalTMC * pVtbl;
   void * pClientCtxt;
};

#define DAL_TMCDeviceAttach(DevId,hDalDevice)\
        DAL_StringDeviceAttachEx(NULL,DevId,DALTMC_STRING_INTERFACE_VERSION,hDalDevice)


static __inline DALResult
DalTMC_GetMode(DalDeviceHandle * _h,uint8 *pMode)
{
   return ((DalTMCHandle *)_h)->pVtbl->GetMode(DALDEVICE_LOCAL_CLIENTID, _h,pMode);
}

static __inline DALResult
DalTMC_SetMode(DalDeviceHandle * _h,uint8 mode)
{
   return ((DalTMCHandle *)_h)->pVtbl->SetMode(DALDEVICE_LOCAL_CLIENTID, _h , mode);
}

static __inline DALResult
DalTMC_GetTraceBuffer(DalDeviceHandle * _h,uint32 *pdwAddr, uint32 *pdwBufLen)
{
   return ((DalTMCHandle *)_h)->pVtbl->GetTraceBuffer(DALDEVICE_LOCAL_CLIENTID, _h ,pdwAddr,pdwBufLen);
}

static __inline DALResult
DalTMC_SetTraceBuffer(DalDeviceHandle * _h,uint32 dwAddr, uint32 dwBufLen)
{
   return ((DalTMCHandle *)_h)->pVtbl->SetTraceBuffer(DALDEVICE_LOCAL_CLIENTID, _h ,dwAddr,dwBufLen);
}



static __inline DALResult
DalTMC_EnableTrace(DalDeviceHandle * _h)
{
   return ((DalTMCHandle *)_h)->pVtbl->EnableTrace(DALDEVICE_LOCAL_CLIENTID, _h );
}


static __inline DALResult
DalTMC_DisableTrace(DalDeviceHandle * _h)
{
   return ((DalTMCHandle *)_h)->pVtbl->DisableTrace(DALDEVICE_LOCAL_CLIENTID, _h );
}


static __inline DALResult
DalTMC_ReadTrace(DalDeviceHandle * _h, byte * buf, uint32  nBufLen, uint32 * pnLenRead)
{
   return ((DalTMCHandle *)_h)->pVtbl->ReadTrace(DALDEVICE_LOCAL_CLIENTID, _h , buf, nBufLen, pnLenRead);
}

#define TMC_FLUSH_NOBLOCK 0
#define TMC_FLUSH_BLOCK   1
#define TMC_FLUSH_STOP    2


static __inline DALResult
DalTMC_Flush(DalDeviceHandle * _h, int  flags)
{
   return ((DalTMCHandle *)_h)->pVtbl->Flush(DALDEVICE_LOCAL_CLIENTID, _h , flags);
}


static __inline DALResult
DalTMC_GetParam(DalDeviceHandle * _h, uint32  param, uint32  *puVal)
{
   return ((DalTMCHandle *)_h)->pVtbl->GetParam(DALDEVICE_LOCAL_CLIENTID, _h , param, puVal);
}


static __inline DALResult
DalTMC_SetParam(DalDeviceHandle * _h, uint32  param, uint32  uVal)
{
   return ((DalTMCHandle *)_h)->pVtbl->SetParam(DALDEVICE_LOCAL_CLIENTID, _h , param, uVal);
}


static __inline DALResult
DalTMC_SaveRegisters(DalDeviceHandle * _h, byte * buf, uint32  nBufLen)
{
   return ((DalTMCHandle *)_h)->pVtbl->SaveRegisters(DALDEVICE_LOCAL_CLIENTID, _h , buf, nBufLen);
}


#endif //DDITMC_H

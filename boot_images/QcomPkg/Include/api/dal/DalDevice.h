/*=============================================================================
  Copyright (c) 2008, 2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

=============================================================================*/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/12/14   aa     Added Function declarations for public API to support String 
                   based devices
 08/12/14   aa     Added history 

=============================================================================*/

#ifndef __DALDEVICE_H__
#define __DALDEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "DALStdDef.h"

#define DALDEVICE_INTERFACE_HANDLE_ID          0xDA1FDA1F
#define DALDEVICE_INVALID_INTERFACE_HANDLE_ID  0xBADBAADD
#define DALINTERFACE_VER_MAJOR_MASK            0xFFFF0000
#define DALINTERFACE_VER_MINOR_MASK            0x0000FFFF

/* macro for retrieving the vtable from an instance of an interface */
#define DALGETPVTBL(p,iname)  (*((DALVTBL(iname) **)((void *)p)))
#define DALVTBLIDX(pVtbl,fcn) ((void**)&((pVtbl)->fcn)-(void**)(pVtbl))
#define DALISREMOTEHANDLE(h)  (((DALHandle)h) & 0x00000001)
#define DALPREPREMOTEHANDLE(h) (h = (DalDeviceHandle *)(((DALHandle)h) & ~0x00000001))
#define DALMARKREMOTEHANDLE(h) ((DalDeviceHandle *)((DALHandle)h | 0x00000001))

#define DALINTERFACE_VERSION(major,minor) (((major&0xFFFF)<<16)|(minor&0xFFFF))


#define DALINTERFACE_GET_MAJOR_VERSION(version)\
        (version&DALINTERFACE_VER_MAJOR_MASK)>>16

#define DALINTERFACE_GET_MINOR_VERSION(version)\
        (version&DALINTERFACE_VER_MINOR_MASK)

#define DALINTERFACE_COMPATIBLE(client, server) \
   ((DALINTERFACE_GET_MAJOR_VERSION(client) == DALINTERFACE_GET_MAJOR_VERSION(server)) && (client <= server))


#define DALDEVICE_POWER_CMD_SUSPEND           101
#define DALDEVICE_POWER_CMD_RESUME            102
#define DALDEVICE_POWER_CMD_POWER_DOWN        103
#define DALDEVICE_POWER_CMD_POWER_UP          104

#define DALDEVICE_POWER_DOMAIN_PLATFORM       1

#define DALDEVICE_SYSREQ_PLUG_AND_PLAY        0x80000001
#define DALDEVICE_SYSREQ_RESET                0x80000002
#define DALDEVICE_SYSREQ_MAP_MEMORY      		 0x80000010
#define DALDEVICE_SYSREQ_UNMAP_MEMORY 	       0x80000012

#define DALFW_SYSREQ_GET_SYSTEM_TABLE          5


typedef struct DalDeviceInfo DalDeviceInfo;
struct DalDeviceInfo
{
   uint32 sizeOfActual;
   uint32 Version;
   char Name[32];
};

typedef struct DalDeviceStatus DalDeviceStatus;
struct DalDeviceStatus
{
   uint32 sizeOfActual;
   uint32 Status;
};

/* use as mask */
#define DAL_OPEN_EXCLUSIVE  0
#define DAL_OPEN_SHARED     1
#define DAL_OPEN_READ       2
#define DAL_OPEN_WRITE      4

typedef struct DalDeviceHandle DalDeviceHandle;
typedef struct DalDevice DalDevice;
struct DalDevice
{
   DALResult (*Attach)(const char*,DALDEVICEID,DalDeviceHandle **);
   uint32    (*Detach)(DalDeviceHandle *);
   DALResult (*Init)(DalDeviceHandle *);
   DALResult (*DeInit)(DalDeviceHandle *);
   DALResult (*Open)(DalDeviceHandle *, uint32);
   DALResult (*Close)(DalDeviceHandle *);
   DALResult (*Info)(DalDeviceHandle *, DalDeviceInfo *, uint32);
   DALResult (*PowerEvent)(DalDeviceHandle *, DalPowerCmd, DalPowerDomain);
   DALResult (*SysRequest)(DalDeviceHandle *, DalSysReq, const void *, uint32,
                           void *,uint32, uint32*);
};
//Users will define interface in the following manner
typedef struct DalInterface DalInterface;
struct DalInterface
{
   struct DalDevice DalDevice;
   //User will add their prototypes here.
};

struct DalDeviceHandle
{
   uint32 dwDalHandleId;
   const DalInterface *pVtbl;
   void         *pClientCtxt;
};

typedef struct DalDeviceHandle * DALDEVICEHANDLE;

typedef struct DalRemoteHandle DalRemoteHandle;
typedef struct DalRemote DalRemote;
struct DalRemote
{
   struct DalDevice DalDevice;
   DALResult (* FCN_0) ( uint32 ddi_idx, DalDeviceHandle *h, uint32 u1 );
   DALResult (* FCN_1) ( uint32 ddi_idx, DalDeviceHandle *h, uint32 u1, uint32 u2 );
   DALResult (* FCN_2) ( uint32 ddi_idx, DalDeviceHandle *h, uint32 u1, uint32* p_u2 );
   DALResult (* FCN_3) ( uint32 ddi_idx, DalDeviceHandle *h, uint32 u1, uint32 u2, uint32 u3 );
   DALResult (* FCN_4) ( uint32 ddi_idx, DalDeviceHandle *h, uint32 u1, uint32 u2, uint32* p_u3 );
   DALResult (* FCN_5) ( uint32 ddi_idx, DalDeviceHandle *h, void * ibuf, uint32 ilen);
   DALResult (* FCN_6) ( uint32 ddi_idx, DalDeviceHandle *h, uint32 u1, void * ibuf, uint32 ilen);
   DALResult (* FCN_7) ( uint32 ddi_idx, DalDeviceHandle *h, void * ibuf, uint32 ilen, void * obuf, uint32 olen, uint32 *oalen);
   DALResult (* FCN_8) ( uint32 ddi_idx, DalDeviceHandle *h, void * ibuf, uint32 ilen, void * obuf, uint32 olen);
   DALResult (* FCN_9) ( uint32 ddi_idx, DalDeviceHandle *h, void * obuf, uint32 olen );
   DALResult (* FCN_10)( uint32 ddi_idx, DalDeviceHandle *h, uint32 u1, void * ibuf, uint32 ilen, void * obuf, uint32 olen, uint32 * oalen);
   DALResult (* FCN_11) ( uint32 ddi_idx, DalDeviceHandle *h, uint32 u1, void * obuf, uint32 olen);
   DALResult (* FCN_12) ( uint32 ddi_idx, DalDeviceHandle *h, uint32 u1, void * obuf, uint32 olen, uint32 *oalen);
   DALResult (* FCN_13) ( uint32 ddi_idx, DalDeviceHandle *h, void * ibuf, uint32 ilen, void * ibuf2, uint32 ilen2, void * obuf, uint32 olen);
   DALResult (* FCN_14) ( uint32 ddi_idx, DalDeviceHandle *h, void * ibuf, uint32 ilen, void * obuf1, uint32 olen, void * obuf2, uint32 olen2, uint32 * oalen);
   DALResult (* FCN_15) ( uint32 ddi_idx, DalDeviceHandle *h, void * ibuf, uint32 ilen, void * ibuf2, uint32 ilen2, void * obuf2, uint32 olen2, uint32 *oalen, void * obuf, uint32 olen );
};

struct DalRemoteHandle
{
   uint32 dwDalHandleId;
   const DalRemote *pVtbl;
   void      *pClientCtxt;
};

/*
   Since the Remote stub driver also inherits IDevice we simply call either it
   or handle directly.
*/

static __inline uint32
DalDevice_Detach(DalDeviceHandle *_h)
{
   DALPREPREMOTEHANDLE(_h);
   return _h->pVtbl->DalDevice.Detach(_h);
}

static __inline DALResult
DalDevice_Init(DalDeviceHandle *_h)
{
   DALPREPREMOTEHANDLE(_h);
   return _h->pVtbl->DalDevice.Init(_h);
}

static __inline DALResult
DalDevice_DeInit(DalDeviceHandle *_h)
{
   DALPREPREMOTEHANDLE(_h);
   return _h->pVtbl->DalDevice.DeInit(_h);
}

static __inline DALResult
DalDevice_PowerEvent(DalDeviceHandle *_h, DalPowerCmd PowerCmd,
                     DalPowerDomain PowerDomain)
{
   DALPREPREMOTEHANDLE(_h);
   return _h->pVtbl->DalDevice.PowerEvent(_h,PowerCmd,PowerDomain);
}

static __inline DALResult
DalDevice_Open(DalDeviceHandle *_h, uint32 mode)
{
   DALPREPREMOTEHANDLE(_h);
   return _h->pVtbl->DalDevice.Open(_h,mode);
}

static __inline DALResult
DalDevice_Close(DalDeviceHandle *_h)
{
   DALPREPREMOTEHANDLE(_h);
   return _h->pVtbl->DalDevice.Close(_h);
}

static __inline DALResult
DalDevice_Info(DalDeviceHandle *_h, DalDeviceInfo* info, uint32 infoSize)
{
   DALPREPREMOTEHANDLE(_h);
   return _h->pVtbl->DalDevice.Info(_h,info,infoSize);
}

static __inline DALResult
DalDevice_SysRequest(DalDeviceHandle *_h, DalSysReq ReqIdx,
                     const unsigned char* SrcBuf, int SrcBufLen,
                     unsigned char* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   DALPREPREMOTEHANDLE(_h);
   return _h->pVtbl->DalDevice.SysRequest(_h,ReqIdx,SrcBuf,SrcBufLen,
                                          DestBuf,DestBufLen,DestBufLenReq);
}

/*
  @brief Establish communication link with the desired device

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device

  @param DeviceId : Desired Device Id
         phDevice : Return parameter, contains DAL Device Handle, if return
                    code is DAL_SUCCESS
  @see DALSYS_GetEnv
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DAL_DeviceAttach(DALDEVICEID DeviceId,
                 DalDeviceHandle **phDevice);

/*
  @brief Establish communication link with the desired device, only local modules
         are searched

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device

  @param pazArg   : Arg to be passed to the driver
         DeviceId : Desired Device Id
         phDevice : Return parameter, contains DAL Device Handle, if return
                    code is DAL_SUCCESS
  @see DALSYS_GetEnv
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DAL_DeviceAttachLocal(const char *pszArg,DALDEVICEID DeviceId,
                      DalDeviceHandle **phDevice);

/*
  @brief Establish communication link with the desired device

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device.
  This function will also search for drivers remotely.

  @param pszArg   : User Argument
         DeviceId : Desired Device Id
         ClientVersion: Version from the Iterface Header file for this interface
         phDevice : Return parameter, contains DAL Device Handle, if return
                    code is DAL_SUCCESS
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DAL_DeviceAttachEx(const char *pszArg, DALDEVICEID DeviceId,
               DALInterfaceVersion ClientVersion,DalDeviceHandle **phDevice);

/*
  @brief Establish communication link with the desired device

  Similar to DALDeviceAttachEx, except that it will only search in remote cores.
*/
DALResult
DAL_DeviceAttachRemote(const char *pszArg,
                   DALDEVICEID DevId,
                   DALInterfaceVersion ClientVersion,
                   DalDeviceHandle **phDALDevice);

/*
  @brief Relinquish communication link with the desired device

  User(s)/client(s) of DAL drivers must use this API to release the
  desired device. Upon successful invocation of the API the Client will have
  successful relinquished communication link with the desired device

  @param hDevice : DAL Device Handle

  @see DALSYS_DeviceAttach
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DAL_DeviceDetach(DalDeviceHandle *hDevice);

/*
  @brief Establish communication link with the desired device, only local modules
         are searched

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device

  @param pazArg     : Arg to be passed to the driver
         pszDevName : Desired Device Id (String Device ID)
         phDevice   : Return parameter, contains DAL Device Handle, if return 
                      code is DAL_SUCCESS
  @see DALSYS_GetEnv
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DAL_StringDeviceAttachLocal(const char *pszArg, const char *pszDevName, 
                           DalDeviceHandle **phDalDevice);

/*
  @brief Establish communication link with the desired device

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device

  @param pszDevName : Desired Device Id (String Device ID)
         phDevice   : Return parameter, contains DAL Device Handle, if return
                      code is DAL_SUCCESS
  @see DALSYS_GetEnv
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DAL_StringDeviceAttach(const char *pszDevName, DalDeviceHandle **phDalDevice);

/*
  @brief Establish communication link with the desired device

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device.
  This function will also search for drivers remotely.

  @param pszArg     : User Argument
         pszDevName : Desired Device Id (String Device ID)
         ClientVersion: Version from the Iterface Header file for this interface
         phDevice : Return parameter, contains DAL Device Handle, if return
                    code is DAL_SUCCESS
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DAL_StringDeviceAttachEx(const char *pszArg,
      const char *pszDevName,
      DALInterfaceVersion ClientVersion,
      DalDeviceHandle **phDALDevice); 
      
#ifdef __cplusplus
}
#endif

#endif

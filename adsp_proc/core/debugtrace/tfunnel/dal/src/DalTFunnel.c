
/*=============================================================================

FILE:         DalTFunnel.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/


#include <halqdss_tfunnel.h>
#include "DALFramework.h"
#include "DalDevice.h"
#include "DDITFunnel.h"
#include "DALDeviceId.h"
#include "string.h" //for memset warning

#define TFUNNEL_CLAIMTAG_RIGHTS  0x1


/*------------------------------------------------------------------------------
Declaring a "TFunnel" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct TFunnelDrvCtxt TFunnelDrvCtxt;
typedef struct TFunnelDevCtxt TFunnelDevCtxt;
typedef struct TFunnelClientCtxt TFunnelClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "TFunnel" Vtable
------------------------------------------------------------------------------*/
typedef struct TFunnelDALVtbl TFunnelDALVtbl;
struct TFunnelDALVtbl
{
   int (*TFunnel_DriverInit)(TFunnelDrvCtxt *);
   int (*TFunnel_DriverDeInit)(TFunnelDrvCtxt *);
};

struct TFunnelDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   TFunnelDrvCtxt  *pTFunnelDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //TFunnel Dev state can be added by developers here
   uint32 nNumPorts;
   DALSYSMemHandle hMem;
};

struct TFunnelDrvCtxt
{
   //Base Members
   TFunnelDALVtbl TFunnelDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   TFunnelDevCtxt TFunnelDevCtxt[1];
   //TFunnel Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "TFunnel" Client Context
------------------------------------------------------------------------------*/
struct TFunnelClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   TFunnelDevCtxt *pTFunnelDevCtxt;
   DalTFunnelHandle DalTFunnelHandle;
   //TFunnel Client state can be added by developers here
};

/*Funnel port configuration structure*/
typedef struct TypeTFunnelPortEnableConfig TFunnelPortEnableConfig;

/*
This structure defines the topology of a funnel configuration.
*/
struct TypeTFunnelPortEnableConfig{
   uint32 funnel_id;
   uint32 port_id;
   TFunnelPortEnableConfig *pFunnelConfig;
};




/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

DALResult
TFunnel_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);


static void TFunnel_AccessGrant(uint32 nFunnelId)
{
   HAL_qdss_tfunnel_UnlockAccess(nFunnelId);
}

static void TFunnel_AccessRevoke(uint32 nFunnelId)
{
   HAL_qdss_tfunnel_LockAccess(nFunnelId);
}

/*
Description:
   Initialize the claim tag b'0 field to 1 to indicate access is available.
This field acts as binary semaphore.

Device reset State: 0x0
*/
void TFunnel_ClaimTagInit(uint32 nFunnelId)
{
   HAL_qdss_tfunnel_UnlockAccess(nFunnelId);
   HAL_qdss_tfunnel_ClrClaimTag(HAL_qdss_tfunnel_ClaimTagImplementMask(nFunnelId),
            nFunnelId);
   HAL_qdss_tfunnel_SetClaimTag(TFUNNEL_CLAIMTAG_RIGHTS,nFunnelId);
   HAL_qdss_tfunnel_LockAccess(nFunnelId);
}

/*Enable a specified funnel*/
static void TFunnel_PortEnable(TFunnelPortEnableConfig *pPortConfig);
static void TFunnel_PortDisable(TFunnelPortEnableConfig *pPortConfig);
static boolean TFunnel_PortStatus(TFunnelPortEnableConfig *pPortConfig);
static void TFunnel_SetPortPriority(TFunnelPortEnableConfig
*pstrPortConfig,uint32 PortPri);





/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/
static DALDEVICEID DalTFunnel_DeviceId[1] = {DALDEVICEID_TFUNNEL_0};

const DALREG_DriverInfo
DALTFunnel_DriverInfo = { TFunnel_DeviceAttach, 1, DalTFunnel_DeviceId };


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32
TFunnel_AddRef(DalTFunnelHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
TFunnel_DriverInit(TFunnelDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
TFunnel_DriverDeInit(TFunnelDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
TFunnel_DeviceDetach(uint32 ClientId, DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult
TFunnel_DeviceInit(uint32 ClientId, DalDeviceHandle *h)
{
   TFunnelDevCtxt *pDevCtxt = (TFunnelDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   DALSYSPropertyVar PropVar;
   DALSYSMemInfo memInfo;
   uint32 HalDevId,NumFunnels;
   DALResult ret;
   uint32 pTable,*pFunnelhysAddr;
   uint32 i;

/*
*TBD:Ability to map a HAL-Funnel ID to DAL Device ID. Ideally all funnels must
be identified by ONE DAL Device Id. Coresight device enumeration would then
identify and enable all the funnels.

Since coresight cotains its own mechanism for enumeration, from DAL's
perspective all funnels are identified by a single device id. Moreover
enumerating coresight components in DAL would be a violation of coresight
device enumeration.

Once enabled funnel ports remain enabled. Trace sources flowing into the sink
need to be controlled by enabling or disabling the source NOT the funnel-port.
*/
   HalDevId = pDevCtxt->DevId - DALDEVICEID_TFUNNEL_0;

   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle( pDevCtxt->DevId,
                                                    pDevCtxt->hProp ))
       return DAL_ERROR;


   if (DAL_SUCCESS!=DALSYS_GetPropertyValue( pDevCtxt->hProp,"num_tfunnels",0,&PropVar ))
         return DAL_ERROR;

   NumFunnels = PropVar.Val.dwVal;
   HAL_qdss_tfunnel_NumFunnels(NumFunnels);

   ret = DALSYS_Malloc(NumFunnels*sizeof(uint32),(void **)&pTable);
   if (ret!=DAL_ERROR)
   {
      HAL_qdss_tfunnel_MemAllocTable(pTable);
   }
   else
   {
      return DAL_ERROR;
   }


   if (DAL_SUCCESS!=DALSYS_GetPropertyValue( pDevCtxt->hProp,"tfunnel_phys_addrs",0,&PropVar ))
      return DAL_ERROR;

   pFunnelhysAddr = PropVar.Val.pdwVal;

   /*
   Number of entries of funnel addresses specified in "tfunnel_phys_addr_arr"
   should correpond to the value in "num_tfunnels".
   */
   if(NumFunnels!=PropVar.dwLen)
   {
      return DAL_ERROR;
   }

   for(i=0;i<PropVar.dwLen;i++) /*iterate through length of the sequence*/
   {
      pDevCtxt->hMem=NULL;
      if(DAL_ERROR==DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO, DALSYS_MEM_ADDR_NOT_SPECIFIED,
                           pFunnelhysAddr[i],4096, &pDevCtxt->hMem, NULL ))
      {
         return DAL_ERROR;
      }
      else
      {
         HalDevId = i;
         DALSYS_MemInfo(pDevCtxt->hMem, &memInfo);
         HAL_qdss_tfunnel_HalInit(memInfo.VirtualAddr,HalDevId);
         pDevCtxt->hMem=NULL;/*Use the same handle for subsequent device pages*/
      }

   }

   return DAL_SUCCESS;
}

static DALResult
TFunnel_DeviceDeInit(uint32 ClientId, DalDeviceHandle *h)
{
   TFunnelDevCtxt *pDevCtxt = (TFunnelDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   uint32 HalDevId;

   HalDevId = pDevCtxt->DevId - DALDEVICEID_TFUNNEL_0;

   TFunnel_AccessGrant(HalDevId);
   HAL_qdss_tfunnel_DisableAllPorts(HalDevId);
   TFunnel_AccessRevoke(HalDevId);

   return DAL_SUCCESS;
}

static DALResult
TFunnel_DevicePowerEvent(uint32 ClientId, DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
TFunnel_DeviceOpen(uint32 ClientId, DalDeviceHandle* h, uint32 mode)
{
   return DAL_SUCCESS;
}

static DALResult
TFunnel_DeviceClose(uint32 ClientId, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
TFunnel_DeviceInfo(uint32 ClientId, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTFUNNEL_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
TFunnel_DeviceSysRequest(uint32 ClientId, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTFunnel Interface.
------------------------------------------------------------------------------*/

static DALResult
TFunnel_DeviceControl(uint32 ClientId,  DalDeviceHandle *h, TFunnelControlType  eCtrl,
TFunnelControlParam *pCtrlParam)
{
   TFunnelDevCtxt *pDevCtxt = (TFunnelDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   DALSYSPropertyVar PropVar;
   DALResult ret=DAL_SUCCESS;


   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle( pDevCtxt->DevId,
                                                   pDevCtxt->hProp ))
      return DAL_ERROR;

  if (DAL_SUCCESS != DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                              pCtrlParam->pPortStr,
                                              0,
                                              &PropVar ))
   {
       return DAL_ERROR;
   }

   //Implementation
   switch(eCtrl)
   {
      case TFUNNEL_PORT_ENABLE:
         if(pCtrlParam->Val.bEnable)
         {
            TFunnel_PortEnable((TFunnelPortEnableConfig *)PropVar.Val.pStruct);
         }
         else
         {
            TFunnel_PortDisable((TFunnelPortEnableConfig *)PropVar.Val.pStruct);
         }
      break;

      case TFUNNEL_PORT_PRIORITY:
         TFunnel_SetPortPriority((TFunnelPortEnableConfig
         *)PropVar.Val.pStruct,pCtrlParam->Val.nPrio);
      break;

      case TFUNNEL_PORT_STATUS:
         pCtrlParam->Val.bEnable =
         TFunnel_PortStatus((TFunnelPortEnableConfig *)PropVar.Val.pStruct);

      default:
         ret=DAL_ERROR;

   }

   return ret;
}

static DALResult
TFunnel_DeviceDisableAll(uint32 ClientHandle, DalDeviceHandle *h)
{
   HAL_qdss_tfunnel_UnlockAccessAll();
   HAL_qdss_tfunnel_DisableAllPortsAllFunnels();
   HAL_qdss_tfunnel_LockAccessAll();
   return DAL_SUCCESS;
}

static void
TFunnel_InitInterface(TFunnelClientCtxt* pclientCtxt)
{
   static const DalTFunnel vtbl =
   {
      {
         TFunnel_DeviceAttach,
         TFunnel_DeviceDetach,
         TFunnel_DeviceInit,
         TFunnel_DeviceDeInit,
         TFunnel_DeviceOpen,
         TFunnel_DeviceClose,
         TFunnel_DeviceInfo,
         TFunnel_DevicePowerEvent,
         TFunnel_DeviceSysRequest
      },

      TFunnel_DeviceControl,
      TFunnel_DeviceDisableAll

   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalTFunnelHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalTFunnelHandle.pVtbl  = &vtbl;
   pclientCtxt->DalTFunnelHandle.pClientCtxt = pclientCtxt;
}

DALResult
TFunnel_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static TFunnelDrvCtxt drvCtxt = {{TFunnel_DriverInit,
                                      TFunnel_DriverDeInit
                                     },1,
                                     sizeof(TFunnelDevCtxt)};
   static TFunnelClientCtxt clientCtxt;

   TFunnelClientCtxt *pclientCtxt = &clientCtxt;


   *phDalDevice = NULL;

   nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      TFunnel_InitInterface(pclientCtxt);
      TFunnel_AddRef(&(pclientCtxt->DalTFunnelHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalTFunnelHandle);
   }
   return nErr;
}

/*
When enabling ports enable all the way through the topology of
inter-connected funnels
*/
static void TFunnel_PortEnable(TFunnelPortEnableConfig *pPortConfig)
{
   while(pPortConfig!=NULL)
   {
      TFunnel_AccessGrant(pPortConfig->funnel_id);

      HAL_qdss_tfunnel_EnablePort(pPortConfig->port_id,pPortConfig->funnel_id);

      TFunnel_AccessRevoke(pPortConfig->funnel_id);

      pPortConfig=pPortConfig->pFunnelConfig;
   }
}

static void TFunnel_PortDisable(TFunnelPortEnableConfig *pPortConfig)
{
   if(pPortConfig!=NULL)
   {
      TFunnel_AccessGrant(pPortConfig->funnel_id);

      HAL_qdss_tfunnel_DisablePort(pPortConfig->port_id,pPortConfig->funnel_id);

      TFunnel_AccessRevoke(pPortConfig->funnel_id);
   }
}

static boolean TFunnel_PortStatus(TFunnelPortEnableConfig *pPortConfig)
{
   boolean ret=TRUE;
   while(pPortConfig!=NULL)
   {
/*
Scan port status through the entire funnel chain. If either port in the chain is
disabled the function returns a port status of DISABLE.
*/
      ret= ret & HAL_qdss_tfunnel_isPortEnabled(pPortConfig->port_id,pPortConfig->funnel_id);
      pPortConfig=pPortConfig->pFunnelConfig;
   }
   return ret;
}

static void TFunnel_SetPortPriority(TFunnelPortEnableConfig
*pstrPortConfig,uint32 PortPri)
{
   if(pstrPortConfig!=NULL)
   {
      TFunnel_AccessGrant(pstrPortConfig->funnel_id);

      HAL_qdss_tfunnel_SetPortPriority(pstrPortConfig->port_id,PortPri,pstrPortConfig->funnel_id);

      TFunnel_AccessRevoke(pstrPortConfig->funnel_id);
   }

}


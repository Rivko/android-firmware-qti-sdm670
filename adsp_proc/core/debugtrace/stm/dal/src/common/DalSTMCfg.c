/*=============================================================================
  Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
           All Rights Reserved.

=============================================================================*/

#include "halqdss_stm_config.h"
#include "DALFramework.h"
#include "DalDevice.h"
#include "DDISTMCfg.h"
#include "qdss_utils.h"

/*------------------------------------------------------------------------------
Declaring a "STMCfg" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct STMCfgDrvCtxt STMCfgDrvCtxt;
typedef struct STMCfgDevCtxt STMCfgDevCtxt;
typedef struct STMCfgClientCtxt STMCfgClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "STMCfg" Vtable
------------------------------------------------------------------------------*/
typedef struct STMCfgDALVtbl STMCfgDALVtbl;
struct STMCfgDALVtbl
{
   int (*STMCfg_DriverInit)(STMCfgDrvCtxt *);
   int (*STMCfg_DriverDeInit)(STMCfgDrvCtxt *);
};


struct STMCfgDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   STMCfgDrvCtxt  *pSTMCfgDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //STMCfg Dev state can be added by developers here
   uint32 claimTag;
   DALSYSMemHandle hMem;
   DALSYSMemInfo memInfo;
   uint32 atid;
   uint32 syncPeriod;

};

struct STMCfgDrvCtxt
{
   //Base Members
   STMCfgDALVtbl STMCfgDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   STMCfgDevCtxt STMCfgDevCtxt[1];
   //STMCfg Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "STMCfg" Client Context
------------------------------------------------------------------------------*/
struct STMCfgClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   STMCfgDevCtxt *pSTMCfgDevCtxt;
   DalSTMCfgHandle DalSTMCfgHandle;
   //STMCfg Client state can be added by developers here
};

static void _AccessGrant(STMCfgDevCtxt *pDevCtxt) 
{
   HAL_qdss_stm_HalConfigInit(pDevCtxt->memInfo.VirtualAddr);
   HAL_qdss_stm_AccessUnLock();
}

static void _AccessRevoke(STMCfgDevCtxt *pDevCtxt)
{
   HAL_qdss_stm_AccessLock();
}

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
STMCfg_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/

const DALREG_DriverInfo
DALSTMCfg_DriverInfo = { STMCfg_DeviceAttach, 0, NULL };



/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32
STMCfg_AddRef(DalSTMCfgHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
STMCfg_DriverInit(STMCfgDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
STMCfg_DriverDeInit(STMCfgDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
STMCfg_DeviceDetach(uint32 ClientId, DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

#define SSC_STM_ATID        23
uint32 qdss_get_ssc_tsctr_freq(void);

static DALResult
STMCfg_DeviceInit(uint32 ClientId, DalDeviceHandle *h)
{
   int nErr = DAL_ERROR;
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   HAL_qdss_stm_PortSelRet portSel;
   DALSYSPropertyVar PropVar;
   boolean nsnid_status;


   TRY(nErr,DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                     "stm_claim_tag",
                                     0,
                                     &PropVar ));

   pDevCtxt->claimTag = PropVar.Val.dwVal;



   TRY(nErr,DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                     "stm_atid",
                                     0,
                                     &PropVar ));

   pDevCtxt->atid = PropVar.Val.dwVal;


   TRY(nErr,DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                     "stm_sync_period",
                                     0,
                                     &PropVar ));

   pDevCtxt->syncPeriod = PropVar.Val.dwVal;

   //validate
   if (pDevCtxt->syncPeriod < 64) {
      pDevCtxt->syncPeriod=64;
   }
   else if (pDevCtxt->syncPeriod > 4095) {
      pDevCtxt->syncPeriod=4095;
   }



   TRY(nErr,DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                     "stm_phys_addr",
                                     0,
                                     &PropVar ));

   TRY(nErr,DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  PropVar.Val.dwVal,
                                  4096, &pDevCtxt->hMem, NULL ));

   DALSYS_MemInfo(pDevCtxt->hMem, &pDevCtxt->memInfo);

   HAL_qdss_stm_HalConfigInit(pDevCtxt->memInfo.VirtualAddr);

   _AccessGrant(pDevCtxt);

   if(!HAL_qdss_stm_isEnabled())
   {
      HAL_qdss_stm_MasterSelectCfg(0, MASTER_SELECT_CTL_ALL);

      HAL_qdss_stm_PortEnableMask(0xFFFFFFFF);

      HAL_qdss_stm_PortSelectCfg(0, 0, PORT_SELECT_CTL_SELECT_ALL, &portSel);

      HAL_qdss_stm_TimeStampEnable();

      HAL_qdss_stm_TraceIdCfg(pDevCtxt->atid);

      if (SSC_STM_ATID == pDevCtxt->atid) {
         uint32 ts_ctr_freq = qdss_get_ssc_tsctr_freq();
         HAL_qdss_stm_SetTimeStampFreq(ts_ctr_freq);
      }

      /*Generate ATID 0x7D for writes to TRIG location*/      
      HAL_qdss_stm_TrigGenTrigLocation();

      HAL_qdss_stm_SyncPeriodCfg(STM_SYNC_MODE_PERIOD_N_BYTES,pDevCtxt->syncPeriod);
      
      HAL_qdss_stm_CompDisable();

      HAL_qdss_stm_SetAuxCR();
   } 
   _AccessRevoke(pDevCtxt);
   nsnid_status=HAL_qdss_stm_AuthStatusNSNID();
   nErr = (nsnid_status==TRUE)?DAL_SUCCESS:DAL_ERROR_DEVICE_ACCESS_DENIED;   
   CATCH(nErr) {}

   return nErr;
}

static DALResult
STMCfg_DeviceDeInit(uint32 ClientId, DalDeviceHandle *h)
{
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   
   if(pDevCtxt->hMem) {
      DALSYS_DestroyObject(pDevCtxt->hMem);
   }

   return DAL_SUCCESS;
}

static DALResult
STMCfg_DevicePowerEvent(uint32 ClientId, DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
STMCfg_DeviceOpen(uint32 ClientId, DalDeviceHandle* h, uint32 mode)
{
   return DAL_SUCCESS;
}

static DALResult
STMCfg_DeviceClose(uint32 ClientId, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
STMCfg_DeviceInfo(uint32 ClientId, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALSTMCFG_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
STMCfg_DeviceSysRequest(uint32 ClientId, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalSTMCfg Interface.
------------------------------------------------------------------------------*/

static DALResult
STMCfg_STMControl(uint32 ClientId,  DalDeviceHandle * h, STMControlType  uCtrl, uint32  uVal)
{
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   DALResult nErr = DAL_SUCCESS;

   _AccessGrant(pDevCtxt);

   switch(uCtrl)
   {
      case STM_EN:
         if(uVal) {
            HAL_qdss_stm_Enable();
         }
         else {
            HAL_qdss_stm_Disable();
         }
         break;

      case STM_SW_TRACE_EN:
         if(uVal)
            HAL_qdss_stm_PortEnableMask(0xFFFFFFFF);
         else
            HAL_qdss_stm_PortEnableMask(0x0);
         break;

   case STM_HW_EVT_EN:
         if(uVal) {
            HAL_qdss_stm_HWEventEnable();
         }
         else {
            HAL_qdss_stm_HWEventDisable();
         }
         break;



      case STM_TIMESTAMP_EN:
         if(uVal)
            HAL_qdss_stm_TimeStampEnable();
         else
            HAL_qdss_stm_TimeStampDisable();
         break;

      case STM_TIMESTAMP_FREQ:
         HAL_qdss_stm_SetTimeStampFreq(uVal);
         break;

      case STM_TIMESTAMP_FREQ_PKTGEN:
         if(uVal)
         {  HAL_qdss_stm_SetTimeStampFreq(HAL_qdss_stm_GetTimeStampFreq());
         }
         break;

      case STM_COMPRESSION_EN:
         if(uVal)
         {
            HAL_qdss_stm_CompEnable();
         }
         else
         {
            HAL_qdss_stm_CompDisable();
         }
      break;

      case STM_SYNC_EN:
         if(uVal)
         {
            HAL_qdss_stm_SyncEnable();
         }
         else
         {
            HAL_qdss_stm_SyncDisable();
         }
         break;

      case STM_SYNC_PERIOD_N:
       if(uVal)
       {
         HAL_qdss_stm_SyncPeriodCfg(STM_SYNC_MODE_PERIOD_N_BYTES,uVal);
       }
       break;

       case STM_SYNC_PERIOD_2POW_N:
        if(uVal)
        {
          HAL_qdss_stm_SyncPeriodCfg(STM_SYNC_MODE_PERIOD_2_POWER_N_BYTES,uVal);
        }
         break;

       case STM_HW_EVT_ENABLE_MASK:
         HAL_qdss_stm_HWEventEnableMask(uVal);
         break;

       case STM_HW_EVT_TRIGGER_MASK:
          HAL_qdss_stm_HWEventTriggerSetMask(uVal);
          break;
      default:
         nErr = DAL_ERROR;
   }

   _AccessRevoke(pDevCtxt);
   return nErr;
}


static DALResult
STMCfg_STMStatus(uint32 ClientHandle, DalDeviceHandle * h, STMControlType  uCtrl, uint32  *puVal)
{
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   DALResult nErr = DAL_SUCCESS;

   _AccessGrant(pDevCtxt);

   switch(uCtrl)
      {
      case STM_EN:
         if (HAL_qdss_stm_isEnabled()) {
            *puVal=1;
         }
         else {
            *puVal=0;
         }
         break;

      case STM_HW_EVT_EN:
         if (HAL_qdss_stm_HWevent_isEnabled()) {
            *puVal=1;
         }
         else {
            *puVal=0;
         }
         break;

      default:
         nErr = DAL_ERROR;
      }

   _AccessRevoke(pDevCtxt);
   return nErr;
}




static void
STMCfg_InitInterface(STMCfgClientCtxt* pclientCtxt)
{
   static const DalSTMCfg vtbl =
   {
      {
         STMCfg_DeviceAttach,
         STMCfg_DeviceDetach,
         STMCfg_DeviceInit,
         STMCfg_DeviceDeInit,
         STMCfg_DeviceOpen,
         STMCfg_DeviceClose,
         STMCfg_DeviceInfo,
         STMCfg_DevicePowerEvent,
         STMCfg_DeviceSysRequest
      },

      STMCfg_STMControl,
      STMCfg_STMStatus,
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalSTMCfgHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalSTMCfgHandle.pVtbl  = &vtbl;
   pclientCtxt->DalSTMCfgHandle.pClientCtxt = pclientCtxt;
}



DALResult
STMCfg_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr = DAL_ERROR;
   STMCfgClientCtxt *pclientCtxt =NULL;
   STMCfgDrvCtxt *pdrvCtxt = NULL;
   const char *pszDev_SLPI="DALDEVICEID_STM_CONFIG_SLPI";

   static STMCfgDrvCtxt drvCtxt = {{STMCfg_DriverInit,
                                    STMCfg_DriverDeInit
      },1,
                                   sizeof(STMCfgDevCtxt)};
   static STMCfgClientCtxt clientCtxt;


   static STMCfgDrvCtxt drvCtxt_SLPI = {{STMCfg_DriverInit,
                                         STMCfg_DriverDeInit
      },1,
                                        sizeof(STMCfgDevCtxt)};
   static STMCfgClientCtxt clientCtxt_SLPI;


   *phDalDevice = NULL;

   if (0==strncmp(pszArg,pszDev_SLPI,strlen(pszDev_SLPI))) {
      pdrvCtxt = &drvCtxt_SLPI;
      pclientCtxt = &clientCtxt_SLPI;
   }
   else {
      pdrvCtxt = &drvCtxt;
      pclientCtxt = &clientCtxt;
   }


   TRY(nErr,DALFW_AttachToStringDevice(pszArg,(DALDrvCtxt *)pdrvCtxt,
                                       (DALClientCtxt *)pclientCtxt));

   STMCfg_InitInterface(pclientCtxt);
   STMCfg_AddRef(&(pclientCtxt->DalSTMCfgHandle));
   TRY(nErr,DALSYS_GetDALPropertyHandleStr(pszArg,
                                           pclientCtxt->pSTMCfgDevCtxt->hProp));

   *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalSTMCfgHandle);

   nErr = DAL_SUCCESS;

   CATCH(nErr) {}
   return nErr;
}

/*=============================================================================
  Copyright (c) 2011-2012 QUALCOMM Technologies Incorporated.
           All Rights Reserved.

$Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/hwevent/dal/src/DalHWEvent.c#1 $
=============================================================================*/

#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDIHWEvent.h"
#include "DDIHWIO.h"
#include "HALhwio.h"
#include "npa.h"
#include "ClockDefs.h"
#include "qdss_hwevent_config.h"


/*
 * Define the page size for HWIO regions - 4K.
 */
#define HWIO_PAGE_SIZE 0x1000

/*
 * Define macro to get the page base and offset given an address.
 */
#define HWIO_PAGE_BASE(nAddr)   ((nAddr) & ~(HWIO_PAGE_SIZE - 1))

#define HWIO_PAGE_OFFSET(nAddr) ((nAddr)& (HWIO_PAGE_SIZE - 1))

#define CORESIGHT_LOCK_OFFSET 0x00000fb0


typedef struct HWEventMemMapType
{
   uint32 uphys_addr;
   uint32 uvirt_addr;
   DALSYSMemHandle hMem;
} HWEventMemMap;

typedef struct HWEventRegisterValue_t HWEventRegisterValue;
typedef struct HWEventAddrRange_t HWEventAddrRange;

/*
address-value pairs for HW Events.
*/
struct HWEventRegisterValue_t{
   uint32 reg_phys_addr;
   uint32 reg_value;
};

struct HWEventAddrRange_t
{
   uint32 lower_range;
   uint32 higher_range;
   uint32 unlock_value;

};

/*------------------------------------------------------------------------------
Declaring a "HWEvent" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct HWEventDrvCtxt HWEventDrvCtxt;
typedef struct HWEventDevCtxt HWEventDevCtxt;
typedef struct HWEventClientCtxt HWEventClientCtxt;

/*---------------------------------------------------------------------------
Declaring a private "HWEvent" Vtable
------------------------------------------------------------------------------*/
typedef struct HWEventDALVtbl HWEventDALVtbl;
struct HWEventDALVtbl
{
   int (*HWEvent_DriverInit)(HWEventDrvCtxt *);
   int (*HWEvent_DriverDeInit)(HWEventDrvCtxt *);
};

struct HWEventDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   HWEventDrvCtxt  *pHWEventDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //HWEvent Dev state can be added by developers here
   HWEventMemMap *pMemMapTable;
   HWEventRegisterValue *pConfigTable;
   HWEventAddrRange *pAddrRangeTable;
   uint32 uTableSize;
   uint32 uAddrRangeTableSize;
};

struct HWEventDrvCtxt
{
   //Base Members
   HWEventDALVtbl HWEventDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   HWEventDevCtxt HWEventDevCtxt[1];
   //HWEvent Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "HWEvent" Client Context
------------------------------------------------------------------------------*/
struct HWEventClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   HWEventDevCtxt *pHWEventDevCtxt;
   DalHWEventHandle DalHWEventHandle;
   //HWEvent Client state can be added by developers here
};



/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
HWEvent_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);

static DALResult HWEvent_FindPhysAddrMapping(uint32 phys_addr,
                                      uint32 *pvirt_addr,
                                      HWEventDevCtxt *pDevCtxt);

static void HWEvent_SetReg(uint32 virtAddr,uint32 regAddr,uint32 regValue);

static DALResult HWEvent_AddPhysAddrMapping(uint32 phys_addr,
                                     uint32 *pvirt_addr,
                                     HWEventDevCtxt *pDevCtxt);

static void CoreSightUnLock(uint32 uBaseAddr);
static void CoreSightLock(uint32 uBaseAddr);




/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/

const DALREG_DriverInfo
DALHWEvent_DriverInfo = { HWEvent_DeviceAttach, 0, NULL};


typedef struct HWEventTableSize_t HWEventTableSize;

struct HWEventTableSize_t{
   uint32 size;
};




/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32
HWEvent_AddRef(DalHWEventHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
HWEvent_DriverInit(HWEventDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
HWEvent_DriverDeInit(HWEventDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

static __inline boolean HWEvent_isBlockedAddress(uint32 phys_addr,HWEventDevCtxt 
*pDevCtxt,uint32 *pUnlockCode)
{
   HWEventAddrRange *pAddrRangeTable;
   uint32 i;
   i=0;
   pAddrRangeTable = pDevCtxt->pAddrRangeTable;
   while((pAddrRangeTable->lower_range!=NULL) &&
        (i<pDevCtxt->uAddrRangeTableSize))
   {
      if((phys_addr>=pAddrRangeTable->lower_range) && (phys_addr<=pAddrRangeTable->higher_range))
      {
         *pUnlockCode=pAddrRangeTable->unlock_value;
         return FALSE;//not blocked
      }
      pAddrRangeTable++;
      i++;
   }
   return TRUE; //not found, is blocked
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
HWEvent_DeviceDetach(uint32 ClientId, DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}



static DALResult
HWEvent_DeviceInit(uint32 ClientId, DalDeviceHandle *h)
{
      DALClientCtxt * pClientCtxt = (DALClientCtxt *)(h->pClientCtxt);
      HWEventDevCtxt *pDevCtxt;
      DALSYSPropertyVar PropVar;
      uint32 uNumReg,phys_addr,virt_addr=NULL;
      DALResult uFound;


      HWEventRegisterValue *pConfigTable;
      HWEventTableSize *pConfigTableSize;
      uint32 i;


      pDevCtxt=(HWEventDevCtxt *)pClientCtxt->pDALDevCtxt;



      if(DAL_SUCCESS!=DALSYS_GetDALPropertyHandleStr(pClientCtxt->pDALDevCtxt->strDeviceName,
         pDevCtxt->hProp))
      {
         return DAL_ERROR;
      }


      if (DAL_SUCCESS!= DALSYS_GetPropertyValue(pDevCtxt->hProp,
                        "hwevent_config",
                        0,
                        &PropVar ))
      {
         return DAL_ERROR;
      }
      pConfigTable=(HWEventRegisterValue *)PropVar.Val.pStruct;
      pDevCtxt->pConfigTable=pConfigTable;

      if (DAL_SUCCESS!= DALSYS_GetPropertyValue(pDevCtxt->hProp,
                        "hwevent_config_size",
                        0,
                        &PropVar ))
      {
         return DAL_ERROR;
      }
      pConfigTableSize=(HWEventTableSize *)PropVar.Val.pStruct;
      uNumReg=pConfigTableSize->size;
      pDevCtxt->uTableSize=uNumReg;

      if (DAL_SUCCESS!= DALSYS_GetPropertyValue(pDevCtxt->hProp,
                        "hwevent_addr_check",
                        0,
                        &PropVar ))
      {
         return DAL_ERROR;
      }
      pDevCtxt->pAddrRangeTable=(HWEventAddrRange *)PropVar.Val.pStruct;

      if (DAL_SUCCESS!= DALSYS_GetPropertyValue(pDevCtxt->hProp,
                        "hwevent_addr_check_size",
                        0,
                        &PropVar ))
      {
         return DAL_ERROR;
      }

      pConfigTableSize=(HWEventTableSize *)PropVar.Val.pStruct;
      pDevCtxt->uAddrRangeTableSize = pConfigTableSize->size;

   /*allocate memory to store virtual address of registers*/
      if (DAL_SUCCESS!=DALSYS_Malloc(uNumReg*sizeof(HWEventMemMap),
                        (void **)&pDevCtxt->pMemMapTable))
      {
         return DAL_ERROR;
      }
      memset(pDevCtxt->pMemMapTable,0x0,uNumReg*sizeof(HWEventMemMap));


      for(i=0;i<uNumReg;i++)
      {
         phys_addr= HWIO_PAGE_BASE(pConfigTable[i].reg_phys_addr);
         /*end of table*/
         if (phys_addr==NULL)
         {
            break;
         }
         uFound = HWEvent_FindPhysAddrMapping(phys_addr,&virt_addr,pDevCtxt);
         if (uFound==DAL_ERROR)
         {
            if (DAL_SUCCESS!=HWEvent_AddPhysAddrMapping(phys_addr,&virt_addr,pDevCtxt))
            {
               return DAL_ERROR;
            }
         }

      }

   return DAL_SUCCESS;
}


static DALResult
HWEvent_DeviceDeInit(uint32 ClientId, DalDeviceHandle *h)
{
   DALResult ret=DAL_ERROR;
   DALClientCtxt * pClientCtxt = (DALClientCtxt *)(h->pClientCtxt);
   HWEventDevCtxt *pDevCtxt;
   uint32 i;

   pDevCtxt=(HWEventDevCtxt *)pClientCtxt->pDALDevCtxt;
   if(pDevCtxt->pMemMapTable!=NULL)
   {
      for(i=0;i<pDevCtxt->uTableSize;i++)
      {
         if(pDevCtxt->pMemMapTable[i].hMem!=NULL)
         {
            DALSYS_DestroyObject(pDevCtxt->pMemMapTable[i].hMem);/*destroy mem object*/
         }
      }
      DALSYS_Free(pDevCtxt->pMemMapTable);
      ret=DAL_SUCCESS;
   }


   return ret;
}

static DALResult
HWEvent_DevicePowerEvent(uint32 ClientId, DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
HWEvent_DeviceOpen(uint32 ClientId, DalDeviceHandle* h, uint32 mode)
{

   return DAL_SUCCESS;
}

static DALResult
HWEvent_DeviceClose(uint32 ClientId, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
HWEvent_DeviceInfo(uint32 ClientId, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALHWEVENT_STRING_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
HWEvent_DeviceSysRequest(uint32 ClientId, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalHWEvent Interface.
------------------------------------------------------------------------------*/
DALResult HWEvent_SetRegister(uint32 ClientId, DalDeviceHandle* h,uint32 phys_addr,uint32 value,
                              uint8 *pret_code)
{
   HWEventDevCtxt *pDevCtxt;
   DALClientCtxt * pClientCtxt = (DALClientCtxt *)(h->pClientCtxt);
   DALSYSMemHandle hMem;
   DALSYSMemInfo MemInfo;
   DALResult retcode = DAL_ERROR;
   uint32 unlock_code;


/*register peek-poke functionality needs to be guarded by a security fuse*/

/*STM.AUTHSTATUS check is performed by STMCfg driver and qdss_ensure_hw_ready */
   pDevCtxt=(HWEventDevCtxt *)pClientCtxt->pDALDevCtxt;

   if(pDevCtxt==NULL)
     return DAL_ERROR;
   if (HWEvent_isBlockedAddress(phys_addr,pDevCtxt,&unlock_code))
   {
      *pret_code=QDSS_HWEVENT_ADDR_CHECK_ERR;
      return DAL_ERROR;
   }

   if (DAL_SUCCESS!=DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
            DALSYS_MEM_ADDR_NOT_SPECIFIED,
            (DALSYSMemAddr)HWIO_PAGE_BASE(phys_addr),
            HWIO_PAGE_SIZE,
            &hMem,
            NULL))
   {
      return DAL_ERROR;
   }
   retcode=DALSYS_MemInfo(hMem, &MemInfo);

   if(MemInfo.VirtualAddr!=NULL)
   {
      if(unlock_code=='U')
      {
         CoreSightUnLock((uint32)HWIO_PAGE_BASE(MemInfo.VirtualAddr));
      }
      HWEvent_SetReg((uint32)MemInfo.VirtualAddr,phys_addr,value);

      if(unlock_code=='U')
      {
         CoreSightLock((uint32)HWIO_PAGE_BASE(MemInfo.VirtualAddr));
      }
   }

   retcode=DALSYS_DestroyObject(hMem);
   if(retcode==DAL_SUCCESS)
      *pret_code=QDSS_HWEVENT_CONFIG_SUCCESS;
   return retcode;
}

DALResult HWEvent_ConfigurePresetEvents(uint32 ClientId, DalDeviceHandle* h)
{
   HWEventDevCtxt *pDevCtxt;
   DALClientCtxt * pClientCtxt = (DALClientCtxt *)(h->pClientCtxt);
   uint32 uNumReg,i,unlock_code;
   DALResult uFound;
   uint32 virt_addr;


   pDevCtxt=(HWEventDevCtxt *)pClientCtxt->pDALDevCtxt;

   if(pDevCtxt==NULL)
      return DAL_ERROR;

   uNumReg = pDevCtxt->uTableSize;


   for(i=0;i<uNumReg;i++)
   {
/*Silently drop writes to blocked address regions without flagging an error*/
      if (!HWEvent_isBlockedAddress(pDevCtxt->pConfigTable[i].reg_phys_addr,
         pDevCtxt,&unlock_code))
      {
         uFound = HWEvent_FindPhysAddrMapping(
                       HWIO_PAGE_BASE(pDevCtxt->pConfigTable[i].reg_phys_addr),
                       &virt_addr,
                       pDevCtxt);
         if (uFound==DAL_ERROR)
         {
            return DAL_ERROR;
         }
         if(virt_addr!=NULL)
         {
            HWEvent_SetReg((uint32)virt_addr,
                        pDevCtxt->pConfigTable[i].reg_phys_addr,
                        pDevCtxt->pConfigTable[i].reg_value);
         }
      }

   }
   return DAL_SUCCESS;
}

static void
HWEvent_InitInterface(HWEventClientCtxt* pclientCtxt)
{
 static const DalHWEvent vtbl =
   {
      {
         HWEvent_DeviceAttach,
         HWEvent_DeviceDetach,
         HWEvent_DeviceInit,
         HWEvent_DeviceDeInit,
         HWEvent_DeviceOpen,
         HWEvent_DeviceClose,
         HWEvent_DeviceInfo,
         HWEvent_DevicePowerEvent,
         HWEvent_DeviceSysRequest
      },
      HWEvent_SetRegister,
      HWEvent_ConfigurePresetEvents
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalHWEventHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalHWEventHandle.pVtbl  = &vtbl;
   pclientCtxt->DalHWEventHandle.pClientCtxt = pclientCtxt;
}

DALResult
HWEvent_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static HWEventDrvCtxt drvCtxt = {{HWEvent_DriverInit,
                                      HWEvent_DriverDeInit
                                     },1,
                                     sizeof(HWEventDevCtxt)};
   static HWEventClientCtxt clientCtxt;

   HWEventClientCtxt *pclientCtxt = &clientCtxt;


   *phDalDevice = NULL;

   nErr = DALFW_AttachToStringDevice(pszArg,(DALDrvCtxt *)&drvCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      HWEvent_InitInterface(pclientCtxt);
      HWEvent_AddRef(&(pclientCtxt->DalHWEventHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalHWEventHandle);
   }
   return nErr;
}

static __inline void HWEvent_SetReg(uint32 virtAddr,uint32 regAddr,uint32
regValue)
{
   //Apply any applicable workarounds on register settings
   qdss_hwe_setreg_remap(&regAddr, &regValue);
   
   virtAddr=virtAddr+HWIO_PAGE_OFFSET(regAddr);//mapping to a 4K aligned virtual page
   out_dword(virtAddr,regValue);
}


static DALResult HWEvent_FindPhysAddrMapping(uint32 phys_addr,uint32 *pvirt_addr,
HWEventDevCtxt *pDevCtxt)
{
   uint32 i;
   uint32 uTableSize=pDevCtxt->uTableSize;
   DALResult DalRet=DAL_ERROR;
   HWEventMemMap *pMap=pDevCtxt->pMemMapTable;
   *pvirt_addr=NULL;


   for(i=0;i<uTableSize;i++)
   {
      if (pMap[i].uphys_addr==phys_addr)
      {
         DalRet=DAL_SUCCESS;
         *pvirt_addr=pMap[i].uvirt_addr;
         break;
      }
   }
   return DalRet;
}

/*This function is called only during driver initialization*/
static DALResult HWEvent_AddPhysAddrMapping(uint32 phys_addr,
               uint32 *pvirt_addr,
               HWEventDevCtxt *pDevCtxt)
{
   uint32 i;
   uint32 uTableSize=pDevCtxt->uTableSize;
   DALSYSMemAddr virt_addr;
   DALResult DalRet=DAL_ERROR;
   DALSYSMemHandle hMem;
   DALSYSMemInfo MemInfo;
   HWEventMemMap *pMap=pDevCtxt->pMemMapTable;




   for(i=0;(i<uTableSize);i++)
   {
      if (pMap[i].uphys_addr==0x0)
      {
         if (DAL_SUCCESS!=DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
               DALSYS_MEM_ADDR_NOT_SPECIFIED,
               (DALSYSMemAddr)phys_addr,
               HWIO_PAGE_SIZE,
               &hMem,
               NULL))
         {
            DalRet = DAL_ERROR;
            break;
         }
         DALSYS_MemInfo(hMem, &MemInfo);
         virt_addr=MemInfo.VirtualAddr;
         pMap[i].uphys_addr=phys_addr;
         pMap[i].uvirt_addr=(uint32)virt_addr;
         pMap[i].hMem=hMem;
         *pvirt_addr=(uint32)virt_addr;
         DalRet = DAL_SUCCESS;
         break;
      }
      else if (pMap[i].uphys_addr==phys_addr) //found existing mapping
      {
         *pvirt_addr=pMap[i].uvirt_addr;
         DalRet = DAL_SUCCESS;
         break;
      }
   }
   return DalRet;
}

static __inline void CoreSightUnLock(uint32 virtAddr)
{
   virtAddr=virtAddr+CORESIGHT_LOCK_OFFSET;
   out_dword(virtAddr,0xC5ACCE55);
}

static __inline void CoreSightLock(uint32 virtAddr)
{
   virtAddr=virtAddr+CORESIGHT_LOCK_OFFSET;
   out_dword(virtAddr,0x0);
}


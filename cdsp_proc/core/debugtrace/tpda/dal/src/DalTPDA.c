/*=============================================================================
  Copyright (c) 2015 Qualcomm Technologies Inc.
           All Rights Reserved.
$Header $
=============================================================================*/

#include "stringl/stringl.h"
#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDITFunnel.h"
#include "DDITPDA.h"
#include "DalTPDA.h"
#include "qdss_throw.h"
#include "HALhwio.h"
#include "hal_qdss_tpda.h"

#define TPDA_PAGE_SIZE 4096

#define TPDM_NAME_LEN 32


/*------------------------------------------------------------------------------
Declaring a "TPDA" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct TPDADrvCtxt_Type TPDADrvCtxt;
typedef struct TPDADevCtxt_Type TPDADevCtxt;
typedef struct TPDAClientCtxt_Type TPDAClientCtxt;

/*---------------------------------------------------------------------------
Declaring a private "TPDA" Vtable
------------------------------------------------------------------------------*/
typedef struct TPDADALVtbl TPDADALVtbl;
struct TPDADALVtbl
{
   int (*TPDA_DriverInit)(TPDADrvCtxt *);
   int (*TPDA_DriverDeInit)(TPDADrvCtxt *);
};

struct TPDADevCtxt_Type
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   TPDADrvCtxt  *pTPDADrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //TPDA Dev state can be added by developers here
   DALSYSMemHandle *hMem; //pointer to memory handles
   uint32 *pVirtAddr;
   TPDADeviceProp *pDeviceAttributes;
   uint32 attribTableLen;
   uint32 attribIndex;

};

struct TPDADrvCtxt_Type
{
   //Base Members
   TPDADALVtbl TPDADALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   TPDADevCtxt TPDADevCtxt[1];
   //TPDA Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "TPDA" Client Context
------------------------------------------------------------------------------*/
struct TPDAClientCtxt_Type
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   TPDADevCtxt *pTPDADevCtxt;
   DalTPDAHandle DalTPDAHandle;
   //TPDA Client state can be added by developers here
};



/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
TPDA_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/

const DALREG_DriverInfo
DALTPDA_LPA_DriverInfo = { TPDA_DeviceAttach, 0, NULL};


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

static uint32
TPDA_AddRef(DalTPDAHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}


/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
TPDA_DriverInit(TPDADrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
TPDA_DriverDeInit(TPDADrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
TPDA_DeviceDetach(uint32 ClientHandle, DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult TPDA_MemMap(uint32 physAddr,
                            DALSYSMemHandle *phMem,
                            uint32 *virtAddr)
{
   int nErr;
   DALSYSMemInfo memInfo;


   TRY(nErr,DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  physAddr,
                                  TPDA_PAGE_SIZE, phMem, NULL ));
   DALSYS_MemInfo(*phMem, &memInfo);
   *virtAddr = memInfo.VirtualAddr;
   CATCH(nErr) {}

   return nErr;
}

#define TPDA_ASYNC_PERIODICITY_DEFAULT   4095
#define WCSS_DBG_TS_CNTR_FREQ_DEFAULT    120000000   /*120 MHz*/


static DALResult
TPDA_DeviceInit(uint32 ClientHandle, DalDeviceHandle *h)
{
   DALClientCtxt * pClientCtxt = (DALClientCtxt *)(h->pClientCtxt);
   TPDADevCtxt *pDevCtxt = (TPDADevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   TPDADeviceProp *pDeviceAttributes;
   TableLength *pAttribLen;
   DALSYSPropertyVar PropVar;
   uint32 nErr,i;
   
   
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(pClientCtxt->pDALDevCtxt->strDeviceName,
                                                    pDevCtxt->hProp ))
       return DAL_ERROR;

   if (DAL_SUCCESS!=DALSYS_GetPropertyValue( pDevCtxt->hProp,"tpda_device_list",0,&PropVar ))
         return DAL_ERROR;

   pDeviceAttributes = (TPDADeviceProp *)PropVar.Val.pStruct;
   pDevCtxt->pDeviceAttributes = pDeviceAttributes;
   
   if (DAL_SUCCESS!=DALSYS_GetPropertyValue(pDevCtxt->hProp,"tpda_device_list_len",0,&PropVar ))
            return DAL_ERROR;
   pAttribLen = (TableLength *)PropVar.Val.pStruct;
   pDevCtxt->attribTableLen=pAttribLen[0].length;
   
   if (DAL_SUCCESS!=DALSYS_Malloc(pDevCtxt->attribTableLen*sizeof(DALSYSMemHandle),(void **)&pDevCtxt->hMem))
      return DAL_ERROR;

   
   memset(pDevCtxt->hMem,0x0,pDevCtxt->attribTableLen*sizeof(DALSYSMemHandle));

   if (DAL_SUCCESS!=DALSYS_Malloc(pDevCtxt->attribTableLen*sizeof(uint32),(void **)&pDevCtxt->pVirtAddr))
      return DAL_ERROR;

   memset(pDevCtxt->pVirtAddr,0x0,pDevCtxt->attribTableLen*sizeof(uint32));      

   for (i=0;i<pDevCtxt->attribTableLen;i++)
   {
      nErr=TPDA_MemMap(pDeviceAttributes[i].phys_addr,&pDevCtxt->hMem[i],&pDevCtxt->pVirtAddr[i]);
      if (nErr!=DAL_SUCCESS)
         return DAL_ERROR;
	  HAL_qdss_tpda_SetBaseAddress(pDevCtxt->pVirtAddr[i]);
	  HAL_qdss_tpda_unlock_access();
	  HAL_qdss_tpda_set_atid(pDeviceAttributes[i].atid);
     HAL_qdss_tpda_set_avfreqen(1);
     HAL_qdss_tpda_set_sync_pkt_periodicity(PERIOD_BYTES,TPDA_ASYNC_PERIODICITY_DEFAULT);
     HAL_qdss_tpda_set_freq_val(WCSS_DBG_TS_CNTR_FREQ_DEFAULT);

	  HAL_qdss_tpda_lock_access();
   }

//Determine size of list
//Perform memory map
//Vote for TPDA clocks when enabling - Disable clocks when detach occurs
   return DAL_SUCCESS;
}


static DALResult
TPDA_DeviceDeInit(uint32 ClientHandle, DalDeviceHandle *h)
{
   return DAL_SUCCESS;
}

static DALResult
TPDA_DevicePowerEvent(uint32 ClientHandle, DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
TPDA_DeviceOpen(uint32 ClientHandle, DalDeviceHandle* h, uint32 mode)
{

   return DAL_SUCCESS;
}

static DALResult
TPDA_DeviceClose(uint32 ClientHandle, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
TPDA_DeviceInfo(uint32 ClientHandle, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTPDA_STRING_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
TPDA_DeviceSysRequest(uint32 ClientHandle, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTPDA Interface.
------------------------------------------------------------------------------*/
static DALResult _TPDA_GetAttribIndex(DalDeviceHandle *h,const char 
*block,uint32 *pIndex)
{
   TPDADevCtxt          *pDevCtxt;
   uint32 i,len;
   uint32 ret=DAL_TPDA_INVALID_BLOCK;
   pDevCtxt = (TPDADevCtxt *)((DALClientCtxt *) (h->pClientCtxt))->pDALDevCtxt;
   len=pDevCtxt->attribTableLen;
   
   if(!pIndex)
     return DAL_ERROR;
   else
     *pIndex=0xfffffff;
     
   
   for (i = 0; i < len; i++) {
       if (!strncmp(block,
                    pDevCtxt->pDeviceAttributes[i].tpda_name,
                    TPDA_NAME_LEN)) {
           *pIndex= i;
           ret=DAL_SUCCESS;
           break;
       }
   }
   
   /* Did not find block name in cti_mem_maps array */
   return ret;

   
}

static DALResult _TPDA_GetTPDMIndex(DalDeviceHandle *h,uint32 tpda_index,const char 
*pTPDMName,uint32 *pIndex)
{
   TPDADevCtxt          *pDevCtxt;
   uint32 i,len;
   uint32 ret=DAL_TPDA_INVALID_BLOCK;
   TPDAPortSpec *pTPDMList;
   pDevCtxt = (TPDADevCtxt *)((DALClientCtxt *) (h->pClientCtxt))->pDALDevCtxt;

   pTPDMList=pDevCtxt->pDeviceAttributes[tpda_index].port_list;
  
   len=pDevCtxt->pDeviceAttributes[tpda_index].port_list_size;
   
   if(!pIndex)
     return DAL_ERROR;
   else
     *pIndex=0xfffffff;
     
   
   for (i = 0; i < len; i++) {
       if (!strncmp(pTPDMName,
                    pTPDMList[i].tpdm_name,
                    TPDM_NAME_LEN)) {
           *pIndex= i;
           ret=DAL_SUCCESS;
           break;
       }
   }
   
   /* Did not find block name in cti_mem_maps array */
   return ret;

   
}



static DALResult _TPDA_GetVirtAddr(DalDeviceHandle *h,
                                     const char *block,
                                     uint32 *virt_addr)
{
       TPDADevCtxt          *pDevCtxt;
       uint32              i,len;
       uint32 ret=DAL_TPDA_INVALID_BLOCK;
       DALSYSMemInfo       mem_info;
   
       pDevCtxt = (TPDADevCtxt *)((DALClientCtxt *) (h->pClientCtxt))->pDALDevCtxt;
       len=pDevCtxt->attribTableLen;

       if(!virt_addr)
         return DAL_ERROR;
       else
         *virt_addr=0;
         
   
       for (i = 0; i < len; i++) {
           if (!strncmp(block,
                        pDevCtxt->pDeviceAttributes[i].tpda_name,
                        TPDA_NAME_LEN)) {
               if (0 != DALSYS_MemInfo(pDevCtxt->hMem[i],&mem_info)) {
                   break;
               }
               *virt_addr = mem_info.VirtualAddr;
               ret=DAL_SUCCESS;
               break;
           }
       }
   
       /* Did not find block name in cti_mem_maps array */
       return ret;
}

static DALResult _TPDA_SetBaseAddress(DalDeviceHandle *h, const char *block)
{
    DALResult   err;
    uint32      virt_addr;

    TRY(err, _TPDA_GetVirtAddr(h, block, &virt_addr));
    HAL_qdss_tpda_SetBaseAddress(virt_addr);

    CATCH(err) {}

    return err;
}


#define TPDM_CMBESIZE_8   0
#define TPDM_CMBESIZE_32  1
#define TPDM_CMBESIZE_64  2



static DALResult TPDA_EnableDisable(uint32 ClientHandle, DalDeviceHandle *phandle,const char 
*pTPDAStr,const char *pTPDMStr,DALBOOL bEnable)
{
   TPDADevCtxt *pDevCtxt = (TPDADevCtxt *)((DALClientCtxt *)(phandle->pClientCtxt))->pDALDevCtxt;
   uint32 i,setbit,tpdm_index,tpda_port_num;
   static DALDEVICEHANDLE hFunnel;
   TFunnelControlParam funnel_param;


   if (DAL_SUCCESS!=_TPDA_SetBaseAddress(phandle,pTPDAStr))
   {
      return DAL_TPDA_INVALID_BLOCK;
   }

   setbit=(bEnable==TRUE)?1:0;

   if(DAL_SUCCESS!=_TPDA_GetAttribIndex(phandle,pTPDAStr,&i))
      return DAL_TPDA_INVALID_BLOCK;

   if (DAL_SUCCESS!=_TPDA_GetTPDMIndex(phandle,i,pTPDMStr,(uint32 *)&tpdm_index))
   {
      return DAL_TPDA_INVALID_BLOCK;
   }

   if (hFunnel== NULL)
   {
      if (DAL_SUCCESS!=DAL_TFunnelDeviceAttach(DALDEVICEID_TFUNNEL_0,&hFunnel))
      {
         return DAL_TPDA_ERROR;
      }
   }      
      funnel_param.pPortStr = pDevCtxt->pDeviceAttributes[i].tpda_funnel_name;
      funnel_param.Val.bEnable=0x1;

   if (DAL_SUCCESS!=DalTFunnel_Control(hFunnel,TFUNNEL_PORT_ENABLE,&funnel_param))
   {
      return DAL_TPDA_ERROR;
   }
   

   tpda_port_num=pDevCtxt->pDeviceAttributes[i].port_list[tpdm_index].tpda_port_num;
   HAL_qdss_tpda_unlock_access();
   if (bEnable) {
      HAL_qdss_tpda_port_set_cmbesize(tpda_port_num,TPDM_CMBESIZE_64);
   }
   HAL_qdss_tpda_port_enable_disable(tpda_port_num,setbit);
   HAL_qdss_tpda_lock_access();
   return DAL_TPDA_SUCCESS;
}

   
static DALResult TPDA_SetParam(uint32 ClientHandle, DalDeviceHandle *phandle,const char *pTPDAStr, uint8 param_id,uint32 val)
{
   TPDADevCtxt *pDevCtxt = (TPDADevCtxt *)((DALClientCtxt *)(phandle->pClientCtxt))->pDALDevCtxt;
   uint32 index,virt_addr;
   DALSYSMemInfo       mem_info;
   DALResult ret=DAL_SUCCESS;

   if(DAL_SUCCESS!=_TPDA_GetAttribIndex(phandle,pTPDAStr,&index))
         return DAL_TPDA_INVALID_BLOCK;

   DALSYS_MemInfo(pDevCtxt->hMem[index],&mem_info);
   virt_addr=mem_info.VirtualAddr;

   HAL_qdss_tpda_SetBaseAddress(virt_addr);
   HAL_qdss_tpda_unlock_access();

   switch(param_id)
   {
      case TPDA_CONTROL_SET_FREQ_VAL:
         HAL_qdss_tpda_set_freq_val(val);
         break;
      case TPDA_CONTROL_GENERATE_FREQ_PKT:
         HAL_qdss_tpda_freq_pkt_req();
         break;
      case TPDA_CONTROL_GLOBAL_FLUSH_REQ:
         HAL_qdss_tpda_global_flush_req();
         break;
      case TPDA_CONTROL_FREQ_PKT_TIMESTAMP_ON_OFF:
         HAL_qdss_tpda_control_freq_timestamp_on_off(val);
         break;
      case TPDA_CONTROL_ASYNC_PERIOD:
         if (val <= TPDA_ASYNC_PERIODICITY_DEFAULT) {
            HAL_qdss_tpda_set_sync_pkt_periodicity(PERIOD_BYTES,val);
         }
         else {
            uint32 x=TPDA_ASYNC_PERIODICITY_DEFAULT;
            uint32 power = 12;
            while ((x < val) && (power <= 27)) {
               x=(x << 1);
               power++;
            }
            //set periodicity to the nearest power of 2 less than the value
            power--;
            HAL_qdss_tpda_set_sync_pkt_periodicity(PERIOD_2POWN, power);
         }
         break;
      case TPDA_CONTROL_GEN_FLAG_PKT_VIA_CTI_ON_OFF:
         HAL_qdss_tpda_flag_pkt_via_CTI(val);
         break;
      case TPDA_CONTROL_GEN_FREQ_PKT_VIA_CTI_ON_OFF:
         HAL_qdss_tpda_freq_pkt_via_CTI(val);
         break;
      case TPDA_CONTROL_GEN_SYNC_PKT_VIA_CTI_ON_OFF:
         HAL_qdss_tpda_sync_pkt_via_CTI(val);
         break;
      default:
	     ret=DAL_ERROR;
         break;
   }
   HAL_qdss_tpda_lock_access();
   return ret;
}

static void
TPDA_InitInterface(TPDAClientCtxt* pclientCtxt)
{
 static const DalTPDA vtbl =
   {
      {
         TPDA_DeviceAttach,
         TPDA_DeviceDetach,
         TPDA_DeviceInit,
         TPDA_DeviceDeInit,
         TPDA_DeviceOpen,
         TPDA_DeviceClose,
         TPDA_DeviceInfo,
         TPDA_DevicePowerEvent,
         TPDA_DeviceSysRequest
      },
      TPDA_EnableDisable,
      TPDA_SetParam
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalTPDAHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalTPDAHandle.pVtbl  = &vtbl;
   pclientCtxt->DalTPDAHandle.pClientCtxt = pclientCtxt;
}

DALResult
TPDA_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static TPDADrvCtxt drvCtxt = {{TPDA_DriverInit,
                                 TPDA_DriverDeInit
                                },1,
                                sizeof(TPDADevCtxt)};
   static TPDAClientCtxt clientCtxt;

   TPDAClientCtxt *pclientCtxt = &clientCtxt;


   *phDalDevice = NULL;

   nErr = DALFW_AttachToStringDevice(pszArg,(DALDrvCtxt *)&drvCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      TPDA_InitInterface(pclientCtxt);
      TPDA_AddRef(&(pclientCtxt->DalTPDAHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalTPDAHandle);
   }
   return nErr;
}

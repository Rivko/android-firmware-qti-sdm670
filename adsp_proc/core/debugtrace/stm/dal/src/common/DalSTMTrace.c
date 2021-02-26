/*=============================================================================
  Copyright (c) 2009 Qualcomm Technologies Incorporated.
           All Rights Reserved.

                  $Header: //components/rel/core.qdsp6/2.1/debugtrace/stm/dal/src/common/DalSTMTrace.c#2 $
=============================================================================*/

#include "DALFramework.h"
#include "DalDevice.h"
#include "DDISTMTrace.h"
#include "string.h" //for memset warning
#include "qdss.h"
#include "qdss_utils.h"


typedef struct STMPortRange STMPortRange;
struct STMPortRange
{
   uint32 uBasePort; // starting port number
   uint32 uNumPorts; // number of ports in the range
   uint32 *pBitMask; // ports allocation management
};

typedef struct _STMPortRangeItem STMPortRangeItem;
struct _STMPortRangeItem
{
   STMPortRange portRange; // port range
   DALSYSMemHandle hMem;   // handle to the allocated memory region
   uint32 uNumFreePorts;   // number of free ports
   uint32 uBaseAddr;       // base address of the memory region
   STMPortRange * mainPortRange;
   STMPortRangeItem * next;
};

/*------------------------------------------------------------------------------
Declaring a "STMTrace" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct STMTraceDrvCtxt STMTraceDrvCtxt;
typedef struct STMTraceDevCtxt STMTraceDevCtxt;
typedef struct STMTraceClientCtxt STMTraceClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "STMTrace" Vtable
------------------------------------------------------------------------------*/
typedef struct STMTraceDALVtbl STMTraceDALVtbl;
struct STMTraceDALVtbl
{
   int (*STMTrace_DriverInit)(STMTraceDrvCtxt *);
   int (*STMTrace_DriverDeInit)(STMTraceDrvCtxt *);
};

struct STMTraceDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   STMTraceDrvCtxt  *pSTMTraceDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //STMTrace Dev state can be added by developers here
   uint32 uSTMSPBaseAddr;
   STMPortRange portRange;     // total available STM ports
   STMPortRange portRangeDMA;  // total available STM ports supporting DMA
   STMPortRangeItem * listHead;// linked list of allocated ports
   DALSYSSyncHandle hSync;
};

struct STMTraceDrvCtxt
{
   //Base Members
   STMTraceDALVtbl STMTraceDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   STMTraceDevCtxt STMTraceDevCtxt[1];
   //STMTrace Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "STMTrace" Client Context
------------------------------------------------------------------------------*/
struct STMTraceClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   STMTraceDevCtxt *pSTMTraceDevCtxt;
   DalSTMTraceHandle DalSTMTraceHandle;
   //STMTrace Client state can be added by developers here
};

/*------------------------------------------------------------------------------
Internal functions
------------------------------------------------------------------------------*/
#define ROUND_UP_32(num)   (((num)+31)&~31)
#define ROUND_UP_16(num)   (((num)+15)&~15)

/* 4k page supports 16 ports each with 256 bytes memory */
#define MAX_STM_PORTS_PER_PAGE      16
#define STM_PORT_SIZE                           256

static DALResult _CollectProperties(STMTraceDevCtxt *pDevCtxt)
{
   DALSYSPropertyVar PropVar;

   /* get the base extended stimulus port address */
   if (DAL_SUCCESS != DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                               "stm_sp_base_addr", 0,&PropVar ))
   {
      return DAL_ERROR;
   }
   pDevCtxt->uSTMSPBaseAddr = PropVar.Val.dwVal;

   /* get the base extended stimulus port address */
   if (DAL_SUCCESS != DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                               "stm_base_port", 0,&PropVar ))
   {
      return DAL_ERROR;
   }
   pDevCtxt->portRange.uBasePort = PropVar.Val.dwVal;

   /* get the number of stimulus ports allocated */
   if (DAL_SUCCESS != DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                                "stm_num_ports", 0, &PropVar ))
   {
      return DAL_ERROR;
   }
   pDevCtxt->portRange.uNumPorts =  ROUND_UP_16(PropVar.Val.dwVal);

   /* get the base extended stimulus port address supporting BAMDMA */
   if (DAL_SUCCESS != DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                               "stm_base_port_dma", 0,
                                               &PropVar ))
   {
      pDevCtxt->portRangeDMA.uBasePort = 0x0; /* No BAMDMA'able ports supported */
      pDevCtxt->portRangeDMA.uNumPorts = 0x0; /* No BAMDMA'able ports supported */
   }
   else
   {
      pDevCtxt->portRange.uBasePort = PropVar.Val.dwVal;
      /* get the number of BAMDMA'able stimulus ports allocated */
      if (DAL_SUCCESS != DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                                  "stm_num_ports_dma", 0,
                                                  &PropVar ))
      {
         /* some mis-configuration issue*/
         return DAL_ERROR;
      }
      pDevCtxt->portRangeDMA.uNumPorts = PropVar.Val.dwVal;
   }

   return DAL_SUCCESS;
}

static DALResult _AllocateMemForBitMasks(uint32** pMask, uint32 uNumBits)
{

   uint32 uMemSize = sizeof(uint32) * (ROUND_UP_32(uNumBits)/32);

//LHT add size check because DALSYS_Malloc will assert after
//DALHeap_Malloc fails on request of size zero.
   if (0 < uMemSize)
   {
      if(DAL_SUCCESS != DALSYS_Malloc(uMemSize, (void **)pMask))
      {
         return DAL_ERROR;
      }

      memset(*pMask, 0, uMemSize);
   }
   return DAL_SUCCESS;
}


static DALResult _AllocBit(uint32 * pMask, uint32 uMaxBits, uint32 *uBitPos)
{
   uint32 i, j, bmsk,val;

   val=(ROUND_UP_32(uMaxBits))/32;

   for(i=0; i<val; i++)
   {
      bmsk = pMask[i];
      if(bmsk==0xFFFFFFFF)
      {
/*Skip loop if free bit positions are not available*/
         continue;
      }
      for(j=0; j<32; j++)
      {
         if(!(bmsk&1))
         {
            if(uMaxBits > (i*32 + j))
            {
               *uBitPos = i*32 + j;
               pMask[i] |= (1<<j);
               return DAL_SUCCESS;
            }
            return DAL_ERROR;
         }
         bmsk >>=1;
      }
   }

   return DAL_ERROR;
}

static DALResult _FreeBit(uint32 *pMask, uint32 uMaxBits, uint32 uBitPos)
{
   uint32 i=0;

   if(uBitPos > uMaxBits)
      return DAL_ERROR;

   while(uBitPos>32)
   {
      uBitPos -=32;
      i++;
   }

   pMask[i] &= ~(1<<uBitPos);

   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
STMTrace_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/
const DALREG_DriverInfo
DALSTMTrace_DriverInfo = { STMTrace_DeviceAttach, 0, NULL };

const DALREG_DriverInfo
DALSTMTrace_SLPI_DriverInfo = { STMTrace_DeviceAttach, 0, NULL };



/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32
STMTrace_AddRef(DalSTMTraceHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
STMTrace_DriverInit(STMTraceDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
STMTrace_DriverDeInit(STMTraceDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
STMTrace_DeviceDetach(uint32 ClientId, DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult
STMTrace_DeviceInit(uint32 ClientId, DalDeviceHandle *h)
{
   STMTraceClientCtxt *pClientCtxt = ((DalSTMTraceHandle *)h)->pClientCtxt;
   STMTraceDevCtxt *pDevCtxt = pClientCtxt->pSTMTraceDevCtxt;

   if(DAL_SUCCESS != _CollectProperties(pDevCtxt))
      return DAL_ERROR;

   if(DAL_SUCCESS != _AllocateMemForBitMasks(&pDevCtxt->portRange.pBitMask,
                                               pDevCtxt->portRange.uNumPorts))
   {
      return DAL_ERROR;
   }

   if(DAL_SUCCESS != _AllocateMemForBitMasks(&pDevCtxt->portRangeDMA.pBitMask,
                                               pDevCtxt->portRangeDMA.uNumPorts))
   {
      return DAL_ERROR;
   }

   if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                                       &pDevCtxt->hSync, NULL))
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

static DALResult
STMTrace_DeviceDeInit(uint32 ClientId, DalDeviceHandle *h)
{
   STMTraceClientCtxt *pClientCtxt = ((DalSTMTraceHandle *)h)->pClientCtxt;
   STMTraceDevCtxt *pDevCtxt = pClientCtxt->pSTMTraceDevCtxt;

  /* also delete the list of ports here */

   if(pDevCtxt->hSync)
   {
      DALSYS_DestroyObject(pDevCtxt->hSync);
   }

   if(pDevCtxt->portRangeDMA.pBitMask)
   {
      DALSYS_Free(pDevCtxt->portRangeDMA.pBitMask);
   }

   if(pDevCtxt->portRange.pBitMask)
   {
      DALSYS_Free(pDevCtxt->portRange.pBitMask);
   }

   return DAL_SUCCESS;
}

static DALResult
STMTrace_DevicePowerEvent(uint32 ClientId, DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
STMTrace_DeviceOpen(uint32 ClientId, DalDeviceHandle* h, uint32 mode)
{
   return DAL_SUCCESS;
}

static DALResult
STMTrace_DeviceClose(uint32 ClientId, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
STMTrace_DeviceInfo(uint32 ClientId, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALSTMTRACE_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
STMTrace_DeviceSysRequest(uint32 ClientId, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalSTMTrace Interface.
------------------------------------------------------------------------------*/

static DALResult
STMTrace_NewPort(uint32 ClientId, DalDeviceHandle * h, uint32 * uPort, uint32 * uPortDMA)
{
   STMTraceClientCtxt *pClientCtxt = ((DalSTMTraceHandle *)h)->pClientCtxt;
   STMTraceDevCtxt *pDevCtxt = pClientCtxt->pSTMTraceDevCtxt;
   STMPortRange *pMainPortRange;
   STMPortRangeItem * currRange = NULL;
   STMPortRangeItem * prevRange = NULL;
   uint32 uBitPos;
   DALSYSMemInfo memInfo;


   if(NULL != uPortDMA) // DMA'able port requested ?
   {
      pMainPortRange = &pDevCtxt->portRangeDMA;
      if(!pMainPortRange->uNumPorts)
      {
         return DAL_ERROR;
      }
   }
   else
   {
      pMainPortRange = &pDevCtxt->portRange;
   }

   DALSYS_SyncEnter(pDevCtxt->hSync);

   currRange = pDevCtxt->listHead;
   while(NULL != currRange)
   {
      // previously allocated range has a free port?
      if(0 != currRange->uNumFreePorts)
         goto alloc_port;

      prevRange = currRange;
      currRange = currRange->next;
   }

   // carve out a new range from the master range now
   if(DAL_SUCCESS != DALSYS_Malloc(sizeof(STMPortRangeItem),(void **)&currRange))
   {
      DALSYS_SyncLeave(pDevCtxt->hSync);
      return DAL_ERROR;
   }

   memset(currRange, 0, sizeof(STMPortRangeItem));

   if(DAL_SUCCESS != _AllocBit(pMainPortRange->pBitMask,
                                pMainPortRange->uNumPorts/MAX_STM_PORTS_PER_PAGE,
                                &uBitPos))
   {
      DALSYS_Free(currRange);
      DALSYS_SyncLeave(pDevCtxt->hSync);
      return DAL_ERROR;
   }
   currRange->portRange.uBasePort = pMainPortRange->uBasePort + (uBitPos * MAX_STM_PORTS_PER_PAGE);
   currRange->portRange.uNumPorts = MAX_STM_PORTS_PER_PAGE;
   currRange->uNumFreePorts = MAX_STM_PORTS_PER_PAGE;
   currRange->mainPortRange = pMainPortRange;

   if(DAL_SUCCESS != _AllocateMemForBitMasks(&currRange->portRange.pBitMask, MAX_STM_PORTS_PER_PAGE))
   {
      _FreeBit(pMainPortRange->pBitMask, pMainPortRange->uNumPorts/MAX_STM_PORTS_PER_PAGE, uBitPos);
      DALSYS_Free(currRange);
      DALSYS_SyncLeave(pDevCtxt->hSync);
      return DAL_ERROR;
   }

   if(DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO, DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                           pDevCtxt->uSTMSPBaseAddr +
                                           (currRange->portRange.uBasePort * STM_PORT_SIZE),
                                           4096, &currRange->hMem, NULL))
   {
      _FreeBit(pMainPortRange->pBitMask, pMainPortRange->uNumPorts/MAX_STM_PORTS_PER_PAGE, uBitPos);
      DALSYS_Free(currRange->portRange.pBitMask);
      DALSYS_Free(currRange);
      DALSYS_SyncLeave(pDevCtxt->hSync);
      return DAL_ERROR;
   }

   DALSYS_MemInfo(currRange->hMem, &memInfo);
   currRange->uBaseAddr = memInfo.VirtualAddr;

   if(NULL != prevRange)
   {
      prevRange->next = currRange;
   }
   else
   {
      pDevCtxt->listHead = currRange;
   }

alloc_port:

   if(DAL_SUCCESS != _AllocBit(currRange->portRange.pBitMask, MAX_STM_PORTS_PER_PAGE, &uBitPos))
   {
      DALSYS_SyncLeave(pDevCtxt->hSync);
      return DAL_ERROR;
   }

   *uPort = currRange->uBaseAddr + (STM_PORT_SIZE * uBitPos);
   if(NULL != uPortDMA)
   {
      /* prepare a 32 bit int with 7 bits starting at 17th bit */
      uint32 uPortOffset = ((currRange->portRange.uBasePort+uBitPos)&0x7F)<<17;
      /* first get the physical address of the allocated port */
      *uPortDMA = pDevCtxt->uSTMSPBaseAddr +
                  ((currRange->portRange.uBasePort + uBitPos) * STM_PORT_SIZE);
      /* then set the 7 bits starting at 17th bit pos to 0's */
      *uPortDMA = *uPortDMA &(~(0x7F<<17));
      /* finally set the lower 7 bits of the stimulus port number at the 17th bit pos */
      *uPortDMA |= uPortOffset;
   }
   currRange->uNumFreePorts--;

   DALSYS_SyncLeave(pDevCtxt->hSync);

   return DAL_SUCCESS;
}

static void
STMTrace_FreePort(uint32 ClientId, DalDeviceHandle * h, uint32 uPort)
{
   STMTraceClientCtxt *pClientCtxt = ((DalSTMTraceHandle *)h)->pClientCtxt;
   STMTraceDevCtxt *pDevCtxt = pClientCtxt->pSTMTraceDevCtxt;

   STMPortRangeItem * currRange = NULL;
   STMPortRangeItem * prevRange = NULL;
   uint32 uBitPos = 0;

   DALSYS_SyncEnter(pDevCtxt->hSync);
   currRange =  pDevCtxt->listHead;
   while(NULL != currRange)
   {
      // does the uPort fall in the current range ?
      if(currRange->uBaseAddr <= uPort &&
         uPort < currRange->uBaseAddr + STM_PORT_SIZE*currRange->portRange.uNumPorts)
      {
         goto free_port;
      }

      prevRange = currRange;
      currRange = currRange->next;
   }
   DALSYS_SyncLeave(pDevCtxt->hSync);
   return;

free_port:

   uBitPos = (uPort-currRange->uBaseAddr)/STM_PORT_SIZE;

   if(DAL_SUCCESS != _FreeBit(currRange->portRange.pBitMask, MAX_STM_PORTS_PER_PAGE, uBitPos))
   {
      DALSYS_SyncLeave(pDevCtxt->hSync);
      return;
   }

   currRange->uNumFreePorts++;
   if(currRange->uNumFreePorts == currRange->portRange.uNumPorts)
   {
      // all ports in this range have been freed so free the bit from
      // main mask
      if(DAL_SUCCESS != _FreeBit(currRange->mainPortRange->pBitMask,
                                 currRange->mainPortRange->uNumPorts/MAX_STM_PORTS_PER_PAGE,
                                 currRange->portRange.uBasePort/MAX_STM_PORTS_PER_PAGE))
      {
         DALSYS_SyncLeave(pDevCtxt->hSync);
         return;
      }

      if(NULL != currRange->hMem)
      {
         DALSYS_DestroyObject(currRange->hMem);
      }

      if(NULL != prevRange)
      {
         prevRange->next = currRange->next;
      }
      else
      {
         pDevCtxt->listHead = pDevCtxt->listHead->next;
      }

      DALSYS_Free(currRange->portRange.pBitMask);
      DALSYS_Free(currRange);
   }
   DALSYS_SyncLeave(pDevCtxt->hSync);
   return;
}

static void
STMTrace_InitInterface(STMTraceClientCtxt* pclientCtxt)
{
   static const DalSTMTrace vtbl =
   {
      {
         STMTrace_DeviceAttach,
         STMTrace_DeviceDetach,
         STMTrace_DeviceInit,
         STMTrace_DeviceDeInit,
         STMTrace_DeviceOpen,
         STMTrace_DeviceClose,
         STMTrace_DeviceInfo,
         STMTrace_DevicePowerEvent,
         STMTrace_DeviceSysRequest
      },

      STMTrace_NewPort,
      STMTrace_FreePort,
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalSTMTraceHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalSTMTraceHandle.pVtbl  = &vtbl;
   pclientCtxt->DalSTMTraceHandle.pClientCtxt = pclientCtxt;
}

DALResult
STMTrace_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   STMTraceClientCtxt *pclientCtxt =NULL;
   STMTraceDrvCtxt *pdrvCtxt = NULL;
   const char *pszDev_SLPI="DALDEVICEID_STM_TRACE_SLPI";

   static STMTraceDrvCtxt drvCtxt = {{STMTrace_DriverInit,
                                      STMTrace_DriverDeInit
                                     },1,
                                     sizeof(STMTraceDevCtxt)};
   static STMTraceClientCtxt clientCtxt;


   static STMTraceDrvCtxt drvCtxt_SLPI = {{STMTrace_DriverInit,
                                      STMTrace_DriverDeInit
                                     },1,
                                     sizeof(STMTraceDevCtxt)};
   static STMTraceClientCtxt clientCtxt_SLPI;



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

   STMTrace_InitInterface(pclientCtxt);
   STMTrace_AddRef(&(pclientCtxt->DalSTMTraceHandle));
   TRY(nErr,DALSYS_GetDALPropertyHandleStr(pszArg,
                                           pclientCtxt->pSTMTraceDevCtxt->hProp));

   *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalSTMTraceHandle);

   nErr = DAL_SUCCESS;

   CATCH(nErr) {}
   return nErr;
}



static int QDSSGetVirtualAddr(uint32 physicalAddr,
                                 uint32 *pVirtualAddr) 
{
   int nErr= DAL_ERROR;
   DALSYSMemHandle hMem = NULL;   // handle to the allocated memory region
   DALSYSMemInfo memInfo;

   TRY(nErr,DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  physicalAddr,
                                  4096, &hMem, NULL));
   
   TRY(nErr,DALSYS_MemInfo(hMem, &memInfo));
   *pVirtualAddr = memInfo.VirtualAddr;
   nErr= DAL_SUCCESS;

   CATCH(nErr) {}
   if (NULL!= hMem) {
      //Unless unmapped it can't be mapped in another PD.
      DALSYS_DestroyObject(hMem);
   }
   return nErr;
}

/*
 Ideally, we should be able to add a virtual page to the island spec
 when the STM trace driver allocates a page from the sensor user PD.
 Since the island spec API does not have that option right now, we map a
 a page in the STM stimulus port address range for sensor PD in the 
  guest OS itself on initialization, to obtain the virtual  address and 
  then free the page. This works  for now, since the 
  Virtual address mapping of the STM stimulus port address is the 
  same in sensor PD as well as guest OS. If this behaviour changes we will
  need the aforementioned method to add a page to the island spec on 
  demand from auser PD.
 */

#define QDSS_STM_BASE_PHYS_OS         0x62AF0000
#define QDSS_STM_BASE_PHYS_SENSOR     0x62AF4000

#define QDSS_SSC_QDSSCSR_BASE_PHYS    0x62B07000


#define NUM_UIMAGE_HWIO_PAGES 3

int QDSSGetUImageHWIOAddrs(uint32 *pVirtAddr,int nAddrs, int *pnAddrsReq)
{
   int nErr = DAL_ERROR;

   *pnAddrsReq = NUM_UIMAGE_HWIO_PAGES;

   if (nAddrs < NUM_UIMAGE_HWIO_PAGES) {
      return DAL_ERROR_INSUFFICIENT_MEMORY;
   }

   TRY(nErr,QDSSGetVirtualAddr(QDSS_STM_BASE_PHYS_OS,&pVirtAddr[0]));
   TRY(nErr,QDSSGetVirtualAddr(QDSS_STM_BASE_PHYS_SENSOR,&pVirtAddr[1]));
   TRY(nErr,QDSSGetVirtualAddr(QDSS_SSC_QDSSCSR_BASE_PHYS,&pVirtAddr[2]));
  
   
   CATCH(nErr) {}

   return nErr;
}



int QDSSGetSTMBaseAddrADSP(uint32 *pVirtualAddr) 
{
   return QDSSGetVirtualAddr(QDSS_STM_BASE_PHYS_OS,pVirtualAddr);
}

int QDSSGetSTMBaseAddrSensor(uint32 *pVirtualAddr) 
{
   return QDSSGetVirtualAddr(QDSS_STM_BASE_PHYS_SENSOR,pVirtualAddr);
}

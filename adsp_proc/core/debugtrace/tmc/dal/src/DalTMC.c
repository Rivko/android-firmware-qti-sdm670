/*=============================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
           All Rights Reserved.

=============================================================================*/

#include "halqdss_etfetb.h"
#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDITMC.h"
#include "HALhwio.h"
#include "qdss_utils.h"


/*---------------------------------------------------------------------------
Declaring a "TMC" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct TMCDrvCtxt TMCDrvCtxt;
typedef struct TMCDevCtxt TMCDevCtxt;
typedef struct TMCClientCtxt TMCClientCtxt;

/*---------------------------------------------------------------------------
Declaring a private "TMC" Vtable
------------------------------------------------------------------------------*/
typedef struct TMCDALVtbl TMCDALVtbl;
struct TMCDALVtbl
{
   int (*TMC_DriverInit)(TMCDrvCtxt *);
   int (*TMC_DriverDeInit)(TMCDrvCtxt *);
};

struct TMCDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   TMCDrvCtxt  *pTMCDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //TMC Dev state can be added by developers here
   DALSYSMemHandle hMemETB;
   DALSYSMemInfo memInfoETB;
};

struct TMCDrvCtxt
{
   //Base Members
   TMCDALVtbl TMCDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   TMCDevCtxt TMCDevCtxt[1];
   //TMC Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "TMC" Client Context
------------------------------------------------------------------------------*/
struct TMCClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   TMCDevCtxt *pTMCDevCtxt;
   DalTMCHandle DalTMCHandle;
   //TMC Client state can be added by developers here
};

#define TMC_PAGE_SIZE 4096

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
TMC_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/

const DALREG_DriverInfo
DALTMC_DriverInfo = { TMC_DeviceAttach, 0, NULL};

const DALREG_DriverInfo
DALTMC_SLPI_DriverInfo = { TMC_DeviceAttach, 0, NULL};



/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

static uint32
TMC_AddRef(DalTMCHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}


/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
TMC_DriverInit(TMCDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
TMC_DriverDeInit(TMCDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
TMC_DeviceDetach(uint32 ClientHandle, DalDeviceHandle* h )
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult TMC_MemMap(uint32 physAddr,
                            DALSYSMemHandle *phMem,
                            DALSYSMemInfo *pMemInfo)
{
   int nErr;

   TRY(nErr,DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  physAddr,
                                  TMC_PAGE_SIZE, phMem, NULL ));
   DALSYS_MemInfo(*phMem, pMemInfo);

   CATCH(nErr) {}

   return nErr;
}


static DALResult
TMC_DeviceInit(uint32 ClientHandle, DalDeviceHandle *h)
{
   int nErr;
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   DALSYSPropertyVar etb_phys_addr_var;                   

   TRY(nErr,DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                     "etb_phys_addr",
                                     0,
                                     &etb_phys_addr_var));

   TRY(nErr,TMC_MemMap(etb_phys_addr_var.Val.dwVal,
                       &pDevCtxt->hMemETB,
                       &pDevCtxt->memInfoETB));

   HAL_qdss_etfetb_HalConfigInit(pDevCtxt->memInfoETB.VirtualAddr);
   HAL_qdss_etfetb_AccessUnLock();

   if (HAL_qdss_etfetb_IsEnabled()) {
      //ETB got initialized from RPM (e.g for sysinit test)
      //Do nothing, we want to preserve what is in there
   }
   else {
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_CBUF);
      HAL_qdss_etfetb_SetFFCRETBDefault(QDSS_TMC_MODE_CBUF);
      HAL_qdss_etfetb_EnableTraceCapture();
   }
   HAL_qdss_etfetb_AccessLock();


   CATCH(nErr) {}
   return nErr;
}


static DALResult
TMC_DeviceDeInit(uint32 ClientHandle, DalDeviceHandle *h)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;

   if(pDevCtxt->hMemETB) {
      DALSYS_DestroyObject(pDevCtxt->hMemETB);
   }

   return DAL_SUCCESS;
}

static DALResult
TMC_DevicePowerEvent(uint32 ClientHandle, DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
TMC_DeviceOpen(uint32 ClientHandle, DalDeviceHandle* h , uint32 mode)
{

   return DAL_SUCCESS;
}

static DALResult
TMC_DeviceClose(uint32 ClientHandle, DalDeviceHandle* h )
{
   return DAL_SUCCESS;
}

static DALResult
TMC_DeviceInfo(uint32 ClientHandle, DalDeviceHandle* h , DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTMC_STRING_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
TMC_DeviceSysRequest(uint32 ClientHandle, DalDeviceHandle* h ,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTMC Interface.
------------------------------------------------------------------------------*/
static  DALResult
TMC_GetMode(uint32 ClientHandle, DalDeviceHandle * _h, uint8 *pMode)
{
   *pMode = (uint8) HAL_qdss_etfetb_GetMode();
   return DAL_SUCCESS;
}


static DALResult TMC_DisableTrace(uint32 ClientHandle, DalDeviceHandle * h);


static  DALResult TMC_SetMode(uint32 ClientHandle, DalDeviceHandle * _h,uint8 mode)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(_h->pClientCtxt))->pDALDevCtxt;

   if (mode > TMC_MODE_HW_FIFO) {
      return DAL_ERROR_INVALID_PARAMETER;
   }

   //Disable trace before switching mode
   //Needs to be enabled by calling EnableTrace

   TMC_DisableTrace(ClientHandle, _h);
   HAL_qdss_etfetb_HalConfigInit(pDevCtxt->memInfoETB.VirtualAddr);
   HAL_qdss_etfetb_AccessUnLock();
   HAL_qdss_etfetb_SetMode(mode);
   HAL_qdss_etfetb_AccessLock();
   
   IGNORE(pDevCtxt);
   return DAL_SUCCESS;
}

static  DALResult
TMC_GetTraceBuffer(uint32 ClientHandle, DalDeviceHandle * _h,uint32 *pdwAddr, uint32 *pdwBufLen)
{
   return DAL_ERROR_NOT_SUPPORTED;
}

static  DALResult
TMC_SetTraceBuffer(uint32 ClientHandle, DalDeviceHandle * _h,uint32 dwAddr, uint32 dwBufLen)
{
   return DAL_ERROR_NOT_SUPPORTED;
}



static DALResult
TMC_EnableTrace(uint32 ClientHandle,  DalDeviceHandle * h)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;

   HAL_qdss_etfetb_HalConfigInit(pDevCtxt->memInfoETB.VirtualAddr);
   HAL_qdss_etfetb_AccessUnLock();
   if (TMC_MODE_CIRCULAR_BUFFER==HAL_qdss_etfetb_GetMode()) { 
      HAL_qdss_etfetb_SetFFCRETBDefault(QDSS_TMC_MODE_CBUF);
   }
   else {
      HAL_qdss_etfetb_SetFFCRETBDefault(QDSS_TMC_MODE_HWFIFO);
   }

   HAL_qdss_etfetb_EnableTraceCapture();
   HAL_qdss_etfetb_AccessLock();

   IGNORE(pDevCtxt);
   return DAL_SUCCESS;
}


static DALResult
TMC_DisableTrace(uint32 ClientHandle,  DalDeviceHandle * h)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;

   HAL_qdss_etfetb_HalConfigInit(pDevCtxt->memInfoETB.VirtualAddr);
   HAL_qdss_etfetb_AccessUnLock();
   HAL_qdss_etfetb_DisableTraceCapture();
   HAL_qdss_etfetb_AccessLock();
   return DAL_SUCCESS;
}


static DALResult
ETB_ReadTrace(TMCDevCtxt *pDevCtxt, byte * buf, uint32  nBufLen, uint32 *pnLenRead)
{
   int nErr = 0;
   uint32 dwVal = 0;
   uint32 i = 0;
   uint32 *pdwBuf = (uint32 *)buf;

   HAL_qdss_etfetb_HalConfigInit(pDevCtxt->memInfoETB.VirtualAddr);
   HAL_qdss_etfetb_AccessUnLock();

   if (!HAL_qdss_etfetb_IsEnabled()) {
      THROW(nErr,DAL_ERROR);
   }

   while (i <= nBufLen-4 )  {
      dwVal = HAL_qdss_etfetb_ReadData();
      if (0xFFFFFFFF == dwVal) {
         break;
      }
      *pdwBuf = dwVal;
      pdwBuf++;
      i+=4;
   }

   if (pnLenRead) {
      *pnLenRead = i;
   }

   nErr =  DAL_SUCCESS;
   CATCH(nErr) {}
   HAL_qdss_etfetb_AccessLock();
   return nErr;
}



static DALResult
TMC_ReadTrace(uint32 ClientHandle,  DalDeviceHandle * h, byte * buf, uint32  nBufLen, uint32 *pnLenRead)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;

   if (TMC_MODE_CIRCULAR_BUFFER!=HAL_qdss_etfetb_GetMode()) { 
      return DAL_ERROR_NOT_ALLOWED;
   }

   return ETB_ReadTrace(pDevCtxt,buf,nBufLen,pnLenRead);
}

static DALResult ETB_Flush(DalDeviceHandle * h,int flags)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;

   HAL_qdss_etfetb_HalConfigInit(pDevCtxt->memInfoETB.VirtualAddr);
   HAL_qdss_etfetb_AccessUnLock();

   if (flags & TMC_FLUSH_STOP) {
      HAL_qdss_etfetb_StopOnFlush(1);
   }

   HAL_qdss_etfetb_Flush();

   if (flags & TMC_FLUSH_STOP) {
      HAL_qdss_etfetb_WaitForTMCReady();
      HAL_qdss_etfetb_StopOnFlush(0);
   }

   if (flags & TMC_FLUSH_BLOCK) {
      HAL_qdss_etfetb_FlushWait();
   }
   HAL_qdss_etfetb_AccessLock();
   return DAL_SUCCESS;
}



static DALResult
TMC_Flush(uint32 ClientHandle,  DalDeviceHandle * h, int flags)
{
   return ETB_Flush(h,flags);
}


static DALResult
TMC_GetParam(uint32 ClientHandle,  DalDeviceHandle * h, uint32  param, uint32  *puVal)
{
   return DAL_ERROR_NOT_SUPPORTED;
}


static DALResult
TMC_SetParam(uint32 ClientHandle,  DalDeviceHandle * h, uint32  param, uint32  uVal)
{
   return DAL_ERROR_NOT_SUPPORTED;
}

static void TMC_SaveRegs(uint32 base_addr, byte *buf, uint32 buf_size)
{
   uint32 offset;
   uint32 max_offset;
#define TMC_RRD_OFFSET	0x10
#define TMC_RWD_OFFSET	0x24


   max_offset = (buf_size >TMC_PAGE_SIZE)  ? TMC_PAGE_SIZE:buf_size;

   for (offset=0; offset+4 <= max_offset; offset+=4) {

      if ((TMC_RRD_OFFSET==offset) || (TMC_RWD_OFFSET == offset)) {
         continue;
      }

      __outpdw((buf+offset), __inpdw((base_addr+offset)));
   }
}


static DALResult
TMC_SaveRegisters(uint32 ClientHandle, DalDeviceHandle * h, byte * buf, uint32  nBufLen)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   int nErr=DAL_SUCCESS;


   HAL_qdss_etfetb_HalConfigInit(pDevCtxt->memInfoETB.VirtualAddr);
   HAL_qdss_etfetb_AccessUnLock();

   TMC_SaveRegs(pDevCtxt->memInfoETB.VirtualAddr,buf,nBufLen);

   HAL_qdss_etfetb_AccessLock();

   return nErr;
}



static void
TMC_InitInterface(TMCClientCtxt* pclientCtxt)
{
 static const DalTMC vtbl =
   {
      {
         TMC_DeviceAttach,
         TMC_DeviceDetach,
         TMC_DeviceInit,
         TMC_DeviceDeInit,
         TMC_DeviceOpen,
         TMC_DeviceClose,
         TMC_DeviceInfo,
         TMC_DevicePowerEvent,
         TMC_DeviceSysRequest
      },

      TMC_GetMode,
      TMC_SetMode,
      TMC_GetTraceBuffer,
      TMC_SetTraceBuffer,
      TMC_EnableTrace,
      TMC_DisableTrace,
      TMC_ReadTrace,
      TMC_Flush,
      TMC_GetParam,
      TMC_SetParam,
      TMC_SaveRegisters,
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalTMCHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalTMCHandle.pVtbl  = &vtbl;
   pclientCtxt->DalTMCHandle.pClientCtxt = pclientCtxt;
}

DALResult
TMC_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   TMCClientCtxt *pclientCtxt =NULL;
   TMCDrvCtxt *pdrvCtxt = NULL;

   static TMCDrvCtxt drvCtxt = {{TMC_DriverInit,
                                 TMC_DriverDeInit
                                },1,
                                sizeof(TMCDevCtxt)};
   static TMCClientCtxt clientCtxt;


   static TMCDrvCtxt drvCtxt_SLPI = {{TMC_DriverInit,
                                      TMC_DriverDeInit
                                },1,
                                sizeof(TMCDevCtxt)};
   static TMCClientCtxt clientCtxt_SLPI;

   const char *pszDev_SLPI="DALDEVICEID_TMC_SLPI";
   
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
   TMC_InitInterface(pclientCtxt);
   TMC_AddRef(&(pclientCtxt->DalTMCHandle));
   TRY(nErr,DALSYS_GetDALPropertyHandleStr(pszArg,
                                           pclientCtxt->pTMCDevCtxt->hProp));


   *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalTMCHandle);
   CATCH(nErr) {}

   return nErr;
}

/*
 *This helper function is called befer SLPI enters island mode 
 */

void ETBSwitchToCBUF(void) 
{
   HAL_qdss_etfetb_AccessUnLock();
 
   if (HAL_qdss_etfetb_IsEnabled()) {
      uint32 ffcr_saved;
      ffcr_saved = HAL_qdss_etfetb_GetFFCR();
      HAL_qdss_etfetb_StopOnFlush(1);
      HAL_qdss_etfetb_Flush();
      HAL_qdss_etfetb_FlushWait();
      HAL_qdss_etfetb_WaitForTMCReadyVal(1);
      HAL_qdss_etfetb_DisableTraceCapture();
      HAL_qdss_etfetb_SetFFCR(ffcr_saved);
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_CBUF);
      HAL_qdss_etfetb_EnableTraceCapture();
      HAL_qdss_etfetb_WaitForTMCReadyVal(0);
   }

   HAL_qdss_etfetb_AccessLock();
}

/*
 *This helper function is called after SLPI exits island mode 
 */
void ETBSwitchToHWFIFO(void) 
{
   uint32 ffcr_saved;

   HAL_qdss_etfetb_AccessUnLock();
   ffcr_saved = HAL_qdss_etfetb_GetFFCR();
   HAL_qdss_etfetb_StopOnFlush(1);
   HAL_qdss_etfetb_Flush();
   HAL_qdss_etfetb_FlushWait();
   HAL_qdss_etfetb_WaitForTMCReadyVal(1);
   HAL_qdss_etfetb_Drain();
   HAL_qdss_etfetb_WaitForTMCReadyVal(1);
   HAL_qdss_etfetb_DisableTraceCapture();
   HAL_qdss_etfetb_SetFFCR(ffcr_saved);
   HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_HWFIFO);

   HAL_qdss_etfetb_EnableTraceCapture(); 
   HAL_qdss_etfetb_WaitForTMCReadyVal(0);
   HAL_qdss_etfetb_AccessLock();
}

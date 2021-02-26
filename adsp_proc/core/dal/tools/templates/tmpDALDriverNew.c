/*=============================================================================
  Copyright (c) 2009 Qualcomm Technologies Incorporated.
           All Rights Reserved.

                  $Header: //components/rel/core.qdsp6/2.1/dal/tools/templates/tmpDALDriverNew.c#1 $
=============================================================================*/

#include "DALFramework.h"
#include "DALDevice.h"
#include "<interface_header_file>"
#include "string.h" //for memset warning


/*------------------------------------------------------------------------------
Declaring a "<driver>" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct <driver>DrvCtxt <driver>DrvCtxt;
typedef struct <driver>DevCtxt <driver>DevCtxt;
typedef struct <driver>ClientCtxt <driver>ClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "<driver>" Vtable
------------------------------------------------------------------------------*/
typedef struct <driver>DALVtbl <driver>DALVtbl;
struct <driver>DALVtbl
{
   int (*<driver>_DriverInit)(<driver>DrvCtxt *);
   int (*<driver>_DriverDeInit)(<driver>DrvCtxt *);
};

struct <driver>DevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   <driver>DrvCtxt  *p<driver>DrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //<driver> Dev state can be added by developers here
};

struct <driver>DrvCtxt
{
   //Base Members
   <driver>DALVtbl <driver>DALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   <driver>DevCtxt <driver>DevCtxt[<driver_num_devices>];
   //<driver> Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "<driver>" Client Context
------------------------------------------------------------------------------*/
struct <driver>ClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   <driver>DevCtxt *p<driver>DevCtxt;
   <interface>Handle <interface>Handle;
   //<driver> Client state can be added by developers here
};




/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
<driver>_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/
static DALDEVICEID Dal<driver>_DeviceId[<driver_num_devices>] = {<device_id_list>};

const DALREG_DriverInfo
DAL<driver>_DriverInfo = { <driver>_DeviceAttach, <driver_num_devices>, Dal<driver>_DeviceId };


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32
<driver>_AddRef(<interface>Handle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
<driver>_DriverInit(<driver>DrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
<driver>_DriverDeInit(<driver>DrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
<driver>_DeviceDetach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   <non_static_client_context_req> if(0 == dwref)  {
   <non_static_client_context_req> //Release the client ctxt;
   <non_static_client_context_req>    DALSYS_Free(h->pClientCtxt);
   <non_static_client_context_req>}
   return dwref;
}

static DALResult
<driver>_DeviceInit(DalDeviceHandle *h)
{
   return DAL_SUCCESS;
}

static DALResult
<driver>_DeviceDeInit(DalDeviceHandle *h)
{
   return DAL_SUCCESS;
}

static DALResult
<driver>_DevicePowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
<driver>_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{
   return DAL_SUCCESS;
}

static DALResult
<driver>_DeviceClose(DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
<driver>_DeviceInfo(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = <Cdriver>_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
<driver>_DeviceSysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in <interface> Interface.
------------------------------------------------------------------------------*/

<cs_user_interface_function_definition>

static void
<driver>_InitInterface(<driver>ClientCtxt* pclientCtxt)
{
   static const <interface> vtbl =
   {
      {
         <driver>_DeviceAttach,
         <driver>_DeviceDetach,
         <driver>_DeviceInit,
         <driver>_DeviceDeInit,
         <driver>_DeviceOpen,
         <driver>_DeviceClose,
         <driver>_DeviceInfo,
         <driver>_DevicePowerEvent,
         <driver>_DeviceSysRequest
      },
   <user_interface_function_vtbl_entry>
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt-><interface>Handle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt-><interface>Handle.pVtbl  = &vtbl;
   pclientCtxt-><interface>Handle.pClientCtxt = pclientCtxt;
}

DALResult
<driver>_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static <driver>DrvCtxt drvCtxt = {{<driver>_DriverInit,
                                      <driver>_DriverDeInit
                                     },<driver_num_devices>,
                                     sizeof(<driver>DevCtxt)};
   <static_client_context_declaration> <driver>ClientCtxt clientCtxt;

   <driver>ClientCtxt *pclientCtxt = <client_context_declaration>;

   <non_static_client_context_req>nErr = DALSYS_Malloc(sizeof(<driver>ClientCtxt),(void **)&pclientCtxt);
   <non_static_client_context_req>DALSYS_memset(pclientCtxt,0,sizeof(<driver>ClientCtxt));

   *phDalDevice = NULL;

   <non_static_client_context_req> if (DAL_SUCCESS == nErr) {
   nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      <driver>_InitInterface(pclientCtxt);
      <driver>_AddRef(&(pclientCtxt-><interface>Handle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt-><interface>Handle);
   }
   <non_static_client_context_req> }
   return nErr;
}




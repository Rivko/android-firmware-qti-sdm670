/*=============================================================================
  Copyright (c) 2009 Qualcomm Technologies Incorporated.
           All Rights Reserved.

                  $Header: //components/rel/core.qdsp6/2.1/settings/deviceconfig/config/StringDeviceOne/DalTestStringOne.c#1 $
=============================================================================*/

#include "DALFramework.h"
#include "DalDevice.h"
#include "DDIStringDeviceOne.h"
#include "string.h" //for memset warning

/*------------------------------------------------------------------------------
Declaring a "TestString" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct TestStringDrvCtxt TestStringDrvCtxt;
typedef struct TestStringDevCtxt TestStringDevCtxt;
typedef struct TestStringClientCtxt TestStringClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "TestString" Vtable
------------------------------------------------------------------------------*/
typedef struct TestStringDALVtbl TestStringDALVtbl;
struct TestStringDALVtbl
{
   int (*TestString_DriverInit)(TestStringDrvCtxt *);
   int (*TestString_DriverDeInit)(TestStringDrvCtxt *);
};

struct TestStringDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   TestStringDrvCtxt  *pTestStringDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   void *hDALSystem;
   void *hDALTimer;
   void *hDALInterrupt;
   void * pSystemTbl;
   void * pDefaultWorkLoop;
   const char *strDeviceName;
   uint32 Reserved[16-6];
};

struct TestStringDrvCtxt
{
   //Base Members
   TestStringDALVtbl TestStringDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   TestStringDevCtxt TestStringDevCtxt[13];
   //TestString Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "TestString" Client Context
------------------------------------------------------------------------------*/
struct TestStringClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   TestStringDevCtxt *pTestStringDevCtxt;
   DalStringDeviceHandle DalStringDeviceHandle;
   //TestString Client state can be added by developers here
};




/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
TestString_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/

//static DALDEVICEID DalTestString_DeviceId[1] = {0xDEADBEEF};
const DALREG_DriverInfo
DALTestStringOne_DriverInfo = { TestString_DeviceAttach,0, NULL };


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32
TestString_AddRef(DalStringDeviceHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
TestString_DriverInit(TestStringDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
TestString_DriverDeInit(TestStringDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
TestString_DeviceDetach(uint32 ClientHandle, DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
    if(0 == dwref)  {
    //Release the client ctxt;
       DALSYS_Free(h->pClientCtxt);
   }
   return dwref;
}

static DALResult
TestString_DeviceInit(uint32 ClientHandle, DalDeviceHandle *h)
{
   return DAL_SUCCESS;
}

static DALResult
TestString_DeviceDeInit(uint32 ClientHandle, DalDeviceHandle *h)
{
   return DAL_SUCCESS;
}

static DALResult
TestString_DevicePowerEvent(uint32 ClientHandle, DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
TestString_DeviceOpen(uint32 ClientHandle, DalDeviceHandle* h, uint32 mode)
{
   return DAL_SUCCESS;
}

static DALResult
TestString_DeviceClose(uint32 ClientHandle, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
TestString_DeviceInfo(uint32 ClientHandle, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALSTRINGDEVICE_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
TestString_DeviceSysRequest(uint32 ClientHandle, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalStringDevice Interface.
------------------------------------------------------------------------------*/



static DALResult 
TestString_ReadTest(uint32 ClientHandle,  DalDeviceHandle * h, void * dest_buffer, uint32  blen) 
{
   //uint32 iStrLen;
   TestStringClientCtxt *pClientCtxt = ((DalStringDeviceHandle *)h)->pClientCtxt;
   strlcpy((char *)dest_buffer,pClientCtxt->pTestStringDevCtxt->strDeviceName,blen);
   //Implementation
   return DAL_SUCCESS;
}


static DALResult 
TestString_WriteTest(uint32 ClientHandle,  DalDeviceHandle * h, void * src_buffer, uint32  blen) 
{
   
   //Implementation
   return DAL_SUCCESS;
}

static void
TestString_InitInterface(TestStringClientCtxt* pclientCtxt)
{
   static const DalStringDevice vtbl =
   {
      {
         TestString_DeviceAttach,
         TestString_DeviceDetach,
         TestString_DeviceInit,
         TestString_DeviceDeInit,
         TestString_DeviceOpen,
         TestString_DeviceClose,
         TestString_DeviceInfo,
         TestString_DevicePowerEvent,
         TestString_DeviceSysRequest
      },
   
      TestString_ReadTest,
      TestString_WriteTest
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalStringDeviceHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalStringDeviceHandle.pVtbl  = &vtbl;
   pclientCtxt->DalStringDeviceHandle.pClientCtxt = pclientCtxt;
}

DALResult
TestString_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{

   DALResult nErr;

   static TestStringDrvCtxt drvCtxt = {{TestString_DriverInit,
                                      TestString_DriverDeInit
                                     },3,
                                     sizeof(TestStringDevCtxt)};

   TestStringClientCtxt *pclientCtxt =  NULL;
          
   nErr = DALSYS_Malloc(sizeof(TestStringClientCtxt),(void **)&pclientCtxt); //Malloc will error fatal upon failure
   DALSYS_memset(pclientCtxt,0,sizeof(TestStringClientCtxt));
   *phDalDevice = NULL;
   nErr = DALFW_AttachToStringDevice(pszArg, (DALDrvCtxt *)&drvCtxt, (DALClientCtxt *)pclientCtxt); 
   if (DAL_SUCCESS == nErr)
   {
      TestString_InitInterface(pclientCtxt);
      TestString_AddRef(&(pclientCtxt->DalStringDeviceHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalStringDeviceHandle);
   }
   return nErr;
}




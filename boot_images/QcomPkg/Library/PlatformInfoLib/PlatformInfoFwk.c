/*
===========================================================================
*/
/**
  @file PlatformInfoFwk.c

  DAL framework implementation file for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================


  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDIPlatformInfo.h"
#include "PlatformInfoInternal.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/

/*
 * Take a DAL driver handle and dereference into the driver context.
 */
#define HANDLE2CTXT(h) \
  &((PlatformInfoDALClientCtxt *)(h->pClientCtxt))->pPlatformInfoDALDevCtxt->pPlatformInfoDALDrvCtxt->DrvCtxt

/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Driver, device and client contexts
 */
typedef struct PlatformInfoDALDrvCtxt PlatformInfoDALDrvCtxt;
typedef struct PlatformInfoDALDevCtxt PlatformInfoDALDevCtxt;
typedef struct PlatformInfoDALClientCtxt PlatformInfoDALClientCtxt;

/**
 * Private PlatformInfo Vtable
 */
typedef struct PlatformInfoDALVtbl PlatformInfoDALVtbl;
struct PlatformInfoDALVtbl
{
   int (*PlatformInfo_DriverInit)(PlatformInfoDALDrvCtxt *);
   int (*PlatformInfo_DriverDeInit)(PlatformInfoDALDrvCtxt *);
};

/**
 * PlatformInfo device context.
 */
struct PlatformInfoDALDevCtxt
{
  /*
   * Base members - do not modify!
   */
   uint32                dwRefs;
   DALDEVICEID           DevId;
   uint32                dwDevCtxtRefIdx;
   PlatformInfoDALDrvCtxt  *pPlatformInfoDALDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32                Reserved[16];
};

/**
 * PlatformInfo client context.
 */
struct PlatformInfoDALClientCtxt
{
  /*
   * Base members - do not modify!
   */
   uint32                  dwRefs;
   uint32                  dwAccessMode;
   void                   *pPortCtxt;
   PlatformInfoDALDevCtxt *pPlatformInfoDALDevCtxt;
   DalPlatformInfoHandle   DalPlatformInfoHandle;
};

/**
 * PlatformInfo driver context.
 */
struct PlatformInfoDALDrvCtxt
{
  /*
   * Base members - do not modify!
   */
  PlatformInfoDALVtbl PlatformInfoDALVtbl;
  uint32              dwNumDev;
  uint32              dwSizeDevCtxt;
  uint32              bInit;
  uint32              dwRefs;
  PlatformInfoDALDevCtxt PlatformInfoDALDevCtxt[1];
};


/*=========================================================================
      Function Prototypes
==========================================================================*/

DALResult PlatformInfo_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);
static DALResult DALPlatformInfo_DriverInit(PlatformInfoDALDrvCtxt *);
static DALResult PlatformInfo_DriverDeInit(PlatformInfoDALDrvCtxt *pPlatformInfo_DriverCtxt);


/*=========================================================================
      Data Declarations
==========================================================================*/

static DALDEVICEID DalPlatformInfo_DeviceId[1] =
{
  DALDEVICEID_PLATFORMINFO
};

const DALREG_DriverInfo DALPlatformInfo_DriverInfo =
{
  PlatformInfo_DeviceAttach, 1, DalPlatformInfo_DeviceId
};

static PlatformInfoDALDrvCtxt DALPlatformInfo_DriverCtxt =
{
  {
    DALPlatformInfo_DriverInit,
    PlatformInfo_DriverDeInit
    },
  1,
  sizeof(PlatformInfoDALDevCtxt)
};


/*=========================================================================
      Functions
==========================================================================*/

static uint32
PlatformInfo_AddRef(DalPlatformInfoHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

static DALResult
PlatformInfo_DriverDeInit(PlatformInfoDALDrvCtxt *pPlatformInfo_DriverCtxt)
{
   return DAL_SUCCESS;
}

static uint32
PlatformInfo_DeviceDetach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult
PlatformInfo_DeviceInit(DalDeviceHandle *h)
{
   return DAL_SUCCESS;
}

static DALResult
PlatformInfo_DeviceDeInit(DalDeviceHandle *h)
{
   return DAL_SUCCESS;
}

static DALResult
PlatformInfo_DevicePowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
PlatformInfo_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{
   return DAL_SUCCESS;
}

static DALResult
PlatformInfo_DeviceClose(DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
PlatformInfo_DeviceInfo(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALPLATFORMINFO_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
PlatformInfo_DeviceSysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

static DALResult DALPlatformInfo_DriverInit(PlatformInfoDALDrvCtxt *pDrvCtxt)
{
  DALResult eResult;

  eResult = DALSYS_GetDALPropertyHandle(DALDEVICEID_PLATFORMINFO,
    pDrvCtxt->PlatformInfoDALDevCtxt[0].hProp);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  return PlatformInfo_Init();
}

static DALResult DALPlatformInfo_GetPlatform (DalDeviceHandle * h, DalPlatformInfoPlatformType * pType)
{
  if (!pType)
  {
    return DAL_ERROR;
  }

  *pType = PlatformInfo_Platform();
  return DAL_SUCCESS;
}

static DALResult DALPlatformInfo_GetPlatformInfo (DalDeviceHandle * h, DalPlatformInfoPlatformInfoType * pInfo)
  { return PlatformInfo_GetPlatformInfo(pInfo); }

static DALResult DALPlatformInfo_GetKeyValue(DalDeviceHandle * h, DalPlatformInfoKeyType eKey, uint32 *nValue)
  { return PlatformInfo_GetKeyValue(eKey, nValue); }

static DALResult DALPlatformInfo_CDTConfigPreDDR (DalDeviceHandle * h, void *pPlatformInfo)
  { return PlatformInfo_CDTConfigPreDDR(pPlatformInfo); }

static DALResult DALPlatformInfo_CDTConfigPostDDR (DalDeviceHandle * h, void *pPlatformInfo)
  { return PlatformInfo_CDTConfigPostDDR(pPlatformInfo); }

/* =========================================================================
**  Function : PlatformInfo_InitInterface
** =========================================================================*/
/**
  Initialize the client context for communicating to the DAL.

  This function initializes the given client context.  It is invoked
  internally when a client attaches to the dal.

  @param *pclientCtxt [in] -- Pointer to the DAL client context.

  @return
  None.

  @dependencies
  None.
*/

static void
PlatformInfo_InitInterface(PlatformInfoDALClientCtxt* pclientCtxt)
{
   static const DalPlatformInfo vtbl =
   {
      {
         PlatformInfo_DeviceAttach,
         PlatformInfo_DeviceDetach,
         PlatformInfo_DeviceInit,
         PlatformInfo_DeviceDeInit,
         PlatformInfo_DeviceOpen,
         PlatformInfo_DeviceClose,
         PlatformInfo_DeviceInfo,
         PlatformInfo_DevicePowerEvent,
         PlatformInfo_DeviceSysRequest
      },

      DALPlatformInfo_GetPlatform,
      DALPlatformInfo_GetPlatformInfo,
      DALPlatformInfo_GetKeyValue,
      DALPlatformInfo_CDTConfigPreDDR,
      DALPlatformInfo_CDTConfigPostDDR
   };

   pclientCtxt->DalPlatformInfoHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalPlatformInfoHandle.pVtbl  = &vtbl;
   pclientCtxt->DalPlatformInfoHandle.pClientCtxt = pclientCtxt;

} /* END PlatformInfo_InitInterface */


/* =========================================================================
**  Function : PlatformInfo_DeviceAttach
** =========================================================================*/
/**
  Internal DAL framework function for attaching to the PlatformInfo DAL.

  This function attaches to the PlatformInfo DAL and initializes the given
  device handle.

  @param *pszArg[in] -- String passed by user during attach.
  @param DeviceId -- The device id, should be DALDEVICEID_PLATFORMINFO
  @param phDalDevice[out] -- DAL device handle to fill in.

  @return
  None.

  @dependencies
  None.
*/

DALResult
PlatformInfo_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                          DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static PlatformInfoDALClientCtxt clientCtxt;

   PlatformInfoDALClientCtxt *pclientCtxt = &clientCtxt;


   *phDalDevice = NULL;

   nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&DALPlatformInfo_DriverCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      PlatformInfo_InitInterface(pclientCtxt);
      PlatformInfo_AddRef(&(pclientCtxt->DalPlatformInfoHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalPlatformInfoHandle);
   }
   return nErr;

} /* END PlatformInfo_DeviceAttach */


/* =========================================================================
**  Function : PlatformInfo_GetPropertyValue
** =========================================================================*/
/**
  See PlatformInfo.h
*/

DALResult
PlatformInfo_GetPropertyValue
(
  const char   *szName,
  void        **pValue
)
{
  DALSYSPropertyVar Var;
  DALResult         eResult;

  eResult =
    DALSYS_GetPropertyValue(DALPlatformInfo_DriverCtxt.PlatformInfoDALDevCtxt[0].hProp,
      szName, 0, &Var);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  *pValue = (void *)Var.Val.pStruct;
  return DAL_SUCCESS;

} /* END PlatformInfo_GetPropertyValue */


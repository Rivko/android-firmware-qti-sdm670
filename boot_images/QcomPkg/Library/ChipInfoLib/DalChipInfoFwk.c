/*==============================================================================

FILE:      DalChipInfoFwk.c

DESCRIPTION: This file implements the ChipInfo DAL DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

    Copyright (c) 2008-2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

====================================================================

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ChipInfoLib/DalChipInfoFwk.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ChipInfo.h"
#include "DDIChipInfo.h"
#include "ChipInfoLocal.h"
#include "ChipInfoDefsLegacy.h"
#include "DALSys.h"
#include "DALReg.h"
#include "DALDeviceId.h"
#include "DALFramework.h"
/*
 * Basic stuff to set up ChipInfo DAL
 */
static DALResult
ChipInfo_DalChipInfo_Attach(const char *, DALDEVICEID,DalDeviceHandle **);

static DALDEVICEID DalChipInfo_DeviceId[1] = {DALDEVICEID_CHIPINFO};

const DALREG_DriverInfo
DALChipInfo_DriverInfo =
{
  ChipInfo_DalChipInfo_Attach,
  1,
	DalChipInfo_DeviceId
};

/*------------------------------------------------------------------------------
Declare a "ChipInfo" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct ChipInfoDrvCtxt ChipInfoDrvCtxt;
typedef struct ChipInfoDevCtxt ChipInfoDevCtxt;
typedef struct ChipInfoClientCtxt ChipInfoClientCtxt;

/*------------------------------------------------------------------------------
Declare a private "ChipInfo" Vtable
------------------------------------------------------------------------------*/
typedef struct ChipInfoDALVtbl ChipInfoDALVtbl;
struct ChipInfoDALVtbl
{
  int (*ChipInfo_DriverInit)(ChipInfoDrvCtxt *);
  int (*ChipInfo_DriverDeInit)(ChipInfoDrvCtxt *);
};

struct ChipInfoDevCtxt
{
  /*
   * Base Members
   */
  uint32            dwRefs;
  DALDEVICEID       DevId;
  uint32            dwDevCtxtRefIdx;
  ChipInfoDrvCtxt  *pChipInfoDrvCtxt;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32 Reserved[16];
};

struct ChipInfoDrvCtxt
{
  /*
   * Base Members
   */
  ChipInfoDALVtbl ChipInfoDALVtbl;
  uint32          dwNumDev;
  uint32          dwSizeDevCtxt;
  uint32          bInit;
  uint32          dwRefs;
  ChipInfoDevCtxt ChipInfoDevCtxt[1];

  /*
   * Handle for communicating with the HWIO DAL.
   */
  DalDeviceHandle       *phDalHWIO;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProps);
};

/*------------------------------------------------------------------------------
Declaring a "ChipInfo" Client Context
------------------------------------------------------------------------------*/
struct ChipInfoClientCtxt
{
  /*
   * Base Members
   */
  uint32  dwRefs;
  uint32  dwAccessMode;
  void *pPortCtxt;
  ChipInfoDevCtxt *pChipInfoDevCtxt;
  DalChipInfoHandle DalChipInfoHandle;
};



static uint32
ChipInfo_DalChipInfo_AddRef(DalChipInfoHandle* h)
{
  return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
The following functions are defined in the DalDevice DAL Interface.
------------------------------------------------------------------------------*/

DALResult
ChipInfo_DriverInit(ChipInfoDrvCtxt *pCtxt)
{
  return (ChipInfo_Init() == TRUE ? DAL_SUCCESS : DAL_ERROR);
}

DALResult
ChipInfo_DriverDeInit(ChipInfoDrvCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

static uint32
ChipInfo_DalChipInfo_Detach(DalDeviceHandle* h)
{
  uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
  return dwref;
}

static DALResult
ChipInfo_DalChipInfo_Init(DalDeviceHandle *h)
{
  ChipInfoClientCtxt *pClientCtxt = (ChipInfoClientCtxt *)(h->pClientCtxt);
  DALSYS_GetDALPropertyHandle(pClientCtxt->pChipInfoDevCtxt->DevId,
                             pClientCtxt->pChipInfoDevCtxt->hProp);
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_DeInit(DalDeviceHandle *h)
{
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_Open(DalDeviceHandle* h, uint32 mode)
{
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_Close(DalDeviceHandle* h)
{
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
  info->Version = DALCHIPINFO_INTERFACE_VERSION;
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
  return DAL_ERROR;
}


/*------------------------------------------------------------------------------
  The following functions are extended in DalChipInfo Interface.

  To maintain backwards compatibility, they are now just wrappers around the
  non-DAL ChipInfo functions.
--------------------------------------------------------------------------------*/

static DALResult
ChipInfo_DalChipInfo_GetChipVersion(DalDeviceHandle * h, ChipInfoVersionType * pnVersion)
{
  *pnVersion = ChipInfo_GetChipVersion();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetRawChipVersion(DalDeviceHandle * h, uint32 * pnVersion)
{
  *pnVersion = ChipInfo_GetRawChipVersion();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetChipId(DalDeviceHandle * h, ChipInfoIdType * peId)
{
  *peId = ChipInfo_GetChipId();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetRawChipId(DalDeviceHandle * h, uint32 * pnId)
{
  *pnId = ChipInfo_GetRawChipId();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetChipIdString(DalDeviceHandle * h, char * szIdString, uint32  nMaxLength)
{
  ChipInfo_GetChipIdString(szIdString, nMaxLength);
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetChipFamily(DalDeviceHandle * h, ChipInfoFamilyType * peFamily)
{
  *peFamily = ChipInfo_GetChipFamily();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetModemSupport(DalDeviceHandle * h, ChipInfoModemType * pnModem)
{
  *pnModem = ChipInfo_GetModemSupport();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetMarketingNameString(DalDeviceHandle * h,  char * pszMarketingNameStr,  uint32  nMaxLength)
{
  ChipInfo_GetMarketingNameString(pszMarketingNameStr, nMaxLength);
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetSerialNumber(DalDeviceHandle * h, ChipInfoSerialNumType* pnSerialNum)
{
  *pnSerialNum = ChipInfo_GetSerialNumber();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetFoundryId(DalDeviceHandle * h, ChipInfoFoundryIdType * peId)
{
  *peId = ChipInfo_GetFoundryId();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetRawDeviceFamily(DalDeviceHandle * h, uint32 * pnId)
{
  *pnId = ChipInfo_GetRawDeviceFamily();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetRawDeviceNumber(DalDeviceHandle * h, uint32 * pnId)
{
  *pnId = ChipInfo_GetRawDeviceNumber();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetQFPROMChipId(DalDeviceHandle * h, ChipInfoQFPROMChipIdType* pnQFPROMChipId)
{
  *pnQFPROMChipId = ChipInfo_GetQFPROMChipId();
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetDefectivePart(DalDeviceHandle * h, ChipInfoPartType ePart, uint32 *pnMask)
{
  return ChipInfo_GetDefectivePart(ePart, pnMask);
}

static DALResult
ChipInfo_DalChipInfo_GetDefectiveCPUs(DalDeviceHandle *h, uint32 nCPUCluster, uint32 *pnMask)
{
  return ChipInfo_GetDefectiveCPUs(nCPUCluster, pnMask);
}

static void
ChipInfo_InitInterface(ChipInfoClientCtxt* pclientCtxt)
{
  static const DalChipInfo vtbl = {
    {
      ChipInfo_DalChipInfo_Attach,
      ChipInfo_DalChipInfo_Detach,
      ChipInfo_DalChipInfo_Init,
      ChipInfo_DalChipInfo_DeInit,
      ChipInfo_DalChipInfo_Open,
      ChipInfo_DalChipInfo_Close,
      ChipInfo_DalChipInfo_Info,
      ChipInfo_DalChipInfo_PowerEvent,
      ChipInfo_DalChipInfo_SysRequest
    } ,
    ChipInfo_DalChipInfo_GetChipVersion,
    ChipInfo_DalChipInfo_GetRawChipVersion,
    ChipInfo_DalChipInfo_GetChipId,
    ChipInfo_DalChipInfo_GetRawChipId,
    ChipInfo_DalChipInfo_GetChipIdString,
    ChipInfo_DalChipInfo_GetChipFamily,
    ChipInfo_DalChipInfo_GetModemSupport,
    ChipInfo_DalChipInfo_GetMarketingNameString,
    ChipInfo_DalChipInfo_GetSerialNumber,
    ChipInfo_DalChipInfo_GetFoundryId,
    ChipInfo_DalChipInfo_GetRawDeviceFamily,
    ChipInfo_DalChipInfo_GetRawDeviceNumber,
    ChipInfo_DalChipInfo_GetQFPROMChipId,
    ChipInfo_DalChipInfo_GetDefectivePart,
    ChipInfo_DalChipInfo_GetDefectiveCPUs,
  };

  /*--------------------------------------------------------------------------
  Depending upon client type setup the vtables (entry points)
  --------------------------------------------------------------------------*/
  pclientCtxt->DalChipInfoHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
  pclientCtxt->DalChipInfoHandle.pVtbl  = &vtbl;
  pclientCtxt->DalChipInfoHandle.pClientCtxt = pclientCtxt;

}

  static ChipInfoDrvCtxt ChipInfo_drvCtxt = {{ChipInfo_DriverInit,
                     ChipInfo_DriverDeInit
                    },1,
                    sizeof(ChipInfoDevCtxt)};

DALResult
ChipInfo_DalChipInfo_Attach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
  DALResult nErr;
  static ChipInfoClientCtxt clientCtxt;

  ChipInfoClientCtxt *pclientCtxt = &clientCtxt;

  *phDalDevice = NULL;

  nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&ChipInfo_drvCtxt,
                             (DALClientCtxt *)pclientCtxt);
  if (DAL_SUCCESS == nErr)
  {
    ChipInfo_InitInterface(pclientCtxt);
    ChipInfo_DalChipInfo_AddRef(&(pclientCtxt->DalChipInfoHandle));
    *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalChipInfoHandle);
  }
  return nErr;
}


/*==============================================================================

FILE:      DalChipInfoFwk.c

DESCRIPTION: This file implements the ChipInfo DAL DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

    Copyright (c) 2008-2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

====================================================================

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/chipinfo/src/DalChipInfoFwk.c#3 $
$DateTime: 2017/10/17 03:59:24 $
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
#include <stringl/stringl.h>

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
ChipInfo_DalChipInfo_Detach(uint32 ClientHandle, DalDeviceHandle *h)
{
  uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
  return dwref;
}

static DALResult
ChipInfo_DalChipInfo_Init(uint32 ClientHandle, DalDeviceHandle *h)
{
  ChipInfoClientCtxt *pClientCtxt = (ChipInfoClientCtxt *)(h->pClientCtxt);
  DALSYS_GetDALPropertyHandle(pClientCtxt->pChipInfoDevCtxt->DevId,
                             pClientCtxt->pChipInfoDevCtxt->hProp);
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_DeInit(uint32 ClientHandle, DalDeviceHandle *h)
{
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_PowerEvent(uint32 ClientHandle, DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_Open(uint32 ClientHandle, DalDeviceHandle *h, uint32 mode)
{
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_Close(uint32 ClientHandle, DalDeviceHandle *h)
{
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_Info(uint32 ClientHandle, DalDeviceHandle *h, DalDeviceInfo *info, uint32 infoSize)
{
  uint32 nVersion = DALCHIPINFO_INTERFACE_VERSION;
  DalDeviceInfo devInfo;

  if (DalSys_CopyFromUser(ClientHandle, &devInfo, info, sizeof(devInfo)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  devInfo.Version = nVersion;

  if (DalSys_CopyToUser(ClientHandle, info, &devInfo, sizeof(*info)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_SysRequest(uint32 ClientHandle, DalDeviceHandle *h,DalSysReq ReqIdx,
                                const void *SrcBuf, uint32 SrcBufLen,
                                void *DestBuf, uint32 DestBufLen, uint32 *DestBufLenReq)
{
  return DAL_ERROR;
}


/*------------------------------------------------------------------------------
  The following functions are extended in DalChipInfo Interface.

  To maintain backwards compatibility, they are now just wrappers around the
  non-DAL ChipInfo functions.
--------------------------------------------------------------------------------*/

static DALResult
ChipInfo_DalChipInfo_GetChipVersion(uint32 ClientHandle,  DalDeviceHandle *h, ChipInfoVersionType *pnVersion)
{
  ChipInfoVersionType nVersion;

  nVersion = ChipInfo_GetChipVersion();
  if (DalSys_CopyToUser(ClientHandle, pnVersion, &nVersion, sizeof(nVersion)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetRawChipVersion(uint32 ClientHandle,  DalDeviceHandle *h, uint32 *pnVersion)
{
  uint32 nRawVersion;

  nRawVersion = ChipInfo_GetRawChipVersion();
  if (DalSys_CopyToUser(ClientHandle, pnVersion, &nRawVersion, sizeof(nRawVersion)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetChipId(uint32 ClientHandle,  DalDeviceHandle *h, ChipInfoIdType *peId)
{
  ChipInfoIdType eId;

  eId = ChipInfo_GetChipId();
  if (DalSys_CopyToUser(ClientHandle, peId, &eId, sizeof(eId)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetRawChipId(uint32 ClientHandle,  DalDeviceHandle *h, uint32 *pnId)
{
  uint32 nId;

  nId = ChipInfo_GetRawChipId();
  if (DalSys_CopyToUser(ClientHandle, pnId, &nId, sizeof(nId)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetChipIdString(uint32 ClientHandle,  DalDeviceHandle *h, char *szIdString, uint32 nMaxLength)
{
  /*
   * NOTE: nMaxLength includes the terminating \0, as does CHIPINFO_MAX_ID_LENGTH
   */
  char szIdStringCopy[CHIPINFO_MAX_ID_LENGTH] = { 0 };
  DALResult result;
  size_t nIdLength;

  result = ChipInfo_GetChipIdString(szIdStringCopy, CHIPINFO_MAX_ID_LENGTH);
  if (result != DAL_SUCCESS)
  {
    return result;
  }

  /* Find the length of the string to be output, truncating if necessary */
  nIdLength = strnlen(szIdStringCopy, CHIPINFO_MAX_ID_LENGTH);

  /* Avoid buffer overflows */
  if (nIdLength >= nMaxLength || nIdLength == CHIPINFO_MAX_ID_LENGTH)
  {
    return DAL_ERROR;
  }

  /* Copy the null-terminated string into the user buffer */
  if (DalSys_CopyToUser(ClientHandle, szIdString, szIdStringCopy, nIdLength + 1))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetChipFamily(uint32 ClientHandle, DalDeviceHandle *h, ChipInfoFamilyType *peFamily)
{
  ChipInfoFamilyType eFamily;

  eFamily = ChipInfo_GetChipFamily();
  if (DalSys_CopyToUser(ClientHandle, peFamily, &eFamily, sizeof(eFamily)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetModemSupport(uint32 ClientHandle, DalDeviceHandle *h, ChipInfoModemType *pnModem)
{
  ChipInfoModemType nModem;

  nModem = ChipInfo_GetModemSupport();
  if (DalSys_CopyToUser(ClientHandle, pnModem, &nModem, sizeof(nModem)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }
  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetMarketingNameString(uint32 ClientHandle, DalDeviceHandle *h, char *szMarketingNameStr, uint32 nMaxLength)
{
  char szMarketingNameStrCopy[CHIPINFO_MAX_ID_LENGTH] = { 0 };
  size_t nIdLength;
  DALResult result;

  result = ChipInfo_GetMarketingNameString(szMarketingNameStrCopy, CHIPINFO_MAX_ID_LENGTH);
  if (result != DAL_SUCCESS)
  {
    return result;
  }

  nIdLength = strnlen(szMarketingNameStrCopy, CHIPINFO_MAX_ID_LENGTH);
  if (nIdLength >= nMaxLength || nIdLength == CHIPINFO_MAX_ID_LENGTH)
  {
    return DAL_ERROR;
  }

  if (DalSys_CopyToUser(ClientHandle, szMarketingNameStr, szMarketingNameStrCopy, nIdLength + 1))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetSerialNumber(uint32 ClientHandle, DalDeviceHandle *h, ChipInfoSerialNumType *pnSerialNum)
{
  ChipInfoSerialNumType nSerialNum;

  nSerialNum = ChipInfo_GetSerialNumber();
  if (DalSys_CopyToUser(ClientHandle, pnSerialNum, &nSerialNum, sizeof(nSerialNum)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetFoundryId(uint32 ClientHandle, DalDeviceHandle *h, ChipInfoFoundryIdType *peId)
{
  ChipInfoFoundryIdType eId;

  eId = ChipInfo_GetFoundryId();
  if (DalSys_CopyToUser(ClientHandle, peId, &eId, sizeof(eId)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetRawDeviceFamily(uint32 ClientHandle, DalDeviceHandle *h, uint32 *pnId)
{
  uint32 nId;

  nId = ChipInfo_GetRawDeviceFamily();
  if (DalSys_CopyToUser(ClientHandle, pnId, &nId, sizeof(nId)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetRawDeviceNumber(uint32 ClientHandle, DalDeviceHandle *h, uint32 *pnId)
{
  uint32 nId;

  nId = ChipInfo_GetRawDeviceNumber();
  if (DalSys_CopyToUser(ClientHandle, pnId, &nId, sizeof(nId)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetQFPROMChipId(uint32 ClientHandle, DalDeviceHandle *h, ChipInfoQFPROMChipIdType *pnQFPROMChipId)
{
  ChipInfoQFPROMChipIdType nQFPROMChipId;

  nQFPROMChipId = ChipInfo_GetQFPROMChipId();
  if (DalSys_CopyToUser(ClientHandle, pnQFPROMChipId, &nQFPROMChipId, sizeof(nQFPROMChipId)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetDefectivePart(uint32 ClientHandle, DalDeviceHandle *h, ChipInfoPartType ePart, uint32 *pnMask)
{
  uint32 nMask;
  DALResult result;

  result = ChipInfo_GetDefectivePart(ePart, &nMask);
  if (result != DAL_SUCCESS)
  {
    return result;
  }

  if (DalSys_CopyToUser(ClientHandle, pnMask, &nMask, sizeof(nMask)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult
ChipInfo_DalChipInfo_GetDefectiveCPUs(uint32 ClientHandle, DalDeviceHandle *h, uint32 nCPUCluster, uint32 *pnMask)
{
  uint32 nMask;
  DALResult result;

  result = ChipInfo_GetDefectiveCPUs(nCPUCluster, &nMask);
  if (result != DAL_SUCCESS)
  {
    return result;
  }

  if (DalSys_CopyToUser(ClientHandle, pnMask, &nMask, sizeof(nMask)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static void
ChipInfo_InitInterface(ChipInfoClientCtxt *pclientCtxt)
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
  pclientCtxt->DalChipInfoHandle.dwVtblLen = sizeof(vtbl)/sizeof(void (*)(void));

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


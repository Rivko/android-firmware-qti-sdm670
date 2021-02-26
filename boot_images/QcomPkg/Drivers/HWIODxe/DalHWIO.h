#ifndef DALHWIO_H
#define DALHWIO_H
/*==============================================================================

FILE:      DalHWIO.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2008-2015 QUALCOMM Technologies, Inc.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALFramework.h"
#include "DDIHWIO.h"


#define DALHWIO_REFCOUNT_STATIC 0xFFFF

/*------------------------------------------------------------------------------
Declaring a "HWIO" Driver,Device and Client Context
------------------------------------------------------------------------------*/

typedef struct HWIODrvCtxt HWIODrvCtxt;
typedef struct HWIODevCtxt HWIODevCtxt;
typedef struct HWIOClientCtxt HWIOClientCtxt;

/*
 * HWIOModuleType
 *
 * Type definition for the module elements in the HWIO memory map structure.
 * Each entry defines a 'MODULE' as taken from the ARM_ADDRESS_FILE.  Each
 * physical entry contains an array of such modules.
 */
typedef struct
{
  char           *szName;
  UINTN           nOffset;
  uint32          dwSizeInBytes;
} HWIOModuleType;

/*
 * HWIOPhysRegionType
 *
 * Type definition for the elements in the HWIO memory map structure
 * declared in msmhwiomap.h.
 */
typedef struct
{
  char           *szName;
  DALSYSMemAddr   pPhysAddr;
  uint64          dwSizeInBytes;
  DALSYSMemAddr   pVirtAddr;
  HWIOModuleType *pModules;
} HWIOPhysRegionType;

/*
 * HWIOVirtRegionType
 *
 * Type definition for the elements in the HWIO memory map structure
 * dynamic information.  This data is allocated as an array that is
 * indexed the same as the HWIOPhysRegionType array.
 */
typedef struct
{
  DALSYSMemAddr   pVirtAddr;
  uint16          dwRefCount;
  DALSYSMemHandle hMem;
} HWIOVirtRegionType;


/* ---------------------------------------------------------------------------
  Private "HWIO" VTable
 -------------------------------------------------------------------------- */

typedef struct HWIODALVtbl HWIODALVtbl;
struct HWIODALVtbl
{
  int (*HWIO_DriverInit)(HWIODrvCtxt *);
  int (*HWIO_DriverDeInit)(HWIODrvCtxt *);
};

struct HWIODevCtxt
{
  /*
   * Base Members
   */
  uint32   dwRefs;
  DALDEVICEID DevId;
  uint32   dwDevCtxtRefIdx;
  HWIODrvCtxt  *pHWIODrvCtxt;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32 Reserved[16];

  /*
   * HWIO Device state can be added by developers here
   */
};

struct HWIODrvCtxt
{
  /*
   * Base Members
   */
  HWIODALVtbl HWIODALVtbl;
  uint32  dwNumDev;
  uint32  dwSizeDevCtxt;
  uint32  bInit;
  uint32  dwRefs;
  HWIODevCtxt HWIODevCtxt[1];

  /*
   * HWIO base map table.
   */
  HWIOPhysRegionType *PhysMap;
  HWIOVirtRegionType *VirtMap;

  /*
   * HWIO sync object.
   */
  DALSYSSyncHandle hMapSync;
  DALSYSSyncObj    mMapSyncObj;
};

/*------------------------------------------------------------------------------
Declaring a "HWIO" Client Context
------------------------------------------------------------------------------*/

struct HWIOClientCtxt
{
  /*
   * Base Members
   */
  uint32        dwRefs;
  uint32        dwAccessMode;
  void         *pPortCtxt;
  HWIODevCtxt  *pHWIODevCtxt;
  DalHWIOHandle DalHWIOHandle;

  /*
   * HWIO Client state can be added by developers here
   */
};


/* ---------------------------------------------------------------------------
  DAL framwork public API Functions
 -------------------------------------------------------------------------- */

DALResult HWIO_DriverInit(HWIODrvCtxt *);
DALResult HWIO_DriverDeInit(HWIODrvCtxt *);
DALResult HWIO_DeviceInit(HWIOClientCtxt *);
DALResult HWIO_DeviceDeInit(HWIOClientCtxt *);
DALResult HWIO_Reset(HWIOClientCtxt *);
DALResult HWIO_PowerEvent(HWIOClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult HWIO_Open(HWIOClientCtxt *, uint32);
DALResult HWIO_Close(HWIOClientCtxt *);
DALResult HWIO_Info(HWIOClientCtxt *,DalDeviceInfo *, uint32);
DALResult HWIO_InheritObjects(HWIOClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);

/* ---------------------------------------------------------------------------
  Public API Functions
 -------------------------------------------------------------------------- */

DALResult HWIO_MapRegion( HWIODrvCtxt *, const char *, uint8 **);
DALResult HWIO_MapRegionByAddress( HWIODrvCtxt *, uint8 *, uint8 **);
DALResult HWIO_UnMapRegion( HWIODrvCtxt *, uint8 *);
DALResult HWIO_MapAllRegions( HWIODrvCtxt *);

/* ---------------------------------------------------------------------------
  Functions privately declared
 -------------------------------------------------------------------------- */

void HWIO_InitBasePointers (HWIODrvCtxt *);

#endif /* DALHWIO_H */



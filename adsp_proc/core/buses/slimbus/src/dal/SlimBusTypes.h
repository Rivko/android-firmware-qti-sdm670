#ifndef SLIMBUSTYPES_H
#define SLIMBUSTYPES_H
/*
===========================================================================

FILE:         SlimBusTypes.h

DESCRIPTION:  This file defines basic typedefs used by the SlimBus driver
              in the WDF environment.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/slimbus/src/dal/SlimBusTypes.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/06/17   MJS     Use DALSYS timer APIs rather than DAL Timer driver.
10/14/15   MJS     Code portability changes for message FIFO index type.
03/18/15   MJS     Support for second Slimbus instance.
02/28/14   MJS     Streamline ISR and don't lock device mutex in ISR.
07/25/13   MJS     GPIO interrupt suport.
02/14/13   MJS     Add macro for IRQL check.
11/29/12   MJS     Support BAM deferred processing within client context.
05/16/12   MJS     Stub out WOA specific macros.
12/11/11   MJS     Initial revision.

===========================================================================
             Copyright (c) 2011-2017 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include <HALcomdef.h>
#include <DALSys.h>
#include <DALFramework.h>
#include <bam.h>
#include <npa.h>
#include <mmpm.h>

#define SB_SUCCESS DAL_SUCCESS
#define SB_ERROR DAL_ERROR

#define _IRQL_requires_(x)
#define UNREFERENCED_PARAMETER(x)
#define SB_CHECK_IRQL()
#define SB_TRACE_HEADER(x) "SlimBus.h"

typedef DALResult SBResult;

typedef DalDeviceHandle *SlimBusHandle;
typedef DALSYSMemHandle SlimBusMemHandle;
typedef DALSYSSyncHandle SlimBusSyncHandle;
typedef DALSYSEventHandle SlimBusEventHandle;
typedef void *SlimBusTimerHandle;
typedef DalDeviceHandle *SlimBusTlmmHandle;
typedef DalDeviceHandle *SlimBusGpioIntHandle;
typedef DALSYSWorkLoopHandle SlimBusWorkLoopHandle;
typedef size_t SlimBusMsgIdxType;

typedef DALBOOL SBBOOL;
typedef DALSYSEventObj SlimBusEventObj;

/*------------------------------------------------------------------------------
Declaring a private "SlimBus" Vtable
------------------------------------------------------------------------------*/
struct SlimBusDrvCtxt;
struct SlimBusDevCtxt;
struct SlimBusClientCtxt;
struct DalSlimBusHandle;
typedef void * (*SlimBusCallbackFunc)(void*,uint32,void*,uint32);

typedef struct DalSlimBusHandle SlimBusRefHandle;

typedef struct SlimBusDrvVtbl SlimBusDrvVtbl;
struct SlimBusDrvVtbl
{
  int (*SlimBus_DriverInit)(struct SlimBusDrvCtxt *);
  int (*SlimBus_DriverDeInit)(struct SlimBusDrvCtxt *);
};

typedef struct
{
  uint32   dwRefs;
  DALDEVICEID DevId;
  uint32   dwDevCtxtRefIdx;
  struct SlimBusDrvCtxt *pSlimBusDrvCtxt;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32 Reserved[16];
} SlimBusBaseDevType;

typedef struct
{
  SlimBusDrvVtbl SlimBusDrvVtbl;
  uint32  dwNumDev;
  uint32  dwSizeDevCtxt;
  uint32  bInit;
  uint32  dwRefs;
} SlimBusBaseDrvType;

typedef struct SlimBusIsrCtxt
{
  uint32 uMsgIntStatus;
  uint32 uFrmIntStatus;
  uint32 uIntfIntStatus;
  uint32 uPgdIntStatus;
  uint32 uPgdPortIntRawStatus;
  uint32 uPgdPortIntMaskedStatus;
  uint32 uPortDisMask;
  uint32 uPortUnderFlowMask;
  uint32 uPortOverFlowMask;
  uint32 uPortDmaMask;
} SlimBusIsrCtxt;

typedef struct
{
  bam_handle hBAM;
  /* Handle to Tlmm driver */
  SlimBusTlmmHandle hTlmm;
  SlimBusGpioIntHandle hGpioInt;
  SlimBusSyncHandle hBamSync;
  uint32 hMMPM;
  npa_client_handle hSVS;
  MmpmRegParamType *pMmpmRegParam;
} SlimBusDevPlatType;

#endif /* SLIMBUSTYPES_H */


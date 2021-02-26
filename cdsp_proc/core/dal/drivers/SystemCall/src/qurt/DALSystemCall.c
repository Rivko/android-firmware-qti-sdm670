/*==============================================================================

FILE:      DALSystemCall.c

DESCRIPTION: This file implements the System Call DAL

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DDISystemCall.h"
#include "DALReg.h"
#include "DALFramework.h"
#include "DALDeviceId.h"
#include "DalDevice.h"

// AVSCR config constants
#define AVSCR_ENABLE_VAL   0x21
#define AVSCR_DISABLE_VAL  0

// Function Pointer Types
typedef uint32 (*DalFunc_uint32)(uint32, DalDeviceHandle *);
typedef DALResult (*DALFunc_DalResult)(uint32, DalDeviceHandle *);
typedef DALResult (*DALFunc_DalResult_uint32)(uint32, DalDeviceHandle *, uint32);
typedef DALResult (*DALPowerEventFunc)(uint32, DalDeviceHandle *, DalPowerCmd,
      DalPowerDomain);
typedef DALResult (*DALSysRequestFunc)(uint32, DalDeviceHandle *, DalSysReq,
      const void *, uint32, void *, uint32, uint32 *);


/*------------------------------------------------------------------------------
Definitions of DAL System Call API
------------------------------------------------------------------------------*/
static DALResult 
SoftwareWaitForInterrupt(uint32 ClientHandle, DalDeviceHandle * _h)
{
   /*TODO*/
   return DAL_SUCCESS;
}

static DALResult 
PowerCollapse(uint32 ClientHandle, DalDeviceHandle * _h)
{
   /*TODO*/
   return DAL_SUCCESS;
}

static DALResult 
SetAVSEnabled(uint32 ClientHandle, DalDeviceHandle * _h, int AVSIsEnabled)
{
   /*TODO*/
   return DAL_SUCCESS;
}

static DALResult 
SetAVSDelaySynthesizerRAW(uint32 ClientHandle, DalDeviceHandle * _h, 
      uint32 AVSDelaySynthesizerRAW)
{
   /*TODO*/
   return DAL_SUCCESS;
}

static DALResult
SyncSysTimeTick(uint32 ClientHandle, DalDeviceHandle * _h, uint32 timeDelta)
{
   /*TODO*/
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Definition of DAL Device Interface
------------------------------------------------------------------------------*/
static DALResult
RetSuccessStub(void)
{
   return DAL_SUCCESS;
}

static DALResult 
GetInfo(uint32 ClientHandle, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALSYSTEMCALL_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
DalSystemCall_Attach(const char *pszArg, DALDEVICEID DeviceId, 
      DalDeviceHandle **phDalDevice)
{
   static const DalSystemCall vtbl = {
      {
         DalSystemCall_Attach,
         (DalFunc_uint32)RetSuccessStub,
         (DALFunc_DalResult)RetSuccessStub,
         (DALFunc_DalResult)RetSuccessStub,
         (DALFunc_DalResult_uint32)RetSuccessStub,
         (DALFunc_DalResult)RetSuccessStub,
         GetInfo,
         (DALPowerEventFunc)RetSuccessStub,
         (DALSysRequestFunc)RetSuccessStub
       } ,
      SoftwareWaitForInterrupt,
      PowerCollapse,
      SetAVSEnabled,
      SetAVSDelaySynthesizerRAW,
      SyncSysTimeTick,
   };  

   static DALDevCtxt systemCallDevCtxt = {
      0x2,  // dwRefs - need to be at least 2 so that DALSYS would not call init
      // don't care about the rest
   };

   static DALClientCtxt systemCallClientCtxt = {
      0x2,  // dwRefs - same as above
      0,    // dwAccessMode
      NULL, // pPortCtxt
      &systemCallDevCtxt   // pDALDevCtxt
   };

   static DalSystemCallHandle hSystemCall = {
      DALDEVICE_INTERFACE_HANDLE_ID,
      &vtbl,
      &systemCallClientCtxt
   };

   *phDalDevice = (DalDeviceHandle *)&hSystemCall;
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
System Call DAL Device Info
------------------------------------------------------------------------------*/
static DALDEVICEID DalSystemCall_DeviceId[1] = {DALDEVICEID_SYSTEMCALL};

const DALREG_DriverInfo 
DALSystemCall_DriverInfo = 
	{ DalSystemCall_Attach, 1, DalSystemCall_DeviceId };

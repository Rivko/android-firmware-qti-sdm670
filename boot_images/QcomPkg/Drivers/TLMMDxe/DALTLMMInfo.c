/*===========================================================================

             M A I N   D A L   T L M M   D R I V E R   F I L E

DESCRIPTION

  Contains the majority of functionality for the DAL TLMM driver.  The
  API in this module MUST always remain target independent.


===========================================================================
             Copyright (c) 2009-2015 Qualcomm Technologies, Inc. 
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
TLMM_DalTlmm_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalTLMM_DeviceId[2] = {DALDEVICEID_TLMM,
                                          DALDEVICEID_TLMM_MASTER};

const DALREG_DriverInfo 
DALTLMM_DriverInfo = { TLMM_DalTlmm_Attach,
     					   2,
						   DalTLMM_DeviceId
                         };



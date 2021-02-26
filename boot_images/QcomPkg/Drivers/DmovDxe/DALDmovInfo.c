/*
===============================================================================

FILE:         DALDmovInfo.c

DESCRIPTION:  
  This file implements the data mover device driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

===============================================================================

                                Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
03/05/10   mk      added DALDEVICEID_DMOV_DEVICE_2 
08/20/09   an      added virtual device DmovConfig
01/18/08   ah      Created

===============================================================================
                   Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===============================================================================
*/

#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
Dmov_DalDmov_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalDmov_DeviceId[ ] = {DALDEVICEID_DMOV_DEVICE_0, DALDEVICEID_DMOV_DEVICE_1,DALDEVICEID_DMOV_DEVICE_2};

const DALREG_DriverInfo 
DALDmov_DriverInfo = { Dmov_DalDmov_Attach,
     				   sizeof( DalDmov_DeviceId)/ sizeof( DalDmov_DeviceId[0]),
					   DalDmov_DeviceId
                     };

const DALREG_DriverInfo 
DALDmovConfig_DriverInfo = { NULL,
     				         0,
					         NULL
                           };


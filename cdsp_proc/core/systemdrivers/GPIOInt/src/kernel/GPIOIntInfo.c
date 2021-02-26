/*==============================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R    

DESCRIPTION
  This file implements the attach mechanism of GPIOIntController 
  DeviceDriver to the dal main framework.The definitions and api in this module 
  are used internally in the GPIO interrupt controller and are not to be exposed 
  to the clients.

REFERENCES

        Copyright © 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/src/kernel/GPIOIntInfo.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
GPIOInt_GPIOInt_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID GPIOInt_DeviceId[2] = { DALDEVICEID_GPIOINT, DALDEVICEID_GPIOINTERRUPTCONTROLLER};

const DALREG_DriverInfo 
DALGPIOInt_DriverInfo = { 
                          GPIOInt_GPIOInt_Attach,
                          2,
                          GPIOInt_DeviceId
                        };



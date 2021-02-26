#ifndef __UGPIOIntKernel_H
#define __UGPIOIntKernel_H
/*===========================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R    

DESCRIPTION
  This modules contains enumeration definitions to support the Kernel uGPIO 
  interrupt controller. The definitions in this module are used internally 
  in the dal GPIO interrupt controller or the QDI interface and are not to be
  exposed to the clients or the user PD uGPIO driver directly . These Apis
  support the maintainance of GPIOInt state across all protection domains.

REFERENCES

       Copyright (c) 2017 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/src/kernel/uGPIOIntInternal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/
#include "com_dtypes.h"
#include "uGPIOInt.h"

/*============================================================================= 
 
  FUNCTION      uGPIOInt_Init

  This is the main driver initialization function that is called in the
  GPIOInt DAL. \n

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the uGPIOInt driver was correctly initialized. \n
  UGPIOINT_ERROR --  Is returned if the uGPIOInt initialization failed. \n
  @dependencies
  None.
 
=============================================================================*/
int32 uGPIOInt_Init
(
  void
);

#endif /* __UGPIOIntKernel_H */

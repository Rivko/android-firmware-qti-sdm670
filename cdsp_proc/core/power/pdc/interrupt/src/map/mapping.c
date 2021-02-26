/*==========================================================================
 
  FILE:       mapping.c

  OVERVIEW:   This file implements default OS specific functions of the PDC 
              mapping API

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/src/map/mapping.c#1 $
$DateTime: 2018/07/30 01:21:36 $
=============================================================================*/ 
#include "comdef.h"
#include "CoreVerify.h"
#include "pdc_types.h"
#include "pdcMap.h"

/*=========================================================================
 *                      EXTERNAL FUNCTION PROTOTYPES
 *========================================================================*/
pdc_status_type mapOS_getInterruptTable(pdc_interrupts *intMap);
pdc_status_type mapOS_getGpioTable(pdc_gpios *gpioMap);

/*=========================================================================
 *                   GLOBAL DEFINITIONS & DECLARATIONS
 *========================================================================*/
/*
 * pdcMap_getInterruptTable
 */
pdc_status_type pdcMap_getInterruptTable(pdc_interrupts *intMap)
{
  CORE_VERIFY_PTR(intMap);
  return(mapOS_getInterruptTable(intMap));
}

/*
 * pdcMap_getGpioTable
 */
pdc_status_type pdcMap_getGpioTable(pdc_gpios *gpioMap)
{
  CORE_VERIFY_PTR(gpioMap);
  return(mapOS_getGpioTable(gpioMap));
}


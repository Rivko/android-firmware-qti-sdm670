/*========================================================================
   This file contains functions to set the parameters for different modules.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dsat/inc/AFEDsatDriver.h#1 $
 ====================================================================== */

#ifndef _DSAT_DRIVER_H_
#define _DSAT_DRIVER_H_
/*==========================================================================
  Include Files
========================================================================== */
#include "AFEInternal.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


  /*==========================================================================
    Function declarations
  ========================================================================== */

  /*=============================================================================
  FUNCTION     ADSPResult  afe_port_dangergen_open

  DESCRIPTION  Attempts to open danger gen functionality for the current port.
  RETURN
            ADSP_EOK if successful
            ADSP_EFAILED if any failure occured
            Note that failure to open dangergen is not fatal, we should proceed
            processing even if it failed.
            Reserving the error code in case in the future we need to make use of
            it

  DEPENDENCIES  None


  SIDE EFFECTS
  ===============================================================================*/

  ADSPResult afe_port_dangergen_open(afe_dev_port_t *pDevPort);

  /*=============================================================================
  FUNCTION     ADSPResult  afe_port_dangergen_close

  DESCRIPTION  Close Dangergen functionality.
  RETURN
            ADSP_EOK if successful
            ADSP_EFAILED if any failure occured
            Note that failure to close dangergen is not fatal, we should proceed
            processing even if it failed.
            Reserving the error code in case in the future we need to make use of
            it

  DEPENDENCIES  None


  SIDE EFFECTS
  ===============================================================================*/

  ADSPResult afe_port_dangergen_close(afe_dev_port_t *pDevPort);

  /*=============================================================================
  FUNCTION     ADSPResult  afe_port_dangergen_enable

  DESCRIPTION  Attempt to enable dangergen watchdog.
  RETURN
            ADSP_EOK if successful
            ADSP_EFAILED if any failure occured
            Note that failure to enable watchdog is not fatal, we should proceed
            processing even if it failed.
            Reserving the error code in case in the future we need to make use of
            it

  DEPENDENCIES  None


  SIDE EFFECTS
  ===============================================================================*/

  ADSPResult afe_port_dangergen_enable(afe_dev_port_t *pDevPort);

  /*=============================================================================
  FUNCTION     ADSPResult  afe_port_dangergen_disable

  DESCRIPTION  Attempt to disable dangergen watchdog.
  RETURN
            ADSP_EOK if successful
            ADSP_EFAILED if any failure occured
            Note that failure to disable watchdog is not fatal, we should proceed
            processing even if it failed.
            Reserving the error code in case in the future we need to make use of
            it

  DEPENDENCIES  None


  SIDE EFFECTS
  ===============================================================================*/

  ADSPResult afe_port_dangergen_disable(afe_dev_port_t *pDevPort);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif

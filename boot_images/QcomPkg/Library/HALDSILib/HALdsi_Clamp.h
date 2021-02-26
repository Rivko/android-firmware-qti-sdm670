#ifndef HALDSI_CLAMP_H
#define HALDSI_CLAMP_H
/*=============================================================================

  File: HALdsi_Clamp.h
  

     Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "HALdsi.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */


/****************************************************************************
*
** FUNCTION: HAL_DSI_ULPSClampConfig()
*/
/*!
* \brief
*   This API configures the DSI ULPS clamp control so that DSI lanes can be driven using different source
*
* \param [in] eDeviceID        - DSI Core Device ID 
* \param [in] bEnable          - DSI ULPS clamp enable/disable
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_ULPSClampConfig(DSI_Device_IDType  eDeviceID,
                             bool32             bEnable);

#ifdef __cplusplus
}
#endif

#endif  /* #define HALDSI_CLAMP_H */

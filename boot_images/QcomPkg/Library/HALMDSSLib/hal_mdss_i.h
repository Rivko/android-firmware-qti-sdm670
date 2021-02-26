/*! \file */

/*
===========================================================================

FILE:         hal_mdss_i.h

Copyright (c) 2014 QUALCOMM Technologies Incorporated. 
All rights reserved. Licensed Material - Restricted Use.
Qualcomm Confidential and Proprietary.
===========================================================================
*/

#ifndef _HAL_MDSS_I_H
#define _HAL_MDSS_I_H

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdss.h"
#include "mdsshwio.h"

/****************************************************************************
*
** FUNCTION: HAL_MDSS_ReadHwInfo_3_0_0()
*/
/*!
* \brief
*     Reads the hardware capabilities for the given MDSS Version(3.0.0)
*
* \param [out]  psMdssHwInfo            - Hardware information
*
* \retval NONE
*
****************************************************************************/
void HAL_MDSS_ReadHwInfo_3_0_0(HAL_MDSS_Hw_Private_Info *psMdssHwInfo);

#endif // _HAL_MDSS_I_H


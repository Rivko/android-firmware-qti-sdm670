/*! \file
*  \n
*  \brief  pm_tz_config.c
*  \n
*  \n This file contains pmic configuration data specific for trust zone image.
*  \n
*  \n &copy; Copyright 2015-2017 QUALCOMM Technologies, Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tz/2.0.2/pmic/pm/config/sdm670_pm670/src/pm_config.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/17   akm     Updated to rpmh voting
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
07/14/14   vtw     Created.
========================================================================== */

/*=========================================================================
      Include Files
==========================================================================*/

#include "comdef.h"
#include "pm_prm_device.h"
#include "pm_rpmh.h"

/*===========================================================================
Data Declarations
===========================================================================*/

/*
 * Configuration data to disable/enable qfprom rail.
 */
pm_prm_vreg_data_type
pm_qfprom_ldo_data[2] =
{
    /*mode                    voltage(mV)  hr(mV)   en*/
    {PM_PRM_VREG_MODE_LPM,       0,          0,     PM_PRM_DEV_DISABLE}, // OFF
    {PM_PRM_VREG_MODE_NPM,    1800,          0,     PM_PRM_DEV_ENABLE},  // ON
};


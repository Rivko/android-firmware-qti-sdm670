#ifndef __PM_CONFIG_H__
#define __PM_CONFIG_H__

/*! \file pm_config.h
*  \n
*  \brief Contains enums and macro definitions used in config data.
*  \n
*  \n &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/inc/pm_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/14   kt      Created
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "vrm.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
// Max voltage supported by VRM for regulators
// can be updated to 0x1FFF in future
#define PM_VRM_MAX_VOLTAGE    0x1FFA

/* Flags used for readibility in config data */
enum
{
    PM_ACCESS_DISALLOWED = 0,
    PM_ACCESS_ALLOWED = 1,
};

enum
{
   PM_SETTLING_DIS = 0,
   PM_SETTLING_EN = 1,
};

enum
{
   PM_SETTLING_ERR_DIS = 0,
   PM_SETTLING_ERR_EN = 1,
};

#endif // __PM_CONFIG_H__



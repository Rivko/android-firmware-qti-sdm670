#ifndef __PMAPP_NPA_H__
#define __PMAPP_NPA_H__
/*===========================================================================


                  P M I C    N P A    H E A D E R    F I L E

DESCRIPTION
  This file contains prototype definitions to support interaction
  with the QUALCOMM Power Management ICs using NPA framework.

Copyright (c) 2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/pmic/pm/pmapp_npa.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/17   kt      New File
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                        DEFINITIONS

===========================================================================*/

/*
 * Generic mode IDs that can be used for clients that only use ON / OFF or
 * ACTIVE / STANDBY states.
 */
#define PMIC_NPA_GROUP_ID_DDR "/pm/ddr"

/*===========================================================================

                        ENUMERATION

===========================================================================*/

enum
{
   PMIC_NPA_MODE_ID_DDR_CFG_0,
   PMIC_NPA_MODE_ID_DDR_CFG_1,
   PMIC_NPA_MODE_ID_DDR_CFG_2,
   PMIC_NPA_MODE_ID_DDR_CFG_3,
   PMIC_NPA_MODE_ID_DDR_CFG_4,
   PMIC_NPA_MODE_ID_DDR_CFG_5,
   PMIC_NPA_MODE_ID_DDR_CFG_6,
   PMIC_NPA_MODE_ID_DDR_CFG_7,
   PMIC_NPA_MODE_ID_DDR_CFG_8,
   PMIC_NPA_MODE_ID_DDR_CFG_9,
   PMIC_NPA_MODE_ID_DDR_CFG_10,
   PMIC_NPA_MODE_ID_DDR_CFG_MAX,
};

#endif // PMAPP_NPA_H


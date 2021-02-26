#ifndef __PM_INTERNAL_H__
#define __PM_INTERNAL_H__

/*! \file
 *  \n
 *  \brief  pm_internal.h
 *  \n
 *  \n This header file contains local definitions for PMIC SW.
 *  \n
 *  \n &copy; Copyright 2014-2017 Qualcomm Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tz/2.0.2/pmic/pm/framework/inc/pm_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
06/16/16   aab     Added support for desired PMIC Register dump
06/25/14   vtw     Created.
=============================================================================*/

#include "com_dtypes.h"

typedef enum
{
  PM_PERIPH_TYPE_LDO,
  PM_PERIPH_TYPE_SMPS,
  PM_PERIPH_TYPE_FULL_PERI_REG,
  PM_PERIPH_TYPE_SINGLE_REG,
  PM_PERIPH_TYPE_INVALID,
} pm_periph_type;

/**
  * PMIC Peripheral Register Info.
  */
typedef struct
{
  uint32 slave_id;  /* Slave ID */
  uint32 base_addr; /* Base Address */
  pm_periph_type periph;
} pm_reg_dump_list_type;


#endif /* __PM_INTERNAL_H__ */

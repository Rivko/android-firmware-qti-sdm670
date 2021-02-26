#ifndef __HALVCSHWIO_H__
#define __HALVCSHWIO_H__
/*
==============================================================================

FILE:         HALldoGenericLDO.h

DESCRIPTION:
  Generic HWIO macros used internally for the VCS HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/vcs/hal/hw/arch/HALvcsGenericHWIO.h#1 $

==============================================================================
            Copyright (c) 2017 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALhwio.h"


/* ============================================================================
**    Definitions
** ==========================================================================*/


#define HAL_VCS_HWIO_ADDR(reg, addr)  (addr + HWIO_OFFS(reg))

#define HAL_VCS_HWIO_IN(reg, addr) \
  inpdw(HAL_VCS_HWIO_ADDR(reg, addr))

#define HAL_VCS_HWIO_INF(reg, addr, fld) \
  ((inpdw(HAL_VCS_HWIO_ADDR(reg, addr)) & HWIO_FMSK(reg, fld)) >> HWIO_SHFT(reg, fld))

#define HAL_VCS_HWIO_INFV(reg, addr, fld, var) \
  ((var & HWIO_FMSK(reg, fld)) >> HWIO_SHFT(reg, fld))

#define HAL_VCS_HWIO_OUT(reg, addr, val) \
  outpdw(HAL_VCS_HWIO_ADDR(reg, addr), val)

#define HAL_VCS_HWIO_OUTF(reg, addr, fld, val) \
  outpdw(HAL_VCS_HWIO_ADDR(reg, addr), \
   (HAL_VCS_HWIO_IN(reg, addr) & ~HWIO_FMSK(reg, fld)) \
   | (((val) << HWIO_SHFT(reg, fld)) & HWIO_FMSK(reg, fld)))

#define HAL_VCS_HWIO_OUTFV(reg, addr, fld, val, var) \
   ((var & ~HWIO_FMSK(reg, fld)) \
   | (((val) << HWIO_SHFT(reg, fld)) & HWIO_FMSK(reg, fld)))


/* ============================================================================
**    Functions
** ==========================================================================*/

#endif  /* __HALVCSHWIO_H__ */

#ifndef HALSB_INTERNAL_H
#define HALSB_INTERNAL_H
/*
============================================================================

FILE:          HALsbInternal.h

DESCRIPTION:   HAL SB Internal header file.

                Copyright (c) 2010, 2011, 2012, 2013 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================

============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/hw/HALsbInternal.h#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-06-17  MJS  Update for new HWIO.
  2013-06-17  MJS  Deprecate HALsbTarget.h.
  2011-11-12  MJS  Remove references to DAL.
  2011-06-09  MJS  Add shadow register support.
  2010-07-26  ps   Initial version.
============================================================================
*/

/* ----------------------------------------------------------------------- 
** Includes
** ----------------------------------------------------------------------- */ 

#include "SlimBusTypes.h"
#include "HALhwio.h"    /* __inpdw() , __outpdw() macros */
#include "SlimBusHwio.h"   /* HWIO registers address , uMask and shift */

/* ----------------------------------------------------------------------- 
** Constants  SB
** ----------------------------------------------------------------------- */ 

#define SB_ADDR(base,hwiosym)        HWIO_SB_##hwiosym##_ADDR(base)
#define SB_ADDRn(base,hwiosym,index) HWIO_SB_##hwiosym##_ADDR(base,index)
#define SB_BSMSK(hwiosym,field)      HWIO_SB_##hwiosym##_##field##_BMSK
#define SB_SHFT(hwiosym,field)       HWIO_SB_##hwiosym##_##field##_SHFT
#define SB_MAXn(hwiosym)             HWIO_SB_##hwiosym##_MAXn

#define SB_HWIO_IN(base, hwiosym)                      __inpdw(SB_ADDR(base,hwiosym))
#define SB_HWIO_OUT(base, hwiosym, val)                __outpdw(SB_ADDR(base,hwiosym), (val))
#define SB_HWIO_INI(base, hwiosym, index)              __inpdw(SB_ADDRn(base,hwiosym,index))
#define SB_HWIO_OUTI(base, hwiosym, index, val)        __outpdw(SB_ADDRn(base,hwiosym,index), (val))
                                                        
#define SB_HWIO_INM(base, hwiosym, mask)               ( SB_HWIO_IN((base), hwiosym) & (mask))
#define SB_HWIO_OUTM(base, hwiosym, mask, val)         SB_HWIO_OUT((base), hwiosym, ((SB_HWIO_IN((base), hwiosym) & (~(mask))) | ((val) & (mask))))
#define SB_HWIO_INMI(base, hwiosym, index, mask)       ( SB_HWIO_INI(base, hwiosym, index) & (mask))
#define SB_HWIO_OUTMI(base, hwiosym, index, mask, val) SB_HWIO_OUTI(base, hwiosym, index, ((SB_HWIO_INI(base, hwiosym, index) & (~(mask))) | ((val) & (mask))))

#define SB_HWIO_INF(base, hwiosym, field)               ( (SB_HWIO_IN((base), hwiosym) & SB_BSMSK(hwiosym,field))>>SB_SHFT(hwiosym,field))
#define SB_HWIO_OUTF(base, hwiosym, field, val)         SB_HWIO_OUT((base), hwiosym, ((SB_HWIO_IN((base), hwiosym) & (~(SB_BSMSK(hwiosym,field)))) | ((val<<SB_SHFT(hwiosym,field)) & (SB_BSMSK(hwiosym,field)))))
#define SB_HWIO_INFI(base, hwiosym, index, field)       ( (SB_HWIO_INI((base), hwiosym , index) & SB_BSMSK(hwiosym,field))>>SB_SHFT(hwiosym,field))
#define SB_HWIO_OUTFI(base, hwiosym, index, field, val) SB_HWIO_OUTI((base), hwiosym, index, ((SB_HWIO_INI((base), hwiosym,index) & (~(SB_BSMSK(hwiosym,field)))) | ((val<<SB_SHFT(hwiosym,field)) & (SB_BSMSK(hwiosym,field)))))

#endif  /* HALSB_INTERNAL_H */

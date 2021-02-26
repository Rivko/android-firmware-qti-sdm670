#ifndef _RSC_HWIO_H_
#define _RSC_HWIO_H_
/*==============================================================================
  FILE:         rsc_hwio.h

  OVERVIEW:     This file contains the generic HWIO wrapper macros.

  DEPENDENCIES: None
                
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/rsc/hal/hwio/rsc_hwio.h#1 $
$DateTime: 2018/07/30 01:21:36 $
================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "HALhwio.h"
#include "rsc_HALhwioBase.h"

/* Ensure prefix is defined at build level */
#ifndef RSCC_HWIO_PREFIX
#error "RSCC_HWIO_PREFIX is undefined"
#endif

#ifndef RSC_HWIO_BASE
#error "HWIO Base not defined"
#endif

/*==========================================================================
 *                                MACROS
 *=========================================================================*/
/* Target independent macros used to access hardware registers. 
 * The base prefix must be defined as part of the build options 
 *
 * @Note
 * This list does not cover all HWIO macros. Only those macros used by 
 * the driver are defined here. If required, new wrapper macros can be added */
 
/* Common helper macros */
#define _HWIO_CONCAT_NAME(prefix, name)         prefix##_##name
#define HWIO_CONCAT_NAME(prefix, name)          _HWIO_CONCAT_NAME(prefix, name)

/*==========================================================================
 *                            SubSystem RSC block macros
 *=========================================================================*/
#define RSC_HWIO_EXPAND_NAME(name)                  name
#define RSC_HWIO_RMSK(name)                         HWIO_RMSK(RSC_HWIO_EXPAND_NAME(name))
#define RSC_HWIO_FMSK(name, hwio_fldsym)            HWIO_FMSK(RSC_HWIO_EXPAND_NAME(name), hwio_fldsym)
#define RSC_HWIO_VAL(name, field, val)              HWIO_VAL(RSC_HWIO_EXPAND_NAME(name), field, val)
#define RSC_HWIO_FVAL(name, field, val)             HWIO_FVAL(RSC_HWIO_EXPAND_NAME(name), field, val)

#define RSCC_HWIO_BASE_ADDR                         HWIO_CONCAT_NAME(RSCC_HWIO_PREFIX, RSCC_RSC_REG_BASE)
#define RSCP_HWIO_BASE_ADDR                         HWIO_CONCAT_NAME(RSCP_HWIO_PREFIX, RSCC_RSC_REG_BASE)


/* EXPLICIT HWIO BASE MACROS */
#define RSC_HWIO_INX(base, name)                              HWIO_INX(base,     RSC_HWIO_EXPAND_NAME(name))
#define RSC_HWIO_INXI(base, name, index)                      HWIO_INXI(base,    RSC_HWIO_EXPAND_NAME(name), index)
#define RSC_HWIO_INXI2(base, name, index1, index2)            HWIO_INXI2(base,   RSC_HWIO_EXPAND_NAME(name), index1, index2)

#define RSC_HWIO_INXM(base, name, mask)                       HWIO_INXM(base,    RSC_HWIO_EXPAND_NAME(name), mask)
#define RSC_HWIO_INXMI(base, name, index, mask)               HWIO_INXMI(base,   RSC_HWIO_EXPAND_NAME(name), index, mask)

#define RSC_HWIO_INXF(base, name, field)                      HWIO_INXF(base,    RSC_HWIO_EXPAND_NAME(name), field)
#define RSC_HWIO_INXFI(base, name, index, field)              HWIO_INXFI(base,   RSC_HWIO_EXPAND_NAME(name), index, field)
#define RSC_HWIO_INXFI2(base, name, idx1, idx2, field)        HWIO_INXFI2(base,  RSC_HWIO_EXPAND_NAME(name), idx1, idx2, field)

#define RSC_HWIO_OUTX(base, name, val)                        HWIO_OUTX(base,    RSC_HWIO_EXPAND_NAME(name), val)

#define RSC_HWIO_OUTXI(base, name, index, val)                HWIO_OUTXI(base,   RSC_HWIO_EXPAND_NAME(name), index, val)

#define RSC_HWIO_OUTXMI(base, name, index, mask, val)         HWIO_OUTXMI(base,  RSC_HWIO_EXPAND_NAME(name), index, mask, val)

#define RSC_HWIO_OUTXF(base, name, field, val)                HWIO_OUTXF(base,   RSC_HWIO_EXPAND_NAME(name), field, val)
#define RSC_HWIO_OUTXFI(base, name, index, field, val)        HWIO_OUTXFI(base,  RSC_HWIO_EXPAND_NAME(name), index, field, val)
#define RSC_HWIO_OUTXFI2(base, name, idx1, idx2, field, val)  HWIO_OUTXFI2(base, RSC_HWIO_EXPAND_NAME(name), idx1, idx2, field, val)

/* IMPLIED HWIO BASE MACROS */
#define RSC_HWIO_IN(name)                                 HWIO_IN(RSC_HWIO_EXPAND_NAME(name))
#define RSC_HWIO_INI(name, idx)                           HWIO_INI(RSC_HWIO_EXPAND_NAME(name), idx)

#define RSC_HWIO_INF(name, field)                         HWIO_INF(RSC_HWIO_EXPAND_NAME(name), field)
#define RSC_HWIO_INFI(name, idx1, field)                  HWIO_INFI(RSC_HWIO_EXPAND_NAME(name), idx1, field)
#define RSC_HWIO_INFI2(name, idx1, idx2, field)           HWIO_INFI2(RSC_HWIO_EXPAND_NAME(name), idx1, idx2, field)
#define RSC_HWIO_INM(name, mask)                          HWIO_INM(RSC_HWIO_EXPAND_NAME(name), mask)

#define RSC_HWIO_OUT(name, val)                           HWIO_OUT(RSC_HWIO_EXPAND_NAME(name), val)
#define RSC_HWIO_OUTI(name, idx, val)                     HWIO_OUTI(RSC_HWIO_EXPAND_NAME(name), idx, val)
#define RSC_HWIO_OUTI2(name, idx1, idx2, val)             HWIO_OUTI2(RSC_HWIO_EXPAND_NAME(name), idx1, idx2, val)

#define RSC_HWIO_OUTF(name, field, val)                   HWIO_OUTF(RSC_HWIO_EXPAND_NAME(name), field, val)
#define RSC_HWIO_OUTF1(name, idx1, field, val)            HWIO_OUTFI(RSC_HWIO_EXPAND_NAME(name), idx1, field, val)
#define RSC_HWIO_OUTF2(name, idx1, idx2, fld, val)        HWIO_OUTFI2(RSC_HWIO_EXPAND_NAME(name), idx1, idx2, fld, val)

#define RSC_HWIO_OUTM(name, idx1, mask, val)              HWIO_OUTM(RSC_HWIO_EXPAND_NAME(name), idx1, mask, val)
#define RSC_HWIO_OUTM2(name, idx1, idx2, mask, val)       HWIO_OUTMI2(RSC_HWIO_EXPAND_NAME(name), idx1, idx2, mask, val)

#ifdef __cplusplus
}
#endif

#endif /* _RSC_HWIO_H_ */


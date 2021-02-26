#ifndef _PDC_HWIO_H_
#define _PDC_HWIO_H_
/*==============================================================================
  FILE:         pdcHwio.h

  OVERVIEW:     This file contains the generic HWIO wrapper macros for sleep.

  DEPENDENCIES: None
                
                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/PdcTargetLib/inc/pdc_hwio.h#1 $
$DateTime: 2017/09/18 09:26:13 $
================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "HALhwio.h"

/* Ensure prefix is defined at build level */
#ifndef PDC_HWIO_PREFIX
#error "PDC_HWIO_PREFIX is undefined"
#endif

#ifndef RSCC_HWIO_PREFIX
#error "RSCC_HWIO_PREFIX is undefined"
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
#define _PDC_HWIO_CONCAT_NAME(prefix, name)         prefix##_##name
#define PDC_HWIO_CONCAT_NAME(prefix, name)          _PDC_HWIO_CONCAT_NAME(prefix, name)

/* PDC block macros */
#define PDC_HWIO_EXPAND_NAME(name)                  PDC_HWIO_CONCAT_NAME(PDC_HWIO_PREFIX, name)

#define PDC_HWIO_RMSK(name)                         HWIO_RMSK(PDC_HWIO_EXPAND_NAME(name))

#define PDC_HWIO_VAL(name, field, val)              HWIO_VAL(PDC_HWIO_EXPAND_NAME(name), field, val)
#define PDC_HWIO_FVAL(name, field, val)             HWIO_FVAL(PDC_HWIO_EXPAND_NAME(name), field, val)

#define PDC_HWIO_IN(name)                           HWIO_IN(PDC_HWIO_EXPAND_NAME(name))
#define PDC_HWIO_INI(name, idx)                     HWIO_INI(PDC_HWIO_EXPAND_NAME(name), idx)

#define PDC_HWIO_INF(name, field)                   HWIO_INF(PDC_HWIO_EXPAND_NAME(name), field)
#define PDC_HWIO_INF1(name, idx1, field)            HWIO_INFI(PDC_HWIO_EXPAND_NAME(name), idx1, field)
#define PDC_HWIO_INF2(name, idx1, idx2, field)      HWIO_INFI2(PDC_HWIO_EXPAND_NAME(name), idx1, idx2, field) 
#define PDC_HWIO_INM(name, mask)                    HWIO_INM(PDC_HWIO_EXPAND_NAME(name), mask)

#define PDC_HWIO_OUT(name, val)                     HWIO_OUT(PDC_HWIO_EXPAND_NAME(name), val)
#define PDC_HWIO_OUTI(name, idx, val)               HWIO_OUTI(PDC_HWIO_EXPAND_NAME(name), idx, val)
#define PDC_HWIO_OUTI2(name, idx1, idx2, val)       HWIO_OUTI2(PDC_HWIO_EXPAND_NAME(name), idx1, idx2, val)

#define PDC_HWIO_OUTF(name, field, val)             HWIO_OUTF(PDC_HWIO_EXPAND_NAME(name), field, val)
#define PDC_HWIO_OUTF1(name, idx1, field, val)      HWIO_OUTFI(PDC_HWIO_EXPAND_NAME(name), idx1, field, val)
#define PDC_HWIO_OUTF2(name, idx1, idx2, fld, val)  HWIO_OUTFI2(PDC_HWIO_EXPAND_NAME(name), idx1, idx2, fld, val)

#define PDC_HWIO_OUTM2(name, idx1, idx2, mask, val) HWIO_OUTMI2(PDC_HWIO_EXPAND_NAME(name), idx1, idx2, mask, val)

#define PDC_HWIO_FMSK(name, hwio_fldsym)            HWIO_FMSK(PDC_HWIO_EXPAND_NAME(name), hwio_fldsym)


/* SubSystem RSC block macros */
#define RSC_HWIO_EXPAND_NAME(name)                  PDC_HWIO_CONCAT_NAME(RSCC_HWIO_PREFIX, name)

#define RSC_HWIO_RMSK(name)                         HWIO_RMSK(RSC_HWIO_EXPAND_NAME(name))

#define RSC_HWIO_VAL(name, field, val)              HWIO_VAL(RSC_HWIO_EXPAND_NAME(name), field, val)
#define RSC_HWIO_FVAL(name, field, val)             HWIO_FVAL(RSC_HWIO_EXPAND_NAME(name), field, val)

#define RSC_HWIO_IN(name)                           HWIO_IN(RSC_HWIO_EXPAND_NAME(name))
#define RSC_HWIO_INI(name, idx)                     HWIO_INI(RSC_HWIO_EXPAND_NAME(name), idx)

#define RSC_HWIO_INF(name, field)                   HWIO_INF(RSC_HWIO_EXPAND_NAME(name), field)
#define RSC_HWIO_INF1(name, idx1, field)            HWIO_INFI(RSC_HWIO_EXPAND_NAME(name), idx1, field)
#define RSC_HWIO_INF2(name, idx1, idx2, field)      HWIO_INFI2(RSC_HWIO_EXPAND_NAME(name), idx1, idx2, field) 
#define RSC_HWIO_INM(name, mask)                    HWIO_INM(RSC_HWIO_EXPAND_NAME(name), mask)

#define RSC_HWIO_OUT(name, val)                     HWIO_OUT(RSC_HWIO_EXPAND_NAME(name), val)
#define RSC_HWIO_OUTI(name, idx, val)               HWIO_OUTI(RSC_HWIO_EXPAND_NAME(name), idx, val)
#define RSC_HWIO_OUTI2(name, idx1, idx2, val)       HWIO_OUTI2(RSC_HWIO_EXPAND_NAME(name), idx1, idx2, val)

#define RSC_HWIO_OUTF(name, field, val)             HWIO_OUTF(RSC_HWIO_EXPAND_NAME(name), field, val)
#define RSC_HWIO_OUTF1(name, idx1, field, val)      HWIO_OUTFI(RSC_HWIO_EXPAND_NAME(name), idx1, field, val)
#define RSC_HWIO_OUTF2(name, idx1, idx2, fld, val)  HWIO_OUTFI2(RSC_HWIO_EXPAND_NAME(name), idx1, idx2, fld, val)

#define RSC_HWIO_OUTM2(name, idx1, idx2, mask, val) HWIO_OUTMI2(RSC_HWIO_EXPAND_NAME(name), idx1, idx2, mask, val)

#define RSC_HWIO_FMSK(name, hwio_fldsym)            HWIO_FMSK(RSC_HWIO_EXPAND_NAME(name), hwio_fldsym)

#ifdef __cplusplus
}
#endif

#endif /* _PDC_HWIO_H_ */


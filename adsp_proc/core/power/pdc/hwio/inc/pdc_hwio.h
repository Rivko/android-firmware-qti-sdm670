#ifndef _PDC_HWIO_H_
#define _PDC_HWIO_H_
/*==============================================================================
  FILE:         pdcHwio.h

  OVERVIEW:     This file contains the generic HWIO wrapper macros for sleep.

  DEPENDENCIES: None
                
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/hwio/inc/pdc_hwio.h#2 $
$DateTime: 2017/11/05 22:36:52 $
================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "HALhwio.h"
#include "pdc_HALhwioBase.h"

/* Ensure prefix is defined at build level */
#ifndef PDC_HWIO_PREFIX
#error "PDC_HWIO_PREFIX is undefined"
#endif

#ifndef RSCC_HWIO_PREFIX
#error "RSCC_HWIO_PREFIX is undefined"
#endif

#ifndef PDC_HWIO_BASE
#error "HWIO Base not defined"
#endif

/*==============================================================================
                             EXTERNAL GLOBAL VARIABLES
 =============================================================================*/
extern uint32 g_pdcHWIObase;

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
 *                            PDC block macros
 *=========================================================================*/
#define PDC_HWIO_EXPAND_NAME(name)                  HWIO_CONCAT_NAME(PDC_HWIO_PREFIX, name)
#define PDC_HWIO_FMSK(name, field)                  HWIO_FMSK(PDC_HWIO_EXPAND_NAME(name), field)
#define PDC_HWIO_SHFT(name, field)                  HWIO_SHFT(PDC_HWIO_EXPAND_NAME(name), field)
#define PDC_HWIO_RMSK(name)                         HWIO_RMSK(PDC_HWIO_EXPAND_NAME(name))
#define PDC_HWIO_FIELD_VAL(name, field, value)      ((value & PDC_HWIO_FMSK(name, field)) >> PDC_HWIO_SHFT(name, field))
#define PDC_HWIO_BASE_ADDR                          HWIO_CONCAT_NAME(PDC_HWIO_EXPAND_NAME(PDC_HWIO_BASE), PDC_REG_BASE)

#define PDC_HWIO_VAL(name, field, val)              HWIO_VAL(PDC_HWIO_EXPAND_NAME(name), field, val)
#define PDC_HWIO_FVAL(name, field, val)             HWIO_FVAL(PDC_HWIO_EXPAND_NAME(name), field, val)

/* EXPLICIT HWIO BASE MACROS */
#define PDC_HWIO_REG_OFFSETX(base, name)                         (HWIO_ADDR(name) - base)
#define PDC_HWIO_REG_OFFSETXI(base, name, idx)                   (HWIO_ADDRI(name, idx1) - base)
#define PDC_HWIO_REG_OFFSETXI2(base, name, idx1, idx2)           (HWIO_ADDRI2(name, idx1, idx2) - base)
#define PDC_HWIO_REG_OFFSETXI3(base, name, idx1, idx2, idx3)     (HWIO_ADDRI3(name, idx1, idx2, idx3) - base)

#define PDC_HWIO_INX(base, name)                          HWIO_INX(base,    PDC_HWIO_EXPAND_NAME(name))
#define PDC_HWIO_INXI(base, name, index)                  HWIO_INXI(base,   PDC_HWIO_EXPAND_NAME(name), index)
#define PDC_HWIO_INXI2(base, name, index1, index2)        HWIO_INXI2(base,  PDC_HWIO_EXPAND_NAME(name), index1, index2)

#define PDC_HWIO_INXM(base, name, mask)                   HWIO_INXM(base,   PDC_HWIO_EXPAND_NAME(name), mask)
#define PDC_HWIO_INXMI(base, name, index, mask)           HWIO_INXMI(base,  PDC_HWIO_EXPAND_NAME(name), index, mask)

#define PDC_HWIO_INXF(base, name, field)                  HWIO_INXF(base,   PDC_HWIO_EXPAND_NAME(name), field)
#define PDC_HWIO_INXFI(base, name, index, field)          HWIO_INXFI(base,  PDC_HWIO_EXPAND_NAME(name), index, field)
#define PDC_HWIO_INXFI2(base, name, index, index2, field) HWIO_INXFI2(base, PDC_HWIO_EXPAND_NAME(name), index1, index2, field)

#define PDC_HWIO_OUTX(base, name, val)                    HWIO_OUTX(base,   PDC_HWIO_EXPAND_NAME(name), val)

#define PDC_HWIO_OUTXI(base, name, index, val)            HWIO_OUTXI(base,  PDC_HWIO_EXPAND_NAME(name), index, val)
#define PDC_HWIO_OUTXI2(base, name, index1, index2, val)  HWIO_OUTXI2(base, PDC_HWIO_EXPAND_NAME(name), index1, index2, val)

#define PDC_HWIO_OUTXF(base, name, field, val)            HWIO_OUTXF(base,  PDC_HWIO_EXPAND_NAME(name), field, val)
#define PDC_HWIO_OUTXFI(base, name, index, field, val)    HWIO_OUTXFI(base, PDC_HWIO_EXPAND_NAME(name), index, field, val)

#define PDC_HWIO_OUTXM(base, name, mask,  val)            HWIO_OUTXM(base,  PDC_HWIO_EXPAND_NAME(name), mask, val)
#define PDC_HWIO_OUTXMI(base, name, index, mask, val)     HWIO_OUTXMI(base, PDC_HWIO_EXPAND_NAME(name), index, mask, val)

/* IMPLIED HWIO BASE MACROS */
#define PDC_HWIO_REG_OFFSET(name)                         (HWIO_ADDRX(g_pdcHWIObase, PDC_HWIO_EXPAND_NAME(name)) - g_pdcHWIObase)
#define PDC_HWIO_REG_OFFSETI(name, idx)                   (HWIO_ADDRXI(g_pdcHWIObase, PDC_HWIO_EXPAND_NAME(name), idx1) - g_pdcHWIObase)
#define PDC_HWIO_REG_OFFSETI2(name, idx1, idx2)           (HWIO_ADDRXI2(g_pdcHWIObase, PDC_HWIO_EXPAND_NAME(name), idx1, idx2) - g_pdcHWIObase)
#define PDC_HWIO_REG_OFFSETI3(name, idx1, idx2, idx3)     (HWIO_ADDRXI3(g_pdcHWIObase, PDC_HWIO_EXPAND_NAME(name), idx1, idx2, idx3) - g_pdcHWIObase)

#define PDC_HWIO_IN(name)                                 HWIO_INX(g_pdcHWIObase,    PDC_HWIO_EXPAND_NAME(name))
#define PDC_HWIO_INI(name, index)                         HWIO_INXI(g_pdcHWIObase,   PDC_HWIO_EXPAND_NAME(name), index)
#define PDC_HWIO_INI2(name, index1, index2)               HWIO_INXI2(g_pdcHWIObase,  PDC_HWIO_EXPAND_NAME(name), index1, index2)

#define PDC_HWIO_INM(name, mask)                          HWIO_INXM(g_pdcHWIObase,   PDC_HWIO_EXPAND_NAME(name), mask)
#define PDC_HWIO_INMI(name, index, mask)                  HWIO_INXMI(g_pdcHWIObase,  PDC_HWIO_EXPAND_NAME(name), index, mask)

#define PDC_HWIO_INF(name, field)                         HWIO_INXF(g_pdcHWIObase,   PDC_HWIO_EXPAND_NAME(name), field)
#define PDC_HWIO_INFI(name, index, field)                 HWIO_INXFI(g_pdcHWIObase,  PDC_HWIO_EXPAND_NAME(name), index, field)
#define PDC_HWIO_INFI2(name, index1, index2, field)       HWIO_INXFI2(g_pdcHWIObase, PDC_HWIO_EXPAND_NAME(name), index1, index2, field)

#define PDC_HWIO_OUT(name, val)                           HWIO_OUTX(g_pdcHWIObase,   PDC_HWIO_EXPAND_NAME(name), val)

#define PDC_HWIO_OUTI(name, index, val)                   HWIO_OUTXI(g_pdcHWIObase,  PDC_HWIO_EXPAND_NAME(name), index, val)
#define PDC_HWIO_OUTI2(name, index1, index2, val)         HWIO_OUTXI2(g_pdcHWIObase, PDC_HWIO_EXPAND_NAME(name), index1, index2, val)

#define PDC_HWIO_OUTF(name, field, val)                   HWIO_OUTXF(g_pdcHWIObase,  PDC_HWIO_EXPAND_NAME(name), field, val)
#define PDC_HWIO_OUTFI(name, index, field, val)           HWIO_OUTXFI(g_pdcHWIObase, PDC_HWIO_EXPAND_NAME(name), index, field, val)
#define PDC_HWIO_OUTFI2(name, index1, index2, field, val) HWIO_OUTXFI2(g_pdcHWIObase,PDC_HWIO_EXPAND_NAME(name), index1, index2, field, val)

#define PDC_HWIO_OUTM(name, mask,  val)                   HWIO_OUTXM(g_pdcHWIObase,  PDC_HWIO_EXPAND_NAME(name), mask, val)
#define PDC_HWIO_OUTMI(name, index, mask, val)            HWIO_OUTXMI(g_pdcHWIObase, PDC_HWIO_EXPAND_NAME(name), index, mask, val)
#define PDC_HWIO_OUTMI2(name, index1, index2, mask, val)  HWIO_OUTXMI2(g_pdcHWIObase,PDC_HWIO_EXPAND_NAME(name), index1, index2, mask, val)

/*==========================================================================
 *                            SubSystem RSC block macros
 *=========================================================================*/
#define RSC_HWIO_EXPAND_NAME(name)                  HWIO_CONCAT_NAME(RSCC_HWIO_PREFIX, name)
#define RSC_HWIO_RMSK(name)                         HWIO_RMSK(RSC_HWIO_EXPAND_NAME(name))
#define RSC_HWIO_FMSK(name, hwio_fldsym)            HWIO_FMSK(RSC_HWIO_EXPAND_NAME(name), hwio_fldsym)
#define RSC_HWIO_BASE_ADDR                          HWIO_CONCAT_NAME(RSCC_HWIO_PREFIX, RSCC_RSC_REG_BASE)
#define RSC_HWIO_VAL(name, field, val)              HWIO_VAL(RSC_HWIO_EXPAND_NAME(name), field, val)
#define RSC_HWIO_FVAL(name, field, val)             HWIO_FVAL(RSC_HWIO_EXPAND_NAME(name), field, val)

/* EXPLICIT HWIO BASE MACROS */
#define RSC_HWIO_INX(base, name)                          HWIO_INX(base,     RSC_HWIO_EXPAND_NAME(name))
#define RSC_HWIO_INXI(base, name, index)                  HWIO_INXI(base,    RSC_HWIO_EXPAND_NAME(name), index)
#define RSC_HWIO_INXI2(base, name, index1, index2)        HWIO_INXI2(base,   RSC_HWIO_EXPAND_NAME(name), index1, index2)

#define RSC_HWIO_INXM(base, name, mask)                   HWIO_INXM(base,    RSC_HWIO_EXPAND_NAME(name), mask)
#define RSC_HWIO_INXMI(base, name, index, mask)           HWIO_INXMI(base,   RSC_HWIO_EXPAND_NAME(name), index, mask)

#define RSC_HWIO_INXF(base, name, field)                  HWIO_INXF(base,    RSC_HWIO_EXPAND_NAME(name), field)
#define RSC_HWIO_INXFI(base, name, field, index)          HWIO_INXFI(base,   RSC_HWIO_EXPAND_NAME(name), field, index)

#define RSC_HWIO_OUTX(base, name, val)                    HWIO_OUTX(base,    RSC_HWIO_EXPAND_NAME(name), val)

#define RSC_HWIO_OUTXI(base, name, index, val)            HWIO_OUTXI(base,   RSC_HWIO_EXPAND_NAME(name), index, val)

#define RSC_HWIO_OUTXF(base, name, field, val)            HWIO_OUTXF(base,   RSC_HWIO_EXPAND_NAME(name), field, val)
#define RSC_HWIO_OUTXFI(base, name, index, field, val)    HWIO_OUTXFI(base,  RSC_HWIO_EXPAND_NAME(name), index, field, val)

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

#define RSC_HWIO_OUTM2(name, idx1, idx2, mask, val)       HWIO_OUTMI2(RSC_HWIO_EXPAND_NAME(name), idx1, idx2, mask, val)

#ifdef __cplusplus
}
#endif

#endif /* _PDC_HWIO_H_ */


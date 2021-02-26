#ifndef _SLEEP_HWIO_H_
#define _SLEEP_HWIO_H_
/*==============================================================================
  FILE:         sleep_hwio.h

  OVERVIEW:     This file contains the generic HWIO wrapper macros for sleep.

  DEPENDENCIES: None
                
                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/hwio/sleep_hwio.h#1 $
$DateTime: 2018/07/30 01:21:36 $
================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "HALhwio.h"
#include "sleep_HALhwio.h"

/* Ensure prefix is defined at build level */
#ifndef SLEEP_HWIO_PREFIX
#error "SLEEP_HWIO_PREFIX is undefined"
#endif

/*==========================================================================
 *                                MACROS
 *=========================================================================*/
/* These macros are used to defer register name - build script can provide
 * the necessary parameters to construct actual register name.
 *
 * @Note
 * This list does not cover all HWIO macros. Only those macros used by 
 * SLEEP layer are defined here. If required, new wrapper macros can
 * be added here. */
#define _SLEEP_HWIO_CONCAT_NAME(prefix, name)         prefix##_##name
#define SLEEP_HWIO_CONCAT_NAME(prefix, name)          _SLEEP_HWIO_CONCAT_NAME(prefix, name)
#define SLEEP_HWIO_EXPAND_NAME(name)                  SLEEP_HWIO_CONCAT_NAME(SLEEP_HWIO_PREFIX, name)

#define SLEEP_HWIO_VAL(name, field, val)              HWIO_VAL(SLEEP_HWIO_EXPAND_NAME(name), field, val)
#define SLEEP_HWIO_FVAL(name, field, val)             HWIO_FVAL(SLEEP_HWIO_EXPAND_NAME(name), field, val)

#define SLEEP_HWIO_IN(name)                           HWIO_IN(SLEEP_HWIO_EXPAND_NAME(name))
#define SLEEP_HWIO_INF(name, field)                   HWIO_INF(SLEEP_HWIO_EXPAND_NAME(name), field)
#define SLEEP_HWIO_INM(name, mask)                    HWIO_INM(SLEEP_HWIO_EXPAND_NAME(name), mask)

#define SLEEP_HWIO_OUT(name, val)                     HWIO_OUT(SLEEP_HWIO_EXPAND_NAME(name), val)
#define SLEEP_HWIO_OUTF(name, field, val)             HWIO_OUTF(SLEEP_HWIO_EXPAND_NAME(name), field, val)

#define SLEEP_HWIO_FMSK(name, hwio_fldsym)            HWIO_FMSK(SLEEP_HWIO_EXPAND_NAME(name), hwio_fldsym)

#ifdef __cplusplus
}
#endif

#endif /* _SLEEP_HWIO_H_ */


/*
 * @file smp2p_os.h
 *
 * OS specific routines and definitions for SMP2P.
 */

/*==============================================================================
     Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/smp2p/inc/smp2p_os.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/17   jlg     Removed  smp2p_os_proc_type and  smp2p_os_intr_type
06/13/14   bc      Added QDI support
10/10/12   pa      Initial revision.
===========================================================================*/
#ifndef SMP2P_OS_H_
#define SMP2P_OS_H_

#include "DALSys.h"
#include "DALFramework.h"
#include "DDIIPCInt.h"
#include "assert.h"
#include "smp2p.h"

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/
#ifdef FEATURE_SMP2P_ASSERT_BRK
/*  To enable this, define FEATURE_SMP2P_ASSERT_BRK, or just change the above
 *  line to #if 1. */
/** Breakpoint assert.  Halts the processor at the assert location rather
 *  than going through the error handling code.
 *  Useful for bringup of new code.
 */
#define ASSERT_BRK(cond) if (!(cond)) { \
  __asm__ __volatile__ ( \
  "brkpt \n" \
  : \
  : \
  :"memory"); }
#else
#define ASSERT_BRK  ASSERT
#endif

/** NULL Interrupt definitions */
#define SMP2P_NULL_IN_INTERRUPT  ( DALIPCINT_INT_32BITS )
#define SMP2P_NULL_OUT_INTERRUPT SMP2P_NULL_IN_INTERRUPT

/** SMP2P log macro.
 *  This macro defines the log routines used by SMP2P to track events that
 *  occur at runtime.
 *  This is defined in the processor specific header file so that each
 *  processor may define it's own policy. */
#define SMP2P_LOG(a,b,c,d)      smp2p_os_log(a,b,c,d, SMP2P_FLAGS_NONE)

/** SMP2P log macro with flags */
#define SMP2P_LOG_EX(a,b,c,d,e) smp2p_os_log(a,b,c,d,e)

/** Defines the number of entries in the SMP2P local log */
#define SMP2P_LOG_NUM_ENTRIES 64

/** This define, if present, enables local logging */
#define SMP2P_LOG_LOCAL

/** This define, if present, enables logging to SMEM log */
/* #define SMP2P_LOG_SMEM */

/*===========================================================================
                       TYPE DEFINITIONS
===========================================================================*/

/** OS abstraction over DAL types */
typedef DalIPCIntProcessorType smp2p_os_proc_type;
typedef DalIPCIntInterruptType smp2p_os_intr_type;

#ifdef FEATURE_SMP2P_QDI_SUPPORTED
typedef struct smp2p_qdi_context smp2p_qdi_context_type; 
#endif

/** SMP2P OS parameters
 * Contains any OS-specific variables needed by the SMP2P API.
 * Currently only defined for DAL, though other OS's may be added.
 */
typedef struct
{
  DALSYSEventHandle hEvent;
  
#ifdef FEATURE_SMP2P_QDI_SUPPORTED
  /* context that includes qdi signal to userspace */
  smp2p_qdi_context_type *smp2p_qdi_context;
#endif
} smp2p_os_params_type;

/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  smp2p_qdi_context_set_entry_name
===========================================================================*/
/**
  Set entry name in os_param. Entry name will be used to find out proper QDI
  signal to userspace client

  @param [out]   ctx         smp2p_context
  @param [in]    entry_name  entry name to set

  @return
  none
  
  @dependencies
  None.
*/
/*=========================================================================*/
#ifdef FEATURE_SMP2P_QDI_SUPPORTED
void smp2p_qdi_context_set_entry_name
(
  smp2p_context_type * ctx,
  char               * entry_name
);
#endif

/* -------------------------------------------------------------
   Memory barrier for REX
------------------------------------------------------------- */
#define SMP2P_MEMORY_BARRIER() memory_barrier()

#endif /* SMP2P_OS_H_ */
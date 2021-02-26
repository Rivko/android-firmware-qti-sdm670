/*============================================================================
@file hwsolver_internal.h

Internal type and function declarations

Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
#ifndef __HWSOLVER_INTERNAL_H__
#define __HWSOLVER_INTERNAL_H__

#include "hwsolver.h"
#include "CoreVerify.h"
#include "qurt.h"
#include "ULogFront.h"
#include "npa_resource.h"
#include "hwsolver_hwio.h"

#ifdef HWSOLVER_USES_ISLAND
#ifdef HWSOLVER_USES_UNPA
#include "unpa_resource.h"
#include "uSleep_npa.h"
#endif
#include "micro_ULog.h"
#include "uSleep_mode_trans.h"
#endif

#define HWSOLVER_NUM_DRVS (HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_MAXd+1)

#ifdef __cplusplus
extern "C" {
#endif

/* Macros for locking/unlocking the mutex around hwsolver API's internal states */ 
#define HWSLVR_LOCK()  qurt_mutex_lock(&hwsolver.lock);
#define HWSLVR_UNLOCK()  qurt_mutex_unlock(&hwsolver.lock);

/*----------------------------------------------------------------------------
 * Types
 * -------------------------------------------------------------------------*/

typedef struct hwsolver_struct 
{
  qurt_mutex_t      lock;
  ULogHandle        log;
  unsigned int      explicit_disabled[HWSOLVER_NUM_DRVS];
  unsigned int      latency_disabled[HWSOLVER_NUM_DRVS];
  npa_query_handle  latency_qh;
  unsigned int      latency_val;

  npa_query_handle  max_duration_qh;

#ifdef HWSOLVER_USES_ISLAND  
  unpa_query_handle ulatency_qh;
#endif  
} hwsolver_struct;


/*----------------------------------------------------------------------------
 * Routines
 * -------------------------------------------------------------------------*/

/**
 * @brief hwsolver_lpr_init
 * Init function for the HWSolver LPR. Signature is due to the routine being
 * a NPA resource available CB.
 */
void hwsolver_lpr_init( void *context, unsigned evt_type,
                        void *data, unsigned int data_size );
                        
/**
 * @brief hwsolver_island_init
 * Initialize island data structures, log handles etc. in DDR/Normal mode.
 */
void hwsolver_island_init( void );

#ifdef __cplusplus
}
#endif

#endif /* __HWSOLVER_INTERNAL_H__ */

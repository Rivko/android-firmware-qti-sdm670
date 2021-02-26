#ifndef _RSC_H_
#define _RSC_H_

/*==============================================================================
  FILE:         rsc.h

  OVERVIEW:     This file contains APIs for external modules to interact with
                RSC HW block(s).

  DEPENDENCIES: None
                
                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/

#include <stddef.h>
#include "HALcomdef.h"
#include "rsc_common.h" 
#include "rsc_hal_bsp.h"

/*=======================================================================
 *                           MACRO DEFINITIONS
 *======================================================================*/
#define SIMPLE_8BIT_MODE 8
#define EXTENDED_16BIT_MODE 16

#ifndef RSC_DYNAMIC_SEQ

#include "comdef.h"
#include "CoreMutex.h"
#include "CoreVerify.h"

#define RSC_SEQ_LOCK(handle)    Core_MutexLock((CoreMutex *)handle->lock)
#define RSC_SEQ_UNLOCK(handle)  Core_MutexUnlock((CoreMutex *)handle->lock)
#define RSC_MUTEX_CREATE(lock)  lock = (void *)Core_MutexCreate(CORE_MUTEXATTR_DEFAULT); \
                                CORE_VERIFY_PTR(lock)

#else /* RSC_DYNAMIC_SEQ */

#define RSC_SEQ_LOCK(handle)
#define RSC_SEQ_UNLOCK(handle)
#define RSC_MUTEX_CREATE(lock)

#endif /* RSC_DYNAMIC_SEQ */


/*=======================================================================
 *                           TYPE DEFINITIONS
 *======================================================================*/
/**
 * @brief Generic enumeration that can be used for different
 * RSC sequence mode ID's
 */
typedef enum
{
	RSC_MODE_CLK_GATE,         /* Gate Clock to Processor Core */
	RSC_MODE_VDD_MIN,          /* Minimize the Processor Core Voltage */
	RSC_MODE_APCR_PLL_ON,      /* APCR/Fast PC with PLL on */
	RSC_MODE_APCR_PLL_LPM,     /* APCR/Fast PC with PLL stand by or off */
	RSC_MODE_APCR_RAIL_LPM,    /* APCR and transition supply rail to LPM */
	RSC_MODE_PWRC,             /* PC Processor Core using PMIC */
	RSC_MODE_PWRC_BHS,         /* PC Q6 CORE using BHS. */
	RSC_MODE_PWRC_L2RET,       /* PC with L2 retention */
	RSC_MODE_L2_RETENTION,     /* Retain Power to L2 Cache Memory */
	RSC_MODE_L2_GDHS,          /* Power Off L2 Cache using Local Switch */
	RSC_MODE_L2_OFF,           /* Power Off L2 Cache */
	RSC_NUM_LOW_POWER_MODES
 }rsc_low_power_mode_type;
 
/**
 * @brief Forward declaration of RSC low power mode structure.
 */
struct rsc_hal_bsp_mode_s;

/**
 * @brief Forward declaration of RSC instance parameter info.
 */
struct rsc_hal_param_info_s;

/**
 * @brief Structure containing information about a specific RSC instance.
 */
typedef struct rsc_s
{
  const char *name;                  			/* Name of the rsc instance - same as controlled entity e.g core_n, clust_m */
  uint8 op_mode;                     			/* RSC sequencer will work in simple or extended mode (8 bit vs 16 bit) */
  struct rsc_hal_bsp_mode_s* modes;  			/* List of modes in this rsc */
  uint32 mode_count;                 			/* Number of modes supported in the above list */

  /* Internal fields */
  size_t offset;                     			/* Offset from subsystem base in mem map */
  uint8 *addr;                       			/* Instance physical address in memory map */
  void  *lock;                      			/* Mutex to prevent multiple operations on this instance */
  uint16  copied_cmds;                    /* Number of commands copied on this SPM instance */

  /* Dynamic mode support */
  struct rsc_hal_bsp_mode_s *dyn_modes;   /* List of dynamic modes for this RSC */
  uint32 dyn_mode_count;             			/* Number of mode in above list */

  /* Param info */
  struct rsc_hal_param_info_s *param_cfg_info; 	/* Holds RSC configuration information */

  /* Onetime boolean values */
  boolean ts_init_done;							        /* Time stamp initialization done are not */
}rsc_t;

/*=======================================================================
 *                           GLOBAL FUNCTIONS
 *======================================================================*/

/**
 * <!-- rsc_driver_init -->
 *
 * @brief Populate available RSC instances. This information is useful for the
 *        clients to initialize respective subsystem RSC.
 *
 * @note This function must be called before calling any other RSC function.
 */
void rsc_driver_init(void);

/**
 * <!-- rsc_instance_init -->
 *
 * @brief Populate RSC instance parameters for respective RSC instance i.e qdsp6/mss.
 *
 * @param handle: Handle to RSC instance (qdsp6/mss) which needs to be initialized.
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_instance_init(rsc_handle handle);

/**
 * <!-- rsc_ts_init -->
 *
 * @brief Initialize time stamp (ts) unit for RSC(qdsp6/mss).
 *
 * @param handle: Handle to RSC instance (qdsp6/mss) to which ts needs to be initialized.
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_ts_init(rsc_handle handle);

/**
 * <!-- rsc_set_low_power_mode -->
 *
 * @brief Sets a low power mode in RSC. The configured low power mode
 *        settings will take place next time RSC state machine kicks
 *        in.
 *
 * @param handle: Handle to RSC which needs to be configured for low power mode.
 * @param mode_id: Low power mode id (typically corresponds to the ones defined
 *              in subsystem HPG).
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_set_low_power_mode(rsc_handle handle, uint8 mode_id);

/**
 * <!-- rsc_get_time_stamp -->
 *
 * @brief Get the time stamp information for RSC instance and respective lpm mode_id
 *
 * @param handle: Handle to RSC which time stamp needs to be queried.
 * @param mode_id: Low power mode id (typically corresponds to the ones defined
 *              in subsystem HPG).
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_get_time_stamp(rsc_handle handle, uint8 mode_id);
#endif

#ifndef _RSC_H_
#define _RSC_H_
/*==============================================================================
  FILE:         rsc.h

  OVERVIEW:     This file contains APIs for external modules to interact with
                RSC HW block(s).

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/rsc/inc/rsc.h#2 $
$DateTime: 2017/11/16 08:24:49 $
==============================================================================*/
#include "comdef.h"
#include "rsc_common.h" 
#include "rsc_hal_bsp.h"
#include "qurt.h"

/*=======================================================================
 *                           MACRO DEFINITIONS
 *======================================================================*/
/* Common macro to fill out structure members for array followed by its size. */
#define RSC_ARRAY(arr)    arr, (sizeof(arr)/sizeof(arr[0]))

/* Macros for mutext locking */
#define RSC_SEQ_LOCK(type)      qurt_mutex_lock(&g_rscs[type].lock)
#define RSC_SEQ_UNLOCK(type)    qurt_mutex_unlock(&g_rscs[type].lock)
#define RSC_MUTEX_CREATE(type)  qurt_mutex_init(&g_rscs[type].lock)

/*=======================================================================
 *                           TYPE DEFINITIONS
 *======================================================================*/
/* Sequence branching data */
typedef struct branch_info_s
{
  uint16  idx;    /* Branch index */
  uint16  offset; /* Branch address offset */
}branch_info;

/* Generic enumeration that can be used for different RSC sequence mode ID's */
typedef enum
{
  /** RSC child modes **/
  RSC_MODE_CLK_GATE,         /* Gate Clock to Processor Core */

  /* APCR modes */
  RSC_MODE_APCR_PLL_ON,         /* APCR/Fast PC with PLL on */
  RSC_MODE_APCR_PLL_LPM,        /* APCR/Fast PC with PLL standby or off */
  RSC_MODE_APCR_PLL_LPM_NO_SS,  /* APCR/Fast PC with PLL standby or off without subsystem sleep set (SLPI ONLY) */ 

  /* Full PC modes - L2 & TCM non-retention */
  RSC_MODE_PWRC,             /* PC Q6 Core using PMIC */
  RSC_MODE_PWRC_BHS,         /* PC Q6 Core using BHS. */

  /* L2 Nonretention modes */
  RSC_MODE_L2_NRET,          /* PMIC PC with L2 non-retention, TCM retention */
  RSC_MODE_L2_NRET_BHS,      /* BHS PC with L2 non-retetion, TCM retention */

  /** RSC parent modes **/
  RSC_PARENT_MODE_SS_LPM,    /* Subsystem sleep */
  RSC_PARENT_MODE_CHIP_LPM,  /* Subsystem sleep + PDC handshake for chip sleep */
  RSC_PARENT_MODE_CG_PLL_LPM,/* Clockgating with PLL in LPM */
 
  /** SW TRIGGERABLE MODE FLAG (last) */
  RSC_SW_TRIGGER_FLAG_LPM    /* Value that is added to indicate a software triggerable sequence */
   
}rsc_low_power_mode_type;

/* Enumeration values to be used for time profiling events */
typedef enum rsc_profile_unit_type_s 
{
  RSC_PROFILE_POWER_DOWN_START  = 0x01,
  RSC_PROFILE_POWER_DOWN_END    = 0x02,
  RSC_PROFILE_POWER_UP_START    = 0x04,
  RSC_PROFILE_POWER_UP_END      = 0x08,

  RSC_PROFILE_ALL_MASK          = ((RSC_PROFILE_POWER_UP_END << 1) - 1)
}rsc_profile_unit_type;

/* Standard fixed number of profiling units */
#define RSC_PROFILE_NUMBER_OF_UNITS 4

/* Structure containing information about a specific RSC instance */
typedef struct rsc_s
{
  rsc_modes_t   *modes;         /* List of modes in this rsc */
  uint32        mode_count;     /* Number of modes supported in the above list */
  uint32        *sequence;      /* Sequence data */
  uint32        sequence_size;  /* Sequence size */
  branch_info   *branch_info;   /* Branch address information */
  uint16        br_count;       /* Number of branch elements */
  uint32        addr;           /* Base address in memory map */
  qurt_mutex_t  lock;
}rsc_t;

/* Data assocated with each timestamp unit */
typedef struct rsc_profile_unit_s
{
  uint64  timeStamp;
  boolean valid;
  boolean overflow;
}rsc_profile_unit;

/*=======================================================================
 *                           EXTERNAL VARIABLES
 *======================================================================*/
extern rsc_t  *g_rscs;
extern uint32 g_rscCount;

/*=======================================================================
 *                           GLOBAL FUNCTIONS
 *======================================================================*/
/**
 * rsc_driver_init
 *
 * @brief Populate available RSC instances. This information is useful for the
 *        clients to initialize respective subsystem RSC.
 *
 * @note This function must be called before calling any other RSC function.
 */
void rsc_driver_init(void);

/**
 * rsc_set_low_power_mode
 *
 * @brief Sets a low power mode in RSC. The configured low power mode
 *        settings will take place next time RSC state machine kicks
 *        in.
 *
 * @param type:     RSC type which needs to be configured for low power mode.
 * @param modeId:   Low power mode id (typically corresponds to the ones defined
 *                  in subsystem HPG).
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_set_low_power_mode(rsc_type type, rsc_low_power_mode_type modeId);

/**
 * rsc_profile_get_unit_data
 *
 * @brief Gets the desired profiling unit data which will be populated after
 *        resuming from power collapse
 *
 * @param type:     RSC type which time stamp needs to be queried.
 * @param unitData: Data structure pointer where the requested unit data
 *                  will be populated.
 * @param bitmask:  Bit mask of the desired profiling units to get
 *
 * @return Number of timestamps have read. Return '0' if no timestamps
 *         have read. 
 */
void rsc_profile_get_unit_data(rsc_type               type,
                               rsc_profile_unit       unitData[RSC_PROFILE_NUMBER_OF_UNITS]);
#endif /* _RSC_H_ */


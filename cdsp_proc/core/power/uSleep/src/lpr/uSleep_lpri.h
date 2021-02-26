#ifndef _USLEEP_LPRI_H_
#define _USLEEP_LPRI_H_
/*==============================================================================
  FILE:         uSleep_lpri.h

  OVERVIEW:     Types & definitions for uSleep supported low power modes

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/lpr/uSleep_lpri.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "uSleep.h"

/*==============================================================================
                              TYPES & MACROS
 =============================================================================*/
/* Maximum number of dynamic LPR function registrations */
#define MAX_DYN_LPR_SUPPORTED   2

/* LPR function types */
typedef void (*uSleep_LPR_enter_func)(uint64 wakeupTime, boolean reEntrant);
typedef void (*uSleep_LPR_exit_func)(void);
typedef void (*uSleep_LPR_init_func)(void);

typedef struct uSleep_lpr_latency_s
{
  uint32  enter;
  uint32  exit;
}uSleep_lpr_latency;

typedef struct uSleep_lpr_callback_s
{
  char          *name;      /* Name of callback. (uses default if NULL) */
  uSleep_lpr_cb func;       /* Callback function */
  void          *userData;  /* Any persistant user supplied data 
                             * Data is not coped and must be availiable 
                             * in island */
}uSleep_lpr_callback;

typedef struct uSleep_dynamic_lpr_s
{
  uSleep_lpr_callback enter[MAX_DYN_LPR_SUPPORTED];
  uSleep_lpr_callback exit[MAX_DYN_LPR_SUPPORTED];
}uSleep_dynamic_lpr;

/* LPR type for uSleep */
typedef struct uSleep_LPR_s
{
  char                  *name;              /* Name of LPRM */
  uSleep_lpr_latency    seed_latency;       /* Enter + exit latency in ticks of LPRM */

  uint32                backoff;            /* Backoff latency in ticks of LPRM */
  uSleep_LPR_init_func  init_func;          /* Setup function that is called on transition */
  uSleep_LPR_enter_func enter_func;         /* Functions to call for LPRM */
  uSleep_LPR_exit_func  exit_func;
  boolean               cached;             /* Flag to indicate if mode is cacheable */
  uint32                enabled_flags;      /* Flags to indicate if mode is enabled */

  /* Populated at runtime by uSleep code */
  uSleep_lpr_latency    working_latency;    /* Latency values actually used. These may be modified 
                                             * at runtime by client API's */
#ifdef DYNAMIC_LPR_CALLBACKS
  uSleep_dynamic_lpr    registered_lprs;    /* Linked list of dynamically registered LPR callbacks */
#endif
}uSleep_LPR;

/* Mask bits to define user and internaly enabled options */
#define USLEEP_LPR_ENABLE_MASK            0x01  /* User enabled mask */
#define USLEEP_LPR_INTERNAL_ENABLE_MASK   0x50  /* Internal enable mask used to override user */

/* LPR is default fully enabled */
#define USLEEP_LPR_ENABLED        (USLEEP_LPR_ENABLE_MASK | USLEEP_LPR_INTERNAL_ENABLE_MASK)

/* LPR is default user disabled and allowed to be user enabled */
#define USLEEP_LPR_DISABLED       USLEEP_LPR_INTERNAL_ENABLE_MASK

/* LPR is internally disabled and never allowed to enable at any point */
#define USLEEP_LPR_INT_DISABLED   0x00

/*==============================================================================
                             EXTERNAL VARIABLES
 =============================================================================*/
extern uSleep_LPR   g_uSleepLPR[USLEEP_NUMBER_LOW_POWER_MODES];
extern uSleep_LPR   g_uSleepDefaultLPR;

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * uSleepLPR_init
 * 
 * @brief This function is called while in normal operational mode and executes 
 *        each low power mode's init function on the transition into island
 *        mode.
 */
void uSleepLPR_init(void);

/** 
 * uSleepLPR_runLPRFunctions
 * 
 * @brief Runs the currently selected low power mode enter or exit function 
 *  
 * @param enterFunction: TRUE   -> Run the currently selected LPRM enter 
 *                                 function
 *                       FALSE  -> Run the currently selected LPRM exit function
 *  
 * @param wakeupDeadline: Value (in absolute ticks) of the backoff adjusted 
 *                        desired wakeup time. 
 *                        If exit function is selected, this value is a don't
 *                        care.
 * @param reEntrant:      Flag to indicate if the mode is being entered after it's initial 
 *                        selection. 
 * @param mode:           LPR to enter or exit 
 */
void uSleepLPR_runLPRFunctions(boolean    enterFunction,
                               uint64     wakeupDeadline,
                               boolean    reEntrant,
                               uSleep_LPR *mode);

/** 
 * uSleepLPR_setModeStatus
 * 
 * @brief Enables or disables a power mode
 *  
 * @param index:  enum index value of the desired mode
 * @param enable: Boolean value to enable or disable the mode
 */
void uSleepLPR_setModeStatus(uSleep_mode_choices index, boolean enable);

#endif /* _USLEEP_LPRI_H_ */


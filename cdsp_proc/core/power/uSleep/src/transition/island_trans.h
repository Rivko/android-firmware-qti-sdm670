#ifndef _ISLAND_TRANS_H_
#define _ISLAND_TRANS_H_
/*==============================================================================
  FILE:         island_trans.h

  OVERVIEW:     Types & prototypes for internal island transition functions

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/transition/island_trans.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                            DEFINITIONS & TYPES
 =============================================================================*/
/* Transtion interrupts */
#define ISLAND_ENTRY_IRQ   29 /* enter_island_mode_irq */
#define ISLAND_EXIT_IRQ    11 /* q6ss_spm_trig_done_irq[1] */

/*==============================================================================
                                  TYPES
 =============================================================================*/
/* Index into the spm_trig_cfgn register that holds the index into the
 * SPM sequence to run when SW triggers it. */
typedef enum
{
  ISLAND_SW_TRIGGER_ENTER = 0,
  ISLAND_SW_TRIGGER_EXIT,
  
  ISLAND_SW_TRIGGER_LAST
}island_soft_trigger;

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * islandTrans_initialize
 *
 * @brief Performs any setup required to trigger island entry
 */ 
void islandTrans_initialize(void);

/**
 * islandTrans_triggerTransition
 *
 * @brief Triggers hardware to begin the island enter or exit process 
 *  
 * @param triggerType: Enter or exit option
 */
void islandTrans_triggerTransition(island_soft_trigger triggerType);

/**
 * islandTrans_enableDebug
 *
 * @brief Enables and disables the ability to debug in island
 *  
 * @param enable: Flag to enable or disable
 */
void islandTrans_enableDebug(boolean enable);

#endif /* _ISLAND_TRANS_H_ */


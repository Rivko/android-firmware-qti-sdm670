#ifndef __PDC_GLOBAL_HAL_H__
#define __PDC_GLOBAL_HAL_H__
/*===========================================================================
                             pdc_global_hal.h

DESCRIPTION:
  This file defines HAL interface for the PDC global section.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Type Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------
#include "comdef.h"

//--------------------------------------------------------------------------
// Constant /Define Declarations 
//--------------------------------------------------------------------------
/*
 * Number of IRQs from each PDC to AOP. 
 * This is expected to be common but if changes, we can move it to
 * target/scons file to be target specific.
 */
#define PDC_GLOBAL_HAL_AOP_IRQ_PER_PDC 2

//===========================================================================
//                       Function Declarations
//===========================================================================
/**
 * <!-- pdc_global_hal_aop_pending_irq -->
 *
 * @brief Returns the information of next pending interrupt from PDC sequencer 
 *        to AOP.
 *
 * @param pdc_id: PDC id AOP is resuming.
 * @param irq_num: IRQ number from the PDC.
 *
 * @return TRUE if an interrupt is pending (pdc_id, irq_num are valid)
 *         FALSE otherwise (pdc_id and irq_num are not valid)
 */
boolean pdc_global_hal_aop_pending_irq (uint32 *pdc_id, uint32 *irq_num);

/**
 * <!-- pdc_global_hal_aop_seq_resume_once -->
 *
 * @brief Function to signal from AOP to PDC sequencer to resume for a given
 *        interrupt ONCE.
 *
 * @param pdc_id: PDC id AOP is resuming.
 * @param irq_num: IRQ number from the PDC.
 */
void pdc_global_hal_aop_seq_resume_once (uint32 pdc_id, uint32 irq_num);

/**
 * <!-- pdc_global_hal_aop_seq_resume -->
 *
 * @brief Function to signal from AOP to PDC sequencer to resume for a given
 *        interrupt. 
 *
 * @param pdc_id: PDC id AOP is resuming.
 * @param irq_num: IRQ number from the PDC.
 */
void pdc_global_hal_aop_seq_resume (uint32 pdc_id, uint32 irq_num);

/**
 * <!-- pdc_global_hal_aop_seq_resume_clear -->
 *
 * @brief Clears the continuous resume signal from AOP to a given PDC 
 *        sequencer.
 *
 * @param pdc_id: PDC id AOP is resuming.
 * @param irq_num: IRQ number from the PDC.
 */
void pdc_global_hal_aop_seq_resume_clear (uint32 pdc_id, uint32 irq_num);

/**
 * <!-- pdc_global_hal_aop_irq_clear -->
 *
 * @brief Function to clear an interrupt from PDC sequencer to AOP
 *
 * SWI and HPG description is a bit ambigous and does not say if it is
 * PDC to AOP or AOP to PDC interrupt (resume signal).
 *
 * @param pdc_id: PDC id AOP is clearing an IRQ for.
 * @param irq_num: IRQ number from the PDC.
 */
void pdc_global_hal_aop_irq_clear (uint32 pdc_id, uint32 irq_num);

#endif  //__PDC_GLOBAL_HAL_H__

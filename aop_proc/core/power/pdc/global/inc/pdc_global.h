#ifndef __PDC_GLOBAL_H__
#define __PDC_GLOBAL_H__
/*===========================================================================
                              pdc_global.h

DESCRIPTION:
  This file defines public interface for PDC global section.

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

//---------------------------------------------------------------------------
//  Type Declarations
//---------------------------------------------------------------------------
/* Enum to indicate PDC ordering for global section. 
 * Note: Keep it in the common include file for now but if this changes, 
 * we will have to move it to target specific header file. */
typedef enum 
{
  PDC_GLOBAL_APPS = 0,
  PDC_GLOBAL_SP = 1,
  PDC_GLOBAL_AUDIO = 2,
  PDC_GLOBAL_SENSORS = 3,
  PDC_GLOBAL_AOP = 4,
  PDC_GLOBAL_DEBUG = 5,
  PDC_GLOBAL_GPU = 6,
  PDC_GLOBAL_DISPLAY = 7,
  PDC_GLOBAL_MODEM = 8,
  PDC_GLOBAL_COMPUTE = 9,
  PDC_GLOBAL_MAX_ID = PDC_GLOBAL_COMPUTE,
  PDC_GLOBAL_MAX_COUNT = 0x7FFFFFFF       /* Forcing 32 bit value */
} pdc_global_pdc_id_t;

/* Structure containing PDC to AOP interrupt information */
typedef struct pdc_global_aop_irq_s
{
  uint32 pdc_id;
  uint32 irq_num;                 /* IRQ number for the PDC */
} pdc_global_aop_irq_t;


//===========================================================================
//                       Function Declarations
//===========================================================================
/**
 * <!-- pdc_global_init -->
 *
 * @brief Initializes pdc global section and prepares for interrupt handling.
 */
void pdc_global_init (void);

/**
 * <!-- pdc_global_aop_pending_irq -->
 *
 * @brief Returns the information of next pending interrupt from PDC sequencer 
 *        to AOP.
 *
 * @param irq: Pointer to structure that will be filled with information 
 *             about pending IRQ. Expected to be non-null.
 *
 * @return FALSE if no interrupts from PDC sequencer is pending.
 *         TRUE if an interrupt is pending (information will be in @param irq)
 */
boolean pdc_global_aop_pending_irq (pdc_global_aop_irq_t *irq);

/**
 * <!-- pdc_global_aop_seq_resume_once -->
 *
 * @brief Function to signal from AOP to PDC sequencer to resume for a given
 *        interrupt ONCE.
 *
 * @param irq: PDC sequencer interrupt for which AOP is sending a resume 
 *             signal (obtained by querying pending interrupt).
 *
 * @return TRUE if AOP can send resume signal successfully
 *         FALSE otherwise (e.g. invalid inputs)
 */
boolean pdc_global_aop_seq_resume_once (const pdc_global_aop_irq_t *irq);

/**
 * <!-- pdc_global_aop_seq_resume -->
 *
 * @brief Function to signal from AOP to PDC sequencer to resume for a given
 *        interrupt. 
 *
 * Unlike resume_once variant, this will unblock PDC sequencer for all 
 * subsequent occurance of same interrupt. 
 *
 * @param irq: PDC sequencer interrupt for which AOP is sending a resume 
 *             signal (obtained by querying pending interrupt).
 *
 * @return TRUE if AOP can send resume signal successfully
 *         FALSE otherwise (e.g. invalid inputs)
 */
boolean pdc_global_aop_seq_resume (const pdc_global_aop_irq_t *irq);

/**
 * <!-- pdc_global_aop_seq_resume_clear -->
 *
 * @brief Clears the continuous resume signal from AOP to a given PDC 
 *        sequencer.
 *
 * @param irq: PDC sequencer interrupt for which AOP is sending a resume 
 *             signal (obtained by querying pending interrupt).
 */
void pdc_global_aop_seq_resume_clear (const pdc_global_aop_irq_t *irq);

/**
 * <!-- pdc_global_aop_irq_clear -->
 *
 * @brief Function to clear an interrupt from PDC sequencer to AOP
 *
 * SWI and HPG description is a bit ambigous and does not say if it is
 * PDC to AOP or AOP to PDC interrupt (resume signal).
 *
 * @param irq: PDC sequencer interrupt which AOP is clearing (or for which
 *             AOP is clearing resume signal). Obtained by querying
 *             pending interrupt.
 */
void pdc_global_aop_irq_clear (const pdc_global_aop_irq_t *irq);

#endif // __PDC_GLOBAL_H__

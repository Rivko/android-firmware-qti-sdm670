/*===========================================================================
                              pdc_global.c

DESCRIPTION:
  This file implements the public interface provided by PDC global wrapper

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "comdef.h"
#include "rex.h"
#include "rinit.h"
#include "CoreVerify.h"
#include "pdc_global.h"
#include "pdc_global_hal.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
/* Un/Lock macros in case we need variants */
#define PDC_AOP_IRQ_LOCK(lock) rex_enter_crit_sect(lock)
#define PDC_AOP_IRQ_UNLOCK(lock) rex_leave_crit_sect(lock)

//---------------------------------------------------------------------------
// Static variable defintions
//---------------------------------------------------------------------------
/* Lock for handling interrupts from PDC sequencer to AOP */
static rex_crit_sect_type _aop_irq_lock;
static rex_crit_sect_type * const aop_irq_lock = &_aop_irq_lock;

//===========================================================================
//                       Function Defintions
//===========================================================================

//---------------------------------------------------------------------------
// Public Function Definitions
//---------------------------------------------------------------------------
/*
 * pdc_global_init
 */
void pdc_global_init (void)
{
  /* Initialize the global PDC section */
  int ret_val = rinit_module_init ("pdc_global");
  CORE_VERIFY (ret_val == RINIT_SUCCESS);

  /* Local initialization */
  rex_init_crit_sect (aop_irq_lock);
}

/*
 * pdc_global_aop_pending_irq
 */
boolean pdc_global_aop_pending_irq (pdc_global_aop_irq_t *irq)
{
  uint32 pdc_id, irq_num;
  boolean irq_pending = pdc_global_hal_aop_pending_irq (&pdc_id, &irq_num);
  if ((irq_pending == TRUE) && (irq != NULL))
  {
    irq->pdc_id = pdc_id;
    irq->irq_num = irq_num;
  }

  return irq_pending;
}

/*
 * pdc_global_aop_seq_resume_once
 */
boolean pdc_global_aop_seq_resume_once (const pdc_global_aop_irq_t *irq)
{
  if ((NULL == irq) || (irq->pdc_id > PDC_GLOBAL_MAX_ID) ||
      (irq->irq_num >= PDC_GLOBAL_HAL_AOP_IRQ_PER_PDC))
  {
    return FALSE;       /* Invalid parameters */
  }

  PDC_AOP_IRQ_LOCK(aop_irq_lock);
  pdc_global_hal_aop_seq_resume_once (irq->pdc_id, irq->irq_num);
  PDC_AOP_IRQ_UNLOCK(aop_irq_lock);

  return TRUE;
}

/*
 * pdc_global_aop_seq_resume
 */
boolean pdc_global_aop_seq_resume (const pdc_global_aop_irq_t *irq)
{
  if ((NULL == irq) || (irq->pdc_id > PDC_GLOBAL_MAX_ID) ||
      (irq->irq_num >= PDC_GLOBAL_HAL_AOP_IRQ_PER_PDC))
  {
    return FALSE;       /* Invalid parameters */
  }

  PDC_AOP_IRQ_LOCK(aop_irq_lock);
  pdc_global_hal_aop_seq_resume (irq->pdc_id, irq->irq_num);
  PDC_AOP_IRQ_UNLOCK(aop_irq_lock);

  return TRUE;
}

/*
 * pdc_global_aop_seq_resume_clear
 */
void pdc_global_aop_seq_resume_clear (const pdc_global_aop_irq_t *irq)
{
  if ((NULL == irq) || (irq->pdc_id > PDC_GLOBAL_MAX_ID) ||
      (irq->irq_num >= PDC_GLOBAL_HAL_AOP_IRQ_PER_PDC))
  {
    return;
  }

  PDC_AOP_IRQ_LOCK (aop_irq_lock);
  pdc_global_hal_aop_seq_resume_clear (irq->pdc_id, irq->irq_num);
  PDC_AOP_IRQ_UNLOCK (aop_irq_lock);
}

/*
 * pdc_global_aop_irq_clear
 */
void pdc_global_aop_irq_clear (const pdc_global_aop_irq_t *irq)
{
  if ((NULL == irq) || (irq->pdc_id > PDC_GLOBAL_MAX_ID) ||
      (irq->irq_num >= PDC_GLOBAL_HAL_AOP_IRQ_PER_PDC))
  {
    return;       /* Invalid parameters */
  }

  PDC_AOP_IRQ_LOCK(aop_irq_lock);
  pdc_global_hal_aop_irq_clear (irq->pdc_id, irq->irq_num);
  PDC_AOP_IRQ_UNLOCK(aop_irq_lock);
}

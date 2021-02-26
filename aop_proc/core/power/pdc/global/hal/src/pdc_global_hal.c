/*===========================================================================
                            pdc_global_hal.c

DESCRIPTION:
  This file implements the HAL interface for PDC global area.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                       Includes and Defines
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "HALhwio.h"
#include "pdc_global_hal.h"
#include "pdc_global_hwio.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
/* Gives IRQ position based on PDC id and irq number */
#define PDC_GLOBAL_HAL_AOP_IRQ_POS(pdc_id, irq_num) \
        (((pdc_id) * PDC_GLOBAL_HAL_AOP_IRQ_PER_PDC) + (irq_num))

//===========================================================================
//                       Function Defintions
//===========================================================================

//---------------------------------------------------------------------------
// Public Function Definitions
//---------------------------------------------------------------------------

/*
 * pdc_global_hal_aop_pending_irq
 */
boolean pdc_global_hal_aop_pending_irq (uint32 *pdc_id, uint32 *irq_num)
{
  boolean ret_val = FALSE;
  uint32 irq_sts = HWIO_IN (RPMH_PDC_AOP_IRQ_STATUS_BANK0);

  if (irq_sts != 0)
  {
    uint32 irq_pos = 31 - __clz (irq_sts);
    if (pdc_id != NULL)
    {
      *pdc_id = irq_sts / PDC_GLOBAL_HAL_AOP_IRQ_PER_PDC;
    }
    if (irq_num != NULL)
    {
      *irq_num = irq_sts % PDC_GLOBAL_HAL_AOP_IRQ_PER_PDC;
    }

    ret_val = TRUE;
  }
  return ret_val;
}

/*
 * pdc_global_hal_aop_seq_resume_once 
 */
void pdc_global_hal_aop_seq_resume_once (uint32 pdc_id, uint32 irq_num)
{
  uint32 irq_pos = PDC_GLOBAL_HAL_AOP_IRQ_POS (pdc_id, irq_num);
  uint32 reg_val = HWIO_IN (RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0);
  reg_val |= (1 << irq_pos);
  HWIO_OUT (RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0, reg_val);
}

/*
 * pdc_global_hal_aop_seq_resume
 */
void pdc_global_hal_aop_seq_resume (uint32 pdc_id, uint32 irq_num)
{
  uint32 irq_pos = PDC_GLOBAL_HAL_AOP_IRQ_POS (pdc_id, irq_num);
  uint32 reg_val = HWIO_IN (RPMH_PDC_AOP_SEQ_RESUME_BANK0);
  reg_val |= (1 << irq_pos);
  HWIO_OUT (RPMH_PDC_AOP_SEQ_RESUME_BANK0, reg_val);
}

/*
 * pdc_global_hal_aop_seq_resume_clear
 */
void pdc_global_hal_aop_seq_resume_clear (uint32 pdc_id, uint32 irq_num)
{
  uint32 irq_pos = PDC_GLOBAL_HAL_AOP_IRQ_POS (pdc_id, irq_num);
  uint32 reg_val = HWIO_IN (RPMH_PDC_AOP_SEQ_RESUME_BANK0);
  reg_val &= ~(1 << irq_pos);
  HWIO_OUT (RPMH_PDC_AOP_SEQ_RESUME_BANK0, reg_val);
}

/*
 * pdc_global_hal_aop_irq_clear
 */
void pdc_global_hal_aop_irq_clear (uint32 pdc_id, uint32 irq_num)
{
  uint32 irq_pos = PDC_GLOBAL_HAL_AOP_IRQ_POS (pdc_id, irq_num);
  uint32 reg_val = HWIO_IN (RPMH_PDC_AOP_IRQ_CLEAR_BANK0);  /* Redundant ? */
  uint32 set = reg_val | (1 << irq_pos);
  uint32 clear = reg_val & (~(1 << irq_pos));

  HWIO_OUT (RPMH_PDC_AOP_IRQ_CLEAR_BANK0, set);
  HWIO_OUT (RPMH_PDC_AOP_IRQ_CLEAR_BANK0, clear);
}

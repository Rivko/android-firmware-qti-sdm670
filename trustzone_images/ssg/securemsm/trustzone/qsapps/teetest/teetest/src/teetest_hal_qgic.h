
/*===========================================================================
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/

#ifndef __TEE_HAL_QGIC_H
#define __TEE_HAL_QGIC_H

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stddef.h>
#include <stdint.h>
#include "teetest.h"
#include "teetest_hal_hwio.h"


/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */


/*
 * HAL_qgic_InterruptType
 *
 * Type definitions for QGIC interrupts.  This is a 32-bit number with
 * the following interpretation:
 *
 * 0-15:    SGI (software generated interrupts)
 * 16-31:   PPI (private peripheral interrupts)
 * 32-1021: SPI (shared peripheral interrupts)
 */
typedef uint32_t HAL_qgic_InterruptType;


/*
 * HAL_QGIC_IRQ_*
 *
 * The macros are used to check the return value when acknowledging or
 * checking the highest priority interrupt.
 *
 *  HAL_QGIC_IRQ_SECURE_NA: Read is in secure mode, and only non-secure
 *                          interrupts are pending.
 *  HAL_QGIC_IRQ_NA:        No interrupts are pending.
 *  HAL_QGIC_IRQ_MAX:       Maximum number of interrupts.
 */
#define HAL_QGIC_IRQ_MAX         (1020)
#define HAL_QGIC_IRQ_SECURE_NA   (1022)
#define HAL_QGIC_IRQ_NA          (1023)


/*
 * HAL_qgic_PriorityType
 *
 * Type definitions for QGIC interrupt priority.  This is a 32-bit
 * number with a range from 0 (highest priority) to 255 (lowest priority).
 */
typedef uint32_t HAL_qgic_PriorityType;


/*
 * HAL_QGIC_PRIORITY_*
 *
 * Macros to show the maximum and minimum priority levels supported by
 * the QGIC.
 *
 *  HAL_QGIC_PRIORITY_HIGHEST:    Highest priority level.
 *  HAL_QGIC_PRIORITY_LOWEST:     Lowest priority level.
 *  HAL_QGIC_PRIORITY_NULL:       Priority level that effectively disables
 *                                the interrupt.
 */
#define HAL_QGIC_PRIORITY_HIGHEST     (0)
#define HAL_QGIC_PRIORITY_LOWEST      (254)
#define HAL_QGIC_PRIORITY_NULL        (255)


/*
 * HAL_qgic_CPUIdType
 *
 * Type definitions for a QGIC CPU identifier.  This is a 32-bit number
 * indexing the CPU.
 */
typedef uint32_t HAL_qgic_CPUIdType;


/*
 * HAL_qgic_TriggerType
 *
 * Enumeration of possible trigger types for an interrupt.
 *
 * HAL_QGIC_TRIGGER_LEVEL:   Interrupt is level sensitive.
 * HAL_QGIC_TRIGGER_EDGE:    Interrupt is edge sensitive.
 */
typedef enum
{
  HAL_QGIC_TRIGGER_LEVEL = 0,
  HAL_QGIC_TRIGGER_EDGE  = 1,

  HAL_ENUM_32BITS(QGIC_TRIGGER)
} HAL_qgic_TriggerType;


/*
 * HAL_qgic_SecureType
 *
 * The security type for an interrupt, either secure or not.
 */
typedef enum
{
  HAL_QGIC_NON_SECURE,
  HAL_QGIC_SECURE,

  HAL_ENUM_32BITS(QGIC_SECURE)
} HAL_qgic_SecureType;


/*---------------------------------------------------------------------------
 *
 *
 * HAL_GQIC_TARGET_x
 *
 * Macros to define a set of targets for an SGI.  Used in HAL_qgic_Trigger.
 *
 *  HAL_QGIC_TARGET_CPU(x): Target the given CPU (0-7)
 *  HAL_GQIC_TARGET_OTHERS: Target all CPUs but the local one.
 *  HAL_GQIC_TARGET_SELF:   Target only the current CPU.
 ---------------------------------------------------------------------------*/
#define HAL_QGIC_TARGET_CPU(x)  (1 << (x))
#define HAL_QGIC_TARGET_OTHERS  (0x10000000)
#define HAL_QGIC_TARGET_SELF    (0x20000000)
#define HAL_QGIC_TARGET_ALL     (HAL_QGIC_TARGET_OTHERS | HAL_QGIC_TARGET_SELF)

/*---------------------------------------------------------------------------
 * Macros to check if an interrupt is SGI, PPI or SPI.
 ---------------------------------------------------------------------------*/
#define HAL_QGIC_IS_SGI(nInt)  ((nInt) <= 15)
#define HAL_QGIC_IS_PPI(nInt)  ((nInt) >= 16 && (nInt) <= 31)
#define HAL_QGIC_IS_SPI(nInt)  ((nInt) >= 32 && (nInt) <= 1020)

/*---------------------------------------------------------------------------
 * Macros to find a register address and field mask from an interrupt
 * number.  This is designed for the register arrays from [0..8].
 ---------------------------------------------------------------------------*/
#define HAL_QGIC_INT2ADDR(addr, nInt)  HWIO_ADDRI(addr, ((nInt) >> 5))
#define HAL_QGIC_INT2MASK(nInt)        (1 << ((nInt) & 0x1F))

/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */
void HAL_qgic_Generate           ( HAL_qgic_InterruptType nInterrupt, HAL_qgic_SecureType eSecure, uint32_t nTargets);
#endif  /* __TEE_HAL_QGIC_H */

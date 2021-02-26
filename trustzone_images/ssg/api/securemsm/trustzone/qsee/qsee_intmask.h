#ifndef QSEE_INTMASK_H
#define QSEE_INTMASK_H

/**
   @file qsee_intmask.h
   @brief Provide APIs to mask and unmap various interrupt sources
*/

/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_intmask.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
09/22/15   pre      Initial version.

===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/** Interrupt mask request succeeded */
#define QSEE_INTMASK_SUCCESS                          0
/** MINK IPC Transport Error */
#define QSEE_INTMASK_ERROR                           -1
/** Returned when a value other than those defined by the
    qsee_intsrc_t enum is passed as a paramter.  Also returned if
    the caller runs in AArch32 and QSEE_DEBUG bit is set. */
#define QSEE_INTMASK_ERROR_UNSUPPORTED_INTERRUPT_BIT -2
/** Returned when a request to unmask IRQs may not be honored due to
    it resulting in system instability due to QSEE currently handling
    an atomic service request. */
#define QSEE_INTMASK_ERROR_MAY_NOT_UNMASK_IRQ        -3

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/**
 * Bitmask definition for supported interrupt sources.
 */
typedef enum
{
  QSEE_FIQ    = 1, /**< Secure interrupts. */
  QSEE_IRQ    = 2, /**< Non-secure interrupts. */
  QSEE_SERROR = 4, /**< SError for AA64 and external aborts for AA32. */
  QSEE_DEBUG  = 8, /**< Debug Exceptions (AA64 only). */
  QSEE_INTSRC_ENUM = 0x1FFFF,
} qsee_intsrc_t;

/**
 * @brief      Disable all supported interrupt sources.
 *
 * @param[out] restore_mask The bit mask of masked interrupt sources
 *                          prior to calling this API.  This may be
 *                          used to restore the interrupt mask state
 *                          after calling this API by passing the
 *                          value to qsee_set_intmask().
 *
 * @warning Interrupt state is not preserved when a syscall goes to
 *          HLOS.
 *
 * @return QSEE_INTMASK_SUCCESS on success
 * @return QSEE_INTMASK_ERROR on failure in minkIPC
 */
int qsee_disable_all_interrupts(qsee_intsrc_t* restore_mask);

/**
 * @brief      Masks and unmasks interrupt sources as directed by
 *             the input.
 *
 * @param[in] int_mask A bit mask of interrupts to enable and disable.
 *                     A value of one indicates the interrupt source
 *                     should be masked.  A value of zero indicates
 *                     the interrupt source should be unmasked.
 *
 * @return QSEE_INTMASK_SUCCESS on success
 * @return QSEE_INTMASK_ERROR on failure in minkIPC
 * @return QSEE_INTMASK_ERROR_UNSUPPORTED_INTERRUPT_BIT on failure due
 * to an unsupported bit pasked in the int_mask parameter
 * @return QSEE_INTMASK_ERROR_MAY_NOT_UNMASK_IRQ on failure due to a
 * request to unmask IRQs when the HLOS has requested an atomic call
 */
int qsee_set_intmask(qsee_intsrc_t int_mask);

/**
 * @brief      Returns the masking status of all supported
 *             interrupt sources.
 *
 * @param[out] int_mask A bit mask of currently masked interrupt
 *                      sources.  A value of one indicates the
 *                      interrupt source is currently masked.
 *
 * @warning Interrupt state is not preserved when a syscall goes to
 *          HLOS.
 *
 * @return QSEE_INTMASK_SUCCESS on success
 * @return QSEE_INTMASK_ERROR on failure in minkIPC
 */
int qsee_get_intmask(qsee_intsrc_t* int_mask);

#endif /* QSEE_INTMASK_H */

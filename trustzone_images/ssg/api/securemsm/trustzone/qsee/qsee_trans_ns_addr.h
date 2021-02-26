#ifndef QSEE_TRANS_NS_ADDR_H
#define QSEE_TRANS_NS_ADDR_H

/**
   @file qsee_trans_ns_addr.h
   @brief Provide APIs to translate a non-secure virtual address to a
          physical address.
*/

/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_trans_ns_addr.h#1 $
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
/** Fault-free translation */
#define QSEE_TRANS_NS_ADDR_SUCCESS                             0
/** MINK IPC Transport Error */
#define QSEE_TRANS_NS_ADDR_ERROR                              -1
/** Returned when an unsupported translation type is passed to
    getPA() */
#define QSEE_TRANS_NS_ADDR_ERROR_UNSUPPORTED_TRANSLATION_TYPE -2
/** Returned when the result of a translation is a translation
    fault. */
#define QSEE_TRANS_NS_ADDR_ERROR_TRANSLATION_FAULT            -3

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
/**
 * Methods of translation supported by ITransAddr
 */
typedef enum
{
  QSEE_EL1_READ  = 0, /**< Translate as NS EL1 read instruction. */
  QSEE_EL1_WRITE = 1, /**< Translate as NS EL1 write instruction. */
  QSEE_EL0_READ  = 2, /**< Translate as NS EL0 read instruction. */
  QSEE_EL0_WRITE = 3, /**< Translate as NS EL0 write instruction. */
  QSEE_TRANSM_ENUM = 0x1FFFF,
} qsee_transm_t;

/**
 * Translate a non-secure virtual address to physical address.
 *
 * param[in]  tm This describes what processor state should attempt
 *               the translation.  This will affect translation
 *               outcome depending on memory access permission
 *               properties.
 * param[in]  va The virtual address to translate.
 * param[out] pa The physical address result of the translation.
 *
 * @note This API will truncate addresses to the page boundary.  The
 *       PA result will also be an address on the page boundary.
 *
 * @warning Only EL1 translation methods are supported by the API.
 * @warning This API only translates the given VA as NS EL1, it does
 *          _not_ confirm the resulting PA is accessible by NS EL1.
 *
 * @return E_SUCCESS on success
 * @return QSEE_TRANS_NS_ADDR_ERROR on error generated from MINK IPC
 * @return QSEE_TRANS_NS_ADDR_ERROR_UNSUPPORTED_TRANSLATION_TYPE on
 *         error when EL0 translation type is requested
 * @return QSEE_TRANS_NS_ADDR_ERROR_TRANSLATION_FAULT when the result
 *         of the requested translation is a translation fault
 */
int qsee_trans_ns_addr(qsee_transm_t tm, uintptr_t va, uintptr_t* pa);

#endif /* QSEE_TRANS_NS_ADDR_H */

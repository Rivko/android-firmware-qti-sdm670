/*============================================================================
@file npa_transaction.h

NPA Transaction API - STUBBED. Transaction support has been removed from NPA.

These routines are only retained for code-compatibility purposes and are NOPs.
DO NOT USE THESE IN NEW CODE

Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/api/power/npa_transaction.h#1 $
==========================================================================*/
#ifndef NPA_TRANSACTION_H
#define NPA_TRANSACTION_H

#include "npa.h"
#include "npa_resource.h"

#ifdef __cplusplus
extern "C" {
#endif

/** DEPRECATED API LINE - DO NOT CROSS FOR NEW CODE */

/** Handle for transaction */
typedef struct npa_transaction *npa_transaction_handle;


/** Transaction attributes alter the behavior of an NPA transaction */
typedef enum
{
  NPA_TRANSACTION_DEFAULT = 0,

  NPA_TRANSACTION_LAZY_LOCKING = NPA_TRANSACTION_DEFAULT,

  NPA_TRANSACTION_PESSIMISTIC_LOCKING = 0x1,
} npa_transaction_attribute;


/** Defined for already legacy reasons */
#define npa_transaction_type unsigned int


/**
 * <!-- npa_define_transaction -->
 *
 * @brief Stubbed routine. Returns a constant non-NULL pointer.
 * Transaction support has been removed from NPA.
 *
 * @return a constant non-NULL pointer.
 * @note DEPRECATED
 */
npa_transaction_handle npa_define_transaction( const char *transaction_name,
                                               unsigned int attributes,
                                               unsigned int num_resources, 
                                               const char *resources[] );


/**
 * <!-- npa_begin_transaction -->
 *
 * @brief Stubbed routine. Does nothing.
 * Transaction support has been removed from NPA.
 *
 * @note DEPRECATED
 */
void npa_begin_transaction( npa_transaction_handle transaction );


/**
 * <!-- npa_end_transaction -->
 *
 * @brief Stubbed routine. Does nothing.
 * Transaction support has been removed from NPA.
 *
 * @note DEPRECATED
 */
void npa_end_transaction( npa_transaction_handle transaction );


/**
 * <!-- npa_destroy_transaction -->
 *
 * @brief Stubbed routine. Does nothing.
 * Transaction support has been removed from NPA.
 *
 * @note DEPRECATED
 */
void npa_destroy_transaction( npa_transaction_handle transaction );


/**
 * <!-- npa_is_transaction_nested -->
 *
 * @brief Stubbed routine. Returns 0.
 * Transaction support has been removed from NPA.
 *
 * @note DEPRECATED
 */
unsigned int npa_is_transaction_nested( npa_transaction_handle transaction );


/**
 * <!-- npa_mark_resource_forked -->
 *
 * @brief Stubbed routine. Does nothing.
 * Transaction support has been removed from NPA.
 *
 * @note DEPRECATED
 */
void npa_mark_resource_forked( npa_resource *resource );


/** Stubbed. Does nothing */
void npa_fork_transaction( npa_transaction_handle transaction,
                           npa_join_function      join_func,
                           void                  *join_data );

/** Stubbed. Does nothing */
void npa_join_transaction( npa_transaction_handle transaction );

/** Stubbed. Does nothing */
void npa_set_transaction_fork_pref( npa_transaction_handle transaction,
                                    npa_fork_pref pref );

/** Stubbed. Always returns NPA_FORK_DEFAULT */
npa_fork_pref 
npa_get_transaction_fork_pref( npa_transaction_handle transaction );

#ifdef __cplusplus
}
#endif

#endif /* NPA_TRANSACTION_H */

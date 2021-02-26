/*============================================================================
@file npa_transaction.h

NPA Transaction API

Copyright (c) 2010 - 2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
All Rights Reserved.
Qualcomm Confidential and Proprietary

==========================================================================*/
#ifndef NPA_TRANSACTION_H
#define NPA_TRANSACTION_H

#include "npa.h"
#include "npa_resource.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Handle for transaction */
typedef struct npa_transaction *npa_transaction_handle;


/** Transaction attributes alter the behavior of an NPA transaction */
typedef enum
{
  /* Default. Implies lazy locking. In transactions with lazy locking,
     resources are only locked when they are accessed in the transaction.
     This minimizes the number of locks acquired and the duration they
     are acquired for to the minimum neccessary for the active transaction,
     but has the possibility of deadlock if the same resource can be
     accessed via multiple paths in the transaction.
  */
  NPA_TRANSACTION_DEFAULT = 0,

  /* Defined for backward compatibility reasons */
  NPA_TRANSACTION_LAZY_LOCKING = NPA_TRANSACTION_DEFAULT,

  /* Require pessimistic locking. All resources that are defined as part
     of the transaction and their dependencies are locked in top-sort order
     at begin transaction. This approach will hold all locks (even on
     resources that aren't required in the current instance of the
     transaction) for the maximum length of time, but will not deadlock
     if the same reosurce is part of the transaction via multiple paths.
  */
  NPA_TRANSACTION_PESSIMISTIC_LOCKING = 0x1,
} npa_transaction_attribute;


/** Defined for legacy reasons; the second argument to npa_define_transaction
    used to be an enum called npa_transaction_type */
#define npa_transaction_type unsigned int


/**
 * <!-- npa_define_transaction -->
 *
 * @brief Define a transaction with the list of resources that will be used in the transaction.
 * 
 * Creation of the transaction handle may be expensive. A transaction
 * handle can be reused for multiple transactions, although any single
 * transaction must be completed before another transaction can be
 * started on the same handle.  If the transaction is created and used
 * within the driver function of a (locked) resource, transaction
 * atomicity will be implicit.
 *
 * The resource array passed into this function must remain valid for
 * the life of the transaction structure.
 *
 * If defined with the DEFAULT or LAZY_LOCKING attribute, it is the
 * responsibility of the person defining the transaction to understand
 * the implications of leaving resources locked beyond the duration of
 * the resource request.  This typically means understanding the
 * dependencies associated with your requests and what things will remain
 * locked, and in particular avoid having a circular path to the same
 * underlying resource, as this introduces an opportunity for deadlock.
 * 
 * If a request is issued to a resource that was defined without
 * locking, the resource will not be locked, although this should not
 * be a problem as lockless resources are either side-effect free or
 * not intended to be generally accessed.
 *
 * Note: the list of resources are all the resources that may be part
 * of this transaction.  Any particular transaction does not
 * necessarily have to access all of the resources that are defined as
 * part of the transaction, but any resources that are not defined as
 * part of the transaction will not be considered part of the
 * transaction.
 *
 * @param transaction_name : name of the transaction, used for logging purposes
 * @param attributes : One or more from npa_transaction_attribute ORd together
 * @param num_resources : number of resources that in the transaction array
 * @param resources : array of resources that are part of the transaction
 *
 * @return the transaction handle for this transaction
 */
npa_transaction_handle npa_define_transaction( const char *transaction_name,
                                               unsigned int attributes,
                                               unsigned int num_resources, 
                                               const char *resources[] );


/**
 * <!-- npa_begin_transaction -->
 *
 * @brief Begin a transaction on the transaction handle defined by npa_define_transaction
 * 
 * Begin a transaction on the given handle.  All resources in the
 * transaction that have an request issued on them will remain locked
 * until npa_end_transaction() is issued.
 *
 * If a remote resource is part of the transaction and the remote
 * protocol supports transactions, the resource request will be added
 * to the remote transaction.  The remote transaction is not guaranteed
 * to be issued until npa_end_transaction. Remote transaction behavior,
 * including ordering of remote requests, is protocol specific.
 *
 * If a request is issued to a resource that was not defined as part of
 * the transaction, the resource behavior is undefined (i.e., it may or
 * may not remain locked until the end of the transaction and it may or
 * may not be included in any remote transaction).
 *
 * Only one transaction may be outstanding on a given transaction
 * handle at any particular point in time.  It is an error to attempt
 * to begin a second transaction on a handle that has an outstanding
 * transaction in process.
 *
 * @param transaction : The transaction that is beginning
 */
void npa_begin_transaction( npa_transaction_handle transaction );


/**
 * <!-- npa_end_transaction -->
 *
 * @brief End a transaction on the transaction handle defined by npa_define_transaction
 * 
 * Complete the transaction that was issued. Completing a transaction
 * will issue any remote transactions that had been initiated on the
 * transaction handle and upon completing free all resource locks that
 * were acquired by the transaction.
 *
 * When this function returns, the transaction has completed and all
 * the underlying locks acquried by the transaction will have been
 * released.
 *
 * @param transaction : The transaction that is ending
 */
void npa_end_transaction( npa_transaction_handle transaction );


/**
 * <!-- npa_destroy_transaction -->
 *
 * @brief  Destroy the transaction handle and free any internal data structures
 *         allocated for it.
 *
 * It is expected that this will not be a common operation, and that
 * typically transactions will be created when the resource is
 * initialized and are never destroyed.
 *
 * @param transaction : The transaction to be destroyed
 */
void npa_destroy_transaction( npa_transaction_handle transaction );


/**
 * <!-- npa_is_transaction_nested -->
 *
 * @brief Returns 1 if the transaction is nested under the currently active
 *        transaction or 0, if not.
 *
 * @param transaction: The transaction in question
 */
unsigned int npa_is_transaction_nested( npa_transaction_handle transaction );


/**
 * <!-- npa_mark_resource_forked -->
 *
 * @brief Marks the resource as being in a pending state, because the current
 * request to it is still outstanding.
 *
 * Resource authors must call this function if their driver functions take
 * advantage of transactions to batch requests.
 *
 * @param resource : The resource in question
 */
void npa_mark_resource_forked( npa_resource *resource );


/** DEPRECATED API LINE - DO NOT CROSS */

/**
 * <!-- npa_fork_transaction -->
 *
 * @brief same as npa_end_transaction 
 *
 * The join_func function is invoked with join_data as argument,
 * after the transaction is complete, before return from this function.
 *
 * @note DEPRECATED
 */
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
/*============================================================================
@file npa_transaction.c

NPA Transaction API - STUBBED. Transaction support has been removed from NPA.

These routines are only retained for code-compatibility purposes and are NOPs.
DO NOT USE THESE IN NEW CODE

Copyright (c) 2010-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/npa/src/framework/npa_transaction.c#1 $ 
==========================================================================*/

#include "npa_internal.h"
#include "npa_inlines.h"
#include "CoreThread.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Transactions are no longer supported in NPA. Return this stub handle
   against all calls to npa_define_transaction */
npa_transaction_handle stub_transaction = (npa_transaction_handle) 0xDEADC0DE;

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
                                               const char *resources[] )
{
  return stub_transaction;
}


/**
 * <!-- npa_destroy_transaction -->
 *
 * @brief Stubbed routine. Does nothing.
 * Transaction support has been removed from NPA.
 *
 * @note DEPRECATED
 */
void npa_destroy_transaction( npa_transaction_handle transaction )
{
  return;
}


/**
 * <!-- npa_begin_transaction -->
 *
 * @brief Stubbed routine. Does nothing.
 * Transaction support has been removed from NPA.
 *
 * @note DEPRECATED
 */
void npa_begin_transaction( npa_transaction_handle transaction )
{
  return;
}


/**
 * <!-- npa_end_transaction -->
 *
 * @brief Stubbed routine. Does nothing.
 * Transaction support has been removed from NPA.
 *
 * @note DEPRECATED
 */
void npa_end_transaction( npa_transaction_handle transaction )
{
  return;
}


/**
 * <!-- npa_mark_resource_forked -->
 *
 * @brief Stubbed routine. Does nothing.
 * Transaction support has been removed from NPA.
 *
 * @note DEPRECATED
 */
void npa_mark_resource_forked( npa_resource *resource )
{
  return;
}


/**
 * <!-- npa_is_transaction_nested -->
 *
 * @brief Stubbed routine. Returns 0.
 * Transaction support has been removed from NPA.
 *
 * @note DEPRECATED
 */
unsigned int
npa_is_transaction_nested( npa_transaction_handle transaction )
{
  return 0;
}

#ifdef __cplusplus
}
#endif

/*============================================================================
@file npa_legacy.c

Legacy functions in the NPA framework. Stubbed and maintained for SC reasons.

Copyright (c) 2009-2014 Qualcomm Technologies Inc.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

============================================================================*/

#include "npa_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/** DEPRECATED API LINE - DO NOT CROSS */

void npa_set_client_fork_pref( npa_client_handle client, npa_fork_pref pref )
{
  /* If fork/join is re-supported in the future, invoke -
     npa_set_request_attribute( client, NPA_REQUEST_FORKABLE ) here */
  return;
}
 
npa_fork_pref npa_get_client_fork_pref( npa_client_handle client )
{
  return NPA_FORK_DEFAULT;
}

npa_join_token npa_fork_resource( npa_resource *resource, 
                                  npa_join_function  join_func,
                                  void              *join_data )
{
  return NULL;
}

void npa_join_request( npa_client_handle client )
{
  ULOG_RT_PRINTF_1( npa.log, "npa_join_request (NOP) (client: 0x%p)",
                    client );
  return;
}

void npa_fork_transaction( npa_transaction_handle   transaction,
                           npa_join_function        join_func,
                           void                    *join_data )
{
  npa_end_transaction( transaction );

  join_func( transaction, join_data );

  ULOG_RT_PRINTF_2( npa.log, "npa_fork_transaction (npa_end_transaction) "
                    "(handle: 0x%p) (transaction_name: \"%s\") ",
                    transaction, 
                    transaction->name );
}

npa_join_token
npa_fork_transaction_extension( npa_transaction_handle  transaction,
                                npa_join_function       join_func,
                                void                   *join_data )
{
  return NULL;
}

void npa_join_transaction( npa_transaction_handle transaction )
{
  ULOG_RT_PRINTF_2( npa.log, "npa_join_transaction (NOP) "
                    "(handle: 0x%p) (transaction_name: \"%s\") ",
                    transaction, 
                    transaction->name );
  return;
}

void npa_set_transaction_fork_pref( npa_transaction_handle transaction,
                                    npa_fork_pref pref )
{
  return;
}

npa_fork_pref 
npa_get_transaction_fork_pref( npa_transaction_handle transaction )
{
  return NPA_FORK_DEFAULT;
}

#ifdef __cplusplus
}
#endif

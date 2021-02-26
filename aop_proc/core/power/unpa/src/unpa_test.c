/*============================================================================
@file unpa_test.c

Unit tests for the UNPA framework

Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary/GTDR

============================================================================*/

#include "unpa_internal.h"

#ifdef UNPA_TEST

static unpa_resource_state
unpa_tr1_driver_fcn( unpa_resource *resource, unpa_client *client,
                     unpa_resource_state state )
{
  return state;
}

static unpa_resource_definition unpa_tr1_def = {
  "ut/tr1",
  unpa_max_update_fcn,
  unpa_tr1_driver_fcn,
  100,
  UNPA_CLIENT_DEFAULT,
  UNPA_RESOURCE_DEFAULT,
  NULL,
};

void unpa_test_query_verify( unpa_query_handle qh, unpa_query_id id,
                             uint32_t expected )
{
  unpa_query_result qr;
  
  CORE_VERIFY( unpa_query( qh, id, &qr ) == UNPA_QUERY_SUCCESS );
  CORE_VERIFY( qr.val == expected );
}

void unpa_test_query_verify_nonzero( unpa_query_handle qh, unpa_query_id id )
{
  unpa_query_result qr;
  
  CORE_VERIFY( unpa_query( qh, id, &qr ) == UNPA_QUERY_SUCCESS );
  CORE_VERIFY( qr.val != 0 );
}

void unpa_test( void )
{
  unpa_query_handle unpa_tq1;
  unpa_query_handle unpa_tq2;
  unpa_client *unpa_tc1, *unpa_tc2, *unpa_tc3;
  
  /* Create resource */
  unpa_create_resource( &unpa_tr1_def, 0 );

  /* Add client */  
  unpa_tc1 = unpa_create_client( "unpatc1", UNPA_CLIENT_REQUIRED, "ut/tr1" );
  CORE_VERIFY_PTR( unpa_tc1 );
  
  unpa_tc2 = unpa_create_client( "unpatc2", UNPA_CLIENT_REQUIRED, "ut/tr1" );
  CORE_VERIFY_PTR( unpa_tc2 );
  
  unpa_tc3 = 
    unpa_create_client( "unpatc3", UNPA_CLIENT_SUPPRESSIBLE, "ut/tr1" );
  CORE_VERIFY_PTR( unpa_tc3 );
  
  /* Query */
  CORE_VERIFY_PTR( unpa_tq1 = unpa_get_query_handle( "ut/tr1" ) );
  
  /* Max update function */
  unpa_issue_request( &unpa_tc1, 20 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 20 );
  
  unpa_issue_request( &unpa_tc2, 10 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 20 );
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_REQUIRED_STATE, 20 );
  
  unpa_issue_request( &unpa_tc3, 30 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 30 );
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_REQUIRED_STATE, 20 );

  unpa_cancel_request( &unpa_tc3 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 20 );  
  
  unpa_cancel_request( &unpa_tc1 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 10 );
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_REQUIRED_STATE, 10 );  
  
  /* Remove client */
  unpa_remove_client( &unpa_tc2 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 0 );
  
  /* For UT; do not do this in actual use */
  unpa_tr1_def.update_fcn = unpa_min_update_fcn;
  
  /* Min update function */
  
  /* Add unpa_tc2 again */
  CORE_VERIFY_PTR( unpa_add_client( &unpa_tc2, "ut/tr1" ) );
  
  unpa_issue_request( &unpa_tc1, 20 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 20 );
  
  unpa_issue_request( &unpa_tc2, 10 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 10 );
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_REQUIRED_STATE, 10 );
  
  unpa_complete_request( &unpa_tc1 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 10 );
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_REQUIRED_STATE, 10 );
  
  unpa_remove_client( &unpa_tc2 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 0 );
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_REQUIRED_STATE, 0 );
  
  unpa_issue_request( &unpa_tc3, 10 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 10 );
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_REQUIRED_STATE, 0 );
  
  unpa_issue_request( &unpa_tc1, 5 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 5 );
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_REQUIRED_STATE, 5 );
  
  unpa_cancel_request( &unpa_tc3 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 5 );
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_REQUIRED_STATE, 5 );
  
  unpa_issue_request( &unpa_tc1, 0 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 0 );
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_REQUIRED_STATE, 0 );
  
  /* For UT; do not do this in actual use */
  unpa_tr1_def.update_fcn = unpa_binary_update_fcn;
  
  /* Binary update function */
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 0 );
  
  unpa_issue_request( &unpa_tc1, 10 );
  
  unpa_test_query_verify_nonzero( unpa_tq1, UNPA_QUERY_ACTIVE_STATE );
  
  unpa_issue_request( &unpa_tc3, 10 );
  
  unpa_test_query_verify_nonzero( unpa_tq1, UNPA_QUERY_ACTIVE_STATE );
  
  unpa_cancel_request( &unpa_tc1 );
  
  unpa_test_query_verify_nonzero( unpa_tq1, UNPA_QUERY_ACTIVE_STATE );
  
  unpa_cancel_request( &unpa_tc3 );
  
  unpa_test_query_verify( unpa_tq1, UNPA_QUERY_ACTIVE_STATE, 0 );
  
  unpa_remove_client( &unpa_tc1 );
  unpa_remove_client ( &unpa_tc3 );
  
  /* Verify that we are unable to create a client to non-existent resource */
  CORE_VERIFY( unpa_add_client( &unpa_tc1, "ut/ts1" ) == NULL );
  
  /* Define stub */
  unpa_stub_resource( "ut/ts1" );
  
  /* Client to stub */
  CORE_VERIFY_PTR( unpa_add_client( &unpa_tc1, "ut/ts1" ) );
  CORE_VERIFY_PTR( unpa_add_client( &unpa_tc3, "ut/ts1" ) );
  
  /* Query stub */
  CORE_VERIFY_PTR( unpa_tq2 = unpa_get_query_handle( "ut/ts1" ) );
  
  unpa_test_query_verify( unpa_tq2, UNPA_QUERY_ACTIVE_STATE, 0 );
  
  /* Request to stub */
  unpa_issue_request( &unpa_tc1, 100 );
  
  unpa_test_query_verify( unpa_tq2, UNPA_QUERY_ACTIVE_STATE, 0 );
  
  unpa_issue_request( &unpa_tc3, 200 );
  
  unpa_test_query_verify( unpa_tq2, UNPA_QUERY_ACTIVE_STATE, 0 );
}

#endif

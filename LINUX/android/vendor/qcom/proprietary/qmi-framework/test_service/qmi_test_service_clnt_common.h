#ifndef QMI_TEST_CLNT_COMMON_H
#define QMI_TEST_CLNT_COMMON_H
/******************************************************************************
  -----------------------------------------------------------------------------
  Copyright (c) 2012,2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "test_service_v01.h"
#include "qmi_test_service_clnt_common_stats.h"

/*Invalid Message ID for adversarial test */
#define TEST_INVALID_MSG_ID          100

/*===========================================================================
  FUNCTION  qmi_test_clnt_common_async_data_test
===========================================================================*/
/*!
@brief
  Main body of qmi_test_clnt_common_async_data_test

@return
  0 for pass, -1 for fail

@note
  - Dependencies
    - N/A
  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_async_data_test(
  qmi_client_type *clnt,
  qmi_txn_handle *txn,
  uint32_t test_length,
  uint32_t data_size_bytes,
  uint32_t test_duration_ms,
  uint32_t verbose,
  qmi_test_clnt_results_type *results);


/*===========================================================================
  FUNCTION  qmi_test_clnt_common_sync_data_test
===========================================================================*/
/*!
@brief
  Main body of qmi_test_clnt_common_sync_data_test

@return
  0 for pass, -1 for fail

@note
  - Dependencies
    - N/A
  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_sync_data_test(
  qmi_client_type *clnt,
  uint32_t test_length,
  uint32_t data_size_bytes,
  uint32_t test_duration_ms,
  uint32_t verbose,
  qmi_test_clnt_results_type *results);


/*===========================================================================
  FUNCTION  qmi_test_clnt_common_sync_raw_data_test
===========================================================================*/
/*!
@brief
  Main body of qmi_test_clnt_common_sync_raw_data_test

@return
  0 for pass, -1 for fail

@note
  - Dependencies
    - N/A
  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_sync_raw_data_test(
  qmi_client_type *clnt,
  uint32_t test_length,
  uint32_t data_size_bytes,
  uint32_t test_duration_ms,
  uint32_t verbose,
  qmi_test_clnt_results_type *results);


/*===========================================================================
  FUNCTION  qmi_test_clnt_common_async_raw_data_test
===========================================================================*/
/*!
@brief
  Main body of qmi_test_clnt_common_async_raw_data_test

@return
  0 for pass, -1 for fail

@note
  - Dependencies
    - N/A
  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_async_raw_data_test(
  qmi_client_type *clnt,
  qmi_txn_handle *txn,
  uint32_t test_length,
  uint32_t data_size_bytes,
  uint32_t test_duration_ms,
  uint32_t verbose,
  qmi_test_clnt_results_type *results);

/*===========================================================================
  FUNCTION  qmi_test_clnt_common_delete_async_txn_test
===========================================================================*/
/*!
@brief
  Main body of qmi_test_clnt_common_delete_async_txn_test

@return
  0 for pass, -1 for fail

@note
  - Dependencies
    - N/A
  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_delete_async_txn_test(
  qmi_client_type *clnt,
  qmi_txn_handle *txn,
  uint32_t test_length,
  uint32_t data_size_bytes,
  int time_out_ms);

/*===========================================================================
  FUNCTION  qmi_test_clnt_common_async_invalid_msg_test
===========================================================================*/
/*!
@brief
  Main body of qmi_test_clnt_common_async_invalid_msg_test

@return
  0 for pass, -1 for fail

@note
  - Dependencies
    - N/A
  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_async_invalid_msg_test(
  qmi_client_type *clnt,
  qmi_txn_handle *txn,
  uint32_t test_length,
  uint32_t data_size_bytes,
  int time_out_ms);

/*===========================================================================
  FUNCTION  qmi_test_clnt_common_sync_basic_test
===========================================================================*/
/*!
@brief
  Main body of qmi_test_clnt_common_sync_basic_test

@return
  0 for pass, -1 for fail

@note
  - Dependencies
    - N/A
  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_sync_basic_test(
  qmi_client_type *clnt,
  uint32_t test_length,
  uint32_t test_duration_ms,
  uint32_t verbose,
  qmi_test_clnt_results_type *results);

/*===========================================================================
  FUNCTION  qmi_test_clnt_common_ind_test
===========================================================================*/
/*!
@brief
  Main body of qmi_test_clnt_common_ind_test

@return
  0 for pass, -1 for fail

@note
  - Dependencies
    - N/A
  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_ind_test(
   uint32_t test_length,
   uint32_t data_size_bytes,
   uint32_t test_duration_ms,
   uint32_t verbose,
   qmi_test_clnt_results_type *results);

#endif /* QMI_TEST_CLNT_COMMON_H */

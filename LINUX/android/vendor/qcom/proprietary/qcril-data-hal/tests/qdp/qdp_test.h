/*!
  @file
  qdp_test.h

  @brief
  common definitions for qdp tests

*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/10   js      created file

===========================================================================*/
#include <stdlib.h>
#include <stdio.h>

#define QDP_TEST_APN_NAME "qdp-test-apn"
#define QDP_TEST_NAI "qdp_test_nai"

#define QDP_TEST_LOG(...) \
  QDP_LOG_DEBUG(__VA_ARGS__); \
  printf(__VA_ARGS__); \
  printf("\n")

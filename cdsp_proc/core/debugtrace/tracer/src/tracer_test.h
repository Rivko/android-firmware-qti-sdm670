#ifndef TRACER_TEST_H
#define TRACER_TEST_H
/*===========================================================================
  @file tracer_test.h

   Tracer tests

===========================================================================*/
/*=============================================================================
  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
 $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer_test.h#1 $
=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

#define TRACER_TEST_RVAL_SUCCESS          0  // Successfully completed.
#define TRACER_TEST_RVAL_ERROR            1  // Error occurred.                                         //
#define TRACER_TEST_RVAL_REQUEST_ERR      2  // Request error: format, param, ...
#define TRACER_TEST_RVAL_UNSUPPORTED      3  // Not supported.
//#define TRACER_TEST_RVAL_FAILED         4  // Failure occurred.


/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

int tracer_test(uint8 type, uint8 subtype,
                uint16 option, uint32 mask,
                uint32 count1, uint32 count2);

#endif /* #ifndef TRACER_TEST_H */


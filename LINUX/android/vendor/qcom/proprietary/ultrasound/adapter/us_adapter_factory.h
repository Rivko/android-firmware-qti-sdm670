/*===========================================================================
                           us_adapter_factory.h

DESCRIPTION: Provide a factory for adapters.

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifndef __US_ADAPTER_FACTORY__
#define __US_ADAPTER_FACTORY__

/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include <framework_adapter.h>

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Typedefs
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Static Variable Definitions
-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Function definitions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Function implementation
------------------------------------------------------------------------------*/
/*============================================================================
  FUNCTION:  create_adapter
============================================================================*/
/**
 * Returns the desired adapter.
 *
 * @param adapter Output parameter for the new adapter.
 * @param lib_path The adapter lib path.
 * @param adapter The gesture framework lib path.
 *
 * @return int -1 null parameter
 *              0 success
 *              1 failure
 */
int create_adapter(FrameworkAdapter **adapter,
                   char *lib_path,
                   char *gesture_fw_lib_path);

/*============================================================================
  FUNCTION:  destroy_adapter
============================================================================*/
/**
 * This function closes the dynamic library.
 */
void destroy_adapter();

#endif //__US_ADAPTER_FACTORY__

/*==============================================================================
FILE: CoreTestsLib.h
 
DESCRIPTION: the coretests module header for DALTF     
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
      Copyright (c) 2015 QUALCOMM Technologies Inc.
              All Rights Reserved.
       Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/08/11   jz      Created.

=============================================================================*/

#ifndef CORE_TESTS_LIB_H
#define CORE_TESTS_LIB_H

/*
 * List of all Init/DeInit functions to be added in DALTF
 */
#include <DDITF.h>

extern const UINTN daltf_setup_number;
extern const TF_TestFunction_Descr* daltf_setup_tests[];


#endif /* CORE_TESTS_LIB_H */

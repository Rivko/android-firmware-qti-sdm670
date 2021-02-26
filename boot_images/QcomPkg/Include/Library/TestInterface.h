/* TestInterface.c
*  This module implements the test interface of UEFI
*
* EXTERNALIZED FUNCTIONS
*
* Copyright (C) 2011 by Qualcomm Technologies, Inc.
* All Rights Reserved.
*
**********************************************************************/
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    ----------------------------------------------------------
05/19/11   sy     Initial check in.
========================================================================*/
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>

/*============================================================================
  MACRO DEFINITION
==============================================================================*/
#define TEST_START(TestCase) TestBegin(__FILE__,TestCase)
#define TEST_STOP(TestCase) TestEnd(__FILE__,TestCase)


/*==============================================================================

  FUNCTION      TestBegin 

  DESCRIPTION   Print the beginning of the test

==============================================================================*/
VOID 
EFIAPI 
TestBegin (
  CHAR8 *TestFile,
  CHAR8 *TestCase);

/*==============================================================================

  FUNCTION      TestEnd

  DESCRIPTION   Print the end of the test

==============================================================================*/
VOID 
EFIAPI TestEnd(
  CHAR8 * TestFile,
  CHAR8 * TestCase);

/*==============================================================================

  FUNCTION      TestStatus

  DESCRIPTION   Output test status : PASSED/FAILED

==============================================================================*/
VOID 
EFIAPI TestStatus(
  CHAR8 * TestCase, 
  EFI_STATUS TestSts); 


/* TestInterface.c
*  This module implements the test interface of UEFI
*
* EXTERNALIZED FUNCTIONS
*
* Copyright (c)  2011-2012 by Qualcomm Technologies, Inc.
* All Rights Reserved.
*
**********************************************************************/
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    ----------------------------------------------------------
04/10/12   jz      Fixed klockwork warnings
05/19/11   sy     Initial check in.
========================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>

/*==============================================================================

  FUNCTION      TestBegin 

  DESCRIPTION   Print the beginning of the test

==============================================================================*/
VOID  
EFIAPI 
TestBegin (
  CHAR8 * TestFile, 
  CHAR8 * TestCase)

{	
  AsciiPrint(" TestFile: %a \n ",TestFile);
  AsciiPrint("================  TEST %a  BEGIN ==============================\n ",TestCase);

}
/*==============================================================================

  FUNCTION      TestEnd

  DESCRIPTION   Print the end of the test

==============================================================================*/
VOID 
EFIAPI 
TestEnd( 
  CHAR8 *TestFile,
  CHAR8 * TestCase)
{
  AsciiPrint("================  TEST %a END ==============================\n ",TestCase );
}

/*==============================================================================

  FUNCTION      TestStatus

  DESCRIPTION   Output test status : PASSED/FAILED

==============================================================================*/
VOID 
EFIAPI
TestStatus(
  CHAR8* TestCase, 
  EFI_STATUS TestSts)
{

  if (TestSts!=0)
  {
	AsciiPrint("*****  TEST %a FAILED with ERR %d ********\n ",TestCase, TestSts);
  }
  else
  {
	AsciiPrint("***** TEST %a PASSED ******** \n ",TestCase); 
  }
}



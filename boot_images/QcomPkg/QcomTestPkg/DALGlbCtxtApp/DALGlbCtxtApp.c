/** @file DALGlbCtxtApp.c

  Test application for DAL Global Context

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 07/30/11   sy      Adding TestInterface Library
 05/11/11   sho     Creation

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIDALGlbCtxt.h>
#include "DALGlbCtxt.h"

EFI_STATUS
EFIAPI
DALGlbCtxtAppMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
   EFI_STATUS status;
   EFI_DALGLBCTXT_PROTOCOL *fp = NULL;
   DALGLB_HEADER * globalCtxt1;
   DALGLB_HEADER * globalCtxt2;

   TEST_START("DALGlbCtxtApp");
   status = gBS->LocateProtocol(&gEfiDalGlbCtxtProtocolGuid, NULL, 
         (VOID **)&fp);
   if (EFI_SUCCESS == status && NULL != fp)
   {
      AsciiPrint("Successfully located DAL global context protocol\n");
   }
   else
   {
      AsciiPrint( 
      "ERROR: Unable to locate DAL global context protocol; status=0x%x; ptr=0x%x\n",
         status, fp);
	  TestStatus("DALGlbCtxtApp",status);
 	  TEST_STOP("DALGlbCtxtApp");
      return EFI_D_ERROR;
   }

   status = fp->Alloc("GLBCTXT TEST", 100, 
         DALGLBCTXT_LOCK_TYPE_SHARED_TOKEN, (void **)&globalCtxt1);
   if (EFI_SUCCESS == status && NULL != globalCtxt1)
   {
      AsciiPrint( 
               "Successfully allocated global context; 0x%x\n",
               globalCtxt1);
   }
   else
   {
      AsciiPrint( 
      "ERROR: Unable to allocate global context; status=0x%x; buffer=0x%x\n",
               status, globalCtxt1);
	  TestStatus("DALGlbCtxtApp",status);
 	  TEST_STOP("DALGlbCtxtApp");
      return EFI_D_ERROR;
   }


   status = fp->Find("GLBCTXT TEST", (void **)&globalCtxt2);
   if (EFI_SUCCESS == status && globalCtxt2 == globalCtxt1)
   {
      AsciiPrint("Successfully found context\n", globalCtxt1);
   }
   else
   {
      AsciiPrint( 
         "ERROR: Unable to find context; status=0x%x; original buffer=0x%x; found buffer=0x%x\n",
               status, globalCtxt1, globalCtxt2);
	  TestStatus("DALGlbCtxtApp",status);
 	  TEST_STOP("DALGlbCtxtApp");
      return EFI_D_ERROR;
   }


   status = fp->LockEx(globalCtxt1, 0);
   if (EFI_SUCCESS == status)
   {
      AsciiPrint("Successfully locked context\n", globalCtxt1);
   }
   else
   {
      AsciiPrint("ERROR: Unable to lock context; status=0x%x\n", status);
	  TestStatus("DALGlbCtxtApp",status);
 	  TEST_STOP("DALGlbCtxtApp");
      return EFI_D_ERROR;
   }


   // try to lock context for 3 seconds, and this should fail
   AsciiPrint("Attempt to lock context and should timeout in 3 secs\n");

   status = fp->LockEx(globalCtxt2, 3000);
   if (EFI_SUCCESS != status)
   {
      AsciiPrint("Expectedly failed to lock context again\n");
   }
   else
   {
      AsciiPrint("ERROR: Unexpectedly able to lock context");
	  TestStatus("DALGlbCtxtApp",status);
 	  TEST_STOP("DALGlbCtxtApp");
      return EFI_D_ERROR;
   }

   status = fp->Unlock(globalCtxt2);
   if (EFI_SUCCESS == status)
   {
      AsciiPrint("Successfully unlocked context\n");
   }
   else
   {
      AsciiPrint("ERROR: Unable to unlock context; status=0x%x\n", status);
	  TestStatus("DALGlbCtxtApp",status);
 	  TEST_STOP("DALGlbCtxtApp");
      return EFI_D_ERROR;
   }


   status = fp->TryLock(globalCtxt1);
   if (EFI_SUCCESS == status)
   {
      AsciiPrint("Successfully locked context\n");
   }
   else
   {
      AsciiPrint("ERROR: Unable to lock context; status=0x%x\n", status);
	  TestStatus("DALGlbCtxtApp",status);
 	  TEST_STOP("DALGlbCtxtApp");
      return EFI_D_ERROR;
   }


   status = fp->Unlock(globalCtxt2);
   if (EFI_SUCCESS == status)
   {
      AsciiPrint("Successfully unlocked context\n");
   }
   else
   {
      AsciiPrint("ERROR: Unable to unlock context; status=0x%x\n", status);
	  TestStatus("DALGlbCtxtApp",status);
 	  TEST_STOP("DALGlbCtxtApp");
      return EFI_D_ERROR;
   }
   
   TestStatus("DALGlbCtxtApp",status);
   TEST_STOP("DALGlbCtxtApp");
   
   return DAL_SUCCESS;
}

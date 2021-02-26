/** @file DebugtraceTestApp.c

  Test application for DebugtraceDxe.

  This file contains test Debugtrace driver entry point.  On
  UEFI, there are two different ways to use Debugtrace driver:
  (1) DAL framework (2) EFI Debugtrace protocol.  This
  application shows sample code for both options.

  Copyright (c) 2015, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.xf/2.1/QcomPkg/QcomTestPkg/DebugtraceTestApp/DebugtraceTestApp.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/24/15   lht      Created.

=============================================================================*/



/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>     //gBS
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>  //TEST_START, TEST_STOP
#include <Protocol/EFIQdss.h>       //EFI_QDSS_PROTOCOL
#include "tracer_event_ids.h"

/*=========================================================================
      Functions
==========================================================================*/


/**
  Debugtrace test application entry point.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

EFI_STATUS
EFIAPI
DebugtraceTestAppMain(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_QDSS_PROTOCOL *QdssProtocolPtr = NULL;
  UINT32 SweId = 1;
  UINT32 data32;
  TEST_START("DebugtraceTest");

  Status = gBS->LocateProtocol(
                  &gEfiQdssProtocolGuid,
                  NULL,
                  (VOID **)&QdssProtocolPtr
                  );
  if (EFI_ERROR(Status)) {
    AsciiPrint("Locate Qdss Protocol failed: %d\r\n", Status);
    return Status;
  }

  Status = QdssProtocolPtr->Init();
  if (EFI_ERROR(Status)) {
    AsciiPrint("Init failed: 0x%08X\r\n", Status);
    return Status;

  } else {
    AsciiPrint("Init passed\r\n");
  }
#if 0
  Status = QdssProtocolPtr->EnableTrace();
  if (EFI_ERROR(Status)) {
    AsciiPrint("EnableTrace failed: %d\r\n", Status);
  } else {
    AsciiPrint("EnableTrace passed\r\n");
  }

#endif
  SweId = 12; //TRACER_EVENT_ALL;
  Status = QdssProtocolPtr-> EnableSwEvent(SweId);
  if (EFI_ERROR(Status)) {
    AsciiPrint("EnableSwEvent %d failed: 0x%08X\r\n", SweId, Status);
  } else {
    AsciiPrint("EnableSwEvent %d passed\r\n", SweId);
  }

  Status = QdssProtocolPtr->TraceSwEvent(TRACER_EVENT_TEST);
  if (EFI_ERROR(Status)) {
    AsciiPrint("Trace of SW event %d failed: 0x%08X\r\n",
               TRACER_EVENT_TEST, Status);
  } else {
    AsciiPrint("Trace of SW event %d passed\r\n", TRACER_EVENT_TEST);
  }

  for (SweId = 0; SweId < 14; SweId++)
  {
      Status = QdssProtocolPtr->TraceSwEvent(SweId);
  }
  AsciiPrint("Traced SW events 0 to %d\r\n", SweId);

  data32 = 0x4112;
  Status = QdssProtocolPtr->TraceSwEventVargs(TRACER_EVENT_TEST2,
                              2, data32, 0x960422);
  if (EFI_ERROR(Status)) {
    AsciiPrint("Trace of SW event data (%d) failed: 0x%08X\r\n",
               TRACER_EVENT_TEST2, Status);
  } else {
    AsciiPrint("Trace of SW event 2 args (%d) passed\r\n", TRACER_EVENT_TEST2);
  }

  Status = QdssProtocolPtr->TraceSwEventVargs(TRACER_EVENT_TEST,
                              3, 0x53534451, 0x61725420, 0x20726563);
  if (EFI_ERROR(Status)) {
    AsciiPrint("Trace of SW event data (%d) failed: 0x%08X\r\n",
               TRACER_EVENT_TEST, Status);
  } else {
    AsciiPrint("Trace of SW event 3 args (%d) passed\r\n", TRACER_EVENT_TEST);
  }

  SweId= TRACER_EVENT_ALL;
  Status = QdssProtocolPtr-> DisableSwEvent(SweId);
  if (EFI_ERROR(Status)) {
    AsciiPrint("DisableSwEvent %d failed: 0x%08X\r\n", SweId, Status);
  } else {
    AsciiPrint("DisableSwEvent %d passed\r\n", SweId);
  }

  Status = QdssProtocolPtr->DisableTrace();
  if (EFI_ERROR(Status)) {
    AsciiPrint("DisableTrace failed: 0x%08X\r\n", Status);
  } else {
    AsciiPrint("DisableTrace passed\r\n");
  }

  TestStatus("DebugtraceTest", Status);
  TEST_STOP("DebugtraceTest");
  return Status;
}


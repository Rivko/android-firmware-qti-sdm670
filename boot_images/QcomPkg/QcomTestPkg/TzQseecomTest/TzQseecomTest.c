/** @file TzQseecomTest.c
   
  Tests for using QSEECOM interface

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
  Portions Copyright (c) 2006 - 2008, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php
  
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/12/16   rj      Initial revision

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/QcomLib.h>
#include <Protocol/EFIQseecom.h>
#include <Include/scm_qsee_interface.h>

#define TEST_APP                            "keymaster"


EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status=EFI_LOAD_ERROR;
  QCOM_QSEECOM_PROTOCOL *QseeComProtocol =NULL;
  UINT32 AppId=0;
  UINTN  Argc;
  CHAR8  **Argv = NULL;
  CHAR8 app[QSEE_MAX_NAME_SIZE];
  UINT32  cmd_id = 0;
  UINT64 data = 0;

  struct qsee_send_cmd_req {
    UINT32  cmd_id;
    UINT64 data;
    UINT64 data2;
    UINT32 len;
    UINT32 start_pkt;
    UINT32 end_pkt;
    UINT32 test_buf_size;
  }cmd_req;

  struct qsee_send_cmd_rsp {
    UINT32 data;
    INT32 status;
  }cmd_rsp;

  //Act on cmd line arguments if fails continue for charge
  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);
  if (EFI_ERROR (Status)) {
    AsciiPrint ("Failed to parse commmand line\n");
    goto Exit;
  }

  switch(Argc)
  {
    case 0:
    default:
      // Use default application and commands/data.
      AsciiStrCpy(app, TEST_APP);
      cmd_id = 0x200;  //get version command for keymaster 
      break;

    case 1:
      // use commandline arguments (tailered to sample app)
      AsciiStrCpy(app, Argv[0]);
      break;

    case 2:
      // use commandline arguments (tailered to sample app)
      AsciiStrCpy(app, Argv[0]);
      cmd_id = AsciiToInt(Argv[1]);
      break;

    case 3:
     // use commandline arguments (tailered to sample app)
     AsciiStrCpy(app, Argv[0]);
     cmd_id = AsciiToInt(Argv[1]);
     data = AsciiToInt(Argv[2]);
     break;
  }
 
  TEST_START("TzQseecomTest");

  Status = gBS->LocateProtocol (&gQcomQseecomProtocolGuid, NULL, (VOID**)&QseeComProtocol);
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("Failed to Locate Qseecom Protocol status=%d\n", Status);
    goto Exit;
  }

  //AsciiPrint("app=%a, cmd_id=%d, data=%d\n", app, cmd_id, data);
  Status = QseeComProtocol->QseecomStartApp(QseeComProtocol, app, &AppId);
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("Failed QseecomStartApp(%a)  status=%d, App_id=%d\n", app, Status, AppId);
    goto Exit;
  }
  AsciiPrint("QseecomStartApp (%a) call success status=%d, AppId=%d\n", app, Status, AppId);


  cmd_req.cmd_id = cmd_id;
  cmd_req.data = data;
  cmd_req.start_pkt = 0;
  cmd_req.end_pkt = 0;
  cmd_req.test_buf_size = 0;
 
  Status = QseeComProtocol->QseecomSendCmd(QseeComProtocol, AppId, (UINT8 *)&cmd_req, sizeof(cmd_req), (UINT8 *)&cmd_rsp, sizeof(cmd_rsp));
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("Failed QseecomSendCmd (%a) status=%d\n", app, Status);
    goto Exit;
  }
  AsciiPrint("QseecomSendCmd (%a) call success status=%d, AppId=%d resp.status=%d, resp.data=%d\n", app, Status, AppId, cmd_rsp.status, cmd_rsp.data);
  
  Status = QseeComProtocol->QseecomShutdownApp(QseeComProtocol, AppId);                                     
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("Failed QseecomShutdownApp (%a) status=%d\n", app, Status);
    goto Exit;
  }
  AsciiPrint("QseecomShutdownApp (%a) call success status=%d, AppId=%d\n", app, Status, AppId);

Exit:
  TestStatus("TzQseecomTest", Status);
  TEST_STOP("TzQseecomTest");

  AsciiPrint("TzQseecomTest completed. Press any key to exit and reset the device\n");
  ReadAnyKey (NULL, 0);
  //gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
  return Status;
}

/** @file PmicMipiBifTest.c

Tests for PMICMipiBif Module

  Copyright (c) 2014, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
09/29/14   al      Updated copyright info
03/04/14   va      New File

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>

#include <Protocol/EFIPmicMipiBif.h>

#include "com_dtypes.h"
#include <Library/QcomLib.h>
#include <Library/MemoryAllocationLib.h>


#define GOTOERRORIFNULL(Str,Status,Error_Exit) if (NULL == Str){ Status = EFI_OUT_OF_RESOURCES; goto Error_Exit; }
#define GOTOERROR(Status,Error_Exit) if (EFI_SUCCESS != Status ) { goto Error_Exit; } 

static EFI_STATUS test_MipiBifAPI(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);
EFI_STATUS PmicMipiBifTest_ProcessCommand (UINTN Argc, CHAR8** Argv, UINT32 *TauCfg_index, UINT32 *SamplingRate_index);
EFI_STATUS PmicMipiBifTest_parseInputArguments(CHAR8 *CmdStr, CHAR8 *arg_Array[]);
CHAR8 * AsciiStrNDup(CONST CHAR8               *Source, UINTN Length);


/**
The user Entry Point for Application. The user code starts with this function
as the real entry point for the application.

@param[in] ImageHandle    The firmware allocated handle for the EFI image.  
@param[in] SystemTable    A pointer to the EFI System Table.

@retval EFI_SUCCESS       The entry point is executed successfully.
@retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
PmicMipiBifTestMain(
   IN EFI_HANDLE        ImageHandle,
   IN EFI_SYSTEM_TABLE  *SystemTable
   )
{
   EFI_STATUS  Status = EFI_SUCCESS;

   TEST_START("PmicMipiBifTest");

   Status = test_MipiBifAPI(ImageHandle, SystemTable);

   TestStatus("PmicMipiBifTest",Status);
   TEST_STOP("PmicMipiBifTest");
   return Status;
}

static EFI_STATUS test_MipiBifAPI(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_QCOM_PMIC_MIPIBIF_PROTOCOL *PmicMipiBifProtocol = NULL;
  uint32 errors = 0;
  uint32 deviceIndex = 0;

  UINT32  SlaveDeviceAddress=0x601;
  UINT32  WriteAddr, ReadAddr;
  UINT32  WriteData=0xF;
  UINT32  ReadWord;
  UINT32  Total_writes=0;
  UINT32  Total_write_fail=0;
  UINT32  Total_write_success=0;
  UINT32  Total_read_fail=0;
  UINT32  Total_writeread_fail=0;
  UINT32  Total_writeread_success=0;
  UINT32  delay=4000;
  UINT32  Throughput;

  PM_UEFI_BSI_TAU_CFG_TYPE        TauCfg_index=PM_UEFI_BSI_TAU_CFG__150_US;
  PM_UEFI_BSI_SAMPLING_RATE_TYPE  SamplingRate_index=PM_UEFI_BSI_SAMPLING_RATE__4X;
  PM_UEFI_BSI_DATA_FORMAT_TYPE    DataFormat_index=PM_UEFI_BSI_DATA_FORMAT__11_BIT;
  UINT32 mask = 0x000000FF;

 UINTN  Argc;
 CHAR8  **Argv = NULL;
 
 //Act on cmd line arguments if fails continue for charge
 Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);
 
 if(Argc > 0 )//Act only if arguments are present
 {
    AsciiPrint("Setting Arguments \r\n");
    Status = PmicMipiBifTest_ProcessCommand(Argc, Argv, &TauCfg_index, &SamplingRate_index);
 }else
 {
   AsciiPrint("Arguments not provided \r\n");
 }

 AsciiPrint("samplingrate_input:%d\n",SamplingRate_index);
 AsciiPrint("TauCfg_index:%d\n",TauCfg_index);

 Status = gBS->LocateProtocol( &gQcomPmicMipiBifProtocolGuid,
                NULL,
               (VOID **)&PmicMipiBifProtocol);

 if (Status != EFI_SUCCESS){
   AsciiPrint("MIPIBIF Protocol not present\n");
   return EFI_DEVICE_ERROR;
 } 
 
 Status = PmicMipiBifProtocol->Init(deviceIndex);
 if (Status != EFI_SUCCESS){
   AsciiPrint("PmicMipiBifProtocol->Init-FAIL deviceIndex:%d\n",deviceIndex);
   return EFI_DEVICE_ERROR;
 }
 Status = PmicMipiBifProtocol->SetMode(deviceIndex, TauCfg_index, SamplingRate_index, DataFormat_index); 
 gBS->Stall(delay);    
 if (Status != EFI_SUCCESS){ 
   AsciiPrint("PmicMipiBifProtocol->SetMode -FAIL deviceIndex:%d,TauCfg_index:%d,SamplingRate_index:%d,DataFormat_index:%d\n",deviceIndex, TauCfg_index, SamplingRate_index, DataFormat_index);
   return EFI_DEVICE_ERROR;
 }
  WriteAddr = 0x1000;
  while (WriteAddr < 0x10FF)
  { //3FF0 - 2KB SRAM available from QCOM BIF to write. (0x1000 --> 0x3FF0)
   Total_writes++;
   WriteData = WriteAddr;
   Status = PmicMipiBifProtocol->SendWords(deviceIndex, SlaveDeviceAddress, WriteAddr, WriteData);
   gBS->Stall(delay);
    if (Status != EFI_SUCCESS){
      Total_write_fail++;
      AsciiPrint("** PmicMipiBifProtocol->SendWords-FAIL ** deviceIndex:%d,SlaveDeviceAddress:0x%04X,WriteAddr:0x%04X,WriteData:0x%04X\n", deviceIndex, SlaveDeviceAddress, WriteAddr, WriteData);
      errors++;
      AsciiPrint("SUCCESS SendWords WriteAddr:0x%04X\n", WriteAddr);
    }
    WriteAddr++;
    Total_write_success++;
    AsciiPrint("** WRITE ** deviceIndex:%d,SlaveDeviceAddress:0x%04X,WriteAddr:0x%04X,WriteData:0x%04X\n", deviceIndex, SlaveDeviceAddress, WriteAddr, WriteData);
  }

  AsciiPrint("**** Reading Data Now ****\r\n");

  //Read Data
  ReadAddr = 0x1000;
  WriteData = ReadAddr;
  while (ReadAddr < 0x10FF)
  { //3FF0 - 2KB SRAM available from QCOM BIF to write. (0x1000 --> 0x3FF0)
    Status = PmicMipiBifProtocol->SendReceive(deviceIndex, SlaveDeviceAddress, ReadAddr, &ReadWord);
    gBS->Stall(delay);
    if (Status != EFI_SUCCESS){
     Total_read_fail++;
     AsciiPrint("** PmicMipiBifProtocol->SendReceive-FAIL ** deviceIndex:%d,SlaveDeviceAddress:0x%04X,ReadAddr:0x%04X\n", deviceIndex, SlaveDeviceAddress, ReadAddr);
     errors++;
    }else{
     if (ReadWord != (ReadAddr & mask)){
       Total_writeread_fail++;
       AsciiPrint("** FAIL **(READ MISMATCH); ReadAddr:0x%04X,WRITE:0x%04X,READ:0x%04X\n", ReadAddr,ReadAddr,ReadWord);
       errors++;
       return Status;
     }else{
       Total_writeread_success++;
       AsciiPrint(" SUCCESS CASE;ReadAddr:0x%04X,WRITE:0x%04X,READ:0x%04X\n", ReadAddr,WriteData,ReadWord);
     }
    }
     ReadAddr++;
     WriteData = ReadAddr;
  }
 
 Throughput = (Total_writeread_success/Total_writes)*100;
 AsciiPrint("Total_writes:%d\r\n",Total_writes);
 AsciiPrint("Total_write_fail:%d\r\n",Total_write_fail);
 AsciiPrint("Total_write_success:%d\r\n",Total_write_success);
 AsciiPrint("Total_read_fail:%d\r\n",Total_read_fail);
 AsciiPrint("Total_writeread_fail:%d\r\n",Total_writeread_fail);
 AsciiPrint("Total_writeread_success:%d\r\n",Total_writeread_success);
 AsciiPrint("Throughput:%d %%\r\n",Throughput);
 
 if (errors == 0) {
     AsciiPrint("pmicMipiBifProtocolMipiBifSendWords TEST: passed\r\n");
     return EFI_SUCCESS;
 }else
   AsciiPrint("pmicMipiBifProtocolMipiBifSendWords TEST: failed\r\n");

  return Status;
}



/**
  PmicMipiBifTest_parseInputArguments()
  
  @brief
..parse command line arguments into array of arguments
**/
EFI_STATUS PmicMipiBifTest_parseInputArguments(CHAR8 *CmdStr, CHAR8 *arg_Array[])
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CHAR8 *OrgStr = NULL, *temp = NULL;
  CHAR8 *firstArg = NULL, *SecArgument = NULL;
  UINT8 count = 0, i = 0;

  if (!CmdStr)
    return EFI_INVALID_PARAMETER;

  OrgStr = AsciiStrNDup(CmdStr, AsciiStrLen(CmdStr));
  GOTOERRORIFNULL(OrgStr,Status,Error_Exit);

  temp = OrgStr;

  temp = AsciiStrStr(temp, ",");
  if(NULL != temp)
  {
     //copy first argument
     count = temp - OrgStr;
     firstArg = (CHAR8 *)AllocatePool(count + 1);
     GOTOERRORIFNULL(firstArg,Status,Error_Exit);
    while(i != count)
    {
      *(firstArg + i) = *(OrgStr + i);
      i++;
    }
    *(firstArg + i ) = '\0';
    arg_Array[0] = firstArg;

    temp++;
    SecArgument = AsciiStrNDup(temp, AsciiStrLen(temp));
    GOTOERRORIFNULL(SecArgument,Status,Error_Exit);
    arg_Array[1] = SecArgument;
  }else
  {
    AsciiPrint("Bad Parameter");
  }

Error_Exit:
  FreePool(OrgStr);
  OrgStr = NULL;

  return Status;

}

#define MAX_LENTHG_EACH_ARGUMENT 4
#define MAX_ARGUMENTS 2


/**
  PmicMipiBifTest_ProcessCommand()
  
  @brief
..Act on command line arguments
**/
EFI_STATUS PmicMipiBifTest_ProcessCommand (UINTN Argc, CHAR8** Argv, UINT32 *TauCfg_index, UINT32 *SamplingRate_index)
{
  EFI_STATUS  Status   = EFI_SUCCESS;
  CHAR8       *CmdStr  = NULL;
  CHAR8       *arg_Array[MAX_ARGUMENTS];

  if(NULL == *Argv || Argc == 0){
    return EFI_INVALID_PARAMETER;
  }

  CmdStr = Argv[0];

  AsciiPrint ("Processing Cmd: %a", CmdStr);
  AsciiPrint ("\r\n");

  Status = PmicMipiBifTest_parseInputArguments(CmdStr, arg_Array);
  GOTOERROR(Status,Error_Exit);

  *TauCfg_index = AsciiToInt(arg_Array[0]);
  AsciiPrint ("Tau Index : %d \r\n", *TauCfg_index);
  
  *SamplingRate_index = AsciiToInt(arg_Array[1]);
  AsciiPrint ("SamplingRate_indexIndex : %d \r\n", *SamplingRate_index);

  FreePool(arg_Array[0]);
  arg_Array[0] = NULL;
  FreePool(arg_Array[1]);
  arg_Array[1] = NULL;

Error_Exit:
  return Status;
}

/**
  Duplicates ASCII string 
  
  @param  Source  Null Terminated string of characters 
  @param  Length  source String length  
  @return  Duplicated String  if successful otherwise return NULL
  
  note: Caller responsibility to Free returned String
**/
CHAR8 * AsciiStrNDup(
  CONST CHAR8               *Source,
  UINTN                     Length
)
{
  CHAR8 *Dest = NULL;
  if(NULL == Source || 0 == Length)
    return NULL;

  Dest = AllocatePool (Length + 1);
  if (NULL == Dest)
  {
    return NULL;
  }

  AsciiStrnCpy(Dest, Source, Length + 1);

  return Dest;
}



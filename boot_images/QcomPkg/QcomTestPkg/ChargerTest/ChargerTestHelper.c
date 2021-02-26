/** @file ChargerTestHelper.c
   
  Helper for Charger test Application

  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/20/17   va      updated for charger ex protocol
 09/29/14   va      New File (Support Cmd line arg for display and targetSOC)

=============================================================================*/
#include "ChargerTest.h"

/**
  ChargerTestH_parseInputArguments()
  
  @brief
..parse command line arguments into array of arguments
**/
EFI_STATUS ChargerTestH_parseInputArguments(CHAR8 *CmdStr, CHAR8 *arg_Array[])
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


/**
  ChargerTestH_ProcessCommand()
  
  @brief
..Act on command line arguments
**/
EFI_STATUS ChargerTestH_ProcessCommand (UINTN Argc, CHAR8** Argv, UINT32 *targetSOC, boolean *displayStatus)
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

  Status = ChargerTestH_parseInputArguments(CmdStr, arg_Array);
  GOTOERROR(Status,Error_Exit);

  *targetSOC = AsciiToInt(arg_Array[0]);
  AsciiPrint ("targetSOC : %d \r\n", *targetSOC);
  
  AsciiStrToLower(arg_Array[1], AsciiStrLen(arg_Array[1]));

  if(0 == AsciiStrCmp(arg_Array[1], "on"))
   *displayStatus = TRUE;
  else if(0 == AsciiStrCmp(arg_Array[1], "off"))
   *displayStatus = FALSE;
  else//default to true if invalid input valueis entered
   *displayStatus = TRUE;

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


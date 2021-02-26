/*============================================================================
@file ChargerTestHelper.h

ChargerTestHelper header file

Copyright (c) 2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/29/14   va      New File (Support Cmd line arg for display and targetSOC)

=============================================================================*/

#ifndef CHARGERTESTHELPER_H
#define CHARGERTESTHELPER_H

/*=========================================================================
      Include Files
==========================================================================*/
#include "ChargerTest.h"
#include <Library/MemoryAllocationLib.h>

/**
  ADC Dependencies 
*/
#include <Protocol/EFIAdc.h>
/**
  Display Dependencies 
*/
#include <Protocol/EFIDisplayPwrCtrl.h>
    
#include <Protocol/EFIUsbfnIo.h>

#include <Protocol/EFIPmicSmbchg.h>

/*=========================================================================
      Macro Definitions
==========================================================================*/
#define MAX_LENTHG_EACH_ARGUMENT 4
#define MAX_ARGUMENTS 2

#define GOTOERRORIFNULL(Str,Status,Error_Exit) if (NULL == Str){ Status = EFI_OUT_OF_RESOURCES; goto Error_Exit; }
#define GOTOERROR(Status,Error_Exit) if (EFI_SUCCESS != Status ) { goto Error_Exit; } 
#define FOREVER for(;;)

/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Function ProtoType
==========================================================================*/

EFI_STATUS ChargerTestH_parseInputArguments(CHAR8 *CmdStr, CHAR8 *arg_Array[]);
EFI_STATUS ChargerTestH_ProcessCommand (UINTN Argc, CHAR8** Argv, UINT32 *targetSOC, boolean *displayStatus);


/* Duplicates ASCII string */
CHAR8 * AsciiStrNDup(CONST CHAR8 *Source, UINTN Length);


#endif // CHARGERTESTHELPER_H


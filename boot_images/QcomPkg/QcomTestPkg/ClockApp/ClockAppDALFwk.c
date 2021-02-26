/** @file ClockAppDALFwk.c
   
  This file contains application test code for clock driver. 
  The code uses DAL framework to make appropriate function 
  calls. 

  Copyright (c) 2010-2012,2014 Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
 01/25/13   vk      Fix warning
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 12/23/10   vs      Created.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIDALSYSProtocol.h>
#include "DALDeviceId.h"
#include "DDIClock.h"
#include "ClockTestUEFI.h"


/*=========================================================================
      Functions
==========================================================================*/


/**
  Clock test using DAL framework only. 

  @param 
  None. 

  @return
  None.
**/

void ClockTestUsingDALFramework()
{
  DALResult           eResult;
  CONST CHAR8         *szClockName;  
  UINT32              nCount, nFreqHz;
  BOOLEAN             nFlag, bIsEnabled;
  DalDeviceHandle     *hClock = NULL;
  ClockIdType         nClockID;

  // Init DALSys
  DALSYS_InitMod(NULL);

  //Attach to the clock DAL
  eResult = DAL_DeviceAttach(DALDEVICEID_CLOCK, &hClock);
  if(eResult != DAL_SUCCESS)
  {
    ASSERT_EFI_ERROR(EFI_PROTOCOL_ERROR);
  }

  AsciiPrint ("\n==================o===================\n");
  AsciiPrint ("Clock Application Driver - DAL Framework\n");
  AsciiPrint ("========================================\n");
  nCount = 0; 
  while(1)
  {
    /*
     * Get clock name
     */
    szClockName = ClockTestList[nCount].szName;
    if(szClockName[0] == '\0') 
    {
       /*
        * Reached the end of clock test array
        */
       break;
    }
    else
    {
      AsciiPrint ("Checking clock \"%a\" ...\n", szClockName);
    }

    /*
     *  For each clock get the ID
     */
    eResult = DalClock_GetClockId(hClock, szClockName, &nClockID);
    if(eResult != DAL_SUCCESS)
    {
      /*
       * This clock is either not found or something has gone wrong. 
       * Either way we can't do rest of the tests so try the next clock. 
       */

      AsciiPrint ("WARNING: Clock \"%a\" ID not obtained!\n", szClockName);
      AsciiPrint ("========================================\n");
      nCount++;
      continue;    
    }

    /*
     * Check IsClockOn
     */
    nFlag = DalClock_IsClockOn(hClock, nClockID);
    AsciiPrint ("IsClockOn: %a\n", (nFlag == TRUE) ? "TRUE":"FALSE");

    /*
     *  Check IsClockEnabled   
     */
    nFlag = DalClock_IsClockEnabled(hClock, nClockID);
    bIsEnabled = nFlag;
    AsciiPrint ("IsClockEnabled: %a\n", (nFlag == TRUE) ? "TRUE":"FALSE");

    /*
     * If clock is not enabled, then enable it
     */
    if(!bIsEnabled)
    {    
      eResult = DalClock_EnableClock(hClock, nClockID);
      if(eResult != DAL_SUCCESS) 
      {
        AsciiPrint ("WARNING: EnableClock: returned an error!\n");
      }
      else
      {
        AsciiPrint ("EnableClock: SUCCESS\n");
      }

      /*
       * Recheck IsClockEnabled
       */
      nFlag = DalClock_IsClockEnabled(hClock, nClockID);
      AsciiPrint ("Rechecking IsClockEnabled: %a\n", (nFlag == TRUE) ? "TRUE":"FALSE");

      /*
       * Recheck IsClockOn
       */
      nFlag = DalClock_IsClockOn(hClock, nClockID);
      AsciiPrint ("Rechecking IsClockOn: %a\n", (nFlag == TRUE) ? "TRUE":"FALSE");

      /*
       * Only change/set clock frequency of the clocks that were enabled by us
       */
      if(ClockTestList[nCount].nFreqHz > 0)
      {
        //Try exact frequency match
        eResult = DalClock_SetClockFrequency(hClock, nClockID, ClockTestList[nCount].nFreqHz, 
                                             (ClockFrequencyType) 3, (uint32 *)&nFreqHz);
        if(eResult != DAL_SUCCESS) 
        {
          AsciiPrint ("WARNING: SetClockFrequency: returned an error!\n");
        }
        else
        {
          AsciiPrint ("SetClockFrequency: %d\n", nFreqHz);
        }
    
        /*
         * Get clock frequency
         */
        eResult = DalClock_GetClockFrequency(hClock, nClockID, (uint32 *)&nFreqHz);
        if(eResult != DAL_SUCCESS) 
        {
          AsciiPrint ("WARNING: GetClockFrequency: returned an error!\n");
        }
        else
        {
          AsciiPrint ("GetClockFrequency: %d\n", nFreqHz);
        }
      }
    }

    /*
     * Calculate clock frequency
     */
    eResult = DalClock_CalcClockFrequency(hClock, nClockID, (uint32 *)&nFreqHz);
    if(eResult != DAL_SUCCESS) 
    {
      AsciiPrint ("WARNING: CalcClockFrequency: returned an error!\n");
    }
    else
    {
      AsciiPrint ("CalcClockFrequency: %d\n", nFreqHz);
    }

    /*
     * Only disable clocks that weren't enabled by us
     */
    if(!bIsEnabled)
    {
      eResult = DalClock_DisableClock(hClock, nClockID);
      if(eResult != DAL_SUCCESS) 
      {
        /*
         * DisableClock 
         */
        AsciiPrint ("WARNING: DisableClock: returned an error!\n");
      }
      else
      {
        AsciiPrint ("DisableClock: SUCCESS\n");
      }
  
      /*
       * Recheck IsClockEnabled
       */
      nFlag = DalClock_IsClockEnabled(hClock, nClockID);
      AsciiPrint ("Rechecking IsClockEnabled: %a\n", (nFlag == TRUE) ? "TRUE":"FALSE");
  
      /*
       * Recheck IsClockOn
       */
      nFlag = DalClock_IsClockOn(hClock, nClockID);
      AsciiPrint ("Rechecking IsClockOn: %a\n", (nFlag == TRUE) ? "TRUE":"FALSE");
    }

    AsciiPrint ("========================================\n");

    nCount++;
  }

  AsciiPrint ("\n");
}

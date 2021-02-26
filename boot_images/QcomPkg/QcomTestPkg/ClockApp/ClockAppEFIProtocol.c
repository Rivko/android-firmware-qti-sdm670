/** @file ClockAppEFIProtocol.c

  This file contains application test code for clock driver.
  The code uses clock EFI protocol interface to make appropriate
  function calls.

  Copyright (c) 2010-2012, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
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
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIClock.h>
#include "ClockTestUEFI.h"


/*=========================================================================
      Functions
==========================================================================*/

/**
  Clock test using EFI protocol only.

  @param
  None.

  @return
  None.
**/

void ClockBISTUsingEFIProtocol()
{
 EFI_STATUS               eResult;
 EFI_TEST_CLOCK_PROTOCOL *pTestClockProtocol;


 AsciiPrint ("\n==================o===================\n");
 AsciiPrint ("Clock Application Driver - EFI Protocol\n");
 AsciiPrint ("========================================\n");

 eResult = gBS->LocateProtocol(&gEfiTestClockProtocolGuid,NULL,
                                 (VOID **)&pTestClockProtocol );

 if (pTestClockProtocol == NULL){
   AsciiPrint ("ClockApp:: Error locating TestClock Protocol \r\n");
   return;
 }

 eResult = pTestClockProtocol->DumpClocksState(pTestClockProtocol,0);
 if (eResult !=  EFI_SUCCESS) {
   AsciiPrint ("ClockApp:: Error DumpClocksState \r\n");
   return;
 }

  AsciiPrint (" >>>>   End Clock Application\n");
}


static EFI_CLOCK_PROTOCOL *ClockProtocol = NULL;

void ClockTestLPM(void)
{

  EFI_STATUS Status = EFI_SUCCESS;
  if(!ClockProtocol)
  {
    Status = gBS->LocateProtocol(&gEfiClockProtocolGuid, NULL, (VOID **)&ClockProtocol);
  }
  if (ClockProtocol && (EFI_SUCCESS == Status))
  {
     Status = ClockProtocol->EnterLowPowerMode(ClockProtocol);

     if(EFI_SUCCESS != Status)
     {
       AsciiPrint( "ClockTestLPM:: EnterLowPowerMode error. \r\n");
     }
     else
     {
       Status = ClockProtocol->ExitLowPowerMode(ClockProtocol);
     }

     if(EFI_SUCCESS != Status)
     {
       AsciiPrint( "ClockTestLPM:: ExitLowPowerMode error. \r\n");
     }
  }
}


typedef struct
{
  const char      *szName;        /**< Clock name. */
  UINTN            nClockId;      /**< Clock ID*/
  UINT32           nFreqHz;      /**< Clock ID*/
} ClockNodeType;

static ClockNodeType aDispClocks[] =
{
  {
   .szName         = "disp_cc_mdss_ahb_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_axi_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_byte0_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_byte0_intf_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_byte1_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_byte1_intf_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_esc0_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_esc1_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_mdp_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_mdp_lut_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_pclk0_clk",
   .nFreqHz        = 0,
  },
  {
   .szName         = "disp_cc_mdss_pclk1_clk",
   .nFreqHz        = 0,
  },
};

void ClockTestDispClocks(void)
{

  EFI_STATUS       eResult = EFI_SUCCESS;
  UINT32           nNumClocks;
  UINTN            nClockId, i;
  ClockNodeType    *pClocksNode;
  CHAR16           sUnicodeName[50];


  if(!ClockProtocol)
  {
    eResult = gBS->LocateProtocol(&gEfiClockProtocolGuid, NULL, (VOID **)&ClockProtocol);
  }

  if (ClockProtocol && (EFI_SUCCESS == eResult))
  {
    eResult = ClockProtocol->GetClockID(ClockProtocol,"gcc_disp_xo_clk",&nClockId);

	if (EFI_SUCCESS != eResult )  AsciiPrint( "ClockTestDispClocks: GetClockId - gcc_disp_xo_clk error. \r\n");;
    eResult = ClockProtocol->EnableClock(ClockProtocol, nClockId);
	if (EFI_SUCCESS != eResult )  AsciiPrint( "ClockTestDispClocks: Clock_EnableClock - gcc_disp_xo_clk error. \r\n");;

    pClocksNode = &aDispClocks[0];
    nNumClocks = sizeof(aDispClocks)/sizeof(aDispClocks[0]);

    AsciiPrint( " Clock-name \t\t Frequency \r\n");
    for (i = 0; i < nNumClocks; i++)
    {
      pClocksNode = &aDispClocks[i];
      eResult = ClockProtocol->GetClockID(ClockProtocol,pClocksNode->szName,&pClocksNode->nClockId);

      AsciiStrToUnicodeStr(pClocksNode->szName,sUnicodeName);
	  if (EFI_SUCCESS != eResult )  AsciiPrint( "ClockTestDispClocks: %s GetClockId error.\n",sUnicodeName );

      eResult = ClockProtocol->GetClockFreqHz(ClockProtocol,pClocksNode->nClockId, &pClocksNode->nFreqHz);

	  if (EFI_SUCCESS != eResult )  AsciiPrint( "ClockTestDispClocks: %s GetClockFrequency error.\n",sUnicodeName );
	  else  AsciiPrint( " %s \t %d \r\n", sUnicodeName, pClocksNode->nFreqHz);
	}
	AsciiPrint( " >>>>    END DISP CLOCKS    <<<<<<<<< \r\n");

  }
}

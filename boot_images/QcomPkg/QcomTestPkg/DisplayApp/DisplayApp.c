/** @file DisplayApp.c
   
  Test application for DisplayDxe graphics output protocol

  Copyright (c) 2010-2017, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
**/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/EFIDisplayPwrCtrl.h>
#include <Protocol/EFIDisplayPwr.h>
#include <Protocol/EFIClock.h>
#include <Protocol/EFIDisplayUtils.h>


/*=========================================================================
   Defines
==========================================================================*/

#define BITMAP_IMAGE_WIDTH   640
#define BITMAP_IMAGE_HEIGHT  480

#define MAX_STRING           64

#define BACKGROUND_R         0x00
#define BACKGROUND_G         0x66
#define BACKGROUND_B         0xCC
#define BACKGROUND_A         0xFF

#define DEFAULT_WAIT         (10*16*1000)    // ~10  Frames
#define DEFAULT_LONG_WAIT    (30*16*1000)    // ~30 Frames


/*=========================================================================
      Type Definitions
==========================================================================*/

/* Enumeration of display variables
 */
typedef enum {
  DISPLAY_VAR_DISABLEDISPLAY = 0,
  DISPLAY_VAR_DISPLAYFLAGS,
  DISPLAY_VAR_DISPLAYID,
  DISPLAY_VAR_MAX,
} EnvVariables;

/* Display Variable flags 
 */
#define   DISPLAY_FLAG_AMOLED         0x80000000
#define   DISPLAY_FLAG_PANELOVERRIDE  0x40000000

/* Test case entry 
 */
typedef struct {
  CHAR8        sTestName[MAX_STRING];
  EFI_STATUS (*pTestFn)(void);
} TestCaseEntry;


/* Environment variable list
 */
typedef struct {
  CHAR16      sVarName[MAX_STRING];
  UINTN       nVarSize;
} EnvVarEntry;


/* Clock list entry
 */
typedef struct {
  CHAR8       sClockName[MAX_STRING];
} ClockListEntry;

/*=========================================================================
     Local Function Declarations
==========================================================================*/

/* Blt operation test */
static EFI_STATUS DisplayBltOperationTest(void);
/* Blt Performance test */
static EFI_STATUS DisplayBltPerformanceTest(void);
/* Backlight brightness control test */
static EFI_STATUS DisplayBacklightBrightnessLevelControlTest(void);
/* Backlight brightness status test */
static EFI_STATUS DisplayBacklightBrightnessLevelStatusTest(void);
/* Display panel power control test */
static EFI_STATUS DisplayPanelPowerControlTest(void);
/* Display panel power status test */
static EFI_STATUS DisplayPanelPowerStatusTest(void);
/* Display panel power state test */
static EFI_STATUS DisplayPanelPowerStateTest(void);
/*  Set an environment variable */
static EFI_STATUS DisplaySetEnvVar(CHAR16 *pVarname, UINT32 uValue);
/*  Get an environment variable */
static EFI_STATUS DisplayGetEnvVar(CHAR16 *pVarname, UINT32 *pValue);
/* Event based Display power control test */
static EFI_STATUS DisplayPowerEventTest(void);
/* Generate a checkerboard pattern bitmap */
static EFI_STATUS DisplayGenerateBitmap(void);
/* Output display app help menu */
static void DisplayTestHelp(void);
/* Display BVT */
static EFI_STATUS DisplayBVT(UINT32 uTestNumber);
/* Commandline parser helper */
static UINTN DisplayParseCommand(CHAR8 *pCommandLine, CHAR8 *pParam, CHAR8 *pValue);
/* Clock Status helper */
static void DisplayClockStatus(CHAR8 *pClockName);
/* Clock dumper helper */
static void DisplayDumpClocks();
/* Display Panel List Utility */
static UINT32 DisplayListPanels(uint32 uCurrentIndex, UINTN bShow);

/*=========================================================================
      Globals
==========================================================================*/

extern EFI_GUID gOSAVendorGuid;
void           *pBitmapImage = NULL;

/* Total number of test cases */
#define NUM_TEST_CASES        (sizeof(TestCaseList)/sizeof(TestCaseEntry))

/* List of display test cases 
 */
TestCaseEntry TestCaseList[] = 
{
  {"Blt Performance",    &DisplayBltPerformanceTest},
  {"Blt Operation",      &DisplayBltOperationTest},
  {"Power Control",      &DisplayPanelPowerControlTest},
  {"Power State",        &DisplayPanelPowerStateTest},
  {"Power Status",       &DisplayPanelPowerStatusTest},
  {"Brightness Control", &DisplayBacklightBrightnessLevelControlTest},  
  {"Brightness Status",  &DisplayBacklightBrightnessLevelStatusTest},  
  {"Power Event",        &DisplayPowerEventTest}
};

/* List of display environment variables
 */
EnvVarEntry EnvVarList[] =
{
  {L"DISABLEDISPLAY", sizeof(UINT8)},     // DISPLAY_VAR_DISABLEDISPLAY
  {L"DISPLAYCONFIG",  sizeof(UINT32)},    // DISPLAY_VAR_DISPLAYFLAGS
  {L"DISPLAYID",      sizeof(UINT32)},    // DISPLAY_VAR_DISPLAYID  
};  


/* Total number of clock entries */
#define NUM_CLOCK_ENTRIES         (sizeof(ClockList)/sizeof(ClockListEntry))    

/* List of all mdss clocks 
 */
ClockListEntry ClockList[] = 
{
   {"gcc_disp_ahb_clk"},
   {"disp_cc_mdss_ahb_clk"},
   {"disp_cc_mdss_mdp_clk"},
   {"disp_cc_mdss_vsync_clk"},
   {"disp_cc_mdss_esc0_clk"},
   {"disp_cc_mdss_byte0_clk"},
   {"disp_cc_mdss_byte0_intf_clk"},
   {"disp_cc_mdss_pclk0_clk"},
   {"disp_cc_mdss_esc1_clk"},   
   {"disp_cc_mdss_byte1_clk"},
   {"disp_cc_mdss_byte1_intf_clk"},
   {"disp_cc_mdss_pclk1_clk"},
};

/*=========================================================================
     Global Functions
==========================================================================*/

/**
  The user entry point for the display application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DisplayAppMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS   eStatus  = EFI_INVALID_PARAMETER;
  UINTN        Argc     = 0;
  UINTN        HasValue = 0;
  CHAR8      **Argv;
  CHAR8        Param[MAX_STRING], Value[MAX_STRING];

  AsciiPrint("DisplayApp (Compiled on: %a)\n\n", __DATE__);
  
  // Get the command line arguments
  if ((EFI_SUCCESS == GetCmdLineArgs (ImageHandle, &Argc, &Argv)) &&
      (Argc > 0))
  {
    // Parse the arguments
    HasValue = DisplayParseCommand((CHAR8*)Argv[0], (CHAR8*)&Param, (CHAR8*)&Value);

    // If we have at least one parameter
    if (HasValue)
    {
      // Command line passed -<Param>:<Value>, we really just want to know if <Value> was passed for the function below so subtract the number of
      // parased commands in the command line.
      HasValue--;
      eStatus = EFI_SUCCESS;
    }
  }

  // Print help on failure
  if (EFI_SUCCESS != eStatus)
  {
    DisplayTestHelp();
    return 0;
  }


  // Execute the BVT
  if (0 == AsciiStrCmp("bvt", Param))
  {
    UINT32 uTestCase = AsciiStrDecimalToUintn(Value);
    
    // Execute the BVT
    DisplayBVT(uTestCase);
  }
  else if (0==AsciiStriCmp("disabledisplay", Param))
  {
      UINTN uValue = AsciiStrDecimalToUintn(Value);  

      // Configure or display the environment variable
      if (EFI_SUCCESS != DisplaySetEnvVar((CHAR16*)&EnvVarList[DISPLAY_VAR_DISABLEDISPLAY].sVarName, uValue))
      {
         AsciiPrint("Error: Failed to set environment variable for disabledisplay.\n");
      }
  }
  else if (0==AsciiStriCmp("amoled", Param))
  {
      UINT32 uFlag;
      UINT32 uMask;

      if (AsciiStrDecimalToUintn(Value))
      {
        uFlag = DISPLAY_FLAG_AMOLED;
        uMask = ~DISPLAY_FLAG_AMOLED;
      }
      else
      {
        uFlag = 0;
        uMask = ~DISPLAY_FLAG_AMOLED;      
      }

      // Configure or display the environment variable
      if (EFI_SUCCESS != DisplaySetEnvVar((CHAR16*)&EnvVarList[DISPLAY_VAR_DISPLAYFLAGS].sVarName, uFlag))
      {
         AsciiPrint("Error: Failed to set environment variable for display flags.\n");
      }

  }
  else if (0==AsciiStriCmp("panelid", Param))
  {
      UINTN uValue = AsciiStrHexToUintn(Value);
      UINT32 nNumPanels  = 0;
      UINTN  uPanelIndex = 0;

      // Configure or display the environment variable
      if (EFI_SUCCESS != DisplaySetEnvVar((CHAR16*)&EnvVarList[DISPLAY_VAR_DISPLAYID].sVarName, uValue))
      {
         AsciiPrint("Error: Failed to set environment variable for display id.\n");
      }

      AsciiPrint("DisplayApp priting panel list:-\n\n");
      nNumPanels = DisplayListPanels(uPanelIndex, 1);
  }
  else
  {
    AsciiPrint("Error: %a is an invalid parameter\n", Param);
  }


  return eStatus;
}

/*=========================================================================
     Local Functions
==========================================================================*/


/* Backlight brightness level control test */
static EFI_STATUS DisplayBVT(UINT32 uTestNumber)
{
  EFI_STATUS eStatus     = EFI_SUCCESS;
  UINT32     uTestStart  = 0;    
  UINT32     uTestEnd    = NUM_TEST_CASES;
  UINT32     uTestCount  = 0;
  UINT32     uTestPassed = 0;
  UINT32     uCount;  
  
  eStatus = DisplayGenerateBitmap();
  
  if (EFI_SUCCESS != eStatus)
  {
    AsciiPrint("Unable to generate bitmap, cannot continue!\n\n");
    return eStatus;
  }

  if ((uTestNumber > 0) &&
      (uTestNumber <= NUM_TEST_CASES))
  {
    uTestStart = uTestNumber-1;
    uTestEnd   = uTestStart+1;
  }

  // Loop all tests
  for (uCount=uTestStart;uCount<uTestEnd;uCount++)
  {
    AsciiPrint("======] Test Case Start   : %a\n\n", TestCaseList[uCount].sTestName);

    // Increment the number of tests run
    uTestCount++;
    if (EFI_SUCCESS == (eStatus = TestCaseList[uCount].pTestFn()))
    {
      // Increment the test pass count
      uTestPassed++;
    }

    AsciiPrint("\n======] Test Case Complete: %a\n", (EFI_SUCCESS==eStatus)?"Passed":"Failed");
  }

  if (uTestCount)
  {
    AsciiPrint("\n\nDisplay BVT Result: %d tests run (%d%% pass rate)\n", uTestCount, (100*uTestPassed)/uTestCount);
  }

  // Free the image buffer
  if (NULL != pBitmapImage)
  {
    gBS->FreePool (pBitmapImage);    
  }

  return eStatus;
}


/* Backlight brightness level control test */
static EFI_STATUS DisplayBacklightBrightnessLevelControlTest(void)
{
  EFI_STATUS                           eStatus = EFI_SUCCESS;
  EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL  *pDisplayPwrCtrlProtocol;
  
  if (EFI_SUCCESS != (eStatus = gBS->LocateProtocol (&gQcomDisplayPwrCtrlProtocolGuid, 
                                                     NULL, 
                                                     (VOID **)&pDisplayPwrCtrlProtocol)))                     
  {
    AsciiPrint ("DisplayBacklightBrightnessLevelControlTest: LocateProtocol() failed.\n");
    return eStatus;
  }
  else
  {
    UINT32            uBackLightLevel = 0;
    EFI_DISPLAY_TYPE  eDisplayType    = EFI_DISPLAY_TYPE_PRIMARY;
    
    AsciiPrint ("DisplayBacklightBrightnessLevelControlTest: Decrease the display backlight...\n");
    /* Decrease the backlight gradually */ 
    for (uBackLightLevel = 100; uBackLightLevel > 0; uBackLightLevel--)
    {
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->DisplayBacklightBrightnessLevelControl(eDisplayType, uBackLightLevel)))
      {
        AsciiPrint ("DisplayBacklightBrightnessLevelControlTest: DisplayBacklightBrightnessLevelControl() failed.\n");
        return eStatus;
      }   
      gBS->Stall(DEFAULT_WAIT); 
    }
    AsciiPrint ("DisplayBacklightBrightnessLevelControlTest: Increase the display backlight...\n");
    /* Increase the backlight gradually */ 
    for (uBackLightLevel = 0; uBackLightLevel <= 100; uBackLightLevel++)
    {
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->DisplayBacklightBrightnessLevelControl(eDisplayType, uBackLightLevel)))
      {
        AsciiPrint ("DisplayBacklightBrightnessLevelControlTest: DisplayBacklightBrightnessLevelControl() failed.\n");
        return eStatus;
      }
      gBS->Stall(DEFAULT_WAIT);
    }         
  }
  return eStatus;
}

/* Backlight brightness status test */
static EFI_STATUS DisplayBacklightBrightnessLevelStatusTest(void)
{
  EFI_STATUS                           eStatus = EFI_SUCCESS;
  EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL  *pDisplayPwrCtrlProtocol;
  
  if (EFI_SUCCESS != (eStatus = gBS->LocateProtocol (&gQcomDisplayPwrCtrlProtocolGuid, 
                                                     NULL, 
                                                     (VOID **)&pDisplayPwrCtrlProtocol)))                     
  {
    AsciiPrint ("DisplayBacklightBrightnessLevelStatusTest: LocateProtocol() failed.\n");
    return eStatus;
  }
  else
  {
    UINT32            uRetBackLightLevel  = 0;
    UINT32            uBackLightLevel     = 0;
    EFI_DISPLAY_TYPE  eDisplayType        = EFI_DISPLAY_TYPE_PRIMARY;
    
    AsciiPrint ("DisplayBacklightBrightnessLevelStatusTest: Decrease the display backlight...\n");
    /* Decrease the backlight gradually */ 
    for (uBackLightLevel = 0; uBackLightLevel <= 100; uBackLightLevel++)
    {
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->DisplayBacklightBrightnessLevelControl(eDisplayType, uBackLightLevel)))
      {
        AsciiPrint ("DisplayBacklightBrightnessLevelStatusTest: DisplayBacklightBrightnessLevelControl() failed.\n");
        return eStatus;
      }
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->DisplayBacklightBrightnessLevelStatus(eDisplayType, &uRetBackLightLevel)))
      {
        AsciiPrint ("DisplayBacklightBrightnessLevelStatusTest: DisplayBackLightBrightnessLevelStatus() failed.\n");
        return eStatus;
      }
      AsciiPrint ("DisplayBacklightBrightnessLevelStatusTest: Backlight level - %d\n", uRetBackLightLevel);
      gBS->Stall(DEFAULT_WAIT); 
    }
  }
  return eStatus;
}

/* Panel power control test */
static EFI_STATUS DisplayPanelPowerControlTest(void)
{
  EFI_STATUS                           eStatus = EFI_SUCCESS;
  EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL  *pDisplayPwrCtrlProtocol;
  
  if (EFI_SUCCESS != (eStatus = gBS->LocateProtocol (&gQcomDisplayPwrCtrlProtocolGuid, 
                                                     NULL, 
                                                     (VOID **)&pDisplayPwrCtrlProtocol)))                     
  {
    AsciiPrint ("DisplayPanelPowerControlTest: LocateProtocol() failed.\n");
    return eStatus;
  }
  else
  {
    UINT32                         cntr = 0;
    EFI_DISPLAY_TYPE               eDisplayType = EFI_DISPLAY_TYPE_PRIMARY;
    EFI_DISPLAY_POWER_CTRL_STATE   ePowerState  = EFI_DISPLAY_POWER_STATE_OFF;    
    
    for (cntr = 0; cntr < 2; cntr++)
    {
      AsciiPrint ("DisplayPanelPowerControlTest: Display panel power off...\n");
      /* Turn off the display */
      ePowerState  = EFI_DISPLAY_POWER_STATE_OFF;
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->DisplayPanelPowerControl(eDisplayType, ePowerState)))
      {
        AsciiPrint ("DisplayPanelPowerControlTest: DisplayPanelPowerControl() failed.\n");
        return eStatus;
      } 
      gBS->Stall(DEFAULT_LONG_WAIT);
    
      /* Dump Clock Status */
      DisplayDumpClocks();
    
      AsciiPrint ("DisplayPanelPowerControlTest: Display panel power on...\n");
      /* Turn on the display */
      ePowerState  = EFI_DISPLAY_POWER_STATE_ON;    
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->DisplayPanelPowerControl(eDisplayType, ePowerState)))
      {
        AsciiPrint ("DisplayPanelPowerControlTest: DisplayPanelPowerControl() failed.\n");
        return eStatus;
      }
      gBS->Stall(DEFAULT_LONG_WAIT);
    
      /* Dump Clock Status */
      DisplayDumpClocks();
    }
  }
  return eStatus;
}

/* Display panel power status test */
static EFI_STATUS DisplayPanelPowerStatusTest(void)
{
  EFI_STATUS                           eStatus = EFI_SUCCESS;
  EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL  *pDisplayPwrCtrlProtocol;
  
  if (EFI_SUCCESS != (eStatus = gBS->LocateProtocol (&gQcomDisplayPwrCtrlProtocolGuid, 
                                                     NULL, 
                                                     (VOID **)&pDisplayPwrCtrlProtocol)))                     
  {
    AsciiPrint ("DisplayPanelPowerStatusTest: LocateProtocol() failed.\n");
    return eStatus;
  }
  else
  {
    UINT32                         cntr            = 0;
    EFI_DISPLAY_POWER_CTRL_STATE   uPanelPwrState  = (EFI_DISPLAY_POWER_CTRL_STATE)0;
    EFI_DISPLAY_TYPE               eDisplayType    = EFI_DISPLAY_TYPE_PRIMARY;
    EFI_DISPLAY_POWER_CTRL_STATE   ePowerState     = EFI_DISPLAY_POWER_STATE_OFF;    
    
    for (cntr = 0; cntr < 3; cntr++)
    {
      /* Turn off the display */
      ePowerState  = EFI_DISPLAY_POWER_STATE_OFF;
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->DisplayPanelPowerControl(eDisplayType, ePowerState)))
      {
        AsciiPrint ("DisplayPanelPowerStatusTest: DisplayPanelPowerControl() failed.\n");
        return eStatus;
      } 
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->DisplayPanelPowerStatus(eDisplayType, &uPanelPwrState)))
      {
        AsciiPrint ("DisplayPanelPowerStatusTest: DisplayPanelPowerStatus() failed.\n");
        return eStatus;
      } 
      AsciiPrint ("DisplayPanelPowerStatusTest: Display panel power state - %d\n", uPanelPwrState ? 1 : 0);
      gBS->Stall(DEFAULT_LONG_WAIT);

      /* Dump Clock Status */
      DisplayDumpClocks();    
    
      /* Turn on the display */
      ePowerState  = EFI_DISPLAY_POWER_STATE_ON;    
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->DisplayPanelPowerControl(eDisplayType, ePowerState)))
      {
        AsciiPrint ("DisplayPanelPowerStatusTest: DisplayPanelPowerControl() failed.\n");
        return eStatus;
      }
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->DisplayPanelPowerStatus(eDisplayType, &uPanelPwrState)))
      {
        AsciiPrint ("DisplayPanelPowerStatusTest: DisplayPanelPowerStatus() failed.\n");
        return eStatus;
      } 
      AsciiPrint ("DisplayPanelPowerStatusTest: Display panel power state - %d\n", uPanelPwrState ? 1 : 0);
      gBS->Stall(DEFAULT_LONG_WAIT);
    
      /* Dump Clock Status */
      DisplayDumpClocks();    
    }
  }
  return eStatus;
}

/* BLT operation test */
static EFI_STATUS DisplayBltOperationTest(void)
{
  EFI_STATUS                     eStatus        = EFI_SUCCESS;
  EFI_GUID                       sGfxOutputGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *pGfxProtocol;
  
  if (EFI_SUCCESS != (eStatus = gBS->LocateProtocol (&sGfxOutputGUID, 
                                                     NULL, 
                                                     (VOID **)&pGfxProtocol)))                     
  {
    AsciiPrint ("DisplayBltOperationTest: LocateProtocol() failed.\n");
    return eStatus;
  }
  else
  {
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *pModeInfo;
    UINTN                                   nModeInfoSize;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL           bgPixel;
    UINTN                                   centerX;
    UINTN                                   centerY;
    UINT32                                  mode = 0;
    
    if (EFI_SUCCESS != (eStatus = pGfxProtocol->SetMode(pGfxProtocol, mode)))
    {
      AsciiPrint ("DisplayBltOperationTest: SetMode() failed.\n");
      return eStatus;
    }
    AsciiPrint ("DisplayBltOperationTest: Set mode - %d\n", mode);
    if (EFI_SUCCESS != (eStatus = pGfxProtocol->QueryMode(pGfxProtocol, 0, &nModeInfoSize, &pModeInfo)))
    {
      AsciiPrint ("DisplayBltOperationTest: QueryMode() failed.\n");
      return eStatus;
    }
    AsciiPrint ("DisplayBltOperationTest: Query mode - %dx%d\n", pModeInfo->HorizontalResolution, pModeInfo->VerticalResolution);
    /* Fill background as Blue */
    bgPixel.Blue     = BACKGROUND_B;
    bgPixel.Green    = BACKGROUND_G;
    bgPixel.Red      = BACKGROUND_R;
    bgPixel.Reserved = BACKGROUND_A;
    
    AsciiPrint ("DisplayBltOperationTest: Blt(EfiBltVideoFill)\n");
    if (EFI_SUCCESS != (eStatus = pGfxProtocol->Blt(pGfxProtocol, 
                                                    (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)&bgPixel,
                                                    EfiBltVideoFill,
                                                    0, 0,
                                                    0, 0, 
                                                    pModeInfo->HorizontalResolution,
                                                    pModeInfo->VerticalResolution,
                                                    0)))
    {
      AsciiPrint ("DisplayBltOperationTest: Blt(EfiBltVideoFill) failed.\n");
      return eStatus;
    }
    gBS->Stall(DEFAULT_LONG_WAIT);
    /* Copy image to center */
    centerX = (pModeInfo->HorizontalResolution>>1) - (BITMAP_IMAGE_WIDTH>>1);
    centerY = (pModeInfo->VerticalResolution>>1) - (BITMAP_IMAGE_HEIGHT>>1);
    AsciiPrint ("DisplayBltOperationTest: Blt(EfiBltBufferToVideo)\n");
    if (EFI_SUCCESS != (eStatus = pGfxProtocol->Blt(pGfxProtocol, 
                                                    (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)pBitmapImage,
                                                    EfiBltBufferToVideo,
                                                    0, 0,
                                                    centerX, centerY,
                                                    BITMAP_IMAGE_WIDTH,
                                                    BITMAP_IMAGE_HEIGHT,
                                                    0)))
    {
      AsciiPrint ("DisplayBltOperationTest: Blt(EfiBltBufferToVideo) failed.\n");
      return eStatus;
    }
    gBS->Stall(DEFAULT_LONG_WAIT);
    /* Copy image to top left */
    AsciiPrint ("DisplayBltOperationTest: Blt(EfiBltVideoToVideo)\n");
    if (EFI_SUCCESS != (eStatus = pGfxProtocol->Blt(pGfxProtocol, 
                                                    NULL,
                                                    EfiBltVideoToVideo,
                                                    centerX, centerY,
                                                    0, 0,                      
                                                    BITMAP_IMAGE_WIDTH,
                                                    BITMAP_IMAGE_HEIGHT,
                                                    0)))
    {
      AsciiPrint ("DisplayBltOperationTest: Blt(EfiBltVideoToVideo) failed.\n");
      return eStatus;
    }
    gBS->Stall(DEFAULT_LONG_WAIT);
  }
  return eStatus; 
}
  
/* Panel power state test */
static EFI_STATUS DisplayPanelPowerStateTest(void)
{
  EFI_STATUS                           eStatus = EFI_SUCCESS;
  EFI_DISPLAY_POWER_PROTOCOL          *pDisplayPwrCtrlProtocol;
  
  if (EFI_SUCCESS != (eStatus = gBS->LocateProtocol (&gEfiDisplayPowerStateProtocolGuid, 
                                                     NULL, 
                                                     (VOID **)&pDisplayPwrCtrlProtocol)))                     
  {
    AsciiPrint ("DisplayPanelPowerStateTest: LocateProtocol() failed.\n");
    return eStatus;
  }
  else
  {
    UINT32                         cntr = 0;
    EFI_DISPLAY_POWER_STATE        ePowerState  = EfiDisplayPowerStateOff;    
    
    for (cntr = 0; cntr < 2; cntr++)
    {
      AsciiPrint ("DisplayPanelPowerStateTest: Display panel power off...\n");
      /* Turn off the display */
      ePowerState  = EfiDisplayPowerStateOff;
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->SetDisplayPowerState(NULL, ePowerState)))
      {
        AsciiPrint ("DisplayPanelPowerStateTest: SetDisplayPowerState() failed.\n");
        return eStatus;
      } 
      /* Retrieve the power status */ 
      ePowerState = EfiDisplayPowerStateUnknown;
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->GetDisplayPowerState(NULL, &ePowerState)))
      {
        AsciiPrint ("DisplayPanelPowerStateTest: GetDisplayPowerState() failed.\n");
        return eStatus;
      } 
      AsciiPrint ("DisplayPanelPowerStateTest: Display panel power status : %d\n", ePowerState);
      gBS->Stall(DEFAULT_LONG_WAIT);
      AsciiPrint ("DisplayPanelPowerStateTest: Display panel power on...\n");
      /* Turn on the display and backlight */
      ePowerState  = EfiDisplayPowerStateMaximum;   
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->SetDisplayPowerState(NULL, ePowerState)))
      {
        AsciiPrint ("DisplayPanelPowerStateTest: SetDisplayPowerState() failed.\n");
        return eStatus;
      }
      /* Retrieve the power status */ 
      ePowerState = EfiDisplayPowerStateUnknown;
      if (EFI_SUCCESS != (eStatus = pDisplayPwrCtrlProtocol->GetDisplayPowerState(NULL, &ePowerState)))
      {
        AsciiPrint ("DisplayPanelPowerStateTest: GetDisplayPowerState() failed.\n");
        return eStatus;
      } 
      AsciiPrint ("DisplayPanelPowerStateTest: Display panel power status : %d\n", ePowerState);
      gBS->Stall(DEFAULT_LONG_WAIT);
    }
  }
  return eStatus;
}

/* DisplaySetEnvVar()
    Set a generic display variable
 */
static EFI_STATUS DisplaySetEnvVar(CHAR16 *pVarname, UINT32 uValue)
{
  EFI_STATUS                          eStatus                = EFI_SUCCESS;
  CHAR8                               VarValue[MAX_STRING]   = "";
  EFI_QCOM_DISPLAY_UTILS_PROTOCOL    *pDisplayUtilsProtocol  = NULL;

  AsciiSPrint (VarValue, sizeof(VarValue), "%x", uValue);

  eStatus = gBS->LocateProtocol(&gQcomDisplayUtilsProtocolGuid, NULL, (void**)&pDisplayUtilsProtocol);
  if ((EFI_SUCCESS !=  eStatus) ||
      (NULL        ==  pDisplayUtilsProtocol))
  {
    DEBUG((EFI_D_WARN, "Locate display utils protocol failed with status: %r\n", eStatus));
  }
  else
  {
    // Set variable value
    eStatus = pDisplayUtilsProtocol->DisplayUtilsSetVariable(pVarname, (UINT8 *)VarValue, AsciiStrLen (VarValue), 0);
  }

  return eStatus;
}


/* DisplayGetEnvVar()
    Get a generic display variable
 */
static EFI_STATUS DisplayGetEnvVar(CHAR16 *pVarname, UINT32 *pValue)
{
  EFI_STATUS                          eStatus                = EFI_SUCCESS;
  CHAR8                               VarValue[MAX_STRING]   = "";
  UINTN                               uStorageSize           = MAX_STRING;
  EFI_QCOM_DISPLAY_UTILS_PROTOCOL    *pDisplayUtilsProtocol  = NULL;

  eStatus = gBS->LocateProtocol(&gQcomDisplayUtilsProtocolGuid, NULL, (void**)&pDisplayUtilsProtocol);
  if ((EFI_SUCCESS !=  eStatus) ||
      (NULL        ==  pDisplayUtilsProtocol))
  {
    DEBUG((EFI_D_WARN, "Locate display utils protocol failed with status: %r\n", eStatus));
  }
  else
  {
    // Get variable value
    eStatus = pDisplayUtilsProtocol->DisplayUtilsGetVariable(pVarname, (UINT8 *)VarValue, &uStorageSize, 0);
  }
  
  if ((EFI_SUCCESS  == eStatus) &&
      (uStorageSize  > 0))
  {
    *pValue = AsciiStrHexToUintn(VarValue);
  }
  else
  {
    *pValue = 0;
  }

  return eStatus;
}


/**
Performance test for Blt operation. 5KB, 100KB and 1MB blocks of data are copied from Buffer to Video, Video to Video and Blt Fill.

**/
static EFI_STATUS DisplayBltPerformanceTest(void)
{
  EFI_STATUS                     eStatus        = EFI_SUCCESS;
  UINT32                         uStartTime;
  UINT32                         uEndTime;
  UINT32                         uTotalFuncTime = 0; 
  UINT32                         perfno;  
  EFI_GUID                       sGfxOutputGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *pGfxProtocol;
  
  if (EFI_SUCCESS != (eStatus = gBS->LocateProtocol (&sGfxOutputGUID, 
                                                     NULL, 
                                                     (VOID **)&pGfxProtocol)))                     
  {
    AsciiPrint ("DisplayBltOperationTest: LocateProtocol() failed.\n");
  }
  else
  {
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *pModeInfo;
    UINTN                                   nModeInfoSize;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL           bgPixel;
    UINT32                                 *BltBuffer = NULL;
    UINTN                                   BufferLen;
    UINT32                                  mode = 0;
    
    if (EFI_SUCCESS != (eStatus = pGfxProtocol->SetMode(pGfxProtocol, mode)))
    {
      AsciiPrint("DisplayBltOperationTest: SetMode() failed.\n");
    }
    else if (EFI_SUCCESS != (eStatus = pGfxProtocol->QueryMode(pGfxProtocol, 0, &nModeInfoSize, &pModeInfo)))
    {
      AsciiPrint("DisplayBltOperationTest: QueryMode() failed.\n");
    }
    else
    {
      UINT32 blocksize[] = {5, 100, 1000};  //3 data blocks of size 5, 100 and 1000 KB.
      UINT32 iter;
      
      for (iter = 0; iter < (sizeof (blocksize)/sizeof (blocksize[0])); iter++)
      {
        UINT32 hori  = (UINT32)pModeInfo->HorizontalResolution;
        UINT32 verti = (blocksize[iter] * 1024) / (hori * 4);
        
        if (verti > pModeInfo->VerticalResolution)
        {
          verti = pModeInfo->VerticalResolution;
        }
        
        /* Fill background as Blue */
        bgPixel.Blue     = BACKGROUND_B;
        bgPixel.Green    = BACKGROUND_G;
        bgPixel.Red      = BACKGROUND_R;
        bgPixel.Reserved = BACKGROUND_A;

        UINT32 loopv;
          
        // Start Time   
        uStartTime = GetTimerCountms();
        for (loopv = 0; loopv < 10000; loopv++)
        {
          if (EFI_SUCCESS != (eStatus = pGfxProtocol->Blt(pGfxProtocol,
                                                          (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)&bgPixel,
                                                           EfiBltVideoFill,
                                                           0, 0,
                                                           0, 0,
                                                           pModeInfo->HorizontalResolution,
                                                           verti,
                                                           0)))
          {
            break;
          }
        }
        if (eStatus == EFI_SUCCESS)
        {
          // End time
          uEndTime = GetTimerCountms();
          //Function execution time(ms)
          uTotalFuncTime = (uEndTime - uStartTime);
          AsciiPrint("Performance numbers for %d KB data block: \n", blocksize[iter]);
          if (uTotalFuncTime == 0)
          {
            AsciiPrint("Total time taken for BltVideoFill(%d KB): 0ms\n", blocksize[iter]);
          }
          else
          {
            perfno = (blocksize[iter] * 1000.0) / (uTotalFuncTime);
            if (perfno >= 1024)
            {
              perfno = perfno / 1024;
              AsciiPrint("BltVideoFill:     %dMB/s \n", perfno);
            }
            else
            {
              AsciiPrint("BltVideoFill:     %dKB/s \n", perfno);
            }
          }
        }
        else
        {
          AsciiPrint("DisplayBltOperationTest: Blt(EfiBltVideoFill) %d KB failed. \n", blocksize[iter]);
          break;
        }
        
        // Buffer to Video
        BufferLen = hori * verti* sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
        eStatus = gBS->AllocatePool(EfiBootServicesData, (BufferLen), (VOID**)&BltBuffer);
        if ((eStatus != EFI_SUCCESS) || (BltBuffer == NULL))
        {
          eStatus = !EFI_SUCCESS;
          break;
        }
        else
        {
          //Zero out the buffer to make it black
          ZeroMem(BltBuffer, (BufferLen));
          // Start Time
          uStartTime = GetTimerCountms();
          for (loopv = 0; loopv < 10000; loopv++)
          {
          if (EFI_SUCCESS != (eStatus = pGfxProtocol->Blt(pGfxProtocol,
                                                           (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)BltBuffer,
                                                            EfiBltBufferToVideo,
                                                            0, 0,
                                                            0, 0,
                                                            pModeInfo->HorizontalResolution,
                                                            verti,
                                                            pModeInfo->HorizontalResolution * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))))
            {
              break;
            }               
          }
          gBS->FreePool (BltBuffer);
          if (EFI_SUCCESS == eStatus)
          {
            // End time
            uEndTime = GetTimerCountms();
            //Function execution time(ms)
            uTotalFuncTime = (uEndTime - uStartTime);
            if (uTotalFuncTime == 0)
            {
              AsciiPrint("Total time taken for BltBufferToVideo(%d KB): 0ms\n", blocksize[iter]);
            }
            else
            {
              perfno = (blocksize[iter] * 1000) / (uTotalFuncTime);
              if (perfno >= 1024)
              {
                perfno = perfno / 1024;
                AsciiPrint("BltBufferToVideo: %dMB/s \n", perfno);
              }
              else
              {
                AsciiPrint("BltBufferToVideo: %dKB/s \n", perfno);
              }
            }
          }  
          else
          {
            AsciiPrint("DisplayBltOperationTest: Blt(EfiBltBufferToVideo) %d KB failed. \n", blocksize[iter]);
            break;
          }           
            
          // Video to Video copy 
          // Start Time
          uStartTime = GetTimerCountms();
          for (loopv = 0; loopv < 10000; loopv++)
          {
            if (EFI_SUCCESS != (eStatus = pGfxProtocol->Blt(pGfxProtocol,NULL,
                                                            EfiBltVideoToVideo,
                                                            0, 0,
                                                            0, 120,
                                                            pModeInfo->HorizontalResolution,
                                                            verti,
                                                            0)))
            {
              break;
            }
          }
          if (EFI_SUCCESS == eStatus)
          {
            // End time
            uEndTime = GetTimerCountms();
            //Function execution time(ms)
            uTotalFuncTime = (UINT32) (uEndTime - uStartTime);
            if (uTotalFuncTime == 0)
            {
              AsciiPrint("Total time taken for BltVideoToVideo(%d KB): 0ms\n", blocksize[iter]);
            }
            else
            {
              perfno = (blocksize[iter] * 1000.0) / (uTotalFuncTime);
              if (perfno >= 1024)
              {
                perfno = perfno / 1024;
                AsciiPrint("BltVideoToVideo:  %dMB/s \n", perfno);
              }
              else
              {
                AsciiPrint("BltVideoToVideo:  %dKB/s \n", perfno);
              }
            }
          }
          else
          {
            AsciiPrint("DisplayBltOperationTest: Blt(EfiBltVideoToVideo) %d KB failed. \n", blocksize[iter]);
          }
        }
      }
    }
  }
  return eStatus; 
}

/* Dummy Power Event handle function needed to create local events for UI_Active/UI_Idle*/
VOID EFIAPI DummyEventHandle( IN EFI_EVENT  Event, IN VOID *Context)
{
  /* Dummy Power Event handle function, do nothing here*/
}
/* Event based display power control test.
*  1. Create UI_Active and UI_Idle events based on the event GUID.
*  2. Signal UI_Idle event, display is expected to be off, all power rails, PLL, clock, GPIO should be turned off
*  3. Signal UI_Acitve event, display is expected to be on, all power rails, PLL, clock, GPIO should be turned on
*  4. Repeat step 2 & 3 one more time
*/
static EFI_STATUS DisplayPowerEventTest(void)
{
  EFI_STATUS    eStatus            = EFI_SUCCESS;
  EFI_GUID      UIActiveEventGuid  = EFI_UI_ACTIVE_EVENT_GUID;
  EFI_GUID      UIIdleEventGuid    = EFI_UI_IDLE_EVENT_GUID;
  EFI_EVENT     UIActiveEvent      = NULL;
  EFI_EVENT     UIIdleEvent        = NULL;


  //Create UI_Acitve/UI_Idle events
  if (EFI_SUCCESS != (eStatus = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, DummyEventHandle, NULL, &UIActiveEventGuid, &UIActiveEvent)))
  {
    AsciiPrint("DisplayPowerEventTest: Failed to create event UI_Active\n");
  }
  else if (EFI_SUCCESS != (eStatus = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, DummyEventHandle, NULL, &UIIdleEventGuid, &UIIdleEvent)))
  {
    AsciiPrint("DisplayPowerEventTest: Failed to create event UI_Idle\n");
  }
  else
  {
    UINT32                         cntr = 0;

    //Loop to turn of and on display multiply times.
    for (cntr = 0; cntr < 2; cntr++)
    {
      /* Turn off the display */
      AsciiPrint("DisplayPowerEventTest: Display panel power off...\n");
      if (EFI_SUCCESS != (eStatus = gBS->SignalEvent(UIIdleEvent)))
      {
        AsciiPrint("DisplayPowerEventTest: failed signal event UI_Idle.\n");
        break;
      }
      gBS->Stall(DEFAULT_LONG_WAIT);

      /* Turn on the display */
      AsciiPrint("DisplayPowerEventTest: Display panel power on...\n");
      if (EFI_SUCCESS != (eStatus = gBS->SignalEvent(UIActiveEvent)))
      {
        AsciiPrint("DisplayPowerEventTest: failed signal event UI_Active.\n");
        break;
      }
      gBS->Stall(DEFAULT_LONG_WAIT);
    }
  }

  //Close Event handles
  if (NULL != UIActiveEvent)
  {
    if (EFI_SUCCESS != (eStatus = gBS->CloseEvent(UIActiveEvent)))
    {
      AsciiPrint("DisplayPowerEventTest: Failed to Close event UI_Active\n");
    }
  }
  if (NULL != UIIdleEvent)
  {
    if (EFI_SUCCESS != (eStatus = gBS->CloseEvent(UIIdleEvent)))
    {
      AsciiPrint("DisplayPowerEventTest: Failed to Close event UI_Idle\n");
    }
  }

  return eStatus;
}


/* Generate a checkerboard bitmap image
*/
static EFI_STATUS DisplayGenerateBitmap(void)
{
  EFI_STATUS    eStatus            = EFI_SUCCESS;

  eStatus = gBS->AllocatePool(EfiBootServicesData, (BITMAP_IMAGE_WIDTH*BITMAP_IMAGE_HEIGHT*4), (VOID**)&pBitmapImage);
  
  if ((eStatus != EFI_SUCCESS) || (pBitmapImage == NULL))
  {
    eStatus = EFI_OUT_OF_RESOURCES;
  }
  else
  {
      UINT32  pattern[2]   = {0xFF993300, 0xFFFFFFF};
      UINT32  *pData       = (UINT32*)pBitmapImage;
      UINT32  uPatternSize = 4;
      UINT32  x, y;

      
      for (y=0; y<BITMAP_IMAGE_HEIGHT ;y++)
      {
        for (x=0; x<BITMAP_IMAGE_WIDTH; x++)
        {
          *pData++ = pattern[((y>>uPatternSize)+(x>>uPatternSize)) & 1];
        }
      }  
  }

  return eStatus;
}


/* DisplayTestApp Help
*/
static void DisplayTestHelp(void)
{
  UINT32 uCount;
  
  AsciiPrint ("Usage:\n");
  AsciiPrint (" DisplayApp -[option]:[value]\n\n");
  AsciiPrint (" options:\n");  
  AsciiPrint ("   -bvt            : Execute the display BVT test or a specific test number\n");  
  AsciiPrint ("   -disabledisplay : Set or clear the disable display variable\n");
  AsciiPrint ("   -amoled         : Enable or disable Amoled power configuration\n");
  AsciiPrint ("   -panelid        : Set or clear the override panel ID\n\n");
  AsciiPrint ("\nTest Case List:\n");

  for (uCount=0;uCount<NUM_TEST_CASES;uCount++)
  {
    AsciiPrint ("  %d : %a\n", uCount+1, TestCaseList[uCount].sTestName);
  }
  
  AsciiPrint ("\n\n");  
}

/* Extract out XML tag for panel names from panel configuration
*/
static void GetPanelName(CHAR8 *pConfig, uint32 uConfigLength, CHAR8 *pPanelName, uint32 uPanelNameLength)
{
   CHAR8  *pTagStart;
   CHAR8  *pTagEnd;

   ZeroMem(pPanelName, uPanelNameLength);

   if ((NULL != (pTagStart = AsciiStrStr(pConfig, "<PanelDescription>"))) &&
       (NULL != (pTagEnd   = AsciiStrStr(pConfig, "</PanelDescription>"))))
   {
     CHAR8   *pNameStart = pTagStart+AsciiStrLen("<PanelDescription>");
     uint32  uLength     = (pTagEnd-pNameStart);
     uint32  uCopyAmount = (uLength<uPanelNameLength)?uLength:uPanelNameLength;

     AsciiStrnCpy(pPanelName, pNameStart, uCopyAmount);
   }
   else if ((NULL != (pTagStart = AsciiStrStr(pConfig, "<PanelName>"))) &&
            (NULL != (pTagEnd   = AsciiStrStr(pConfig, "</PanelName>"))))
   {
     CHAR8   *pNameStart = pTagStart+AsciiStrLen("<PanelName>");
     uint32  uLength     = (pTagEnd-pNameStart);
     uint32  uCopyAmount = (uLength<uPanelNameLength)?uLength:uPanelNameLength;

     AsciiStrnCpy(pPanelName, pNameStart, uCopyAmount);
   }
   else
   {
     AsciiStrnCpy(pPanelName, "Panel (Unknown configuration)", AsciiStrLen("Panel (Unknown configuration)"));
   }
}


/* DisplayListPanels Help
*/
static UINT32 DisplayListPanels(uint32 uCurrentIndex, UINTN bShow)
{
  EFI_STATUS                        eStatus            = EFI_SUCCESS;
  UINT32                            uNumPanels         = 0;
  EFI_GUID                          sDisplayUtilGUID   = EFI_DISPLAYUTILS_PROTOCOL_GUID;
  EFI_QCOM_DISPLAY_UTILS_PROTOCOL  *pDisplayUtilProtocol;

  if (EFI_SUCCESS != (eStatus = gBS->LocateProtocol(&sDisplayUtilGUID,
                                                     NULL,
                                                     (VOID **)&pDisplayUtilProtocol)))
  {
     AsciiPrint("DisplayListPanels: LocateProtocol() failed.\n");
  }
  else
  {
    EFI_DISPLAY_UTILS_PANEL_CONFIG_PARAMS sPanelConfig;
    UINT32                                uIndex      = 0;
    UINT32                                uParamSize  = sizeof(sPanelConfig);

    if (bShow)
    {
      AsciiPrint ("Available Panel List:\n");
      AsciiPrint ("%2a 0] Default Panel (Auto detect)\n", (uCurrentIndex==0)?"*":"");
    }

    do
    {
      ZeroMem(&sPanelConfig, sizeof(sPanelConfig));
      sPanelConfig.uPanelIndex = uIndex;
      if (EFI_SUCCESS == (eStatus = pDisplayUtilProtocol->DisplayUtilsGetProperty(EFI_DISPLAY_UTILS_PANEL_CONFIG, (VOID*)&sPanelConfig, &uParamSize)))
      {
         if (bShow)
         {
           char sName[128];

           GetPanelName((CHAR8*)sPanelConfig.pPanelConfig, sPanelConfig.uPanelConfigLength, (CHAR8*)&sName, sizeof(sName));

            AsciiPrint ("%2a %d] %a\n", (uCurrentIndex==(uIndex+1))?"*":"", uIndex+1, sName);
         }

         uIndex++;
         uNumPanels++;
      }
    }  while (eStatus == EFI_SUCCESS);
  }


  if (bShow)
  {
    AsciiPrint ("\n\n");
    AsciiPrint ("* - Current override selection");
    AsciiPrint ("\n\n");
  }

  return uNumPanels;
}

/* DisplayTest Command line parser
 * 
                                0123456789012345
                                 1234567 1234567
 * Parse a string in the format -<Param>:<Value>
*/
static UINTN DisplayParseCommand(CHAR8 *pCommandLine, CHAR8 *pParam, CHAR8 *pValue)
{
  UINTN params = 0;
  UINTN strlength = AsciiStrLen(pCommandLine);

  // Initialize output to null
  pParam[0] = '\0';
  pValue[0] = '\0';

  // If we detect the swich and at least 1 valid character
  if ((pCommandLine[0] == '-') &&
      (pCommandLine[0] != ':') &&
      (pCommandLine[0] != ' ') &&
      (strlength > 2))
  {
    UINTN uCount;

    // At least one param detected
    params++;

    // Find the length of the parameter
    for (uCount = 1; uCount<strlength; uCount++)
    {
      if ((pCommandLine[uCount] == ':') ||
        (pCommandLine[uCount] == ' '))
      {
        break;
      }
    }

    // Copy the paramter
    AsciiStrnCpy(pParam, &pCommandLine[1], uCount - 1);
    pParam[uCount-1] = '\0';

    // Copy the optional value
    if ((pCommandLine[uCount] == ':') &&
      ((uCount + 1)<strlength))
    {
      params++;

      AsciiStrnCpy(pValue, &pCommandLine[uCount + 1], strlength - uCount - 1);
      pValue[strlength-uCount-1] = '\0';
    }
  }

  return params;
}

/* DisplayDumpClocks 
 * - Helper to dump all clocks
 * 
 */
static void DisplayDumpClocks()
{
  uint32 i;
  
  AsciiPrint("%-30a %4a %10a\n", "Clock Name", "State", "Frequency");
  for (i=0;i<NUM_CLOCK_ENTRIES;i++)
  {
     DisplayClockStatus(ClockList[i].sClockName);
  }
}

/* DisplayClockStatus 
 * - Display the status of a clock 
 * 
 */
static void DisplayClockStatus(CHAR8 *pClockName)
{
  EFI_CLOCK_PROTOCOL  *mClockProtocol;
  UINTN                uClockId;
  
  // Grab the clock protocol
  if (NULL == pClockName)
  {
    AsciiPrint("ERROR: Cannot query invalid clock\n");
  }
  else if (EFI_SUCCESS != gBS->LocateProtocol(&gEfiClockProtocolGuid,
                                         NULL,
                                         (VOID **) &mClockProtocol))
  {
    AsciiPrint("ERROR: Failed to read clock %a\n", pClockName);
  }
  else if (EFI_SUCCESS != mClockProtocol->GetClockID(mClockProtocol,  pClockName, &uClockId))
  {
    AsciiPrint("ERROR: %a is not a valid clock\n", pClockName);
  }
  else 
  {
    BOOLEAN bClockOn = FALSE; 
    UINT32  uFreq    = 0;
    
      if (EFI_SUCCESS != mClockProtocol->IsClockOn(mClockProtocol, uClockId, &bClockOn))
    {
         AsciiPrint("ERROR: Failed to query clock status of %a\n", pClockName);    
    }
    
    if (EFI_SUCCESS != mClockProtocol->GetClockFreqHz(mClockProtocol, uClockId, &uFreq))
    {
         AsciiPrint("ERROR: Failed to query clock frequency of %a\n", pClockName);    
    }
    
    AsciiPrint("%-30a %3a    %dHz\n", pClockName, (bClockOn)?"ON":"OFF", (bClockOn)?uFreq:0);  
  }
}

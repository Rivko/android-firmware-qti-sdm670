/** @file WinDsdtUpdateControllerUI.c
   
  Application to launch configurable menu. The menu cfg file can be
  Passed in as param.
  
  Copyright 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     -----------------------------------------------------------
  02/17/2017   vinitb  Added qccdi option related changes
  01/30/2017   vinitb  Added sensors/gpu option related changes
  09/23/2016   ssumit      Initial version
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomUtilsLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EfiWinAcpiUpdate.h>
#include <Protocol/AcpiSystemDescriptionTable.h>

#define ENABLE         TRUE
#define DISABLE        FALSE

/* FeatureString : Name of feature 
   Feature : Type of feature 
*/

EFI_STATUS
featureaction (CONST CHAR8* FeatureString , UINTN Feature)
{
    EFI_STATUS                              Status;
    EFI_INPUT_KEY                           Key;
    static EFI_WIN_ACPI_UPDATE_PROTOCOL     *gWinAcpi = NULL;
    BOOLEAN                                 configuration = FALSE;  

	// Locate Win ACPI update protocol
    Status = gBS->LocateProtocol(
                        &gEfiWinAcpiUpdateProtocolGuid,
                        NULL,
                        (VOID**) &gWinAcpi
                        );

    if (EFI_ERROR (Status)) 
    {
        DEBUG((EFI_D_ERROR, " Failed to Locate WinAcpiUpdate protocol \r\n"));
        return EFI_LOAD_ERROR;
    }

	//  Get the feature configuration
    Status = gWinAcpi->GetFeatureVariable(
                        gWinAcpi,
                        Feature,
                        &configuration
                        );
    
    if (EFI_ERROR (Status)) 
    {
        AsciiPrint("\r\nFailed to get the configuration\n");
        return EFI_PROTOCOL_ERROR;
    }

    if( configuration == TRUE )
    {
        AsciiPrint("\r\n%a is ENABLED \n", FeatureString);
    }
    else
    {
         AsciiPrint("\r\n%a is DISABLED \n", FeatureString);       
    }

    AsciiPrint("\r\nPress [UP] key to Enable, [DOWN] key to Disable, [Other] key NO change\r\n");

    Status = ReadAnyKey (
                    &Key,
                    0
                    );

    if (EFI_ERROR(Status))
    {
        return EFI_DEVICE_ERROR;        
    }

    if (Key.ScanCode == SCAN_UP)
    {
        AsciiPrint("\r\nEnabling: \"%a\"\r\n\r\n", FeatureString);
        configuration = TRUE;
        Status = gWinAcpi->SetFeatureVariable(
                                    gWinAcpi,
                                    Feature,
                                    configuration
                                    );

        if (EFI_ERROR (Status)) 
        {
            AsciiPrint("\r\nFailed to get the configuration\n");
            return EFI_PROTOCOL_ERROR;
        }

        Status = gWinAcpi->CommitFeatureVariable(gWinAcpi);
        if (EFI_ERROR (Status)) 
        {
            AsciiPrint("\r\nFailed to commit the feature configuration\n");
            return EFI_PROTOCOL_ERROR;
        }
        
        AsciiPrint("\r\nEnabled: \"%a\"\r\n\r\n", FeatureString);
    }
    else if (Key.ScanCode == SCAN_DOWN)
    {
        AsciiPrint("\r\nDisabling: \"%a\"\r\n\r\n", FeatureString);
        configuration = FALSE;
        Status = gWinAcpi->SetFeatureVariable(
                                    gWinAcpi,
                                    Feature,
                                    configuration
                                    );
        if (EFI_ERROR (Status)) 
        {
            AsciiPrint("\r\nFailed to get the configuration\n");
            return EFI_PROTOCOL_ERROR;
        }
        
        Status = gWinAcpi->CommitFeatureVariable(gWinAcpi);
        if (EFI_ERROR (Status)) 
        {
            AsciiPrint("\r\nFailed to commit the feature configuration\n");
            return EFI_PROTOCOL_ERROR;
        }

        AsciiPrint("\r\nDisabled: \"%a\"\r\n\r\n", FeatureString);
    }
    else
    {
        AsciiPrint("\r\n\nNot confirmed, exiting.\r\n");
        return EFI_NO_MAPPING;
    }

    return EFI_SUCCESS;
}


/* FeatureString : Type of feature 
   Action : Toggle the value
   Flagp : Feature to enable or disable
*/

EFI_STATUS
QueryFeature (CONST CHAR8* FeatureString , UINTN Feature)
{
    EFI_STATUS                              Status;
    static EFI_WIN_ACPI_UPDATE_PROTOCOL     *gWinAcpi = NULL;
    BOOLEAN                                 configuration = FALSE;  

	// Locate Win ACPI update protocol
    Status = gBS->LocateProtocol(
                        &gEfiWinAcpiUpdateProtocolGuid,
                        NULL,
                        (VOID**) &gWinAcpi
                        );

    if (EFI_ERROR (Status)) 
    {
        DEBUG((EFI_D_ERROR, " Failed to Locate WinAcpiUpdate protocol \r\n"));
        return EFI_LOAD_ERROR;
    }

	//  Get the feature configuration
    Status = gWinAcpi->GetFeatureVariable(
                        gWinAcpi,
                        Feature,
                        &configuration
                        );
    
    if (EFI_ERROR (Status)) 
    {
        AsciiPrint("\r\nFailed to get the configuration\n");
        return EFI_PROTOCOL_ERROR;
    }
    
    if( configuration == TRUE )
    {
        AsciiPrint("\r\n%a is ENABLED \n", FeatureString);
    }
    else
    {
         AsciiPrint("\r\n%a is DISABLED \n", FeatureString);       
    }
    return EFI_SUCCESS;
}

/* FeatureString : Type of feature 
   Action : Toggle the value
   Flagp : Feature to enable or disable
*/

EFI_STATUS
EnableFeature (CONST CHAR8* FeatureString , UINTN Feature)
{
    EFI_STATUS                              Status;
    static EFI_WIN_ACPI_UPDATE_PROTOCOL     *gWinAcpi = NULL;
    BOOLEAN                                 configuration = TRUE;  

	// Locate Win ACPI update protocol
    Status = gBS->LocateProtocol(
                        &gEfiWinAcpiUpdateProtocolGuid,
                        NULL,
                        (VOID**) &gWinAcpi
                        );

    if (EFI_ERROR (Status)) 
    {
        DEBUG((EFI_D_ERROR, " Failed to Locate WinAcpiUpdate protocol \r\n"));
        return EFI_LOAD_ERROR;
    }

        Status = gWinAcpi->SetFeatureVariable(
                                    gWinAcpi,
                                    Feature,
                                    configuration
                                    );

        if (EFI_ERROR (Status)) 
        {
            AsciiPrint("\r\nFailed to get the configuration\n");
            return EFI_PROTOCOL_ERROR;
        }

        Status = gWinAcpi->CommitFeatureVariable(gWinAcpi);
        if (EFI_ERROR (Status)) 
        {
            AsciiPrint("\r\nFailed to commit the feature configuration\n");
            return EFI_PROTOCOL_ERROR;
        }
        
        AsciiPrint("\r\nEnabled: \"%a\"\r\n\r\n", FeatureString);
    return EFI_SUCCESS;
}

/* FeatureString : Type of feature 
   Action : Toggle the value
   Flagp : Feature to enable or disable
*/

EFI_STATUS
DisableFeature (CONST CHAR8* FeatureString , UINTN Feature)
{
    EFI_STATUS                              Status;
    static EFI_WIN_ACPI_UPDATE_PROTOCOL     *gWinAcpi = NULL;
    BOOLEAN                                 configuration = FALSE;  

	// Locate Win ACPI update protocol
    Status = gBS->LocateProtocol(
                        &gEfiWinAcpiUpdateProtocolGuid,
                        NULL,
                        (VOID**) &gWinAcpi
                        );

    if (EFI_ERROR (Status)) 
    {
        DEBUG((EFI_D_ERROR, " Failed to Locate WinAcpiUpdate protocol \r\n"));
        return EFI_LOAD_ERROR;
    }

        Status = gWinAcpi->SetFeatureVariable(
                                    gWinAcpi,
                                    Feature,
                                    configuration
                                    );
        if (EFI_ERROR (Status)) 
        {
            AsciiPrint("\r\nFailed to get the configuration\n");
            return EFI_PROTOCOL_ERROR;
        }
        
        Status = gWinAcpi->CommitFeatureVariable(gWinAcpi);
        if (EFI_ERROR (Status)) 
        {
            AsciiPrint("\r\nFailed to commit the feature configuration\n");
            return EFI_PROTOCOL_ERROR;
        }

        AsciiPrint("\r\nDisabled: \"%a\"\r\n\r\n", FeatureString);
    return EFI_SUCCESS;
}

/**
  Entry point for WinDsdtUpdateControllerUI APP

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS           Erase successfully.
  @retval EFI_DEVICE_ERROR      Failed to program
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory for 
                                the buffers
**/

EFI_STATUS
EFIAPI
ControllerUIMain ( 
                IN EFI_HANDLE         ImageHandle,
                IN EFI_SYSTEM_TABLE   *SystemTable
                )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINTN                       ErrorCode = 0;
  UINTN                       Argc;
  CHAR8                      **Argv, **TempArgv;
  CONST CHAR8*               FeatureString;
  UINTN                      Feature=ENABLE;
    
  Status = GetCmdLineArgs (
                ImageHandle,
                &Argc,
                &Argv
                );

  if (EFI_ERROR(Status))
  {
	AsciiPrint ("Failed to get cmd line args with status 0x%x\r\n", Status);
    return Status;  
  }

  TempArgv = Argv;

  /* Check if the app was launched by shell and the WinDsdtUpdateControllerUI str is actually the
   * WinDsdtUpdateControllerUI app name itself, ie it would be "WinDsdtUpdateControllerUI" or "WinDsdtUpdateControllerUI.efi", then again process
   * to split, this is because Ebl doesn't pass the name of app, but shell does
   * other methods could yeild mixed results (like BDS launching apps via Menu, etc) */
  if (Argc &&
      ((AsciiStriCmp (Argv[0], "WinDsdtUpdateControllerUI") == 0) ||
       (AsciiStriCmp (Argv[0], "WinDsdtUpdateControllerUI.efi") == 0)))
  { 
    --Argc;
    ++Argv;
  }

  /* Now both cmdStr and ArgStr have proper values. If no argument, then we can launch
   * BDS Menu by default */
  if (Argc == 0)
  {
    ErrorCode = LaunchMenu ("DriverList.cfg");
  }

  if (Argc > 0)
  {
	if (AsciiStriCmp ((CHAR8*)Argv[0], "FCamera") == 0)
	{
        FeatureString = "Front Camera";
        Feature= EFI_FEATURE_FRONT_CAMERA;
	}
	else if (AsciiStriCmp ((CHAR8*)Argv[0], "BCamera") == 0)
	{
        FeatureString = "Back Camera";
        Feature= EFI_FEATURE_BACK_CAMERA;
	}
	else if (AsciiStriCmp ((CHAR8*)Argv[0], "BLUETOOTH") == 0)
	{
        FeatureString = "BLUETOOTH";
        Feature= EFI_FEATURE_BT;
	}
	else if (AsciiStriCmp ((CHAR8*)Argv[0], "AUDIO") == 0)
	{
        FeatureString = "AUDIO";
        Feature= EFI_FEATURE_AUDIO;
	}
	else if (AsciiStriCmp ((CHAR8*)Argv[0], "MODEM") == 0)
	{
        FeatureString = "MODEM";
        Feature= EFI_FEATURE_MODEM;
	}
	else if (AsciiStriCmp ((CHAR8*)Argv[0], "USB") == 0)
	{
        FeatureString = "USB";
        Feature= EFI_FEATURE_USB;
	}
	else if (AsciiStriCmp((CHAR8*)Argv[0], "WLAN") == 0)
	{
		FeatureString = "WLAN";
		Feature = EFI_FEATURE_WLAN;
	}
	else if (AsciiStriCmp((CHAR8*)Argv[0], "SENSORS") == 0)
	{
		FeatureString = "SENSORS";
		Feature = EFI_FEATURE_SENSORS;
	}
	else if (AsciiStriCmp((CHAR8*)Argv[0], "GRAPHICS") == 0)
	{
		FeatureString = "GRAPHICS";
		Feature = EFI_FEATURE_GPU;
	}
	else if (AsciiStriCmp((CHAR8*)Argv[0], "QCCDI") == 0)
	{
		FeatureString = "QCCDI";
		Feature = EFI_FEATURE_QCCDI;
	}
	else
	{
        AsciiPrint ("Improper argument\r\n");
		return EFI_SUCCESS;	
	}

    if (Argc == 1)
    {
        if (featureaction (FeatureString,Feature ) != EFI_SUCCESS)
        {
            AsciiPrint ("Failed to get Feature setting\r\n");
            return EFI_SUCCESS;	
        }
    }
    else if (Argc == 2)
    {
        if (AsciiStriCmp ((CHAR8*)Argv[1], "ENABLE") == 0)
        {
            if (EnableFeature (FeatureString,Feature ) != EFI_SUCCESS)
            {
                AsciiPrint ("Failed to get Feature setting\r\n");
                return EFI_SUCCESS;	
            }
        }

        else if (AsciiStriCmp ((CHAR8*)Argv[1], "DISABLE") == 0 )
        {
            if (DisableFeature (FeatureString,Feature ) != EFI_SUCCESS)
            {
                AsciiPrint ("Failed to get Feature setting\r\n");
                return EFI_SUCCESS;	
            }         
        }

        else if ( ( AsciiStriCmp ((CHAR8*)Argv[1], "QUERY") == 0 ))
        {
            if (QueryFeature (FeatureString,Feature ) != EFI_SUCCESS)
            {
                AsciiPrint ("Failed to get Feature setting\r\n");
                return EFI_SUCCESS;	
            }            
        }
    }
    else
    {
        AsciiPrint ("Improper arguments passed \r\n");
    }

  }

  if (TempArgv)
    FreePool (TempArgv);

  return Status;
}

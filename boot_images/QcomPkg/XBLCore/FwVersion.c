/** @file FwVersion.c

  Prepares firmware version information

  Copyright (c) 2015-2017,2018 Qualcomm Technologies, Inc.
  All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 01/27/17   ai       Fix use of retail
 10/18/16   bh       Fix KW errors
 03/28/16   bh       Fix bug when img version is not set
 12/10/15   bh       Fix KW errors + bugs
 10/21/15   vk       Align prints
 09/30/15   bh       Initial revision
=============================================================================*/

#include <Library/HobLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/DebugLib.h>
#include <Library/BootConfig.h>
#include <Library/QcomBaseLib.h>
#include <BuildVersion.h>
#include <image_version.h>
#include "FwVersion.h"

extern EFI_GUID gQcomProdmodeInfoGuid;

#define MAX_VERSION_STR_LENGTH    128

#define MAX_RELEASE_DATE_STR_LENGTH 11

#define CHAR_TO_INT(c1, c2, c3, c4) \
  (c1 | c2 << 8 | c3 << 16 | c4 << 24)

#define JAN CHAR_TO_INT('J', 'a', 'n', ' ')
#define FEB CHAR_TO_INT('F', 'e', 'b', ' ')
#define MAR CHAR_TO_INT('M', 'a', 'r', ' ')
#define APR CHAR_TO_INT('A', 'p', 'r', ' ')
#define MAY CHAR_TO_INT('M', 'a', 'y', ' ')
#define JUN CHAR_TO_INT('J', 'u', 'n', ' ')
#define JUL CHAR_TO_INT('J', 'u', 'l', ' ')
#define AUG CHAR_TO_INT('A', 'u', 'g', ' ')
#define SEP CHAR_TO_INT('S', 'e', 'p', ' ')
#define OCT CHAR_TO_INT('O', 'c', 't', ' ')
#define NOV CHAR_TO_INT('N', 'o', 'v', ' ')
#define DEC CHAR_TO_INT('D', 'e', 'c', ' ')

extern EFI_GUID gQcomFwVersionStringGuid;
extern EFI_GUID gQcomFwVersionHexGuid;
extern EFI_GUID gQcomRelDateStringGuid;

CHAR8 *BuildVersionNum = UEFI_BUILD_VERSION;

EFI_STATUS
GetImageVersionString
(
  CHAR8* ImgVerStr
)
{
  CHAR8* StrPtr = QC_IMAGE_VERSION_STRING_AUTO_UPDATED;
  if (StrPtr == NULL)
    return EFI_NOT_FOUND;

  while (*StrPtr != '=')
    StrPtr++;

  StrPtr++;

  CopyMem(ImgVerStr, StrPtr, IMAGE_QC_VERSION_STRING_LENGTH);

  return EFI_SUCCESS;
}

VOID
DisplayEarlyInfo(VOID)
{
  EFI_STATUS Status;
  CHAR8 FinalVersionString[MAX_VERSION_STR_LENGTH];
  CHAR8 ImgVerAsciiStr[IMAGE_QC_VERSION_STRING_LENGTH];
  CHAR8 RelDateString[MAX_RELEASE_DATE_STR_LENGTH];
  UINTN FileNameBuffLen = sizeof (ImgVerAsciiStr);
  CHAR16 *VerStr = NULL;
  CHAR8 *DateString = __DATE__;
  UINTN i = 0;
  UINTN index = 0;
  UINTN Version;
  UINTN CharsToCopy = 0;
  UINTN Length = 0;

  ZeroMem((VOID *) FinalVersionString, sizeof(FinalVersionString));

  Status = GetConfigString("PlatConfigFileName", ImgVerAsciiStr, &FileNameBuffLen);
  if (EFI_ERROR(Status) || (FileNameBuffLen == 0))
    DEBUG ((EFI_D_ERROR, "Error reading config file name\n"));
  else
  {
    if ((ImgVerAsciiStr != NULL) && (FileNameBuffLen != 0))
      DEBUG ((EFI_D_WARN, "CONF File   : %a\n", ImgVerAsciiStr));
  }

  VerStr = FixedPcdGetPtr(PcdFirmwareVersionString);
  
  if(VerStr != NULL)
  {
   Length = StrLen(VerStr); 
	if((Length > 0) && (Length < MAX_VERSION_STR_LENGTH))
	{    
		while((i <= Length ) && (i < (MAX_VERSION_STR_LENGTH - 1)) && (*VerStr != 0))
		{
		  FinalVersionString[i] = (CHAR8) *VerStr;
		  VerStr++;
		  i++;
		}
	}
  }

  
  // Append Date string
  if ((i+7) >= (MAX_VERSION_STR_LENGTH -1))
  {
    DEBUG ((EFI_D_ERROR, "UEFI Ver    : Invalid Length\n", FinalVersionString));
    return;
  }
  
  Version = (UINTN)(FinalVersionString[0] - '0') << (7UL * 4UL);
  Version |= (UINTN)(FinalVersionString[2] - '0') << (6UL * 4UL);

  FinalVersionString[i++] = '.';

  FinalVersionString[i++] = DateString[9];
  FinalVersionString[i++] = DateString[10];
  

  Version |= (UINTN)(FinalVersionString[i-2] - '0') << (5UL * 4UL);
  Version |= (UINTN)(FinalVersionString[i-1] - '0') << (4UL * 4UL);
  
  switch (CHAR_TO_INT(DateString[0], DateString[1], DateString[2], DateString[3]))
  {
    case JAN:
      FinalVersionString[i++] = '0'; FinalVersionString[i++] = '1'; break;
    case FEB:
      FinalVersionString[i++] = '0'; FinalVersionString[i++] = '2'; break;
    case MAR:
      FinalVersionString[i++] = '0'; FinalVersionString[i++] = '3'; break;
    case APR:
      FinalVersionString[i++] = '0'; FinalVersionString[i++] = '4'; break;
    case MAY:
      FinalVersionString[i++] = '0'; FinalVersionString[i++] = '5'; break;
    case JUN:
      FinalVersionString[i++] = '0'; FinalVersionString[i++] = '6'; break;
    case JUL:
      FinalVersionString[i++] = '0'; FinalVersionString[i++] = '7'; break;
    case AUG:
      FinalVersionString[i++] = '0'; FinalVersionString[i++] = '8'; break;
    case SEP:
      FinalVersionString[i++] = '0'; FinalVersionString[i++] = '9'; break;
    case OCT:
      FinalVersionString[i++] = '1'; FinalVersionString[i++] = '0'; break;
    case NOV:
      FinalVersionString[i++] = '1'; FinalVersionString[i++] = '1'; break;
    case DEC:
      FinalVersionString[i++] = '1'; FinalVersionString[i++] = '2'; break;
  }
  
  // fill in the RelDateString with month
  RelDateString[index++] = FinalVersionString[i-2];
  RelDateString[index++] = FinalVersionString[i-1];
  RelDateString[index++] = '/';
  // fill in the RelDateString with date
  RelDateString[index++] = DateString[4];
  RelDateString[index++] = DateString[5];
  RelDateString[index++] = '/';
  // fill in the RelDateString with year
  RelDateString[index++] = DateString[7];
  RelDateString[index++] = DateString[8];
  RelDateString[index++] = DateString[9];
  RelDateString[index++] = DateString[10];
  RelDateString[index] = 0;
  
  Version |= (UINTN)(FinalVersionString[i-2] - '0') << (3UL * 4UL);
  Version |= (UINTN)(FinalVersionString[i-1] - '0') << (2UL * 4UL);
  

  // append date to make the version string
  if (DateString[4] == ' ')
  {
    FinalVersionString[i++] = '0';
  }
  else
  {
    FinalVersionString[i++] = DateString[4];
  }
  FinalVersionString[i++] = DateString[5];

  // Append date to Version
  Version |= (UINTN)(FinalVersionString[i-2] - '0') << (1UL * 4UL);
  Version |= (UINTN)(FinalVersionString[i-1] - '0');
  
  
  // Null terminate the FinalVersionString
  //FinalVersionString[i++] = 0;
  FinalVersionString[i++] = '.';

  // Append Image version string component
  Status = GetImageVersionString(ImgVerAsciiStr);
  if (Status == EFI_SUCCESS)
  {
    CharsToCopy = AsciiStrLen(ImgVerAsciiStr);
    // Check for room to add build version plus NULL terminator
    if ((i + CharsToCopy + 1) > MAX_VERSION_STR_LENGTH)
    {
      CharsToCopy = MAX_VERSION_STR_LENGTH - i - 1;
    }

    index = 0;
    for(index = 0; index < CharsToCopy; index++)
    {
      FinalVersionString[i++] = ImgVerAsciiStr[index];
    }
  }
  else
  {
    DEBUG((EFI_D_WARN, "Image version not found\n"));
  }
 
  // Only append Build Version number if not set to 0
  if (AsciiStrCmp(BuildVersionNum, "0") != 0)
  {
    //FinalVersionString[i++] = '.';
    CharsToCopy = AsciiStrLen(BuildVersionNum);
    // Check for room to add build version plus NULL terminator
    if((i + CharsToCopy + 1) > MAX_VERSION_STR_LENGTH)
    {
      /* Set length of characters to copy to string */
      CharsToCopy = MAX_VERSION_STR_LENGTH - i - 1;
    }
    
    // Add Build Version number
    for(index = 0; index < CharsToCopy; index++)
    {
      FinalVersionString[i++] = BuildVersionNum[index];
    }
  }

  if (i < MAX_VERSION_STR_LENGTH) {
    FinalVersionString[i] = 0;
  } else {
    FinalVersionString[MAX_VERSION_STR_LENGTH-1] = 0;
  }

  DEBUG ((EFI_D_ERROR, "UEFI Ver    : %a\n", FinalVersionString));

  BuildGuidDataHob (&gQcomFwVersionStringGuid, &FinalVersionString, AsciiStrSize(FinalVersionString));
  BuildGuidDataHob (&gQcomFwVersionHexGuid, &Version, sizeof(UINT32));
  BuildGuidDataHob (&gQcomRelDateStringGuid, &RelDateString, AsciiStrSize(RelDateString));

  if(sizeof (UINTN) == 0x8)
    DEBUG ((EFI_D_ERROR, "Build Info  : 64b %a %a\n", __DATE__, __TIME__));
  else
    DEBUG ((EFI_D_ERROR, "Build Info  : 32b %a %a\n", __DATE__, __TIME__));

  if (boot_from_ufs()) 
    DEBUG ((EFI_D_ERROR,   "Boot Device : UFS\n"));
  else if (boot_from_emmc())
    DEBUG ((EFI_D_ERROR,   "Boot Device : eMMC\n"));
  else if (boot_from_spi_nor())
    DEBUG ((EFI_D_ERROR,   "Boot Device : SPI\n"));
  else
  {
     DEBUG ((EFI_D_ERROR, "Unable to determine boot device ...\n"));
     ASSERT (FALSE);
     CpuDeadLoop();
  }

  if (PRODMODE_ENABLED)
    DEBUG ((EFI_D_ERROR, "PROD Mode   : TRUE\n"));
  else
    DEBUG ((EFI_D_ERROR, "PROD Mode   : FALSE\n"));

  if (RETAIL)
    DEBUG ((EFI_D_ERROR, "Retail      : TRUE\n"));
  else
    DEBUG ((EFI_D_ERROR, "Retail      : FALSE\n"));

}



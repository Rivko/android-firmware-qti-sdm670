/** @file
  Implements the Menu Lib.
   
  Copyright (c) 2013-2016, Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/05/16   bh      Update file system call after API change
 01/14/15   bh      Fixed LLVM warning
 10/17/14   tj      KW Fixes
 10/16/14   vk      LoadFileFromPartition signature update
 10/02/14   yg      KW Fixes
 07/15/14   rli     Change FilterChip to use chip name string
 07/07/14   rli     Add support for FilterChip
 11/18/13   ma      Use pwr key to select items in menu
 11/01/13   vk      Fix 64 bit compiler warnings
 07/01/13   yg      Move file as lib
 05/04/13   yg      Formatting cleanup when console is bigger than mode 0
 25/03/13   yg      Num navigation cleanup
 08/03/13   yg      Pick max resolution for menu
 12/10/12   yg      Draw optimization with Num key support
 09/17/12   yg      Use ReadAnyKey API
 08/09/12   yg      Adapt to new GetAnyKey API
 07/18/12   yg      Change to load cfg file from FV as last resort
 07/09/12   yg      Initial version
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/EfiFileLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomUtilsLib.h>
#include <Library/PrintLib.h>
#include <Library/ParserLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/LoadedImage.h>
#include <Library/GuidedFvLib.h>

extern EFI_GUID gToolsFvGuid;

CHAR8  StrBuffer[64];
CHAR8  *PlatformTypePtr =NULL, *ChipNamePtr = NULL, *ChipVerPtr = NULL;
STATIC UINTN FilterChipVotes = 0;

STATIC UINT8 *FreeBufferPtr, *EndPtr;

typedef struct _ArgType ArgType;
typedef struct _MenuItem MenuItem;
STATIC UINT8* HelpString;

/* Command line arguments */
struct _ArgType
{
  UINT8*           Arg;
  ArgType*  Next;
};

/* Needs of a menu item */
struct _MenuItem
{
  UINTN              MenuOffset;
  UINT8*             LabelString;
  EFI_GUID*          FvGuid;
  UINT8*             AppToLaunch;
  UINTN              ArgCount;
  ArgType*           Arguments;
  BOOLEAN            DisableFlag;
  MenuItem*   Next;
};

/* Overall menu context */
typedef struct
{
  UINTN        MenuCount;
  UINTN        SelectedItem;
  UINTN        DefaultSelect;
  UINTN        Indent;
  UINTN        FirstRow;
  UINTN        MaxRows;
  UINTN        MaxCols;
  MenuItem*    CurrentMenuItem;
  ArgType*     CurrentArg;
  MenuItem*    MenuItems;
  CHAR8        MenuLabel[64];
  UINTN        LabelSize;
}MenuContext;

STATIC MenuContext MenuCtx;

STATIC void*
AllocMem (UINTN Size)
{
  UINT8* AllocatedPtr;

  if (Size == 0)
    return NULL;

  if (FreeBufferPtr == NULL)
  {
    if ((FreeBufferPtr = AllocateZeroPool (EFI_PAGE_SIZE)) == NULL)
    {
      DEBUG((EFI_D_WARN, "MemoryAlloc failed\n"));
      return NULL;
    }
    EndPtr = FreeBufferPtr + EFI_PAGE_SIZE;
  }

  Size = (Size + 3) & (~3);
  if (FreeBufferPtr + Size > EndPtr)
  {
    DEBUG((EFI_D_WARN, "MemoryAlloc failed\n"));
    return NULL;
  }

  AllocatedPtr = FreeBufferPtr;
  FreeBufferPtr += Size;
  return AllocatedPtr;
}

/* Menu options */
STATIC VOID
ProcessOptions (UINT8* Key, UINT8* Value)
{
  UINT32 BinVal;

  if (AsciiStriCmp ((CHAR8*)Key, "Indent") == 0)
  {
    if (HexStrToBin (Value, AsciiStrLen((CHAR8*)Value), &BinVal, sizeof(BinVal)))
    {
      if (BinVal <= 12)
        MenuCtx.Indent = BinVal;
    }
  }
  else
    if (AsciiStriCmp ((CHAR8*)Key, "FirstRow") == 0)
    {
      if (HexStrToBin (Value, AsciiStrLen((CHAR8*)Value), &BinVal, sizeof(BinVal)))
      {
        if (BinVal <= 12)
          MenuCtx.FirstRow = BinVal;
      }
    }
    else
      if (AsciiStriCmp ((CHAR8*)Key, "DefaultSelect") == 0)
      {
        if (HexStrToBin (Value, AsciiStrLen((CHAR8*)Value), &BinVal, sizeof(BinVal)))
          MenuCtx.DefaultSelect = BinVal;
      }
}

/* Add Cmd Line argument to the menu item */
STATIC VOID
AddArg (MenuItem *MenuItm, ArgType *Arg)
{
  ArgType *TempPtr;

  if (MenuItm == NULL || Arg == NULL)
    return;

  if (MenuItm->Arguments == NULL)
  {
    MenuItm->Arguments = Arg;
    return;
  }

  TempPtr = MenuItm->Arguments;
  while (TempPtr->Next)
    TempPtr = TempPtr->Next;

  TempPtr->Next = Arg;
}

/* Remove Disabled menu items */
STATIC VOID
RemoveDisabledMenuItems (MenuContext* Menu)
{
  UINTN    MenuOffset;
  MenuItem *TempPtr, *PrevPtr;

  if (Menu == NULL ||  Menu->MenuItems == NULL)
    return;

  TempPtr = Menu->MenuItems;
  MenuOffset = TempPtr->MenuOffset;
  PrevPtr = NULL;
  while (TempPtr)
  {
    if (TempPtr->DisableFlag)
    {
       if (PrevPtr)
       {
         PrevPtr->Next = TempPtr->Next;
       }
       else
       {
         Menu->MenuItems = TempPtr->Next;
       }
       MenuCtx.MenuCount--;
    }
    else
    {
      TempPtr->MenuOffset = MenuOffset++;
      PrevPtr = TempPtr;
    }
    TempPtr = TempPtr->Next;
  }

}

/* Add a New Menu item */
STATIC VOID
AddMenuItem (MenuContext* Menu, MenuItem * MenuItm)
{
  MenuItem *TempPtr;

  if (Menu == NULL || MenuItm == NULL)
    return;

  if (Menu->MenuItems == NULL)
  {
    Menu->MenuItems = MenuItm;
    return;
  }

  TempPtr = Menu->MenuItems;
  while (TempPtr->Next)
    TempPtr = TempPtr->Next;

  TempPtr->Next = MenuItm;
}

STATIC VOID
MenuKeyValCb (UINT8* Section, UINT8* Key, UINT8* Value)
{
  UINTN Sz;
  UINT8* StrPtr;
  STATIC UINTN OptionCbCnt = 0;
  CHAR16 *UniStr;
  DEBUG(( EFI_D_INFO, "Section = \"%s\", Key = \"%s\", Value = \"%s\"\n", Section, Key, Value));

  /* options should be first ones, increase the number to as many options we support */
  if (OptionCbCnt < 5)
  {
    ++OptionCbCnt;

    ProcessOptions (Key, Value);

    if (MenuCtx.MenuLabel[0] == 0)
    {
      AsciiStrCpy (MenuCtx.MenuLabel, (CHAR8*)Section);
      MenuCtx.LabelSize = AsciiStrLen (MenuCtx.MenuLabel);
    }
  }

  Sz = AsciiStrLen ((CHAR8*)Value) + 1;
  StrPtr = AllocMem (Sz);
  if (StrPtr == NULL)
    return;

  AsciiStrnCpy ((CHAR8*)StrPtr, (CHAR8*)Value, Sz);
  UniStr = AllocatePool(2*AsciiStrSize((CHAR8 *)StrPtr));
  if (UniStr == NULL)
    return;
  AsciiStrToUnicodeStr((CHAR8 *)StrPtr, UniStr);

  if (AsciiStriCmp ((CHAR8*)Key, "App") == 0)
  {
    MenuCtx.CurrentMenuItem->AppToLaunch = StrPtr;
    AddMenuItem (&MenuCtx, MenuCtx.CurrentMenuItem);
    MenuCtx.MenuCount++;
    return;
  }

  if ((AsciiStriCmp ((CHAR8*)Key, "Arg") == 0) && MenuCtx.CurrentMenuItem)
  {
    ArgType  *ArgPtr;

    ArgPtr = AllocMem (sizeof(*ArgPtr));
    if (ArgPtr == NULL)
      return;
    ArgPtr->Arg = StrPtr;

    AddArg (MenuCtx.CurrentMenuItem, ArgPtr);
    MenuCtx.CurrentArg = ArgPtr;
    MenuCtx.CurrentMenuItem->ArgCount++;
    return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "Label") == 0)
  {
    /* TODO: Assert on all memory alloc failures */
    MenuCtx.CurrentMenuItem = AllocMem (sizeof(*MenuCtx.CurrentMenuItem));
    if (MenuCtx.CurrentMenuItem == NULL)
      return;
    MenuCtx.CurrentMenuItem->MenuOffset = MenuCtx.MenuCount;
    MenuCtx.CurrentMenuItem->LabelString = StrPtr;
    MenuCtx.CurrentMenuItem->DisableFlag = FALSE;
    FilterChipVotes = 0;
    return;
  }

  if ((AsciiStriCmp((CHAR8 *)Key, "FvGuid") == 0) && MenuCtx.CurrentMenuItem)
  {
    MenuCtx.CurrentMenuItem->FvGuid = AllocMem (sizeof(EFI_GUID));
    if (MenuCtx.CurrentMenuItem->FvGuid == NULL)
      return;
    StrToGuid(UniStr, MenuCtx.CurrentMenuItem->FvGuid);
    return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "MenuHelp") == 0)
  {
    HelpString = StrPtr;
    /* Limit the help string */
    if (AsciiStrLen ((CHAR8*)HelpString) >= (MenuCtx.MaxCols - 2))
      HelpString [MenuCtx.MaxCols - 2] = 0;
    return;
  }

  if ((AsciiStriCmp ((CHAR8*)Key, "FilterChip") == 0) && MenuCtx.CurrentMenuItem)
  {
    if (NULL == ChipNamePtr)
    {
      if (GetPlatformStrings(StrBuffer, sizeof(StrBuffer), &PlatformTypePtr, &ChipNamePtr, &ChipVerPtr) != EFI_SUCCESS)
      {
        return;
      }
    }
    if ( MenuCtx.CurrentMenuItem != NULL)
    {
      if (AsciiStrStr (ChipNamePtr, (CHAR8*)StrPtr) != NULL)
      {
        MenuCtx.CurrentMenuItem->DisableFlag = FALSE;
        FilterChipVotes++;
      }
      else if (FilterChipVotes == 0)
      {
        MenuCtx.CurrentMenuItem->DisableFlag = TRUE;
      }
    }
    return;
  }

}

/* How many lines to backup from bottom for help line */
#define HELP_LINE_BACKUP    2

STATIC VOID
DisplayMenu (MenuContext* Menu)
{
  MenuItem *TempMenuItem;
  UINTN     Row, Col, i;

  AsciiPrint ("\r\n"); // For UART

  gST->ConOut->ClearScreen (gST->ConOut);

  AsciiPrint (" %a:\r\n ", MenuCtx.MenuLabel);

  for (i = 0; i < MenuCtx.LabelSize; ++i)
    AsciiPrint ("-");
  AsciiPrint ("-\r\n");

  /* Leave space for BDS Menu heading */
  Row = 1;

  /* Default select only once */
  if (Menu->DefaultSelect < Menu->MenuCount)
  {
    Menu->SelectedItem = Menu->DefaultSelect;
    Menu->DefaultSelect = Menu->MenuCount;
  }

  TempMenuItem = Menu->MenuItems;

  Row += Menu->FirstRow;

  Col = 2;  // hardcode the column alignment

  while (TempMenuItem)
  {
    /* We could have some sane menu on UART as well */
    AsciiPrint ("\r\n  "); // This includes the above column alignment 2 spaces

    /* For now just limit the number of menu items to as many screen lines */
    if (Row >= (Menu->MaxRows - HELP_LINE_BACKUP))
      break;

    ++Row;

    if (TempMenuItem->MenuOffset == Menu->SelectedItem)
      AsciiPrint ("-> ");
    else
      AsciiPrint ("   ");

    AsciiPrint ("%2d  %a", TempMenuItem->MenuOffset,
                           TempMenuItem->LabelString);

    TempMenuItem = TempMenuItem->Next;
  }

  Row = Menu->MaxRows;
  Col = 2;
  Row -= HELP_LINE_BACKUP;  /* Go back by configured lines */

  AsciiPrint ("\r\n\r\n"); // for UART

  gST->ConOut->SetCursorPosition (gST->ConOut, Col, Row);

  if (HelpString == NULL)
    AsciiPrint ("KeyMap=> Up: Vol+, Down: Vol-, Sel: Camera/Home/Pwr Exit: Esc");
  else
    AsciiPrint ((CHAR8*)HelpString);

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
}

STATIC VOID
LaunchSelectedMenu (MenuContext* Menu)
{
  ArgType  *TempArg;
  MenuItem *TempMenuItem;
  UINTN  i;
  UINTN  Argc = 0;
  UINT8 *Argv[8];

  TempMenuItem = Menu->MenuItems;
  i = Menu->SelectedItem;
  while (i && TempMenuItem)
  {
    TempMenuItem = TempMenuItem->Next;
    --i;
  }

  if (TempMenuItem == NULL)
    return;

  TempArg = TempMenuItem->Arguments;

//  AsciiPrint ("\n\nLaunching %a\n\n", TempMenuItem->LabelString);

  DEBUG(( EFI_D_INFO, "App : %s", TempMenuItem->AppToLaunch));

  /* For now just have single arg, multiple can be supported if needed in future */
  if (1)
  {
    if (TempArg)
      Argv[Argc++] = TempArg->Arg;
    else
      Argv[Argc++] = (UINT8 *) "";
  }
  else
  {
    while (TempArg)
    {
      Argv[Argc++] = TempArg->Arg;
      DEBUG(( EFI_D_INFO, "  %s", TempArg->Arg));
      TempArg = TempArg->Next;
    }
  }

  DEBUG(( EFI_D_INFO, "\n"));

  gST->ConOut->ClearScreen (gST->ConOut);
  
  if (TempMenuItem->FvGuid == NULL)
    TempMenuItem->FvGuid = &gToolsFvGuid;

  //Launch app
  LaunchAppFromGuidedFv(TempMenuItem->FvGuid, 
                        (CHAR8 *)TempMenuItem->AppToLaunch, 
                        (CHAR8 *) Argv[Argc-1]);
}

/* Returns 0 if key is handled and we need to further continue
 * Returns -ve number if we have to exit the menu */
INTN
ProcessKey (UINTN Key)
{
  UINT32 PrevSelected, ReDraw;
  STATIC UINT32 NumSelection = 0;

  PrevSelected = MenuCtx.SelectedItem;
  ReDraw = FALSE;

  switch(Key){
  case SCAN_UP:
    if (MenuCtx.SelectedItem == 0)
      MenuCtx.SelectedItem = MenuCtx.MenuCount - 1;
    else
      MenuCtx.SelectedItem--;
    NumSelection = 0;
    break;

  case SCAN_DOWN:
    if (MenuCtx.SelectedItem >= (MenuCtx.MenuCount - 1))
      MenuCtx.SelectedItem = 0;
    else
      MenuCtx.SelectedItem++;
    NumSelection = 0;
    break;

  case SCAN_SUSPEND:
  case SCAN_HOME:
  case CHAR_LINEFEED:
  case CHAR_CARRIAGE_RETURN:
    /* We should have first item as Exit, so that accidental press will Exit */
    if (MenuCtx.SelectedItem == 0)
      return -1;
    else
    {
      LaunchSelectedMenu (&MenuCtx);
      ReDraw = TRUE;
    }
    NumSelection = 0;
    break;

  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    {
      UINT32 Num = Key - '0';

      if ((NumSelection * 10 + Num) >= MenuCtx.MenuCount)
        NumSelection = Num;
      else
        NumSelection = NumSelection * 10 + Num;

      MenuCtx.SelectedItem = NumSelection;
    }
   break; 
  }

  /* Draw menu only if something changed */
  if ((MenuCtx.SelectedItem != PrevSelected) || (ReDraw == TRUE))
    DisplayMenu (&MenuCtx);

  return 0;
}

//Root Partition
extern EFI_GUID gEfiEmmcGppPartition1Guid;
extern EFI_GUID gEfiEmmcUserPartitionGuid;

//Partition Types
extern EFI_GUID gEfiPlatPartitionTypeGuid;
extern EFI_GUID gEfiSdRemovableGuid;

#define BDS_MENU_CONFIG_FILE           L"BDS_Menu.cfg"
#define BDS_MENU_CONFIG_FILE_IN_FV     "fv1:\\BDS_Menu.cfg"

#define FILE_IN_FV_PREPEND     "fv1:\\"

CHAR16 ConfigFileWChar[64];
CHAR8  ConfigFileinFV[64];

EFI_STATUS
LaunchMenu (CHAR8* MenuFileName OPTIONAL)
{
  UINT32           BlkIOAttrib = 0;
  UINTN            FileSize = 0;
  UINTN            FileSizeFv = 0;
  INTN             Pd;
  UINTN            KeyCode;
  EFI_STATUS       Status;
  UINT8*           FileBuffer;
  EFI_INPUT_KEY    Key;
  UINTN            FailCnt;

  if (MenuFileName == NULL)
    MenuFileName = "BDS_Menu.cfg";

  FreeBufferPtr = NULL;
  EndPtr = NULL;

  SetMem (&MenuCtx, sizeof(MenuCtx), 0);
  HelpString = NULL;

  // Set attributes for fetching Block IO handle
  BlkIOAttrib = BLK_IO_SEL_PARTITIONED_MBR;
  BlkIOAttrib |= BLK_IO_SEL_PARTITIONED_GPT;
  BlkIOAttrib |= BLK_IO_SEL_MEDIA_TYPE_REMOVABLE;
  BlkIOAttrib |= BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM;

  AsciiStrToUnicodeStr (MenuFileName, ConfigFileWChar);

  // Load Menu config file into buffer
  // Attempt to load from removable card if one is existing
  Status = LoadFileFromPartition(BlkIOAttrib,
                                 ConfigFileWChar,
                                 &gEfiSdRemovableGuid,
                                 NULL, NULL,
                                 &FileBuffer, &FileSize);

  // Attempt to load from Internal location if extern not found
  if (Status != EFI_SUCCESS)
  {
    Status = LoadFileFromGuidedFv (&gToolsFvGuid,
                                   MenuFileName, 
                                   &FileBuffer,
                                   &FileSize);

    /* If load from everywhere fails then fall back to loading from FV1 */
    if (Status != EFI_SUCCESS)
    {
      AsciiStrCpy (ConfigFileinFV, FILE_IN_FV_PREPEND);
      AsciiStrCat (ConfigFileinFV, MenuFileName);

      Status = LoadFromFV (ConfigFileinFV, &FileBuffer, &FileSizeFv);
      FileSize =  (UINT32) FileSizeFv;
    }
  }

  if (Status != EFI_SUCCESS)
  {
    AsciiPrint ("Unable to open Menu Config file\r\n");
    return EFI_LOAD_ERROR;
  }

  Pd = OpenParser (FileBuffer, FileSize, NULL);

  if (Pd < 0)
  {
    DEBUG(( EFI_D_WARN, "Parser open failed\n"));
    return EFI_LOAD_ERROR;
  }

  /* Pick resolution of max supported console */
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->MaxMode - 1, &MenuCtx.MaxCols, &MenuCtx.MaxRows);

  EnumKeyValues (Pd, NULL, MenuKeyValCb);

 /* Clean off Menu from filtered items */
  RemoveDisabledMenuItems (&MenuCtx);
  DisplayMenu (&MenuCtx);

  FailCnt = 0;

  /* Wait until the key is released Since its Hotkey, more Complex
   * processing needed to handle USB Keyboard and keep usability */
  while (1)
  {
    /* Wait 250ms before reading */
    gBS->Stall (250000);

    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    if (Status != EFI_SUCCESS)
    {
      if (FailCnt > 2)
        break;
      ++FailCnt;
    }
  }

  while (1)
  {
    if (ReadAnyKey (&Key, 0) != EFI_SUCCESS)
      break;

    KeyCode = Key.ScanCode;
    if (KeyCode == 0)
      KeyCode = Key.UnicodeChar;

    if (KeyCode == CHAR_BACKSPACE || KeyCode == SCAN_ESC)
    {
      AsciiPrint ("\r\n\r\nExiting Menu...\r\n");
      break;
    }

    if (ProcessKey (KeyCode) < 0)
      break;
  }
  gST->ConOut->ClearScreen (gST->ConOut);
  gST->ConIn->Reset(gST->ConIn,FALSE);

  if (FileBuffer)
    FreePool (FileBuffer);

  CloseParser (Pd);

  return EFI_SUCCESS;
}

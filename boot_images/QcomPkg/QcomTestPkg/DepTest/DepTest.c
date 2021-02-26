/** @file DepTest
   
  Tests for Data Execution Prevention (AARCH64)
  DepTest -Alloc Code or -M Data : test memory allocation and free buffer
  DepTest -Map : Check for any EfiLoaderCode not set as XN and RO
  DepTest -X : test XN only for all
  DepTest -R : test RO only for all
  DepTest -I : Image name

  Copyright (c) 2016 Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/26/16   vk      Fix KW
 01/02/16   vk      Initital revision

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <PiDxe.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PeCoffExtraActionLib.h>
#include <Library/PeCoffGetEntryPointLib.h>     
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ArmLib.h>
#include <Guid/DebugImageInfoTable.h> 
#include <Guid/DxeServices.h> 
#include <Library/TestInterface.h>
#include <Library/QcomLib.h>
#include <Protocol/Cpu.h>
#define MAX_IMAGE_SIZE 0x80000

#define AARCH64_NOP_OPCODE 0xAA0003E0
#define AARCH64_RET_OPCODE 0xD65F03C0

#define MAX_IMG_PATH_LEN 256

CHAR8 TestImage[MAX_IMG_PATH_LEN];

typedef enum _TestOptions { Default, TestXn, TestRo, TestAll,TestOneImg, TestAllocCode, TestAllocData } TestOptions;
TestOptions TestOption = Default;

STATIC BOOLEAN
CheckSkipDriverList (CHAR8* DriverName)
{
  UINTN m = 0;
  CHAR8 *SkipList[] = {"DxeCore", "ArmCpuDxe", "ArmGicDxe", "ArmTimerDxe", "Ebl", "DepTest", "SimpleTextInOutSerial"};
  #define NUM_SKIP_LIST (sizeof(SkipList)/sizeof (UINTN))

  for (m = 0; m < NUM_SKIP_LIST; m++)
  {
    if ( AsciiStrStr(DriverName, SkipList[m]) )
      return TRUE;
  }

  return FALSE;
}

EFI_STATUS
LocateDebugTable (EFI_DEBUG_IMAGE_INFO **DebugTablePtr, UINTN *NumDbgImages)
{
  EFI_CONFIGURATION_TABLE           *EfiCfgTblPtr = NULL;
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER *EfiDbgImgInfTbl = NULL;
  UINTN                             NumConfigTblEntries = 0;
  UINTN                             i = 0;

  if ((DebugTablePtr == NULL) || (NumDbgImages == NULL))
    return EFI_INVALID_PARAMETER;

  if (gST == NULL)
    return EFI_UNSUPPORTED;	  

  NumConfigTblEntries = gST->NumberOfTableEntries;
  EfiCfgTblPtr = gST->ConfigurationTable;

  if (EfiCfgTblPtr == NULL)
    return EFI_NOT_FOUND;

  /* Look at all configuration table entries for debug table */
  for (i = 0; i < NumConfigTblEntries; i++)
  {
    if (EfiCfgTblPtr == NULL)
      return EFI_NOT_FOUND;

    if(CompareGuid (&EfiCfgTblPtr->VendorGuid, &gEfiDebugImageInfoTableGuid) == TRUE)
    {
      if (EfiCfgTblPtr->VendorTable)
        EfiDbgImgInfTbl = (EFI_DEBUG_IMAGE_INFO_TABLE_HEADER*) EfiCfgTblPtr->VendorTable;

      if (EfiDbgImgInfTbl != NULL)
        *NumDbgImages = EfiDbgImgInfTbl->TableSize;
    }
    else
    {
      EfiCfgTblPtr++;
    }
  }

  if (EfiDbgImgInfTbl== NULL)
    return EFI_NOT_FOUND;

  if (EfiDbgImgInfTbl->EfiDebugImageInfoTable != NULL)
  {
    *DebugTablePtr = EfiDbgImgInfTbl->EfiDebugImageInfoTable;
     return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
AddNopAndExecBuf(VOID *Base, UINTN Size) 
{
  VOID  (*Func)(VOID);
  UINT32 *p;
  UINTN i = 0;

  if ( (Base == NULL) || (Size == 0) )
    return EFI_INVALID_PARAMETER;

  /* Fill buffer with NOP instruction */
  p = (UINT32*)Base;
  {
    for (i=0; i < Size; i+=sizeof(UINT32))
    {
      *p = AARCH64_NOP_OPCODE;
      p++;
    }
  }

  /* Add last instruction as ret */
  p--;
  *p = AARCH64_RET_OPCODE;

  /* Set function pointer to first instruction */
  Func = (VOID*) Base;
  ArmInvalidateInstructionCache();

  /* Jump to first instruction */
  Func();
	
  return EFI_SUCCESS;
}

EFI_STATUS
TestExecImage (VOID *Base, UINTN Size)
{
  VOID  *BackUp;

  BackUp = (VOID*) AllocateZeroPool (MAX_IMAGE_SIZE);
  if (BackUp == NULL)
    return EFI_OUT_OF_RESOURCES;

  SetMem (BackUp, MAX_IMAGE_SIZE, 0);

  /* Copy image content to backup buffer, replace with NOP and return.
   * Start execution, and on return, replace image data. */
  CopyMem (BackUp, Base, Size);
  AddNopAndExecBuf (Base, Size);
  CopyMem (Base, BackUp, Size);

  return EFI_SUCCESS;
}


EFI_STATUS
ProcessCommand (UINTN Argc, CHAR8** Argv)
{
  CHAR8 *CmdStr, *ArgStr = NULL;

  CmdStr = Argv[0];
  --Argc;

  if (Argc)
    ArgStr = Argv[1];

#if 1
  AsciiPrint ("Processing Cmd: %a", CmdStr);
  if (ArgStr)
     AsciiPrint (", Arg: %a", ArgStr);
  AsciiPrint ("\r\n");
#endif

  SetMem (&TestImage, sizeof (TestImage), 0);

  if (AsciiStriCmp ((CHAR8*)CmdStr, "-X") == 0)
    TestOption = TestXn;

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "-R") == 0)
    TestOption = TestRo;

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "-A") == 0)
    TestOption = TestAll;

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "-Alloc") == 0)
  {
    if ((Argv[1] == NULL) || (Argc < 1))
      return EFI_INVALID_PARAMETER;

    if (AsciiStriCmp ((CHAR8*)Argv[1], "Data") == 0)
      TestOption = TestAllocData;
    else if (AsciiStriCmp ((CHAR8*)Argv[1], "Code") == 0)
      TestOption = TestAllocCode;
    else
      return EFI_INVALID_PARAMETER;
  }

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "-I") == 0)
  {
    TestOption = TestOneImg;
    if ((Argv[1] == NULL) || (Argc < 1))
      return EFI_INVALID_PARAMETER;

    AsciiStrCpy (TestImage, Argv[1]);
    AsciiPrint ("TestImage: %a\n", TestImage);
  }
  	  
  return EFI_SUCCESS;
}

EFI_STATUS
ParseCommandLine (EFI_HANDLE *ImageHandle)
{
  EFI_STATUS Status = EFI_INVALID_PARAMETER;	
  UINTN      Argc = 0;
  CHAR8      **Argv, **TempArgv;

  Status = GetCmdLineArgs (*ImageHandle, &Argc, &Argv);
  if (EFI_ERROR(Status))
    return Status;

  if (Argv != NULL)
  {
    TempArgv = Argv;

    /* Check if the app was launched by shell and the Cmd str is actually the
     * Cmd app name itself, ie it would be "cmd" or "cmd.efi", then again process
     * to split, this is because Ebl doesn't pass the name of app, but shell does
     * other methods could yeild mixed results (like BDS launching apps via Menu, etc) */
    if ((AsciiStriCmp (Argv[0], "DepTest") == 0) ||
        (AsciiStriCmp (Argv[0], "DepTest.efi") == 0))
    {
      --Argc;
      if (Argc == 0)
      {
        FreePool (Argv);
        AsciiPrint ("No Command to process... \r\n");
        return EFI_SUCCESS;
      }
      ++Argv;
    }
  }

  if ((Argc != 0) && (Argv != NULL))
    Status = ProcessCommand (Argc, Argv);

  return Status;
}

EFI_STATUS
TestSingleImage (IN CHAR8* ImageName)
{
  EFI_DEBUG_IMAGE_INFO  *DebugTablePtr = NULL;
  UINTN                 NumDbgImages = 0;	
  EFI_STATUS            Status;	
  CHAR8                 *pName = NULL;
  CHAR8                 *CurrentImage = NULL; 
  VOID                  *Base = NULL;
  UINTN                 Size = 0; 
  UINTN                 i = 0;

  Status = LocateDebugTable (&DebugTablePtr, &NumDbgImages);
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint ("Unable to locate debug table !, %r\n", Status);
    return Status;	  
  }

  if (ImageName == NULL)
    return EFI_INVALID_PARAMETER;	  

  for (i = 0; i < NumDbgImages; i++)
  {
    if ( (DebugTablePtr == NULL) || (DebugTablePtr->NormalImage == NULL) )
      break;

    if ((DebugTablePtr->NormalImage->ImageInfoType == EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL) &&
        (DebugTablePtr->NormalImage->LoadedImageProtocolInstance != NULL))
    {  
      Base = DebugTablePtr->NormalImage->LoadedImageProtocolInstance->ImageBase;

      pName = PeCoffLoaderGetPdbPointer (Base);
      if (pName != NULL)
        CurrentImage = AsciiStrStr(pName, "/DEBUG/") + AsciiStrLen ("/DEBUG/");
      else
	continue;
      
      if (CurrentImage == NULL)
        continue;

      if (AsciiStriCmp (ImageName, CurrentImage) == 0)
      {
        Size = DebugTablePtr->NormalImage->LoadedImageProtocolInstance->ImageSize;
        AsciiPrint ("Base 0x%x Size 0x%x \t Image[0x%02x] %-032a \t", Base, Size, i, CurrentImage);

	if (CheckSkipDriverList (CurrentImage) == TRUE)
        {
          AsciiPrint ("[SKIP]\n");
          break;
        }

        AsciiPrint ("[TEST]\n");
        TestExecImage (Base, Size);
      }
    }

    DebugTablePtr++;
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
TestAllImages (VOID)
{
  EFI_DEBUG_IMAGE_INFO  *DebugTablePtr = NULL;
  UINTN                 NumDbgImages = 0;	
  EFI_STATUS            Status ;	
  CHAR8                 *pName = NULL;
  VOID                  *Base = NULL;
  UINTN                 i = 0;

  Status = LocateDebugTable (&DebugTablePtr, &NumDbgImages);
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint ("Unable to locate debug table !, %r\n", Status);
    return Status;	  
  }
  
  for (i = 0; i < NumDbgImages; i++)
  {
    if ( (DebugTablePtr == NULL) || (DebugTablePtr->NormalImage == NULL) )
      break;

    if ((DebugTablePtr->NormalImage->ImageInfoType == EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL) &&
        (DebugTablePtr->NormalImage->LoadedImageProtocolInstance != NULL))
    {  
      Base = DebugTablePtr->NormalImage->LoadedImageProtocolInstance->ImageBase;

      pName = PeCoffLoaderGetPdbPointer (Base);
      if (pName != NULL)
        pName = AsciiStrStr(pName, "/DEBUG/");

      TestSingleImage (pName+AsciiStrLen("/DEBUG/"));
    }

    DebugTablePtr++;
  }

  return EFI_UNSUPPORTED;
}

STATIC
EFI_STATUS
ClearExecNever (UINT64 Addr, UINT64 Size)
{
  EFI_STATUS Status=EFI_SUCCESS;

  AsciiPrint ("%a():Clear XN, Start = 0x%016lp Size = 0x%016lx ", __FUNCTION__, Addr, Size);
#if 0	
  EFI_CPU_ARCH_PROTOCOL *Cpu;

  Status = gBS->LocateProtocol(&gEfiCpuArchProtocolGuid, NULL, (VOID **)&Cpu);

  Status = Cpu->SetMemoryAttributes (Cpu, Addr, Size, EFI_MEMORY_WB);
#endif  
#if 0
  EFI_DXE_SERVICES *pDS;
 
  EfiGetSystemConfigurationTable(&gEfiDxeServicesTableGuid, (VOID **) &pDS);
  Status = pDS->SetMemorySpaceAttributes(Addr, Size, EFI_MEMORY_WB);

  if (Status != EFI_SUCCESS)
    AsciiPrint (("[Clear]\n"));
  else
    AsciiPrint (("[Fail]\n"));
#endif
  return Status;
}

STATIC
EFI_STATUS
TestAlloc (VOID)
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  EFI_PHYSICAL_ADDRESS AllocAddr = 0;
  VOID *AllocBuf = NULL;
  UINTN AllocSize = 0;

  AllocSize = 1024 * 4;  /* Bytes */ 

  if (gDS == NULL)
    AsciiPrint ("Memory Allocation gDS NULL\n");
	  

  if (TestOption == TestAllocData)
    Status =  gBS->AllocatePages (AllocateAnyPages, EfiBootServicesData, EFI_SIZE_TO_PAGES(AllocSize), &AllocAddr);
  else if (TestOption == TestAllocCode)
    Status =  gBS->AllocatePages (AllocateAnyPages, EfiBootServicesCode, EFI_SIZE_TO_PAGES(AllocSize), &AllocAddr);

  if (Status != EFI_SUCCESS)
    return Status;

  AllocBuf = (VOID*)AllocAddr;

  AsciiPrint ("Memory Allocation at 0x%016lp  Size 0x%016lx\n", AllocBuf, AllocSize);
  SetMem (AllocBuf, AllocSize, 0);       /* Zero fill buffer */

  if (TestOption == TestAllocCode)
    ClearExecNever (AllocAddr, AllocSize);

  Status = AddNopAndExecBuf (AllocBuf, AllocSize);
  if (TestOption == TestAllocData)       /* No return if DEP is OK */
    return EFI_UNSUPPORTED;
  else if (TestOption == TestAllocCode)  /* Return if DEP is OK */
    return EFI_SUCCESS;

  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status = EFI_DEVICE_ERROR;

  TEST_START("DEP");

  Status = ParseCommandLine (&ImageHandle);
  if (Status != EFI_SUCCESS)
    return Status;

  if (TestOption == TestAll)
    Status = TestAllImages ();
  else if (TestOption == TestOneImg)
  {
    AsciiPrint ("Testing single image: %a\n", TestImage);
    if ( (TestImage != NULL) && (AsciiStrLen (TestImage) != 0) )
    {
      AsciiStrCat (TestImage, ".dll");	    
      Status = TestSingleImage (TestImage);
      if (Status == EFI_NOT_FOUND)
      {
        AsciiPrint ("Unable to find image, check image name\n");
      }
    }
  }
  else if ((TestOption == TestAllocCode) || (TestOption == TestAllocData) )
  {
    AsciiPrint ("Testing Memory Allocation\n");
    Status = TestAlloc();
  }

  TestStatus("DEP", Status);
  TEST_STOP("DEP");
  return Status;
}

/** @file VarTest.c
   
  Tests for Variable Storage.

  Copyright (c) 2010-2012,2015-2017, Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/26/17   c_vkuppa Fix for vartest failure
 07/05/16   bh      Update file system call after API change
 03/02/15   vk      LLVM build fix
 29/03/13   vk      Update default cfg location to fv2
 11/02/12   vk      Initial Revision.

=============================================================================*/

#ifdef __ARM_ARCH 
#define ON_TARGET
#endif

#ifdef __arm

#define ON_TARGET

#else //__arm
#if 0 //For KW
#define UEFI_SIMULATOR
#include "UefiSim.h"
#endif
#endif //UEFI_SIMULATOR

#ifdef ON_TARGET
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/EfiFileLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Protocol/BlockIo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/LoadedImage.h>
#include <Library/ParserLib.h>
#include <Protocol/EFIRng.h>
#endif

#include "VarStorageMgr.h"

#define DWORD_ALIGN(x) (ALIGN_VALUE((x), 4))

extern EFI_GUID gEfiPlatPartitionTypeGuid;

typedef struct {
  CHAR8  Guid;
  CHAR16 Name;
  UINT32 NameSz;
  CHAR8  Data;
  UINT32 DataSz;
  CHAR8  ExtData;
  UINTN  ExtDataSz;
} TestRig;

/* Test configuration parameters */
#define RANDOM_TEST

#define MAX_TABLE_SIZE    (11 * 1024) 
#define MAX_VERIFY_ARRAY  50
#define MAX_TEST_ARRAY    50

#define MAX_NAME_SZ        32
#define MAX_DATA_SZ        369
#define MAX_EXT_DATA_SZ    32
#define MAX_GUID_COUNT     5

#define TEMP_BUFFER_SZ     (5*1024)

#define VERIFY_ARRAY_FILE       L"VerifyArray.bin"
#define TEST_ARRAY_FILE         L"TestArray.bin"
#define VARTEST_CONFIG_FILE     L"VarTest.cfg"
#define VARTEST_CONFIG_FILE_FV  "fv2:\\VarTest.cfg"

#define VARIABLE_ATTRIBUTE (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
//#define VARIABLE_ATTRIBUTE (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS)

/* Variables updated from config file data */
STATIC UINT32 MaxTestArray = MAX_TEST_ARRAY;
STATIC UINT32 MaxVerifyArray = MAX_VERIFY_ARRAY;
STATIC UINT32 MaxNameSz = MAX_NAME_SZ;
STATIC UINT32 MaxDataSz = MAX_DATA_SZ;
STATIC UINT32 MaxExtDataSz = MAX_EXT_DATA_SZ;
STATIC UINT32 MaxGuidCount = MAX_GUID_COUNT;
STATIC UINT32 PowerCycle = 0;
STATIC UINT32 FillTestMode = 0;
/* 1 for verify array, 2 for test array, 3 disable both */
STATIC UINT32 PrintArrays = 0; 
STATIC UINT32 DisplayTestConfig = 0;

STATIC EFI_HANDLE   gImageHandleTest;
enum TestModes {IN_TEST_MODE, IN_VERIFY_MODE};
STATIC UINT8   VarTestModeFlag = 0;

STATIC EFI_GUID GuidBuff;
STATIC CHAR16   NameBuff[TEMP_BUFFER_SZ];
STATIC CHAR8    DataBuff[TEMP_BUFFER_SZ];
STATIC CHAR8    ExtDataBuff[TEMP_BUFFER_SZ];

#ifdef RANDOM_TEST
STATIC TestRig  TestArray[MAX_TEST_ARRAY];
#else
STATIC TestRig  TestArray[] = {
 //G   N   NS  D   DS  E    ES 
  {2, 'A', 1, 'a', 15, 'x', 8 }, 
  {2, 'B', 2, 'b', 15, 'x', 12}, 
  {2, 'C', 3, 'c', 15, 'x', 8 }, 

  {2, 'D', 1, 'd', 16, 'x', 8},
  {2, 'E', 2, 'e', 16, 'x', 4 },
  {2, 'F', 3, 'f', 16, 'x', 4 },

  {2, 'G', 1, 'g', 17, 'x', 8},
  {2, 'H', 2, 'h', 17, 'x', 4 },
  {2, 'I', 3, 'i', 17, 'x', 4 },

#if 0
 {2, 'A', 1, 'a', 21, 'e', 8 }, {2, 'G',  2, 'a', 22, 'x', 12},  {2,  'M', 1,  'a', 23, 'e', 8 }, {2, 'S', 2, 'a', 64, 'e', 8} , {1, 'A', 1, 'a', 64, 'e', 4 },
 {3, 'C', 1, 'a', 64, 'e', 8 }, {3, 'H',  2, 'a', 64, 'x', 12},  {1,  'N', 1,  'a', 64, 'e', 8 }, {1, 'A', 2, 'a', 64, 'e', 8} , {3, 'B', 1, 'a', 64, 'e', 4 },
 {2, 'D', 1, 'a', 64, 'e', 8 }, {2, 'I',  2, 'a', 64, 'x', 12},  {3,  'O', 1,  'a', 64, 'e', 8 }, {2, 'B', 2, 'a', 64, 'e', 8} , {1, 'C', 1, 'a', 64, 'e', 4 },
 {2, 'E', 1, 'a', 64, 'e', 8 }, {1, 'J',  2, 'a', 64, 'x', 12},  {1,  'P', 1,  'a', 64, 'e', 8 }, {1, 'C', 2, 'a', 64, 'e', 8} , {2, 'D', 1, 'a', 64, 'e', 4 },
 {1, 'F', 1, 'a', 64, 'e', 8 }, {2, 'K',  1, 'a', 64, 'x', 12},  {2,  'Q', 1,  'a', 64, 'e', 8 }, {3, 'D', 2, 'a', 64, 'e', 8} , {1, 'E', 1, 'a', 64, 'e', 4 },
#endif
};
#endif

STATIC TestRig  VerifyArray[MAX_VERIFY_ARRAY];
STATIC UINT32   VerifyAryPos = 0;

STATIC VOID
PrintVerifyArrRec (CHAR16* String, UINT32 Offset)
{
  if ((PrintArrays) == 0 || (PrintArrays == 1))
   return;
  Print(L"%s", String);
  AsciiPrint (" @%-d\t %-x  %c %-4d\t%c %-4d\n", 
              Offset, VerifyArray[Offset].Guid, 
              VerifyArray[Offset].Name, VerifyArray[Offset].NameSz, 
              VerifyArray[Offset].Data, VerifyArray[Offset].DataSz);
  return;
}

STATIC VOID
PrintTestArrRec (CHAR16* String, UINT32 Offset)
{
  if ((PrintArrays == 0) || (PrintArrays == 2))
   return;
  Print(L"%s", String);
  AsciiPrint (" @%-d\t %-x  %c %-4d\t%c %-4d\n", 
              Offset, TestArray[Offset].Guid, 
              TestArray[Offset].Name, TestArray[Offset].NameSz, 
              TestArray[Offset].Data, TestArray[Offset].DataSz);
  return;
}

STATIC EFI_STATUS
PrintTableStat (CHAR16* String, VOID* Handle)
{
  EFI_STATUS Status;
  UINT64 MaxStorSz = 0, FreeStorSz = 0, MaxVarSz = 0;
  Status = GetTableStat (Handle, &MaxStorSz, &FreeStorSz, &MaxVarSz);
  Print (L"%s MaxSz %lld FreeSz %lld MaxVarSz %lld\n",
              String, MaxStorSz, FreeStorSz, MaxVarSz);
  return Status;
}

STATIC EFI_STATUS
TestWriteFile (CHAR16 *FileName, VOID *Buff, UINTN *WriteCount)
{
  EFI_STATUS Status;
  Status = WriteFile (FileName,
                      NULL,
                      &gEfiPlatPartitionTypeGuid,
                      NULL,
                      TRUE,
                      NULL,
                      WriteCount,
                      0,
                      (UINT8*)Buff,
                      *WriteCount);
  return Status;
}

STATIC EFI_STATUS
TestDeleteFile (CHAR16 *FileName)
{
#ifdef UEFI_SIMULATOR
  system("del *.bin");
  return EFI_SUCCESS;
#else 
  EFI_STATUS Status;

  Status = DeleteFile(FileName,
                    NULL,
                    &gEfiPlatPartitionTypeGuid,
                    NULL,
                    TRUE,
                    NULL
                    );

  if(EFI_ERROR(Status))
      Print(L"TestDeleteFile: Unable to delete %s\n", FileName);

  return Status;
#endif
}

#ifdef ON_TARGET
STATIC 
BOOLEAN RandomGen (UINT8 *pNonce, UINT32 RandomSize)
{
   UINT16 RandomLen = RandomSize;
   EFI_STATUS Status;
   EFI_GUID AlgoID;
   UINTN AlgoIDSize = sizeof(AlgoID);
   STATIC EFI_QCOM_RNG_PROTOCOL *RngProtocol = NULL;

   if (RngProtocol == NULL)
   {
      gBS->LocateProtocol(&gQcomRngProtocolGuid, NULL, (VOID **) &RngProtocol);
      if (RngProtocol == NULL)
         return FALSE;
   }

   Status = RngProtocol->GetInfo(RngProtocol, &AlgoIDSize, &AlgoID);

   if (EFI_SUCCESS != Status)
      return FALSE;

   Status = RngProtocol->GetRNG(RngProtocol, &AlgoID, RandomLen, pNonce);
   
   if (EFI_SUCCESS != Status)
      return FALSE;

   return TRUE;
}
#endif

STATIC UINT32
Rand (VOID)
{
#ifdef ON_TARGET
  STATIC UINT32 RandomVal = 0;
  if (RandomGen (((UINT8*)&RandomVal), sizeof (RandomVal)) == TRUE)
  {
    return RandomVal;
  }
  else
  {
    AsciiPrint ("RNG ERROR\n");
    return 0;
  }
#endif

#ifdef UEFI_SIMULATOR
  return rand ();
#endif
}

STATIC VOID
InitRandom (VOID)
{
#ifdef UEFI_SIMULATOR
STATIC RunOnce = 1;
  if(RunOnce)
  {
    srand ((unsigned)time(NULL));
    RunOnce = 0;
  }
#endif

  return;
}

STATIC CHAR8
GetRandomChar (VOID)
{
  CHAR8 RandomChar = 0;

  while (1)
  {
    RandomChar = ((CHAR8) Rand());
    if (((RandomChar >= 'A') && (RandomChar <= 'Z')) || 
        ((RandomChar >= 'a') && (RandomChar <= 'z')))
      break;
  }
  return RandomChar;
}

STATIC UINT32
GetRandomInt (UINT32 MaxVal, UINT32 AllowZeroEntry)
{
   UINT32 Val = 0;
   while (1)
   {
     Val = Rand () % MaxVal;
     if (Val == 0)
     {
       if(AllowZeroEntry == TRUE)
         return Val;
       else
         continue;
     }
     else
       return Val;
   }
}

STATIC UINT32
InitRandomTestArray (TestRig *Arr, UINT32 NumOfEntries)
{
  UINT32 i = 0;

  InitRandom ();

  for(i = 0; i < NumOfEntries; i++)
  {
    Arr[i].Guid      = (CHAR8) GetRandomInt (MaxGuidCount, FALSE);
    Arr[i].Name      = GetRandomChar ();
    Arr[i].NameSz    = GetRandomInt (MaxNameSz, FALSE);
    Arr[i].Data      = GetRandomChar ();
    Arr[i].DataSz    = GetRandomInt (MaxDataSz, TRUE);
    Arr[i].ExtData   = GetRandomChar ();
    Arr[i].ExtDataSz = GetRandomInt(MaxExtDataSz, TRUE);
    Arr[i].ExtDataSz = DWORD_ALIGN (Arr[i].ExtDataSz);
  }

  return 0;
}

STATIC VOID
RandomizeTestArrayEntries (TestRig *Arr, UINT32 NumOfEntries, UINT32 Step)
{
  UINT32 i = 0;

  InitRandom ();

  if(Step == 0)
    Step = 1;

  //If we reach verify array limit do not add new
  if(NumOfEntries > MaxVerifyArray) 
  {
    AsciiPrint("TM: Verify Array full\n");
    return;
  }

  for (i = 0; i < NumOfEntries; i++)
  {

#if 0  
   //If DataSz is 0 randomize name
   if(Arr[i].DataSz == 0)
   {
     Arr[i].Name = GetRandomChar ();
     Arr[i].NameSz    = GetRandomInt (MaxNameSz, FALSE);
   }
#endif
    Arr[i].DataSz = GetRandomInt(MaxDataSz, TRUE);
    Arr[i].ExtDataSz = GetRandomInt(MaxExtDataSz, TRUE);
    Arr[i].ExtDataSz  = DWORD_ALIGN (Arr[i].ExtDataSz); 
    if(GetRandomInt(10, FALSE) < 5)
      if ((i%Step) == 0)
        Arr[i].DataSz = 0;
    
  }
}

/* Check if variable exists in current verify array */
STATIC INT32
UpdateVerifyAry (CHAR8 Guid, CHAR16 Name, UINT32 NameSz)
{
  UINT32 i;

  for (i=0; i<VerifyAryPos; ++i)
  {
    if ((VerifyArray[i].Guid == Guid) && (VerifyArray[i].Name == Name) && (VerifyArray[i].NameSz == NameSz))
      return i;
  }

  return -1;
}

VOID
ZeroOutTempBuffers(VOID)
{
  SetMem (NameBuff, sizeof(NameBuff), 0);
  SetMem (&GuidBuff, sizeof(GuidBuff), 0);
  SetMem (DataBuff, sizeof(DataBuff), 0);
  SetMem (ExtDataBuff, sizeof(ExtDataBuff), 0);
}

STATIC VOID
SetGuid (EFI_GUID* Guid, CHAR8 d)
{
  SetMem (Guid, sizeof(EFI_GUID), d);
}

STATIC VOID
SetName (CHAR16* Name, UINT32 Sz, CHAR16 d)
{
  UINT32 i;
  for (i=0; i<Sz; ++i)
    Name[i] = d;
  Name[Sz] = 0;
}

STATIC VOID
SetData (CHAR8* Dat, UINT32 Sz, CHAR8 d)
{
  UINT32 i;
  for (i=0; i<Sz; ++i)
    Dat[i] = d;
}

STATIC VOID
SetExtData (CHAR8* ExtDat, UINT32 Sz, CHAR8 e)
{
  UINT32 i;
  for (i=0; i<Sz; ++i)
    ExtDat[i] = e;
}

STATIC VOID 
IncPos (VOID)
{
  if (VerifyAryPos < MaxVerifyArray)
    VerifyAryPos++;
}

STATIC INT32
TestSet (VOID* Handle, CHAR8 GuidFill, CHAR16 NameFill, UINT32 NameSz, 
         CHAR8 DataFill, UINT32 DataSz, CHAR8 ExtDataFill, UINTN ExtDataSz)
{
  EFI_STATUS Status;
  UINT64 InitialMaxStorSz = 0, InitialFreeStorSz = 0, InitialMaxVarSz = 0;
  INT32 VerifyOffset;

  if((NameSz > MaxNameSz) || (DataSz > MaxDataSz) || (ExtDataSz > MaxExtDataSz))
  {
    AsciiPrint("TS: Upsupported Sizes ! Invalid Parameter");
    return -1;
  }

  GetTableStat (Handle, &InitialMaxStorSz, &InitialFreeStorSz, &InitialMaxVarSz);

  VerifyOffset = UpdateVerifyAry (GuidFill, NameFill, NameSz);

  if (VerifyOffset < 0)
  {
    VerifyOffset = VerifyAryPos;
    VerifyArray[VerifyAryPos].Guid = GuidFill;
    VerifyArray[VerifyAryPos].Name = NameFill;
    VerifyArray[VerifyAryPos].NameSz = NameSz;
    VerifyArray[VerifyAryPos].Data = DataFill;
    VerifyArray[VerifyAryPos].DataSz = DataSz;
    VerifyArray[VerifyAryPos].ExtData = ExtDataFill;
    VerifyArray[VerifyAryPos].ExtDataSz = ExtDataSz;
    IncPos ();
  }
  else
  {
    VerifyArray[VerifyOffset].Data = DataFill;
    VerifyArray[VerifyOffset].DataSz = DataSz;
    VerifyArray[VerifyOffset].ExtData = ExtDataFill;
    VerifyArray[VerifyOffset].ExtDataSz = ExtDataSz;
  }

  ZeroOutTempBuffers();

  SetName (NameBuff, NameSz, NameFill);
  SetGuid (&GuidBuff, GuidFill);
  SetData (DataBuff, DataSz, DataFill);
  SetExtData (ExtDataBuff, ExtDataSz, ExtDataFill);

  if (DataSz == 0)
  {
    PrintVerifyArrRec (L"TS: Del", VerifyOffset);
    Status = DeleteVariableEx (Handle, NameBuff, &GuidBuff);
    if (Status == EFI_SUCCESS)
    {
      //AsciiPrint ("TestSet: -delete Variable, VerifyOffset = %d\n", VerifyOffset);
    }
    else
    {
       if ( VerifyArray[VerifyOffset].DataSz != 0 )
         AsciiPrint ("TS: Unable to del Variable, VerifyOffset = %d\n", VerifyOffset);
    }
  }
  else
  {
    PrintVerifyArrRec(L"TS: Set", VerifyOffset);
    Status = SetVariableEx (Handle, NameBuff, &GuidBuff,
                            DataSz, DataBuff, ExtDataBuff, &ExtDataSz);
    if (Status != EFI_SUCCESS)
    {
        PrintVerifyArrRec (L"TS: Unable to add variable ", VerifyOffset);
        PrintTableStat(L"TS: Unable to add", Handle);
        return -1;
    }
  }

  return VerifyOffset;
}

STATIC INT32
VerifyData (UINT32 Offset, CHAR8* DataBuffer, CHAR8 *ExtDataBuffer)
{
  UINT32 i;
  UINT32 Sz = VerifyArray[Offset].DataSz;
  CHAR8  d = VerifyArray[Offset].Data;

  /* Extended Record Cannot be verified using UEFI Application*/ 
#ifdef UEFI_SIMULATOR
  UINT32 ExtSz = VerifyArray[Offset].ExtDataSz;
  CHAR8  e = VerifyArray[Offset].ExtData;
  for (i=0; i<ExtSz; ++i)
  {
    if (ExtDataBuffer[i] != e)
      return -1;
  }
#endif

  /* Compare Data */
  for (i=0; i<Sz; ++i)
  {
    if (DataBuffer[i] != d)
      return -1;
  }

  return 0;
}

/* Check if variable in verify array is present, if TryDeleted is set,
 * check for deleted variables */
STATIC EFI_STATUS
TestGet (VOID *Handle, UINT32 Offset, UINT32 TryDeleted)
{
  EFI_STATUS Status;
  UINTN VarSz = sizeof(DataBuff);
  UINT32 ExtDataSz = sizeof(ExtDataBuff);

  ZeroOutTempBuffers();

  SetName (NameBuff,  VerifyArray[Offset].NameSz, VerifyArray[Offset].Name);
  SetGuid (&GuidBuff,  VerifyArray[Offset].Guid);

  /* Verify Array DataSz is Zero, deleted variable, TryDeleted is false */
  if ((VerifyArray[Offset].DataSz == 0) && (TryDeleted == 0) && (VarTestModeFlag == IN_TEST_MODE))
    return EFI_SUCCESS;

  /* Get the variable, and check if it matches with info in verify array
   * also, check if we find a variable which is deleted per verify array */
  Status = GetVariableEx (Handle, NameBuff, &GuidBuff, &VarSz, &DataBuff, &ExtDataBuff, &ExtDataSz);
  if (Status == EFI_SUCCESS)
  {
    /* print only in verify mode */
    if (VarTestModeFlag == IN_VERIFY_MODE) 
      PrintVerifyArrRec(L"TG: Get", Offset);

    /* read variable, check if data matches with what is in verify array */
    if (VerifyData (Offset, DataBuff, ExtDataBuff) < 0)
    {
      PrintVerifyArrRec (L"TG ERROR: Verify", Offset);
#ifdef UEFI_SIMULATOR
      /* Extended Record Verification is possible on Simulator only */
      AsciiPrint ("TG Got N:%c DS:%-d D:%c ED%c ES%-d\n", 
                    NameBuff[1], VarSz, DataBuff[0], ExtDataBuff[0], ExtDataSz);
#endif
      Status = EFI_LOAD_ERROR;
    }

    /* Chek if we tried to read a deleted variable because TryDeleted is set
     * If we get it, we did not remove it from table, this is error condition */
    if ((VerifyArray[Offset].DataSz == 0) && (TryDeleted == 1))
    {
      PrintVerifyArrRec (L"TG ERROR: GotDelVar", Offset);
      Status = EFI_LOAD_ERROR;
    }
  }
  else
  {
    PrintVerifyArrRec (L"TG: Dl-", Offset);

    /* In verify mode, check for deleted variables, if not found, this is good ! not error */
    if ((VarTestModeFlag == IN_VERIFY_MODE) && (TryDeleted == 1))
      Status = EFI_SUCCESS;
  }

  return Status;
}

/* Enumerate all variables, delete test ones, ie.,
 * with numbered guids. Read and verify these are deleted 
 * Delete Verify and Test Array file, and reset VerifyArray */
STATIC VOID
TestEmptyTable (VOID *Handle)
{
  EFI_STATUS Status = EFI_SUCCESS;
  STATIC CHAR16 VarName[TEMP_BUFFER_SZ];
  UINTN VarSz = sizeof(VarName);
  EFI_GUID TestVendorGuid;
  EFI_GUID VendorGuid;
  UINT8 i = 0;
  UINT32 VarCount = 0;
  UINT32 TempVerifyArrPos = 0;
  SetMem(VarName, sizeof(VarName), 0);
  AsciiPrint("TE: Clean up test variables\n");
  while(Status != EFI_NOT_FOUND)
  {
    VarSz = sizeof(VarName);
    Status =  GetNextVariableNameEx (Handle,
                                     &VarSz,
                                     VarName,
                                     &VendorGuid);
    if(Status == EFI_SUCCESS)
    {
      for(i =0; i < MaxGuidCount; i++)
      {
        SetMem(&TestVendorGuid, sizeof(EFI_GUID), i);
        if(CompareGuid(&TestVendorGuid, &VendorGuid))
        {
          Status = DeleteVariableEx (Handle, VarName, &VendorGuid);
          if(Status == EFI_SUCCESS)
          {
            /* Print(L"TE: Deleted variable: %s, L = %d\n", VarName, VarSz); */
            /* Update verify array */
            TempVerifyArrPos = UpdateVerifyAry(VendorGuid.Data4[0], VarName[0], VarSz);
            if(TempVerifyArrPos != -1)
              VerifyArray[TempVerifyArrPos].DataSz = 0;

            VarCount++;
            SetMem(VarName, sizeof(VarName), 0);
          }
          else
          {
            Print(L"TE: Error deleting variable: %s\n", VarName);
          }
        }
      }
    }
  }
  AsciiPrint("TE: Deleted %d test variables. \n", VarCount);
  
  PrintTableStat(L"TE:", Handle); 

  /* Delete files, since test variables are gone, file is invalid */
  TestDeleteFile(VERIFY_ARRAY_FILE);
  TestDeleteFile(TEST_ARRAY_FILE);
  
  /* Zero out verify array, required if power cycle is disabled */
  if(PowerCycle == 0)
  {
    SetMem(VerifyArray, sizeof(VerifyArray), 0);
    VerifyAryPos = 0;
  }
}


/* Enumerate all variables and print */
STATIC VOID
TestDumpTable (VOID *Handle)
{
  EFI_STATUS Status = EFI_SUCCESS;
  STATIC CHAR16 VarName[TEMP_BUFFER_SZ];
  UINTN VarSz = 0;
  EFI_GUID VendorGuid;
  UINT32 VarCount = 0;

  VarSz = sizeof (VarName);
  SetMem (VarName, sizeof (VarName), 0);

  AsciiPrint ("Dump Table\n");
  AsciiPrint ("----------\n");

  while (Status != EFI_NOT_FOUND)
  {
    VarSz = sizeof (VarName);
    Status =  GetNextVariableNameEx (Handle, &VarSz, VarName, &VendorGuid);
    if(Status == EFI_SUCCESS)
    {
      Print(L"[%d] %s, L = %d\n", VarCount, VarName, VarSz);
      VarCount++;
    }

    if(Status == EFI_NOT_FOUND)
      break;

    if(Status == EFI_BUFFER_TOO_SMALL)
      break;
  }
  AsciiPrint("TD: count = %d variables\n", VarCount);
  PrintTableStat(L"TD:", Handle); 
}


/* Verify current stored variables by comparing with verify array. If TryDeleted
 * flag is set then check entries with Data Size 0 in verify array. */

STATIC EFI_STATUS
VerifyEntries(VOID *Handle, UINT32 TryDeleted)
{
  UINT32 i;
  EFI_STATUS Status = EFI_LOAD_ERROR;

  for (i=0; i<VerifyAryPos; ++i)
  {
    if((Status = TestGet (Handle, i, TryDeleted)) != EFI_SUCCESS)
    {
      return Status;
    }
  }

  return Status;
}

STATIC EFI_STATUS
SetVarTestModeFlag(UINT8 NewFlag)
{
#ifdef ON_TARGET
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize = 0;

  VarSize = sizeof(Flag);

  Status = gRT->GetVariable (L"VarTestMode", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  /* No variable assume test mode, otherwise set as specified */
  if (Status == EFI_NOT_FOUND)
    Flag = IN_TEST_MODE;
  else
    Flag = NewFlag;

  VarSize = sizeof(Flag);

  Status = gRT->SetVariable (L"VarTestMode", &gQcomTokenSpaceGuid,
                             (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                             VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    AsciiPrint("SetVarTestModeFlag: Failed to switch test mode !\n");

  return Status;
#else
  return EFI_SUCCESS;
#endif
}

STATIC UINT8
GetVarTestModeFlag(VOID* Handle)
{
  UINT8         Flag = 0;
#ifdef ON_TARGET
  EFI_STATUS    Status;
  UINTN         VarSize = 0;

  VarSize = sizeof(Flag);

  Status = gRT->GetVariable (L"VarTestMode", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  /* No variable, assume this is first run, 
   * delete all variables withtest guids, 
   * set current run mode as TEST
   * and set variable to verify mode for next run  */
  if (Status == EFI_NOT_FOUND) 
  {
    TestEmptyTable (Handle);
    Flag = IN_TEST_MODE;
    SetVarTestModeFlag(IN_VERIFY_MODE);

    /* Add this app to be launched on next boot
     * For now only works if app is launched from SD card */
    Status = AddDevPathToBootOrder (gImageHandleTest, NULL,
                                    L"Var Test", NULL, 0, 0);
  }
#else
  Flag = VarTestModeFlag;
#endif

  return Flag;
}

EFI_STATUS
SwitchTestMode (UINT8 NewMode)
{
#ifdef ON_TARGET
  EFI_STATUS Status = EFI_SUCCESS;
  Status = SetVarTestModeFlag(NewMode);
  VarTestModeFlag = NewMode;
  
  if(PowerCycle)
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);

  return Status;
#else
  VarTestModeFlag = NewMode;
  return EFI_SUCCESS;
#endif
}

#ifdef ON_TARGET
STATIC VOID
CfgKeyValCb (UINT8* Section, UINT8* Key, UINT8* Value)
{
  /* AsciiPrint ("Key = %a Value = %a\n", Key, Value); */

  if (AsciiStrCmp ((CHAR8*)Key, "MaxTestArray") == 0)
    MaxTestArray = AsciiStrDecimalToUintn ((CHAR8*)Value);

  if (AsciiStrCmp ((CHAR8*)Key, "MaxVerifyArray") == 0)
    MaxVerifyArray = AsciiStrDecimalToUintn((CHAR8*)Value);

  if (AsciiStrCmp((CHAR8*)Key, "MaxDataSize") == 0)
    MaxDataSz = AsciiStrDecimalToUintn((CHAR8*)Value);

  if (AsciiStrCmp((CHAR8*)Key, "MaxNameSize") == 0)
    MaxNameSz = AsciiStrDecimalToUintn((CHAR8*)Value);

  if (AsciiStrCmp((CHAR8*)Key, "MaxExtDataSize") == 0)
    MaxExtDataSz = AsciiStrDecimalToUintn((CHAR8*)Value);

  if (AsciiStrCmp((CHAR8*)Key, "MaxGuidCount") == 0)
    MaxGuidCount = AsciiStrDecimalToUintn((CHAR8*)Value);

  if (AsciiStrCmp((CHAR8*)Key, "PowerCycle") == 0)
    PowerCycle = AsciiStrDecimalToUintn((CHAR8*)Value);

  if (AsciiStrCmp((CHAR8*)Key, "FillTestMode") == 0)
    FillTestMode = AsciiStrDecimalToUintn((CHAR8*)Value);

  if (AsciiStrCmp((CHAR8*)Key, "PrintArrays") == 0)
    PrintArrays = AsciiStrDecimalToUintn((CHAR8*)Value);

  if (AsciiStrCmp((CHAR8*)Key, "DisplayTestConfig") == 0)
    DisplayTestConfig = AsciiStrDecimalToUintn((CHAR8*)Value);

  return;
}

EFI_STATUS
LoadFromFV (IN   CHAR8*    Name, 
            OUT  UINT8**   FileBuffer,
            OUT  UINTN*    FileSize)
{
  EFI_STATUS                  Status;
  EFI_OPEN_FILE               *File;

  if (Name == NULL || FileBuffer == NULL || FileSize == NULL)
    return EFI_INVALID_PARAMETER;

  File = EfiOpen (Name, EFI_FILE_MODE_READ, EFI_SECTION_RAW);
  if (File == NULL)
    return EFI_LOAD_ERROR;
  
  Status = EfiReadAllocatePool (File, (VOID**)FileBuffer, FileSize);

  EfiClose (File);

  return Status;
}

/* Read configuration values from file */
STATIC INT32
SetTestConfig(VOID)
{

  EFI_STATUS Status;
  UINT8      *CfgBuffer = NULL;
  UINTN      FileSize = 0;
  INTN       Pd;
  UINT32     BlkIOAttrib = 0;

  BlkIOAttrib = BLK_IO_SEL_PARTITIONED_MBR; 
  BlkIOAttrib |= BLK_IO_SEL_MEDIA_TYPE_REMOVABLE;
  BlkIOAttrib |= BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM;
 
  Status = LoadFileFromPartition (BlkIOAttrib,
                                  VARTEST_CONFIG_FILE,
                                  (EFI_GUID*)&BlkIOAttrib,
                                  (EFI_GUID*)&BlkIOAttrib,
                                  NULL,
                                  &CfgBuffer, &FileSize);
  if (Status != EFI_SUCCESS)
    Status = LoadFromFV (VARTEST_CONFIG_FILE_FV, &CfgBuffer, &FileSize);

  if (EFI_ERROR(Status) || (CfgBuffer == NULL) || (FileSize == 0) ) 
  {
    AsciiPrint("SetTestConfig: Error loading config file ...\r\n");
    return -1;
  }

  Pd = OpenParser (CfgBuffer, FileSize, NULL);

  if (Pd < 0)
  {
    AsciiPrint ("Parser open failed\r\n");
    return -1;
  }

	EnumKeyValues (Pd, (UINT8*)"Variable Test Parameters", CfgKeyValCb);

  if (DisplayTestConfig)
  {
    AsciiPrint ("\nConfiguration\n-------------\n");
    AsciiPrint ("MaxTestArray %d\nMaxVerifyArray %d\nMaxDataSz %d\nMaxNameSz %d\n"
               "MaxExtDataSz %d\nMaxGuidCount %d\nPowerCycle %d\n",
               MaxTestArray, MaxVerifyArray, MaxDataSz, MaxNameSz, 
               MaxExtDataSz, MaxGuidCount, PowerCycle);
  }

  if ((MaxDataSz > TEMP_BUFFER_SZ) || (MaxNameSz > TEMP_BUFFER_SZ) || 
      (MaxExtDataSz > TEMP_BUFFER_SZ) || (MaxTestArray > MAX_TEST_ARRAY) ||
      (MaxVerifyArray > MAX_VERIFY_ARRAY))
  {
    AsciiPrint ("TestConfig: ERROR, Invalid test configuration, TempBuffSz < TestParam\n");
    return -1;
  }

  CloseParser(Pd);
  return 0;
}
#endif

STATIC INT32
TestInit (VOID** Handle)
{
#ifdef UEFI_SIMULATOR
  UINT8 *p;
  p = (UINT8*) AllocatePool (MAX_TABLE_SIZE);
  SetMem (p, MAX_TABLE_SIZE, '_');
  system("del *.bin");

  InitializeTable (Handle, p, MAX_TABLE_SIZE, (VARIABLE_ATTRIBUTE));

  if (Handle == NULL)
  {
    AsciiPrint ("Error: Unable to Initialize Table!\n");
    return -1;
  }
#else
  STATIC UINT32 DummyHandle = 0x42424242;
  *Handle = &DummyHandle;

  /* On target use config file */
  if (SetTestConfig() < 0)
    return -1;
#endif
  PrintTableStat (L"TI:", *Handle); 

  return 0;
}

/* 1. Load test array if file present,
 * 2. Randomize it and save test array to file
 * 3. Set TestArray variables, read and verify 
 * 4. If verified, save verify array for next run in VERIFY mode
 * 5. If out of space, delete test variables, restart in TEST mode
 * 6. Power cycle */

STATIC EFI_STATUS
RunInTestMode (VOID* Handle)
{
  EFI_STATUS Status;
  UINT64 InitialMaxStorSz = 0, InitialFreeStorSz = 0, InitialMaxVarSz = 0;
  UINT32 TestArrSize = 0;
  INT32 TestOffset = 0;
  UINT32 i = 0;
  STATIC UINTN WriteCount = 0, ReadCount = 0;
  UINT32 FlagError = 0;

  GetTableStat (Handle, &InitialMaxStorSz, &InitialFreeStorSz, &InitialMaxVarSz);

  if ((InitialMaxVarSz == 0) || (InitialFreeStorSz == 0))
  {
    AsciiPrint ("TM ERROR: Not enough space to write initial test variable set.\n");
    AsciiPrint ("TM ERROR: Clean RPMB / BS Partition and rerun test, exiting !\n");
    return EFI_OUT_OF_RESOURCES;
  }

#ifdef RANDOM_TEST
  ReadCount = sizeof(TestArray);
  Status = ReadFile (TEST_ARRAY_FILE, NULL, &gEfiPlatPartitionTypeGuid, NULL, TRUE,
                    NULL, &ReadCount, 0, (UINT8 *)TestArray, ReadCount);
  if (EFI_ERROR(Status) || (ReadCount <= 0))
  {
    AsciiPrint ("TM: Unable to load TestArray file, create new array!\n");
    TestArrSize = MaxTestArray;
    InitRandomTestArray (TestArray, MaxTestArray);
  }
  else
  {
    TestArrSize = ReadCount/sizeof(TestRig);
    AsciiPrint ("TM: Read TestArr File, FSz %d, ArrSz %d\n", 
                ReadCount, TestArrSize);
    RandomizeTestArrayEntries (TestArray, TestArrSize, GetRandomInt(3, TRUE));
  }
#else
  TestArrSize = sizeof(TestArray)/sizeof(*TestArray);
#endif

  for (i=0; i < TestArrSize; i++)
   PrintTestArrRec(L"TA:", i); 

  /* Save test array */
  WriteCount = sizeof(TestRig) * TestArrSize;

  Status = TestWriteFile (TEST_ARRAY_FILE, (VOID *)TestArray, &WriteCount);
  if (EFI_ERROR(Status))
  {
    AsciiPrint ("TM: Failed to save Test Array !\n");
    return EFI_LOAD_ERROR;
  }
  else
    AsciiPrint ("TM: TestArr FSz %d\n", WriteCount);

  /* By now we should have a test array, either loaded from file, 
   * or created new, and randomized, saved to file */
  for (i=0; ((i<TestArrSize) && (FlagError == 0)); i++)
  {
    TestOffset = TestSet (Handle, TestArray[i].Guid,
                          TestArray[i].Name, TestArray[i].NameSz,
                          TestArray[i].Data, TestArray[i].DataSz,
                          TestArray[i].ExtData, TestArray[i].ExtDataSz);
    if (TestOffset == -1)
    {
      AsciiPrint ("TM: SetVariable  failure !\n");
      return EFI_OUT_OF_RESOURCES;
    }

    Status = TestGet (Handle, TestOffset, FALSE);

    if (Status != EFI_SUCCESS)
    {
      AsciiPrint ("TG: Initial TestGet failed, TestArray[%d] \n", i);
      AsciiPrint ("TM: Initial verification failed, offset %d\n", i);
      FlagError = 1;
      return EFI_LOAD_ERROR;
    }

    Status = VerifyEntries(Handle, FALSE);
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint ("TM: Initial verification failed, offset %d\n", i);
      FlagError = 1;
      return EFI_LOAD_ERROR;
    }
  }

  PrintTableStat (L"TM: Init Verify:", Handle);
  AsciiPrint ("TM: Init OK, tested %d \n", VerifyAryPos);

  /* Save verify array */
  WriteCount = sizeof(TestRig) * VerifyAryPos;

  Status = TestWriteFile (VERIFY_ARRAY_FILE, (VOID *)VerifyArray, &WriteCount);
  if (EFI_ERROR(Status))
    AsciiPrint ("TM: Failed to save Verify Array !\n");
  else
    AsciiPrint ("TM: VerifyArr FSz %d, VerifyAryPos %d\n", WriteCount, VerifyAryPos);
  
  /* Switch Mode Flag variable and power cycle */
  Status = SwitchTestMode (IN_VERIFY_MODE);

#ifdef ON_TARGET
  //if we get here while on target, we did not reset, or PowerCycle is disabled !
  if (PowerCycle)
    return EFI_LOAD_ERROR;
  else
    return EFI_SUCCESS;
#else
  //on simulator we just switch mode
  return EFI_SUCCESS;
#endif
}

/* 1. Load Verify Array from file, perform var reads and verify
   2. Back to  TestMode by setting test mode variable and power cycling
*/

STATIC EFI_STATUS
RunInVerifyMode (VOID *Handle)
{
  EFI_STATUS Status;
  UINTN ReadCount = MaxVerifyArray * sizeof (TestRig);

  Status = ReadFile (VERIFY_ARRAY_FILE, NULL, &gEfiPlatPartitionTypeGuid, NULL, TRUE,
                    NULL, &ReadCount, 0, (UINT8 *)VerifyArray, ReadCount);
  if (EFI_ERROR (Status) || (ReadCount <= 0))
  {
    AsciiPrint ("VM: Unable to load verify array, exiting test !\n");
    return EFI_LOAD_ERROR;
  }
  else
  {
    VerifyAryPos = ReadCount / sizeof (TestRig);
    AsciiPrint ("VM: VerifyArr FileSz = %d AryPos = %d \n", 
                ReadCount, VerifyAryPos);
  }

  Status = VerifyEntries (Handle, TRUE);
  if (EFI_ERROR (Status))
    AsciiPrint ("VM: Verification Error !\n");
  else
  {
    /* Switch Mode Flag variable to test mode and power cycle */
    AsciiPrint ("VM: OK\n");
    Status = SwitchTestMode (IN_TEST_MODE);
  }
  return Status;
}

#if 0 //TODO
INT32
VariableGetVariations(VOID* Handle)
{
  /* Set Variable AAAA, Guid 1 */
  /* Set Variable AAAA, Guid 2 */
  /* Get AAAA, Guid 1*/
  /* Get AAAA, Guid 2 */
  /* Get AAA, Guid 1, should fail */
  /* Get AAA, Guid 2, should fail */
  /* Get AAAAA, Guid 1 */
  /* Get AAAAA, Guid 2 */
  
  /* Del Variable AAAA, Guid 1 */
  /* Get AAAA, Guid 1 */
  /* Get AAAA, Guid 2 */
  /* Get AAA, Guid 1 */
  /* Get AAA, Guid 2 */
  /* Get AAAAA, Guid 1 */
  /* Get AAAAA, Guid 2 */

  /* Det Variable AAAA, Guid Y */
  /* Get AAAA, Guid 1 */
  /* Get AAAA, Guid 2 */
  /* Get AAA, Guid 1 */
  /* Get AAA, Guid 2 */
  /* Get AAAAA, Guid 1 */
  /* Get AAAAA, Guid 2 */

  /* Set Variable AAA, Guid 1 */
  /* Set Variable AAA, Guid 2 */
  /* Get AAA, Guid 1 */
  /* Get AAA, Guid 2 */
  /* Get AA, Guid 1 */
  /* Get AA, Guid 2 */
  /* Get AAAA, Guid 1 */
  /* Get AAAA, Guid 2 */
}
#endif

INT32 
FillTest(VOID* Handle)
{
  EFI_STATUS Status;
  STATIC UINT64 MaxStorSz = 0, FreeStorSz = 0, MaxVarSz = 0;
  
  CHAR16 *VarNamePtr = L"A";
  CHAR16 *ExtDataPtr = L"ext";
  
  INT32 TmpVarDataSz = 0;
  INT32 FirstSuccessWrite = 0;
  UINTN TmpExtDataSz = StrSize (ExtDataPtr);
  UINT32 Count = 0;
  UINT8 ErrFlag = 0;
  UINT8 Done = 0;
  Print (L"[Fill]\n");

  /* We look for first succesful write 
   * Then check for range -4 to +4, all should pass
   * Delete Variable, should pass
   * */
  while ((Count != 128) && (ErrFlag == 0) && (Done == 0))
  {
    GetTableStat (Handle, &MaxStorSz, &FreeStorSz, &MaxVarSz);
	MaxStorSz = (MaxStorSz > TEMP_BUFFER_SZ) ? TEMP_BUFFER_SZ : MaxStorSz;
 	FreeStorSz = (FreeStorSz > TEMP_BUFFER_SZ) ? TEMP_BUFFER_SZ : FreeStorSz;
 	MaxVarSz = (MaxVarSz > TEMP_BUFFER_SZ) ? TEMP_BUFFER_SZ : MaxVarSz;

    TmpVarDataSz = (INT32) MaxVarSz - StrSize (VarNamePtr) - StrSize (ExtDataPtr) - Count;

    ZeroOutTempBuffers ();

    CopyMem (NameBuff, VarNamePtr, StrSize (VarNamePtr));
    SetMem (DataBuff, TmpVarDataSz, 'd');
    CopyMem (ExtDataBuff, ExtDataPtr, StrSize (ExtDataPtr));
    SetMem (&GuidBuff, sizeof(EFI_GUID), 0x8);

    /* PrintTableStat (L"FT: BS:", Handle); */
    Status = SetVariableEx (Handle, 
                            NameBuff,
                            &GuidBuff,
                            TmpVarDataSz,
                            DataBuff, 
                            ExtDataBuff,
                            &TmpExtDataSz);
    /* PrintTableStat (L"FT: AS:", Handle); */
    if (Status != EFI_SUCCESS)
    {
      /* Print(L"Error setting var Cnt %d Status 0x%x\n", Count, Status); */
    }
    else
    {
      /* We set variable, now update with +0, -1, -1, -3, -4, +1, +2, +3, +4 */
      INT32 n = -4;
      INT32 FailCount = 0;
      FirstSuccessWrite = TmpVarDataSz;
      Print(L"FSW %d\n", FirstSuccessWrite);
      for(n = -4; ((n != 4) && (ErrFlag != 1)); n++)
      {
        ZeroOutTempBuffers ();

        TmpVarDataSz = FirstSuccessWrite;
        TmpVarDataSz += n;
        Print(L"VarDtSz %d\n", TmpVarDataSz);

        CopyMem (NameBuff, VarNamePtr, StrSize (VarNamePtr));
        SetMem (&GuidBuff, sizeof(EFI_GUID), 0x8);
        SetMem (DataBuff, TmpVarDataSz, 'u');
        CopyMem (ExtDataBuff, ExtDataPtr, StrSize (ExtDataPtr));

        PrintTableStat (L"FT: BS:", Handle);
        Status = SetVariableEx (Handle, 
                                NameBuff,
                                &GuidBuff,
                                TmpVarDataSz,
                                DataBuff, 
                                ExtDataBuff,
                                &TmpExtDataSz);
        if (Status != EFI_SUCCESS)
        {
          /* Keep track of failures, +1, +2, +3, +4 should fail */
          PrintTableStat (L"FT: AS: Fail OK", Handle);
          FailCount++;
          if(FailCount >= 4)
          {
            Print(L"FailCOunt = %d", FailCount);
            ErrFlag = 1;
          }
        }
        else
          PrintTableStat (L"FT: AS:", Handle);
      }

      ZeroOutTempBuffers ();
      CopyMem (NameBuff, VarNamePtr, StrSize (VarNamePtr));
      SetMem (&GuidBuff, sizeof(EFI_GUID), 0x8);

      Status = DeleteVariableEx (Handle, NameBuff, &GuidBuff);
      if (Status != EFI_SUCCESS)
      {
        Print(L"Error del var Cnt %d Status 0x%x\n", Count, Status);
        ErrFlag = 1;
      }
      else
      {
        Done = 1;
        PrintTableStat (L"FT: AD", Handle);
      }
    }

    ZeroOutTempBuffers ();
    Count++;
  }

  if(ErrFlag == 0)
    return 0;
  else
    return -1;
}

EFI_STATUS
VariableSim (CHAR8 *Arg)
{
  EFI_STATUS Status = EFI_LOAD_ERROR;
  STATIC VOID *Handle = NULL;
  UINT32 FlagError = 0;
  UINT32 Count = 0;

  if (TestInit (&Handle) < 0)
  {
    AsciiPrint ("TestInit: ERROR, check parameters !\n");
    return EFI_LOAD_ERROR;
  }

  VarTestModeFlag = GetVarTestModeFlag (Handle);

  if (FillTestMode)
  {
    FlagError = FillTest(Handle);
    if (FlagError)
      return EFI_LOAD_ERROR;
  }

  while (FlagError == 0)
  {
    if (FillTestMode)
    {
      FlagError = FillTest(Handle);
      if (FlagError)
        return EFI_LOAD_ERROR;
    }

    if (VarTestModeFlag == IN_TEST_MODE)
    {
      AsciiPrint ("\n[TEST] %d\n", Count++);
      Status = RunInTestMode (Handle);
      if (EFI_ERROR (Status))
      {
        AsciiPrint ("******************************************\n");
        AsciiPrint ("* VariableSim ERROR: Test  mode failure! *\n");
        AsciiPrint ("******************************************\n");

        //cleanup
        TestDumpTable (Handle);
        //TODO: Dump RPMB here
        TestEmptyTable (Handle);
        TestDumpTable (Handle);

        if (Status != EFI_OUT_OF_RESOURCES)
          FlagError = 1;
        else 
          FlagError = 0;

        Status = SwitchTestMode (IN_TEST_MODE);
      }
    }
    else if (VarTestModeFlag == IN_VERIFY_MODE)
    {
      AsciiPrint ("\n[VERIFY]\n");
      Status = RunInVerifyMode (Handle);
      if (EFI_ERROR (Status))
      {
        AsciiPrint ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
        AsciiPrint ("^ VariableSim ERROR: Verify  mode failure! ^\n");
        AsciiPrint ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

        TestDumpTable (Handle);
        TestEmptyTable (Handle);
        TestDumpTable (Handle);

        FlagError = 1;

        Status = SwitchTestMode (IN_TEST_MODE);
      }
    }
  }
  
  if(FlagError == 1)
    return EFI_VOLUME_CORRUPTED;
  else
    return Status;
}

#ifdef UEFI_SIMULATOR
int main ()
{
  EFI_STATUS Status;

  Status = VariableSim (NULL);
  //Loop here on simulator, will be here on error only
  while (Status != EFI_SUCCESS);
}
#endif

#ifdef ON_TARGET
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
VarTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;

  gImageHandleTest = ImageHandle;
  Status = VariableSim(NULL);
  return Status;
}

#endif

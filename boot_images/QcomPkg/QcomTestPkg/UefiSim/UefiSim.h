#ifndef __UEFI_SIM_H__
#define __UEFI_SIM_H__
#define _CRT_SECURE_NO_WARNINGS
#include "UefiSimTypes.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//#define DWORD_ALIGN(x) (((x) + 3) & (~3))
#define ALIGN_VALUE(Value, Alignment) ((Value) + (((Alignment) - (Value)) & ((Alignment) - 1)))

typedef struct {
  UINT32 Data1;
  UINT16 Data2;
  UINT16 Data3;
  UINT8 Data4[8];
}GUID;


#if defined (__GNUC__)
  #define ALIGN_GCC(x)  __attribute__((aligned(x)))
  #define ALIGN_RVCT(x)
#else
  #define ALIGN_GCC(x)
  #define ALIGN_RVCT(x)  __align(x)
#endif

typedef int (*CRC_FN) (UINT8*, UINT32, UINT32*);

typedef struct
{
  CRC_FN   CalculateCrc32;

}BS;

extern BS* gBS;

#define STATIC static
#define VOID void
#define CONST const

typedef GUID EFI_GUID;
typedef UINT32 EFI_STATUS;

#define EFIAPI
#define IN
#define OUT
#define OPTIONAL

#define ASSERT_EFI_ERROR(x)

#define EFI_SUCCESS                (0)
#define EFI_LOAD_ERROR             (1)
#define EFI_INVALID_PARAMETER      (2)
#define EFI_OUT_OF_RESOURCES       (9)
#define EFI_VOLUME_CORRUPTED       (10)
#define EFI_INCOMPATIBLE_VERSION   (25)
#define EFI_BAD_BUFFER_SIZE        (50)
#define EFI_BUFFER_TOO_SMALL       (60)
#define EFI_NOT_FOUND              (70)
#define EFI_FAILURE                (80)

#define EFI_ERROR(x) (x != EFI_SUCCESS)

/// Attributes of variable.
#define EFI_VARIABLE_NON_VOLATILE                            0x00000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS                      0x00000002
#define EFI_VARIABLE_RUNTIME_ACCESS                          0x00000004

/// Attributes of Authenticated Variable
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS              0x00000010
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS   0x00000020
#define EFI_VARIABLE_APPEND_WRITE                            0x00000040

#define Print wprintf

VOID CopyGuid (GUID* dst, GUID* src);

int CompareGuid (GUID* dst, GUID* src);

int CompareMem (VOID* dst, VOID* src, UINT32 Size);

VOID SetMem (VOID* ptr, UINT32 size, UINT8 val);

VOID CopyMem (VOID* dst, VOID* src, UINT32 size);

VOID* AllocatePool(UINTN AllocationSize);

UINTN EFIAPI StrLen ( IN  CONST CHAR16  *String );
INT32 EFIAPI StrCmp ( IN  CONST CHAR16  *FirstString, IN CONST CHAR16  *SecondString );
INT32 EFIAPI StrnCmp (
  IN      CONST CHAR16              *FirstString,
  IN      CONST CHAR16              *SecondString,
  IN      UINTN                     Length
  );

CHAR16 *
EFIAPI
StrnCpy (
  OUT     CHAR16                    *Destination,
  IN      CONST CHAR16              *Source,
  IN      UINTN                     Length
  );


CHAR8* EFIAPI
UnicodeStrToAsciiStr (
  IN      CONST CHAR16              *Source,
  OUT     CHAR8                     *Destination
  );

INTN
EFIAPI
AsciiStrCmp (
  IN      CONST CHAR8               *FirstString,
  IN      CONST CHAR8               *SecondString
  );


#define FreePool free
#define AsciiPrint printf

EFI_STATUS
WriteFile(
  IN     CHAR16           *Path,
  IN     EFI_GUID         *RootDeviceType,
  IN     EFI_GUID         *PartitionType,
  IN     BOOLEAN          SelectNonRemovable,
  IN     CHAR8            *VolumeName,
  IN OUT UINTN            *BytesToWrite,
  IN     UINT64           WriteOffset,
  OUT    UINT8            *FileBuffer,
  IN     UINT32           FileBufferSize
  );

EFI_STATUS
ReadFile(
  IN     CHAR16           *Path,
  IN     EFI_GUID         *RootDeviceType,
  IN     EFI_GUID         *PartitionType,
  IN     BOOLEAN          SelectNonRemovable,
  IN     CHAR8            *VolumeName,
  IN OUT UINTN            *BytesToRead,
  IN     UINT64           ReadOffset,
  OUT    UINT8            *FileBuffer,
  IN     UINT32           FileBufferSize
  );

EFIAPI
INT32
AsciiStrDecimalToUintn (
  IN      CONST CHAR8  *String
  );

UINTN
EFIAPI
StrSize (
  IN      CONST CHAR16              *String
  );

#endif /* __UEFI_SIM_H__ */

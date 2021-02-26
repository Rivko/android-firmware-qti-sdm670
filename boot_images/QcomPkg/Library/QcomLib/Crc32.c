/** @file
   
  This file implements 32-bit CRC calculations.

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved. 
  Portions Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.
   
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/10/11   niting  Initial revision.

=============================================================================*/

#include <Uefi.h>

#define CRC32_POLYNOMIAL 0x04c11db7
#define CRC32_SEED       0xffffffff

UINT32         mCrcTable[256];
STATIC BOOLEAN CrcInitialized = FALSE;

/**
  This internal function reverses bits for 32-bit data.

  @param  Value                 The data to be reversed.

  @return                       Data reversed.

**/
UINT32
ReverseBits (
  UINT32  Value
  )
{
  UINTN   Index;
  UINT32  NewValue;

  NewValue = 0;
  for (Index = 0; Index < 32; Index++)
  {
    if ((Value & (1 << Index)) != 0)
    {
      NewValue = NewValue | (1 << (31 - Index));
    }
  }

  return NewValue;
}

/**
  Initialize the CRC32 table.

**/
VOID
InitializeCrc32Table ( VOID )
{
  UINTN   TableEntry;
  UINTN   Index;
  UINT32  Value;

  if (CrcInitialized == FALSE)
  {
    for (TableEntry = 0; TableEntry < 256; TableEntry++)
    {
      Value = ReverseBits ((UINT32) TableEntry);
      for (Index = 0; Index < 8; Index++)
      {
        if ((Value & 0x80000000) != 0)
        {
          Value = (Value << 1) ^ CRC32_POLYNOMIAL;
        }
        else
        {
          Value = Value << 1;
        }
      }
  
      mCrcTable[TableEntry] = ReverseBits (Value);
    }

    CrcInitialized = TRUE;
  }
}

/**
  Calculate CRC32 for data.

  @param Data            Pointer to the data to calculate CRC on.
  @param DataSize        Size in bytes of the data.
  @param CrcOut          Pointer to the CRC32 of the data.

  @retval  EFI_SUCCESS            The CRC32 for data is calculated successfully.
  @retval  EFI_INVALID_PARAMETER  Data or CrcOut is NULL.

**/
EFI_STATUS
EFIAPI
CalculateCrc32 (
  IN  VOID    *Data,
  IN  UINTN   DataSize,
  OUT UINT32  *CrcOut
  )
{
  UINT32  Crc;
  UINTN   Index;
  UINT8   *Ptr;

  if (Data == NULL || DataSize == 0 || CrcOut == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (CrcInitialized == FALSE)
  {
    InitializeCrc32Table();
  }

  Crc = CRC32_SEED;
  for (Index = 0, Ptr = Data; Index < DataSize; Index++, Ptr++)
  {
    Crc = (Crc >> 8) ^ mCrcTable[(UINT8) Crc ^ *Ptr];
  }

  *CrcOut = Crc ^ CRC32_SEED;

  return EFI_SUCCESS;
}


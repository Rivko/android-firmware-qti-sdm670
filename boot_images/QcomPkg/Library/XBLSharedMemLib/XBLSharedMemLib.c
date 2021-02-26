/** @file MemMgr.c
   
  Implementation of XBL Shared Memory Lib interface

  Copyright (c) 2014, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/11/14   niting  New file

=============================================================================*/

#include <Library/BaseLib.h>
#include <Library/XBLSharedMemLib.h>

/* Default Alignment */
#ifndef DEFAULT_MEM_ALIGNMENT
  #define DEFAULT_MEM_ALIGNMENT  32
#endif

#define XMEM_ALLOC_CONTEXT_SIGNATURE \
        ('X' | ('S' << 8) | ('H' << 16) | ('M' << 24))

#define XMEM_ALLOC_VERSION 0x00010000
#define MAX_NAME_SIZE 16

/* 
  Memory Layout of XMEM context 
 
  |---------------------|
  | Signature           |
  |---------------------|
  | Version             |
  |---------------------|
  | MemoryPoolStart     |
  |---------------------|
  | FreeMemoryBase      |
  |---------------------|
  | MemoryPoolSize      |
  |---------------------|
  | Reserved            |
  |---------------------|
  | RegionName          |
  | (16 bytes)          |
  | i.e. "DDRSharedMem" |
  |                     |
  |---------------------| Start of XMEM records
  | Id (16 bytes)       |
  |(i.e. "MyData1")     |
  |                     |
  |                     |
  |---------------------|
  | Size (i.e. 0xC)     |
  |---------------------|
  | Data                |
  | i.e. 0xC bytes of   |
  |      data           |
  |---------------------|
  | Id (16 bytes)       |
  |(i.e. "MySharedData")|
  |                     |
  |                     |
  |---------------------|
  | Size (i.e. 0x10)    |
  |---------------------|
  | Data                |
  | i.e. 0x10 bytes of  |
  |      data           |
  |                     |
  |---------------------| FreeMemoryBase
  |                     |
  |                     |
  |                     |
  |                     |
  | Available Memory    |
  |                     |
  |                     |
  |                     |
  |                     |
  |                     |
  |                     |
  |                     |
  |                     |
  |                     |
  |                     |
  |---------------------|
*/


typedef struct _XMEM_ALLOC_CONTEXT {
   UINT32     Signature;
   UINT32     Version;
   UINT8      *MemoryPoolStart;      // Start of memory pool
   UINT8      *MemoryPoolEnd;        // End of memory pool
   UINT8      *FreeMemoryBase;       // Location of first available memory
   UINT64     MemoryPoolSize;        // Size in bytes of pool
   CHAR8      RegionName[MAX_NAME_SIZE]; // Null terminated Ascii string
}XMEM_ALLOC_CONTEXT;

typedef struct _XMEM_RECORD {
  CHAR8       Name[MAX_NAME_SIZE];   // Null terminated Ascii string
  UINT64      Size;                  // Size of data - does not include this structure
  UINT8       Reserved[8];           // For alignment so data is 32-byte aligned
}XMEM_RECORD;

#define ALIGN_TO_UPPERBOUNDARY(x)  \
        ((((UINTN)(x)) + (DEFAULT_MEM_ALIGNMENT - 1)) &  \
        ~(DEFAULT_MEM_ALIGNMENT - 1))

/* Array to keep track of contexts. Helps for context lookups */
#define XMEM_MAX_CONTEXTS 4
STATIC XMEM_ALLOC_CONTEXT *ContextArray[XMEM_MAX_CONTEXTS];
STATIC UINT32 NumContexts = 0;

/* Returns TRUE if Address in within RegionAddress range */
STATIC
BOOLEAN
InRange(VOID *Address, VOID *RegionAddress, UINT64 RegionSize)
{
  
  if (((UINTN)Address >= (UINTN)RegionAddress) &&
      ((UINTN)Address <= (UINTN)RegionAddress + RegionSize))
  {
    return TRUE;
  }

  return FALSE;
}

VOID* XBLInitSharedMemMgr (CHAR8            *RegionName,
                           UINT8            *MemPool, 
                           UINTN            *MemPoolSize)
{
  XMEM_ALLOC_CONTEXT *MemContextPtr;
  UINT32 Index;

  /* Either MemPool and MemPoolSize are both valid or both NULL */
  if ((MemPoolSize && (MemPool == NULL)) || (MemPool && (MemPoolSize == NULL)) || 
      (RegionName == NULL))
  {
    return NULL;
  }

  if (MemPool == NULL)
  {
    /* Return existing context if it exists */
    for (Index = 0; Index < NumContexts; Index++)
    {
      /* Compare all but NULL character */
      if (AsciiStrnCmp(RegionName, ContextArray[Index]->RegionName, MAX_NAME_SIZE-1) == 0)
      {
        return (VOID*)ContextArray[Index];
      }
    }

    /* No matching context found */
    return NULL;
  }

  /* Check for minimum size required */
  if ((*MemPoolSize == 0) || 
      (*MemPoolSize <= (sizeof(XMEM_ALLOC_CONTEXT) + sizeof(XMEM_RECORD))))
  {
    return NULL;
  }

  MemContextPtr = (XMEM_ALLOC_CONTEXT *)MemPool;

  /* Check if memory context already exists at MemPool location
     - if so, then just return existing context */
  if (MemContextPtr->Signature == XMEM_ALLOC_CONTEXT_SIGNATURE)
  {
    return MemContextPtr;
  }

  /* Confirm there is no overlap of regions for new context */
  for (Index = 0; Index < NumContexts; Index++)
  {
    if (InRange(MemPool, ContextArray[Index], ContextArray[Index]->MemoryPoolSize))
    {
      /* New region overlaps or contained within existing region */
      return NULL;
    }

    if (InRange(ContextArray[Index], MemPool,(UINT64)(*MemPoolSize)))
    {
      /* Existing region overlaps or contained in new region */
      return NULL;
    }
  }

  /* Allocate new context */

  /* Check if new context can be allocated */
  if (NumContexts >= XMEM_MAX_CONTEXTS)
    return NULL;

  ContextArray[NumContexts++] = MemContextPtr;

  MemContextPtr->FreeMemoryBase = 
    (UINT8*)ALIGN_TO_UPPERBOUNDARY(MemPool + sizeof(XMEM_ALLOC_CONTEXT));
  MemContextPtr->MemoryPoolStart = MemContextPtr->FreeMemoryBase;
  MemContextPtr->MemoryPoolEnd = MemContextPtr->MemoryPoolStart + (*MemPoolSize);

  *MemPoolSize -= ((UINTN)MemContextPtr->FreeMemoryBase - (UINTN)MemPool);
  MemContextPtr->MemoryPoolSize = (UINT64)*MemPoolSize;

  /* Pointer math involved, so we should never wrap back in this range */
  if ((UINTN)MemContextPtr->FreeMemoryBase + MemContextPtr->MemoryPoolSize < 
      (UINTN)MemContextPtr->FreeMemoryBase)
    return NULL;
  
  /* Copy region name */
  AsciiStrnCpy(MemContextPtr->RegionName, RegionName, MAX_NAME_SIZE);
  MemContextPtr->RegionName[MAX_NAME_SIZE-1] = '\0'; // NULL terminate

  MemContextPtr->Version = XMEM_ALLOC_VERSION;
  MemContextPtr->Signature = XMEM_ALLOC_CONTEXT_SIGNATURE;

  return MemContextPtr;
}

UINT8* XBLAllocSharedMem (VOID*   Ctxt, 
                          CHAR8  *Name,
                          UINTN  *Size)
{
  XMEM_ALLOC_CONTEXT *MemContextPtr = (XMEM_ALLOC_CONTEXT *)Ctxt;
  XMEM_RECORD *RecordPtr = NULL;
  INTN SizeLeft;
  
  if ((MemContextPtr == NULL) || (Name == NULL) || (Size == 0))
    return NULL;

  if (MemContextPtr->Signature != XMEM_ALLOC_CONTEXT_SIGNATURE)
    return NULL;

  /* Account for record */
  *Size = ALIGN_TO_UPPERBOUNDARY(*Size + sizeof(XMEM_RECORD));

  RecordPtr = (XMEM_RECORD*)MemContextPtr->MemoryPoolStart;
  /* Parse to see if record exists */
  while ((UINTN)RecordPtr < (UINTN)MemContextPtr->FreeMemoryBase)
  {
    UINTN NextRecord;
    if (AsciiStrnCmp(Name, RecordPtr->Name, MAX_NAME_SIZE-1) == 0)
    {
      /* Found existing matching record */
      *Size = RecordPtr->Size;
      return (UINT8*)((UINTN)RecordPtr + sizeof(XMEM_RECORD));
    }
    
    /* Get next record location - account for record structure */
    NextRecord = (UINTN)RecordPtr + RecordPtr->Size + sizeof(XMEM_RECORD);
    RecordPtr = (XMEM_RECORD *)NextRecord;
  }

  /* Could not find existing record, allocate new memory */

  /* Check if there is enough memory - account for new record */
  SizeLeft = (UINTN)MemContextPtr->MemoryPoolEnd - 
             (UINTN)MemContextPtr->FreeMemoryBase - 
             sizeof(XMEM_RECORD);
  if (SizeLeft < 0)
    SizeLeft = 0;

  if (SizeLeft < *Size)
  {
    /* Return available memory left - account for record structure */
    *Size = SizeLeft;
    return NULL;
  }

  /* Allocate record */
  RecordPtr = (XMEM_RECORD*)MemContextPtr->FreeMemoryBase;
  AsciiStrnCpy(RecordPtr->Name, Name, MAX_NAME_SIZE);
  RecordPtr->Name[MAX_NAME_SIZE-1] = '\0'; // NULL terminate
  RecordPtr->Size = *Size - sizeof(XMEM_RECORD);

  /* Update location of next available memory */
  MemContextPtr->FreeMemoryBase += *Size;

  return (UINT8*)((UINTN)RecordPtr + sizeof(XMEM_RECORD));
}


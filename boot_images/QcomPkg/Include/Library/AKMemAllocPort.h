/** @file

  Copyright (c) 2015, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/20/15   ai      Created
=============================================================================*/
#ifndef __AK_MEMALLOC_PORT_H__
#define __AK_MEMALLOC_PORT_H__

#include <Uefi.h>
#include <MpIoMsgInfo.h>

#define MEMALLOCPORT      MPIO_PORT('M','e','m','A','l','l','o','c')

#define AK_MEMALLOC_SERVICE_ALLOCATE_PAGES    0x0
#define AK_MEMALLOC_SERVICE_FREE_PAGES        0x1
#define AK_MEMALLOC_SERVICE_ALLOCATE_POOL     0x2
#define AK_MEMALLOC_SERVICE_FREE_POOL         0x3

typedef struct {
  VOID        *UserBuffer;
} FN_FREE_POOL_PARAMS;

typedef struct {
 EFI_MEMORY_TYPE   PoolType;
 UINTN             Size;
 VOID              **UserBuffer;
} FN_ALLOCATE_POOL_PARAMS;

typedef struct {
  EFI_PHYSICAL_ADDRESS   Memory;
  UINTN                  NumberOfPages;
} FN_FREE_PAGES_PARAMS;

typedef struct {
  EFI_ALLOCATE_TYPE     Type;
  EFI_MEMORY_TYPE       MemoryType;
  UINTN                 NumberOfPages;
  EFI_PHYSICAL_ADDRESS  *Memory;
} FN_ALLOCATE_PAGES_PARAMS;

#endif // __AK_MEMALLOC_PORT_H__


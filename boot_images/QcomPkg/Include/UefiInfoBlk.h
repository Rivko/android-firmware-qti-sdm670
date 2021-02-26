/** @file UefiInfoBlk.h
  UEFI Info blocking holding helpful informational and statistics data.

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.

  Qualcomm Technologies Proprietary and Confidential.

**/
#ifndef __UEFI_INFO_BLK_H__
#define __UEFI_INFO_BLK_H__

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 29/08/16   vk      Add pointer to crash cpu context
 02/12/16   bh      Change attribute name
 10/27/15   bh      Change attribute name
 10/15/15   bh      Add entries for debug scripts
 05/19/15   vk      MemBase is not a pointer
 08/05/14   vk      Add 32/64 bit support
 10/04/13   yg      Add DxeSymbolAddress
 29/03/13   vk      Add support for run time symbol load
 03/19/13   vk      Add system table pointer to InfoBlk, move to common
 02/29/13   vk      Add carve out mode to InfoBlk
 29/01/12   vk      Add Memory table, and config table, rev update
 12/01/12   yg      Created
 
=============================================================================*/

#include <Uefi.h>
#include <Protocol/DebugSupport.h>

#define UEFI_INFO_BLOCK_SIGNATURE  ('I' | ('B' << 8) | ('l' << 16) | ('k' << 24))

/* Version indicating the InfoBlock structure */
#define UEFI_INFO_BLOCK_VERSION    0x0001000B

typedef struct {             /* Offsets:   32 bit    64 bit   */
  /* Ver 0x00010001 */
  UINT32       Signature;              /*   0x0       0x0      */
  UINT32       StructVersion;          /*   0x4       0x4      */
  CHAR8*       RelInfo;                /*   0x8       0x8      */
  UINT64       FdMemBase;              /* * 0x10      0x10     */
  UINT64       DDRMemSize;             /*   0x18      0x18     */
  UINTN*       StackBase;              /*   0x20      0x20     */
  UINTN        StackSize;              /*   0x24      0x28     */
  UINTN*       HobBase;                /*   0x28      0x30     */
  UINTN*       UartLogBufferPtr;       /*   0x2C      0x38     */
  UINTN        UartLogBufferLen;       /*   0x30      0x40     */
  /* Ver 0x00010002 */                             
  UINTN*       MemTablePtr;            /*   0x34      0x48     */
  UINTN        MemTableCount;          /*   0x38      0x50     */
  UINTN*       ConfigTablePtr;         /*   0x3C      0x58     */
  UINTN        ConfigTableCount;       /*   0x40      0x60     */
  /* Ver 0x00010003 */                             
  UINTN        InCarveOutMode;         /*   0x44      0x68     */
  /* Ver 0x00010004 */                             
  UINTN*       SystemTablePtr;         /*   0x48      0x70     */
  /* Ver 0x00010005 */                             
  UINTN*       DebugTablePtr;          /*   0x4C      0x78     */
  UINTN        DebugTableEntryCnt;     /*   0x50      0x80     */
  UINTN*       RuntimeDbgTablePtr;     /*   0x54      0x88     */
  UINTN        RtDbgTableEntryCnt;     /*   0x58      0x90     */
  /* Ver 0x00010007 */                             
  UINT64       DxeSymbolAddr;          /*   0x60      0x98     */
                                                  
  /* Ver 0x00010008 */                             
  UINT32       CmdStartOffset;         /*   0x68      0xA0     */
  UINT32       TotalCmdSize;           /*   0x6C      0xA4     */
  UINTN        SymbolLoadCmdStrPtr;    /*   0x70      0xA8     */
  UINTN        LoaderBreakPoint;       /*   0x74      0xB0     */
  CHAR8        DebugStopDriverStr[32]; /*   0x78      0xB8     */
  CHAR8        BkptModuleName[32];     /*   0x98      0xD8     */
  UINT32       SymbolLoadCmdStrSize;   /*   0xB8      0xF8     */
  UINT32       LastCmdSize;            /*   0xBC      0xFC     */

  /* Ver 0x00010009 */                             
  CHAR8        DirStr[32];             /*   0xC0      0x100    */

  /* Ver 0x0001000A */                            
  UINT8        BdsHotkey;              /*   0xE0      0x120    */

  /* Ver 0x0001000B */                            
  EFI_SYSTEM_CONTEXT   CrashContext;   /*   0xE4      0x128    */

  /* Ver 0x0001000x */ 
  /* Add new fields here and increment version macro */
                                       /*   0xE0      0x120     */

}UefiInfoBlkType;

#endif  /* __UEFI_INFO_BLK_H__ */



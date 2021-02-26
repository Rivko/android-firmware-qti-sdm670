/** @file
  Header file for Guided FV library internal declaration
   
  Copyright (c) 2017 Qualcomm Technologies, Inc. All rights reserved. 
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/08/17   bd      Initial revision

=============================================================================*/
#ifndef _GUIDED_FV_INTERNAL_H_
#define _GUIDED_FV_INTERNAL_H_

/* Split ELF file */
EFI_STATUS  EFISplitELFFile(void *elf_base, void **meta_base, void **prog_base, uint32 *prog_size);

/* Authenticate ELF file */
EFI_STATUS AuthenticateElf(void* ElfBase, UINTN ImageSize, void **FvBase);

#endif // _GUIDED_FV_INTERNAL_H_




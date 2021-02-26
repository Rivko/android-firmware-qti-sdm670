/** @file
  This module performs generic ELF image parsing and create meta data. 

  Copyright (c) 2017, Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/08/17   bd      Initial revision

=============================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <api/boot/boot_elf_header.h>

/*===========================================================================
                    VARIABLE DECLARATIONS FOR MODULE
===========================================================================*/


#define ELF_P_FLAGS_MASK (0x0F << 24)
#define ELF_P_FLAGS_HASH (0x02 << 24)
#define ELF_P_FLAGS_LOAD (0x08 << 24)

#define MI_PBT_MAX_SEGMENTS 100
#define MI_PBT_ELF_PT_LOAD  0x1

/*===========================================================================
                        LOCAL FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  calculate_segment_offsets
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies the ELF Header by sanity checking various fields
*   within the ELF Header structure 
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE - If ELF Header has passed verification
*   FALSE - Otherwise
* 
*/
static void calculate_segment_offsets(void * elf_base, Elf64_Ehdr *ehdr, uint8 **hash_seg_base, uint32 *hash_seg_size,
  uint8 **prog_seg_base, uint32 *prog_seg_size)
{
  int i = 0;
  uint32 flags;
  Elf64_Phdr * phdr = (Elf64_Phdr *)( ( (uint8 *)elf_base ) + ehdr->e_phoff );
  Elf64_Phdr * current_phdr;

  for (i = 0; i < ehdr->e_phnum; i++)
  {
    current_phdr = phdr + i;
    flags = current_phdr->p_flags;

    /* hash segment has permission value in flag as 0x2 */
    if ( (flags & ELF_P_FLAGS_MASK) == ELF_P_FLAGS_HASH)
    {
      *hash_seg_base = ( (uint8 *)elf_base ) + current_phdr->p_offset;
      *hash_seg_size = current_phdr->p_filesz;
    }
    /* loadable program segment has p_type value as 1 */
    else if (MI_PBT_ELF_PT_LOAD ==  current_phdr->p_type)
    {
      *prog_seg_base = ( (uint8 *)elf_base ) + current_phdr->p_offset;
      *prog_seg_size += current_phdr->p_memsz;
    }
  }

  return;
}

static void calculate_segment_offsets_32(void * elf_base, Elf32_Ehdr *ehdr_32, uint8 **hash_seg_base, uint32 *hash_seg_size,
  uint8 **prog_seg_base, uint32 *prog_seg_size)
{
  int i = 0;
  uint32 flags;
  Elf32_Phdr * phdr = (Elf32_Phdr *)( ( (uint8 *)elf_base ) + ehdr_32->e_phoff );
  Elf32_Phdr * current_phdr;

  for (i = 0; i < ehdr_32->e_phnum; i++)
  {
    current_phdr = phdr + i;
    flags = current_phdr->p_flags;

    /* hash segment has permission value in flag as 0x2 */
    if ( (flags & ELF_P_FLAGS_MASK) == ELF_P_FLAGS_HASH)
    {
      *hash_seg_base = ( (uint8 *)elf_base ) + current_phdr->p_offset;
      *hash_seg_size = current_phdr->p_memsz;
    }
    /* loadable program segment has p_type value as 1 */
    else if (MI_PBT_ELF_PT_LOAD ==  current_phdr->p_type)
    {
      *prog_seg_base = ( (uint8 *)elf_base ) + current_phdr->p_offset;
      *prog_seg_size += current_phdr->p_memsz;
    }
  }

  return;
}

/*===========================================================================
**  Function :  boot_elf_verify_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies the ELF Header by sanity checking various fields
*   within the ELF Header structure 
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE - If ELF Header has passed verification
*   FALSE - Otherwise
* 
*/
static boolean boot_elf_verify_header(Elf64_Ehdr *ehdr)
{
  /* Sanity check the ELF header,
    * and ensure that there is at least 1 prog-header entry
    * and total number of prog-hdr segments are not more than the allocated prog-hdr buffer
    */
  return (  ehdr->e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
          ehdr->e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
          ehdr->e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
          ehdr->e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
          ehdr->e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_64 &&
          ehdr->e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
          ehdr->e_ehsize == sizeof(Elf64_Ehdr) &&
          ehdr->e_phentsize == sizeof(Elf64_Phdr) &&
          ehdr->e_phnum != 0 &&
          ehdr->e_phnum <= MI_PBT_MAX_SEGMENTS);
}

static boolean boot_elf_verify_header_32(Elf32_Ehdr *ehdr_32)
{
  /* Sanity check the ELF header,
    * and ensure that there is at least 1 prog-header entry
    * and total number of prog-hdr segments are not more than the allocated prog-hdr buffer
    */
  return (  ehdr_32->e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
          ehdr_32->e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
          ehdr_32->e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
          ehdr_32->e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
          ehdr_32->e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_32 &&
          ehdr_32->e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
          ehdr_32->e_ehsize == sizeof(Elf32_Ehdr) &&
          ehdr_32->e_phentsize == sizeof(Elf32_Phdr) &&
          ehdr_32->e_phnum != 0 &&
          ehdr_32->e_phnum <= MI_PBT_MAX_SEGMENTS);
}

/*===========================================================================
**  Function : EFISplitELFFile
** ==========================================================================
*/
/*!
* 
* @brief
*   This function splits the input ELF file into META data and programme data
*
* @par Dependencies
*   None 
* 
* @retval
*   EFI_SUCCESS - If ELF file split is successful;
                  Input pointers will be updated to point META & programme data areas
*   Error code  - Otherwise
* 
*/
EFI_STATUS  EFISplitELFFile
  (
    void   *  elf_base,
    void   ** meta_base, 
    void   ** prog_base,
    uint32 *  prog_size
  )
{
  uint32 meta_buffer_size = 0;
  uint32 current_meta_buffer_size = 0;
  uint8 * meta_buffer = NULL;
  Elf32_Ehdr *ehdr_32 = NULL;
  Elf64_Ehdr *ehdr = NULL;
  Elf32_Phdr *ephdr_32 = NULL;
  Elf64_Phdr * ephdr = NULL;
  uint8 elf_format;  
  uint32 hash_seg_size = 0;
  uint8 *hash_seg_base = NULL;
  uint32 prog_seg_size = 0;
  uint8 *prog_seg_base = NULL;

  /* Check the EI_CLASS to see if it's 32bit or 64bit ELF 
   * Note the e_ident format is the same in both 32bit and 64bit
   */
  elf_format = ((Elf64_Ehdr *)elf_base)->e_ident[ELFINFO_CLASS_INDEX];

  if ( elf_format == ELF_CLASS_32 )
  {
    //CopyMem (&ehdr_32, elf_base, sizeof(Elf32_Ehdr));
    ehdr_32 = (Elf32_Ehdr *)elf_base;
    ephdr_32 = (Elf32_Phdr *)( ( (uint8 *)elf_base ) + ehdr_32->e_phoff );

    if ( boot_elf_verify_header_32(ehdr_32) != TRUE )
    {
      DEBUG(( EFI_D_WARN, "%a: Failed to verify ELF_32 hdr\r\n", __func__));
      return EFI_VOLUME_CORRUPTED;
    }

    /* Calculate hash and program segment offsets */
    calculate_segment_offsets_32(elf_base, ehdr_32, &hash_seg_base, &hash_seg_size,&prog_seg_base, &prog_seg_size);

    /* Determine meta buffer size */
    meta_buffer_size = sizeof(Elf32_Ehdr) + ( sizeof(Elf32_Phdr) * ehdr_32->e_phnum ) + hash_seg_size;

    /* Allocate un-cached buffer */
    meta_buffer = (uint8 *)UncachedAllocateAlignedPool (meta_buffer_size, EFI_PAGE_SIZE);

    /* initialize copy buffer size to zero */
    current_meta_buffer_size = 0;

    /* Copy ELF Header */
    CopyMem(meta_buffer + current_meta_buffer_size, ehdr_32, sizeof(Elf32_Ehdr));
    /* Move buffer offset to the end of ELF header */
    current_meta_buffer_size += sizeof(Elf32_Ehdr);

    /* Copy Program Headers */
    CopyMem(meta_buffer + current_meta_buffer_size, ephdr_32, sizeof(Elf32_Phdr) * ehdr_32->e_phnum);
    /* Move buffer offset to the end of program headers */
    current_meta_buffer_size += (sizeof(Elf32_Phdr) * ehdr_32->e_phnum);

    /* if there is a hash segment copy it */
    if (hash_seg_size != 0)
    {
      CopyMem(meta_buffer + current_meta_buffer_size, (void *)hash_seg_base, hash_seg_size);
    }
  }
  else if ( elf_format == ELF_CLASS_64 )
  {
    //CopyMem (&ehdr, elf_base, sizeof(Elf64_Ehdr));
    ehdr = (Elf64_Ehdr *)elf_base;
    ephdr = (Elf64_Phdr *)( ( (uint8 *)elf_base ) + ehdr->e_phoff );

    if ( boot_elf_verify_header(ehdr) != TRUE )
    {
      DEBUG(( EFI_D_WARN, "%a: Failed to verify ELF hdr\r\n", __func__));
      return EFI_VOLUME_CORRUPTED;
    }

    /* Calculate hash and program segment offsets */
    calculate_segment_offsets(elf_base, ehdr, &hash_seg_base, &hash_seg_size, &prog_seg_base, &prog_seg_size);

    /* Determine meta buffer size */
    meta_buffer_size = sizeof(Elf64_Ehdr) + ( sizeof(Elf64_Phdr) * ehdr->e_phnum ) + hash_seg_size;

    /* Allocate un-cached buffer */
    meta_buffer = (uint8 *)UncachedAllocateAlignedPool (meta_buffer_size, EFI_PAGE_SIZE);

    /* initialize copy buffer size to zero */
    current_meta_buffer_size = 0;

    /* Copy ELF Header */
    CopyMem(meta_buffer + current_meta_buffer_size, ehdr, sizeof(Elf64_Ehdr));
    /* Move buffer offset to the end of ELF header */
    current_meta_buffer_size += sizeof(Elf64_Ehdr);

    /* Copy Program Headers */
    CopyMem(meta_buffer + current_meta_buffer_size, ephdr, sizeof(Elf64_Phdr) * ehdr->e_phnum);
    /* Move buffer offset to the end of program headers */
    current_meta_buffer_size += (sizeof(Elf32_Phdr) * ehdr->e_phnum);

    /* if there is a hash segment copy it */
    if (hash_seg_size != 0)
    {
      CopyMem(meta_buffer + current_meta_buffer_size, (void *)hash_seg_base, hash_seg_size);
    }
  }
  else
  {
    DEBUG(( EFI_D_WARN, "%a: Not a valid ELF format\r\n", __func__));
    return EFI_ABORTED;
  }

  /* Copy meta buffer base, program segment base & program segment size */
  *meta_base = meta_buffer;
  *prog_base = prog_seg_base;
  *prog_size = prog_seg_size;

  return EFI_SUCCESS;
} 

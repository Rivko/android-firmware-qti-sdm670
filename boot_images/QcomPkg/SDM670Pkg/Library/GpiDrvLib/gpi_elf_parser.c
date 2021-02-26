/*===========================================================================

  @file
    gpi_elf_parser.h

  @brief
    This module performs generic ELF image parsing and create meta data. 

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/11/17   ts         Initial revision to parse ELF.

===============================================================================
          Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.
                        All Rights Reserved.
                 Qualcomm Confidential and Proprietary.
=============================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <api/boot/boot_elf_header.h>
#include <api/boot/MIprogressive.h>
/*===========================================================================
                    VARIABLE DECLARATIONS FOR MODULE
===========================================================================*/

/* ELF format (object class), default to 64bit */
static uint8 elf_format;

/* ELF header for  bit format */
static Elf32_Ehdr ehdr_32;

/* ELF header for 64 bit format */
static Elf64_Ehdr ehdr;

/* Program header base for 32 bit format */
static Elf32_Phdr * ephdr_32 = NULL;

/* Program header base for 64 bit format */
static Elf64_Phdr * ephdr = NULL;

/* size and address of the hash segment if any */
static uint32 hash_seg_size = 0;
static void * hash_seg_base = NULL;

/* size and address of the prog segment if any */
static uint32 prog_seg_size = 0;
static void * prog_seg_base = NULL;

#define ELF_P_FLAGS_MASK (0x0F << 24)
#define ELF_P_FLAGS_HASH (0x02 << 24)
#define ELF_P_FLAGS_LOAD (0x08 << 24)

/*===========================================================================
                        LOCAL FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  calculate_segment_offsets/_32
** ==========================================================================
*/
/*!
* 
* @brief
*   These functions parse through the ELF header structure and extracts
*   corresponding start offsets for hash and program segments. There is
*   a implementation for 32 bit and 64 bit ELF formats
*
* @par Dependencies
*   None 
* 
* @retval
*   None 
* 
*/
static void calculate_segment_offsets(void * elf_base)
{
  int i = 0;
  uint32 flags;
  Elf64_Phdr * phdr = (Elf64_Phdr *)( ( (uint8 *)elf_base ) + ehdr.e_phoff );
  Elf64_Phdr * current_phdr;

  for (i = 0; i < ehdr.e_phnum; i++)
  {
    current_phdr = phdr + i;
    flags = current_phdr->p_flags;

    /* hash segment has permission value in flag as 0x2 */
    if ( (flags & ELF_P_FLAGS_MASK) == ELF_P_FLAGS_HASH)
    {
      hash_seg_base = ( (uint8 *)elf_base ) + current_phdr->p_offset;
      hash_seg_size = current_phdr->p_filesz;
    }
    /* loadable program segment has permission value in flag as 0x8 */
    else if ( (flags & ELF_P_FLAGS_MASK) == ELF_P_FLAGS_LOAD)
    {
      if ( prog_seg_base == NULL )
      {
	prog_seg_base = ( (uint8 *)elf_base ) + current_phdr->p_offset;
      }

      /* valid program segments size would be the cumulative sizes of all
       * program segments
       */
      prog_seg_size += current_phdr->p_memsz;
    }
  }

  return;
}

static void calculate_segment_offsets_32(void * elf_base)
{
  int i = 0;
  uint32 flags;
  Elf32_Phdr * phdr = (Elf32_Phdr *)( ( (uint8 *)elf_base ) + ehdr_32.e_phoff );
  Elf32_Phdr * current_phdr;

  for (i = 0; i < ehdr_32.e_phnum; i++)
  {
    current_phdr = phdr + i;
    flags = current_phdr->p_flags;

    /* hash segment has permission value in flag as 0x2 */
    if ( (flags & ELF_P_FLAGS_MASK) == ELF_P_FLAGS_HASH)
    {
      hash_seg_base = ( (uint8 *)elf_base ) + current_phdr->p_offset;
      hash_seg_size = current_phdr->p_memsz;
    }
    /* loadable program segment has permission value in flag as 0x8 */
    else if ( (flags & ELF_P_FLAGS_MASK) == ELF_P_FLAGS_LOAD)
    {
      if ( prog_seg_base == NULL )
      {
	prog_seg_base = ( (uint8 *)elf_base ) + current_phdr->p_offset;
      }

      /* valid program segments size would be the cumulative sizes of all
       * program segments
       */
      prog_seg_size += current_phdr->p_memsz;
    }
  }

  return;
}

/*===========================================================================
**  Function :  boot_elf_verify_header/_32
** ==========================================================================
*/
/*!
* 
* @brief
*   These functions verifies the ELF Header by sanity checking various fields
*   within the ELF Header structure. There is a implementation for 32 bit and
*   64 bit ELF formats.
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE - If ELF Header has passed verification
*   FALSE - Otherwise
* 
*/
static boolean boot_elf_verify_header(void)
{
  /* Sanity check the ELF header,
   * and ensure that there is at least 1 prog-header entry
   * and total number of prog-hdr segments are not more than the allocated prog-hdr buffer
   */
  return (  ehdr.e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
          ehdr.e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
          ehdr.e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
          ehdr.e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
          ehdr.e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_64 &&
          ehdr.e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
          ehdr.e_ehsize == sizeof(Elf64_Ehdr) &&
          ehdr.e_phentsize == sizeof(Elf64_Phdr) &&
          ehdr.e_phnum != 0 &&
          ehdr.e_phnum <= MI_PBT_MAX_SEGMENTS);
}

static boolean boot_elf_verify_header_32(void)
{
  /* Sanity check the ELF header,
   * and ensure that there is at least 1 prog-header entry
   * and total number of prog-hdr segments are not more than the allocated prog-hdr buffer
   */
  return (  ehdr_32.e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
          ehdr_32.e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
          ehdr_32.e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
          ehdr_32.e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
          ehdr_32.e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_32 &&
          ehdr_32.e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
          ehdr_32.e_ehsize == sizeof(Elf32_Ehdr) &&
          ehdr_32.e_phentsize == sizeof(Elf32_Phdr) &&
          ehdr_32.e_phnum != 0 &&
          ehdr_32.e_phnum <= MI_PBT_MAX_SEGMENTS);
}

/*===========================================================================
**  Function : efi_split_elf_file
** ==========================================================================
*/
/*!
* 
* @brief
*   This function split ELF file into hash segments and loadable program
*   segments.
*
* @par Dependencies
*   None 
* 
* @retval
*   EFI_STATUS
* 
*/
EFI_STATUS  efi_split_elf_file
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

  /* Check the EI_CLASS to see if it's 32bit or 64bit ELF 
   * Note the e_ident format is the same in both 32bit and 64bit
   */
  elf_format = ((Elf64_Ehdr *)elf_base)->e_ident[ELFINFO_CLASS_INDEX];

  if ( elf_format == ELF_CLASS_32 )
  {
    CopyMem (&ehdr_32, elf_base, sizeof(Elf32_Ehdr));

    /* Make of note of program header base from within ELF */
    ephdr_32 = (Elf32_Phdr *)( ( (uint8 *)elf_base ) + ehdr_32.e_phoff );

    /* Check if this is a valid ELF */
    if ( boot_elf_verify_header_32() != TRUE )
    {
      DEBUG(( EFI_D_WARN, "%a: Failed to verify ELF_32 hdr\r\n", __func__));
      return EFI_SECURITY_VIOLATION;
    }

    /* Calculate hash and program segment offsets */
    calculate_segment_offsets_32(elf_base);

    /* Determine meta buffer size */
    meta_buffer_size = sizeof(Elf32_Ehdr) + ( sizeof(Elf32_Phdr) * ehdr_32.e_phnum ) + hash_seg_size;

    /* Allocate un-cached buffer */
    meta_buffer = (uint8 *)UncachedAllocatePool (meta_buffer_size);

    /* initialize copy buffer size to zero */
    current_meta_buffer_size = 0;

    /* Copy ELF Header */
    CopyMem(meta_buffer + current_meta_buffer_size, &ehdr_32, sizeof(Elf32_Ehdr));
    /* Move buffer offset to the end of ELF header */
    current_meta_buffer_size += sizeof(Elf32_Ehdr);

    /* Copy Program Headers */
    CopyMem(meta_buffer + current_meta_buffer_size, ephdr_32, sizeof(Elf32_Phdr) * ehdr_32.e_phnum);
    /* Move buffer offset to the end of program headers */
    current_meta_buffer_size += (sizeof(Elf32_Phdr) * ehdr_32.e_phnum);

    /* if there is a hash segment copy it */
    if (hash_seg_size != 0)
    {
      CopyMem(meta_buffer + current_meta_buffer_size, hash_seg_base, hash_seg_size);
    }
  }
  else if ( elf_format == ELF_CLASS_64 )
  {
    CopyMem (&ehdr, elf_base, sizeof(Elf64_Ehdr));

    /* Make of note of program header base from within ELF */
    ephdr = (Elf64_Phdr *)( ( (uint8 *)elf_base ) + ehdr.e_phoff );

    /* Check if this is a valid ELF */
    if ( boot_elf_verify_header() != TRUE )
    {
      DEBUG(( EFI_D_WARN, "%a: Failed to verify ELF hdr\r\n", __func__));
      return EFI_SECURITY_VIOLATION;
    }

    /* Calculate hash and program segment offsets */
    calculate_segment_offsets(elf_base);

    /* Determine meta buffer size */
    meta_buffer_size = sizeof(Elf64_Ehdr) + ( sizeof(Elf64_Phdr) * ehdr.e_phnum ) + hash_seg_size;

    /* Allocate un-cached buffer */
    meta_buffer = (uint8 *)UncachedAllocatePool (meta_buffer_size);

    /* initialize copy buffer size to zero */
    current_meta_buffer_size = 0;

    /* Copy ELF Header */
    CopyMem(meta_buffer + current_meta_buffer_size, &ehdr, sizeof(Elf64_Ehdr));
    /* Move buffer offset to the end of ELF header */
    current_meta_buffer_size += sizeof(Elf64_Ehdr);

    /* Copy Program Headers */
    CopyMem(meta_buffer + current_meta_buffer_size, ephdr, sizeof(Elf64_Phdr) * ehdr.e_phnum);
    /* Move buffer offset to the end of program headers */
    current_meta_buffer_size += (sizeof(Elf32_Phdr) * ehdr.e_phnum);

    /* if there is a hash segment copy it */
    if (hash_seg_size != 0)
    {
      CopyMem(meta_buffer + current_meta_buffer_size, hash_seg_base, hash_seg_size);
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

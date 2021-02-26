#ifndef __BOOT_ELF_HEADER_H__
#define __BOOT_ELF_HEADER_H__
/*===========================================================================

                ELF Header Definitions

DESCRIPTION
  This header file gives the definition of the structures in ELF header.
  
  Copyright 2014 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.


   
when         who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/19/13   jz        Created (pulled out of miprogressive.h), added 64bit ELF support
===========================================================================*/

#include "boot_comdef.h"

/*
 *
 *                        ELF Header Definitions
 *                       ------------------------
 *
 * This section contains definitions necessary to parse an ELF header and
 * find ELF Header and Segment Header and extract length and offsets.
 *
 */

/* The first 52 bytes of the file is the ELF header  */

#define ELFINFO_MAGIC_SIZE (16)

typedef struct
{
  unsigned char e_ident[ELFINFO_MAGIC_SIZE]; /* Magic number and other info   */
  uint16        e_type;                /* Object file type                    */
  uint16        e_machine;             /* Architecture                        */
  uint32        e_version;             /* Object file version                 */
  uint32        e_entry;               /* Entry point virtual address         */
  uint32        e_phoff;               /* Program header table file offset    */
  uint32        e_shoff;               /* Section header table file offset    */
  uint32        e_flags;               /* Processor-specific flags            */
  uint16        e_ehsize;              /* ELF header size in bytes            */
  uint16        e_phentsize;           /* Program header table entry size     */
  uint16        e_phnum;               /* Program header table entry count    */
  uint16        e_shentsize;           /* Section header table entry size     */
  uint16        e_shnum;               /* Section header table entry count    */
  uint16        e_shstrndx;            /* Section header string table index   */
} Elf32_Ehdr;

/* Fields in the e_ident array.  The ELFINFO_*_INDEX macros are 
 * indices into the array.  The macros under each ELFINFO_* macro
 * is the values the byte may have.  
 */

#define ELFINFO_MAG0_INDEX    0     /* File identification byte 0 index */
#define ELFINFO_MAG0          0x7f  /* Magic number byte 0              */

#define ELFINFO_MAG1_INDEX    1     /* File identification byte 1 index */
#define ELFINFO_MAG1         'E'    /* Magic number byte 1              */

#define ELFINFO_MAG2_INDEX    2     /* File identification byte 2 index */
#define ELFINFO_MAG2         'L'    /* Magic number byte 2              */

#define ELFINFO_MAG3_INDEX    3     /* File identification byte 3 index */
#define ELFINFO_MAG3         'F'    /* Magic number byte 3              */

#define ELFINFO_CLASS_INDEX   4     /* File class byte index            */

/* ELF Object Type */
#define ELF_CLASS_32          1     /* 32-bit objects                   */
 

/* Version information */
#define ELFINFO_VERSION_INDEX 6    /* File version byte index          */
#define ELF_VERSION_CURRENT   1    /* Current version                  */

/* Program segment header.  */
typedef struct
{
  uint32 p_type;                   /* Segment type */
  uint32 p_offset;                 /* Segment file offset */
  uint32 p_vaddr;                  /* Segment virtual address */
  uint32 p_paddr;                  /* Segment physical address */
  uint32 p_filesz;                 /* Segment size in file */
  uint32 p_memsz;                  /* Segment size in memory */
  uint32 p_flags;                  /* Segment flags */
  uint32 p_align;                  /* Segment alignment */
} Elf32_Phdr;

/* Section header.  */
typedef struct
{
  uint32	sh_name;		/* Section name */
  uint32	sh_type;		/* Section type */
  uint32	sh_flags;		/* Section flags */
  uint32	sh_addr;		/* Section addr */
  uint32	sh_offset;		/* Section offset */
  uint32	sh_size;		/* Section size */
  uint32	sh_link;		/* Section link */
  uint32	sh_info;		/* Section info */
  uint32	sh_addralign;	/* Section align */
  uint32	sh_entsize;	    /* Section ent size */
} Elf32_Shdr;

/* 64bit ELF header */
typedef struct
{
  unsigned char e_ident[ELFINFO_MAGIC_SIZE]; /* Magic number and other info   */
  uint16        e_type;                   /* Object file type                    */
  uint16        e_machine;             /* Architecture                        */
  uint32        e_version;              /* Object file version                 */
  uint64        e_entry;                  /* Entry point virtual address         */
  uint64        e_phoff;                  /* Program header table file offset    */
  uint64        e_shoff;                  /* Section header table file offset    */
  uint32        e_flags;                  /* Processor-specific flags            */
  uint16        e_ehsize;                /* ELF header size in bytes            */
  uint16        e_phentsize;           /* Program header table entry size     */
  uint16        e_phnum;               /* Program header table entry count    */
  uint16        e_shentsize;           /* Section header table entry size     */
  uint16        e_shnum;                /* Section header table entry count    */
  uint16        e_shstrndx;            /* Section header string table index   */
} Elf64_Ehdr;

/* ELF Object Type */
#define ELF_CLASS_64          2     /* 64-bit objects                   */
 
/* 64 bit Program segment header.  */
typedef struct
{
  uint32 p_type;                   /* Segment type */
  uint32 p_flags;                  /* Segment flags */
  uint64 p_offset;                 /* Segment file offset */
  uint64 p_vaddr;                 /* Segment virtual address */
  uint64 p_paddr;                 /* Segment physical address */
  uint64 p_filesz;                  /* Segment size in file */
  uint64 p_memsz;               /* Segment size in memory */
  uint64 p_align;                  /* Segment alignment */
} Elf64_Phdr;

/* 64bit Section header.  */
typedef struct
{
  uint32	sh_name;		/* Section name */
  uint32	sh_type;		/* Section type */
  uint64	sh_flags;		/* Section flags */
  uint64	sh_addr;		/* Section addr */
  uint64	sh_offset;		/* Section offset */
  uint64	sh_size;		       /* Section size */
  uint32	sh_link;		       /* Section link */
  uint32	sh_info;		       /* Section info */
  uint64	sh_addralign;	/* Section align */
  uint64	sh_entsize;	       /* Section ent size */
} Elf64_Shdr;


#endif /* __BOOT_ELF_HEADER_H__ */


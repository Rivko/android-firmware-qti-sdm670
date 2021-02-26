#ifndef PD_ELF_H
#define PD_ELF_H
/** vi: tw=128 ts=3 sw=3 et
@file pd_elf.h
@brief This file contains the API details for the Protection Domain Monitor, API 1.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/pd_mon/src/pd_elf.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

/*

The ELF (Extensibile Linking Format) is a common standard file format for
executables, object code, libraries, and core dumps. The information contained
within this header file is derived from public industry standards documentation.

An ELF file is comprised of many optional segments; some which have associated
runtime memory addresses, and others which contain debug or diagnostic information.
The only required section is the ELF Header which acts as a table of contents to
the other sections.

|================================
| ELF Header
|================================
| Program Header
|================================
|| .code
|================================
|| .data
|================================
|| .bss
|================================
| Section Header Table
|================================
|| .debug
|================================

Each OS and Architecture may also extend the ELF file with additional arbitrary
and optional segments. Such extenstions and their specific use cases are outside
of the scope of the general ELF file docmentation cited in the bibliography below.

ELF32 details are based on the Tool Interface Standard (TIS) Executable and Linking
Format (ELF) Specification Version 1.2, May 1995. ELF64 details are based on ELF64
Object File Format Version 1.5, Draft 2, May 1998.

BIBLIOGRAPHY

Wikipedia Entry, Technical Overview and General Description
http://en.wikipedia.org/wiki/Executable_and_Linkable_Format

System V Application Binary Interface Edition 4.1 (March 1997)
http://www.sco.com/developers/devspecs/gabi41.pdf
http://www.sco.com/developers/gabi/

System V ABI Update (October 2009)
http://www.sco.com/developers/gabi/

ELF-64 Object File Format Version 1.5 Draft 2 (May 1998)
http://downloads.openwatcom.org/ftp/devel/docs/elf-64-gen.pdf
http://www.x86-64.org/documentation/abi.pdf

ELF for the ARM Architecture
http://infocenter.arm.com/help/topic/com.arm.doc.ihi0044b/IHI0044B_aaelf.pdf

ELF for the HEXAGON Architecture
https://www.codeaurora.org/patches/quic/hexagon/

Executable and Linkable Format, Wikipedia
http://en.wikipedia.org/wiki/Executable_and_Linkable_Format

*/

#include "stdarg.h"
#include "stdint.h"
#include "stdlib.h"
#include "stringl/stringl.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*
Standard Types
*/
typedef uint8_t Elf32_UChar;                                                     /**< Unsigned Char 8 Bit */
typedef uint16_t Elf32_Half;                                                     /**< Unsigned 16 Bit */
typedef uint32_t Elf32_Word;                                                     /**< Unsigned 32 Bit */
typedef int32_t Elf32_Sword;                                                     /**< Signed 32 Bit */
typedef uint64_t Elf32_Xword;                                                    /**< Unsigned 64 Bit */
typedef int64_t Elf32_Sxword;                                                    /**< Signed 64 Bit */
typedef uint32_t Elf32_Addr;                                                     /**< Address 32 Bit */
typedef uint32_t Elf32_Off;                                                      /**< File Offset 32 Bit */
typedef uint16_t Elf32_Section;                                                  /**< Section Index 16 Bit */
typedef uint16_t Elf32_Versym;                                                   /**< Version Symbol 16 Bit */

/*
Standard Types
*/
typedef uint8_t Elf64_UChar;                                                     /**< Unsigned Char 8 Bit */
typedef uint16_t Elf64_Half;                                                     /**< Unsigned 16 Bit */
typedef uint32_t Elf64_Word;                                                     /**< Unsigned 32 Bit */
typedef int32_t Elf64_Sword;                                                     /**< Signed 32 Bit */
typedef uint64_t Elf64_Xword;                                                    /**< Unsigned 64 Bit */
typedef int64_t Elf64_Sxword;                                                    /**< Signed 64 Bit */
typedef uint64_t Elf64_Addr;                                                     /**< Address 64 Bit */
typedef uint64_t Elf64_Off;                                                      /**< File Offset 64 Bit */
typedef uint16_t Elf64_Section;                                                  /**< Section Index 16 Bit */
typedef uint16_t Elf64_Versym;                                                   /**< Version Symbol 16 Bit */

/*
File Header
*/
enum
{
   EI_MAG0 = 0,                                                                  /**< File Identification */
   EI_MAG1 = 1,                                                                  /**< File Identification */
   EI_MAG2 = 2,                                                                  /**< File Identification */
   EI_MAG3 = 3,                                                                  /**< File Identification */
   EI_CLASS = 4,                                                                 /**< File Class */
   EI_DATA = 5,                                                                  /**< File Encoding */
   EI_VERSION = 6,                                                               /**< File Version (EV_CURRENT) */
   EI_OSABI = 7,                                                                 /**< File OS/ABI Version */
   EI_ABIVERSION = 8,                                                            /**< File ABI Version */
   EI_PAD = 9,                                                                   /**< File Padding Offset */
   EI_NIDENT = 16                                                                /**< File Sizeof Member E_IDENT */
};

/*
File Header
*/
typedef struct Elf32_Ehdr_s
{
   Elf32_UChar e_ident[EI_NIDENT];                                               /**< Magic Numbers */
   Elf32_Half e_type;                                                            /**< File Type */
   Elf32_Half e_machine;                                                         /**< Architecture */
   Elf32_Word e_version;                                                         /**< File Version */
   Elf32_Addr e_entry;                                                           /**< Entry Point (Virtual Address) */
   Elf32_Off e_phoff;                                                            /**< Program Header (File Offset) */
   Elf32_Off e_shoff;                                                            /**< Section Header (File Offset) */
   Elf32_Word e_flags;                                                           /**< Architecture Flags */
   Elf32_Half e_ehsize;                                                          /**< Header Size (Bytes) */
   Elf32_Half e_phentsize;                                                       /**< Phdr Size (Bytes) */
   Elf32_Half e_phnum;                                                           /**< Phdr Table Count */
   Elf32_Half e_shentsize;                                                       /**< Shdr Size (Bytes) */
   Elf32_Half e_shnum;                                                           /**< Shdr Table Count */
   Elf32_Half e_shstrndx;                                                        /**< Shdr String Table Index */

} Elf32_Ehdr;

/*
File Header
*/
typedef struct Elf64_Ehdr_s
{
   Elf64_UChar e_ident[EI_NIDENT];                                               /**< Magic Numbers */
   Elf64_Half e_type;                                                            /**< File Type */
   Elf64_Half e_machine;                                                         /**< Architecture */
   Elf64_Word e_version;                                                         /**< File Version */
   Elf64_Addr e_entry;                                                           /**< Entry Point (Virtual Address) */
   Elf64_Off e_phoff;                                                            /**< Program Header (File Offset) */
   Elf64_Off e_shoff;                                                            /**< Section Header (File Offset) */
   Elf64_Word e_flags;                                                           /**< Architecture Flags */
   Elf64_Half e_ehsize;                                                          /**< Header Size (Bytes) */
   Elf64_Half e_phentsize;                                                       /**< Phdr Size (Bytes) */
   Elf64_Half e_phnum;                                                           /**< Phdr Table Count */
   Elf64_Half e_shentsize;                                                       /**< Shdr Size (Bytes) */
   Elf64_Half e_shnum;                                                           /**< Shdr Table Count */
   Elf64_Half e_shstrndx;                                                        /**< Shdr String Table Index */

} Elf64_Ehdr;

/*
Section Header
*/
typedef struct Elf32_Shdr_s
{
   Elf32_Word sh_name;                                                           /**< Section Name (StrTab Index) */
   Elf32_Word sh_type;                                                           /**< Section Type */
   Elf32_Word sh_flags;                                                          /**< Section Flags */
   Elf32_Addr sh_addr;                                                           /**< Section Address (Virtual Execution Entry) */
   Elf32_Off sh_offset;                                                          /**< Section Offset (File Offset) */
   Elf32_Word sh_size;                                                           /**< Section Size (Bytes) */
   Elf32_Word sh_link;                                                           /**< Section Link (Next Section) */
   Elf32_Word sh_info;                                                           /**< Section Information */
   Elf32_Word sh_addralign;                                                      /**< Section Alignment */
   Elf32_Word sh_entsize;                                                        /**< Section Entry Size */

} Elf32_Shdr;

/*
Section Header
*/
typedef struct Elf64_Shdr_s
{
   Elf64_Word sh_name;                                                           /**< Section Name (StrTab Index) */
   Elf64_Word sh_type;                                                           /**< Section Type */
   Elf64_Xword sh_flags;                                                         /**< Section Flags */
   Elf64_Addr sh_addr;                                                           /**< Section Address (Virtual Execution Entry) */
   Elf64_Off sh_offset;                                                          /**< Section Offset (File Offset) */
   Elf64_Xword sh_size;                                                          /**< Section Size (Bytes) */
   Elf64_Word sh_link;                                                           /**< Section Link (Next Section) */
   Elf64_Word sh_info;                                                           /**< Section Information */
   Elf64_Xword sh_addralign;                                                     /**< Section Alignment */
   Elf64_Xword sh_entsize;                                                       /**< Section Entry Size */

} Elf64_Shdr;

/*
Symbol Table Entry
*/
typedef struct Elf32_Sym_s
{
   Elf32_Word st_name;                                                           /**< Symbol Name (StrTab Index) */
   Elf32_Addr st_value;                                                          /**< Symbol Value */
   Elf32_Word st_size;                                                           /**< Symbol Size */
   Elf32_UChar st_info;                                                          /**< Symbol Type */
   Elf32_UChar st_other;                                                         /**< Symbol Visibility */
   Elf32_Section st_shndx;                                                       /**< Section Index */

} Elf32_Sym;

/*
Symbol Table Entry
*/
typedef struct Elf64_Sym_s
{
   Elf64_Word st_name;                                                           /**< Symbol Name (StrTab Index) */
   Elf64_UChar st_info;                                                          /**< Symbol Type */
   Elf64_UChar st_other;                                                         /**< Symbol Visibility */
   Elf64_Section st_shndx;                                                       /**< Section Index */
   Elf64_Addr st_value;                                                          /**< Symbol Value */
   Elf64_Xword st_size;                                                          /**< Symbol Size */

} Elf64_Sym;

/*
SymInfo Table Entry
*/
typedef struct Elf32_Syminfo_s
{
   Elf32_Half si_boundto;                                                        /**< Symbol Direct Bindings */
   Elf32_Half si_flags;                                                          /**< Symbol Flags */

} Elf32_Syminfo;

/*
SymInfo Table Entry
*/
typedef struct Elf64_Syminfo_s
{
   Elf64_Half si_boundto;                                                        /**< Symbol Direct Bindings */
   Elf64_Half si_flags;                                                          /**< Symbol Flags */

} Elf64_Syminfo;

/*
Relocation Entry
*/
typedef struct Elf32_Rel_s
{
   Elf32_Addr r_offset;                                                          /**< Relocation Address */
   Elf32_Word r_info;                                                            /**< Relocation Type and Index */

} Elf32_Rel;

/*
Relocation Entry
*/
typedef struct Elf64_Rel_s
{
   Elf64_Addr r_offset;                                                          /**< Relocation Address */
   Elf64_Xword r_info;                                                           /**< Relocation Type and Index */

} Elf64_Rel;

/*
Relocation Addend Entry
*/
typedef struct Elf32_Rela_s
{
   Elf32_Addr r_offset;                                                          /**< Relocation Address */
   Elf32_Word r_info;                                                            /**< Relocation Type and Index */
   Elf32_Sword r_addend;                                                         /**< Relocation Addend */

} Elf32_Rela;

/*
Relocation Addend Entry
*/
typedef struct Elf64_Rela_s
{
   Elf64_Addr r_offset;                                                          /**< Relocation Address */
   Elf64_Xword r_info;                                                           /**< Relocation Type and Index */
   Elf64_Sxword r_addend;                                                        /**< Relocation Addend */

} Elf64_Rela;

/*
Program Section Header
*/
typedef struct Elf32_Phdr_s
{
   Elf32_Word p_type;                                                            /**< Segment Type */
   Elf32_Off p_offset;                                                           /**< Segment Offset (File Offset) */
   Elf32_Addr p_vaddr;                                                           /**< Segment Address (Virtual Address) */
   Elf32_Addr p_paddr;                                                           /**< Segment Address (Physical Address) */
   Elf32_Word p_filesz;                                                          /**< Segment Size (File) */
   Elf32_Word p_memsz;                                                           /**< Segment Size (Memory) */
   Elf32_Word p_flags;                                                           /**< Segment Flags */
   Elf32_Word p_align;                                                           /**< Segment Alignment */

} Elf32_Phdr;

/*
Program Section Header
*/
typedef struct Elf64_Phdr_s
{
   Elf64_Word p_type;                                                            /**< Segment Type */
   Elf64_Word p_flags;                                                           /**< Segment Flags */
   Elf64_Off p_offset;                                                           /**< Segment Offset (File Offset) */
   Elf64_Addr p_vaddr;                                                           /**< Segment Address (Virtual Address) */
   Elf64_Addr p_paddr;                                                           /**< Segment Address (Physical Address) */
   Elf64_Xword p_filesz;                                                         /**< Segment Size (File) */
   Elf64_Xword p_memsz;                                                          /**< Segment Size (Memory) */
   Elf64_Xword p_align;                                                          /**< Segment Alignment */

} Elf64_Phdr;

/*
Dynamic Section Entry
*/
typedef struct Elf32_Dyn_s
{
   Elf32_Sword d_tag;                                                            /**< Dynamic Section Entry Type */

   union
   {
      Elf32_Word d_val;                                                          /**< Value */
      Elf32_Addr d_ptr;                                                          /**< Address */

   } d_un;

} Elf32_Dyn;

/*
Dynamic Section Entry
*/
typedef struct Elf64_Dyn_s
{
   Elf64_Sxword d_tag;                                                           /**< Dynamic Section Entry Type */

   union
   {
      Elf64_Xword d_val;                                                         /**< Value */
      Elf64_Addr d_ptr;                                                          /**< Address */

   } d_un;

} Elf64_Dyn;

/*
Note Section Header
*/
typedef struct Elf32_Nhdr_s
{
   Elf32_Word n_namesz;                                                          /**< Note Name Size */
   Elf32_Word n_descsz;                                                          /**< Note Description Size */
   Elf32_Word n_type;                                                            /**< Note Type */

} Elf32_Nhdr;

/*
Note Section Header
*/
typedef struct Elf64_Nhdr_s
{
   Elf64_Word n_namesz;                                                          /**< Note Name Size */
   Elf64_Word n_descsz;                                                          /**< Note Description Size */
   Elf64_Word n_type;                                                            /**< Note Type */

} Elf64_Nhdr;

/*
File Header, Member e_ident
*/
enum                                                                             /**< Offset [EI_MAG0 .. EI_MAG3] */
{
   ELFMAG0 = 0x7f,                                                               /**< Magic Number */
   ELFMAG1 = 'E',                                                                /**< Magic Number */
   ELFMAG2 = 'L',                                                                /**< Magic Number */
   ELFMAG3 = 'F'                                                                 /**< Magic Number */
};

static const char Elf_Magic[] = { 0x7f, 'E', 'L', 'F', '\0' };                   /**< strncmp(.., Elf_Magic, sizeof(Elf_Magic)) */

enum                                                                             /**< Offset EI_CLASS */
{
   ELFCLASSNONE = 0,                                                             /**< Invalid class */
   ELFCLASS32 = 1,                                                               /**< 32 Bit */
   ELFCLASS64 = 2                                                                /**< 64 Bit */
};

enum                                                                             /**< Offset EI_DATA */
{
   ELFDATANONE = 0,                                                              /**< Invalid Endian */
   ELFDATA2LSB = 1,                                                              /**< Little Endian */
   ELFDATA2MSB = 2                                                               /**< Big Endian */
};

enum                                                                             /**< Offset EI_OSABI */
{
   ELFOSABI_NONE = 0,                                                            /**< Unknown */
   ELFOSABI_SYSV = 0,                                                            /**< UNIX System V */
   ELFOSABI_NETBSD = 2,                                                          /**< NetBSD */
   ELFOSABI_LINUX = 3,                                                           /**< Linux */
   ELFOSABI_FREEBSD = 9,                                                         /**< FreeBSD */
   ELFOSABI_OPENBSD = 12,                                                        /**< OpenBSD */
   ELFOSABI_ARM = 97,                                                            /**< ARM */
   ELFOSABI_STANDALONE = 255                                                     /**< Embedded */
};

/*
File Header, Member e_type
*/
enum
{
   ET_NONE = 0,                                                                  /**< Unknown */
   ET_REL = 1,                                                                   /**< Relocatable */
   ET_EXEC = 2,                                                                  /**< Executable */
   ET_DYN = 3,                                                                   /**< Shared Object */
   ET_CORE = 4                                                                   /**< Core */
};

/*
File Header, Member e_machine
*/
enum
{
   EM_NONE = 0,                                                                  /**< Unknown */
   EM_ARM = 40,                                                                  /**< ARM AArch32 */
   EM_HEXAGON = 164,                                                             /**< Qualcomm Hexagon */
   EM_AARCH64 = 183                                                              /**< ARM AArch64 */
};

/*
File Header, Member e_version
*/
enum
{
   EV_NONE = 0,                                                                  /**< Invalid */
   EV_CURRENT = 1                                                                /**< Current */
};

/*
Section Header, Special Sections Index
*/
enum
{
   SHN_UNDEF = 0                                                                 /**< Undefined */
};

/*
Section Header, Member sh_type
*/
enum
{
   SHT_NULL = 0,                                                                 /**< Unused */
   SHT_PROGBITS = 1,                                                             /**< Program Data */
   SHT_SYMTAB = 2,                                                               /**< Symbol Table */
   SHT_STRTAB = 3,                                                               /**< String Table */
   SHT_RELA = 4,                                                                 /**< Relocation Table, Addends */
   SHT_HASH = 5,                                                                 /**< Symbol Hash Table */
   SHT_DYNAMIC = 6,                                                              /**< Dynamic Linking */
   SHT_NOTE = 7,                                                                 /**< Notes */
   SHT_NOBITS = 8,                                                               /**< Program Data (BSS) */
   SHT_REL = 9,                                                                  /**< Relocation Table */
   SHT_SHLIB = 10,                                                               /**< Reserved */
   SHT_DYNSYM = 11,                                                              /**< Dynamic Linking Symbols */
   SHT_INIT_ARRAY = 14,                                                          /**< Constructors Array */
   SHT_FINI_ARRAY = 15,                                                          /**< Destructors Array */
   SHT_PREINIT_ARRAY = 16,                                                       /**< Preconstructors Array */
   SHT_GROUP = 17,                                                               /**< Section Group */
   SHT_SYMTAB_SHNDX = 18                                                         /**< Extended Section Index */
};

/*
Section Header, Member sh_flags
*/
enum
{
   SHF_WRITE = 0x0001,                                                           /**< Writable */
   SHF_ALLOC = 0x0002,                                                           /**< Allocates on Execution */
   SHF_EXECINSTR = 0x0004,                                                       /**< Executable */
   SHF_MERGE = 0x0010,                                                           /**< Mergable */
   SHF_STRINGS = 0x0020,                                                         /**< NULL Terminated Strings */
   SHF_INFO_LINK = 0x0040,                                                       /**< Link to Next in sh_info */
   SHF_LINK_ORDER = 0x0080,                                                      /**< Preserve Order */
   SHF_OS_NONCONFORMING = 0x0100,                                                /**< Non-standard OS Specific */
   SHF_GROUP = 0x0200,                                                           /**< Section Group Member */
   SHF_TLS = 0x00020,                                                            /**< Section TLS Container */
   SHF_MASKOS = 0x0ff00000,                                                      /**< OS Specific */
   SHF_MASKPROC = 0xf0000000                                                     /**< Architecture Specific */
};

/*
SymInfo Header, Member si_boundto
*/
enum
{
   SYMINFO_BT_SELF = 0xffff,                                                     /**< Symbol Binding Self */
   SYMINFO_BT_PARENT = 0xfffe,                                                   /**< Symbol Binding Parent */
   SYMINFO_BT_LOWRESERVE = 0xff00                                                /**< Begin Reserved Entries */
};

/*
SymInfo Header, Member si_flags
*/
enum
{
   SYMINFO_FLG_DIRECT = 0x0001,                                                  /**< Direct Binding */
   SYMINFO_FLG_PASSTHRU = 0x0002,                                                /**< Pass-thru to Translator */
   SYMINFO_FLG_COPY = 0x0004,                                                    /**< Copy Relocation */
   SYMINFO_FLG_LAZYLOAD = 0x0008                                                 /**< Lazy Loaded */
};

/*
SymInfo Header
*/
enum
{
   SYMINFO_NONE = 0,
   SYMINFO_CURRENT = 1
};

/*
Symbol Table Entry, Member st_info
*/
#define ELF32_ST_BIND(val) (((Elf_UChar)(val))>>4)
#define ELF32_ST_TYPE(val) ((val)&0x0f)
#define ELF32_ST_INFO(bind, type) (((bind)<<4)+((type)&0x0f))

#define ELF64_ST_BIND(val) ELF32_ST_BIND(val)
#define ELF64_ST_TYPE(val) ELF32_ST_TYPE(val)
#define ELF64_ST_INFO(bind, type) ELF32_ST_INFO(bind, type)

/*
Symbol Table Entry, Member st_info.st_bind
*/
enum
{
   STB_LOCAL = 0,                                                                /**< Local */
   STB_GLOBAL = 1,                                                               /**< Global */
   STB_WEAK = 2,                                                                 /**< Weak */
   STB_LOOS = 10,                                                                /**< OS Specific Begins */
   STB_HIOS = 12,                                                                /**< OS Specific Ends */
   STB_LOPROC = 13,                                                              /**< Architecture Specific Begins */
   STB_HIPROC = 15                                                               /**< Architecture Specific Ends */
};

/*
Symbol Table Entry, Member st_info.st_type
*/
enum
{
   STT_NOTYPE = 0,                                                               /**< No Type Specified */
   STT_OBJECT = 1,                                                               /**< Data */
   STT_FUNC = 2,                                                                 /**< Code */
   STT_SECTION = 3,                                                              /**< Section Association */
   STT_FILE = 4,                                                                 /**< File Name */
   STT_COMMON = 5,                                                               /**< COMMON Data */
   STT_TLS = 6,                                                                  /**< TLS Data */
   STT_LOOS = 10,                                                                /**< OS Specific Begins */
   STT_HIOS = 12,                                                                /**< OS Specific Ends */
   STT_LOPROC = 13,                                                              /**< Architecture Specific Begins */
   STT_HIPROC = 15                                                               /**< Architecture Specific Ends */
};

/*
Symbol Table Entry, Member st_name
*/
enum
{
   STN_UNDEF = 0                                                                  /**< Ends Chain */
};

/*
Symbol Table Entry, Member st_other
*/
#define ELF32_ST_VISIBILITY(o) ((o)&0x03)
#define ELF64_ST_VISIBILITY(o) ELF32_ST_VISIBILITY(o)

enum
{
   STV_DEFAULT = 0,                                                              /**< Default Visibility */
   STV_INTERNAL = 1,                                                             /**< Architecture Hidden */
   STV_HIDDEN = 2,                                                               /**< Hidden to Other Modules */
   STV_PROTECTED = 3                                                             /**< Not Exported */
};

/*
Relocation Entryy, Member r_info
*/
#define ELF32_R_SYM(val) ((val)>>8)
#define ELF32_R_TYPE(val) ((val)&0xff)
#define ELF32_R_INFO(sym, type) (((sym)<<8)+((type)&0xff))

#define ELF64_R_SYM(val) ((val)>>32)
#define ELF64_R_TYPE(val) ((val)&0xffffffff)
#define ELF64_R_INFO(sym, type) ((((Elf64_Xword)(sym))<<32)+(type))

/*
Program Section Header, Member p_type
*/
enum
{
   PT_NULL = 0,                                                                  /**< Program Header Table Empty */
   PT_LOAD = 1,                                                                  /**< Loadable Segment */
   PT_DYNAMIC = 2,                                                               /**< Dynamic Linking */
   PT_INTERP = 3,                                                                /**< Program Interpreter */
   PT_NOTE = 4,                                                                  /**< Note Segment */
   PT_SHLIB = 5,                                                                 /**< Reserved */
   PT_PHDR = 6,                                                                  /**< Program Header Self Reference */
   PT_TLS = 7,                                                                   /**< TLS Segment */
   PT_LOOS = 0x60000000,                                                         /**< OS Specific Begins */
   PT_HIOS = 0x6fffffff,                                                         /**< OS Specific Ends */
   PT_LOPROC = 0x70000000,                                                       /**< Architecture Specific Begins */
   PT_HIPROC = 0x7fffffff                                                        /**< Architecture Specific Ends */
};

/*
Program Section Header, Member p_flags
*/
enum
{
   PF_X = 0x0001,                                                                /**< Segment Executable */
   PF_W = 0x0002,                                                                /**< Segment Writable */
   PF_R = 0x0004,                                                                /**< Segment Readable */
   PF_MASKOS = 0x0ff00000,                                                       /**< OS Specific */
   PF_MASKPROC = 0xf0000000                                                      /**< Architecture Specific */
};

/*
Note Segment Descriptor Types for Core Files
*/
enum
{
   NT_NONE = 0,
   NT_PRSTATUS = 1,                                                              /**< Note struct prstatus */
   NT_FPREGSET = 2,                                                              /**< Note struct fpregset */
   NT_PRPSINFO = 3,                                                              /**< Note struct prpsinfo */
   NT_PRXREG = 4,                                                                /**< Note struct prxregset */
   NT_TASKSTRUCT = 4,                                                            /**< Note struct task control */
   NT_PLATFORM = 5,                                                              /**< Note sysinfo(SI_PLATFORM) */
   NT_AUXV = 6,                                                                  /**< Note auxv[] */
   NT_GWINDOWS = 7,                                                              /**< Note struct gwindows */
   NT_ASRS = 8,                                                                  /**< Note struct asrset */
   NT_PSTATUS = 10,                                                              /**< Note struct pstatus */
   NT_PSINFO = 13,                                                               /**< Note struct psinfo */
   NT_PRCRED = 14,                                                               /**< Note struct prcred */
   NT_UTSNAME = 15,                                                              /**< Note struct utsname */
   NT_LWPSTATUS = 16,                                                            /**< Note struct lwpstatus */
   NT_LWPSINFO = 17,                                                             /**< Note struct lwpinfo */
   NT_PRFPXREG = 20                                                              /**< Note struct fprxregset */
};

enum
{
   NT_VERSION = 1                                                                /**< Version String */
};

/*
Dynamic Section Entry, Member d_tag
*/
enum
{
   DT_NULL = 0,                                                                  /**< Dynamic Section End */
   DT_NEEDED = 1,                                                                /**< Dependent Library */
   DT_PLTRELSZ = 2,                                                              /**< PLT Relocs Size */
   DT_PLTGOT = 3,                                                                /**< Architecture Defined */
   DT_HASH = 4,                                                                  /**< Symbol Hash Table Address */
   DT_STRTAB = 5,                                                                /**< String Table Address */
   DT_SYMTAB = 6,                                                                /**< Symbol Table Address */
   DT_RELA = 7,                                                                  /**< Reloca Table Address */
   DT_RELASZ = 8,                                                                /**< Reloca Table Size */
   DT_RELAENT = 9,                                                               /**< Sizeof Reloca Entry */
   DT_STRSZ = 10,                                                                /**< Sizeof String Table */
   DT_SYMENT = 11,                                                               /**< Sizeof Symbol Table Entry */
   DT_INIT = 12,                                                                 /**< Constructors Table Address */
   DT_FINI = 13,                                                                 /**< Destructors Table Address */
   DT_SONAME = 14,                                                               /**< Shared Object Name */
   DT_RPATH = 15,                                                                /**< Library Search Path (Deprecated) */
   DT_SYMBOLIC = 16,                                                             /**< Symbol Search Begins With */
   DT_REL = 17,                                                                  /**< Rel Table Address */
   DT_RELSZ = 18,                                                                /**< Rel Table Size */
   DT_RELENT = 19,                                                               /**< Sizeof Rel Entry */
   DT_PLTREL = 20,                                                               /**< PLT Rel Type */
   DT_DEBUG = 21,                                                                /**< Debugging */
   DT_TEXTREL = 22,                                                              /**< Rel Modifies .text */
   DT_JMPREL = 23,                                                               /**< PLT Rel Address */
   DT_BIND_NOW = 24,                                                             /**< Process Relocations */
   DT_INIT_ARRAY = 25,                                                           /**< Constructors Array Address */
   DT_FINI_ARRAY = 26,                                                           /**< Destructors Array Address */
   DT_INIT_ARRAYSZ = 27,                                                         /**< Constructors Array Size */
   DT_FINI_ARRAYSZ = 28,                                                         /**< Destructors Array Size */
   DT_RUNPATH = 29,                                                              /**< Library Search Path */
   DT_FLAGS = 30,                                                                /**< Flags of Loading Object */
   DT_ENCODING = 32,                                                             /**< Encoded Range Begins */
   DT_PREINIT_ARRAY = 32,                                                        /**< Preconstructor Array Address */
   DT_PREINIT_ARRAYSZ = 33,                                                      /**< Preconstructor Array Size */
   DT_LOOS = 0x6000000d,                                                         /**< OS Specific Begins */
   DT_HIOS = 0x6ffff000,                                                         /**< OS Specific Ends */
   DT_LOPROC = 0x70000000,                                                       /**< Architecture Specific Begins */
   DT_HIPROC = 0x7fffffff,                                                       /**< Architecture Specific Ends */
   DT_VERSYM = 0x6ffffff0,
   DT_RELACOUNT = 0x6ffffff9,
   DT_RELCOUNT = 0x6ffffffa,
   DT_FLAGS_1 = 0x6ffffffb,                                                      /**< State flags, see DF_1_* below */
   DT_VERDEF = 0x6ffffffc,                                                       /**< Version Definition Table Address */
   DT_VERDEFNUM = 0x6ffffffd,                                                    /**< Version Definition Table Size */
   DT_VERNEED = 0x6ffffffe,                                                      /**< Version Needed Table Address */
   DT_VERNEEDNUM = 0x6fffffff                                                    /**< Version Needed Table Size */
};

/*
Dynamic Section Entry, Member d_un.d_val
*/
enum
{
   DF_ORIGIN = 0x00000001,                                                       /**< Uses DF_ORIGIN */
   DF_SYMBOLIC = 0x00000002,                                                     /**< Resolutions Begins */
   DF_TEXTREL = 0x00000004,                                                      /**< .text Relocations */
   DF_BIND_NOW = 0x00000008,                                                     /**< No Lazy Binding */
   DF_STATIC_TLS = 0x00000010                                                    /**< TLS Model Static */
};

/*
Dynamic Section Entry, Member d_un.d_val, DT_FLAGS_1
*/
enum
{
   DF_1_NOW = 0x00000001,                                                        /**< Set RTLD_NOW */
   DF_1_GLOBAL = 0x00000002,                                                     /**< Set RTLD_GLOBAL */
   DF_1_GROUP = 0x00000004,                                                      /**< Set RTLD_GROUP */
   DF_1_NODELETE = 0x00000008,                                                   /**< Set RTLD_NODELETE */
   DF_1_LOADFLTR = 0x00000010,                                                   /**< Trigger Filtee Loading */
   DF_1_INITFIRST = 0x00000020,                                                  /**< Set RTLD_INITFIRST */
   DF_1_NOOPEN = 0x00000040,                                                     /**< Set RTLD_NOOPEN */
   DF_1_ORIGIN = 0x00000080,                                                     /**< $ORIGIN Handled */
   DF_1_DIRECT = 0x00000100,                                                     /**< Direct Binding */
   DF_1_TRANS = 0x00000200,
   DF_1_INTERPOSE = 0x00000400,                                                  /**< Object Interposes */
   DF_1_NODEFLIB = 0x00000800,                                                   /**< Ignore Default Lib Search Path */
   DF_1_NODUMP = 0x00001000,                                                     /**< Object Does Not Support Dump */
   DF_1_CONFALT = 0x00002000,                                                    /**< Configuration Alternative */
   DF_1_ENDFILTEE = 0x00004000,                                                  /**< Filtee Terminates Filters Search */
   DF_1_DISPRELDNE = 0x00008000,                                                 /**< Disp Reloc Build Time */
   DF_1_DISPRELPND = 0x00010000                                                  /**< Disp Reloc Run Time */
};


enum
{
   DTF_1_PARINIT = 0x00000001,                                                   /**< Flags of DT_FEATURE_1 */
   DTF_1_CONFEXP = 0x00000002                                                    /**< Flags of DT_FEATURE_1 */
};

enum
{
   DF_P1_LAZYLOAD = 0x00000001,                                                  /**< Lazy Load Next */
   DF_P1_GROUPPERM = 0x00000002                                                  /**< Symbols From Next Not Available */
};

/*
API
*/

typedef enum { PD_ELF_STATUS_SUCCESS = 0, PD_ELF_STATUS_ERROR = -1 } PD_ELF_STATUS;
typedef enum { PD_ELF_BUFFER , PD_ELF_LIST } PD_ELF_ADDR_TYPE;


typedef struct pd_elf_prgtab_s
{
   PD_ELF_ADDR_TYPE addr_type;

   void* addr;

   Elf32_Word size;

   Elf32_Addr unmapped_addr;

   Elf32_Word p_type;
   
   Elf32_Phdr * phdr;

   struct pd_elf_prgtab_s* next;

} pd_elf_prgtab_t, *pd_elf_prgtab_p;

typedef struct pd_elf_sectab_s
{
   Elf32_Shdr shdr;

   struct pd_elf_sectab_s* next;

} pd_elf_sectab_t, *pd_elf_sectab_p, **pd_elf_sectab_pp;


Elf32_Word pd_elf_hash(Elf32_UChar const* name);

PD_ELF_STATUS pd_elf_init(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, pd_elf_sectab_p* sectab_p);
PD_ELF_STATUS pd_elf_prgtab_alloc(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, Elf32_Word type, char* name, Elf32_Word ntype, void* addr, Elf32_Word size);
PD_ELF_STATUS pd_elf_prgtab_alloc_list(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, uint32 list_sz, void* head_ptr, uint32 data_sz, void* virt_addr, uint32 virt_sz);
PD_ELF_STATUS pd_elf_sectab_alloc(Elf32_Ehdr* header_p, pd_elf_sectab_p* sectab_p, Elf32_Word type, Elf32_Word link);
PD_ELF_STATUS pd_elf_prgtab_prepare_header(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, void ** prgtab_phdr, unsigned int prgtab_phdr_num );
PD_ELF_STATUS pd_elf_prepare_offsets(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, pd_elf_sectab_p* sectab_p);
PD_ELF_STATUS pd_elf_write(int fd, Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, pd_elf_sectab_p* sectab_p, void ** prgtab_phdr);
PD_ELF_STATUS pd_elf_term(Elf32_Ehdr* header_p, pd_elf_prgtab_p* prgtab_p, pd_elf_sectab_p* sectab_p, void **prgtab_phdr);

#if defined(__cplusplus)
}
#endif

#endif

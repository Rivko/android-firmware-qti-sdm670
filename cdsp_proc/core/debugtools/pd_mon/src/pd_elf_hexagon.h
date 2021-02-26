#ifndef PD_ELF_HEXAGON_H
#define PD_ELF_HEXAGON_H
/** vi: tw=128 ts=3 sw=3 et
@file pd_elf_hexagon.h
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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/pd_mon/src/pd_elf_hexagon.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#if defined(__cplusplus)
extern "C"
{
#endif

struct regfile_hexagon
{
   unsigned long r0;
   unsigned long r1;
   unsigned long r2;
   unsigned long r3;
   unsigned long r4;
   unsigned long r5;
   unsigned long r6;
   unsigned long r7;
   unsigned long r8;
   unsigned long r9;
   unsigned long r10;
   unsigned long r11;
   unsigned long r12;
   unsigned long r13;
   unsigned long r14;
   unsigned long r15;
   unsigned long r16;
   unsigned long r17;
   unsigned long r18;
   unsigned long r19;
   unsigned long r20;
   unsigned long r21;
   unsigned long r22;
   unsigned long r23;
   unsigned long r24;
   unsigned long r25;
   unsigned long r26;
   unsigned long r27;
   unsigned long r28;
   unsigned long r29;
   unsigned long r30;
   unsigned long r31;
   unsigned long sa0;
   unsigned long lc0;
   unsigned long sa1;
   unsigned long lc1;
   unsigned long m0;
   unsigned long m1;
   unsigned long usr;
   unsigned long p3_0;
   unsigned long gp;
   unsigned long ugp;
   unsigned long pc;
   unsigned long cause;
   unsigned long badva;
#if defined(HEXAGON_ARCH_VERSION) && HEXAGON_ARCH_VERSION < 4
   unsigned long pad1;  /* pad out to 48 words total */
   unsigned long pad2;  /* pad out to 48 words total */
   unsigned long pad3;  /* pad out to 48 words total */
#else
   unsigned long cs0;
   unsigned long cs1;
   unsigned long pad1;  /* pad out to 48 words total */
#endif
};

/*
HEXAGON, Header Flags
*/

enum
{
   EF_HEXAGON_MACH_V2 = 0x00000001,                                              /**< Hexagon V2 */
   EF_HEXAGON_MACH_V3 = 0x00000002,                                              /**< Hexagon V3 */
   EF_HEXAGON_MACH_V4 = 0x00000003,                                              /**< Hexagon V4 */
   EF_HEXAGON_MACH_V5 = 0x00000004,                                              /**< Hexagon V5 */
   EF_HEXAGON_ISA_MACH = 0x00000000,                                             /**< Same as specified in bits[3:0] of e_flags */
   EF_HEXAGON_ISA_V2 = 0x00000010,                                               /**< Hexagon V2 ISA */
   EF_HEXAGON_ISA_V3 = 0x00000020,                                               /**< Hexagon V3 ISA */
   EF_HEXAGON_ISA_V4 = 0x00000030,                                               /**< Hexagon V4 ISA */
   EF_HEXAGON_ISA_V5 = 0x00000040                                                /**< Hexagon V5 ISA */
};

enum
{
   SHN_HEXAGON_SCOMMON = 0xff00,                                                 /**< Other access sizes */
   SHN_HEXAGON_SCOMMON_1 = 0xff01,                                               /**< Byte-sized access */
   SHN_HEXAGON_SCOMMON_2 = 0xff02,                                               /**< Half-word-sized access */
   SHN_HEXAGON_SCOMMON_4 = 0xff03,                                               /**< Word-sized access */
   SHN_HEXAGON_SCOMMON_8 = 0xff04                                                /**< Double-word-size access */
};

enum
{
   R_HEX_NONE = 0,
   R_HEX_B22_PCREL = 1,
   R_HEX_B15_PCREL = 2,
   R_HEX_B7_PCREL = 3,
   R_HEX_LO16 = 4,
   R_HEX_HI16 = 5,
   R_HEX_32 = 6,
   R_HEX_16 = 7,
   R_HEX_8 = 8,
   R_HEX_GPREL16_0 = 9,
   R_HEX_GPREL16_1 = 10,
   R_HEX_GPREL16_2 = 11,
   R_HEX_GPREL16_3 = 12,
   R_HEX_HL16 = 13,
   R_HEX_B13_PCREL = 14,
   R_HEX_B9_PCREL = 15,
   R_HEX_B32_PCREL_X = 16,
   R_HEX_32_6_X = 17,
   R_HEX_B22_PCREL_X = 18,
   R_HEX_B15_PCREL_X = 19,
   R_HEX_B13_PCREL_X = 20,
   R_HEX_B9_PCREL_X = 21,
   R_HEX_B7_PCREL_X = 22,
   R_HEX_16_X = 23,
   R_HEX_12_X = 24,
   R_HEX_11_X = 25,
   R_HEX_10_X = 26,
   R_HEX_9_X = 27,
   R_HEX_8_X = 28,
   R_HEX_7_X = 29,
   R_HEX_6_X = 30,
   R_HEX_32_PCREL = 31,
   R_HEX_COPY = 32,
   R_HEX_GLOB_DAT = 33,
   R_HEX_JMP_SLOT = 34,
   R_HEX_RELATIVE = 35,
   R_HEX_PLT_B22_PCREL = 36,
   R_HEX_GOTREL_LO16 = 37,
   R_HEX_GOTREL_HI16 = 38,
   R_HEX_GOTREL_32 = 39,
   R_HEX_GOT_LO16 = 40,
   R_HEX_GOT_HI16 = 41,
   R_HEX_GOT_32 = 42,
   R_HEX_GOT_16 = 43,
   R_HEX_DTPMOD_32 = 44,
   R_HEX_DTPREL_LO16 = 45,
   R_HEX_DTPREL_HI16 = 46,
   R_HEX_DTPREL_32 = 47,
   R_HEX_DTPREL_16 = 48,
   R_HEX_GD_PLT_B22_PCREL = 49,
   R_HEX_GD_GOT_LO16 = 50,
   R_HEX_GD_GOT_HI16 = 51,
   R_HEX_GD_GOT_32 = 52,
   R_HEX_GD_GOT_16 = 53,
   R_HEX_IE_LO16 = 54,
   R_HEX_IE_HI16 = 55,
   R_HEX_IE_32 = 56,
   R_HEX_IE_GOT_LO16 = 57,
   R_HEX_IE_GOT_HI16 = 58,
   R_HEX_IE_GOT_32 = 59,
   R_HEX_IE_GOT_16 = 60,
   R_HEX_TPREL_LO16 = 61,
   R_HEX_TPREL_HI16 = 62,
   R_HEX_TPREL_32 = 63,
   R_HEX_TPREL_16 = 64,
   R_HEX_6_PCREL_X = 65,
   R_HEX_GOTREL_32_6_X = 66,
   R_HEX_GOTREL_16_X = 67,
   R_HEX_GOTREL_11_X = 68,
   R_HEX_GOT_32_6_X = 69,
   R_HEX_GOT_16_X = 70,
   R_HEX_GOT_11_X = 71,
   R_HEX_DTPREL_32_6_X = 72,
   R_HEX_DTPREL_16_X = 73,
   R_HEX_DTPREL_11_X = 74,
   R_HEX_GD_GOT_32_6_X = 75,
   R_HEX_GD_GOT_16_X = 76,
   R_HEX_GD_GOT_11_X = 77,
   R_HEX_IE_32_6_X = 78,
   R_HEX_IE_16_X = 79,
   R_HEX_IE_GOT_32_6_X = 80,
   R_HEX_IE_GOT_16_X = 81,
   R_HEX_IE_GOT_11_X = 82,
   R_HEX_TPREL_32_6_X = 83,
   R_HEX_TPREL_16_X = 84,
   R_HEX_TPREL_11_X = 85
};

#define NT_HEXAGON_REGFILE             0x60000000 // .note type
#define NT_HEXAGON_DUMP_NAME           0x20000000 // .note type; arbitrary note type
#define NT_HEXAGON_DUMP_PROCESS        0x20000001 // .note type; arbitrary note type
#define NT_HEXAGON_DUMP_VERSION        0x20000002 // .note type; arbitrary note type
#define NT_HEXAGON_DUMP_CONTEXT        0x20000003 // .note type; arbitrary note type

#if defined(__cplusplus)
}
#endif

#endif

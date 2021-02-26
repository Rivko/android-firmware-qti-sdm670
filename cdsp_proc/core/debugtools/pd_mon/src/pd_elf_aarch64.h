#ifndef PD_ELF_AARCH64_H
#define PD_ELF_AARCH64_H
/** vi: tw=128 ts=3 sw=3 et
@file pd_elf_aarch64.h
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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/pd_mon/src/pd_elf_aarch64.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#include "pd_elf_aarch32.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct regfile_aarch64
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
};

enum
{
   R_AARCH64_NONE = 0x100,
   R_AARCH64_ABS64 = 0x101,
   R_AARCH64_ABS32 = 0x102,
   R_AARCH64_ABS16 = 0x103,
   R_AARCH64_PREL64 = 0x104,
   R_AARCH64_PREL32 = 0x105,
   R_AARCH64_PREL16 = 0x106,
   R_AARCH64_MOVW_UABS_G0 = 0x107,
   R_AARCH64_MOVW_UABS_G0_NC = 0x108,
   R_AARCH64_MOVW_UABS_G1 = 0x109,
   R_AARCH64_MOVW_UABS_G1_NC = 0x10a,
   R_AARCH64_MOVW_UABS_G2 = 0x10b,
   R_AARCH64_MOVW_UABS_G2_NC = 0x10c,
   R_AARCH64_MOVW_UABS_G3 = 0x10d,
   R_AARCH64_MOVW_SABS_G0 = 0x10e,
   R_AARCH64_MOVW_SABS_G1 = 0x10f,
   R_AARCH64_MOVW_SABS_G2 = 0x110,
   R_AARCH64_LD_PREL_LO19 = 0x111,
   R_AARCH64_ADR_PREL_LO21 = 0x112,
   R_AARCH64_ADR_PREL_PG_HI21 = 0x113,
   R_AARCH64_ADD_ABS_LO12_NC = 0x115,
   R_AARCH64_LDST8_ABS_LO12_NC = 0x116,
   R_AARCH64_TSTBR14 = 0x117,
   R_AARCH64_CONDBR19 = 0x118,
   R_AARCH64_JUMP26 = 0x11a,
   R_AARCH64_CALL26 = 0x11b,
   R_AARCH64_LDST16_ABS_LO12_NC = 0x11c,
   R_AARCH64_LDST32_ABS_LO12_NC = 0x11d,
   R_AARCH64_LDST64_ABS_LO12_NC = 0x11e,
   R_AARCH64_LDST128_ABS_LO12_NC = 0x12b,
   R_AARCH64_ADR_GOT_PAGE = 0x137,
   R_AARCH64_LD64_GOT_LO12_NC = 0x138,
   R_AARCH64_TLSLD_MOVW_DTPREL_G2 = 0x20b,
   R_AARCH64_TLSLD_MOVW_DTPREL_G1 = 0x20c,
   R_AARCH64_TLSLD_MOVW_DTPREL_G1_NC = 0x20d,
   R_AARCH64_TLSLD_MOVW_DTPREL_G0 = 0x20e,
   R_AARCH64_TLSLD_MOVW_DTPREL_G0_NC = 0x20f,
   R_AARCH64_TLSLD_ADD_DTPREL_HI12 = 0x210,
   R_AARCH64_TLSLD_ADD_DTPREL_LO12 = 0x211,
   R_AARCH64_TLSLD_ADD_DTPREL_LO12_NC = 0x212,
   R_AARCH64_TLSLD_LDST8_DTPREL_LO12 = 0x213,
   R_AARCH64_TLSLD_LDST8_DTPREL_LO12_NC = 0x214,
   R_AARCH64_TLSLD_LDST16_DTPREL_LO12 = 0x215,
   R_AARCH64_TLSLD_LDST16_DTPREL_LO12_NC = 0x216,
   R_AARCH64_TLSLD_LDST32_DTPREL_LO12 = 0x217,
   R_AARCH64_TLSLD_LDST32_DTPREL_LO12_NC = 0x218,
   R_AARCH64_TLSLD_LDST64_DTPREL_LO12 = 0x219,
   R_AARCH64_TLSLD_LDST64_DTPREL_LO12_NC = 0x21a,
   R_AARCH64_TLSIE_MOVW_GOTTPREL_G1 = 0x21b,
   R_AARCH64_TLSIE_MOVW_GOTTPREL_G0_NC = 0x21c,
   R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21 = 0x21d,
   R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC = 0x21e,
   R_AARCH64_TLSIE_LD_GOTTPREL_PREL19 = 0x21f,
   R_AARCH64_TLSLE_MOVW_TPREL_G2 = 0x220,
   R_AARCH64_TLSLE_MOVW_TPREL_G1 = 0x221,
   R_AARCH64_TLSLE_MOVW_TPREL_G1_NC = 0x222,
   R_AARCH64_TLSLE_MOVW_TPREL_G0 = 0x223,
   R_AARCH64_TLSLE_MOVW_TPREL_G0_NC = 0x224,
   R_AARCH64_TLSLE_ADD_TPREL_HI12 = 0x225,
   R_AARCH64_TLSLE_ADD_TPREL_LO12 = 0x226,
   R_AARCH64_TLSLE_ADD_TPREL_LO12_NC = 0x227,
   R_AARCH64_TLSLE_LDST8_TPREL_LO12 = 0x228,
   R_AARCH64_TLSLE_LDST8_TPREL_LO12_NC = 0x229,
   R_AARCH64_TLSLE_LDST16_TPREL_LO12 = 0x22a,
   R_AARCH64_TLSLE_LDST16_TPREL_LO12_NC = 0x22b,
   R_AARCH64_TLSLE_LDST32_TPREL_LO12 = 0x22c,
   R_AARCH64_TLSLE_LDST32_TPREL_LO12_NC = 0x22d,
   R_AARCH64_TLSLE_LDST64_TPREL_LO12 = 0x22e,
   R_AARCH64_TLSLE_LDST64_TPREL_LO12_NC = 0x22f,
   R_AARCH64_TLSDESC_ADR_PAGE = 0x232,
   R_AARCH64_TLSDESC_LD64_LO12_NC = 0x233,
   R_AARCH64_TLSDESC_ADD_LO12_NC = 0x234,
   R_AARCH64_TLSDESC_CALL = 0x239
};

#if defined(__cplusplus)
}
#endif

#endif

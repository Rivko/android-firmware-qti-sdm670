/***********************************************************************
 * fs_unicode_sjis.c
 *
 * Conversion tables for Shift-JIS (OEM Codepage 932)
 * Copyright (C) 2007, 2009, 2012 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-20   wek   Cleanup customer header file inclusion.
2009-04-22   ebb   Removed memory protection support.
2007-07-10   sh    Added Shift-JIS short filename conversion support.

===========================================================================*/

#include "fs_config_i.h"        /* For FEATURE_EFS_CONVERT_SHIFT_JIS flag */
#include "fs_unicode.h"
#include "fs_unicode_sjis.h"

/*
 * This (huge) switch tree maps a Shift-JIS (Codepage 932) DCBS value
 * to a Unicode codepoint.  The values are based on this reference:
 * http://www.microsoft.com/globaldev/reference/dbcs/932.mspx
 *
 * The Unicode translations are derived from Codepage 932 table here:
 * http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP932.TXT
 * Dated 04/15/98
 *
 * This large series of switch statements is not as evil as it looks
 * -- most compilers translate this into a nice tree of range-check
 * logic and lookup tables.
 *
 * Ideally, this would be one flat list of cases for each dcbs word,
 * but Lint correctly asserts that ANSI C compilers are not required
 * to honor more than 1023 cases.  For this reason, it is broken up,
 * by lead byte, into multiple switch statements.  The generated code
 * is largely the same.
 *
 * Targets that chose not to have Shift-JIS support may exclude this
 * table (for ROM space savings) by undefining
 * FEATURE_EFS_CONVERT_SHIFT_JIS.
 */

/* Note that on some machines, Lint8 takes approximately *77 MINUTES*
 * to lint this file, producing no warnings or errors.  This flag
 * prevents Lint from seeing the code so that Lint can complete in
 * normal time.  When making changes to this file, disable this
 * supression and make at least one pass with Lint. */
#ifdef _lint
#undef FEATURE_EFS_CONVERT_SHIFT_JIS
#endif

UTF16
fs_sjis_dcbs_to_utf16 (unsigned char high_byte, unsigned char low_byte)
{
#ifndef FEATURE_EFS_CONVERT_SHIFT_JIS
  (void) low_byte;
  (void) high_byte;             /* Arguments are ignored. */
  return FS_SJIS_UNKNOWN;       /* All characters become unknown */
#else
  switch (high_byte)
  {
// Auto-generated code BEGIN:
    /* Lead Byte: 0x81xx */
    case 0x81:
      switch (low_byte)
      {
        case 0x40:            /* 0x8140 */
          return 0x3000;
        case 0x41:            /* 0x8141 */
          return 0x3001;
        case 0x42:            /* 0x8142 */
          return 0x3002;
        case 0x43:            /* 0x8143 */
          return 0xFF0C;
        case 0x44:            /* 0x8144 */
          return 0xFF0E;
        case 0x45:            /* 0x8145 */
          return 0x30FB;
        case 0x46:            /* 0x8146 */
          return 0xFF1A;
        case 0x47:            /* 0x8147 */
          return 0xFF1B;
        case 0x48:            /* 0x8148 */
          return 0xFF1F;
        case 0x49:            /* 0x8149 */
          return 0xFF01;
        case 0x4A:            /* 0x814A */
          return 0x309B;
        case 0x4B:            /* 0x814B */
          return 0x309C;
        case 0x4C:            /* 0x814C */
          return 0x00B4;
        case 0x4D:            /* 0x814D */
          return 0xFF40;
        case 0x4E:            /* 0x814E */
          return 0x00A8;
        case 0x4F:            /* 0x814F */
          return 0xFF3E;
        case 0x50:            /* 0x8150 */
          return 0xFFE3;
        case 0x51:            /* 0x8151 */
          return 0xFF3F;
        case 0x52:            /* 0x8152 */
          return 0x30FD;
        case 0x53:            /* 0x8153 */
          return 0x30FE;
        case 0x54:            /* 0x8154 */
          return 0x309D;
        case 0x55:            /* 0x8155 */
          return 0x309E;
        case 0x56:            /* 0x8156 */
          return 0x3003;
        case 0x57:            /* 0x8157 */
          return 0x4EDD;
        case 0x58:            /* 0x8158 */
          return 0x3005;
        case 0x59:            /* 0x8159 */
          return 0x3006;
        case 0x5A:            /* 0x815A */
          return 0x3007;
        case 0x5B:            /* 0x815B */
          return 0x30FC;
        case 0x5C:            /* 0x815C */
          return 0x2015;
        case 0x5D:            /* 0x815D */
          return 0x2010;
        case 0x5E:            /* 0x815E */
          return 0xFF0F;
        case 0x5F:            /* 0x815F */
          return 0xFF3C;
        case 0x60:            /* 0x8160 */
          return 0xFF5E;
        case 0x61:            /* 0x8161 */
          return 0x2225;
        case 0x62:            /* 0x8162 */
          return 0xFF5C;
        case 0x63:            /* 0x8163 */
          return 0x2026;
        case 0x64:            /* 0x8164 */
          return 0x2025;
        case 0x65:            /* 0x8165 */
          return 0x2018;
        case 0x66:            /* 0x8166 */
          return 0x2019;
        case 0x67:            /* 0x8167 */
          return 0x201C;
        case 0x68:            /* 0x8168 */
          return 0x201D;
        case 0x69:            /* 0x8169 */
          return 0xFF08;
        case 0x6A:            /* 0x816A */
          return 0xFF09;
        case 0x6B:            /* 0x816B */
          return 0x3014;
        case 0x6C:            /* 0x816C */
          return 0x3015;
        case 0x6D:            /* 0x816D */
          return 0xFF3B;
        case 0x6E:            /* 0x816E */
          return 0xFF3D;
        case 0x6F:            /* 0x816F */
          return 0xFF5B;
        case 0x70:            /* 0x8170 */
          return 0xFF5D;
        case 0x71:            /* 0x8171 */
          return 0x3008;
        case 0x72:            /* 0x8172 */
          return 0x3009;
        case 0x73:            /* 0x8173 */
          return 0x300A;
        case 0x74:            /* 0x8174 */
          return 0x300B;
        case 0x75:            /* 0x8175 */
          return 0x300C;
        case 0x76:            /* 0x8176 */
          return 0x300D;
        case 0x77:            /* 0x8177 */
          return 0x300E;
        case 0x78:            /* 0x8178 */
          return 0x300F;
        case 0x79:            /* 0x8179 */
          return 0x3010;
        case 0x7A:            /* 0x817A */
          return 0x3011;
        case 0x7B:            /* 0x817B */
          return 0xFF0B;
        case 0x7C:            /* 0x817C */
          return 0xFF0D;
        case 0x7D:            /* 0x817D */
          return 0x00B1;
        case 0x7E:            /* 0x817E */
          return 0x00D7;
        case 0x80:            /* 0x8180 */
          return 0x00F7;
        case 0x81:            /* 0x8181 */
          return 0xFF1D;
        case 0x82:            /* 0x8182 */
          return 0x2260;
        case 0x83:            /* 0x8183 */
          return 0xFF1C;
        case 0x84:            /* 0x8184 */
          return 0xFF1E;
        case 0x85:            /* 0x8185 */
          return 0x2266;
        case 0x86:            /* 0x8186 */
          return 0x2267;
        case 0x87:            /* 0x8187 */
          return 0x221E;
        case 0x88:            /* 0x8188 */
          return 0x2234;
        case 0x89:            /* 0x8189 */
          return 0x2642;
        case 0x8A:            /* 0x818A */
          return 0x2640;
        case 0x8B:            /* 0x818B */
          return 0x00B0;
        case 0x8C:            /* 0x818C */
          return 0x2032;
        case 0x8D:            /* 0x818D */
          return 0x2033;
        case 0x8E:            /* 0x818E */
          return 0x2103;
        case 0x8F:            /* 0x818F */
          return 0xFFE5;
        case 0x90:            /* 0x8190 */
          return 0xFF04;
        case 0x91:            /* 0x8191 */
          return 0xFFE0;
        case 0x92:            /* 0x8192 */
          return 0xFFE1;
        case 0x93:            /* 0x8193 */
          return 0xFF05;
        case 0x94:            /* 0x8194 */
          return 0xFF03;
        case 0x95:            /* 0x8195 */
          return 0xFF06;
        case 0x96:            /* 0x8196 */
          return 0xFF0A;
        case 0x97:            /* 0x8197 */
          return 0xFF20;
        case 0x98:            /* 0x8198 */
          return 0x00A7;
        case 0x99:            /* 0x8199 */
          return 0x2606;
        case 0x9A:            /* 0x819A */
          return 0x2605;
        case 0x9B:            /* 0x819B */
          return 0x25CB;
        case 0x9C:            /* 0x819C */
          return 0x25CF;
        case 0x9D:            /* 0x819D */
          return 0x25CE;
        case 0x9E:            /* 0x819E */
          return 0x25C7;
        case 0x9F:            /* 0x819F */
          return 0x25C6;
        case 0xA0:            /* 0x81A0 */
          return 0x25A1;
        case 0xA1:            /* 0x81A1 */
          return 0x25A0;
        case 0xA2:            /* 0x81A2 */
          return 0x25B3;
        case 0xA3:            /* 0x81A3 */
          return 0x25B2;
        case 0xA4:            /* 0x81A4 */
          return 0x25BD;
        case 0xA5:            /* 0x81A5 */
          return 0x25BC;
        case 0xA6:            /* 0x81A6 */
          return 0x203B;
        case 0xA7:            /* 0x81A7 */
          return 0x3012;
        case 0xA8:            /* 0x81A8 */
          return 0x2192;
        case 0xA9:            /* 0x81A9 */
          return 0x2190;
        case 0xAA:            /* 0x81AA */
          return 0x2191;
        case 0xAB:            /* 0x81AB */
          return 0x2193;
        case 0xAC:            /* 0x81AC */
          return 0x3013;
        case 0xB8:            /* 0x81B8 */
          return 0x2208;
        case 0xB9:            /* 0x81B9 */
          return 0x220B;
        case 0xBA:            /* 0x81BA */
          return 0x2286;
        case 0xBB:            /* 0x81BB */
          return 0x2287;
        case 0xBC:            /* 0x81BC */
          return 0x2282;
        case 0xBD:            /* 0x81BD */
          return 0x2283;
        case 0xBE:            /* 0x81BE */
          return 0x222A;
        case 0xBF:            /* 0x81BF */
          return 0x2229;
        case 0xC8:            /* 0x81C8 */
          return 0x2227;
        case 0xC9:            /* 0x81C9 */
          return 0x2228;
        case 0xCA:            /* 0x81CA */
          return 0xFFE2;
        case 0xCB:            /* 0x81CB */
          return 0x21D2;
        case 0xCC:            /* 0x81CC */
          return 0x21D4;
        case 0xCD:            /* 0x81CD */
          return 0x2200;
        case 0xCE:            /* 0x81CE */
          return 0x2203;
        case 0xDA:            /* 0x81DA */
          return 0x2220;
        case 0xDB:            /* 0x81DB */
          return 0x22A5;
        case 0xDC:            /* 0x81DC */
          return 0x2312;
        case 0xDD:            /* 0x81DD */
          return 0x2202;
        case 0xDE:            /* 0x81DE */
          return 0x2207;
        case 0xDF:            /* 0x81DF */
          return 0x2261;
        case 0xE0:            /* 0x81E0 */
          return 0x2252;
        case 0xE1:            /* 0x81E1 */
          return 0x226A;
        case 0xE2:            /* 0x81E2 */
          return 0x226B;
        case 0xE3:            /* 0x81E3 */
          return 0x221A;
        case 0xE4:            /* 0x81E4 */
          return 0x223D;
        case 0xE5:            /* 0x81E5 */
          return 0x221D;
        case 0xE6:            /* 0x81E6 */
          return 0x2235;
        case 0xE7:            /* 0x81E7 */
          return 0x222B;
        case 0xE8:            /* 0x81E8 */
          return 0x222C;
        case 0xF0:            /* 0x81F0 */
          return 0x212B;
        case 0xF1:            /* 0x81F1 */
          return 0x2030;
        case 0xF2:            /* 0x81F2 */
          return 0x266F;
        case 0xF3:            /* 0x81F3 */
          return 0x266D;
        case 0xF4:            /* 0x81F4 */
          return 0x266A;
        case 0xF5:            /* 0x81F5 */
          return 0x2020;
        case 0xF6:            /* 0x81F6 */
          return 0x2021;
        case 0xF7:            /* 0x81F7 */
          return 0x00B6;
        case 0xFC:            /* 0x81FC */
          return 0x25EF;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 81 */

    /* Lead Byte: 0x82xx */
    case 0x82:
      switch (low_byte)
      {
        case 0x4F:            /* 0x824F */
          return 0xFF10;
        case 0x50:            /* 0x8250 */
          return 0xFF11;
        case 0x51:            /* 0x8251 */
          return 0xFF12;
        case 0x52:            /* 0x8252 */
          return 0xFF13;
        case 0x53:            /* 0x8253 */
          return 0xFF14;
        case 0x54:            /* 0x8254 */
          return 0xFF15;
        case 0x55:            /* 0x8255 */
          return 0xFF16;
        case 0x56:            /* 0x8256 */
          return 0xFF17;
        case 0x57:            /* 0x8257 */
          return 0xFF18;
        case 0x58:            /* 0x8258 */
          return 0xFF19;
        case 0x60:            /* 0x8260 */
          return 0xFF21;
        case 0x61:            /* 0x8261 */
          return 0xFF22;
        case 0x62:            /* 0x8262 */
          return 0xFF23;
        case 0x63:            /* 0x8263 */
          return 0xFF24;
        case 0x64:            /* 0x8264 */
          return 0xFF25;
        case 0x65:            /* 0x8265 */
          return 0xFF26;
        case 0x66:            /* 0x8266 */
          return 0xFF27;
        case 0x67:            /* 0x8267 */
          return 0xFF28;
        case 0x68:            /* 0x8268 */
          return 0xFF29;
        case 0x69:            /* 0x8269 */
          return 0xFF2A;
        case 0x6A:            /* 0x826A */
          return 0xFF2B;
        case 0x6B:            /* 0x826B */
          return 0xFF2C;
        case 0x6C:            /* 0x826C */
          return 0xFF2D;
        case 0x6D:            /* 0x826D */
          return 0xFF2E;
        case 0x6E:            /* 0x826E */
          return 0xFF2F;
        case 0x6F:            /* 0x826F */
          return 0xFF30;
        case 0x70:            /* 0x8270 */
          return 0xFF31;
        case 0x71:            /* 0x8271 */
          return 0xFF32;
        case 0x72:            /* 0x8272 */
          return 0xFF33;
        case 0x73:            /* 0x8273 */
          return 0xFF34;
        case 0x74:            /* 0x8274 */
          return 0xFF35;
        case 0x75:            /* 0x8275 */
          return 0xFF36;
        case 0x76:            /* 0x8276 */
          return 0xFF37;
        case 0x77:            /* 0x8277 */
          return 0xFF38;
        case 0x78:            /* 0x8278 */
          return 0xFF39;
        case 0x79:            /* 0x8279 */
          return 0xFF3A;
        case 0x81:            /* 0x8281 */
          return 0xFF41;
        case 0x82:            /* 0x8282 */
          return 0xFF42;
        case 0x83:            /* 0x8283 */
          return 0xFF43;
        case 0x84:            /* 0x8284 */
          return 0xFF44;
        case 0x85:            /* 0x8285 */
          return 0xFF45;
        case 0x86:            /* 0x8286 */
          return 0xFF46;
        case 0x87:            /* 0x8287 */
          return 0xFF47;
        case 0x88:            /* 0x8288 */
          return 0xFF48;
        case 0x89:            /* 0x8289 */
          return 0xFF49;
        case 0x8A:            /* 0x828A */
          return 0xFF4A;
        case 0x8B:            /* 0x828B */
          return 0xFF4B;
        case 0x8C:            /* 0x828C */
          return 0xFF4C;
        case 0x8D:            /* 0x828D */
          return 0xFF4D;
        case 0x8E:            /* 0x828E */
          return 0xFF4E;
        case 0x8F:            /* 0x828F */
          return 0xFF4F;
        case 0x90:            /* 0x8290 */
          return 0xFF50;
        case 0x91:            /* 0x8291 */
          return 0xFF51;
        case 0x92:            /* 0x8292 */
          return 0xFF52;
        case 0x93:            /* 0x8293 */
          return 0xFF53;
        case 0x94:            /* 0x8294 */
          return 0xFF54;
        case 0x95:            /* 0x8295 */
          return 0xFF55;
        case 0x96:            /* 0x8296 */
          return 0xFF56;
        case 0x97:            /* 0x8297 */
          return 0xFF57;
        case 0x98:            /* 0x8298 */
          return 0xFF58;
        case 0x99:            /* 0x8299 */
          return 0xFF59;
        case 0x9A:            /* 0x829A */
          return 0xFF5A;
        case 0x9F:            /* 0x829F */
          return 0x3041;
        case 0xA0:            /* 0x82A0 */
          return 0x3042;
        case 0xA1:            /* 0x82A1 */
          return 0x3043;
        case 0xA2:            /* 0x82A2 */
          return 0x3044;
        case 0xA3:            /* 0x82A3 */
          return 0x3045;
        case 0xA4:            /* 0x82A4 */
          return 0x3046;
        case 0xA5:            /* 0x82A5 */
          return 0x3047;
        case 0xA6:            /* 0x82A6 */
          return 0x3048;
        case 0xA7:            /* 0x82A7 */
          return 0x3049;
        case 0xA8:            /* 0x82A8 */
          return 0x304A;
        case 0xA9:            /* 0x82A9 */
          return 0x304B;
        case 0xAA:            /* 0x82AA */
          return 0x304C;
        case 0xAB:            /* 0x82AB */
          return 0x304D;
        case 0xAC:            /* 0x82AC */
          return 0x304E;
        case 0xAD:            /* 0x82AD */
          return 0x304F;
        case 0xAE:            /* 0x82AE */
          return 0x3050;
        case 0xAF:            /* 0x82AF */
          return 0x3051;
        case 0xB0:            /* 0x82B0 */
          return 0x3052;
        case 0xB1:            /* 0x82B1 */
          return 0x3053;
        case 0xB2:            /* 0x82B2 */
          return 0x3054;
        case 0xB3:            /* 0x82B3 */
          return 0x3055;
        case 0xB4:            /* 0x82B4 */
          return 0x3056;
        case 0xB5:            /* 0x82B5 */
          return 0x3057;
        case 0xB6:            /* 0x82B6 */
          return 0x3058;
        case 0xB7:            /* 0x82B7 */
          return 0x3059;
        case 0xB8:            /* 0x82B8 */
          return 0x305A;
        case 0xB9:            /* 0x82B9 */
          return 0x305B;
        case 0xBA:            /* 0x82BA */
          return 0x305C;
        case 0xBB:            /* 0x82BB */
          return 0x305D;
        case 0xBC:            /* 0x82BC */
          return 0x305E;
        case 0xBD:            /* 0x82BD */
          return 0x305F;
        case 0xBE:            /* 0x82BE */
          return 0x3060;
        case 0xBF:            /* 0x82BF */
          return 0x3061;
        case 0xC0:            /* 0x82C0 */
          return 0x3062;
        case 0xC1:            /* 0x82C1 */
          return 0x3063;
        case 0xC2:            /* 0x82C2 */
          return 0x3064;
        case 0xC3:            /* 0x82C3 */
          return 0x3065;
        case 0xC4:            /* 0x82C4 */
          return 0x3066;
        case 0xC5:            /* 0x82C5 */
          return 0x3067;
        case 0xC6:            /* 0x82C6 */
          return 0x3068;
        case 0xC7:            /* 0x82C7 */
          return 0x3069;
        case 0xC8:            /* 0x82C8 */
          return 0x306A;
        case 0xC9:            /* 0x82C9 */
          return 0x306B;
        case 0xCA:            /* 0x82CA */
          return 0x306C;
        case 0xCB:            /* 0x82CB */
          return 0x306D;
        case 0xCC:            /* 0x82CC */
          return 0x306E;
        case 0xCD:            /* 0x82CD */
          return 0x306F;
        case 0xCE:            /* 0x82CE */
          return 0x3070;
        case 0xCF:            /* 0x82CF */
          return 0x3071;
        case 0xD0:            /* 0x82D0 */
          return 0x3072;
        case 0xD1:            /* 0x82D1 */
          return 0x3073;
        case 0xD2:            /* 0x82D2 */
          return 0x3074;
        case 0xD3:            /* 0x82D3 */
          return 0x3075;
        case 0xD4:            /* 0x82D4 */
          return 0x3076;
        case 0xD5:            /* 0x82D5 */
          return 0x3077;
        case 0xD6:            /* 0x82D6 */
          return 0x3078;
        case 0xD7:            /* 0x82D7 */
          return 0x3079;
        case 0xD8:            /* 0x82D8 */
          return 0x307A;
        case 0xD9:            /* 0x82D9 */
          return 0x307B;
        case 0xDA:            /* 0x82DA */
          return 0x307C;
        case 0xDB:            /* 0x82DB */
          return 0x307D;
        case 0xDC:            /* 0x82DC */
          return 0x307E;
        case 0xDD:            /* 0x82DD */
          return 0x307F;
        case 0xDE:            /* 0x82DE */
          return 0x3080;
        case 0xDF:            /* 0x82DF */
          return 0x3081;
        case 0xE0:            /* 0x82E0 */
          return 0x3082;
        case 0xE1:            /* 0x82E1 */
          return 0x3083;
        case 0xE2:            /* 0x82E2 */
          return 0x3084;
        case 0xE3:            /* 0x82E3 */
          return 0x3085;
        case 0xE4:            /* 0x82E4 */
          return 0x3086;
        case 0xE5:            /* 0x82E5 */
          return 0x3087;
        case 0xE6:            /* 0x82E6 */
          return 0x3088;
        case 0xE7:            /* 0x82E7 */
          return 0x3089;
        case 0xE8:            /* 0x82E8 */
          return 0x308A;
        case 0xE9:            /* 0x82E9 */
          return 0x308B;
        case 0xEA:            /* 0x82EA */
          return 0x308C;
        case 0xEB:            /* 0x82EB */
          return 0x308D;
        case 0xEC:            /* 0x82EC */
          return 0x308E;
        case 0xED:            /* 0x82ED */
          return 0x308F;
        case 0xEE:            /* 0x82EE */
          return 0x3090;
        case 0xEF:            /* 0x82EF */
          return 0x3091;
        case 0xF0:            /* 0x82F0 */
          return 0x3092;
        case 0xF1:            /* 0x82F1 */
          return 0x3093;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 82 */

    /* Lead Byte: 0x83xx */
    case 0x83:
      switch (low_byte)
      {
        case 0x40:            /* 0x8340 */
          return 0x30A1;
        case 0x41:            /* 0x8341 */
          return 0x30A2;
        case 0x42:            /* 0x8342 */
          return 0x30A3;
        case 0x43:            /* 0x8343 */
          return 0x30A4;
        case 0x44:            /* 0x8344 */
          return 0x30A5;
        case 0x45:            /* 0x8345 */
          return 0x30A6;
        case 0x46:            /* 0x8346 */
          return 0x30A7;
        case 0x47:            /* 0x8347 */
          return 0x30A8;
        case 0x48:            /* 0x8348 */
          return 0x30A9;
        case 0x49:            /* 0x8349 */
          return 0x30AA;
        case 0x4A:            /* 0x834A */
          return 0x30AB;
        case 0x4B:            /* 0x834B */
          return 0x30AC;
        case 0x4C:            /* 0x834C */
          return 0x30AD;
        case 0x4D:            /* 0x834D */
          return 0x30AE;
        case 0x4E:            /* 0x834E */
          return 0x30AF;
        case 0x4F:            /* 0x834F */
          return 0x30B0;
        case 0x50:            /* 0x8350 */
          return 0x30B1;
        case 0x51:            /* 0x8351 */
          return 0x30B2;
        case 0x52:            /* 0x8352 */
          return 0x30B3;
        case 0x53:            /* 0x8353 */
          return 0x30B4;
        case 0x54:            /* 0x8354 */
          return 0x30B5;
        case 0x55:            /* 0x8355 */
          return 0x30B6;
        case 0x56:            /* 0x8356 */
          return 0x30B7;
        case 0x57:            /* 0x8357 */
          return 0x30B8;
        case 0x58:            /* 0x8358 */
          return 0x30B9;
        case 0x59:            /* 0x8359 */
          return 0x30BA;
        case 0x5A:            /* 0x835A */
          return 0x30BB;
        case 0x5B:            /* 0x835B */
          return 0x30BC;
        case 0x5C:            /* 0x835C */
          return 0x30BD;
        case 0x5D:            /* 0x835D */
          return 0x30BE;
        case 0x5E:            /* 0x835E */
          return 0x30BF;
        case 0x5F:            /* 0x835F */
          return 0x30C0;
        case 0x60:            /* 0x8360 */
          return 0x30C1;
        case 0x61:            /* 0x8361 */
          return 0x30C2;
        case 0x62:            /* 0x8362 */
          return 0x30C3;
        case 0x63:            /* 0x8363 */
          return 0x30C4;
        case 0x64:            /* 0x8364 */
          return 0x30C5;
        case 0x65:            /* 0x8365 */
          return 0x30C6;
        case 0x66:            /* 0x8366 */
          return 0x30C7;
        case 0x67:            /* 0x8367 */
          return 0x30C8;
        case 0x68:            /* 0x8368 */
          return 0x30C9;
        case 0x69:            /* 0x8369 */
          return 0x30CA;
        case 0x6A:            /* 0x836A */
          return 0x30CB;
        case 0x6B:            /* 0x836B */
          return 0x30CC;
        case 0x6C:            /* 0x836C */
          return 0x30CD;
        case 0x6D:            /* 0x836D */
          return 0x30CE;
        case 0x6E:            /* 0x836E */
          return 0x30CF;
        case 0x6F:            /* 0x836F */
          return 0x30D0;
        case 0x70:            /* 0x8370 */
          return 0x30D1;
        case 0x71:            /* 0x8371 */
          return 0x30D2;
        case 0x72:            /* 0x8372 */
          return 0x30D3;
        case 0x73:            /* 0x8373 */
          return 0x30D4;
        case 0x74:            /* 0x8374 */
          return 0x30D5;
        case 0x75:            /* 0x8375 */
          return 0x30D6;
        case 0x76:            /* 0x8376 */
          return 0x30D7;
        case 0x77:            /* 0x8377 */
          return 0x30D8;
        case 0x78:            /* 0x8378 */
          return 0x30D9;
        case 0x79:            /* 0x8379 */
          return 0x30DA;
        case 0x7A:            /* 0x837A */
          return 0x30DB;
        case 0x7B:            /* 0x837B */
          return 0x30DC;
        case 0x7C:            /* 0x837C */
          return 0x30DD;
        case 0x7D:            /* 0x837D */
          return 0x30DE;
        case 0x7E:            /* 0x837E */
          return 0x30DF;
        case 0x80:            /* 0x8380 */
          return 0x30E0;
        case 0x81:            /* 0x8381 */
          return 0x30E1;
        case 0x82:            /* 0x8382 */
          return 0x30E2;
        case 0x83:            /* 0x8383 */
          return 0x30E3;
        case 0x84:            /* 0x8384 */
          return 0x30E4;
        case 0x85:            /* 0x8385 */
          return 0x30E5;
        case 0x86:            /* 0x8386 */
          return 0x30E6;
        case 0x87:            /* 0x8387 */
          return 0x30E7;
        case 0x88:            /* 0x8388 */
          return 0x30E8;
        case 0x89:            /* 0x8389 */
          return 0x30E9;
        case 0x8A:            /* 0x838A */
          return 0x30EA;
        case 0x8B:            /* 0x838B */
          return 0x30EB;
        case 0x8C:            /* 0x838C */
          return 0x30EC;
        case 0x8D:            /* 0x838D */
          return 0x30ED;
        case 0x8E:            /* 0x838E */
          return 0x30EE;
        case 0x8F:            /* 0x838F */
          return 0x30EF;
        case 0x90:            /* 0x8390 */
          return 0x30F0;
        case 0x91:            /* 0x8391 */
          return 0x30F1;
        case 0x92:            /* 0x8392 */
          return 0x30F2;
        case 0x93:            /* 0x8393 */
          return 0x30F3;
        case 0x94:            /* 0x8394 */
          return 0x30F4;
        case 0x95:            /* 0x8395 */
          return 0x30F5;
        case 0x96:            /* 0x8396 */
          return 0x30F6;
        case 0x9F:            /* 0x839F */
          return 0x0391;
        case 0xA0:            /* 0x83A0 */
          return 0x0392;
        case 0xA1:            /* 0x83A1 */
          return 0x0393;
        case 0xA2:            /* 0x83A2 */
          return 0x0394;
        case 0xA3:            /* 0x83A3 */
          return 0x0395;
        case 0xA4:            /* 0x83A4 */
          return 0x0396;
        case 0xA5:            /* 0x83A5 */
          return 0x0397;
        case 0xA6:            /* 0x83A6 */
          return 0x0398;
        case 0xA7:            /* 0x83A7 */
          return 0x0399;
        case 0xA8:            /* 0x83A8 */
          return 0x039A;
        case 0xA9:            /* 0x83A9 */
          return 0x039B;
        case 0xAA:            /* 0x83AA */
          return 0x039C;
        case 0xAB:            /* 0x83AB */
          return 0x039D;
        case 0xAC:            /* 0x83AC */
          return 0x039E;
        case 0xAD:            /* 0x83AD */
          return 0x039F;
        case 0xAE:            /* 0x83AE */
          return 0x03A0;
        case 0xAF:            /* 0x83AF */
          return 0x03A1;
        case 0xB0:            /* 0x83B0 */
          return 0x03A3;
        case 0xB1:            /* 0x83B1 */
          return 0x03A4;
        case 0xB2:            /* 0x83B2 */
          return 0x03A5;
        case 0xB3:            /* 0x83B3 */
          return 0x03A6;
        case 0xB4:            /* 0x83B4 */
          return 0x03A7;
        case 0xB5:            /* 0x83B5 */
          return 0x03A8;
        case 0xB6:            /* 0x83B6 */
          return 0x03A9;
        case 0xBF:            /* 0x83BF */
          return 0x03B1;
        case 0xC0:            /* 0x83C0 */
          return 0x03B2;
        case 0xC1:            /* 0x83C1 */
          return 0x03B3;
        case 0xC2:            /* 0x83C2 */
          return 0x03B4;
        case 0xC3:            /* 0x83C3 */
          return 0x03B5;
        case 0xC4:            /* 0x83C4 */
          return 0x03B6;
        case 0xC5:            /* 0x83C5 */
          return 0x03B7;
        case 0xC6:            /* 0x83C6 */
          return 0x03B8;
        case 0xC7:            /* 0x83C7 */
          return 0x03B9;
        case 0xC8:            /* 0x83C8 */
          return 0x03BA;
        case 0xC9:            /* 0x83C9 */
          return 0x03BB;
        case 0xCA:            /* 0x83CA */
          return 0x03BC;
        case 0xCB:            /* 0x83CB */
          return 0x03BD;
        case 0xCC:            /* 0x83CC */
          return 0x03BE;
        case 0xCD:            /* 0x83CD */
          return 0x03BF;
        case 0xCE:            /* 0x83CE */
          return 0x03C0;
        case 0xCF:            /* 0x83CF */
          return 0x03C1;
        case 0xD0:            /* 0x83D0 */
          return 0x03C3;
        case 0xD1:            /* 0x83D1 */
          return 0x03C4;
        case 0xD2:            /* 0x83D2 */
          return 0x03C5;
        case 0xD3:            /* 0x83D3 */
          return 0x03C6;
        case 0xD4:            /* 0x83D4 */
          return 0x03C7;
        case 0xD5:            /* 0x83D5 */
          return 0x03C8;
        case 0xD6:            /* 0x83D6 */
          return 0x03C9;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 83 */

    /* Lead Byte: 0x84xx */
    case 0x84:
      switch (low_byte)
      {
        case 0x40:            /* 0x8440 */
          return 0x0410;
        case 0x41:            /* 0x8441 */
          return 0x0411;
        case 0x42:            /* 0x8442 */
          return 0x0412;
        case 0x43:            /* 0x8443 */
          return 0x0413;
        case 0x44:            /* 0x8444 */
          return 0x0414;
        case 0x45:            /* 0x8445 */
          return 0x0415;
        case 0x46:            /* 0x8446 */
          return 0x0401;
        case 0x47:            /* 0x8447 */
          return 0x0416;
        case 0x48:            /* 0x8448 */
          return 0x0417;
        case 0x49:            /* 0x8449 */
          return 0x0418;
        case 0x4A:            /* 0x844A */
          return 0x0419;
        case 0x4B:            /* 0x844B */
          return 0x041A;
        case 0x4C:            /* 0x844C */
          return 0x041B;
        case 0x4D:            /* 0x844D */
          return 0x041C;
        case 0x4E:            /* 0x844E */
          return 0x041D;
        case 0x4F:            /* 0x844F */
          return 0x041E;
        case 0x50:            /* 0x8450 */
          return 0x041F;
        case 0x51:            /* 0x8451 */
          return 0x0420;
        case 0x52:            /* 0x8452 */
          return 0x0421;
        case 0x53:            /* 0x8453 */
          return 0x0422;
        case 0x54:            /* 0x8454 */
          return 0x0423;
        case 0x55:            /* 0x8455 */
          return 0x0424;
        case 0x56:            /* 0x8456 */
          return 0x0425;
        case 0x57:            /* 0x8457 */
          return 0x0426;
        case 0x58:            /* 0x8458 */
          return 0x0427;
        case 0x59:            /* 0x8459 */
          return 0x0428;
        case 0x5A:            /* 0x845A */
          return 0x0429;
        case 0x5B:            /* 0x845B */
          return 0x042A;
        case 0x5C:            /* 0x845C */
          return 0x042B;
        case 0x5D:            /* 0x845D */
          return 0x042C;
        case 0x5E:            /* 0x845E */
          return 0x042D;
        case 0x5F:            /* 0x845F */
          return 0x042E;
        case 0x60:            /* 0x8460 */
          return 0x042F;
        case 0x70:            /* 0x8470 */
          return 0x0430;
        case 0x71:            /* 0x8471 */
          return 0x0431;
        case 0x72:            /* 0x8472 */
          return 0x0432;
        case 0x73:            /* 0x8473 */
          return 0x0433;
        case 0x74:            /* 0x8474 */
          return 0x0434;
        case 0x75:            /* 0x8475 */
          return 0x0435;
        case 0x76:            /* 0x8476 */
          return 0x0451;
        case 0x77:            /* 0x8477 */
          return 0x0436;
        case 0x78:            /* 0x8478 */
          return 0x0437;
        case 0x79:            /* 0x8479 */
          return 0x0438;
        case 0x7A:            /* 0x847A */
          return 0x0439;
        case 0x7B:            /* 0x847B */
          return 0x043A;
        case 0x7C:            /* 0x847C */
          return 0x043B;
        case 0x7D:            /* 0x847D */
          return 0x043C;
        case 0x7E:            /* 0x847E */
          return 0x043D;
        case 0x80:            /* 0x8480 */
          return 0x043E;
        case 0x81:            /* 0x8481 */
          return 0x043F;
        case 0x82:            /* 0x8482 */
          return 0x0440;
        case 0x83:            /* 0x8483 */
          return 0x0441;
        case 0x84:            /* 0x8484 */
          return 0x0442;
        case 0x85:            /* 0x8485 */
          return 0x0443;
        case 0x86:            /* 0x8486 */
          return 0x0444;
        case 0x87:            /* 0x8487 */
          return 0x0445;
        case 0x88:            /* 0x8488 */
          return 0x0446;
        case 0x89:            /* 0x8489 */
          return 0x0447;
        case 0x8A:            /* 0x848A */
          return 0x0448;
        case 0x8B:            /* 0x848B */
          return 0x0449;
        case 0x8C:            /* 0x848C */
          return 0x044A;
        case 0x8D:            /* 0x848D */
          return 0x044B;
        case 0x8E:            /* 0x848E */
          return 0x044C;
        case 0x8F:            /* 0x848F */
          return 0x044D;
        case 0x90:            /* 0x8490 */
          return 0x044E;
        case 0x91:            /* 0x8491 */
          return 0x044F;
        case 0x9F:            /* 0x849F */
          return 0x2500;
        case 0xA0:            /* 0x84A0 */
          return 0x2502;
        case 0xA1:            /* 0x84A1 */
          return 0x250C;
        case 0xA2:            /* 0x84A2 */
          return 0x2510;
        case 0xA3:            /* 0x84A3 */
          return 0x2518;
        case 0xA4:            /* 0x84A4 */
          return 0x2514;
        case 0xA5:            /* 0x84A5 */
          return 0x251C;
        case 0xA6:            /* 0x84A6 */
          return 0x252C;
        case 0xA7:            /* 0x84A7 */
          return 0x2524;
        case 0xA8:            /* 0x84A8 */
          return 0x2534;
        case 0xA9:            /* 0x84A9 */
          return 0x253C;
        case 0xAA:            /* 0x84AA */
          return 0x2501;
        case 0xAB:            /* 0x84AB */
          return 0x2503;
        case 0xAC:            /* 0x84AC */
          return 0x250F;
        case 0xAD:            /* 0x84AD */
          return 0x2513;
        case 0xAE:            /* 0x84AE */
          return 0x251B;
        case 0xAF:            /* 0x84AF */
          return 0x2517;
        case 0xB0:            /* 0x84B0 */
          return 0x2523;
        case 0xB1:            /* 0x84B1 */
          return 0x2533;
        case 0xB2:            /* 0x84B2 */
          return 0x252B;
        case 0xB3:            /* 0x84B3 */
          return 0x253B;
        case 0xB4:            /* 0x84B4 */
          return 0x254B;
        case 0xB5:            /* 0x84B5 */
          return 0x2520;
        case 0xB6:            /* 0x84B6 */
          return 0x252F;
        case 0xB7:            /* 0x84B7 */
          return 0x2528;
        case 0xB8:            /* 0x84B8 */
          return 0x2537;
        case 0xB9:            /* 0x84B9 */
          return 0x253F;
        case 0xBA:            /* 0x84BA */
          return 0x251D;
        case 0xBB:            /* 0x84BB */
          return 0x2530;
        case 0xBC:            /* 0x84BC */
          return 0x2525;
        case 0xBD:            /* 0x84BD */
          return 0x2538;
        case 0xBE:            /* 0x84BE */
          return 0x2542;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 84 */

    /* Lead Byte: 0x87xx */
    case 0x87:
      switch (low_byte)
      {
        case 0x40:            /* 0x8740 */
          return 0x2460;
        case 0x41:            /* 0x8741 */
          return 0x2461;
        case 0x42:            /* 0x8742 */
          return 0x2462;
        case 0x43:            /* 0x8743 */
          return 0x2463;
        case 0x44:            /* 0x8744 */
          return 0x2464;
        case 0x45:            /* 0x8745 */
          return 0x2465;
        case 0x46:            /* 0x8746 */
          return 0x2466;
        case 0x47:            /* 0x8747 */
          return 0x2467;
        case 0x48:            /* 0x8748 */
          return 0x2468;
        case 0x49:            /* 0x8749 */
          return 0x2469;
        case 0x4A:            /* 0x874A */
          return 0x246A;
        case 0x4B:            /* 0x874B */
          return 0x246B;
        case 0x4C:            /* 0x874C */
          return 0x246C;
        case 0x4D:            /* 0x874D */
          return 0x246D;
        case 0x4E:            /* 0x874E */
          return 0x246E;
        case 0x4F:            /* 0x874F */
          return 0x246F;
        case 0x50:            /* 0x8750 */
          return 0x2470;
        case 0x51:            /* 0x8751 */
          return 0x2471;
        case 0x52:            /* 0x8752 */
          return 0x2472;
        case 0x53:            /* 0x8753 */
          return 0x2473;
        case 0x54:            /* 0x8754 */
          return 0x2160;
        case 0x55:            /* 0x8755 */
          return 0x2161;
        case 0x56:            /* 0x8756 */
          return 0x2162;
        case 0x57:            /* 0x8757 */
          return 0x2163;
        case 0x58:            /* 0x8758 */
          return 0x2164;
        case 0x59:            /* 0x8759 */
          return 0x2165;
        case 0x5A:            /* 0x875A */
          return 0x2166;
        case 0x5B:            /* 0x875B */
          return 0x2167;
        case 0x5C:            /* 0x875C */
          return 0x2168;
        case 0x5D:            /* 0x875D */
          return 0x2169;
        case 0x5F:            /* 0x875F */
          return 0x3349;
        case 0x60:            /* 0x8760 */
          return 0x3314;
        case 0x61:            /* 0x8761 */
          return 0x3322;
        case 0x62:            /* 0x8762 */
          return 0x334D;
        case 0x63:            /* 0x8763 */
          return 0x3318;
        case 0x64:            /* 0x8764 */
          return 0x3327;
        case 0x65:            /* 0x8765 */
          return 0x3303;
        case 0x66:            /* 0x8766 */
          return 0x3336;
        case 0x67:            /* 0x8767 */
          return 0x3351;
        case 0x68:            /* 0x8768 */
          return 0x3357;
        case 0x69:            /* 0x8769 */
          return 0x330D;
        case 0x6A:            /* 0x876A */
          return 0x3326;
        case 0x6B:            /* 0x876B */
          return 0x3323;
        case 0x6C:            /* 0x876C */
          return 0x332B;
        case 0x6D:            /* 0x876D */
          return 0x334A;
        case 0x6E:            /* 0x876E */
          return 0x333B;
        case 0x6F:            /* 0x876F */
          return 0x339C;
        case 0x70:            /* 0x8770 */
          return 0x339D;
        case 0x71:            /* 0x8771 */
          return 0x339E;
        case 0x72:            /* 0x8772 */
          return 0x338E;
        case 0x73:            /* 0x8773 */
          return 0x338F;
        case 0x74:            /* 0x8774 */
          return 0x33C4;
        case 0x75:            /* 0x8775 */
          return 0x33A1;
        case 0x7E:            /* 0x877E */
          return 0x337B;
        case 0x80:            /* 0x8780 */
          return 0x301D;
        case 0x81:            /* 0x8781 */
          return 0x301F;
        case 0x82:            /* 0x8782 */
          return 0x2116;
        case 0x83:            /* 0x8783 */
          return 0x33CD;
        case 0x84:            /* 0x8784 */
          return 0x2121;
        case 0x85:            /* 0x8785 */
          return 0x32A4;
        case 0x86:            /* 0x8786 */
          return 0x32A5;
        case 0x87:            /* 0x8787 */
          return 0x32A6;
        case 0x88:            /* 0x8788 */
          return 0x32A7;
        case 0x89:            /* 0x8789 */
          return 0x32A8;
        case 0x8A:            /* 0x878A */
          return 0x3231;
        case 0x8B:            /* 0x878B */
          return 0x3232;
        case 0x8C:            /* 0x878C */
          return 0x3239;
        case 0x8D:            /* 0x878D */
          return 0x337E;
        case 0x8E:            /* 0x878E */
          return 0x337D;
        case 0x8F:            /* 0x878F */
          return 0x337C;
        case 0x90:            /* 0x8790 */
          return 0x2252;
        case 0x91:            /* 0x8791 */
          return 0x2261;
        case 0x92:            /* 0x8792 */
          return 0x222B;
        case 0x93:            /* 0x8793 */
          return 0x222E;
        case 0x94:            /* 0x8794 */
          return 0x2211;
        case 0x95:            /* 0x8795 */
          return 0x221A;
        case 0x96:            /* 0x8796 */
          return 0x22A5;
        case 0x97:            /* 0x8797 */
          return 0x2220;
        case 0x98:            /* 0x8798 */
          return 0x221F;
        case 0x99:            /* 0x8799 */
          return 0x22BF;
        case 0x9A:            /* 0x879A */
          return 0x2235;
        case 0x9B:            /* 0x879B */
          return 0x2229;
        case 0x9C:            /* 0x879C */
          return 0x222A;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 87 */

    /* Lead Byte: 0x88xx */
    case 0x88:
      switch (low_byte)
      {
        case 0x9F:            /* 0x889F */
          return 0x4E9C;
        case 0xA0:            /* 0x88A0 */
          return 0x5516;
        case 0xA1:            /* 0x88A1 */
          return 0x5A03;
        case 0xA2:            /* 0x88A2 */
          return 0x963F;
        case 0xA3:            /* 0x88A3 */
          return 0x54C0;
        case 0xA4:            /* 0x88A4 */
          return 0x611B;
        case 0xA5:            /* 0x88A5 */
          return 0x6328;
        case 0xA6:            /* 0x88A6 */
          return 0x59F6;
        case 0xA7:            /* 0x88A7 */
          return 0x9022;
        case 0xA8:            /* 0x88A8 */
          return 0x8475;
        case 0xA9:            /* 0x88A9 */
          return 0x831C;
        case 0xAA:            /* 0x88AA */
          return 0x7A50;
        case 0xAB:            /* 0x88AB */
          return 0x60AA;
        case 0xAC:            /* 0x88AC */
          return 0x63E1;
        case 0xAD:            /* 0x88AD */
          return 0x6E25;
        case 0xAE:            /* 0x88AE */
          return 0x65ED;
        case 0xAF:            /* 0x88AF */
          return 0x8466;
        case 0xB0:            /* 0x88B0 */
          return 0x82A6;
        case 0xB1:            /* 0x88B1 */
          return 0x9BF5;
        case 0xB2:            /* 0x88B2 */
          return 0x6893;
        case 0xB3:            /* 0x88B3 */
          return 0x5727;
        case 0xB4:            /* 0x88B4 */
          return 0x65A1;
        case 0xB5:            /* 0x88B5 */
          return 0x6271;
        case 0xB6:            /* 0x88B6 */
          return 0x5B9B;
        case 0xB7:            /* 0x88B7 */
          return 0x59D0;
        case 0xB8:            /* 0x88B8 */
          return 0x867B;
        case 0xB9:            /* 0x88B9 */
          return 0x98F4;
        case 0xBA:            /* 0x88BA */
          return 0x7D62;
        case 0xBB:            /* 0x88BB */
          return 0x7DBE;
        case 0xBC:            /* 0x88BC */
          return 0x9B8E;
        case 0xBD:            /* 0x88BD */
          return 0x6216;
        case 0xBE:            /* 0x88BE */
          return 0x7C9F;
        case 0xBF:            /* 0x88BF */
          return 0x88B7;
        case 0xC0:            /* 0x88C0 */
          return 0x5B89;
        case 0xC1:            /* 0x88C1 */
          return 0x5EB5;
        case 0xC2:            /* 0x88C2 */
          return 0x6309;
        case 0xC3:            /* 0x88C3 */
          return 0x6697;
        case 0xC4:            /* 0x88C4 */
          return 0x6848;
        case 0xC5:            /* 0x88C5 */
          return 0x95C7;
        case 0xC6:            /* 0x88C6 */
          return 0x978D;
        case 0xC7:            /* 0x88C7 */
          return 0x674F;
        case 0xC8:            /* 0x88C8 */
          return 0x4EE5;
        case 0xC9:            /* 0x88C9 */
          return 0x4F0A;
        case 0xCA:            /* 0x88CA */
          return 0x4F4D;
        case 0xCB:            /* 0x88CB */
          return 0x4F9D;
        case 0xCC:            /* 0x88CC */
          return 0x5049;
        case 0xCD:            /* 0x88CD */
          return 0x56F2;
        case 0xCE:            /* 0x88CE */
          return 0x5937;
        case 0xCF:            /* 0x88CF */
          return 0x59D4;
        case 0xD0:            /* 0x88D0 */
          return 0x5A01;
        case 0xD1:            /* 0x88D1 */
          return 0x5C09;
        case 0xD2:            /* 0x88D2 */
          return 0x60DF;
        case 0xD3:            /* 0x88D3 */
          return 0x610F;
        case 0xD4:            /* 0x88D4 */
          return 0x6170;
        case 0xD5:            /* 0x88D5 */
          return 0x6613;
        case 0xD6:            /* 0x88D6 */
          return 0x6905;
        case 0xD7:            /* 0x88D7 */
          return 0x70BA;
        case 0xD8:            /* 0x88D8 */
          return 0x754F;
        case 0xD9:            /* 0x88D9 */
          return 0x7570;
        case 0xDA:            /* 0x88DA */
          return 0x79FB;
        case 0xDB:            /* 0x88DB */
          return 0x7DAD;
        case 0xDC:            /* 0x88DC */
          return 0x7DEF;
        case 0xDD:            /* 0x88DD */
          return 0x80C3;
        case 0xDE:            /* 0x88DE */
          return 0x840E;
        case 0xDF:            /* 0x88DF */
          return 0x8863;
        case 0xE0:            /* 0x88E0 */
          return 0x8B02;
        case 0xE1:            /* 0x88E1 */
          return 0x9055;
        case 0xE2:            /* 0x88E2 */
          return 0x907A;
        case 0xE3:            /* 0x88E3 */
          return 0x533B;
        case 0xE4:            /* 0x88E4 */
          return 0x4E95;
        case 0xE5:            /* 0x88E5 */
          return 0x4EA5;
        case 0xE6:            /* 0x88E6 */
          return 0x57DF;
        case 0xE7:            /* 0x88E7 */
          return 0x80B2;
        case 0xE8:            /* 0x88E8 */
          return 0x90C1;
        case 0xE9:            /* 0x88E9 */
          return 0x78EF;
        case 0xEA:            /* 0x88EA */
          return 0x4E00;
        case 0xEB:            /* 0x88EB */
          return 0x58F1;
        case 0xEC:            /* 0x88EC */
          return 0x6EA2;
        case 0xED:            /* 0x88ED */
          return 0x9038;
        case 0xEE:            /* 0x88EE */
          return 0x7A32;
        case 0xEF:            /* 0x88EF */
          return 0x8328;
        case 0xF0:            /* 0x88F0 */
          return 0x828B;
        case 0xF1:            /* 0x88F1 */
          return 0x9C2F;
        case 0xF2:            /* 0x88F2 */
          return 0x5141;
        case 0xF3:            /* 0x88F3 */
          return 0x5370;
        case 0xF4:            /* 0x88F4 */
          return 0x54BD;
        case 0xF5:            /* 0x88F5 */
          return 0x54E1;
        case 0xF6:            /* 0x88F6 */
          return 0x56E0;
        case 0xF7:            /* 0x88F7 */
          return 0x59FB;
        case 0xF8:            /* 0x88F8 */
          return 0x5F15;
        case 0xF9:            /* 0x88F9 */
          return 0x98F2;
        case 0xFA:            /* 0x88FA */
          return 0x6DEB;
        case 0xFB:            /* 0x88FB */
          return 0x80E4;
        case 0xFC:            /* 0x88FC */
          return 0x852D;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 88 */

    /* Lead Byte: 0x89xx */
    case 0x89:
      switch (low_byte)
      {
        case 0x40:            /* 0x8940 */
          return 0x9662;
        case 0x41:            /* 0x8941 */
          return 0x9670;
        case 0x42:            /* 0x8942 */
          return 0x96A0;
        case 0x43:            /* 0x8943 */
          return 0x97FB;
        case 0x44:            /* 0x8944 */
          return 0x540B;
        case 0x45:            /* 0x8945 */
          return 0x53F3;
        case 0x46:            /* 0x8946 */
          return 0x5B87;
        case 0x47:            /* 0x8947 */
          return 0x70CF;
        case 0x48:            /* 0x8948 */
          return 0x7FBD;
        case 0x49:            /* 0x8949 */
          return 0x8FC2;
        case 0x4A:            /* 0x894A */
          return 0x96E8;
        case 0x4B:            /* 0x894B */
          return 0x536F;
        case 0x4C:            /* 0x894C */
          return 0x9D5C;
        case 0x4D:            /* 0x894D */
          return 0x7ABA;
        case 0x4E:            /* 0x894E */
          return 0x4E11;
        case 0x4F:            /* 0x894F */
          return 0x7893;
        case 0x50:            /* 0x8950 */
          return 0x81FC;
        case 0x51:            /* 0x8951 */
          return 0x6E26;
        case 0x52:            /* 0x8952 */
          return 0x5618;
        case 0x53:            /* 0x8953 */
          return 0x5504;
        case 0x54:            /* 0x8954 */
          return 0x6B1D;
        case 0x55:            /* 0x8955 */
          return 0x851A;
        case 0x56:            /* 0x8956 */
          return 0x9C3B;
        case 0x57:            /* 0x8957 */
          return 0x59E5;
        case 0x58:            /* 0x8958 */
          return 0x53A9;
        case 0x59:            /* 0x8959 */
          return 0x6D66;
        case 0x5A:            /* 0x895A */
          return 0x74DC;
        case 0x5B:            /* 0x895B */
          return 0x958F;
        case 0x5C:            /* 0x895C */
          return 0x5642;
        case 0x5D:            /* 0x895D */
          return 0x4E91;
        case 0x5E:            /* 0x895E */
          return 0x904B;
        case 0x5F:            /* 0x895F */
          return 0x96F2;
        case 0x60:            /* 0x8960 */
          return 0x834F;
        case 0x61:            /* 0x8961 */
          return 0x990C;
        case 0x62:            /* 0x8962 */
          return 0x53E1;
        case 0x63:            /* 0x8963 */
          return 0x55B6;
        case 0x64:            /* 0x8964 */
          return 0x5B30;
        case 0x65:            /* 0x8965 */
          return 0x5F71;
        case 0x66:            /* 0x8966 */
          return 0x6620;
        case 0x67:            /* 0x8967 */
          return 0x66F3;
        case 0x68:            /* 0x8968 */
          return 0x6804;
        case 0x69:            /* 0x8969 */
          return 0x6C38;
        case 0x6A:            /* 0x896A */
          return 0x6CF3;
        case 0x6B:            /* 0x896B */
          return 0x6D29;
        case 0x6C:            /* 0x896C */
          return 0x745B;
        case 0x6D:            /* 0x896D */
          return 0x76C8;
        case 0x6E:            /* 0x896E */
          return 0x7A4E;
        case 0x6F:            /* 0x896F */
          return 0x9834;
        case 0x70:            /* 0x8970 */
          return 0x82F1;
        case 0x71:            /* 0x8971 */
          return 0x885B;
        case 0x72:            /* 0x8972 */
          return 0x8A60;
        case 0x73:            /* 0x8973 */
          return 0x92ED;
        case 0x74:            /* 0x8974 */
          return 0x6DB2;
        case 0x75:            /* 0x8975 */
          return 0x75AB;
        case 0x76:            /* 0x8976 */
          return 0x76CA;
        case 0x77:            /* 0x8977 */
          return 0x99C5;
        case 0x78:            /* 0x8978 */
          return 0x60A6;
        case 0x79:            /* 0x8979 */
          return 0x8B01;
        case 0x7A:            /* 0x897A */
          return 0x8D8A;
        case 0x7B:            /* 0x897B */
          return 0x95B2;
        case 0x7C:            /* 0x897C */
          return 0x698E;
        case 0x7D:            /* 0x897D */
          return 0x53AD;
        case 0x7E:            /* 0x897E */
          return 0x5186;
        case 0x80:            /* 0x8980 */
          return 0x5712;
        case 0x81:            /* 0x8981 */
          return 0x5830;
        case 0x82:            /* 0x8982 */
          return 0x5944;
        case 0x83:            /* 0x8983 */
          return 0x5BB4;
        case 0x84:            /* 0x8984 */
          return 0x5EF6;
        case 0x85:            /* 0x8985 */
          return 0x6028;
        case 0x86:            /* 0x8986 */
          return 0x63A9;
        case 0x87:            /* 0x8987 */
          return 0x63F4;
        case 0x88:            /* 0x8988 */
          return 0x6CBF;
        case 0x89:            /* 0x8989 */
          return 0x6F14;
        case 0x8A:            /* 0x898A */
          return 0x708E;
        case 0x8B:            /* 0x898B */
          return 0x7114;
        case 0x8C:            /* 0x898C */
          return 0x7159;
        case 0x8D:            /* 0x898D */
          return 0x71D5;
        case 0x8E:            /* 0x898E */
          return 0x733F;
        case 0x8F:            /* 0x898F */
          return 0x7E01;
        case 0x90:            /* 0x8990 */
          return 0x8276;
        case 0x91:            /* 0x8991 */
          return 0x82D1;
        case 0x92:            /* 0x8992 */
          return 0x8597;
        case 0x93:            /* 0x8993 */
          return 0x9060;
        case 0x94:            /* 0x8994 */
          return 0x925B;
        case 0x95:            /* 0x8995 */
          return 0x9D1B;
        case 0x96:            /* 0x8996 */
          return 0x5869;
        case 0x97:            /* 0x8997 */
          return 0x65BC;
        case 0x98:            /* 0x8998 */
          return 0x6C5A;
        case 0x99:            /* 0x8999 */
          return 0x7525;
        case 0x9A:            /* 0x899A */
          return 0x51F9;
        case 0x9B:            /* 0x899B */
          return 0x592E;
        case 0x9C:            /* 0x899C */
          return 0x5965;
        case 0x9D:            /* 0x899D */
          return 0x5F80;
        case 0x9E:            /* 0x899E */
          return 0x5FDC;
        case 0x9F:            /* 0x899F */
          return 0x62BC;
        case 0xA0:            /* 0x89A0 */
          return 0x65FA;
        case 0xA1:            /* 0x89A1 */
          return 0x6A2A;
        case 0xA2:            /* 0x89A2 */
          return 0x6B27;
        case 0xA3:            /* 0x89A3 */
          return 0x6BB4;
        case 0xA4:            /* 0x89A4 */
          return 0x738B;
        case 0xA5:            /* 0x89A5 */
          return 0x7FC1;
        case 0xA6:            /* 0x89A6 */
          return 0x8956;
        case 0xA7:            /* 0x89A7 */
          return 0x9D2C;
        case 0xA8:            /* 0x89A8 */
          return 0x9D0E;
        case 0xA9:            /* 0x89A9 */
          return 0x9EC4;
        case 0xAA:            /* 0x89AA */
          return 0x5CA1;
        case 0xAB:            /* 0x89AB */
          return 0x6C96;
        case 0xAC:            /* 0x89AC */
          return 0x837B;
        case 0xAD:            /* 0x89AD */
          return 0x5104;
        case 0xAE:            /* 0x89AE */
          return 0x5C4B;
        case 0xAF:            /* 0x89AF */
          return 0x61B6;
        case 0xB0:            /* 0x89B0 */
          return 0x81C6;
        case 0xB1:            /* 0x89B1 */
          return 0x6876;
        case 0xB2:            /* 0x89B2 */
          return 0x7261;
        case 0xB3:            /* 0x89B3 */
          return 0x4E59;
        case 0xB4:            /* 0x89B4 */
          return 0x4FFA;
        case 0xB5:            /* 0x89B5 */
          return 0x5378;
        case 0xB6:            /* 0x89B6 */
          return 0x6069;
        case 0xB7:            /* 0x89B7 */
          return 0x6E29;
        case 0xB8:            /* 0x89B8 */
          return 0x7A4F;
        case 0xB9:            /* 0x89B9 */
          return 0x97F3;
        case 0xBA:            /* 0x89BA */
          return 0x4E0B;
        case 0xBB:            /* 0x89BB */
          return 0x5316;
        case 0xBC:            /* 0x89BC */
          return 0x4EEE;
        case 0xBD:            /* 0x89BD */
          return 0x4F55;
        case 0xBE:            /* 0x89BE */
          return 0x4F3D;
        case 0xBF:            /* 0x89BF */
          return 0x4FA1;
        case 0xC0:            /* 0x89C0 */
          return 0x4F73;
        case 0xC1:            /* 0x89C1 */
          return 0x52A0;
        case 0xC2:            /* 0x89C2 */
          return 0x53EF;
        case 0xC3:            /* 0x89C3 */
          return 0x5609;
        case 0xC4:            /* 0x89C4 */
          return 0x590F;
        case 0xC5:            /* 0x89C5 */
          return 0x5AC1;
        case 0xC6:            /* 0x89C6 */
          return 0x5BB6;
        case 0xC7:            /* 0x89C7 */
          return 0x5BE1;
        case 0xC8:            /* 0x89C8 */
          return 0x79D1;
        case 0xC9:            /* 0x89C9 */
          return 0x6687;
        case 0xCA:            /* 0x89CA */
          return 0x679C;
        case 0xCB:            /* 0x89CB */
          return 0x67B6;
        case 0xCC:            /* 0x89CC */
          return 0x6B4C;
        case 0xCD:            /* 0x89CD */
          return 0x6CB3;
        case 0xCE:            /* 0x89CE */
          return 0x706B;
        case 0xCF:            /* 0x89CF */
          return 0x73C2;
        case 0xD0:            /* 0x89D0 */
          return 0x798D;
        case 0xD1:            /* 0x89D1 */
          return 0x79BE;
        case 0xD2:            /* 0x89D2 */
          return 0x7A3C;
        case 0xD3:            /* 0x89D3 */
          return 0x7B87;
        case 0xD4:            /* 0x89D4 */
          return 0x82B1;
        case 0xD5:            /* 0x89D5 */
          return 0x82DB;
        case 0xD6:            /* 0x89D6 */
          return 0x8304;
        case 0xD7:            /* 0x89D7 */
          return 0x8377;
        case 0xD8:            /* 0x89D8 */
          return 0x83EF;
        case 0xD9:            /* 0x89D9 */
          return 0x83D3;
        case 0xDA:            /* 0x89DA */
          return 0x8766;
        case 0xDB:            /* 0x89DB */
          return 0x8AB2;
        case 0xDC:            /* 0x89DC */
          return 0x5629;
        case 0xDD:            /* 0x89DD */
          return 0x8CA8;
        case 0xDE:            /* 0x89DE */
          return 0x8FE6;
        case 0xDF:            /* 0x89DF */
          return 0x904E;
        case 0xE0:            /* 0x89E0 */
          return 0x971E;
        case 0xE1:            /* 0x89E1 */
          return 0x868A;
        case 0xE2:            /* 0x89E2 */
          return 0x4FC4;
        case 0xE3:            /* 0x89E3 */
          return 0x5CE8;
        case 0xE4:            /* 0x89E4 */
          return 0x6211;
        case 0xE5:            /* 0x89E5 */
          return 0x7259;
        case 0xE6:            /* 0x89E6 */
          return 0x753B;
        case 0xE7:            /* 0x89E7 */
          return 0x81E5;
        case 0xE8:            /* 0x89E8 */
          return 0x82BD;
        case 0xE9:            /* 0x89E9 */
          return 0x86FE;
        case 0xEA:            /* 0x89EA */
          return 0x8CC0;
        case 0xEB:            /* 0x89EB */
          return 0x96C5;
        case 0xEC:            /* 0x89EC */
          return 0x9913;
        case 0xED:            /* 0x89ED */
          return 0x99D5;
        case 0xEE:            /* 0x89EE */
          return 0x4ECB;
        case 0xEF:            /* 0x89EF */
          return 0x4F1A;
        case 0xF0:            /* 0x89F0 */
          return 0x89E3;
        case 0xF1:            /* 0x89F1 */
          return 0x56DE;
        case 0xF2:            /* 0x89F2 */
          return 0x584A;
        case 0xF3:            /* 0x89F3 */
          return 0x58CA;
        case 0xF4:            /* 0x89F4 */
          return 0x5EFB;
        case 0xF5:            /* 0x89F5 */
          return 0x5FEB;
        case 0xF6:            /* 0x89F6 */
          return 0x602A;
        case 0xF7:            /* 0x89F7 */
          return 0x6094;
        case 0xF8:            /* 0x89F8 */
          return 0x6062;
        case 0xF9:            /* 0x89F9 */
          return 0x61D0;
        case 0xFA:            /* 0x89FA */
          return 0x6212;
        case 0xFB:            /* 0x89FB */
          return 0x62D0;
        case 0xFC:            /* 0x89FC */
          return 0x6539;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 89 */

    /* Lead Byte: 0x8Axx */
    case 0x8A:
      switch (low_byte)
      {
        case 0x40:            /* 0x8A40 */
          return 0x9B41;
        case 0x41:            /* 0x8A41 */
          return 0x6666;
        case 0x42:            /* 0x8A42 */
          return 0x68B0;
        case 0x43:            /* 0x8A43 */
          return 0x6D77;
        case 0x44:            /* 0x8A44 */
          return 0x7070;
        case 0x45:            /* 0x8A45 */
          return 0x754C;
        case 0x46:            /* 0x8A46 */
          return 0x7686;
        case 0x47:            /* 0x8A47 */
          return 0x7D75;
        case 0x48:            /* 0x8A48 */
          return 0x82A5;
        case 0x49:            /* 0x8A49 */
          return 0x87F9;
        case 0x4A:            /* 0x8A4A */
          return 0x958B;
        case 0x4B:            /* 0x8A4B */
          return 0x968E;
        case 0x4C:            /* 0x8A4C */
          return 0x8C9D;
        case 0x4D:            /* 0x8A4D */
          return 0x51F1;
        case 0x4E:            /* 0x8A4E */
          return 0x52BE;
        case 0x4F:            /* 0x8A4F */
          return 0x5916;
        case 0x50:            /* 0x8A50 */
          return 0x54B3;
        case 0x51:            /* 0x8A51 */
          return 0x5BB3;
        case 0x52:            /* 0x8A52 */
          return 0x5D16;
        case 0x53:            /* 0x8A53 */
          return 0x6168;
        case 0x54:            /* 0x8A54 */
          return 0x6982;
        case 0x55:            /* 0x8A55 */
          return 0x6DAF;
        case 0x56:            /* 0x8A56 */
          return 0x788D;
        case 0x57:            /* 0x8A57 */
          return 0x84CB;
        case 0x58:            /* 0x8A58 */
          return 0x8857;
        case 0x59:            /* 0x8A59 */
          return 0x8A72;
        case 0x5A:            /* 0x8A5A */
          return 0x93A7;
        case 0x5B:            /* 0x8A5B */
          return 0x9AB8;
        case 0x5C:            /* 0x8A5C */
          return 0x6D6C;
        case 0x5D:            /* 0x8A5D */
          return 0x99A8;
        case 0x5E:            /* 0x8A5E */
          return 0x86D9;
        case 0x5F:            /* 0x8A5F */
          return 0x57A3;
        case 0x60:            /* 0x8A60 */
          return 0x67FF;
        case 0x61:            /* 0x8A61 */
          return 0x86CE;
        case 0x62:            /* 0x8A62 */
          return 0x920E;
        case 0x63:            /* 0x8A63 */
          return 0x5283;
        case 0x64:            /* 0x8A64 */
          return 0x5687;
        case 0x65:            /* 0x8A65 */
          return 0x5404;
        case 0x66:            /* 0x8A66 */
          return 0x5ED3;
        case 0x67:            /* 0x8A67 */
          return 0x62E1;
        case 0x68:            /* 0x8A68 */
          return 0x64B9;
        case 0x69:            /* 0x8A69 */
          return 0x683C;
        case 0x6A:            /* 0x8A6A */
          return 0x6838;
        case 0x6B:            /* 0x8A6B */
          return 0x6BBB;
        case 0x6C:            /* 0x8A6C */
          return 0x7372;
        case 0x6D:            /* 0x8A6D */
          return 0x78BA;
        case 0x6E:            /* 0x8A6E */
          return 0x7A6B;
        case 0x6F:            /* 0x8A6F */
          return 0x899A;
        case 0x70:            /* 0x8A70 */
          return 0x89D2;
        case 0x71:            /* 0x8A71 */
          return 0x8D6B;
        case 0x72:            /* 0x8A72 */
          return 0x8F03;
        case 0x73:            /* 0x8A73 */
          return 0x90ED;
        case 0x74:            /* 0x8A74 */
          return 0x95A3;
        case 0x75:            /* 0x8A75 */
          return 0x9694;
        case 0x76:            /* 0x8A76 */
          return 0x9769;
        case 0x77:            /* 0x8A77 */
          return 0x5B66;
        case 0x78:            /* 0x8A78 */
          return 0x5CB3;
        case 0x79:            /* 0x8A79 */
          return 0x697D;
        case 0x7A:            /* 0x8A7A */
          return 0x984D;
        case 0x7B:            /* 0x8A7B */
          return 0x984E;
        case 0x7C:            /* 0x8A7C */
          return 0x639B;
        case 0x7D:            /* 0x8A7D */
          return 0x7B20;
        case 0x7E:            /* 0x8A7E */
          return 0x6A2B;
        case 0x80:            /* 0x8A80 */
          return 0x6A7F;
        case 0x81:            /* 0x8A81 */
          return 0x68B6;
        case 0x82:            /* 0x8A82 */
          return 0x9C0D;
        case 0x83:            /* 0x8A83 */
          return 0x6F5F;
        case 0x84:            /* 0x8A84 */
          return 0x5272;
        case 0x85:            /* 0x8A85 */
          return 0x559D;
        case 0x86:            /* 0x8A86 */
          return 0x6070;
        case 0x87:            /* 0x8A87 */
          return 0x62EC;
        case 0x88:            /* 0x8A88 */
          return 0x6D3B;
        case 0x89:            /* 0x8A89 */
          return 0x6E07;
        case 0x8A:            /* 0x8A8A */
          return 0x6ED1;
        case 0x8B:            /* 0x8A8B */
          return 0x845B;
        case 0x8C:            /* 0x8A8C */
          return 0x8910;
        case 0x8D:            /* 0x8A8D */
          return 0x8F44;
        case 0x8E:            /* 0x8A8E */
          return 0x4E14;
        case 0x8F:            /* 0x8A8F */
          return 0x9C39;
        case 0x90:            /* 0x8A90 */
          return 0x53F6;
        case 0x91:            /* 0x8A91 */
          return 0x691B;
        case 0x92:            /* 0x8A92 */
          return 0x6A3A;
        case 0x93:            /* 0x8A93 */
          return 0x9784;
        case 0x94:            /* 0x8A94 */
          return 0x682A;
        case 0x95:            /* 0x8A95 */
          return 0x515C;
        case 0x96:            /* 0x8A96 */
          return 0x7AC3;
        case 0x97:            /* 0x8A97 */
          return 0x84B2;
        case 0x98:            /* 0x8A98 */
          return 0x91DC;
        case 0x99:            /* 0x8A99 */
          return 0x938C;
        case 0x9A:            /* 0x8A9A */
          return 0x565B;
        case 0x9B:            /* 0x8A9B */
          return 0x9D28;
        case 0x9C:            /* 0x8A9C */
          return 0x6822;
        case 0x9D:            /* 0x8A9D */
          return 0x8305;
        case 0x9E:            /* 0x8A9E */
          return 0x8431;
        case 0x9F:            /* 0x8A9F */
          return 0x7CA5;
        case 0xA0:            /* 0x8AA0 */
          return 0x5208;
        case 0xA1:            /* 0x8AA1 */
          return 0x82C5;
        case 0xA2:            /* 0x8AA2 */
          return 0x74E6;
        case 0xA3:            /* 0x8AA3 */
          return 0x4E7E;
        case 0xA4:            /* 0x8AA4 */
          return 0x4F83;
        case 0xA5:            /* 0x8AA5 */
          return 0x51A0;
        case 0xA6:            /* 0x8AA6 */
          return 0x5BD2;
        case 0xA7:            /* 0x8AA7 */
          return 0x520A;
        case 0xA8:            /* 0x8AA8 */
          return 0x52D8;
        case 0xA9:            /* 0x8AA9 */
          return 0x52E7;
        case 0xAA:            /* 0x8AAA */
          return 0x5DFB;
        case 0xAB:            /* 0x8AAB */
          return 0x559A;
        case 0xAC:            /* 0x8AAC */
          return 0x582A;
        case 0xAD:            /* 0x8AAD */
          return 0x59E6;
        case 0xAE:            /* 0x8AAE */
          return 0x5B8C;
        case 0xAF:            /* 0x8AAF */
          return 0x5B98;
        case 0xB0:            /* 0x8AB0 */
          return 0x5BDB;
        case 0xB1:            /* 0x8AB1 */
          return 0x5E72;
        case 0xB2:            /* 0x8AB2 */
          return 0x5E79;
        case 0xB3:            /* 0x8AB3 */
          return 0x60A3;
        case 0xB4:            /* 0x8AB4 */
          return 0x611F;
        case 0xB5:            /* 0x8AB5 */
          return 0x6163;
        case 0xB6:            /* 0x8AB6 */
          return 0x61BE;
        case 0xB7:            /* 0x8AB7 */
          return 0x63DB;
        case 0xB8:            /* 0x8AB8 */
          return 0x6562;
        case 0xB9:            /* 0x8AB9 */
          return 0x67D1;
        case 0xBA:            /* 0x8ABA */
          return 0x6853;
        case 0xBB:            /* 0x8ABB */
          return 0x68FA;
        case 0xBC:            /* 0x8ABC */
          return 0x6B3E;
        case 0xBD:            /* 0x8ABD */
          return 0x6B53;
        case 0xBE:            /* 0x8ABE */
          return 0x6C57;
        case 0xBF:            /* 0x8ABF */
          return 0x6F22;
        case 0xC0:            /* 0x8AC0 */
          return 0x6F97;
        case 0xC1:            /* 0x8AC1 */
          return 0x6F45;
        case 0xC2:            /* 0x8AC2 */
          return 0x74B0;
        case 0xC3:            /* 0x8AC3 */
          return 0x7518;
        case 0xC4:            /* 0x8AC4 */
          return 0x76E3;
        case 0xC5:            /* 0x8AC5 */
          return 0x770B;
        case 0xC6:            /* 0x8AC6 */
          return 0x7AFF;
        case 0xC7:            /* 0x8AC7 */
          return 0x7BA1;
        case 0xC8:            /* 0x8AC8 */
          return 0x7C21;
        case 0xC9:            /* 0x8AC9 */
          return 0x7DE9;
        case 0xCA:            /* 0x8ACA */
          return 0x7F36;
        case 0xCB:            /* 0x8ACB */
          return 0x7FF0;
        case 0xCC:            /* 0x8ACC */
          return 0x809D;
        case 0xCD:            /* 0x8ACD */
          return 0x8266;
        case 0xCE:            /* 0x8ACE */
          return 0x839E;
        case 0xCF:            /* 0x8ACF */
          return 0x89B3;
        case 0xD0:            /* 0x8AD0 */
          return 0x8ACC;
        case 0xD1:            /* 0x8AD1 */
          return 0x8CAB;
        case 0xD2:            /* 0x8AD2 */
          return 0x9084;
        case 0xD3:            /* 0x8AD3 */
          return 0x9451;
        case 0xD4:            /* 0x8AD4 */
          return 0x9593;
        case 0xD5:            /* 0x8AD5 */
          return 0x9591;
        case 0xD6:            /* 0x8AD6 */
          return 0x95A2;
        case 0xD7:            /* 0x8AD7 */
          return 0x9665;
        case 0xD8:            /* 0x8AD8 */
          return 0x97D3;
        case 0xD9:            /* 0x8AD9 */
          return 0x9928;
        case 0xDA:            /* 0x8ADA */
          return 0x8218;
        case 0xDB:            /* 0x8ADB */
          return 0x4E38;
        case 0xDC:            /* 0x8ADC */
          return 0x542B;
        case 0xDD:            /* 0x8ADD */
          return 0x5CB8;
        case 0xDE:            /* 0x8ADE */
          return 0x5DCC;
        case 0xDF:            /* 0x8ADF */
          return 0x73A9;
        case 0xE0:            /* 0x8AE0 */
          return 0x764C;
        case 0xE1:            /* 0x8AE1 */
          return 0x773C;
        case 0xE2:            /* 0x8AE2 */
          return 0x5CA9;
        case 0xE3:            /* 0x8AE3 */
          return 0x7FEB;
        case 0xE4:            /* 0x8AE4 */
          return 0x8D0B;
        case 0xE5:            /* 0x8AE5 */
          return 0x96C1;
        case 0xE6:            /* 0x8AE6 */
          return 0x9811;
        case 0xE7:            /* 0x8AE7 */
          return 0x9854;
        case 0xE8:            /* 0x8AE8 */
          return 0x9858;
        case 0xE9:            /* 0x8AE9 */
          return 0x4F01;
        case 0xEA:            /* 0x8AEA */
          return 0x4F0E;
        case 0xEB:            /* 0x8AEB */
          return 0x5371;
        case 0xEC:            /* 0x8AEC */
          return 0x559C;
        case 0xED:            /* 0x8AED */
          return 0x5668;
        case 0xEE:            /* 0x8AEE */
          return 0x57FA;
        case 0xEF:            /* 0x8AEF */
          return 0x5947;
        case 0xF0:            /* 0x8AF0 */
          return 0x5B09;
        case 0xF1:            /* 0x8AF1 */
          return 0x5BC4;
        case 0xF2:            /* 0x8AF2 */
          return 0x5C90;
        case 0xF3:            /* 0x8AF3 */
          return 0x5E0C;
        case 0xF4:            /* 0x8AF4 */
          return 0x5E7E;
        case 0xF5:            /* 0x8AF5 */
          return 0x5FCC;
        case 0xF6:            /* 0x8AF6 */
          return 0x63EE;
        case 0xF7:            /* 0x8AF7 */
          return 0x673A;
        case 0xF8:            /* 0x8AF8 */
          return 0x65D7;
        case 0xF9:            /* 0x8AF9 */
          return 0x65E2;
        case 0xFA:            /* 0x8AFA */
          return 0x671F;
        case 0xFB:            /* 0x8AFB */
          return 0x68CB;
        case 0xFC:            /* 0x8AFC */
          return 0x68C4;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 8A */

    /* Lead Byte: 0x8Bxx */
    case 0x8B:
      switch (low_byte)
      {
        case 0x40:            /* 0x8B40 */
          return 0x6A5F;
        case 0x41:            /* 0x8B41 */
          return 0x5E30;
        case 0x42:            /* 0x8B42 */
          return 0x6BC5;
        case 0x43:            /* 0x8B43 */
          return 0x6C17;
        case 0x44:            /* 0x8B44 */
          return 0x6C7D;
        case 0x45:            /* 0x8B45 */
          return 0x757F;
        case 0x46:            /* 0x8B46 */
          return 0x7948;
        case 0x47:            /* 0x8B47 */
          return 0x5B63;
        case 0x48:            /* 0x8B48 */
          return 0x7A00;
        case 0x49:            /* 0x8B49 */
          return 0x7D00;
        case 0x4A:            /* 0x8B4A */
          return 0x5FBD;
        case 0x4B:            /* 0x8B4B */
          return 0x898F;
        case 0x4C:            /* 0x8B4C */
          return 0x8A18;
        case 0x4D:            /* 0x8B4D */
          return 0x8CB4;
        case 0x4E:            /* 0x8B4E */
          return 0x8D77;
        case 0x4F:            /* 0x8B4F */
          return 0x8ECC;
        case 0x50:            /* 0x8B50 */
          return 0x8F1D;
        case 0x51:            /* 0x8B51 */
          return 0x98E2;
        case 0x52:            /* 0x8B52 */
          return 0x9A0E;
        case 0x53:            /* 0x8B53 */
          return 0x9B3C;
        case 0x54:            /* 0x8B54 */
          return 0x4E80;
        case 0x55:            /* 0x8B55 */
          return 0x507D;
        case 0x56:            /* 0x8B56 */
          return 0x5100;
        case 0x57:            /* 0x8B57 */
          return 0x5993;
        case 0x58:            /* 0x8B58 */
          return 0x5B9C;
        case 0x59:            /* 0x8B59 */
          return 0x622F;
        case 0x5A:            /* 0x8B5A */
          return 0x6280;
        case 0x5B:            /* 0x8B5B */
          return 0x64EC;
        case 0x5C:            /* 0x8B5C */
          return 0x6B3A;
        case 0x5D:            /* 0x8B5D */
          return 0x72A0;
        case 0x5E:            /* 0x8B5E */
          return 0x7591;
        case 0x5F:            /* 0x8B5F */
          return 0x7947;
        case 0x60:            /* 0x8B60 */
          return 0x7FA9;
        case 0x61:            /* 0x8B61 */
          return 0x87FB;
        case 0x62:            /* 0x8B62 */
          return 0x8ABC;
        case 0x63:            /* 0x8B63 */
          return 0x8B70;
        case 0x64:            /* 0x8B64 */
          return 0x63AC;
        case 0x65:            /* 0x8B65 */
          return 0x83CA;
        case 0x66:            /* 0x8B66 */
          return 0x97A0;
        case 0x67:            /* 0x8B67 */
          return 0x5409;
        case 0x68:            /* 0x8B68 */
          return 0x5403;
        case 0x69:            /* 0x8B69 */
          return 0x55AB;
        case 0x6A:            /* 0x8B6A */
          return 0x6854;
        case 0x6B:            /* 0x8B6B */
          return 0x6A58;
        case 0x6C:            /* 0x8B6C */
          return 0x8A70;
        case 0x6D:            /* 0x8B6D */
          return 0x7827;
        case 0x6E:            /* 0x8B6E */
          return 0x6775;
        case 0x6F:            /* 0x8B6F */
          return 0x9ECD;
        case 0x70:            /* 0x8B70 */
          return 0x5374;
        case 0x71:            /* 0x8B71 */
          return 0x5BA2;
        case 0x72:            /* 0x8B72 */
          return 0x811A;
        case 0x73:            /* 0x8B73 */
          return 0x8650;
        case 0x74:            /* 0x8B74 */
          return 0x9006;
        case 0x75:            /* 0x8B75 */
          return 0x4E18;
        case 0x76:            /* 0x8B76 */
          return 0x4E45;
        case 0x77:            /* 0x8B77 */
          return 0x4EC7;
        case 0x78:            /* 0x8B78 */
          return 0x4F11;
        case 0x79:            /* 0x8B79 */
          return 0x53CA;
        case 0x7A:            /* 0x8B7A */
          return 0x5438;
        case 0x7B:            /* 0x8B7B */
          return 0x5BAE;
        case 0x7C:            /* 0x8B7C */
          return 0x5F13;
        case 0x7D:            /* 0x8B7D */
          return 0x6025;
        case 0x7E:            /* 0x8B7E */
          return 0x6551;
        case 0x80:            /* 0x8B80 */
          return 0x673D;
        case 0x81:            /* 0x8B81 */
          return 0x6C42;
        case 0x82:            /* 0x8B82 */
          return 0x6C72;
        case 0x83:            /* 0x8B83 */
          return 0x6CE3;
        case 0x84:            /* 0x8B84 */
          return 0x7078;
        case 0x85:            /* 0x8B85 */
          return 0x7403;
        case 0x86:            /* 0x8B86 */
          return 0x7A76;
        case 0x87:            /* 0x8B87 */
          return 0x7AAE;
        case 0x88:            /* 0x8B88 */
          return 0x7B08;
        case 0x89:            /* 0x8B89 */
          return 0x7D1A;
        case 0x8A:            /* 0x8B8A */
          return 0x7CFE;
        case 0x8B:            /* 0x8B8B */
          return 0x7D66;
        case 0x8C:            /* 0x8B8C */
          return 0x65E7;
        case 0x8D:            /* 0x8B8D */
          return 0x725B;
        case 0x8E:            /* 0x8B8E */
          return 0x53BB;
        case 0x8F:            /* 0x8B8F */
          return 0x5C45;
        case 0x90:            /* 0x8B90 */
          return 0x5DE8;
        case 0x91:            /* 0x8B91 */
          return 0x62D2;
        case 0x92:            /* 0x8B92 */
          return 0x62E0;
        case 0x93:            /* 0x8B93 */
          return 0x6319;
        case 0x94:            /* 0x8B94 */
          return 0x6E20;
        case 0x95:            /* 0x8B95 */
          return 0x865A;
        case 0x96:            /* 0x8B96 */
          return 0x8A31;
        case 0x97:            /* 0x8B97 */
          return 0x8DDD;
        case 0x98:            /* 0x8B98 */
          return 0x92F8;
        case 0x99:            /* 0x8B99 */
          return 0x6F01;
        case 0x9A:            /* 0x8B9A */
          return 0x79A6;
        case 0x9B:            /* 0x8B9B */
          return 0x9B5A;
        case 0x9C:            /* 0x8B9C */
          return 0x4EA8;
        case 0x9D:            /* 0x8B9D */
          return 0x4EAB;
        case 0x9E:            /* 0x8B9E */
          return 0x4EAC;
        case 0x9F:            /* 0x8B9F */
          return 0x4F9B;
        case 0xA0:            /* 0x8BA0 */
          return 0x4FA0;
        case 0xA1:            /* 0x8BA1 */
          return 0x50D1;
        case 0xA2:            /* 0x8BA2 */
          return 0x5147;
        case 0xA3:            /* 0x8BA3 */
          return 0x7AF6;
        case 0xA4:            /* 0x8BA4 */
          return 0x5171;
        case 0xA5:            /* 0x8BA5 */
          return 0x51F6;
        case 0xA6:            /* 0x8BA6 */
          return 0x5354;
        case 0xA7:            /* 0x8BA7 */
          return 0x5321;
        case 0xA8:            /* 0x8BA8 */
          return 0x537F;
        case 0xA9:            /* 0x8BA9 */
          return 0x53EB;
        case 0xAA:            /* 0x8BAA */
          return 0x55AC;
        case 0xAB:            /* 0x8BAB */
          return 0x5883;
        case 0xAC:            /* 0x8BAC */
          return 0x5CE1;
        case 0xAD:            /* 0x8BAD */
          return 0x5F37;
        case 0xAE:            /* 0x8BAE */
          return 0x5F4A;
        case 0xAF:            /* 0x8BAF */
          return 0x602F;
        case 0xB0:            /* 0x8BB0 */
          return 0x6050;
        case 0xB1:            /* 0x8BB1 */
          return 0x606D;
        case 0xB2:            /* 0x8BB2 */
          return 0x631F;
        case 0xB3:            /* 0x8BB3 */
          return 0x6559;
        case 0xB4:            /* 0x8BB4 */
          return 0x6A4B;
        case 0xB5:            /* 0x8BB5 */
          return 0x6CC1;
        case 0xB6:            /* 0x8BB6 */
          return 0x72C2;
        case 0xB7:            /* 0x8BB7 */
          return 0x72ED;
        case 0xB8:            /* 0x8BB8 */
          return 0x77EF;
        case 0xB9:            /* 0x8BB9 */
          return 0x80F8;
        case 0xBA:            /* 0x8BBA */
          return 0x8105;
        case 0xBB:            /* 0x8BBB */
          return 0x8208;
        case 0xBC:            /* 0x8BBC */
          return 0x854E;
        case 0xBD:            /* 0x8BBD */
          return 0x90F7;
        case 0xBE:            /* 0x8BBE */
          return 0x93E1;
        case 0xBF:            /* 0x8BBF */
          return 0x97FF;
        case 0xC0:            /* 0x8BC0 */
          return 0x9957;
        case 0xC1:            /* 0x8BC1 */
          return 0x9A5A;
        case 0xC2:            /* 0x8BC2 */
          return 0x4EF0;
        case 0xC3:            /* 0x8BC3 */
          return 0x51DD;
        case 0xC4:            /* 0x8BC4 */
          return 0x5C2D;
        case 0xC5:            /* 0x8BC5 */
          return 0x6681;
        case 0xC6:            /* 0x8BC6 */
          return 0x696D;
        case 0xC7:            /* 0x8BC7 */
          return 0x5C40;
        case 0xC8:            /* 0x8BC8 */
          return 0x66F2;
        case 0xC9:            /* 0x8BC9 */
          return 0x6975;
        case 0xCA:            /* 0x8BCA */
          return 0x7389;
        case 0xCB:            /* 0x8BCB */
          return 0x6850;
        case 0xCC:            /* 0x8BCC */
          return 0x7C81;
        case 0xCD:            /* 0x8BCD */
          return 0x50C5;
        case 0xCE:            /* 0x8BCE */
          return 0x52E4;
        case 0xCF:            /* 0x8BCF */
          return 0x5747;
        case 0xD0:            /* 0x8BD0 */
          return 0x5DFE;
        case 0xD1:            /* 0x8BD1 */
          return 0x9326;
        case 0xD2:            /* 0x8BD2 */
          return 0x65A4;
        case 0xD3:            /* 0x8BD3 */
          return 0x6B23;
        case 0xD4:            /* 0x8BD4 */
          return 0x6B3D;
        case 0xD5:            /* 0x8BD5 */
          return 0x7434;
        case 0xD6:            /* 0x8BD6 */
          return 0x7981;
        case 0xD7:            /* 0x8BD7 */
          return 0x79BD;
        case 0xD8:            /* 0x8BD8 */
          return 0x7B4B;
        case 0xD9:            /* 0x8BD9 */
          return 0x7DCA;
        case 0xDA:            /* 0x8BDA */
          return 0x82B9;
        case 0xDB:            /* 0x8BDB */
          return 0x83CC;
        case 0xDC:            /* 0x8BDC */
          return 0x887F;
        case 0xDD:            /* 0x8BDD */
          return 0x895F;
        case 0xDE:            /* 0x8BDE */
          return 0x8B39;
        case 0xDF:            /* 0x8BDF */
          return 0x8FD1;
        case 0xE0:            /* 0x8BE0 */
          return 0x91D1;
        case 0xE1:            /* 0x8BE1 */
          return 0x541F;
        case 0xE2:            /* 0x8BE2 */
          return 0x9280;
        case 0xE3:            /* 0x8BE3 */
          return 0x4E5D;
        case 0xE4:            /* 0x8BE4 */
          return 0x5036;
        case 0xE5:            /* 0x8BE5 */
          return 0x53E5;
        case 0xE6:            /* 0x8BE6 */
          return 0x533A;
        case 0xE7:            /* 0x8BE7 */
          return 0x72D7;
        case 0xE8:            /* 0x8BE8 */
          return 0x7396;
        case 0xE9:            /* 0x8BE9 */
          return 0x77E9;
        case 0xEA:            /* 0x8BEA */
          return 0x82E6;
        case 0xEB:            /* 0x8BEB */
          return 0x8EAF;
        case 0xEC:            /* 0x8BEC */
          return 0x99C6;
        case 0xED:            /* 0x8BED */
          return 0x99C8;
        case 0xEE:            /* 0x8BEE */
          return 0x99D2;
        case 0xEF:            /* 0x8BEF */
          return 0x5177;
        case 0xF0:            /* 0x8BF0 */
          return 0x611A;
        case 0xF1:            /* 0x8BF1 */
          return 0x865E;
        case 0xF2:            /* 0x8BF2 */
          return 0x55B0;
        case 0xF3:            /* 0x8BF3 */
          return 0x7A7A;
        case 0xF4:            /* 0x8BF4 */
          return 0x5076;
        case 0xF5:            /* 0x8BF5 */
          return 0x5BD3;
        case 0xF6:            /* 0x8BF6 */
          return 0x9047;
        case 0xF7:            /* 0x8BF7 */
          return 0x9685;
        case 0xF8:            /* 0x8BF8 */
          return 0x4E32;
        case 0xF9:            /* 0x8BF9 */
          return 0x6ADB;
        case 0xFA:            /* 0x8BFA */
          return 0x91E7;
        case 0xFB:            /* 0x8BFB */
          return 0x5C51;
        case 0xFC:            /* 0x8BFC */
          return 0x5C48;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 8B */

    /* Lead Byte: 0x8Cxx */
    case 0x8C:
      switch (low_byte)
      {
        case 0x40:            /* 0x8C40 */
          return 0x6398;
        case 0x41:            /* 0x8C41 */
          return 0x7A9F;
        case 0x42:            /* 0x8C42 */
          return 0x6C93;
        case 0x43:            /* 0x8C43 */
          return 0x9774;
        case 0x44:            /* 0x8C44 */
          return 0x8F61;
        case 0x45:            /* 0x8C45 */
          return 0x7AAA;
        case 0x46:            /* 0x8C46 */
          return 0x718A;
        case 0x47:            /* 0x8C47 */
          return 0x9688;
        case 0x48:            /* 0x8C48 */
          return 0x7C82;
        case 0x49:            /* 0x8C49 */
          return 0x6817;
        case 0x4A:            /* 0x8C4A */
          return 0x7E70;
        case 0x4B:            /* 0x8C4B */
          return 0x6851;
        case 0x4C:            /* 0x8C4C */
          return 0x936C;
        case 0x4D:            /* 0x8C4D */
          return 0x52F2;
        case 0x4E:            /* 0x8C4E */
          return 0x541B;
        case 0x4F:            /* 0x8C4F */
          return 0x85AB;
        case 0x50:            /* 0x8C50 */
          return 0x8A13;
        case 0x51:            /* 0x8C51 */
          return 0x7FA4;
        case 0x52:            /* 0x8C52 */
          return 0x8ECD;
        case 0x53:            /* 0x8C53 */
          return 0x90E1;
        case 0x54:            /* 0x8C54 */
          return 0x5366;
        case 0x55:            /* 0x8C55 */
          return 0x8888;
        case 0x56:            /* 0x8C56 */
          return 0x7941;
        case 0x57:            /* 0x8C57 */
          return 0x4FC2;
        case 0x58:            /* 0x8C58 */
          return 0x50BE;
        case 0x59:            /* 0x8C59 */
          return 0x5211;
        case 0x5A:            /* 0x8C5A */
          return 0x5144;
        case 0x5B:            /* 0x8C5B */
          return 0x5553;
        case 0x5C:            /* 0x8C5C */
          return 0x572D;
        case 0x5D:            /* 0x8C5D */
          return 0x73EA;
        case 0x5E:            /* 0x8C5E */
          return 0x578B;
        case 0x5F:            /* 0x8C5F */
          return 0x5951;
        case 0x60:            /* 0x8C60 */
          return 0x5F62;
        case 0x61:            /* 0x8C61 */
          return 0x5F84;
        case 0x62:            /* 0x8C62 */
          return 0x6075;
        case 0x63:            /* 0x8C63 */
          return 0x6176;
        case 0x64:            /* 0x8C64 */
          return 0x6167;
        case 0x65:            /* 0x8C65 */
          return 0x61A9;
        case 0x66:            /* 0x8C66 */
          return 0x63B2;
        case 0x67:            /* 0x8C67 */
          return 0x643A;
        case 0x68:            /* 0x8C68 */
          return 0x656C;
        case 0x69:            /* 0x8C69 */
          return 0x666F;
        case 0x6A:            /* 0x8C6A */
          return 0x6842;
        case 0x6B:            /* 0x8C6B */
          return 0x6E13;
        case 0x6C:            /* 0x8C6C */
          return 0x7566;
        case 0x6D:            /* 0x8C6D */
          return 0x7A3D;
        case 0x6E:            /* 0x8C6E */
          return 0x7CFB;
        case 0x6F:            /* 0x8C6F */
          return 0x7D4C;
        case 0x70:            /* 0x8C70 */
          return 0x7D99;
        case 0x71:            /* 0x8C71 */
          return 0x7E4B;
        case 0x72:            /* 0x8C72 */
          return 0x7F6B;
        case 0x73:            /* 0x8C73 */
          return 0x830E;
        case 0x74:            /* 0x8C74 */
          return 0x834A;
        case 0x75:            /* 0x8C75 */
          return 0x86CD;
        case 0x76:            /* 0x8C76 */
          return 0x8A08;
        case 0x77:            /* 0x8C77 */
          return 0x8A63;
        case 0x78:            /* 0x8C78 */
          return 0x8B66;
        case 0x79:            /* 0x8C79 */
          return 0x8EFD;
        case 0x7A:            /* 0x8C7A */
          return 0x981A;
        case 0x7B:            /* 0x8C7B */
          return 0x9D8F;
        case 0x7C:            /* 0x8C7C */
          return 0x82B8;
        case 0x7D:            /* 0x8C7D */
          return 0x8FCE;
        case 0x7E:            /* 0x8C7E */
          return 0x9BE8;
        case 0x80:            /* 0x8C80 */
          return 0x5287;
        case 0x81:            /* 0x8C81 */
          return 0x621F;
        case 0x82:            /* 0x8C82 */
          return 0x6483;
        case 0x83:            /* 0x8C83 */
          return 0x6FC0;
        case 0x84:            /* 0x8C84 */
          return 0x9699;
        case 0x85:            /* 0x8C85 */
          return 0x6841;
        case 0x86:            /* 0x8C86 */
          return 0x5091;
        case 0x87:            /* 0x8C87 */
          return 0x6B20;
        case 0x88:            /* 0x8C88 */
          return 0x6C7A;
        case 0x89:            /* 0x8C89 */
          return 0x6F54;
        case 0x8A:            /* 0x8C8A */
          return 0x7A74;
        case 0x8B:            /* 0x8C8B */
          return 0x7D50;
        case 0x8C:            /* 0x8C8C */
          return 0x8840;
        case 0x8D:            /* 0x8C8D */
          return 0x8A23;
        case 0x8E:            /* 0x8C8E */
          return 0x6708;
        case 0x8F:            /* 0x8C8F */
          return 0x4EF6;
        case 0x90:            /* 0x8C90 */
          return 0x5039;
        case 0x91:            /* 0x8C91 */
          return 0x5026;
        case 0x92:            /* 0x8C92 */
          return 0x5065;
        case 0x93:            /* 0x8C93 */
          return 0x517C;
        case 0x94:            /* 0x8C94 */
          return 0x5238;
        case 0x95:            /* 0x8C95 */
          return 0x5263;
        case 0x96:            /* 0x8C96 */
          return 0x55A7;
        case 0x97:            /* 0x8C97 */
          return 0x570F;
        case 0x98:            /* 0x8C98 */
          return 0x5805;
        case 0x99:            /* 0x8C99 */
          return 0x5ACC;
        case 0x9A:            /* 0x8C9A */
          return 0x5EFA;
        case 0x9B:            /* 0x8C9B */
          return 0x61B2;
        case 0x9C:            /* 0x8C9C */
          return 0x61F8;
        case 0x9D:            /* 0x8C9D */
          return 0x62F3;
        case 0x9E:            /* 0x8C9E */
          return 0x6372;
        case 0x9F:            /* 0x8C9F */
          return 0x691C;
        case 0xA0:            /* 0x8CA0 */
          return 0x6A29;
        case 0xA1:            /* 0x8CA1 */
          return 0x727D;
        case 0xA2:            /* 0x8CA2 */
          return 0x72AC;
        case 0xA3:            /* 0x8CA3 */
          return 0x732E;
        case 0xA4:            /* 0x8CA4 */
          return 0x7814;
        case 0xA5:            /* 0x8CA5 */
          return 0x786F;
        case 0xA6:            /* 0x8CA6 */
          return 0x7D79;
        case 0xA7:            /* 0x8CA7 */
          return 0x770C;
        case 0xA8:            /* 0x8CA8 */
          return 0x80A9;
        case 0xA9:            /* 0x8CA9 */
          return 0x898B;
        case 0xAA:            /* 0x8CAA */
          return 0x8B19;
        case 0xAB:            /* 0x8CAB */
          return 0x8CE2;
        case 0xAC:            /* 0x8CAC */
          return 0x8ED2;
        case 0xAD:            /* 0x8CAD */
          return 0x9063;
        case 0xAE:            /* 0x8CAE */
          return 0x9375;
        case 0xAF:            /* 0x8CAF */
          return 0x967A;
        case 0xB0:            /* 0x8CB0 */
          return 0x9855;
        case 0xB1:            /* 0x8CB1 */
          return 0x9A13;
        case 0xB2:            /* 0x8CB2 */
          return 0x9E78;
        case 0xB3:            /* 0x8CB3 */
          return 0x5143;
        case 0xB4:            /* 0x8CB4 */
          return 0x539F;
        case 0xB5:            /* 0x8CB5 */
          return 0x53B3;
        case 0xB6:            /* 0x8CB6 */
          return 0x5E7B;
        case 0xB7:            /* 0x8CB7 */
          return 0x5F26;
        case 0xB8:            /* 0x8CB8 */
          return 0x6E1B;
        case 0xB9:            /* 0x8CB9 */
          return 0x6E90;
        case 0xBA:            /* 0x8CBA */
          return 0x7384;
        case 0xBB:            /* 0x8CBB */
          return 0x73FE;
        case 0xBC:            /* 0x8CBC */
          return 0x7D43;
        case 0xBD:            /* 0x8CBD */
          return 0x8237;
        case 0xBE:            /* 0x8CBE */
          return 0x8A00;
        case 0xBF:            /* 0x8CBF */
          return 0x8AFA;
        case 0xC0:            /* 0x8CC0 */
          return 0x9650;
        case 0xC1:            /* 0x8CC1 */
          return 0x4E4E;
        case 0xC2:            /* 0x8CC2 */
          return 0x500B;
        case 0xC3:            /* 0x8CC3 */
          return 0x53E4;
        case 0xC4:            /* 0x8CC4 */
          return 0x547C;
        case 0xC5:            /* 0x8CC5 */
          return 0x56FA;
        case 0xC6:            /* 0x8CC6 */
          return 0x59D1;
        case 0xC7:            /* 0x8CC7 */
          return 0x5B64;
        case 0xC8:            /* 0x8CC8 */
          return 0x5DF1;
        case 0xC9:            /* 0x8CC9 */
          return 0x5EAB;
        case 0xCA:            /* 0x8CCA */
          return 0x5F27;
        case 0xCB:            /* 0x8CCB */
          return 0x6238;
        case 0xCC:            /* 0x8CCC */
          return 0x6545;
        case 0xCD:            /* 0x8CCD */
          return 0x67AF;
        case 0xCE:            /* 0x8CCE */
          return 0x6E56;
        case 0xCF:            /* 0x8CCF */
          return 0x72D0;
        case 0xD0:            /* 0x8CD0 */
          return 0x7CCA;
        case 0xD1:            /* 0x8CD1 */
          return 0x88B4;
        case 0xD2:            /* 0x8CD2 */
          return 0x80A1;
        case 0xD3:            /* 0x8CD3 */
          return 0x80E1;
        case 0xD4:            /* 0x8CD4 */
          return 0x83F0;
        case 0xD5:            /* 0x8CD5 */
          return 0x864E;
        case 0xD6:            /* 0x8CD6 */
          return 0x8A87;
        case 0xD7:            /* 0x8CD7 */
          return 0x8DE8;
        case 0xD8:            /* 0x8CD8 */
          return 0x9237;
        case 0xD9:            /* 0x8CD9 */
          return 0x96C7;
        case 0xDA:            /* 0x8CDA */
          return 0x9867;
        case 0xDB:            /* 0x8CDB */
          return 0x9F13;
        case 0xDC:            /* 0x8CDC */
          return 0x4E94;
        case 0xDD:            /* 0x8CDD */
          return 0x4E92;
        case 0xDE:            /* 0x8CDE */
          return 0x4F0D;
        case 0xDF:            /* 0x8CDF */
          return 0x5348;
        case 0xE0:            /* 0x8CE0 */
          return 0x5449;
        case 0xE1:            /* 0x8CE1 */
          return 0x543E;
        case 0xE2:            /* 0x8CE2 */
          return 0x5A2F;
        case 0xE3:            /* 0x8CE3 */
          return 0x5F8C;
        case 0xE4:            /* 0x8CE4 */
          return 0x5FA1;
        case 0xE5:            /* 0x8CE5 */
          return 0x609F;
        case 0xE6:            /* 0x8CE6 */
          return 0x68A7;
        case 0xE7:            /* 0x8CE7 */
          return 0x6A8E;
        case 0xE8:            /* 0x8CE8 */
          return 0x745A;
        case 0xE9:            /* 0x8CE9 */
          return 0x7881;
        case 0xEA:            /* 0x8CEA */
          return 0x8A9E;
        case 0xEB:            /* 0x8CEB */
          return 0x8AA4;
        case 0xEC:            /* 0x8CEC */
          return 0x8B77;
        case 0xED:            /* 0x8CED */
          return 0x9190;
        case 0xEE:            /* 0x8CEE */
          return 0x4E5E;
        case 0xEF:            /* 0x8CEF */
          return 0x9BC9;
        case 0xF0:            /* 0x8CF0 */
          return 0x4EA4;
        case 0xF1:            /* 0x8CF1 */
          return 0x4F7C;
        case 0xF2:            /* 0x8CF2 */
          return 0x4FAF;
        case 0xF3:            /* 0x8CF3 */
          return 0x5019;
        case 0xF4:            /* 0x8CF4 */
          return 0x5016;
        case 0xF5:            /* 0x8CF5 */
          return 0x5149;
        case 0xF6:            /* 0x8CF6 */
          return 0x516C;
        case 0xF7:            /* 0x8CF7 */
          return 0x529F;
        case 0xF8:            /* 0x8CF8 */
          return 0x52B9;
        case 0xF9:            /* 0x8CF9 */
          return 0x52FE;
        case 0xFA:            /* 0x8CFA */
          return 0x539A;
        case 0xFB:            /* 0x8CFB */
          return 0x53E3;
        case 0xFC:            /* 0x8CFC */
          return 0x5411;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 8C */

    /* Lead Byte: 0x8Dxx */
    case 0x8D:
      switch (low_byte)
      {
        case 0x40:            /* 0x8D40 */
          return 0x540E;
        case 0x41:            /* 0x8D41 */
          return 0x5589;
        case 0x42:            /* 0x8D42 */
          return 0x5751;
        case 0x43:            /* 0x8D43 */
          return 0x57A2;
        case 0x44:            /* 0x8D44 */
          return 0x597D;
        case 0x45:            /* 0x8D45 */
          return 0x5B54;
        case 0x46:            /* 0x8D46 */
          return 0x5B5D;
        case 0x47:            /* 0x8D47 */
          return 0x5B8F;
        case 0x48:            /* 0x8D48 */
          return 0x5DE5;
        case 0x49:            /* 0x8D49 */
          return 0x5DE7;
        case 0x4A:            /* 0x8D4A */
          return 0x5DF7;
        case 0x4B:            /* 0x8D4B */
          return 0x5E78;
        case 0x4C:            /* 0x8D4C */
          return 0x5E83;
        case 0x4D:            /* 0x8D4D */
          return 0x5E9A;
        case 0x4E:            /* 0x8D4E */
          return 0x5EB7;
        case 0x4F:            /* 0x8D4F */
          return 0x5F18;
        case 0x50:            /* 0x8D50 */
          return 0x6052;
        case 0x51:            /* 0x8D51 */
          return 0x614C;
        case 0x52:            /* 0x8D52 */
          return 0x6297;
        case 0x53:            /* 0x8D53 */
          return 0x62D8;
        case 0x54:            /* 0x8D54 */
          return 0x63A7;
        case 0x55:            /* 0x8D55 */
          return 0x653B;
        case 0x56:            /* 0x8D56 */
          return 0x6602;
        case 0x57:            /* 0x8D57 */
          return 0x6643;
        case 0x58:            /* 0x8D58 */
          return 0x66F4;
        case 0x59:            /* 0x8D59 */
          return 0x676D;
        case 0x5A:            /* 0x8D5A */
          return 0x6821;
        case 0x5B:            /* 0x8D5B */
          return 0x6897;
        case 0x5C:            /* 0x8D5C */
          return 0x69CB;
        case 0x5D:            /* 0x8D5D */
          return 0x6C5F;
        case 0x5E:            /* 0x8D5E */
          return 0x6D2A;
        case 0x5F:            /* 0x8D5F */
          return 0x6D69;
        case 0x60:            /* 0x8D60 */
          return 0x6E2F;
        case 0x61:            /* 0x8D61 */
          return 0x6E9D;
        case 0x62:            /* 0x8D62 */
          return 0x7532;
        case 0x63:            /* 0x8D63 */
          return 0x7687;
        case 0x64:            /* 0x8D64 */
          return 0x786C;
        case 0x65:            /* 0x8D65 */
          return 0x7A3F;
        case 0x66:            /* 0x8D66 */
          return 0x7CE0;
        case 0x67:            /* 0x8D67 */
          return 0x7D05;
        case 0x68:            /* 0x8D68 */
          return 0x7D18;
        case 0x69:            /* 0x8D69 */
          return 0x7D5E;
        case 0x6A:            /* 0x8D6A */
          return 0x7DB1;
        case 0x6B:            /* 0x8D6B */
          return 0x8015;
        case 0x6C:            /* 0x8D6C */
          return 0x8003;
        case 0x6D:            /* 0x8D6D */
          return 0x80AF;
        case 0x6E:            /* 0x8D6E */
          return 0x80B1;
        case 0x6F:            /* 0x8D6F */
          return 0x8154;
        case 0x70:            /* 0x8D70 */
          return 0x818F;
        case 0x71:            /* 0x8D71 */
          return 0x822A;
        case 0x72:            /* 0x8D72 */
          return 0x8352;
        case 0x73:            /* 0x8D73 */
          return 0x884C;
        case 0x74:            /* 0x8D74 */
          return 0x8861;
        case 0x75:            /* 0x8D75 */
          return 0x8B1B;
        case 0x76:            /* 0x8D76 */
          return 0x8CA2;
        case 0x77:            /* 0x8D77 */
          return 0x8CFC;
        case 0x78:            /* 0x8D78 */
          return 0x90CA;
        case 0x79:            /* 0x8D79 */
          return 0x9175;
        case 0x7A:            /* 0x8D7A */
          return 0x9271;
        case 0x7B:            /* 0x8D7B */
          return 0x783F;
        case 0x7C:            /* 0x8D7C */
          return 0x92FC;
        case 0x7D:            /* 0x8D7D */
          return 0x95A4;
        case 0x7E:            /* 0x8D7E */
          return 0x964D;
        case 0x80:            /* 0x8D80 */
          return 0x9805;
        case 0x81:            /* 0x8D81 */
          return 0x9999;
        case 0x82:            /* 0x8D82 */
          return 0x9AD8;
        case 0x83:            /* 0x8D83 */
          return 0x9D3B;
        case 0x84:            /* 0x8D84 */
          return 0x525B;
        case 0x85:            /* 0x8D85 */
          return 0x52AB;
        case 0x86:            /* 0x8D86 */
          return 0x53F7;
        case 0x87:            /* 0x8D87 */
          return 0x5408;
        case 0x88:            /* 0x8D88 */
          return 0x58D5;
        case 0x89:            /* 0x8D89 */
          return 0x62F7;
        case 0x8A:            /* 0x8D8A */
          return 0x6FE0;
        case 0x8B:            /* 0x8D8B */
          return 0x8C6A;
        case 0x8C:            /* 0x8D8C */
          return 0x8F5F;
        case 0x8D:            /* 0x8D8D */
          return 0x9EB9;
        case 0x8E:            /* 0x8D8E */
          return 0x514B;
        case 0x8F:            /* 0x8D8F */
          return 0x523B;
        case 0x90:            /* 0x8D90 */
          return 0x544A;
        case 0x91:            /* 0x8D91 */
          return 0x56FD;
        case 0x92:            /* 0x8D92 */
          return 0x7A40;
        case 0x93:            /* 0x8D93 */
          return 0x9177;
        case 0x94:            /* 0x8D94 */
          return 0x9D60;
        case 0x95:            /* 0x8D95 */
          return 0x9ED2;
        case 0x96:            /* 0x8D96 */
          return 0x7344;
        case 0x97:            /* 0x8D97 */
          return 0x6F09;
        case 0x98:            /* 0x8D98 */
          return 0x8170;
        case 0x99:            /* 0x8D99 */
          return 0x7511;
        case 0x9A:            /* 0x8D9A */
          return 0x5FFD;
        case 0x9B:            /* 0x8D9B */
          return 0x60DA;
        case 0x9C:            /* 0x8D9C */
          return 0x9AA8;
        case 0x9D:            /* 0x8D9D */
          return 0x72DB;
        case 0x9E:            /* 0x8D9E */
          return 0x8FBC;
        case 0x9F:            /* 0x8D9F */
          return 0x6B64;
        case 0xA0:            /* 0x8DA0 */
          return 0x9803;
        case 0xA1:            /* 0x8DA1 */
          return 0x4ECA;
        case 0xA2:            /* 0x8DA2 */
          return 0x56F0;
        case 0xA3:            /* 0x8DA3 */
          return 0x5764;
        case 0xA4:            /* 0x8DA4 */
          return 0x58BE;
        case 0xA5:            /* 0x8DA5 */
          return 0x5A5A;
        case 0xA6:            /* 0x8DA6 */
          return 0x6068;
        case 0xA7:            /* 0x8DA7 */
          return 0x61C7;
        case 0xA8:            /* 0x8DA8 */
          return 0x660F;
        case 0xA9:            /* 0x8DA9 */
          return 0x6606;
        case 0xAA:            /* 0x8DAA */
          return 0x6839;
        case 0xAB:            /* 0x8DAB */
          return 0x68B1;
        case 0xAC:            /* 0x8DAC */
          return 0x6DF7;
        case 0xAD:            /* 0x8DAD */
          return 0x75D5;
        case 0xAE:            /* 0x8DAE */
          return 0x7D3A;
        case 0xAF:            /* 0x8DAF */
          return 0x826E;
        case 0xB0:            /* 0x8DB0 */
          return 0x9B42;
        case 0xB1:            /* 0x8DB1 */
          return 0x4E9B;
        case 0xB2:            /* 0x8DB2 */
          return 0x4F50;
        case 0xB3:            /* 0x8DB3 */
          return 0x53C9;
        case 0xB4:            /* 0x8DB4 */
          return 0x5506;
        case 0xB5:            /* 0x8DB5 */
          return 0x5D6F;
        case 0xB6:            /* 0x8DB6 */
          return 0x5DE6;
        case 0xB7:            /* 0x8DB7 */
          return 0x5DEE;
        case 0xB8:            /* 0x8DB8 */
          return 0x67FB;
        case 0xB9:            /* 0x8DB9 */
          return 0x6C99;
        case 0xBA:            /* 0x8DBA */
          return 0x7473;
        case 0xBB:            /* 0x8DBB */
          return 0x7802;
        case 0xBC:            /* 0x8DBC */
          return 0x8A50;
        case 0xBD:            /* 0x8DBD */
          return 0x9396;
        case 0xBE:            /* 0x8DBE */
          return 0x88DF;
        case 0xBF:            /* 0x8DBF */
          return 0x5750;
        case 0xC0:            /* 0x8DC0 */
          return 0x5EA7;
        case 0xC1:            /* 0x8DC1 */
          return 0x632B;
        case 0xC2:            /* 0x8DC2 */
          return 0x50B5;
        case 0xC3:            /* 0x8DC3 */
          return 0x50AC;
        case 0xC4:            /* 0x8DC4 */
          return 0x518D;
        case 0xC5:            /* 0x8DC5 */
          return 0x6700;
        case 0xC6:            /* 0x8DC6 */
          return 0x54C9;
        case 0xC7:            /* 0x8DC7 */
          return 0x585E;
        case 0xC8:            /* 0x8DC8 */
          return 0x59BB;
        case 0xC9:            /* 0x8DC9 */
          return 0x5BB0;
        case 0xCA:            /* 0x8DCA */
          return 0x5F69;
        case 0xCB:            /* 0x8DCB */
          return 0x624D;
        case 0xCC:            /* 0x8DCC */
          return 0x63A1;
        case 0xCD:            /* 0x8DCD */
          return 0x683D;
        case 0xCE:            /* 0x8DCE */
          return 0x6B73;
        case 0xCF:            /* 0x8DCF */
          return 0x6E08;
        case 0xD0:            /* 0x8DD0 */
          return 0x707D;
        case 0xD1:            /* 0x8DD1 */
          return 0x91C7;
        case 0xD2:            /* 0x8DD2 */
          return 0x7280;
        case 0xD3:            /* 0x8DD3 */
          return 0x7815;
        case 0xD4:            /* 0x8DD4 */
          return 0x7826;
        case 0xD5:            /* 0x8DD5 */
          return 0x796D;
        case 0xD6:            /* 0x8DD6 */
          return 0x658E;
        case 0xD7:            /* 0x8DD7 */
          return 0x7D30;
        case 0xD8:            /* 0x8DD8 */
          return 0x83DC;
        case 0xD9:            /* 0x8DD9 */
          return 0x88C1;
        case 0xDA:            /* 0x8DDA */
          return 0x8F09;
        case 0xDB:            /* 0x8DDB */
          return 0x969B;
        case 0xDC:            /* 0x8DDC */
          return 0x5264;
        case 0xDD:            /* 0x8DDD */
          return 0x5728;
        case 0xDE:            /* 0x8DDE */
          return 0x6750;
        case 0xDF:            /* 0x8DDF */
          return 0x7F6A;
        case 0xE0:            /* 0x8DE0 */
          return 0x8CA1;
        case 0xE1:            /* 0x8DE1 */
          return 0x51B4;
        case 0xE2:            /* 0x8DE2 */
          return 0x5742;
        case 0xE3:            /* 0x8DE3 */
          return 0x962A;
        case 0xE4:            /* 0x8DE4 */
          return 0x583A;
        case 0xE5:            /* 0x8DE5 */
          return 0x698A;
        case 0xE6:            /* 0x8DE6 */
          return 0x80B4;
        case 0xE7:            /* 0x8DE7 */
          return 0x54B2;
        case 0xE8:            /* 0x8DE8 */
          return 0x5D0E;
        case 0xE9:            /* 0x8DE9 */
          return 0x57FC;
        case 0xEA:            /* 0x8DEA */
          return 0x7895;
        case 0xEB:            /* 0x8DEB */
          return 0x9DFA;
        case 0xEC:            /* 0x8DEC */
          return 0x4F5C;
        case 0xED:            /* 0x8DED */
          return 0x524A;
        case 0xEE:            /* 0x8DEE */
          return 0x548B;
        case 0xEF:            /* 0x8DEF */
          return 0x643E;
        case 0xF0:            /* 0x8DF0 */
          return 0x6628;
        case 0xF1:            /* 0x8DF1 */
          return 0x6714;
        case 0xF2:            /* 0x8DF2 */
          return 0x67F5;
        case 0xF3:            /* 0x8DF3 */
          return 0x7A84;
        case 0xF4:            /* 0x8DF4 */
          return 0x7B56;
        case 0xF5:            /* 0x8DF5 */
          return 0x7D22;
        case 0xF6:            /* 0x8DF6 */
          return 0x932F;
        case 0xF7:            /* 0x8DF7 */
          return 0x685C;
        case 0xF8:            /* 0x8DF8 */
          return 0x9BAD;
        case 0xF9:            /* 0x8DF9 */
          return 0x7B39;
        case 0xFA:            /* 0x8DFA */
          return 0x5319;
        case 0xFB:            /* 0x8DFB */
          return 0x518A;
        case 0xFC:            /* 0x8DFC */
          return 0x5237;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 8D */

    /* Lead Byte: 0x8Exx */
    case 0x8E:
      switch (low_byte)
      {
        case 0x40:            /* 0x8E40 */
          return 0x5BDF;
        case 0x41:            /* 0x8E41 */
          return 0x62F6;
        case 0x42:            /* 0x8E42 */
          return 0x64AE;
        case 0x43:            /* 0x8E43 */
          return 0x64E6;
        case 0x44:            /* 0x8E44 */
          return 0x672D;
        case 0x45:            /* 0x8E45 */
          return 0x6BBA;
        case 0x46:            /* 0x8E46 */
          return 0x85A9;
        case 0x47:            /* 0x8E47 */
          return 0x96D1;
        case 0x48:            /* 0x8E48 */
          return 0x7690;
        case 0x49:            /* 0x8E49 */
          return 0x9BD6;
        case 0x4A:            /* 0x8E4A */
          return 0x634C;
        case 0x4B:            /* 0x8E4B */
          return 0x9306;
        case 0x4C:            /* 0x8E4C */
          return 0x9BAB;
        case 0x4D:            /* 0x8E4D */
          return 0x76BF;
        case 0x4E:            /* 0x8E4E */
          return 0x6652;
        case 0x4F:            /* 0x8E4F */
          return 0x4E09;
        case 0x50:            /* 0x8E50 */
          return 0x5098;
        case 0x51:            /* 0x8E51 */
          return 0x53C2;
        case 0x52:            /* 0x8E52 */
          return 0x5C71;
        case 0x53:            /* 0x8E53 */
          return 0x60E8;
        case 0x54:            /* 0x8E54 */
          return 0x6492;
        case 0x55:            /* 0x8E55 */
          return 0x6563;
        case 0x56:            /* 0x8E56 */
          return 0x685F;
        case 0x57:            /* 0x8E57 */
          return 0x71E6;
        case 0x58:            /* 0x8E58 */
          return 0x73CA;
        case 0x59:            /* 0x8E59 */
          return 0x7523;
        case 0x5A:            /* 0x8E5A */
          return 0x7B97;
        case 0x5B:            /* 0x8E5B */
          return 0x7E82;
        case 0x5C:            /* 0x8E5C */
          return 0x8695;
        case 0x5D:            /* 0x8E5D */
          return 0x8B83;
        case 0x5E:            /* 0x8E5E */
          return 0x8CDB;
        case 0x5F:            /* 0x8E5F */
          return 0x9178;
        case 0x60:            /* 0x8E60 */
          return 0x9910;
        case 0x61:            /* 0x8E61 */
          return 0x65AC;
        case 0x62:            /* 0x8E62 */
          return 0x66AB;
        case 0x63:            /* 0x8E63 */
          return 0x6B8B;
        case 0x64:            /* 0x8E64 */
          return 0x4ED5;
        case 0x65:            /* 0x8E65 */
          return 0x4ED4;
        case 0x66:            /* 0x8E66 */
          return 0x4F3A;
        case 0x67:            /* 0x8E67 */
          return 0x4F7F;
        case 0x68:            /* 0x8E68 */
          return 0x523A;
        case 0x69:            /* 0x8E69 */
          return 0x53F8;
        case 0x6A:            /* 0x8E6A */
          return 0x53F2;
        case 0x6B:            /* 0x8E6B */
          return 0x55E3;
        case 0x6C:            /* 0x8E6C */
          return 0x56DB;
        case 0x6D:            /* 0x8E6D */
          return 0x58EB;
        case 0x6E:            /* 0x8E6E */
          return 0x59CB;
        case 0x6F:            /* 0x8E6F */
          return 0x59C9;
        case 0x70:            /* 0x8E70 */
          return 0x59FF;
        case 0x71:            /* 0x8E71 */
          return 0x5B50;
        case 0x72:            /* 0x8E72 */
          return 0x5C4D;
        case 0x73:            /* 0x8E73 */
          return 0x5E02;
        case 0x74:            /* 0x8E74 */
          return 0x5E2B;
        case 0x75:            /* 0x8E75 */
          return 0x5FD7;
        case 0x76:            /* 0x8E76 */
          return 0x601D;
        case 0x77:            /* 0x8E77 */
          return 0x6307;
        case 0x78:            /* 0x8E78 */
          return 0x652F;
        case 0x79:            /* 0x8E79 */
          return 0x5B5C;
        case 0x7A:            /* 0x8E7A */
          return 0x65AF;
        case 0x7B:            /* 0x8E7B */
          return 0x65BD;
        case 0x7C:            /* 0x8E7C */
          return 0x65E8;
        case 0x7D:            /* 0x8E7D */
          return 0x679D;
        case 0x7E:            /* 0x8E7E */
          return 0x6B62;
        case 0x80:            /* 0x8E80 */
          return 0x6B7B;
        case 0x81:            /* 0x8E81 */
          return 0x6C0F;
        case 0x82:            /* 0x8E82 */
          return 0x7345;
        case 0x83:            /* 0x8E83 */
          return 0x7949;
        case 0x84:            /* 0x8E84 */
          return 0x79C1;
        case 0x85:            /* 0x8E85 */
          return 0x7CF8;
        case 0x86:            /* 0x8E86 */
          return 0x7D19;
        case 0x87:            /* 0x8E87 */
          return 0x7D2B;
        case 0x88:            /* 0x8E88 */
          return 0x80A2;
        case 0x89:            /* 0x8E89 */
          return 0x8102;
        case 0x8A:            /* 0x8E8A */
          return 0x81F3;
        case 0x8B:            /* 0x8E8B */
          return 0x8996;
        case 0x8C:            /* 0x8E8C */
          return 0x8A5E;
        case 0x8D:            /* 0x8E8D */
          return 0x8A69;
        case 0x8E:            /* 0x8E8E */
          return 0x8A66;
        case 0x8F:            /* 0x8E8F */
          return 0x8A8C;
        case 0x90:            /* 0x8E90 */
          return 0x8AEE;
        case 0x91:            /* 0x8E91 */
          return 0x8CC7;
        case 0x92:            /* 0x8E92 */
          return 0x8CDC;
        case 0x93:            /* 0x8E93 */
          return 0x96CC;
        case 0x94:            /* 0x8E94 */
          return 0x98FC;
        case 0x95:            /* 0x8E95 */
          return 0x6B6F;
        case 0x96:            /* 0x8E96 */
          return 0x4E8B;
        case 0x97:            /* 0x8E97 */
          return 0x4F3C;
        case 0x98:            /* 0x8E98 */
          return 0x4F8D;
        case 0x99:            /* 0x8E99 */
          return 0x5150;
        case 0x9A:            /* 0x8E9A */
          return 0x5B57;
        case 0x9B:            /* 0x8E9B */
          return 0x5BFA;
        case 0x9C:            /* 0x8E9C */
          return 0x6148;
        case 0x9D:            /* 0x8E9D */
          return 0x6301;
        case 0x9E:            /* 0x8E9E */
          return 0x6642;
        case 0x9F:            /* 0x8E9F */
          return 0x6B21;
        case 0xA0:            /* 0x8EA0 */
          return 0x6ECB;
        case 0xA1:            /* 0x8EA1 */
          return 0x6CBB;
        case 0xA2:            /* 0x8EA2 */
          return 0x723E;
        case 0xA3:            /* 0x8EA3 */
          return 0x74BD;
        case 0xA4:            /* 0x8EA4 */
          return 0x75D4;
        case 0xA5:            /* 0x8EA5 */
          return 0x78C1;
        case 0xA6:            /* 0x8EA6 */
          return 0x793A;
        case 0xA7:            /* 0x8EA7 */
          return 0x800C;
        case 0xA8:            /* 0x8EA8 */
          return 0x8033;
        case 0xA9:            /* 0x8EA9 */
          return 0x81EA;
        case 0xAA:            /* 0x8EAA */
          return 0x8494;
        case 0xAB:            /* 0x8EAB */
          return 0x8F9E;
        case 0xAC:            /* 0x8EAC */
          return 0x6C50;
        case 0xAD:            /* 0x8EAD */
          return 0x9E7F;
        case 0xAE:            /* 0x8EAE */
          return 0x5F0F;
        case 0xAF:            /* 0x8EAF */
          return 0x8B58;
        case 0xB0:            /* 0x8EB0 */
          return 0x9D2B;
        case 0xB1:            /* 0x8EB1 */
          return 0x7AFA;
        case 0xB2:            /* 0x8EB2 */
          return 0x8EF8;
        case 0xB3:            /* 0x8EB3 */
          return 0x5B8D;
        case 0xB4:            /* 0x8EB4 */
          return 0x96EB;
        case 0xB5:            /* 0x8EB5 */
          return 0x4E03;
        case 0xB6:            /* 0x8EB6 */
          return 0x53F1;
        case 0xB7:            /* 0x8EB7 */
          return 0x57F7;
        case 0xB8:            /* 0x8EB8 */
          return 0x5931;
        case 0xB9:            /* 0x8EB9 */
          return 0x5AC9;
        case 0xBA:            /* 0x8EBA */
          return 0x5BA4;
        case 0xBB:            /* 0x8EBB */
          return 0x6089;
        case 0xBC:            /* 0x8EBC */
          return 0x6E7F;
        case 0xBD:            /* 0x8EBD */
          return 0x6F06;
        case 0xBE:            /* 0x8EBE */
          return 0x75BE;
        case 0xBF:            /* 0x8EBF */
          return 0x8CEA;
        case 0xC0:            /* 0x8EC0 */
          return 0x5B9F;
        case 0xC1:            /* 0x8EC1 */
          return 0x8500;
        case 0xC2:            /* 0x8EC2 */
          return 0x7BE0;
        case 0xC3:            /* 0x8EC3 */
          return 0x5072;
        case 0xC4:            /* 0x8EC4 */
          return 0x67F4;
        case 0xC5:            /* 0x8EC5 */
          return 0x829D;
        case 0xC6:            /* 0x8EC6 */
          return 0x5C61;
        case 0xC7:            /* 0x8EC7 */
          return 0x854A;
        case 0xC8:            /* 0x8EC8 */
          return 0x7E1E;
        case 0xC9:            /* 0x8EC9 */
          return 0x820E;
        case 0xCA:            /* 0x8ECA */
          return 0x5199;
        case 0xCB:            /* 0x8ECB */
          return 0x5C04;
        case 0xCC:            /* 0x8ECC */
          return 0x6368;
        case 0xCD:            /* 0x8ECD */
          return 0x8D66;
        case 0xCE:            /* 0x8ECE */
          return 0x659C;
        case 0xCF:            /* 0x8ECF */
          return 0x716E;
        case 0xD0:            /* 0x8ED0 */
          return 0x793E;
        case 0xD1:            /* 0x8ED1 */
          return 0x7D17;
        case 0xD2:            /* 0x8ED2 */
          return 0x8005;
        case 0xD3:            /* 0x8ED3 */
          return 0x8B1D;
        case 0xD4:            /* 0x8ED4 */
          return 0x8ECA;
        case 0xD5:            /* 0x8ED5 */
          return 0x906E;
        case 0xD6:            /* 0x8ED6 */
          return 0x86C7;
        case 0xD7:            /* 0x8ED7 */
          return 0x90AA;
        case 0xD8:            /* 0x8ED8 */
          return 0x501F;
        case 0xD9:            /* 0x8ED9 */
          return 0x52FA;
        case 0xDA:            /* 0x8EDA */
          return 0x5C3A;
        case 0xDB:            /* 0x8EDB */
          return 0x6753;
        case 0xDC:            /* 0x8EDC */
          return 0x707C;
        case 0xDD:            /* 0x8EDD */
          return 0x7235;
        case 0xDE:            /* 0x8EDE */
          return 0x914C;
        case 0xDF:            /* 0x8EDF */
          return 0x91C8;
        case 0xE0:            /* 0x8EE0 */
          return 0x932B;
        case 0xE1:            /* 0x8EE1 */
          return 0x82E5;
        case 0xE2:            /* 0x8EE2 */
          return 0x5BC2;
        case 0xE3:            /* 0x8EE3 */
          return 0x5F31;
        case 0xE4:            /* 0x8EE4 */
          return 0x60F9;
        case 0xE5:            /* 0x8EE5 */
          return 0x4E3B;
        case 0xE6:            /* 0x8EE6 */
          return 0x53D6;
        case 0xE7:            /* 0x8EE7 */
          return 0x5B88;
        case 0xE8:            /* 0x8EE8 */
          return 0x624B;
        case 0xE9:            /* 0x8EE9 */
          return 0x6731;
        case 0xEA:            /* 0x8EEA */
          return 0x6B8A;
        case 0xEB:            /* 0x8EEB */
          return 0x72E9;
        case 0xEC:            /* 0x8EEC */
          return 0x73E0;
        case 0xED:            /* 0x8EED */
          return 0x7A2E;
        case 0xEE:            /* 0x8EEE */
          return 0x816B;
        case 0xEF:            /* 0x8EEF */
          return 0x8DA3;
        case 0xF0:            /* 0x8EF0 */
          return 0x9152;
        case 0xF1:            /* 0x8EF1 */
          return 0x9996;
        case 0xF2:            /* 0x8EF2 */
          return 0x5112;
        case 0xF3:            /* 0x8EF3 */
          return 0x53D7;
        case 0xF4:            /* 0x8EF4 */
          return 0x546A;
        case 0xF5:            /* 0x8EF5 */
          return 0x5BFF;
        case 0xF6:            /* 0x8EF6 */
          return 0x6388;
        case 0xF7:            /* 0x8EF7 */
          return 0x6A39;
        case 0xF8:            /* 0x8EF8 */
          return 0x7DAC;
        case 0xF9:            /* 0x8EF9 */
          return 0x9700;
        case 0xFA:            /* 0x8EFA */
          return 0x56DA;
        case 0xFB:            /* 0x8EFB */
          return 0x53CE;
        case 0xFC:            /* 0x8EFC */
          return 0x5468;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 8E */

    /* Lead Byte: 0x8Fxx */
    case 0x8F:
      switch (low_byte)
      {
        case 0x40:            /* 0x8F40 */
          return 0x5B97;
        case 0x41:            /* 0x8F41 */
          return 0x5C31;
        case 0x42:            /* 0x8F42 */
          return 0x5DDE;
        case 0x43:            /* 0x8F43 */
          return 0x4FEE;
        case 0x44:            /* 0x8F44 */
          return 0x6101;
        case 0x45:            /* 0x8F45 */
          return 0x62FE;
        case 0x46:            /* 0x8F46 */
          return 0x6D32;
        case 0x47:            /* 0x8F47 */
          return 0x79C0;
        case 0x48:            /* 0x8F48 */
          return 0x79CB;
        case 0x49:            /* 0x8F49 */
          return 0x7D42;
        case 0x4A:            /* 0x8F4A */
          return 0x7E4D;
        case 0x4B:            /* 0x8F4B */
          return 0x7FD2;
        case 0x4C:            /* 0x8F4C */
          return 0x81ED;
        case 0x4D:            /* 0x8F4D */
          return 0x821F;
        case 0x4E:            /* 0x8F4E */
          return 0x8490;
        case 0x4F:            /* 0x8F4F */
          return 0x8846;
        case 0x50:            /* 0x8F50 */
          return 0x8972;
        case 0x51:            /* 0x8F51 */
          return 0x8B90;
        case 0x52:            /* 0x8F52 */
          return 0x8E74;
        case 0x53:            /* 0x8F53 */
          return 0x8F2F;
        case 0x54:            /* 0x8F54 */
          return 0x9031;
        case 0x55:            /* 0x8F55 */
          return 0x914B;
        case 0x56:            /* 0x8F56 */
          return 0x916C;
        case 0x57:            /* 0x8F57 */
          return 0x96C6;
        case 0x58:            /* 0x8F58 */
          return 0x919C;
        case 0x59:            /* 0x8F59 */
          return 0x4EC0;
        case 0x5A:            /* 0x8F5A */
          return 0x4F4F;
        case 0x5B:            /* 0x8F5B */
          return 0x5145;
        case 0x5C:            /* 0x8F5C */
          return 0x5341;
        case 0x5D:            /* 0x8F5D */
          return 0x5F93;
        case 0x5E:            /* 0x8F5E */
          return 0x620E;
        case 0x5F:            /* 0x8F5F */
          return 0x67D4;
        case 0x60:            /* 0x8F60 */
          return 0x6C41;
        case 0x61:            /* 0x8F61 */
          return 0x6E0B;
        case 0x62:            /* 0x8F62 */
          return 0x7363;
        case 0x63:            /* 0x8F63 */
          return 0x7E26;
        case 0x64:            /* 0x8F64 */
          return 0x91CD;
        case 0x65:            /* 0x8F65 */
          return 0x9283;
        case 0x66:            /* 0x8F66 */
          return 0x53D4;
        case 0x67:            /* 0x8F67 */
          return 0x5919;
        case 0x68:            /* 0x8F68 */
          return 0x5BBF;
        case 0x69:            /* 0x8F69 */
          return 0x6DD1;
        case 0x6A:            /* 0x8F6A */
          return 0x795D;
        case 0x6B:            /* 0x8F6B */
          return 0x7E2E;
        case 0x6C:            /* 0x8F6C */
          return 0x7C9B;
        case 0x6D:            /* 0x8F6D */
          return 0x587E;
        case 0x6E:            /* 0x8F6E */
          return 0x719F;
        case 0x6F:            /* 0x8F6F */
          return 0x51FA;
        case 0x70:            /* 0x8F70 */
          return 0x8853;
        case 0x71:            /* 0x8F71 */
          return 0x8FF0;
        case 0x72:            /* 0x8F72 */
          return 0x4FCA;
        case 0x73:            /* 0x8F73 */
          return 0x5CFB;
        case 0x74:            /* 0x8F74 */
          return 0x6625;
        case 0x75:            /* 0x8F75 */
          return 0x77AC;
        case 0x76:            /* 0x8F76 */
          return 0x7AE3;
        case 0x77:            /* 0x8F77 */
          return 0x821C;
        case 0x78:            /* 0x8F78 */
          return 0x99FF;
        case 0x79:            /* 0x8F79 */
          return 0x51C6;
        case 0x7A:            /* 0x8F7A */
          return 0x5FAA;
        case 0x7B:            /* 0x8F7B */
          return 0x65EC;
        case 0x7C:            /* 0x8F7C */
          return 0x696F;
        case 0x7D:            /* 0x8F7D */
          return 0x6B89;
        case 0x7E:            /* 0x8F7E */
          return 0x6DF3;
        case 0x80:            /* 0x8F80 */
          return 0x6E96;
        case 0x81:            /* 0x8F81 */
          return 0x6F64;
        case 0x82:            /* 0x8F82 */
          return 0x76FE;
        case 0x83:            /* 0x8F83 */
          return 0x7D14;
        case 0x84:            /* 0x8F84 */
          return 0x5DE1;
        case 0x85:            /* 0x8F85 */
          return 0x9075;
        case 0x86:            /* 0x8F86 */
          return 0x9187;
        case 0x87:            /* 0x8F87 */
          return 0x9806;
        case 0x88:            /* 0x8F88 */
          return 0x51E6;
        case 0x89:            /* 0x8F89 */
          return 0x521D;
        case 0x8A:            /* 0x8F8A */
          return 0x6240;
        case 0x8B:            /* 0x8F8B */
          return 0x6691;
        case 0x8C:            /* 0x8F8C */
          return 0x66D9;
        case 0x8D:            /* 0x8F8D */
          return 0x6E1A;
        case 0x8E:            /* 0x8F8E */
          return 0x5EB6;
        case 0x8F:            /* 0x8F8F */
          return 0x7DD2;
        case 0x90:            /* 0x8F90 */
          return 0x7F72;
        case 0x91:            /* 0x8F91 */
          return 0x66F8;
        case 0x92:            /* 0x8F92 */
          return 0x85AF;
        case 0x93:            /* 0x8F93 */
          return 0x85F7;
        case 0x94:            /* 0x8F94 */
          return 0x8AF8;
        case 0x95:            /* 0x8F95 */
          return 0x52A9;
        case 0x96:            /* 0x8F96 */
          return 0x53D9;
        case 0x97:            /* 0x8F97 */
          return 0x5973;
        case 0x98:            /* 0x8F98 */
          return 0x5E8F;
        case 0x99:            /* 0x8F99 */
          return 0x5F90;
        case 0x9A:            /* 0x8F9A */
          return 0x6055;
        case 0x9B:            /* 0x8F9B */
          return 0x92E4;
        case 0x9C:            /* 0x8F9C */
          return 0x9664;
        case 0x9D:            /* 0x8F9D */
          return 0x50B7;
        case 0x9E:            /* 0x8F9E */
          return 0x511F;
        case 0x9F:            /* 0x8F9F */
          return 0x52DD;
        case 0xA0:            /* 0x8FA0 */
          return 0x5320;
        case 0xA1:            /* 0x8FA1 */
          return 0x5347;
        case 0xA2:            /* 0x8FA2 */
          return 0x53EC;
        case 0xA3:            /* 0x8FA3 */
          return 0x54E8;
        case 0xA4:            /* 0x8FA4 */
          return 0x5546;
        case 0xA5:            /* 0x8FA5 */
          return 0x5531;
        case 0xA6:            /* 0x8FA6 */
          return 0x5617;
        case 0xA7:            /* 0x8FA7 */
          return 0x5968;
        case 0xA8:            /* 0x8FA8 */
          return 0x59BE;
        case 0xA9:            /* 0x8FA9 */
          return 0x5A3C;
        case 0xAA:            /* 0x8FAA */
          return 0x5BB5;
        case 0xAB:            /* 0x8FAB */
          return 0x5C06;
        case 0xAC:            /* 0x8FAC */
          return 0x5C0F;
        case 0xAD:            /* 0x8FAD */
          return 0x5C11;
        case 0xAE:            /* 0x8FAE */
          return 0x5C1A;
        case 0xAF:            /* 0x8FAF */
          return 0x5E84;
        case 0xB0:            /* 0x8FB0 */
          return 0x5E8A;
        case 0xB1:            /* 0x8FB1 */
          return 0x5EE0;
        case 0xB2:            /* 0x8FB2 */
          return 0x5F70;
        case 0xB3:            /* 0x8FB3 */
          return 0x627F;
        case 0xB4:            /* 0x8FB4 */
          return 0x6284;
        case 0xB5:            /* 0x8FB5 */
          return 0x62DB;
        case 0xB6:            /* 0x8FB6 */
          return 0x638C;
        case 0xB7:            /* 0x8FB7 */
          return 0x6377;
        case 0xB8:            /* 0x8FB8 */
          return 0x6607;
        case 0xB9:            /* 0x8FB9 */
          return 0x660C;
        case 0xBA:            /* 0x8FBA */
          return 0x662D;
        case 0xBB:            /* 0x8FBB */
          return 0x6676;
        case 0xBC:            /* 0x8FBC */
          return 0x677E;
        case 0xBD:            /* 0x8FBD */
          return 0x68A2;
        case 0xBE:            /* 0x8FBE */
          return 0x6A1F;
        case 0xBF:            /* 0x8FBF */
          return 0x6A35;
        case 0xC0:            /* 0x8FC0 */
          return 0x6CBC;
        case 0xC1:            /* 0x8FC1 */
          return 0x6D88;
        case 0xC2:            /* 0x8FC2 */
          return 0x6E09;
        case 0xC3:            /* 0x8FC3 */
          return 0x6E58;
        case 0xC4:            /* 0x8FC4 */
          return 0x713C;
        case 0xC5:            /* 0x8FC5 */
          return 0x7126;
        case 0xC6:            /* 0x8FC6 */
          return 0x7167;
        case 0xC7:            /* 0x8FC7 */
          return 0x75C7;
        case 0xC8:            /* 0x8FC8 */
          return 0x7701;
        case 0xC9:            /* 0x8FC9 */
          return 0x785D;
        case 0xCA:            /* 0x8FCA */
          return 0x7901;
        case 0xCB:            /* 0x8FCB */
          return 0x7965;
        case 0xCC:            /* 0x8FCC */
          return 0x79F0;
        case 0xCD:            /* 0x8FCD */
          return 0x7AE0;
        case 0xCE:            /* 0x8FCE */
          return 0x7B11;
        case 0xCF:            /* 0x8FCF */
          return 0x7CA7;
        case 0xD0:            /* 0x8FD0 */
          return 0x7D39;
        case 0xD1:            /* 0x8FD1 */
          return 0x8096;
        case 0xD2:            /* 0x8FD2 */
          return 0x83D6;
        case 0xD3:            /* 0x8FD3 */
          return 0x848B;
        case 0xD4:            /* 0x8FD4 */
          return 0x8549;
        case 0xD5:            /* 0x8FD5 */
          return 0x885D;
        case 0xD6:            /* 0x8FD6 */
          return 0x88F3;
        case 0xD7:            /* 0x8FD7 */
          return 0x8A1F;
        case 0xD8:            /* 0x8FD8 */
          return 0x8A3C;
        case 0xD9:            /* 0x8FD9 */
          return 0x8A54;
        case 0xDA:            /* 0x8FDA */
          return 0x8A73;
        case 0xDB:            /* 0x8FDB */
          return 0x8C61;
        case 0xDC:            /* 0x8FDC */
          return 0x8CDE;
        case 0xDD:            /* 0x8FDD */
          return 0x91A4;
        case 0xDE:            /* 0x8FDE */
          return 0x9266;
        case 0xDF:            /* 0x8FDF */
          return 0x937E;
        case 0xE0:            /* 0x8FE0 */
          return 0x9418;
        case 0xE1:            /* 0x8FE1 */
          return 0x969C;
        case 0xE2:            /* 0x8FE2 */
          return 0x9798;
        case 0xE3:            /* 0x8FE3 */
          return 0x4E0A;
        case 0xE4:            /* 0x8FE4 */
          return 0x4E08;
        case 0xE5:            /* 0x8FE5 */
          return 0x4E1E;
        case 0xE6:            /* 0x8FE6 */
          return 0x4E57;
        case 0xE7:            /* 0x8FE7 */
          return 0x5197;
        case 0xE8:            /* 0x8FE8 */
          return 0x5270;
        case 0xE9:            /* 0x8FE9 */
          return 0x57CE;
        case 0xEA:            /* 0x8FEA */
          return 0x5834;
        case 0xEB:            /* 0x8FEB */
          return 0x58CC;
        case 0xEC:            /* 0x8FEC */
          return 0x5B22;
        case 0xED:            /* 0x8FED */
          return 0x5E38;
        case 0xEE:            /* 0x8FEE */
          return 0x60C5;
        case 0xEF:            /* 0x8FEF */
          return 0x64FE;
        case 0xF0:            /* 0x8FF0 */
          return 0x6761;
        case 0xF1:            /* 0x8FF1 */
          return 0x6756;
        case 0xF2:            /* 0x8FF2 */
          return 0x6D44;
        case 0xF3:            /* 0x8FF3 */
          return 0x72B6;
        case 0xF4:            /* 0x8FF4 */
          return 0x7573;
        case 0xF5:            /* 0x8FF5 */
          return 0x7A63;
        case 0xF6:            /* 0x8FF6 */
          return 0x84B8;
        case 0xF7:            /* 0x8FF7 */
          return 0x8B72;
        case 0xF8:            /* 0x8FF8 */
          return 0x91B8;
        case 0xF9:            /* 0x8FF9 */
          return 0x9320;
        case 0xFA:            /* 0x8FFA */
          return 0x5631;
        case 0xFB:            /* 0x8FFB */
          return 0x57F4;
        case 0xFC:            /* 0x8FFC */
          return 0x98FE;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 8F */

    /* Lead Byte: 0x90xx */
    case 0x90:
      switch (low_byte)
      {
        case 0x40:            /* 0x9040 */
          return 0x62ED;
        case 0x41:            /* 0x9041 */
          return 0x690D;
        case 0x42:            /* 0x9042 */
          return 0x6B96;
        case 0x43:            /* 0x9043 */
          return 0x71ED;
        case 0x44:            /* 0x9044 */
          return 0x7E54;
        case 0x45:            /* 0x9045 */
          return 0x8077;
        case 0x46:            /* 0x9046 */
          return 0x8272;
        case 0x47:            /* 0x9047 */
          return 0x89E6;
        case 0x48:            /* 0x9048 */
          return 0x98DF;
        case 0x49:            /* 0x9049 */
          return 0x8755;
        case 0x4A:            /* 0x904A */
          return 0x8FB1;
        case 0x4B:            /* 0x904B */
          return 0x5C3B;
        case 0x4C:            /* 0x904C */
          return 0x4F38;
        case 0x4D:            /* 0x904D */
          return 0x4FE1;
        case 0x4E:            /* 0x904E */
          return 0x4FB5;
        case 0x4F:            /* 0x904F */
          return 0x5507;
        case 0x50:            /* 0x9050 */
          return 0x5A20;
        case 0x51:            /* 0x9051 */
          return 0x5BDD;
        case 0x52:            /* 0x9052 */
          return 0x5BE9;
        case 0x53:            /* 0x9053 */
          return 0x5FC3;
        case 0x54:            /* 0x9054 */
          return 0x614E;
        case 0x55:            /* 0x9055 */
          return 0x632F;
        case 0x56:            /* 0x9056 */
          return 0x65B0;
        case 0x57:            /* 0x9057 */
          return 0x664B;
        case 0x58:            /* 0x9058 */
          return 0x68EE;
        case 0x59:            /* 0x9059 */
          return 0x699B;
        case 0x5A:            /* 0x905A */
          return 0x6D78;
        case 0x5B:            /* 0x905B */
          return 0x6DF1;
        case 0x5C:            /* 0x905C */
          return 0x7533;
        case 0x5D:            /* 0x905D */
          return 0x75B9;
        case 0x5E:            /* 0x905E */
          return 0x771F;
        case 0x5F:            /* 0x905F */
          return 0x795E;
        case 0x60:            /* 0x9060 */
          return 0x79E6;
        case 0x61:            /* 0x9061 */
          return 0x7D33;
        case 0x62:            /* 0x9062 */
          return 0x81E3;
        case 0x63:            /* 0x9063 */
          return 0x82AF;
        case 0x64:            /* 0x9064 */
          return 0x85AA;
        case 0x65:            /* 0x9065 */
          return 0x89AA;
        case 0x66:            /* 0x9066 */
          return 0x8A3A;
        case 0x67:            /* 0x9067 */
          return 0x8EAB;
        case 0x68:            /* 0x9068 */
          return 0x8F9B;
        case 0x69:            /* 0x9069 */
          return 0x9032;
        case 0x6A:            /* 0x906A */
          return 0x91DD;
        case 0x6B:            /* 0x906B */
          return 0x9707;
        case 0x6C:            /* 0x906C */
          return 0x4EBA;
        case 0x6D:            /* 0x906D */
          return 0x4EC1;
        case 0x6E:            /* 0x906E */
          return 0x5203;
        case 0x6F:            /* 0x906F */
          return 0x5875;
        case 0x70:            /* 0x9070 */
          return 0x58EC;
        case 0x71:            /* 0x9071 */
          return 0x5C0B;
        case 0x72:            /* 0x9072 */
          return 0x751A;
        case 0x73:            /* 0x9073 */
          return 0x5C3D;
        case 0x74:            /* 0x9074 */
          return 0x814E;
        case 0x75:            /* 0x9075 */
          return 0x8A0A;
        case 0x76:            /* 0x9076 */
          return 0x8FC5;
        case 0x77:            /* 0x9077 */
          return 0x9663;
        case 0x78:            /* 0x9078 */
          return 0x976D;
        case 0x79:            /* 0x9079 */
          return 0x7B25;
        case 0x7A:            /* 0x907A */
          return 0x8ACF;
        case 0x7B:            /* 0x907B */
          return 0x9808;
        case 0x7C:            /* 0x907C */
          return 0x9162;
        case 0x7D:            /* 0x907D */
          return 0x56F3;
        case 0x7E:            /* 0x907E */
          return 0x53A8;
        case 0x80:            /* 0x9080 */
          return 0x9017;
        case 0x81:            /* 0x9081 */
          return 0x5439;
        case 0x82:            /* 0x9082 */
          return 0x5782;
        case 0x83:            /* 0x9083 */
          return 0x5E25;
        case 0x84:            /* 0x9084 */
          return 0x63A8;
        case 0x85:            /* 0x9085 */
          return 0x6C34;
        case 0x86:            /* 0x9086 */
          return 0x708A;
        case 0x87:            /* 0x9087 */
          return 0x7761;
        case 0x88:            /* 0x9088 */
          return 0x7C8B;
        case 0x89:            /* 0x9089 */
          return 0x7FE0;
        case 0x8A:            /* 0x908A */
          return 0x8870;
        case 0x8B:            /* 0x908B */
          return 0x9042;
        case 0x8C:            /* 0x908C */
          return 0x9154;
        case 0x8D:            /* 0x908D */
          return 0x9310;
        case 0x8E:            /* 0x908E */
          return 0x9318;
        case 0x8F:            /* 0x908F */
          return 0x968F;
        case 0x90:            /* 0x9090 */
          return 0x745E;
        case 0x91:            /* 0x9091 */
          return 0x9AC4;
        case 0x92:            /* 0x9092 */
          return 0x5D07;
        case 0x93:            /* 0x9093 */
          return 0x5D69;
        case 0x94:            /* 0x9094 */
          return 0x6570;
        case 0x95:            /* 0x9095 */
          return 0x67A2;
        case 0x96:            /* 0x9096 */
          return 0x8DA8;
        case 0x97:            /* 0x9097 */
          return 0x96DB;
        case 0x98:            /* 0x9098 */
          return 0x636E;
        case 0x99:            /* 0x9099 */
          return 0x6749;
        case 0x9A:            /* 0x909A */
          return 0x6919;
        case 0x9B:            /* 0x909B */
          return 0x83C5;
        case 0x9C:            /* 0x909C */
          return 0x9817;
        case 0x9D:            /* 0x909D */
          return 0x96C0;
        case 0x9E:            /* 0x909E */
          return 0x88FE;
        case 0x9F:            /* 0x909F */
          return 0x6F84;
        case 0xA0:            /* 0x90A0 */
          return 0x647A;
        case 0xA1:            /* 0x90A1 */
          return 0x5BF8;
        case 0xA2:            /* 0x90A2 */
          return 0x4E16;
        case 0xA3:            /* 0x90A3 */
          return 0x702C;
        case 0xA4:            /* 0x90A4 */
          return 0x755D;
        case 0xA5:            /* 0x90A5 */
          return 0x662F;
        case 0xA6:            /* 0x90A6 */
          return 0x51C4;
        case 0xA7:            /* 0x90A7 */
          return 0x5236;
        case 0xA8:            /* 0x90A8 */
          return 0x52E2;
        case 0xA9:            /* 0x90A9 */
          return 0x59D3;
        case 0xAA:            /* 0x90AA */
          return 0x5F81;
        case 0xAB:            /* 0x90AB */
          return 0x6027;
        case 0xAC:            /* 0x90AC */
          return 0x6210;
        case 0xAD:            /* 0x90AD */
          return 0x653F;
        case 0xAE:            /* 0x90AE */
          return 0x6574;
        case 0xAF:            /* 0x90AF */
          return 0x661F;
        case 0xB0:            /* 0x90B0 */
          return 0x6674;
        case 0xB1:            /* 0x90B1 */
          return 0x68F2;
        case 0xB2:            /* 0x90B2 */
          return 0x6816;
        case 0xB3:            /* 0x90B3 */
          return 0x6B63;
        case 0xB4:            /* 0x90B4 */
          return 0x6E05;
        case 0xB5:            /* 0x90B5 */
          return 0x7272;
        case 0xB6:            /* 0x90B6 */
          return 0x751F;
        case 0xB7:            /* 0x90B7 */
          return 0x76DB;
        case 0xB8:            /* 0x90B8 */
          return 0x7CBE;
        case 0xB9:            /* 0x90B9 */
          return 0x8056;
        case 0xBA:            /* 0x90BA */
          return 0x58F0;
        case 0xBB:            /* 0x90BB */
          return 0x88FD;
        case 0xBC:            /* 0x90BC */
          return 0x897F;
        case 0xBD:            /* 0x90BD */
          return 0x8AA0;
        case 0xBE:            /* 0x90BE */
          return 0x8A93;
        case 0xBF:            /* 0x90BF */
          return 0x8ACB;
        case 0xC0:            /* 0x90C0 */
          return 0x901D;
        case 0xC1:            /* 0x90C1 */
          return 0x9192;
        case 0xC2:            /* 0x90C2 */
          return 0x9752;
        case 0xC3:            /* 0x90C3 */
          return 0x9759;
        case 0xC4:            /* 0x90C4 */
          return 0x6589;
        case 0xC5:            /* 0x90C5 */
          return 0x7A0E;
        case 0xC6:            /* 0x90C6 */
          return 0x8106;
        case 0xC7:            /* 0x90C7 */
          return 0x96BB;
        case 0xC8:            /* 0x90C8 */
          return 0x5E2D;
        case 0xC9:            /* 0x90C9 */
          return 0x60DC;
        case 0xCA:            /* 0x90CA */
          return 0x621A;
        case 0xCB:            /* 0x90CB */
          return 0x65A5;
        case 0xCC:            /* 0x90CC */
          return 0x6614;
        case 0xCD:            /* 0x90CD */
          return 0x6790;
        case 0xCE:            /* 0x90CE */
          return 0x77F3;
        case 0xCF:            /* 0x90CF */
          return 0x7A4D;
        case 0xD0:            /* 0x90D0 */
          return 0x7C4D;
        case 0xD1:            /* 0x90D1 */
          return 0x7E3E;
        case 0xD2:            /* 0x90D2 */
          return 0x810A;
        case 0xD3:            /* 0x90D3 */
          return 0x8CAC;
        case 0xD4:            /* 0x90D4 */
          return 0x8D64;
        case 0xD5:            /* 0x90D5 */
          return 0x8DE1;
        case 0xD6:            /* 0x90D6 */
          return 0x8E5F;
        case 0xD7:            /* 0x90D7 */
          return 0x78A9;
        case 0xD8:            /* 0x90D8 */
          return 0x5207;
        case 0xD9:            /* 0x90D9 */
          return 0x62D9;
        case 0xDA:            /* 0x90DA */
          return 0x63A5;
        case 0xDB:            /* 0x90DB */
          return 0x6442;
        case 0xDC:            /* 0x90DC */
          return 0x6298;
        case 0xDD:            /* 0x90DD */
          return 0x8A2D;
        case 0xDE:            /* 0x90DE */
          return 0x7A83;
        case 0xDF:            /* 0x90DF */
          return 0x7BC0;
        case 0xE0:            /* 0x90E0 */
          return 0x8AAC;
        case 0xE1:            /* 0x90E1 */
          return 0x96EA;
        case 0xE2:            /* 0x90E2 */
          return 0x7D76;
        case 0xE3:            /* 0x90E3 */
          return 0x820C;
        case 0xE4:            /* 0x90E4 */
          return 0x8749;
        case 0xE5:            /* 0x90E5 */
          return 0x4ED9;
        case 0xE6:            /* 0x90E6 */
          return 0x5148;
        case 0xE7:            /* 0x90E7 */
          return 0x5343;
        case 0xE8:            /* 0x90E8 */
          return 0x5360;
        case 0xE9:            /* 0x90E9 */
          return 0x5BA3;
        case 0xEA:            /* 0x90EA */
          return 0x5C02;
        case 0xEB:            /* 0x90EB */
          return 0x5C16;
        case 0xEC:            /* 0x90EC */
          return 0x5DDD;
        case 0xED:            /* 0x90ED */
          return 0x6226;
        case 0xEE:            /* 0x90EE */
          return 0x6247;
        case 0xEF:            /* 0x90EF */
          return 0x64B0;
        case 0xF0:            /* 0x90F0 */
          return 0x6813;
        case 0xF1:            /* 0x90F1 */
          return 0x6834;
        case 0xF2:            /* 0x90F2 */
          return 0x6CC9;
        case 0xF3:            /* 0x90F3 */
          return 0x6D45;
        case 0xF4:            /* 0x90F4 */
          return 0x6D17;
        case 0xF5:            /* 0x90F5 */
          return 0x67D3;
        case 0xF6:            /* 0x90F6 */
          return 0x6F5C;
        case 0xF7:            /* 0x90F7 */
          return 0x714E;
        case 0xF8:            /* 0x90F8 */
          return 0x717D;
        case 0xF9:            /* 0x90F9 */
          return 0x65CB;
        case 0xFA:            /* 0x90FA */
          return 0x7A7F;
        case 0xFB:            /* 0x90FB */
          return 0x7BAD;
        case 0xFC:            /* 0x90FC */
          return 0x7DDA;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 90 */

    /* Lead Byte: 0x91xx */
    case 0x91:
      switch (low_byte)
      {
        case 0x40:            /* 0x9140 */
          return 0x7E4A;
        case 0x41:            /* 0x9141 */
          return 0x7FA8;
        case 0x42:            /* 0x9142 */
          return 0x817A;
        case 0x43:            /* 0x9143 */
          return 0x821B;
        case 0x44:            /* 0x9144 */
          return 0x8239;
        case 0x45:            /* 0x9145 */
          return 0x85A6;
        case 0x46:            /* 0x9146 */
          return 0x8A6E;
        case 0x47:            /* 0x9147 */
          return 0x8CCE;
        case 0x48:            /* 0x9148 */
          return 0x8DF5;
        case 0x49:            /* 0x9149 */
          return 0x9078;
        case 0x4A:            /* 0x914A */
          return 0x9077;
        case 0x4B:            /* 0x914B */
          return 0x92AD;
        case 0x4C:            /* 0x914C */
          return 0x9291;
        case 0x4D:            /* 0x914D */
          return 0x9583;
        case 0x4E:            /* 0x914E */
          return 0x9BAE;
        case 0x4F:            /* 0x914F */
          return 0x524D;
        case 0x50:            /* 0x9150 */
          return 0x5584;
        case 0x51:            /* 0x9151 */
          return 0x6F38;
        case 0x52:            /* 0x9152 */
          return 0x7136;
        case 0x53:            /* 0x9153 */
          return 0x5168;
        case 0x54:            /* 0x9154 */
          return 0x7985;
        case 0x55:            /* 0x9155 */
          return 0x7E55;
        case 0x56:            /* 0x9156 */
          return 0x81B3;
        case 0x57:            /* 0x9157 */
          return 0x7CCE;
        case 0x58:            /* 0x9158 */
          return 0x564C;
        case 0x59:            /* 0x9159 */
          return 0x5851;
        case 0x5A:            /* 0x915A */
          return 0x5CA8;
        case 0x5B:            /* 0x915B */
          return 0x63AA;
        case 0x5C:            /* 0x915C */
          return 0x66FE;
        case 0x5D:            /* 0x915D */
          return 0x66FD;
        case 0x5E:            /* 0x915E */
          return 0x695A;
        case 0x5F:            /* 0x915F */
          return 0x72D9;
        case 0x60:            /* 0x9160 */
          return 0x758F;
        case 0x61:            /* 0x9161 */
          return 0x758E;
        case 0x62:            /* 0x9162 */
          return 0x790E;
        case 0x63:            /* 0x9163 */
          return 0x7956;
        case 0x64:            /* 0x9164 */
          return 0x79DF;
        case 0x65:            /* 0x9165 */
          return 0x7C97;
        case 0x66:            /* 0x9166 */
          return 0x7D20;
        case 0x67:            /* 0x9167 */
          return 0x7D44;
        case 0x68:            /* 0x9168 */
          return 0x8607;
        case 0x69:            /* 0x9169 */
          return 0x8A34;
        case 0x6A:            /* 0x916A */
          return 0x963B;
        case 0x6B:            /* 0x916B */
          return 0x9061;
        case 0x6C:            /* 0x916C */
          return 0x9F20;
        case 0x6D:            /* 0x916D */
          return 0x50E7;
        case 0x6E:            /* 0x916E */
          return 0x5275;
        case 0x6F:            /* 0x916F */
          return 0x53CC;
        case 0x70:            /* 0x9170 */
          return 0x53E2;
        case 0x71:            /* 0x9171 */
          return 0x5009;
        case 0x72:            /* 0x9172 */
          return 0x55AA;
        case 0x73:            /* 0x9173 */
          return 0x58EE;
        case 0x74:            /* 0x9174 */
          return 0x594F;
        case 0x75:            /* 0x9175 */
          return 0x723D;
        case 0x76:            /* 0x9176 */
          return 0x5B8B;
        case 0x77:            /* 0x9177 */
          return 0x5C64;
        case 0x78:            /* 0x9178 */
          return 0x531D;
        case 0x79:            /* 0x9179 */
          return 0x60E3;
        case 0x7A:            /* 0x917A */
          return 0x60F3;
        case 0x7B:            /* 0x917B */
          return 0x635C;
        case 0x7C:            /* 0x917C */
          return 0x6383;
        case 0x7D:            /* 0x917D */
          return 0x633F;
        case 0x7E:            /* 0x917E */
          return 0x63BB;
        case 0x80:            /* 0x9180 */
          return 0x64CD;
        case 0x81:            /* 0x9181 */
          return 0x65E9;
        case 0x82:            /* 0x9182 */
          return 0x66F9;
        case 0x83:            /* 0x9183 */
          return 0x5DE3;
        case 0x84:            /* 0x9184 */
          return 0x69CD;
        case 0x85:            /* 0x9185 */
          return 0x69FD;
        case 0x86:            /* 0x9186 */
          return 0x6F15;
        case 0x87:            /* 0x9187 */
          return 0x71E5;
        case 0x88:            /* 0x9188 */
          return 0x4E89;
        case 0x89:            /* 0x9189 */
          return 0x75E9;
        case 0x8A:            /* 0x918A */
          return 0x76F8;
        case 0x8B:            /* 0x918B */
          return 0x7A93;
        case 0x8C:            /* 0x918C */
          return 0x7CDF;
        case 0x8D:            /* 0x918D */
          return 0x7DCF;
        case 0x8E:            /* 0x918E */
          return 0x7D9C;
        case 0x8F:            /* 0x918F */
          return 0x8061;
        case 0x90:            /* 0x9190 */
          return 0x8349;
        case 0x91:            /* 0x9191 */
          return 0x8358;
        case 0x92:            /* 0x9192 */
          return 0x846C;
        case 0x93:            /* 0x9193 */
          return 0x84BC;
        case 0x94:            /* 0x9194 */
          return 0x85FB;
        case 0x95:            /* 0x9195 */
          return 0x88C5;
        case 0x96:            /* 0x9196 */
          return 0x8D70;
        case 0x97:            /* 0x9197 */
          return 0x9001;
        case 0x98:            /* 0x9198 */
          return 0x906D;
        case 0x99:            /* 0x9199 */
          return 0x9397;
        case 0x9A:            /* 0x919A */
          return 0x971C;
        case 0x9B:            /* 0x919B */
          return 0x9A12;
        case 0x9C:            /* 0x919C */
          return 0x50CF;
        case 0x9D:            /* 0x919D */
          return 0x5897;
        case 0x9E:            /* 0x919E */
          return 0x618E;
        case 0x9F:            /* 0x919F */
          return 0x81D3;
        case 0xA0:            /* 0x91A0 */
          return 0x8535;
        case 0xA1:            /* 0x91A1 */
          return 0x8D08;
        case 0xA2:            /* 0x91A2 */
          return 0x9020;
        case 0xA3:            /* 0x91A3 */
          return 0x4FC3;
        case 0xA4:            /* 0x91A4 */
          return 0x5074;
        case 0xA5:            /* 0x91A5 */
          return 0x5247;
        case 0xA6:            /* 0x91A6 */
          return 0x5373;
        case 0xA7:            /* 0x91A7 */
          return 0x606F;
        case 0xA8:            /* 0x91A8 */
          return 0x6349;
        case 0xA9:            /* 0x91A9 */
          return 0x675F;
        case 0xAA:            /* 0x91AA */
          return 0x6E2C;
        case 0xAB:            /* 0x91AB */
          return 0x8DB3;
        case 0xAC:            /* 0x91AC */
          return 0x901F;
        case 0xAD:            /* 0x91AD */
          return 0x4FD7;
        case 0xAE:            /* 0x91AE */
          return 0x5C5E;
        case 0xAF:            /* 0x91AF */
          return 0x8CCA;
        case 0xB0:            /* 0x91B0 */
          return 0x65CF;
        case 0xB1:            /* 0x91B1 */
          return 0x7D9A;
        case 0xB2:            /* 0x91B2 */
          return 0x5352;
        case 0xB3:            /* 0x91B3 */
          return 0x8896;
        case 0xB4:            /* 0x91B4 */
          return 0x5176;
        case 0xB5:            /* 0x91B5 */
          return 0x63C3;
        case 0xB6:            /* 0x91B6 */
          return 0x5B58;
        case 0xB7:            /* 0x91B7 */
          return 0x5B6B;
        case 0xB8:            /* 0x91B8 */
          return 0x5C0A;
        case 0xB9:            /* 0x91B9 */
          return 0x640D;
        case 0xBA:            /* 0x91BA */
          return 0x6751;
        case 0xBB:            /* 0x91BB */
          return 0x905C;
        case 0xBC:            /* 0x91BC */
          return 0x4ED6;
        case 0xBD:            /* 0x91BD */
          return 0x591A;
        case 0xBE:            /* 0x91BE */
          return 0x592A;
        case 0xBF:            /* 0x91BF */
          return 0x6C70;
        case 0xC0:            /* 0x91C0 */
          return 0x8A51;
        case 0xC1:            /* 0x91C1 */
          return 0x553E;
        case 0xC2:            /* 0x91C2 */
          return 0x5815;
        case 0xC3:            /* 0x91C3 */
          return 0x59A5;
        case 0xC4:            /* 0x91C4 */
          return 0x60F0;
        case 0xC5:            /* 0x91C5 */
          return 0x6253;
        case 0xC6:            /* 0x91C6 */
          return 0x67C1;
        case 0xC7:            /* 0x91C7 */
          return 0x8235;
        case 0xC8:            /* 0x91C8 */
          return 0x6955;
        case 0xC9:            /* 0x91C9 */
          return 0x9640;
        case 0xCA:            /* 0x91CA */
          return 0x99C4;
        case 0xCB:            /* 0x91CB */
          return 0x9A28;
        case 0xCC:            /* 0x91CC */
          return 0x4F53;
        case 0xCD:            /* 0x91CD */
          return 0x5806;
        case 0xCE:            /* 0x91CE */
          return 0x5BFE;
        case 0xCF:            /* 0x91CF */
          return 0x8010;
        case 0xD0:            /* 0x91D0 */
          return 0x5CB1;
        case 0xD1:            /* 0x91D1 */
          return 0x5E2F;
        case 0xD2:            /* 0x91D2 */
          return 0x5F85;
        case 0xD3:            /* 0x91D3 */
          return 0x6020;
        case 0xD4:            /* 0x91D4 */
          return 0x614B;
        case 0xD5:            /* 0x91D5 */
          return 0x6234;
        case 0xD6:            /* 0x91D6 */
          return 0x66FF;
        case 0xD7:            /* 0x91D7 */
          return 0x6CF0;
        case 0xD8:            /* 0x91D8 */
          return 0x6EDE;
        case 0xD9:            /* 0x91D9 */
          return 0x80CE;
        case 0xDA:            /* 0x91DA */
          return 0x817F;
        case 0xDB:            /* 0x91DB */
          return 0x82D4;
        case 0xDC:            /* 0x91DC */
          return 0x888B;
        case 0xDD:            /* 0x91DD */
          return 0x8CB8;
        case 0xDE:            /* 0x91DE */
          return 0x9000;
        case 0xDF:            /* 0x91DF */
          return 0x902E;
        case 0xE0:            /* 0x91E0 */
          return 0x968A;
        case 0xE1:            /* 0x91E1 */
          return 0x9EDB;
        case 0xE2:            /* 0x91E2 */
          return 0x9BDB;
        case 0xE3:            /* 0x91E3 */
          return 0x4EE3;
        case 0xE4:            /* 0x91E4 */
          return 0x53F0;
        case 0xE5:            /* 0x91E5 */
          return 0x5927;
        case 0xE6:            /* 0x91E6 */
          return 0x7B2C;
        case 0xE7:            /* 0x91E7 */
          return 0x918D;
        case 0xE8:            /* 0x91E8 */
          return 0x984C;
        case 0xE9:            /* 0x91E9 */
          return 0x9DF9;
        case 0xEA:            /* 0x91EA */
          return 0x6EDD;
        case 0xEB:            /* 0x91EB */
          return 0x7027;
        case 0xEC:            /* 0x91EC */
          return 0x5353;
        case 0xED:            /* 0x91ED */
          return 0x5544;
        case 0xEE:            /* 0x91EE */
          return 0x5B85;
        case 0xEF:            /* 0x91EF */
          return 0x6258;
        case 0xF0:            /* 0x91F0 */
          return 0x629E;
        case 0xF1:            /* 0x91F1 */
          return 0x62D3;
        case 0xF2:            /* 0x91F2 */
          return 0x6CA2;
        case 0xF3:            /* 0x91F3 */
          return 0x6FEF;
        case 0xF4:            /* 0x91F4 */
          return 0x7422;
        case 0xF5:            /* 0x91F5 */
          return 0x8A17;
        case 0xF6:            /* 0x91F6 */
          return 0x9438;
        case 0xF7:            /* 0x91F7 */
          return 0x6FC1;
        case 0xF8:            /* 0x91F8 */
          return 0x8AFE;
        case 0xF9:            /* 0x91F9 */
          return 0x8338;
        case 0xFA:            /* 0x91FA */
          return 0x51E7;
        case 0xFB:            /* 0x91FB */
          return 0x86F8;
        case 0xFC:            /* 0x91FC */
          return 0x53EA;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 91 */

    /* Lead Byte: 0x92xx */
    case 0x92:
      switch (low_byte)
      {
        case 0x40:            /* 0x9240 */
          return 0x53E9;
        case 0x41:            /* 0x9241 */
          return 0x4F46;
        case 0x42:            /* 0x9242 */
          return 0x9054;
        case 0x43:            /* 0x9243 */
          return 0x8FB0;
        case 0x44:            /* 0x9244 */
          return 0x596A;
        case 0x45:            /* 0x9245 */
          return 0x8131;
        case 0x46:            /* 0x9246 */
          return 0x5DFD;
        case 0x47:            /* 0x9247 */
          return 0x7AEA;
        case 0x48:            /* 0x9248 */
          return 0x8FBF;
        case 0x49:            /* 0x9249 */
          return 0x68DA;
        case 0x4A:            /* 0x924A */
          return 0x8C37;
        case 0x4B:            /* 0x924B */
          return 0x72F8;
        case 0x4C:            /* 0x924C */
          return 0x9C48;
        case 0x4D:            /* 0x924D */
          return 0x6A3D;
        case 0x4E:            /* 0x924E */
          return 0x8AB0;
        case 0x4F:            /* 0x924F */
          return 0x4E39;
        case 0x50:            /* 0x9250 */
          return 0x5358;
        case 0x51:            /* 0x9251 */
          return 0x5606;
        case 0x52:            /* 0x9252 */
          return 0x5766;
        case 0x53:            /* 0x9253 */
          return 0x62C5;
        case 0x54:            /* 0x9254 */
          return 0x63A2;
        case 0x55:            /* 0x9255 */
          return 0x65E6;
        case 0x56:            /* 0x9256 */
          return 0x6B4E;
        case 0x57:            /* 0x9257 */
          return 0x6DE1;
        case 0x58:            /* 0x9258 */
          return 0x6E5B;
        case 0x59:            /* 0x9259 */
          return 0x70AD;
        case 0x5A:            /* 0x925A */
          return 0x77ED;
        case 0x5B:            /* 0x925B */
          return 0x7AEF;
        case 0x5C:            /* 0x925C */
          return 0x7BAA;
        case 0x5D:            /* 0x925D */
          return 0x7DBB;
        case 0x5E:            /* 0x925E */
          return 0x803D;
        case 0x5F:            /* 0x925F */
          return 0x80C6;
        case 0x60:            /* 0x9260 */
          return 0x86CB;
        case 0x61:            /* 0x9261 */
          return 0x8A95;
        case 0x62:            /* 0x9262 */
          return 0x935B;
        case 0x63:            /* 0x9263 */
          return 0x56E3;
        case 0x64:            /* 0x9264 */
          return 0x58C7;
        case 0x65:            /* 0x9265 */
          return 0x5F3E;
        case 0x66:            /* 0x9266 */
          return 0x65AD;
        case 0x67:            /* 0x9267 */
          return 0x6696;
        case 0x68:            /* 0x9268 */
          return 0x6A80;
        case 0x69:            /* 0x9269 */
          return 0x6BB5;
        case 0x6A:            /* 0x926A */
          return 0x7537;
        case 0x6B:            /* 0x926B */
          return 0x8AC7;
        case 0x6C:            /* 0x926C */
          return 0x5024;
        case 0x6D:            /* 0x926D */
          return 0x77E5;
        case 0x6E:            /* 0x926E */
          return 0x5730;
        case 0x6F:            /* 0x926F */
          return 0x5F1B;
        case 0x70:            /* 0x9270 */
          return 0x6065;
        case 0x71:            /* 0x9271 */
          return 0x667A;
        case 0x72:            /* 0x9272 */
          return 0x6C60;
        case 0x73:            /* 0x9273 */
          return 0x75F4;
        case 0x74:            /* 0x9274 */
          return 0x7A1A;
        case 0x75:            /* 0x9275 */
          return 0x7F6E;
        case 0x76:            /* 0x9276 */
          return 0x81F4;
        case 0x77:            /* 0x9277 */
          return 0x8718;
        case 0x78:            /* 0x9278 */
          return 0x9045;
        case 0x79:            /* 0x9279 */
          return 0x99B3;
        case 0x7A:            /* 0x927A */
          return 0x7BC9;
        case 0x7B:            /* 0x927B */
          return 0x755C;
        case 0x7C:            /* 0x927C */
          return 0x7AF9;
        case 0x7D:            /* 0x927D */
          return 0x7B51;
        case 0x7E:            /* 0x927E */
          return 0x84C4;
        case 0x80:            /* 0x9280 */
          return 0x9010;
        case 0x81:            /* 0x9281 */
          return 0x79E9;
        case 0x82:            /* 0x9282 */
          return 0x7A92;
        case 0x83:            /* 0x9283 */
          return 0x8336;
        case 0x84:            /* 0x9284 */
          return 0x5AE1;
        case 0x85:            /* 0x9285 */
          return 0x7740;
        case 0x86:            /* 0x9286 */
          return 0x4E2D;
        case 0x87:            /* 0x9287 */
          return 0x4EF2;
        case 0x88:            /* 0x9288 */
          return 0x5B99;
        case 0x89:            /* 0x9289 */
          return 0x5FE0;
        case 0x8A:            /* 0x928A */
          return 0x62BD;
        case 0x8B:            /* 0x928B */
          return 0x663C;
        case 0x8C:            /* 0x928C */
          return 0x67F1;
        case 0x8D:            /* 0x928D */
          return 0x6CE8;
        case 0x8E:            /* 0x928E */
          return 0x866B;
        case 0x8F:            /* 0x928F */
          return 0x8877;
        case 0x90:            /* 0x9290 */
          return 0x8A3B;
        case 0x91:            /* 0x9291 */
          return 0x914E;
        case 0x92:            /* 0x9292 */
          return 0x92F3;
        case 0x93:            /* 0x9293 */
          return 0x99D0;
        case 0x94:            /* 0x9294 */
          return 0x6A17;
        case 0x95:            /* 0x9295 */
          return 0x7026;
        case 0x96:            /* 0x9296 */
          return 0x732A;
        case 0x97:            /* 0x9297 */
          return 0x82E7;
        case 0x98:            /* 0x9298 */
          return 0x8457;
        case 0x99:            /* 0x9299 */
          return 0x8CAF;
        case 0x9A:            /* 0x929A */
          return 0x4E01;
        case 0x9B:            /* 0x929B */
          return 0x5146;
        case 0x9C:            /* 0x929C */
          return 0x51CB;
        case 0x9D:            /* 0x929D */
          return 0x558B;
        case 0x9E:            /* 0x929E */
          return 0x5BF5;
        case 0x9F:            /* 0x929F */
          return 0x5E16;
        case 0xA0:            /* 0x92A0 */
          return 0x5E33;
        case 0xA1:            /* 0x92A1 */
          return 0x5E81;
        case 0xA2:            /* 0x92A2 */
          return 0x5F14;
        case 0xA3:            /* 0x92A3 */
          return 0x5F35;
        case 0xA4:            /* 0x92A4 */
          return 0x5F6B;
        case 0xA5:            /* 0x92A5 */
          return 0x5FB4;
        case 0xA6:            /* 0x92A6 */
          return 0x61F2;
        case 0xA7:            /* 0x92A7 */
          return 0x6311;
        case 0xA8:            /* 0x92A8 */
          return 0x66A2;
        case 0xA9:            /* 0x92A9 */
          return 0x671D;
        case 0xAA:            /* 0x92AA */
          return 0x6F6E;
        case 0xAB:            /* 0x92AB */
          return 0x7252;
        case 0xAC:            /* 0x92AC */
          return 0x753A;
        case 0xAD:            /* 0x92AD */
          return 0x773A;
        case 0xAE:            /* 0x92AE */
          return 0x8074;
        case 0xAF:            /* 0x92AF */
          return 0x8139;
        case 0xB0:            /* 0x92B0 */
          return 0x8178;
        case 0xB1:            /* 0x92B1 */
          return 0x8776;
        case 0xB2:            /* 0x92B2 */
          return 0x8ABF;
        case 0xB3:            /* 0x92B3 */
          return 0x8ADC;
        case 0xB4:            /* 0x92B4 */
          return 0x8D85;
        case 0xB5:            /* 0x92B5 */
          return 0x8DF3;
        case 0xB6:            /* 0x92B6 */
          return 0x929A;
        case 0xB7:            /* 0x92B7 */
          return 0x9577;
        case 0xB8:            /* 0x92B8 */
          return 0x9802;
        case 0xB9:            /* 0x92B9 */
          return 0x9CE5;
        case 0xBA:            /* 0x92BA */
          return 0x52C5;
        case 0xBB:            /* 0x92BB */
          return 0x6357;
        case 0xBC:            /* 0x92BC */
          return 0x76F4;
        case 0xBD:            /* 0x92BD */
          return 0x6715;
        case 0xBE:            /* 0x92BE */
          return 0x6C88;
        case 0xBF:            /* 0x92BF */
          return 0x73CD;
        case 0xC0:            /* 0x92C0 */
          return 0x8CC3;
        case 0xC1:            /* 0x92C1 */
          return 0x93AE;
        case 0xC2:            /* 0x92C2 */
          return 0x9673;
        case 0xC3:            /* 0x92C3 */
          return 0x6D25;
        case 0xC4:            /* 0x92C4 */
          return 0x589C;
        case 0xC5:            /* 0x92C5 */
          return 0x690E;
        case 0xC6:            /* 0x92C6 */
          return 0x69CC;
        case 0xC7:            /* 0x92C7 */
          return 0x8FFD;
        case 0xC8:            /* 0x92C8 */
          return 0x939A;
        case 0xC9:            /* 0x92C9 */
          return 0x75DB;
        case 0xCA:            /* 0x92CA */
          return 0x901A;
        case 0xCB:            /* 0x92CB */
          return 0x585A;
        case 0xCC:            /* 0x92CC */
          return 0x6802;
        case 0xCD:            /* 0x92CD */
          return 0x63B4;
        case 0xCE:            /* 0x92CE */
          return 0x69FB;
        case 0xCF:            /* 0x92CF */
          return 0x4F43;
        case 0xD0:            /* 0x92D0 */
          return 0x6F2C;
        case 0xD1:            /* 0x92D1 */
          return 0x67D8;
        case 0xD2:            /* 0x92D2 */
          return 0x8FBB;
        case 0xD3:            /* 0x92D3 */
          return 0x8526;
        case 0xD4:            /* 0x92D4 */
          return 0x7DB4;
        case 0xD5:            /* 0x92D5 */
          return 0x9354;
        case 0xD6:            /* 0x92D6 */
          return 0x693F;
        case 0xD7:            /* 0x92D7 */
          return 0x6F70;
        case 0xD8:            /* 0x92D8 */
          return 0x576A;
        case 0xD9:            /* 0x92D9 */
          return 0x58F7;
        case 0xDA:            /* 0x92DA */
          return 0x5B2C;
        case 0xDB:            /* 0x92DB */
          return 0x7D2C;
        case 0xDC:            /* 0x92DC */
          return 0x722A;
        case 0xDD:            /* 0x92DD */
          return 0x540A;
        case 0xDE:            /* 0x92DE */
          return 0x91E3;
        case 0xDF:            /* 0x92DF */
          return 0x9DB4;
        case 0xE0:            /* 0x92E0 */
          return 0x4EAD;
        case 0xE1:            /* 0x92E1 */
          return 0x4F4E;
        case 0xE2:            /* 0x92E2 */
          return 0x505C;
        case 0xE3:            /* 0x92E3 */
          return 0x5075;
        case 0xE4:            /* 0x92E4 */
          return 0x5243;
        case 0xE5:            /* 0x92E5 */
          return 0x8C9E;
        case 0xE6:            /* 0x92E6 */
          return 0x5448;
        case 0xE7:            /* 0x92E7 */
          return 0x5824;
        case 0xE8:            /* 0x92E8 */
          return 0x5B9A;
        case 0xE9:            /* 0x92E9 */
          return 0x5E1D;
        case 0xEA:            /* 0x92EA */
          return 0x5E95;
        case 0xEB:            /* 0x92EB */
          return 0x5EAD;
        case 0xEC:            /* 0x92EC */
          return 0x5EF7;
        case 0xED:            /* 0x92ED */
          return 0x5F1F;
        case 0xEE:            /* 0x92EE */
          return 0x608C;
        case 0xEF:            /* 0x92EF */
          return 0x62B5;
        case 0xF0:            /* 0x92F0 */
          return 0x633A;
        case 0xF1:            /* 0x92F1 */
          return 0x63D0;
        case 0xF2:            /* 0x92F2 */
          return 0x68AF;
        case 0xF3:            /* 0x92F3 */
          return 0x6C40;
        case 0xF4:            /* 0x92F4 */
          return 0x7887;
        case 0xF5:            /* 0x92F5 */
          return 0x798E;
        case 0xF6:            /* 0x92F6 */
          return 0x7A0B;
        case 0xF7:            /* 0x92F7 */
          return 0x7DE0;
        case 0xF8:            /* 0x92F8 */
          return 0x8247;
        case 0xF9:            /* 0x92F9 */
          return 0x8A02;
        case 0xFA:            /* 0x92FA */
          return 0x8AE6;
        case 0xFB:            /* 0x92FB */
          return 0x8E44;
        case 0xFC:            /* 0x92FC */
          return 0x9013;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 92 */

    /* Lead Byte: 0x93xx */
    case 0x93:
      switch (low_byte)
      {
        case 0x40:            /* 0x9340 */
          return 0x90B8;
        case 0x41:            /* 0x9341 */
          return 0x912D;
        case 0x42:            /* 0x9342 */
          return 0x91D8;
        case 0x43:            /* 0x9343 */
          return 0x9F0E;
        case 0x44:            /* 0x9344 */
          return 0x6CE5;
        case 0x45:            /* 0x9345 */
          return 0x6458;
        case 0x46:            /* 0x9346 */
          return 0x64E2;
        case 0x47:            /* 0x9347 */
          return 0x6575;
        case 0x48:            /* 0x9348 */
          return 0x6EF4;
        case 0x49:            /* 0x9349 */
          return 0x7684;
        case 0x4A:            /* 0x934A */
          return 0x7B1B;
        case 0x4B:            /* 0x934B */
          return 0x9069;
        case 0x4C:            /* 0x934C */
          return 0x93D1;
        case 0x4D:            /* 0x934D */
          return 0x6EBA;
        case 0x4E:            /* 0x934E */
          return 0x54F2;
        case 0x4F:            /* 0x934F */
          return 0x5FB9;
        case 0x50:            /* 0x9350 */
          return 0x64A4;
        case 0x51:            /* 0x9351 */
          return 0x8F4D;
        case 0x52:            /* 0x9352 */
          return 0x8FED;
        case 0x53:            /* 0x9353 */
          return 0x9244;
        case 0x54:            /* 0x9354 */
          return 0x5178;
        case 0x55:            /* 0x9355 */
          return 0x586B;
        case 0x56:            /* 0x9356 */
          return 0x5929;
        case 0x57:            /* 0x9357 */
          return 0x5C55;
        case 0x58:            /* 0x9358 */
          return 0x5E97;
        case 0x59:            /* 0x9359 */
          return 0x6DFB;
        case 0x5A:            /* 0x935A */
          return 0x7E8F;
        case 0x5B:            /* 0x935B */
          return 0x751C;
        case 0x5C:            /* 0x935C */
          return 0x8CBC;
        case 0x5D:            /* 0x935D */
          return 0x8EE2;
        case 0x5E:            /* 0x935E */
          return 0x985B;
        case 0x5F:            /* 0x935F */
          return 0x70B9;
        case 0x60:            /* 0x9360 */
          return 0x4F1D;
        case 0x61:            /* 0x9361 */
          return 0x6BBF;
        case 0x62:            /* 0x9362 */
          return 0x6FB1;
        case 0x63:            /* 0x9363 */
          return 0x7530;
        case 0x64:            /* 0x9364 */
          return 0x96FB;
        case 0x65:            /* 0x9365 */
          return 0x514E;
        case 0x66:            /* 0x9366 */
          return 0x5410;
        case 0x67:            /* 0x9367 */
          return 0x5835;
        case 0x68:            /* 0x9368 */
          return 0x5857;
        case 0x69:            /* 0x9369 */
          return 0x59AC;
        case 0x6A:            /* 0x936A */
          return 0x5C60;
        case 0x6B:            /* 0x936B */
          return 0x5F92;
        case 0x6C:            /* 0x936C */
          return 0x6597;
        case 0x6D:            /* 0x936D */
          return 0x675C;
        case 0x6E:            /* 0x936E */
          return 0x6E21;
        case 0x6F:            /* 0x936F */
          return 0x767B;
        case 0x70:            /* 0x9370 */
          return 0x83DF;
        case 0x71:            /* 0x9371 */
          return 0x8CED;
        case 0x72:            /* 0x9372 */
          return 0x9014;
        case 0x73:            /* 0x9373 */
          return 0x90FD;
        case 0x74:            /* 0x9374 */
          return 0x934D;
        case 0x75:            /* 0x9375 */
          return 0x7825;
        case 0x76:            /* 0x9376 */
          return 0x783A;
        case 0x77:            /* 0x9377 */
          return 0x52AA;
        case 0x78:            /* 0x9378 */
          return 0x5EA6;
        case 0x79:            /* 0x9379 */
          return 0x571F;
        case 0x7A:            /* 0x937A */
          return 0x5974;
        case 0x7B:            /* 0x937B */
          return 0x6012;
        case 0x7C:            /* 0x937C */
          return 0x5012;
        case 0x7D:            /* 0x937D */
          return 0x515A;
        case 0x7E:            /* 0x937E */
          return 0x51AC;
        case 0x80:            /* 0x9380 */
          return 0x51CD;
        case 0x81:            /* 0x9381 */
          return 0x5200;
        case 0x82:            /* 0x9382 */
          return 0x5510;
        case 0x83:            /* 0x9383 */
          return 0x5854;
        case 0x84:            /* 0x9384 */
          return 0x5858;
        case 0x85:            /* 0x9385 */
          return 0x5957;
        case 0x86:            /* 0x9386 */
          return 0x5B95;
        case 0x87:            /* 0x9387 */
          return 0x5CF6;
        case 0x88:            /* 0x9388 */
          return 0x5D8B;
        case 0x89:            /* 0x9389 */
          return 0x60BC;
        case 0x8A:            /* 0x938A */
          return 0x6295;
        case 0x8B:            /* 0x938B */
          return 0x642D;
        case 0x8C:            /* 0x938C */
          return 0x6771;
        case 0x8D:            /* 0x938D */
          return 0x6843;
        case 0x8E:            /* 0x938E */
          return 0x68BC;
        case 0x8F:            /* 0x938F */
          return 0x68DF;
        case 0x90:            /* 0x9390 */
          return 0x76D7;
        case 0x91:            /* 0x9391 */
          return 0x6DD8;
        case 0x92:            /* 0x9392 */
          return 0x6E6F;
        case 0x93:            /* 0x9393 */
          return 0x6D9B;
        case 0x94:            /* 0x9394 */
          return 0x706F;
        case 0x95:            /* 0x9395 */
          return 0x71C8;
        case 0x96:            /* 0x9396 */
          return 0x5F53;
        case 0x97:            /* 0x9397 */
          return 0x75D8;
        case 0x98:            /* 0x9398 */
          return 0x7977;
        case 0x99:            /* 0x9399 */
          return 0x7B49;
        case 0x9A:            /* 0x939A */
          return 0x7B54;
        case 0x9B:            /* 0x939B */
          return 0x7B52;
        case 0x9C:            /* 0x939C */
          return 0x7CD6;
        case 0x9D:            /* 0x939D */
          return 0x7D71;
        case 0x9E:            /* 0x939E */
          return 0x5230;
        case 0x9F:            /* 0x939F */
          return 0x8463;
        case 0xA0:            /* 0x93A0 */
          return 0x8569;
        case 0xA1:            /* 0x93A1 */
          return 0x85E4;
        case 0xA2:            /* 0x93A2 */
          return 0x8A0E;
        case 0xA3:            /* 0x93A3 */
          return 0x8B04;
        case 0xA4:            /* 0x93A4 */
          return 0x8C46;
        case 0xA5:            /* 0x93A5 */
          return 0x8E0F;
        case 0xA6:            /* 0x93A6 */
          return 0x9003;
        case 0xA7:            /* 0x93A7 */
          return 0x900F;
        case 0xA8:            /* 0x93A8 */
          return 0x9419;
        case 0xA9:            /* 0x93A9 */
          return 0x9676;
        case 0xAA:            /* 0x93AA */
          return 0x982D;
        case 0xAB:            /* 0x93AB */
          return 0x9A30;
        case 0xAC:            /* 0x93AC */
          return 0x95D8;
        case 0xAD:            /* 0x93AD */
          return 0x50CD;
        case 0xAE:            /* 0x93AE */
          return 0x52D5;
        case 0xAF:            /* 0x93AF */
          return 0x540C;
        case 0xB0:            /* 0x93B0 */
          return 0x5802;
        case 0xB1:            /* 0x93B1 */
          return 0x5C0E;
        case 0xB2:            /* 0x93B2 */
          return 0x61A7;
        case 0xB3:            /* 0x93B3 */
          return 0x649E;
        case 0xB4:            /* 0x93B4 */
          return 0x6D1E;
        case 0xB5:            /* 0x93B5 */
          return 0x77B3;
        case 0xB6:            /* 0x93B6 */
          return 0x7AE5;
        case 0xB7:            /* 0x93B7 */
          return 0x80F4;
        case 0xB8:            /* 0x93B8 */
          return 0x8404;
        case 0xB9:            /* 0x93B9 */
          return 0x9053;
        case 0xBA:            /* 0x93BA */
          return 0x9285;
        case 0xBB:            /* 0x93BB */
          return 0x5CE0;
        case 0xBC:            /* 0x93BC */
          return 0x9D07;
        case 0xBD:            /* 0x93BD */
          return 0x533F;
        case 0xBE:            /* 0x93BE */
          return 0x5F97;
        case 0xBF:            /* 0x93BF */
          return 0x5FB3;
        case 0xC0:            /* 0x93C0 */
          return 0x6D9C;
        case 0xC1:            /* 0x93C1 */
          return 0x7279;
        case 0xC2:            /* 0x93C2 */
          return 0x7763;
        case 0xC3:            /* 0x93C3 */
          return 0x79BF;
        case 0xC4:            /* 0x93C4 */
          return 0x7BE4;
        case 0xC5:            /* 0x93C5 */
          return 0x6BD2;
        case 0xC6:            /* 0x93C6 */
          return 0x72EC;
        case 0xC7:            /* 0x93C7 */
          return 0x8AAD;
        case 0xC8:            /* 0x93C8 */
          return 0x6803;
        case 0xC9:            /* 0x93C9 */
          return 0x6A61;
        case 0xCA:            /* 0x93CA */
          return 0x51F8;
        case 0xCB:            /* 0x93CB */
          return 0x7A81;
        case 0xCC:            /* 0x93CC */
          return 0x6934;
        case 0xCD:            /* 0x93CD */
          return 0x5C4A;
        case 0xCE:            /* 0x93CE */
          return 0x9CF6;
        case 0xCF:            /* 0x93CF */
          return 0x82EB;
        case 0xD0:            /* 0x93D0 */
          return 0x5BC5;
        case 0xD1:            /* 0x93D1 */
          return 0x9149;
        case 0xD2:            /* 0x93D2 */
          return 0x701E;
        case 0xD3:            /* 0x93D3 */
          return 0x5678;
        case 0xD4:            /* 0x93D4 */
          return 0x5C6F;
        case 0xD5:            /* 0x93D5 */
          return 0x60C7;
        case 0xD6:            /* 0x93D6 */
          return 0x6566;
        case 0xD7:            /* 0x93D7 */
          return 0x6C8C;
        case 0xD8:            /* 0x93D8 */
          return 0x8C5A;
        case 0xD9:            /* 0x93D9 */
          return 0x9041;
        case 0xDA:            /* 0x93DA */
          return 0x9813;
        case 0xDB:            /* 0x93DB */
          return 0x5451;
        case 0xDC:            /* 0x93DC */
          return 0x66C7;
        case 0xDD:            /* 0x93DD */
          return 0x920D;
        case 0xDE:            /* 0x93DE */
          return 0x5948;
        case 0xDF:            /* 0x93DF */
          return 0x90A3;
        case 0xE0:            /* 0x93E0 */
          return 0x5185;
        case 0xE1:            /* 0x93E1 */
          return 0x4E4D;
        case 0xE2:            /* 0x93E2 */
          return 0x51EA;
        case 0xE3:            /* 0x93E3 */
          return 0x8599;
        case 0xE4:            /* 0x93E4 */
          return 0x8B0E;
        case 0xE5:            /* 0x93E5 */
          return 0x7058;
        case 0xE6:            /* 0x93E6 */
          return 0x637A;
        case 0xE7:            /* 0x93E7 */
          return 0x934B;
        case 0xE8:            /* 0x93E8 */
          return 0x6962;
        case 0xE9:            /* 0x93E9 */
          return 0x99B4;
        case 0xEA:            /* 0x93EA */
          return 0x7E04;
        case 0xEB:            /* 0x93EB */
          return 0x7577;
        case 0xEC:            /* 0x93EC */
          return 0x5357;
        case 0xED:            /* 0x93ED */
          return 0x6960;
        case 0xEE:            /* 0x93EE */
          return 0x8EDF;
        case 0xEF:            /* 0x93EF */
          return 0x96E3;
        case 0xF0:            /* 0x93F0 */
          return 0x6C5D;
        case 0xF1:            /* 0x93F1 */
          return 0x4E8C;
        case 0xF2:            /* 0x93F2 */
          return 0x5C3C;
        case 0xF3:            /* 0x93F3 */
          return 0x5F10;
        case 0xF4:            /* 0x93F4 */
          return 0x8FE9;
        case 0xF5:            /* 0x93F5 */
          return 0x5302;
        case 0xF6:            /* 0x93F6 */
          return 0x8CD1;
        case 0xF7:            /* 0x93F7 */
          return 0x8089;
        case 0xF8:            /* 0x93F8 */
          return 0x8679;
        case 0xF9:            /* 0x93F9 */
          return 0x5EFF;
        case 0xFA:            /* 0x93FA */
          return 0x65E5;
        case 0xFB:            /* 0x93FB */
          return 0x4E73;
        case 0xFC:            /* 0x93FC */
          return 0x5165;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 93 */

    /* Lead Byte: 0x94xx */
    case 0x94:
      switch (low_byte)
      {
        case 0x40:            /* 0x9440 */
          return 0x5982;
        case 0x41:            /* 0x9441 */
          return 0x5C3F;
        case 0x42:            /* 0x9442 */
          return 0x97EE;
        case 0x43:            /* 0x9443 */
          return 0x4EFB;
        case 0x44:            /* 0x9444 */
          return 0x598A;
        case 0x45:            /* 0x9445 */
          return 0x5FCD;
        case 0x46:            /* 0x9446 */
          return 0x8A8D;
        case 0x47:            /* 0x9447 */
          return 0x6FE1;
        case 0x48:            /* 0x9448 */
          return 0x79B0;
        case 0x49:            /* 0x9449 */
          return 0x7962;
        case 0x4A:            /* 0x944A */
          return 0x5BE7;
        case 0x4B:            /* 0x944B */
          return 0x8471;
        case 0x4C:            /* 0x944C */
          return 0x732B;
        case 0x4D:            /* 0x944D */
          return 0x71B1;
        case 0x4E:            /* 0x944E */
          return 0x5E74;
        case 0x4F:            /* 0x944F */
          return 0x5FF5;
        case 0x50:            /* 0x9450 */
          return 0x637B;
        case 0x51:            /* 0x9451 */
          return 0x649A;
        case 0x52:            /* 0x9452 */
          return 0x71C3;
        case 0x53:            /* 0x9453 */
          return 0x7C98;
        case 0x54:            /* 0x9454 */
          return 0x4E43;
        case 0x55:            /* 0x9455 */
          return 0x5EFC;
        case 0x56:            /* 0x9456 */
          return 0x4E4B;
        case 0x57:            /* 0x9457 */
          return 0x57DC;
        case 0x58:            /* 0x9458 */
          return 0x56A2;
        case 0x59:            /* 0x9459 */
          return 0x60A9;
        case 0x5A:            /* 0x945A */
          return 0x6FC3;
        case 0x5B:            /* 0x945B */
          return 0x7D0D;
        case 0x5C:            /* 0x945C */
          return 0x80FD;
        case 0x5D:            /* 0x945D */
          return 0x8133;
        case 0x5E:            /* 0x945E */
          return 0x81BF;
        case 0x5F:            /* 0x945F */
          return 0x8FB2;
        case 0x60:            /* 0x9460 */
          return 0x8997;
        case 0x61:            /* 0x9461 */
          return 0x86A4;
        case 0x62:            /* 0x9462 */
          return 0x5DF4;
        case 0x63:            /* 0x9463 */
          return 0x628A;
        case 0x64:            /* 0x9464 */
          return 0x64AD;
        case 0x65:            /* 0x9465 */
          return 0x8987;
        case 0x66:            /* 0x9466 */
          return 0x6777;
        case 0x67:            /* 0x9467 */
          return 0x6CE2;
        case 0x68:            /* 0x9468 */
          return 0x6D3E;
        case 0x69:            /* 0x9469 */
          return 0x7436;
        case 0x6A:            /* 0x946A */
          return 0x7834;
        case 0x6B:            /* 0x946B */
          return 0x5A46;
        case 0x6C:            /* 0x946C */
          return 0x7F75;
        case 0x6D:            /* 0x946D */
          return 0x82AD;
        case 0x6E:            /* 0x946E */
          return 0x99AC;
        case 0x6F:            /* 0x946F */
          return 0x4FF3;
        case 0x70:            /* 0x9470 */
          return 0x5EC3;
        case 0x71:            /* 0x9471 */
          return 0x62DD;
        case 0x72:            /* 0x9472 */
          return 0x6392;
        case 0x73:            /* 0x9473 */
          return 0x6557;
        case 0x74:            /* 0x9474 */
          return 0x676F;
        case 0x75:            /* 0x9475 */
          return 0x76C3;
        case 0x76:            /* 0x9476 */
          return 0x724C;
        case 0x77:            /* 0x9477 */
          return 0x80CC;
        case 0x78:            /* 0x9478 */
          return 0x80BA;
        case 0x79:            /* 0x9479 */
          return 0x8F29;
        case 0x7A:            /* 0x947A */
          return 0x914D;
        case 0x7B:            /* 0x947B */
          return 0x500D;
        case 0x7C:            /* 0x947C */
          return 0x57F9;
        case 0x7D:            /* 0x947D */
          return 0x5A92;
        case 0x7E:            /* 0x947E */
          return 0x6885;
        case 0x80:            /* 0x9480 */
          return 0x6973;
        case 0x81:            /* 0x9481 */
          return 0x7164;
        case 0x82:            /* 0x9482 */
          return 0x72FD;
        case 0x83:            /* 0x9483 */
          return 0x8CB7;
        case 0x84:            /* 0x9484 */
          return 0x58F2;
        case 0x85:            /* 0x9485 */
          return 0x8CE0;
        case 0x86:            /* 0x9486 */
          return 0x966A;
        case 0x87:            /* 0x9487 */
          return 0x9019;
        case 0x88:            /* 0x9488 */
          return 0x877F;
        case 0x89:            /* 0x9489 */
          return 0x79E4;
        case 0x8A:            /* 0x948A */
          return 0x77E7;
        case 0x8B:            /* 0x948B */
          return 0x8429;
        case 0x8C:            /* 0x948C */
          return 0x4F2F;
        case 0x8D:            /* 0x948D */
          return 0x5265;
        case 0x8E:            /* 0x948E */
          return 0x535A;
        case 0x8F:            /* 0x948F */
          return 0x62CD;
        case 0x90:            /* 0x9490 */
          return 0x67CF;
        case 0x91:            /* 0x9491 */
          return 0x6CCA;
        case 0x92:            /* 0x9492 */
          return 0x767D;
        case 0x93:            /* 0x9493 */
          return 0x7B94;
        case 0x94:            /* 0x9494 */
          return 0x7C95;
        case 0x95:            /* 0x9495 */
          return 0x8236;
        case 0x96:            /* 0x9496 */
          return 0x8584;
        case 0x97:            /* 0x9497 */
          return 0x8FEB;
        case 0x98:            /* 0x9498 */
          return 0x66DD;
        case 0x99:            /* 0x9499 */
          return 0x6F20;
        case 0x9A:            /* 0x949A */
          return 0x7206;
        case 0x9B:            /* 0x949B */
          return 0x7E1B;
        case 0x9C:            /* 0x949C */
          return 0x83AB;
        case 0x9D:            /* 0x949D */
          return 0x99C1;
        case 0x9E:            /* 0x949E */
          return 0x9EA6;
        case 0x9F:            /* 0x949F */
          return 0x51FD;
        case 0xA0:            /* 0x94A0 */
          return 0x7BB1;
        case 0xA1:            /* 0x94A1 */
          return 0x7872;
        case 0xA2:            /* 0x94A2 */
          return 0x7BB8;
        case 0xA3:            /* 0x94A3 */
          return 0x8087;
        case 0xA4:            /* 0x94A4 */
          return 0x7B48;
        case 0xA5:            /* 0x94A5 */
          return 0x6AE8;
        case 0xA6:            /* 0x94A6 */
          return 0x5E61;
        case 0xA7:            /* 0x94A7 */
          return 0x808C;
        case 0xA8:            /* 0x94A8 */
          return 0x7551;
        case 0xA9:            /* 0x94A9 */
          return 0x7560;
        case 0xAA:            /* 0x94AA */
          return 0x516B;
        case 0xAB:            /* 0x94AB */
          return 0x9262;
        case 0xAC:            /* 0x94AC */
          return 0x6E8C;
        case 0xAD:            /* 0x94AD */
          return 0x767A;
        case 0xAE:            /* 0x94AE */
          return 0x9197;
        case 0xAF:            /* 0x94AF */
          return 0x9AEA;
        case 0xB0:            /* 0x94B0 */
          return 0x4F10;
        case 0xB1:            /* 0x94B1 */
          return 0x7F70;
        case 0xB2:            /* 0x94B2 */
          return 0x629C;
        case 0xB3:            /* 0x94B3 */
          return 0x7B4F;
        case 0xB4:            /* 0x94B4 */
          return 0x95A5;
        case 0xB5:            /* 0x94B5 */
          return 0x9CE9;
        case 0xB6:            /* 0x94B6 */
          return 0x567A;
        case 0xB7:            /* 0x94B7 */
          return 0x5859;
        case 0xB8:            /* 0x94B8 */
          return 0x86E4;
        case 0xB9:            /* 0x94B9 */
          return 0x96BC;
        case 0xBA:            /* 0x94BA */
          return 0x4F34;
        case 0xBB:            /* 0x94BB */
          return 0x5224;
        case 0xBC:            /* 0x94BC */
          return 0x534A;
        case 0xBD:            /* 0x94BD */
          return 0x53CD;
        case 0xBE:            /* 0x94BE */
          return 0x53DB;
        case 0xBF:            /* 0x94BF */
          return 0x5E06;
        case 0xC0:            /* 0x94C0 */
          return 0x642C;
        case 0xC1:            /* 0x94C1 */
          return 0x6591;
        case 0xC2:            /* 0x94C2 */
          return 0x677F;
        case 0xC3:            /* 0x94C3 */
          return 0x6C3E;
        case 0xC4:            /* 0x94C4 */
          return 0x6C4E;
        case 0xC5:            /* 0x94C5 */
          return 0x7248;
        case 0xC6:            /* 0x94C6 */
          return 0x72AF;
        case 0xC7:            /* 0x94C7 */
          return 0x73ED;
        case 0xC8:            /* 0x94C8 */
          return 0x7554;
        case 0xC9:            /* 0x94C9 */
          return 0x7E41;
        case 0xCA:            /* 0x94CA */
          return 0x822C;
        case 0xCB:            /* 0x94CB */
          return 0x85E9;
        case 0xCC:            /* 0x94CC */
          return 0x8CA9;
        case 0xCD:            /* 0x94CD */
          return 0x7BC4;
        case 0xCE:            /* 0x94CE */
          return 0x91C6;
        case 0xCF:            /* 0x94CF */
          return 0x7169;
        case 0xD0:            /* 0x94D0 */
          return 0x9812;
        case 0xD1:            /* 0x94D1 */
          return 0x98EF;
        case 0xD2:            /* 0x94D2 */
          return 0x633D;
        case 0xD3:            /* 0x94D3 */
          return 0x6669;
        case 0xD4:            /* 0x94D4 */
          return 0x756A;
        case 0xD5:            /* 0x94D5 */
          return 0x76E4;
        case 0xD6:            /* 0x94D6 */
          return 0x78D0;
        case 0xD7:            /* 0x94D7 */
          return 0x8543;
        case 0xD8:            /* 0x94D8 */
          return 0x86EE;
        case 0xD9:            /* 0x94D9 */
          return 0x532A;
        case 0xDA:            /* 0x94DA */
          return 0x5351;
        case 0xDB:            /* 0x94DB */
          return 0x5426;
        case 0xDC:            /* 0x94DC */
          return 0x5983;
        case 0xDD:            /* 0x94DD */
          return 0x5E87;
        case 0xDE:            /* 0x94DE */
          return 0x5F7C;
        case 0xDF:            /* 0x94DF */
          return 0x60B2;
        case 0xE0:            /* 0x94E0 */
          return 0x6249;
        case 0xE1:            /* 0x94E1 */
          return 0x6279;
        case 0xE2:            /* 0x94E2 */
          return 0x62AB;
        case 0xE3:            /* 0x94E3 */
          return 0x6590;
        case 0xE4:            /* 0x94E4 */
          return 0x6BD4;
        case 0xE5:            /* 0x94E5 */
          return 0x6CCC;
        case 0xE6:            /* 0x94E6 */
          return 0x75B2;
        case 0xE7:            /* 0x94E7 */
          return 0x76AE;
        case 0xE8:            /* 0x94E8 */
          return 0x7891;
        case 0xE9:            /* 0x94E9 */
          return 0x79D8;
        case 0xEA:            /* 0x94EA */
          return 0x7DCB;
        case 0xEB:            /* 0x94EB */
          return 0x7F77;
        case 0xEC:            /* 0x94EC */
          return 0x80A5;
        case 0xED:            /* 0x94ED */
          return 0x88AB;
        case 0xEE:            /* 0x94EE */
          return 0x8AB9;
        case 0xEF:            /* 0x94EF */
          return 0x8CBB;
        case 0xF0:            /* 0x94F0 */
          return 0x907F;
        case 0xF1:            /* 0x94F1 */
          return 0x975E;
        case 0xF2:            /* 0x94F2 */
          return 0x98DB;
        case 0xF3:            /* 0x94F3 */
          return 0x6A0B;
        case 0xF4:            /* 0x94F4 */
          return 0x7C38;
        case 0xF5:            /* 0x94F5 */
          return 0x5099;
        case 0xF6:            /* 0x94F6 */
          return 0x5C3E;
        case 0xF7:            /* 0x94F7 */
          return 0x5FAE;
        case 0xF8:            /* 0x94F8 */
          return 0x6787;
        case 0xF9:            /* 0x94F9 */
          return 0x6BD8;
        case 0xFA:            /* 0x94FA */
          return 0x7435;
        case 0xFB:            /* 0x94FB */
          return 0x7709;
        case 0xFC:            /* 0x94FC */
          return 0x7F8E;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 94 */

    /* Lead Byte: 0x95xx */
    case 0x95:
      switch (low_byte)
      {
        case 0x40:            /* 0x9540 */
          return 0x9F3B;
        case 0x41:            /* 0x9541 */
          return 0x67CA;
        case 0x42:            /* 0x9542 */
          return 0x7A17;
        case 0x43:            /* 0x9543 */
          return 0x5339;
        case 0x44:            /* 0x9544 */
          return 0x758B;
        case 0x45:            /* 0x9545 */
          return 0x9AED;
        case 0x46:            /* 0x9546 */
          return 0x5F66;
        case 0x47:            /* 0x9547 */
          return 0x819D;
        case 0x48:            /* 0x9548 */
          return 0x83F1;
        case 0x49:            /* 0x9549 */
          return 0x8098;
        case 0x4A:            /* 0x954A */
          return 0x5F3C;
        case 0x4B:            /* 0x954B */
          return 0x5FC5;
        case 0x4C:            /* 0x954C */
          return 0x7562;
        case 0x4D:            /* 0x954D */
          return 0x7B46;
        case 0x4E:            /* 0x954E */
          return 0x903C;
        case 0x4F:            /* 0x954F */
          return 0x6867;
        case 0x50:            /* 0x9550 */
          return 0x59EB;
        case 0x51:            /* 0x9551 */
          return 0x5A9B;
        case 0x52:            /* 0x9552 */
          return 0x7D10;
        case 0x53:            /* 0x9553 */
          return 0x767E;
        case 0x54:            /* 0x9554 */
          return 0x8B2C;
        case 0x55:            /* 0x9555 */
          return 0x4FF5;
        case 0x56:            /* 0x9556 */
          return 0x5F6A;
        case 0x57:            /* 0x9557 */
          return 0x6A19;
        case 0x58:            /* 0x9558 */
          return 0x6C37;
        case 0x59:            /* 0x9559 */
          return 0x6F02;
        case 0x5A:            /* 0x955A */
          return 0x74E2;
        case 0x5B:            /* 0x955B */
          return 0x7968;
        case 0x5C:            /* 0x955C */
          return 0x8868;
        case 0x5D:            /* 0x955D */
          return 0x8A55;
        case 0x5E:            /* 0x955E */
          return 0x8C79;
        case 0x5F:            /* 0x955F */
          return 0x5EDF;
        case 0x60:            /* 0x9560 */
          return 0x63CF;
        case 0x61:            /* 0x9561 */
          return 0x75C5;
        case 0x62:            /* 0x9562 */
          return 0x79D2;
        case 0x63:            /* 0x9563 */
          return 0x82D7;
        case 0x64:            /* 0x9564 */
          return 0x9328;
        case 0x65:            /* 0x9565 */
          return 0x92F2;
        case 0x66:            /* 0x9566 */
          return 0x849C;
        case 0x67:            /* 0x9567 */
          return 0x86ED;
        case 0x68:            /* 0x9568 */
          return 0x9C2D;
        case 0x69:            /* 0x9569 */
          return 0x54C1;
        case 0x6A:            /* 0x956A */
          return 0x5F6C;
        case 0x6B:            /* 0x956B */
          return 0x658C;
        case 0x6C:            /* 0x956C */
          return 0x6D5C;
        case 0x6D:            /* 0x956D */
          return 0x7015;
        case 0x6E:            /* 0x956E */
          return 0x8CA7;
        case 0x6F:            /* 0x956F */
          return 0x8CD3;
        case 0x70:            /* 0x9570 */
          return 0x983B;
        case 0x71:            /* 0x9571 */
          return 0x654F;
        case 0x72:            /* 0x9572 */
          return 0x74F6;
        case 0x73:            /* 0x9573 */
          return 0x4E0D;
        case 0x74:            /* 0x9574 */
          return 0x4ED8;
        case 0x75:            /* 0x9575 */
          return 0x57E0;
        case 0x76:            /* 0x9576 */
          return 0x592B;
        case 0x77:            /* 0x9577 */
          return 0x5A66;
        case 0x78:            /* 0x9578 */
          return 0x5BCC;
        case 0x79:            /* 0x9579 */
          return 0x51A8;
        case 0x7A:            /* 0x957A */
          return 0x5E03;
        case 0x7B:            /* 0x957B */
          return 0x5E9C;
        case 0x7C:            /* 0x957C */
          return 0x6016;
        case 0x7D:            /* 0x957D */
          return 0x6276;
        case 0x7E:            /* 0x957E */
          return 0x6577;
        case 0x80:            /* 0x9580 */
          return 0x65A7;
        case 0x81:            /* 0x9581 */
          return 0x666E;
        case 0x82:            /* 0x9582 */
          return 0x6D6E;
        case 0x83:            /* 0x9583 */
          return 0x7236;
        case 0x84:            /* 0x9584 */
          return 0x7B26;
        case 0x85:            /* 0x9585 */
          return 0x8150;
        case 0x86:            /* 0x9586 */
          return 0x819A;
        case 0x87:            /* 0x9587 */
          return 0x8299;
        case 0x88:            /* 0x9588 */
          return 0x8B5C;
        case 0x89:            /* 0x9589 */
          return 0x8CA0;
        case 0x8A:            /* 0x958A */
          return 0x8CE6;
        case 0x8B:            /* 0x958B */
          return 0x8D74;
        case 0x8C:            /* 0x958C */
          return 0x961C;
        case 0x8D:            /* 0x958D */
          return 0x9644;
        case 0x8E:            /* 0x958E */
          return 0x4FAE;
        case 0x8F:            /* 0x958F */
          return 0x64AB;
        case 0x90:            /* 0x9590 */
          return 0x6B66;
        case 0x91:            /* 0x9591 */
          return 0x821E;
        case 0x92:            /* 0x9592 */
          return 0x8461;
        case 0x93:            /* 0x9593 */
          return 0x856A;
        case 0x94:            /* 0x9594 */
          return 0x90E8;
        case 0x95:            /* 0x9595 */
          return 0x5C01;
        case 0x96:            /* 0x9596 */
          return 0x6953;
        case 0x97:            /* 0x9597 */
          return 0x98A8;
        case 0x98:            /* 0x9598 */
          return 0x847A;
        case 0x99:            /* 0x9599 */
          return 0x8557;
        case 0x9A:            /* 0x959A */
          return 0x4F0F;
        case 0x9B:            /* 0x959B */
          return 0x526F;
        case 0x9C:            /* 0x959C */
          return 0x5FA9;
        case 0x9D:            /* 0x959D */
          return 0x5E45;
        case 0x9E:            /* 0x959E */
          return 0x670D;
        case 0x9F:            /* 0x959F */
          return 0x798F;
        case 0xA0:            /* 0x95A0 */
          return 0x8179;
        case 0xA1:            /* 0x95A1 */
          return 0x8907;
        case 0xA2:            /* 0x95A2 */
          return 0x8986;
        case 0xA3:            /* 0x95A3 */
          return 0x6DF5;
        case 0xA4:            /* 0x95A4 */
          return 0x5F17;
        case 0xA5:            /* 0x95A5 */
          return 0x6255;
        case 0xA6:            /* 0x95A6 */
          return 0x6CB8;
        case 0xA7:            /* 0x95A7 */
          return 0x4ECF;
        case 0xA8:            /* 0x95A8 */
          return 0x7269;
        case 0xA9:            /* 0x95A9 */
          return 0x9B92;
        case 0xAA:            /* 0x95AA */
          return 0x5206;
        case 0xAB:            /* 0x95AB */
          return 0x543B;
        case 0xAC:            /* 0x95AC */
          return 0x5674;
        case 0xAD:            /* 0x95AD */
          return 0x58B3;
        case 0xAE:            /* 0x95AE */
          return 0x61A4;
        case 0xAF:            /* 0x95AF */
          return 0x626E;
        case 0xB0:            /* 0x95B0 */
          return 0x711A;
        case 0xB1:            /* 0x95B1 */
          return 0x596E;
        case 0xB2:            /* 0x95B2 */
          return 0x7C89;
        case 0xB3:            /* 0x95B3 */
          return 0x7CDE;
        case 0xB4:            /* 0x95B4 */
          return 0x7D1B;
        case 0xB5:            /* 0x95B5 */
          return 0x96F0;
        case 0xB6:            /* 0x95B6 */
          return 0x6587;
        case 0xB7:            /* 0x95B7 */
          return 0x805E;
        case 0xB8:            /* 0x95B8 */
          return 0x4E19;
        case 0xB9:            /* 0x95B9 */
          return 0x4F75;
        case 0xBA:            /* 0x95BA */
          return 0x5175;
        case 0xBB:            /* 0x95BB */
          return 0x5840;
        case 0xBC:            /* 0x95BC */
          return 0x5E63;
        case 0xBD:            /* 0x95BD */
          return 0x5E73;
        case 0xBE:            /* 0x95BE */
          return 0x5F0A;
        case 0xBF:            /* 0x95BF */
          return 0x67C4;
        case 0xC0:            /* 0x95C0 */
          return 0x4E26;
        case 0xC1:            /* 0x95C1 */
          return 0x853D;
        case 0xC2:            /* 0x95C2 */
          return 0x9589;
        case 0xC3:            /* 0x95C3 */
          return 0x965B;
        case 0xC4:            /* 0x95C4 */
          return 0x7C73;
        case 0xC5:            /* 0x95C5 */
          return 0x9801;
        case 0xC6:            /* 0x95C6 */
          return 0x50FB;
        case 0xC7:            /* 0x95C7 */
          return 0x58C1;
        case 0xC8:            /* 0x95C8 */
          return 0x7656;
        case 0xC9:            /* 0x95C9 */
          return 0x78A7;
        case 0xCA:            /* 0x95CA */
          return 0x5225;
        case 0xCB:            /* 0x95CB */
          return 0x77A5;
        case 0xCC:            /* 0x95CC */
          return 0x8511;
        case 0xCD:            /* 0x95CD */
          return 0x7B86;
        case 0xCE:            /* 0x95CE */
          return 0x504F;
        case 0xCF:            /* 0x95CF */
          return 0x5909;
        case 0xD0:            /* 0x95D0 */
          return 0x7247;
        case 0xD1:            /* 0x95D1 */
          return 0x7BC7;
        case 0xD2:            /* 0x95D2 */
          return 0x7DE8;
        case 0xD3:            /* 0x95D3 */
          return 0x8FBA;
        case 0xD4:            /* 0x95D4 */
          return 0x8FD4;
        case 0xD5:            /* 0x95D5 */
          return 0x904D;
        case 0xD6:            /* 0x95D6 */
          return 0x4FBF;
        case 0xD7:            /* 0x95D7 */
          return 0x52C9;
        case 0xD8:            /* 0x95D8 */
          return 0x5A29;
        case 0xD9:            /* 0x95D9 */
          return 0x5F01;
        case 0xDA:            /* 0x95DA */
          return 0x97AD;
        case 0xDB:            /* 0x95DB */
          return 0x4FDD;
        case 0xDC:            /* 0x95DC */
          return 0x8217;
        case 0xDD:            /* 0x95DD */
          return 0x92EA;
        case 0xDE:            /* 0x95DE */
          return 0x5703;
        case 0xDF:            /* 0x95DF */
          return 0x6355;
        case 0xE0:            /* 0x95E0 */
          return 0x6B69;
        case 0xE1:            /* 0x95E1 */
          return 0x752B;
        case 0xE2:            /* 0x95E2 */
          return 0x88DC;
        case 0xE3:            /* 0x95E3 */
          return 0x8F14;
        case 0xE4:            /* 0x95E4 */
          return 0x7A42;
        case 0xE5:            /* 0x95E5 */
          return 0x52DF;
        case 0xE6:            /* 0x95E6 */
          return 0x5893;
        case 0xE7:            /* 0x95E7 */
          return 0x6155;
        case 0xE8:            /* 0x95E8 */
          return 0x620A;
        case 0xE9:            /* 0x95E9 */
          return 0x66AE;
        case 0xEA:            /* 0x95EA */
          return 0x6BCD;
        case 0xEB:            /* 0x95EB */
          return 0x7C3F;
        case 0xEC:            /* 0x95EC */
          return 0x83E9;
        case 0xED:            /* 0x95ED */
          return 0x5023;
        case 0xEE:            /* 0x95EE */
          return 0x4FF8;
        case 0xEF:            /* 0x95EF */
          return 0x5305;
        case 0xF0:            /* 0x95F0 */
          return 0x5446;
        case 0xF1:            /* 0x95F1 */
          return 0x5831;
        case 0xF2:            /* 0x95F2 */
          return 0x5949;
        case 0xF3:            /* 0x95F3 */
          return 0x5B9D;
        case 0xF4:            /* 0x95F4 */
          return 0x5CF0;
        case 0xF5:            /* 0x95F5 */
          return 0x5CEF;
        case 0xF6:            /* 0x95F6 */
          return 0x5D29;
        case 0xF7:            /* 0x95F7 */
          return 0x5E96;
        case 0xF8:            /* 0x95F8 */
          return 0x62B1;
        case 0xF9:            /* 0x95F9 */
          return 0x6367;
        case 0xFA:            /* 0x95FA */
          return 0x653E;
        case 0xFB:            /* 0x95FB */
          return 0x65B9;
        case 0xFC:            /* 0x95FC */
          return 0x670B;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 95 */

    /* Lead Byte: 0x96xx */
    case 0x96:
      switch (low_byte)
      {
        case 0x40:            /* 0x9640 */
          return 0x6CD5;
        case 0x41:            /* 0x9641 */
          return 0x6CE1;
        case 0x42:            /* 0x9642 */
          return 0x70F9;
        case 0x43:            /* 0x9643 */
          return 0x7832;
        case 0x44:            /* 0x9644 */
          return 0x7E2B;
        case 0x45:            /* 0x9645 */
          return 0x80DE;
        case 0x46:            /* 0x9646 */
          return 0x82B3;
        case 0x47:            /* 0x9647 */
          return 0x840C;
        case 0x48:            /* 0x9648 */
          return 0x84EC;
        case 0x49:            /* 0x9649 */
          return 0x8702;
        case 0x4A:            /* 0x964A */
          return 0x8912;
        case 0x4B:            /* 0x964B */
          return 0x8A2A;
        case 0x4C:            /* 0x964C */
          return 0x8C4A;
        case 0x4D:            /* 0x964D */
          return 0x90A6;
        case 0x4E:            /* 0x964E */
          return 0x92D2;
        case 0x4F:            /* 0x964F */
          return 0x98FD;
        case 0x50:            /* 0x9650 */
          return 0x9CF3;
        case 0x51:            /* 0x9651 */
          return 0x9D6C;
        case 0x52:            /* 0x9652 */
          return 0x4E4F;
        case 0x53:            /* 0x9653 */
          return 0x4EA1;
        case 0x54:            /* 0x9654 */
          return 0x508D;
        case 0x55:            /* 0x9655 */
          return 0x5256;
        case 0x56:            /* 0x9656 */
          return 0x574A;
        case 0x57:            /* 0x9657 */
          return 0x59A8;
        case 0x58:            /* 0x9658 */
          return 0x5E3D;
        case 0x59:            /* 0x9659 */
          return 0x5FD8;
        case 0x5A:            /* 0x965A */
          return 0x5FD9;
        case 0x5B:            /* 0x965B */
          return 0x623F;
        case 0x5C:            /* 0x965C */
          return 0x66B4;
        case 0x5D:            /* 0x965D */
          return 0x671B;
        case 0x5E:            /* 0x965E */
          return 0x67D0;
        case 0x5F:            /* 0x965F */
          return 0x68D2;
        case 0x60:            /* 0x9660 */
          return 0x5192;
        case 0x61:            /* 0x9661 */
          return 0x7D21;
        case 0x62:            /* 0x9662 */
          return 0x80AA;
        case 0x63:            /* 0x9663 */
          return 0x81A8;
        case 0x64:            /* 0x9664 */
          return 0x8B00;
        case 0x65:            /* 0x9665 */
          return 0x8C8C;
        case 0x66:            /* 0x9666 */
          return 0x8CBF;
        case 0x67:            /* 0x9667 */
          return 0x927E;
        case 0x68:            /* 0x9668 */
          return 0x9632;
        case 0x69:            /* 0x9669 */
          return 0x5420;
        case 0x6A:            /* 0x966A */
          return 0x982C;
        case 0x6B:            /* 0x966B */
          return 0x5317;
        case 0x6C:            /* 0x966C */
          return 0x50D5;
        case 0x6D:            /* 0x966D */
          return 0x535C;
        case 0x6E:            /* 0x966E */
          return 0x58A8;
        case 0x6F:            /* 0x966F */
          return 0x64B2;
        case 0x70:            /* 0x9670 */
          return 0x6734;
        case 0x71:            /* 0x9671 */
          return 0x7267;
        case 0x72:            /* 0x9672 */
          return 0x7766;
        case 0x73:            /* 0x9673 */
          return 0x7A46;
        case 0x74:            /* 0x9674 */
          return 0x91E6;
        case 0x75:            /* 0x9675 */
          return 0x52C3;
        case 0x76:            /* 0x9676 */
          return 0x6CA1;
        case 0x77:            /* 0x9677 */
          return 0x6B86;
        case 0x78:            /* 0x9678 */
          return 0x5800;
        case 0x79:            /* 0x9679 */
          return 0x5E4C;
        case 0x7A:            /* 0x967A */
          return 0x5954;
        case 0x7B:            /* 0x967B */
          return 0x672C;
        case 0x7C:            /* 0x967C */
          return 0x7FFB;
        case 0x7D:            /* 0x967D */
          return 0x51E1;
        case 0x7E:            /* 0x967E */
          return 0x76C6;
        case 0x80:            /* 0x9680 */
          return 0x6469;
        case 0x81:            /* 0x9681 */
          return 0x78E8;
        case 0x82:            /* 0x9682 */
          return 0x9B54;
        case 0x83:            /* 0x9683 */
          return 0x9EBB;
        case 0x84:            /* 0x9684 */
          return 0x57CB;
        case 0x85:            /* 0x9685 */
          return 0x59B9;
        case 0x86:            /* 0x9686 */
          return 0x6627;
        case 0x87:            /* 0x9687 */
          return 0x679A;
        case 0x88:            /* 0x9688 */
          return 0x6BCE;
        case 0x89:            /* 0x9689 */
          return 0x54E9;
        case 0x8A:            /* 0x968A */
          return 0x69D9;
        case 0x8B:            /* 0x968B */
          return 0x5E55;
        case 0x8C:            /* 0x968C */
          return 0x819C;
        case 0x8D:            /* 0x968D */
          return 0x6795;
        case 0x8E:            /* 0x968E */
          return 0x9BAA;
        case 0x8F:            /* 0x968F */
          return 0x67FE;
        case 0x90:            /* 0x9690 */
          return 0x9C52;
        case 0x91:            /* 0x9691 */
          return 0x685D;
        case 0x92:            /* 0x9692 */
          return 0x4EA6;
        case 0x93:            /* 0x9693 */
          return 0x4FE3;
        case 0x94:            /* 0x9694 */
          return 0x53C8;
        case 0x95:            /* 0x9695 */
          return 0x62B9;
        case 0x96:            /* 0x9696 */
          return 0x672B;
        case 0x97:            /* 0x9697 */
          return 0x6CAB;
        case 0x98:            /* 0x9698 */
          return 0x8FC4;
        case 0x99:            /* 0x9699 */
          return 0x4FAD;
        case 0x9A:            /* 0x969A */
          return 0x7E6D;
        case 0x9B:            /* 0x969B */
          return 0x9EBF;
        case 0x9C:            /* 0x969C */
          return 0x4E07;
        case 0x9D:            /* 0x969D */
          return 0x6162;
        case 0x9E:            /* 0x969E */
          return 0x6E80;
        case 0x9F:            /* 0x969F */
          return 0x6F2B;
        case 0xA0:            /* 0x96A0 */
          return 0x8513;
        case 0xA1:            /* 0x96A1 */
          return 0x5473;
        case 0xA2:            /* 0x96A2 */
          return 0x672A;
        case 0xA3:            /* 0x96A3 */
          return 0x9B45;
        case 0xA4:            /* 0x96A4 */
          return 0x5DF3;
        case 0xA5:            /* 0x96A5 */
          return 0x7B95;
        case 0xA6:            /* 0x96A6 */
          return 0x5CAC;
        case 0xA7:            /* 0x96A7 */
          return 0x5BC6;
        case 0xA8:            /* 0x96A8 */
          return 0x871C;
        case 0xA9:            /* 0x96A9 */
          return 0x6E4A;
        case 0xAA:            /* 0x96AA */
          return 0x84D1;
        case 0xAB:            /* 0x96AB */
          return 0x7A14;
        case 0xAC:            /* 0x96AC */
          return 0x8108;
        case 0xAD:            /* 0x96AD */
          return 0x5999;
        case 0xAE:            /* 0x96AE */
          return 0x7C8D;
        case 0xAF:            /* 0x96AF */
          return 0x6C11;
        case 0xB0:            /* 0x96B0 */
          return 0x7720;
        case 0xB1:            /* 0x96B1 */
          return 0x52D9;
        case 0xB2:            /* 0x96B2 */
          return 0x5922;
        case 0xB3:            /* 0x96B3 */
          return 0x7121;
        case 0xB4:            /* 0x96B4 */
          return 0x725F;
        case 0xB5:            /* 0x96B5 */
          return 0x77DB;
        case 0xB6:            /* 0x96B6 */
          return 0x9727;
        case 0xB7:            /* 0x96B7 */
          return 0x9D61;
        case 0xB8:            /* 0x96B8 */
          return 0x690B;
        case 0xB9:            /* 0x96B9 */
          return 0x5A7F;
        case 0xBA:            /* 0x96BA */
          return 0x5A18;
        case 0xBB:            /* 0x96BB */
          return 0x51A5;
        case 0xBC:            /* 0x96BC */
          return 0x540D;
        case 0xBD:            /* 0x96BD */
          return 0x547D;
        case 0xBE:            /* 0x96BE */
          return 0x660E;
        case 0xBF:            /* 0x96BF */
          return 0x76DF;
        case 0xC0:            /* 0x96C0 */
          return 0x8FF7;
        case 0xC1:            /* 0x96C1 */
          return 0x9298;
        case 0xC2:            /* 0x96C2 */
          return 0x9CF4;
        case 0xC3:            /* 0x96C3 */
          return 0x59EA;
        case 0xC4:            /* 0x96C4 */
          return 0x725D;
        case 0xC5:            /* 0x96C5 */
          return 0x6EC5;
        case 0xC6:            /* 0x96C6 */
          return 0x514D;
        case 0xC7:            /* 0x96C7 */
          return 0x68C9;
        case 0xC8:            /* 0x96C8 */
          return 0x7DBF;
        case 0xC9:            /* 0x96C9 */
          return 0x7DEC;
        case 0xCA:            /* 0x96CA */
          return 0x9762;
        case 0xCB:            /* 0x96CB */
          return 0x9EBA;
        case 0xCC:            /* 0x96CC */
          return 0x6478;
        case 0xCD:            /* 0x96CD */
          return 0x6A21;
        case 0xCE:            /* 0x96CE */
          return 0x8302;
        case 0xCF:            /* 0x96CF */
          return 0x5984;
        case 0xD0:            /* 0x96D0 */
          return 0x5B5F;
        case 0xD1:            /* 0x96D1 */
          return 0x6BDB;
        case 0xD2:            /* 0x96D2 */
          return 0x731B;
        case 0xD3:            /* 0x96D3 */
          return 0x76F2;
        case 0xD4:            /* 0x96D4 */
          return 0x7DB2;
        case 0xD5:            /* 0x96D5 */
          return 0x8017;
        case 0xD6:            /* 0x96D6 */
          return 0x8499;
        case 0xD7:            /* 0x96D7 */
          return 0x5132;
        case 0xD8:            /* 0x96D8 */
          return 0x6728;
        case 0xD9:            /* 0x96D9 */
          return 0x9ED9;
        case 0xDA:            /* 0x96DA */
          return 0x76EE;
        case 0xDB:            /* 0x96DB */
          return 0x6762;
        case 0xDC:            /* 0x96DC */
          return 0x52FF;
        case 0xDD:            /* 0x96DD */
          return 0x9905;
        case 0xDE:            /* 0x96DE */
          return 0x5C24;
        case 0xDF:            /* 0x96DF */
          return 0x623B;
        case 0xE0:            /* 0x96E0 */
          return 0x7C7E;
        case 0xE1:            /* 0x96E1 */
          return 0x8CB0;
        case 0xE2:            /* 0x96E2 */
          return 0x554F;
        case 0xE3:            /* 0x96E3 */
          return 0x60B6;
        case 0xE4:            /* 0x96E4 */
          return 0x7D0B;
        case 0xE5:            /* 0x96E5 */
          return 0x9580;
        case 0xE6:            /* 0x96E6 */
          return 0x5301;
        case 0xE7:            /* 0x96E7 */
          return 0x4E5F;
        case 0xE8:            /* 0x96E8 */
          return 0x51B6;
        case 0xE9:            /* 0x96E9 */
          return 0x591C;
        case 0xEA:            /* 0x96EA */
          return 0x723A;
        case 0xEB:            /* 0x96EB */
          return 0x8036;
        case 0xEC:            /* 0x96EC */
          return 0x91CE;
        case 0xED:            /* 0x96ED */
          return 0x5F25;
        case 0xEE:            /* 0x96EE */
          return 0x77E2;
        case 0xEF:            /* 0x96EF */
          return 0x5384;
        case 0xF0:            /* 0x96F0 */
          return 0x5F79;
        case 0xF1:            /* 0x96F1 */
          return 0x7D04;
        case 0xF2:            /* 0x96F2 */
          return 0x85AC;
        case 0xF3:            /* 0x96F3 */
          return 0x8A33;
        case 0xF4:            /* 0x96F4 */
          return 0x8E8D;
        case 0xF5:            /* 0x96F5 */
          return 0x9756;
        case 0xF6:            /* 0x96F6 */
          return 0x67F3;
        case 0xF7:            /* 0x96F7 */
          return 0x85AE;
        case 0xF8:            /* 0x96F8 */
          return 0x9453;
        case 0xF9:            /* 0x96F9 */
          return 0x6109;
        case 0xFA:            /* 0x96FA */
          return 0x6108;
        case 0xFB:            /* 0x96FB */
          return 0x6CB9;
        case 0xFC:            /* 0x96FC */
          return 0x7652;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 96 */

    /* Lead Byte: 0x97xx */
    case 0x97:
      switch (low_byte)
      {
        case 0x40:            /* 0x9740 */
          return 0x8AED;
        case 0x41:            /* 0x9741 */
          return 0x8F38;
        case 0x42:            /* 0x9742 */
          return 0x552F;
        case 0x43:            /* 0x9743 */
          return 0x4F51;
        case 0x44:            /* 0x9744 */
          return 0x512A;
        case 0x45:            /* 0x9745 */
          return 0x52C7;
        case 0x46:            /* 0x9746 */
          return 0x53CB;
        case 0x47:            /* 0x9747 */
          return 0x5BA5;
        case 0x48:            /* 0x9748 */
          return 0x5E7D;
        case 0x49:            /* 0x9749 */
          return 0x60A0;
        case 0x4A:            /* 0x974A */
          return 0x6182;
        case 0x4B:            /* 0x974B */
          return 0x63D6;
        case 0x4C:            /* 0x974C */
          return 0x6709;
        case 0x4D:            /* 0x974D */
          return 0x67DA;
        case 0x4E:            /* 0x974E */
          return 0x6E67;
        case 0x4F:            /* 0x974F */
          return 0x6D8C;
        case 0x50:            /* 0x9750 */
          return 0x7336;
        case 0x51:            /* 0x9751 */
          return 0x7337;
        case 0x52:            /* 0x9752 */
          return 0x7531;
        case 0x53:            /* 0x9753 */
          return 0x7950;
        case 0x54:            /* 0x9754 */
          return 0x88D5;
        case 0x55:            /* 0x9755 */
          return 0x8A98;
        case 0x56:            /* 0x9756 */
          return 0x904A;
        case 0x57:            /* 0x9757 */
          return 0x9091;
        case 0x58:            /* 0x9758 */
          return 0x90F5;
        case 0x59:            /* 0x9759 */
          return 0x96C4;
        case 0x5A:            /* 0x975A */
          return 0x878D;
        case 0x5B:            /* 0x975B */
          return 0x5915;
        case 0x5C:            /* 0x975C */
          return 0x4E88;
        case 0x5D:            /* 0x975D */
          return 0x4F59;
        case 0x5E:            /* 0x975E */
          return 0x4E0E;
        case 0x5F:            /* 0x975F */
          return 0x8A89;
        case 0x60:            /* 0x9760 */
          return 0x8F3F;
        case 0x61:            /* 0x9761 */
          return 0x9810;
        case 0x62:            /* 0x9762 */
          return 0x50AD;
        case 0x63:            /* 0x9763 */
          return 0x5E7C;
        case 0x64:            /* 0x9764 */
          return 0x5996;
        case 0x65:            /* 0x9765 */
          return 0x5BB9;
        case 0x66:            /* 0x9766 */
          return 0x5EB8;
        case 0x67:            /* 0x9767 */
          return 0x63DA;
        case 0x68:            /* 0x9768 */
          return 0x63FA;
        case 0x69:            /* 0x9769 */
          return 0x64C1;
        case 0x6A:            /* 0x976A */
          return 0x66DC;
        case 0x6B:            /* 0x976B */
          return 0x694A;
        case 0x6C:            /* 0x976C */
          return 0x69D8;
        case 0x6D:            /* 0x976D */
          return 0x6D0B;
        case 0x6E:            /* 0x976E */
          return 0x6EB6;
        case 0x6F:            /* 0x976F */
          return 0x7194;
        case 0x70:            /* 0x9770 */
          return 0x7528;
        case 0x71:            /* 0x9771 */
          return 0x7AAF;
        case 0x72:            /* 0x9772 */
          return 0x7F8A;
        case 0x73:            /* 0x9773 */
          return 0x8000;
        case 0x74:            /* 0x9774 */
          return 0x8449;
        case 0x75:            /* 0x9775 */
          return 0x84C9;
        case 0x76:            /* 0x9776 */
          return 0x8981;
        case 0x77:            /* 0x9777 */
          return 0x8B21;
        case 0x78:            /* 0x9778 */
          return 0x8E0A;
        case 0x79:            /* 0x9779 */
          return 0x9065;
        case 0x7A:            /* 0x977A */
          return 0x967D;
        case 0x7B:            /* 0x977B */
          return 0x990A;
        case 0x7C:            /* 0x977C */
          return 0x617E;
        case 0x7D:            /* 0x977D */
          return 0x6291;
        case 0x7E:            /* 0x977E */
          return 0x6B32;
        case 0x80:            /* 0x9780 */
          return 0x6C83;
        case 0x81:            /* 0x9781 */
          return 0x6D74;
        case 0x82:            /* 0x9782 */
          return 0x7FCC;
        case 0x83:            /* 0x9783 */
          return 0x7FFC;
        case 0x84:            /* 0x9784 */
          return 0x6DC0;
        case 0x85:            /* 0x9785 */
          return 0x7F85;
        case 0x86:            /* 0x9786 */
          return 0x87BA;
        case 0x87:            /* 0x9787 */
          return 0x88F8;
        case 0x88:            /* 0x9788 */
          return 0x6765;
        case 0x89:            /* 0x9789 */
          return 0x83B1;
        case 0x8A:            /* 0x978A */
          return 0x983C;
        case 0x8B:            /* 0x978B */
          return 0x96F7;
        case 0x8C:            /* 0x978C */
          return 0x6D1B;
        case 0x8D:            /* 0x978D */
          return 0x7D61;
        case 0x8E:            /* 0x978E */
          return 0x843D;
        case 0x8F:            /* 0x978F */
          return 0x916A;
        case 0x90:            /* 0x9790 */
          return 0x4E71;
        case 0x91:            /* 0x9791 */
          return 0x5375;
        case 0x92:            /* 0x9792 */
          return 0x5D50;
        case 0x93:            /* 0x9793 */
          return 0x6B04;
        case 0x94:            /* 0x9794 */
          return 0x6FEB;
        case 0x95:            /* 0x9795 */
          return 0x85CD;
        case 0x96:            /* 0x9796 */
          return 0x862D;
        case 0x97:            /* 0x9797 */
          return 0x89A7;
        case 0x98:            /* 0x9798 */
          return 0x5229;
        case 0x99:            /* 0x9799 */
          return 0x540F;
        case 0x9A:            /* 0x979A */
          return 0x5C65;
        case 0x9B:            /* 0x979B */
          return 0x674E;
        case 0x9C:            /* 0x979C */
          return 0x68A8;
        case 0x9D:            /* 0x979D */
          return 0x7406;
        case 0x9E:            /* 0x979E */
          return 0x7483;
        case 0x9F:            /* 0x979F */
          return 0x75E2;
        case 0xA0:            /* 0x97A0 */
          return 0x88CF;
        case 0xA1:            /* 0x97A1 */
          return 0x88E1;
        case 0xA2:            /* 0x97A2 */
          return 0x91CC;
        case 0xA3:            /* 0x97A3 */
          return 0x96E2;
        case 0xA4:            /* 0x97A4 */
          return 0x9678;
        case 0xA5:            /* 0x97A5 */
          return 0x5F8B;
        case 0xA6:            /* 0x97A6 */
          return 0x7387;
        case 0xA7:            /* 0x97A7 */
          return 0x7ACB;
        case 0xA8:            /* 0x97A8 */
          return 0x844E;
        case 0xA9:            /* 0x97A9 */
          return 0x63A0;
        case 0xAA:            /* 0x97AA */
          return 0x7565;
        case 0xAB:            /* 0x97AB */
          return 0x5289;
        case 0xAC:            /* 0x97AC */
          return 0x6D41;
        case 0xAD:            /* 0x97AD */
          return 0x6E9C;
        case 0xAE:            /* 0x97AE */
          return 0x7409;
        case 0xAF:            /* 0x97AF */
          return 0x7559;
        case 0xB0:            /* 0x97B0 */
          return 0x786B;
        case 0xB1:            /* 0x97B1 */
          return 0x7C92;
        case 0xB2:            /* 0x97B2 */
          return 0x9686;
        case 0xB3:            /* 0x97B3 */
          return 0x7ADC;
        case 0xB4:            /* 0x97B4 */
          return 0x9F8D;
        case 0xB5:            /* 0x97B5 */
          return 0x4FB6;
        case 0xB6:            /* 0x97B6 */
          return 0x616E;
        case 0xB7:            /* 0x97B7 */
          return 0x65C5;
        case 0xB8:            /* 0x97B8 */
          return 0x865C;
        case 0xB9:            /* 0x97B9 */
          return 0x4E86;
        case 0xBA:            /* 0x97BA */
          return 0x4EAE;
        case 0xBB:            /* 0x97BB */
          return 0x50DA;
        case 0xBC:            /* 0x97BC */
          return 0x4E21;
        case 0xBD:            /* 0x97BD */
          return 0x51CC;
        case 0xBE:            /* 0x97BE */
          return 0x5BEE;
        case 0xBF:            /* 0x97BF */
          return 0x6599;
        case 0xC0:            /* 0x97C0 */
          return 0x6881;
        case 0xC1:            /* 0x97C1 */
          return 0x6DBC;
        case 0xC2:            /* 0x97C2 */
          return 0x731F;
        case 0xC3:            /* 0x97C3 */
          return 0x7642;
        case 0xC4:            /* 0x97C4 */
          return 0x77AD;
        case 0xC5:            /* 0x97C5 */
          return 0x7A1C;
        case 0xC6:            /* 0x97C6 */
          return 0x7CE7;
        case 0xC7:            /* 0x97C7 */
          return 0x826F;
        case 0xC8:            /* 0x97C8 */
          return 0x8AD2;
        case 0xC9:            /* 0x97C9 */
          return 0x907C;
        case 0xCA:            /* 0x97CA */
          return 0x91CF;
        case 0xCB:            /* 0x97CB */
          return 0x9675;
        case 0xCC:            /* 0x97CC */
          return 0x9818;
        case 0xCD:            /* 0x97CD */
          return 0x529B;
        case 0xCE:            /* 0x97CE */
          return 0x7DD1;
        case 0xCF:            /* 0x97CF */
          return 0x502B;
        case 0xD0:            /* 0x97D0 */
          return 0x5398;
        case 0xD1:            /* 0x97D1 */
          return 0x6797;
        case 0xD2:            /* 0x97D2 */
          return 0x6DCB;
        case 0xD3:            /* 0x97D3 */
          return 0x71D0;
        case 0xD4:            /* 0x97D4 */
          return 0x7433;
        case 0xD5:            /* 0x97D5 */
          return 0x81E8;
        case 0xD6:            /* 0x97D6 */
          return 0x8F2A;
        case 0xD7:            /* 0x97D7 */
          return 0x96A3;
        case 0xD8:            /* 0x97D8 */
          return 0x9C57;
        case 0xD9:            /* 0x97D9 */
          return 0x9E9F;
        case 0xDA:            /* 0x97DA */
          return 0x7460;
        case 0xDB:            /* 0x97DB */
          return 0x5841;
        case 0xDC:            /* 0x97DC */
          return 0x6D99;
        case 0xDD:            /* 0x97DD */
          return 0x7D2F;
        case 0xDE:            /* 0x97DE */
          return 0x985E;
        case 0xDF:            /* 0x97DF */
          return 0x4EE4;
        case 0xE0:            /* 0x97E0 */
          return 0x4F36;
        case 0xE1:            /* 0x97E1 */
          return 0x4F8B;
        case 0xE2:            /* 0x97E2 */
          return 0x51B7;
        case 0xE3:            /* 0x97E3 */
          return 0x52B1;
        case 0xE4:            /* 0x97E4 */
          return 0x5DBA;
        case 0xE5:            /* 0x97E5 */
          return 0x601C;
        case 0xE6:            /* 0x97E6 */
          return 0x73B2;
        case 0xE7:            /* 0x97E7 */
          return 0x793C;
        case 0xE8:            /* 0x97E8 */
          return 0x82D3;
        case 0xE9:            /* 0x97E9 */
          return 0x9234;
        case 0xEA:            /* 0x97EA */
          return 0x96B7;
        case 0xEB:            /* 0x97EB */
          return 0x96F6;
        case 0xEC:            /* 0x97EC */
          return 0x970A;
        case 0xED:            /* 0x97ED */
          return 0x9E97;
        case 0xEE:            /* 0x97EE */
          return 0x9F62;
        case 0xEF:            /* 0x97EF */
          return 0x66A6;
        case 0xF0:            /* 0x97F0 */
          return 0x6B74;
        case 0xF1:            /* 0x97F1 */
          return 0x5217;
        case 0xF2:            /* 0x97F2 */
          return 0x52A3;
        case 0xF3:            /* 0x97F3 */
          return 0x70C8;
        case 0xF4:            /* 0x97F4 */
          return 0x88C2;
        case 0xF5:            /* 0x97F5 */
          return 0x5EC9;
        case 0xF6:            /* 0x97F6 */
          return 0x604B;
        case 0xF7:            /* 0x97F7 */
          return 0x6190;
        case 0xF8:            /* 0x97F8 */
          return 0x6F23;
        case 0xF9:            /* 0x97F9 */
          return 0x7149;
        case 0xFA:            /* 0x97FA */
          return 0x7C3E;
        case 0xFB:            /* 0x97FB */
          return 0x7DF4;
        case 0xFC:            /* 0x97FC */
          return 0x806F;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 97 */

    /* Lead Byte: 0x98xx */
    case 0x98:
      switch (low_byte)
      {
        case 0x40:            /* 0x9840 */
          return 0x84EE;
        case 0x41:            /* 0x9841 */
          return 0x9023;
        case 0x42:            /* 0x9842 */
          return 0x932C;
        case 0x43:            /* 0x9843 */
          return 0x5442;
        case 0x44:            /* 0x9844 */
          return 0x9B6F;
        case 0x45:            /* 0x9845 */
          return 0x6AD3;
        case 0x46:            /* 0x9846 */
          return 0x7089;
        case 0x47:            /* 0x9847 */
          return 0x8CC2;
        case 0x48:            /* 0x9848 */
          return 0x8DEF;
        case 0x49:            /* 0x9849 */
          return 0x9732;
        case 0x4A:            /* 0x984A */
          return 0x52B4;
        case 0x4B:            /* 0x984B */
          return 0x5A41;
        case 0x4C:            /* 0x984C */
          return 0x5ECA;
        case 0x4D:            /* 0x984D */
          return 0x5F04;
        case 0x4E:            /* 0x984E */
          return 0x6717;
        case 0x4F:            /* 0x984F */
          return 0x697C;
        case 0x50:            /* 0x9850 */
          return 0x6994;
        case 0x51:            /* 0x9851 */
          return 0x6D6A;
        case 0x52:            /* 0x9852 */
          return 0x6F0F;
        case 0x53:            /* 0x9853 */
          return 0x7262;
        case 0x54:            /* 0x9854 */
          return 0x72FC;
        case 0x55:            /* 0x9855 */
          return 0x7BED;
        case 0x56:            /* 0x9856 */
          return 0x8001;
        case 0x57:            /* 0x9857 */
          return 0x807E;
        case 0x58:            /* 0x9858 */
          return 0x874B;
        case 0x59:            /* 0x9859 */
          return 0x90CE;
        case 0x5A:            /* 0x985A */
          return 0x516D;
        case 0x5B:            /* 0x985B */
          return 0x9E93;
        case 0x5C:            /* 0x985C */
          return 0x7984;
        case 0x5D:            /* 0x985D */
          return 0x808B;
        case 0x5E:            /* 0x985E */
          return 0x9332;
        case 0x5F:            /* 0x985F */
          return 0x8AD6;
        case 0x60:            /* 0x9860 */
          return 0x502D;
        case 0x61:            /* 0x9861 */
          return 0x548C;
        case 0x62:            /* 0x9862 */
          return 0x8A71;
        case 0x63:            /* 0x9863 */
          return 0x6B6A;
        case 0x64:            /* 0x9864 */
          return 0x8CC4;
        case 0x65:            /* 0x9865 */
          return 0x8107;
        case 0x66:            /* 0x9866 */
          return 0x60D1;
        case 0x67:            /* 0x9867 */
          return 0x67A0;
        case 0x68:            /* 0x9868 */
          return 0x9DF2;
        case 0x69:            /* 0x9869 */
          return 0x4E99;
        case 0x6A:            /* 0x986A */
          return 0x4E98;
        case 0x6B:            /* 0x986B */
          return 0x9C10;
        case 0x6C:            /* 0x986C */
          return 0x8A6B;
        case 0x6D:            /* 0x986D */
          return 0x85C1;
        case 0x6E:            /* 0x986E */
          return 0x8568;
        case 0x6F:            /* 0x986F */
          return 0x6900;
        case 0x70:            /* 0x9870 */
          return 0x6E7E;
        case 0x71:            /* 0x9871 */
          return 0x7897;
        case 0x72:            /* 0x9872 */
          return 0x8155;
        case 0x9F:            /* 0x989F */
          return 0x5F0C;
        case 0xA0:            /* 0x98A0 */
          return 0x4E10;
        case 0xA1:            /* 0x98A1 */
          return 0x4E15;
        case 0xA2:            /* 0x98A2 */
          return 0x4E2A;
        case 0xA3:            /* 0x98A3 */
          return 0x4E31;
        case 0xA4:            /* 0x98A4 */
          return 0x4E36;
        case 0xA5:            /* 0x98A5 */
          return 0x4E3C;
        case 0xA6:            /* 0x98A6 */
          return 0x4E3F;
        case 0xA7:            /* 0x98A7 */
          return 0x4E42;
        case 0xA8:            /* 0x98A8 */
          return 0x4E56;
        case 0xA9:            /* 0x98A9 */
          return 0x4E58;
        case 0xAA:            /* 0x98AA */
          return 0x4E82;
        case 0xAB:            /* 0x98AB */
          return 0x4E85;
        case 0xAC:            /* 0x98AC */
          return 0x8C6B;
        case 0xAD:            /* 0x98AD */
          return 0x4E8A;
        case 0xAE:            /* 0x98AE */
          return 0x8212;
        case 0xAF:            /* 0x98AF */
          return 0x5F0D;
        case 0xB0:            /* 0x98B0 */
          return 0x4E8E;
        case 0xB1:            /* 0x98B1 */
          return 0x4E9E;
        case 0xB2:            /* 0x98B2 */
          return 0x4E9F;
        case 0xB3:            /* 0x98B3 */
          return 0x4EA0;
        case 0xB4:            /* 0x98B4 */
          return 0x4EA2;
        case 0xB5:            /* 0x98B5 */
          return 0x4EB0;
        case 0xB6:            /* 0x98B6 */
          return 0x4EB3;
        case 0xB7:            /* 0x98B7 */
          return 0x4EB6;
        case 0xB8:            /* 0x98B8 */
          return 0x4ECE;
        case 0xB9:            /* 0x98B9 */
          return 0x4ECD;
        case 0xBA:            /* 0x98BA */
          return 0x4EC4;
        case 0xBB:            /* 0x98BB */
          return 0x4EC6;
        case 0xBC:            /* 0x98BC */
          return 0x4EC2;
        case 0xBD:            /* 0x98BD */
          return 0x4ED7;
        case 0xBE:            /* 0x98BE */
          return 0x4EDE;
        case 0xBF:            /* 0x98BF */
          return 0x4EED;
        case 0xC0:            /* 0x98C0 */
          return 0x4EDF;
        case 0xC1:            /* 0x98C1 */
          return 0x4EF7;
        case 0xC2:            /* 0x98C2 */
          return 0x4F09;
        case 0xC3:            /* 0x98C3 */
          return 0x4F5A;
        case 0xC4:            /* 0x98C4 */
          return 0x4F30;
        case 0xC5:            /* 0x98C5 */
          return 0x4F5B;
        case 0xC6:            /* 0x98C6 */
          return 0x4F5D;
        case 0xC7:            /* 0x98C7 */
          return 0x4F57;
        case 0xC8:            /* 0x98C8 */
          return 0x4F47;
        case 0xC9:            /* 0x98C9 */
          return 0x4F76;
        case 0xCA:            /* 0x98CA */
          return 0x4F88;
        case 0xCB:            /* 0x98CB */
          return 0x4F8F;
        case 0xCC:            /* 0x98CC */
          return 0x4F98;
        case 0xCD:            /* 0x98CD */
          return 0x4F7B;
        case 0xCE:            /* 0x98CE */
          return 0x4F69;
        case 0xCF:            /* 0x98CF */
          return 0x4F70;
        case 0xD0:            /* 0x98D0 */
          return 0x4F91;
        case 0xD1:            /* 0x98D1 */
          return 0x4F6F;
        case 0xD2:            /* 0x98D2 */
          return 0x4F86;
        case 0xD3:            /* 0x98D3 */
          return 0x4F96;
        case 0xD4:            /* 0x98D4 */
          return 0x5118;
        case 0xD5:            /* 0x98D5 */
          return 0x4FD4;
        case 0xD6:            /* 0x98D6 */
          return 0x4FDF;
        case 0xD7:            /* 0x98D7 */
          return 0x4FCE;
        case 0xD8:            /* 0x98D8 */
          return 0x4FD8;
        case 0xD9:            /* 0x98D9 */
          return 0x4FDB;
        case 0xDA:            /* 0x98DA */
          return 0x4FD1;
        case 0xDB:            /* 0x98DB */
          return 0x4FDA;
        case 0xDC:            /* 0x98DC */
          return 0x4FD0;
        case 0xDD:            /* 0x98DD */
          return 0x4FE4;
        case 0xDE:            /* 0x98DE */
          return 0x4FE5;
        case 0xDF:            /* 0x98DF */
          return 0x501A;
        case 0xE0:            /* 0x98E0 */
          return 0x5028;
        case 0xE1:            /* 0x98E1 */
          return 0x5014;
        case 0xE2:            /* 0x98E2 */
          return 0x502A;
        case 0xE3:            /* 0x98E3 */
          return 0x5025;
        case 0xE4:            /* 0x98E4 */
          return 0x5005;
        case 0xE5:            /* 0x98E5 */
          return 0x4F1C;
        case 0xE6:            /* 0x98E6 */
          return 0x4FF6;
        case 0xE7:            /* 0x98E7 */
          return 0x5021;
        case 0xE8:            /* 0x98E8 */
          return 0x5029;
        case 0xE9:            /* 0x98E9 */
          return 0x502C;
        case 0xEA:            /* 0x98EA */
          return 0x4FFE;
        case 0xEB:            /* 0x98EB */
          return 0x4FEF;
        case 0xEC:            /* 0x98EC */
          return 0x5011;
        case 0xED:            /* 0x98ED */
          return 0x5006;
        case 0xEE:            /* 0x98EE */
          return 0x5043;
        case 0xEF:            /* 0x98EF */
          return 0x5047;
        case 0xF0:            /* 0x98F0 */
          return 0x6703;
        case 0xF1:            /* 0x98F1 */
          return 0x5055;
        case 0xF2:            /* 0x98F2 */
          return 0x5050;
        case 0xF3:            /* 0x98F3 */
          return 0x5048;
        case 0xF4:            /* 0x98F4 */
          return 0x505A;
        case 0xF5:            /* 0x98F5 */
          return 0x5056;
        case 0xF6:            /* 0x98F6 */
          return 0x506C;
        case 0xF7:            /* 0x98F7 */
          return 0x5078;
        case 0xF8:            /* 0x98F8 */
          return 0x5080;
        case 0xF9:            /* 0x98F9 */
          return 0x509A;
        case 0xFA:            /* 0x98FA */
          return 0x5085;
        case 0xFB:            /* 0x98FB */
          return 0x50B4;
        case 0xFC:            /* 0x98FC */
          return 0x50B2;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 98 */

    /* Lead Byte: 0x99xx */
    case 0x99:
      switch (low_byte)
      {
        case 0x40:            /* 0x9940 */
          return 0x50C9;
        case 0x41:            /* 0x9941 */
          return 0x50CA;
        case 0x42:            /* 0x9942 */
          return 0x50B3;
        case 0x43:            /* 0x9943 */
          return 0x50C2;
        case 0x44:            /* 0x9944 */
          return 0x50D6;
        case 0x45:            /* 0x9945 */
          return 0x50DE;
        case 0x46:            /* 0x9946 */
          return 0x50E5;
        case 0x47:            /* 0x9947 */
          return 0x50ED;
        case 0x48:            /* 0x9948 */
          return 0x50E3;
        case 0x49:            /* 0x9949 */
          return 0x50EE;
        case 0x4A:            /* 0x994A */
          return 0x50F9;
        case 0x4B:            /* 0x994B */
          return 0x50F5;
        case 0x4C:            /* 0x994C */
          return 0x5109;
        case 0x4D:            /* 0x994D */
          return 0x5101;
        case 0x4E:            /* 0x994E */
          return 0x5102;
        case 0x4F:            /* 0x994F */
          return 0x5116;
        case 0x50:            /* 0x9950 */
          return 0x5115;
        case 0x51:            /* 0x9951 */
          return 0x5114;
        case 0x52:            /* 0x9952 */
          return 0x511A;
        case 0x53:            /* 0x9953 */
          return 0x5121;
        case 0x54:            /* 0x9954 */
          return 0x513A;
        case 0x55:            /* 0x9955 */
          return 0x5137;
        case 0x56:            /* 0x9956 */
          return 0x513C;
        case 0x57:            /* 0x9957 */
          return 0x513B;
        case 0x58:            /* 0x9958 */
          return 0x513F;
        case 0x59:            /* 0x9959 */
          return 0x5140;
        case 0x5A:            /* 0x995A */
          return 0x5152;
        case 0x5B:            /* 0x995B */
          return 0x514C;
        case 0x5C:            /* 0x995C */
          return 0x5154;
        case 0x5D:            /* 0x995D */
          return 0x5162;
        case 0x5E:            /* 0x995E */
          return 0x7AF8;
        case 0x5F:            /* 0x995F */
          return 0x5169;
        case 0x60:            /* 0x9960 */
          return 0x516A;
        case 0x61:            /* 0x9961 */
          return 0x516E;
        case 0x62:            /* 0x9962 */
          return 0x5180;
        case 0x63:            /* 0x9963 */
          return 0x5182;
        case 0x64:            /* 0x9964 */
          return 0x56D8;
        case 0x65:            /* 0x9965 */
          return 0x518C;
        case 0x66:            /* 0x9966 */
          return 0x5189;
        case 0x67:            /* 0x9967 */
          return 0x518F;
        case 0x68:            /* 0x9968 */
          return 0x5191;
        case 0x69:            /* 0x9969 */
          return 0x5193;
        case 0x6A:            /* 0x996A */
          return 0x5195;
        case 0x6B:            /* 0x996B */
          return 0x5196;
        case 0x6C:            /* 0x996C */
          return 0x51A4;
        case 0x6D:            /* 0x996D */
          return 0x51A6;
        case 0x6E:            /* 0x996E */
          return 0x51A2;
        case 0x6F:            /* 0x996F */
          return 0x51A9;
        case 0x70:            /* 0x9970 */
          return 0x51AA;
        case 0x71:            /* 0x9971 */
          return 0x51AB;
        case 0x72:            /* 0x9972 */
          return 0x51B3;
        case 0x73:            /* 0x9973 */
          return 0x51B1;
        case 0x74:            /* 0x9974 */
          return 0x51B2;
        case 0x75:            /* 0x9975 */
          return 0x51B0;
        case 0x76:            /* 0x9976 */
          return 0x51B5;
        case 0x77:            /* 0x9977 */
          return 0x51BD;
        case 0x78:            /* 0x9978 */
          return 0x51C5;
        case 0x79:            /* 0x9979 */
          return 0x51C9;
        case 0x7A:            /* 0x997A */
          return 0x51DB;
        case 0x7B:            /* 0x997B */
          return 0x51E0;
        case 0x7C:            /* 0x997C */
          return 0x8655;
        case 0x7D:            /* 0x997D */
          return 0x51E9;
        case 0x7E:            /* 0x997E */
          return 0x51ED;
        case 0x80:            /* 0x9980 */
          return 0x51F0;
        case 0x81:            /* 0x9981 */
          return 0x51F5;
        case 0x82:            /* 0x9982 */
          return 0x51FE;
        case 0x83:            /* 0x9983 */
          return 0x5204;
        case 0x84:            /* 0x9984 */
          return 0x520B;
        case 0x85:            /* 0x9985 */
          return 0x5214;
        case 0x86:            /* 0x9986 */
          return 0x520E;
        case 0x87:            /* 0x9987 */
          return 0x5227;
        case 0x88:            /* 0x9988 */
          return 0x522A;
        case 0x89:            /* 0x9989 */
          return 0x522E;
        case 0x8A:            /* 0x998A */
          return 0x5233;
        case 0x8B:            /* 0x998B */
          return 0x5239;
        case 0x8C:            /* 0x998C */
          return 0x524F;
        case 0x8D:            /* 0x998D */
          return 0x5244;
        case 0x8E:            /* 0x998E */
          return 0x524B;
        case 0x8F:            /* 0x998F */
          return 0x524C;
        case 0x90:            /* 0x9990 */
          return 0x525E;
        case 0x91:            /* 0x9991 */
          return 0x5254;
        case 0x92:            /* 0x9992 */
          return 0x526A;
        case 0x93:            /* 0x9993 */
          return 0x5274;
        case 0x94:            /* 0x9994 */
          return 0x5269;
        case 0x95:            /* 0x9995 */
          return 0x5273;
        case 0x96:            /* 0x9996 */
          return 0x527F;
        case 0x97:            /* 0x9997 */
          return 0x527D;
        case 0x98:            /* 0x9998 */
          return 0x528D;
        case 0x99:            /* 0x9999 */
          return 0x5294;
        case 0x9A:            /* 0x999A */
          return 0x5292;
        case 0x9B:            /* 0x999B */
          return 0x5271;
        case 0x9C:            /* 0x999C */
          return 0x5288;
        case 0x9D:            /* 0x999D */
          return 0x5291;
        case 0x9E:            /* 0x999E */
          return 0x8FA8;
        case 0x9F:            /* 0x999F */
          return 0x8FA7;
        case 0xA0:            /* 0x99A0 */
          return 0x52AC;
        case 0xA1:            /* 0x99A1 */
          return 0x52AD;
        case 0xA2:            /* 0x99A2 */
          return 0x52BC;
        case 0xA3:            /* 0x99A3 */
          return 0x52B5;
        case 0xA4:            /* 0x99A4 */
          return 0x52C1;
        case 0xA5:            /* 0x99A5 */
          return 0x52CD;
        case 0xA6:            /* 0x99A6 */
          return 0x52D7;
        case 0xA7:            /* 0x99A7 */
          return 0x52DE;
        case 0xA8:            /* 0x99A8 */
          return 0x52E3;
        case 0xA9:            /* 0x99A9 */
          return 0x52E6;
        case 0xAA:            /* 0x99AA */
          return 0x98ED;
        case 0xAB:            /* 0x99AB */
          return 0x52E0;
        case 0xAC:            /* 0x99AC */
          return 0x52F3;
        case 0xAD:            /* 0x99AD */
          return 0x52F5;
        case 0xAE:            /* 0x99AE */
          return 0x52F8;
        case 0xAF:            /* 0x99AF */
          return 0x52F9;
        case 0xB0:            /* 0x99B0 */
          return 0x5306;
        case 0xB1:            /* 0x99B1 */
          return 0x5308;
        case 0xB2:            /* 0x99B2 */
          return 0x7538;
        case 0xB3:            /* 0x99B3 */
          return 0x530D;
        case 0xB4:            /* 0x99B4 */
          return 0x5310;
        case 0xB5:            /* 0x99B5 */
          return 0x530F;
        case 0xB6:            /* 0x99B6 */
          return 0x5315;
        case 0xB7:            /* 0x99B7 */
          return 0x531A;
        case 0xB8:            /* 0x99B8 */
          return 0x5323;
        case 0xB9:            /* 0x99B9 */
          return 0x532F;
        case 0xBA:            /* 0x99BA */
          return 0x5331;
        case 0xBB:            /* 0x99BB */
          return 0x5333;
        case 0xBC:            /* 0x99BC */
          return 0x5338;
        case 0xBD:            /* 0x99BD */
          return 0x5340;
        case 0xBE:            /* 0x99BE */
          return 0x5346;
        case 0xBF:            /* 0x99BF */
          return 0x5345;
        case 0xC0:            /* 0x99C0 */
          return 0x4E17;
        case 0xC1:            /* 0x99C1 */
          return 0x5349;
        case 0xC2:            /* 0x99C2 */
          return 0x534D;
        case 0xC3:            /* 0x99C3 */
          return 0x51D6;
        case 0xC4:            /* 0x99C4 */
          return 0x535E;
        case 0xC5:            /* 0x99C5 */
          return 0x5369;
        case 0xC6:            /* 0x99C6 */
          return 0x536E;
        case 0xC7:            /* 0x99C7 */
          return 0x5918;
        case 0xC8:            /* 0x99C8 */
          return 0x537B;
        case 0xC9:            /* 0x99C9 */
          return 0x5377;
        case 0xCA:            /* 0x99CA */
          return 0x5382;
        case 0xCB:            /* 0x99CB */
          return 0x5396;
        case 0xCC:            /* 0x99CC */
          return 0x53A0;
        case 0xCD:            /* 0x99CD */
          return 0x53A6;
        case 0xCE:            /* 0x99CE */
          return 0x53A5;
        case 0xCF:            /* 0x99CF */
          return 0x53AE;
        case 0xD0:            /* 0x99D0 */
          return 0x53B0;
        case 0xD1:            /* 0x99D1 */
          return 0x53B6;
        case 0xD2:            /* 0x99D2 */
          return 0x53C3;
        case 0xD3:            /* 0x99D3 */
          return 0x7C12;
        case 0xD4:            /* 0x99D4 */
          return 0x96D9;
        case 0xD5:            /* 0x99D5 */
          return 0x53DF;
        case 0xD6:            /* 0x99D6 */
          return 0x66FC;
        case 0xD7:            /* 0x99D7 */
          return 0x71EE;
        case 0xD8:            /* 0x99D8 */
          return 0x53EE;
        case 0xD9:            /* 0x99D9 */
          return 0x53E8;
        case 0xDA:            /* 0x99DA */
          return 0x53ED;
        case 0xDB:            /* 0x99DB */
          return 0x53FA;
        case 0xDC:            /* 0x99DC */
          return 0x5401;
        case 0xDD:            /* 0x99DD */
          return 0x543D;
        case 0xDE:            /* 0x99DE */
          return 0x5440;
        case 0xDF:            /* 0x99DF */
          return 0x542C;
        case 0xE0:            /* 0x99E0 */
          return 0x542D;
        case 0xE1:            /* 0x99E1 */
          return 0x543C;
        case 0xE2:            /* 0x99E2 */
          return 0x542E;
        case 0xE3:            /* 0x99E3 */
          return 0x5436;
        case 0xE4:            /* 0x99E4 */
          return 0x5429;
        case 0xE5:            /* 0x99E5 */
          return 0x541D;
        case 0xE6:            /* 0x99E6 */
          return 0x544E;
        case 0xE7:            /* 0x99E7 */
          return 0x548F;
        case 0xE8:            /* 0x99E8 */
          return 0x5475;
        case 0xE9:            /* 0x99E9 */
          return 0x548E;
        case 0xEA:            /* 0x99EA */
          return 0x545F;
        case 0xEB:            /* 0x99EB */
          return 0x5471;
        case 0xEC:            /* 0x99EC */
          return 0x5477;
        case 0xED:            /* 0x99ED */
          return 0x5470;
        case 0xEE:            /* 0x99EE */
          return 0x5492;
        case 0xEF:            /* 0x99EF */
          return 0x547B;
        case 0xF0:            /* 0x99F0 */
          return 0x5480;
        case 0xF1:            /* 0x99F1 */
          return 0x5476;
        case 0xF2:            /* 0x99F2 */
          return 0x5484;
        case 0xF3:            /* 0x99F3 */
          return 0x5490;
        case 0xF4:            /* 0x99F4 */
          return 0x5486;
        case 0xF5:            /* 0x99F5 */
          return 0x54C7;
        case 0xF6:            /* 0x99F6 */
          return 0x54A2;
        case 0xF7:            /* 0x99F7 */
          return 0x54B8;
        case 0xF8:            /* 0x99F8 */
          return 0x54A5;
        case 0xF9:            /* 0x99F9 */
          return 0x54AC;
        case 0xFA:            /* 0x99FA */
          return 0x54C4;
        case 0xFB:            /* 0x99FB */
          return 0x54C8;
        case 0xFC:            /* 0x99FC */
          return 0x54A8;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 99 */

    /* Lead Byte: 0x9Axx */
    case 0x9A:
      switch (low_byte)
      {
        case 0x40:            /* 0x9A40 */
          return 0x54AB;
        case 0x41:            /* 0x9A41 */
          return 0x54C2;
        case 0x42:            /* 0x9A42 */
          return 0x54A4;
        case 0x43:            /* 0x9A43 */
          return 0x54BE;
        case 0x44:            /* 0x9A44 */
          return 0x54BC;
        case 0x45:            /* 0x9A45 */
          return 0x54D8;
        case 0x46:            /* 0x9A46 */
          return 0x54E5;
        case 0x47:            /* 0x9A47 */
          return 0x54E6;
        case 0x48:            /* 0x9A48 */
          return 0x550F;
        case 0x49:            /* 0x9A49 */
          return 0x5514;
        case 0x4A:            /* 0x9A4A */
          return 0x54FD;
        case 0x4B:            /* 0x9A4B */
          return 0x54EE;
        case 0x4C:            /* 0x9A4C */
          return 0x54ED;
        case 0x4D:            /* 0x9A4D */
          return 0x54FA;
        case 0x4E:            /* 0x9A4E */
          return 0x54E2;
        case 0x4F:            /* 0x9A4F */
          return 0x5539;
        case 0x50:            /* 0x9A50 */
          return 0x5540;
        case 0x51:            /* 0x9A51 */
          return 0x5563;
        case 0x52:            /* 0x9A52 */
          return 0x554C;
        case 0x53:            /* 0x9A53 */
          return 0x552E;
        case 0x54:            /* 0x9A54 */
          return 0x555C;
        case 0x55:            /* 0x9A55 */
          return 0x5545;
        case 0x56:            /* 0x9A56 */
          return 0x5556;
        case 0x57:            /* 0x9A57 */
          return 0x5557;
        case 0x58:            /* 0x9A58 */
          return 0x5538;
        case 0x59:            /* 0x9A59 */
          return 0x5533;
        case 0x5A:            /* 0x9A5A */
          return 0x555D;
        case 0x5B:            /* 0x9A5B */
          return 0x5599;
        case 0x5C:            /* 0x9A5C */
          return 0x5580;
        case 0x5D:            /* 0x9A5D */
          return 0x54AF;
        case 0x5E:            /* 0x9A5E */
          return 0x558A;
        case 0x5F:            /* 0x9A5F */
          return 0x559F;
        case 0x60:            /* 0x9A60 */
          return 0x557B;
        case 0x61:            /* 0x9A61 */
          return 0x557E;
        case 0x62:            /* 0x9A62 */
          return 0x5598;
        case 0x63:            /* 0x9A63 */
          return 0x559E;
        case 0x64:            /* 0x9A64 */
          return 0x55AE;
        case 0x65:            /* 0x9A65 */
          return 0x557C;
        case 0x66:            /* 0x9A66 */
          return 0x5583;
        case 0x67:            /* 0x9A67 */
          return 0x55A9;
        case 0x68:            /* 0x9A68 */
          return 0x5587;
        case 0x69:            /* 0x9A69 */
          return 0x55A8;
        case 0x6A:            /* 0x9A6A */
          return 0x55DA;
        case 0x6B:            /* 0x9A6B */
          return 0x55C5;
        case 0x6C:            /* 0x9A6C */
          return 0x55DF;
        case 0x6D:            /* 0x9A6D */
          return 0x55C4;
        case 0x6E:            /* 0x9A6E */
          return 0x55DC;
        case 0x6F:            /* 0x9A6F */
          return 0x55E4;
        case 0x70:            /* 0x9A70 */
          return 0x55D4;
        case 0x71:            /* 0x9A71 */
          return 0x5614;
        case 0x72:            /* 0x9A72 */
          return 0x55F7;
        case 0x73:            /* 0x9A73 */
          return 0x5616;
        case 0x74:            /* 0x9A74 */
          return 0x55FE;
        case 0x75:            /* 0x9A75 */
          return 0x55FD;
        case 0x76:            /* 0x9A76 */
          return 0x561B;
        case 0x77:            /* 0x9A77 */
          return 0x55F9;
        case 0x78:            /* 0x9A78 */
          return 0x564E;
        case 0x79:            /* 0x9A79 */
          return 0x5650;
        case 0x7A:            /* 0x9A7A */
          return 0x71DF;
        case 0x7B:            /* 0x9A7B */
          return 0x5634;
        case 0x7C:            /* 0x9A7C */
          return 0x5636;
        case 0x7D:            /* 0x9A7D */
          return 0x5632;
        case 0x7E:            /* 0x9A7E */
          return 0x5638;
        case 0x80:            /* 0x9A80 */
          return 0x566B;
        case 0x81:            /* 0x9A81 */
          return 0x5664;
        case 0x82:            /* 0x9A82 */
          return 0x562F;
        case 0x83:            /* 0x9A83 */
          return 0x566C;
        case 0x84:            /* 0x9A84 */
          return 0x566A;
        case 0x85:            /* 0x9A85 */
          return 0x5686;
        case 0x86:            /* 0x9A86 */
          return 0x5680;
        case 0x87:            /* 0x9A87 */
          return 0x568A;
        case 0x88:            /* 0x9A88 */
          return 0x56A0;
        case 0x89:            /* 0x9A89 */
          return 0x5694;
        case 0x8A:            /* 0x9A8A */
          return 0x568F;
        case 0x8B:            /* 0x9A8B */
          return 0x56A5;
        case 0x8C:            /* 0x9A8C */
          return 0x56AE;
        case 0x8D:            /* 0x9A8D */
          return 0x56B6;
        case 0x8E:            /* 0x9A8E */
          return 0x56B4;
        case 0x8F:            /* 0x9A8F */
          return 0x56C2;
        case 0x90:            /* 0x9A90 */
          return 0x56BC;
        case 0x91:            /* 0x9A91 */
          return 0x56C1;
        case 0x92:            /* 0x9A92 */
          return 0x56C3;
        case 0x93:            /* 0x9A93 */
          return 0x56C0;
        case 0x94:            /* 0x9A94 */
          return 0x56C8;
        case 0x95:            /* 0x9A95 */
          return 0x56CE;
        case 0x96:            /* 0x9A96 */
          return 0x56D1;
        case 0x97:            /* 0x9A97 */
          return 0x56D3;
        case 0x98:            /* 0x9A98 */
          return 0x56D7;
        case 0x99:            /* 0x9A99 */
          return 0x56EE;
        case 0x9A:            /* 0x9A9A */
          return 0x56F9;
        case 0x9B:            /* 0x9A9B */
          return 0x5700;
        case 0x9C:            /* 0x9A9C */
          return 0x56FF;
        case 0x9D:            /* 0x9A9D */
          return 0x5704;
        case 0x9E:            /* 0x9A9E */
          return 0x5709;
        case 0x9F:            /* 0x9A9F */
          return 0x5708;
        case 0xA0:            /* 0x9AA0 */
          return 0x570B;
        case 0xA1:            /* 0x9AA1 */
          return 0x570D;
        case 0xA2:            /* 0x9AA2 */
          return 0x5713;
        case 0xA3:            /* 0x9AA3 */
          return 0x5718;
        case 0xA4:            /* 0x9AA4 */
          return 0x5716;
        case 0xA5:            /* 0x9AA5 */
          return 0x55C7;
        case 0xA6:            /* 0x9AA6 */
          return 0x571C;
        case 0xA7:            /* 0x9AA7 */
          return 0x5726;
        case 0xA8:            /* 0x9AA8 */
          return 0x5737;
        case 0xA9:            /* 0x9AA9 */
          return 0x5738;
        case 0xAA:            /* 0x9AAA */
          return 0x574E;
        case 0xAB:            /* 0x9AAB */
          return 0x573B;
        case 0xAC:            /* 0x9AAC */
          return 0x5740;
        case 0xAD:            /* 0x9AAD */
          return 0x574F;
        case 0xAE:            /* 0x9AAE */
          return 0x5769;
        case 0xAF:            /* 0x9AAF */
          return 0x57C0;
        case 0xB0:            /* 0x9AB0 */
          return 0x5788;
        case 0xB1:            /* 0x9AB1 */
          return 0x5761;
        case 0xB2:            /* 0x9AB2 */
          return 0x577F;
        case 0xB3:            /* 0x9AB3 */
          return 0x5789;
        case 0xB4:            /* 0x9AB4 */
          return 0x5793;
        case 0xB5:            /* 0x9AB5 */
          return 0x57A0;
        case 0xB6:            /* 0x9AB6 */
          return 0x57B3;
        case 0xB7:            /* 0x9AB7 */
          return 0x57A4;
        case 0xB8:            /* 0x9AB8 */
          return 0x57AA;
        case 0xB9:            /* 0x9AB9 */
          return 0x57B0;
        case 0xBA:            /* 0x9ABA */
          return 0x57C3;
        case 0xBB:            /* 0x9ABB */
          return 0x57C6;
        case 0xBC:            /* 0x9ABC */
          return 0x57D4;
        case 0xBD:            /* 0x9ABD */
          return 0x57D2;
        case 0xBE:            /* 0x9ABE */
          return 0x57D3;
        case 0xBF:            /* 0x9ABF */
          return 0x580A;
        case 0xC0:            /* 0x9AC0 */
          return 0x57D6;
        case 0xC1:            /* 0x9AC1 */
          return 0x57E3;
        case 0xC2:            /* 0x9AC2 */
          return 0x580B;
        case 0xC3:            /* 0x9AC3 */
          return 0x5819;
        case 0xC4:            /* 0x9AC4 */
          return 0x581D;
        case 0xC5:            /* 0x9AC5 */
          return 0x5872;
        case 0xC6:            /* 0x9AC6 */
          return 0x5821;
        case 0xC7:            /* 0x9AC7 */
          return 0x5862;
        case 0xC8:            /* 0x9AC8 */
          return 0x584B;
        case 0xC9:            /* 0x9AC9 */
          return 0x5870;
        case 0xCA:            /* 0x9ACA */
          return 0x6BC0;
        case 0xCB:            /* 0x9ACB */
          return 0x5852;
        case 0xCC:            /* 0x9ACC */
          return 0x583D;
        case 0xCD:            /* 0x9ACD */
          return 0x5879;
        case 0xCE:            /* 0x9ACE */
          return 0x5885;
        case 0xCF:            /* 0x9ACF */
          return 0x58B9;
        case 0xD0:            /* 0x9AD0 */
          return 0x589F;
        case 0xD1:            /* 0x9AD1 */
          return 0x58AB;
        case 0xD2:            /* 0x9AD2 */
          return 0x58BA;
        case 0xD3:            /* 0x9AD3 */
          return 0x58DE;
        case 0xD4:            /* 0x9AD4 */
          return 0x58BB;
        case 0xD5:            /* 0x9AD5 */
          return 0x58B8;
        case 0xD6:            /* 0x9AD6 */
          return 0x58AE;
        case 0xD7:            /* 0x9AD7 */
          return 0x58C5;
        case 0xD8:            /* 0x9AD8 */
          return 0x58D3;
        case 0xD9:            /* 0x9AD9 */
          return 0x58D1;
        case 0xDA:            /* 0x9ADA */
          return 0x58D7;
        case 0xDB:            /* 0x9ADB */
          return 0x58D9;
        case 0xDC:            /* 0x9ADC */
          return 0x58D8;
        case 0xDD:            /* 0x9ADD */
          return 0x58E5;
        case 0xDE:            /* 0x9ADE */
          return 0x58DC;
        case 0xDF:            /* 0x9ADF */
          return 0x58E4;
        case 0xE0:            /* 0x9AE0 */
          return 0x58DF;
        case 0xE1:            /* 0x9AE1 */
          return 0x58EF;
        case 0xE2:            /* 0x9AE2 */
          return 0x58FA;
        case 0xE3:            /* 0x9AE3 */
          return 0x58F9;
        case 0xE4:            /* 0x9AE4 */
          return 0x58FB;
        case 0xE5:            /* 0x9AE5 */
          return 0x58FC;
        case 0xE6:            /* 0x9AE6 */
          return 0x58FD;
        case 0xE7:            /* 0x9AE7 */
          return 0x5902;
        case 0xE8:            /* 0x9AE8 */
          return 0x590A;
        case 0xE9:            /* 0x9AE9 */
          return 0x5910;
        case 0xEA:            /* 0x9AEA */
          return 0x591B;
        case 0xEB:            /* 0x9AEB */
          return 0x68A6;
        case 0xEC:            /* 0x9AEC */
          return 0x5925;
        case 0xED:            /* 0x9AED */
          return 0x592C;
        case 0xEE:            /* 0x9AEE */
          return 0x592D;
        case 0xEF:            /* 0x9AEF */
          return 0x5932;
        case 0xF0:            /* 0x9AF0 */
          return 0x5938;
        case 0xF1:            /* 0x9AF1 */
          return 0x593E;
        case 0xF2:            /* 0x9AF2 */
          return 0x7AD2;
        case 0xF3:            /* 0x9AF3 */
          return 0x5955;
        case 0xF4:            /* 0x9AF4 */
          return 0x5950;
        case 0xF5:            /* 0x9AF5 */
          return 0x594E;
        case 0xF6:            /* 0x9AF6 */
          return 0x595A;
        case 0xF7:            /* 0x9AF7 */
          return 0x5958;
        case 0xF8:            /* 0x9AF8 */
          return 0x5962;
        case 0xF9:            /* 0x9AF9 */
          return 0x5960;
        case 0xFA:            /* 0x9AFA */
          return 0x5967;
        case 0xFB:            /* 0x9AFB */
          return 0x596C;
        case 0xFC:            /* 0x9AFC */
          return 0x5969;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 9A */

    /* Lead Byte: 0x9Bxx */
    case 0x9B:
      switch (low_byte)
      {
        case 0x40:            /* 0x9B40 */
          return 0x5978;
        case 0x41:            /* 0x9B41 */
          return 0x5981;
        case 0x42:            /* 0x9B42 */
          return 0x599D;
        case 0x43:            /* 0x9B43 */
          return 0x4F5E;
        case 0x44:            /* 0x9B44 */
          return 0x4FAB;
        case 0x45:            /* 0x9B45 */
          return 0x59A3;
        case 0x46:            /* 0x9B46 */
          return 0x59B2;
        case 0x47:            /* 0x9B47 */
          return 0x59C6;
        case 0x48:            /* 0x9B48 */
          return 0x59E8;
        case 0x49:            /* 0x9B49 */
          return 0x59DC;
        case 0x4A:            /* 0x9B4A */
          return 0x598D;
        case 0x4B:            /* 0x9B4B */
          return 0x59D9;
        case 0x4C:            /* 0x9B4C */
          return 0x59DA;
        case 0x4D:            /* 0x9B4D */
          return 0x5A25;
        case 0x4E:            /* 0x9B4E */
          return 0x5A1F;
        case 0x4F:            /* 0x9B4F */
          return 0x5A11;
        case 0x50:            /* 0x9B50 */
          return 0x5A1C;
        case 0x51:            /* 0x9B51 */
          return 0x5A09;
        case 0x52:            /* 0x9B52 */
          return 0x5A1A;
        case 0x53:            /* 0x9B53 */
          return 0x5A40;
        case 0x54:            /* 0x9B54 */
          return 0x5A6C;
        case 0x55:            /* 0x9B55 */
          return 0x5A49;
        case 0x56:            /* 0x9B56 */
          return 0x5A35;
        case 0x57:            /* 0x9B57 */
          return 0x5A36;
        case 0x58:            /* 0x9B58 */
          return 0x5A62;
        case 0x59:            /* 0x9B59 */
          return 0x5A6A;
        case 0x5A:            /* 0x9B5A */
          return 0x5A9A;
        case 0x5B:            /* 0x9B5B */
          return 0x5ABC;
        case 0x5C:            /* 0x9B5C */
          return 0x5ABE;
        case 0x5D:            /* 0x9B5D */
          return 0x5ACB;
        case 0x5E:            /* 0x9B5E */
          return 0x5AC2;
        case 0x5F:            /* 0x9B5F */
          return 0x5ABD;
        case 0x60:            /* 0x9B60 */
          return 0x5AE3;
        case 0x61:            /* 0x9B61 */
          return 0x5AD7;
        case 0x62:            /* 0x9B62 */
          return 0x5AE6;
        case 0x63:            /* 0x9B63 */
          return 0x5AE9;
        case 0x64:            /* 0x9B64 */
          return 0x5AD6;
        case 0x65:            /* 0x9B65 */
          return 0x5AFA;
        case 0x66:            /* 0x9B66 */
          return 0x5AFB;
        case 0x67:            /* 0x9B67 */
          return 0x5B0C;
        case 0x68:            /* 0x9B68 */
          return 0x5B0B;
        case 0x69:            /* 0x9B69 */
          return 0x5B16;
        case 0x6A:            /* 0x9B6A */
          return 0x5B32;
        case 0x6B:            /* 0x9B6B */
          return 0x5AD0;
        case 0x6C:            /* 0x9B6C */
          return 0x5B2A;
        case 0x6D:            /* 0x9B6D */
          return 0x5B36;
        case 0x6E:            /* 0x9B6E */
          return 0x5B3E;
        case 0x6F:            /* 0x9B6F */
          return 0x5B43;
        case 0x70:            /* 0x9B70 */
          return 0x5B45;
        case 0x71:            /* 0x9B71 */
          return 0x5B40;
        case 0x72:            /* 0x9B72 */
          return 0x5B51;
        case 0x73:            /* 0x9B73 */
          return 0x5B55;
        case 0x74:            /* 0x9B74 */
          return 0x5B5A;
        case 0x75:            /* 0x9B75 */
          return 0x5B5B;
        case 0x76:            /* 0x9B76 */
          return 0x5B65;
        case 0x77:            /* 0x9B77 */
          return 0x5B69;
        case 0x78:            /* 0x9B78 */
          return 0x5B70;
        case 0x79:            /* 0x9B79 */
          return 0x5B73;
        case 0x7A:            /* 0x9B7A */
          return 0x5B75;
        case 0x7B:            /* 0x9B7B */
          return 0x5B78;
        case 0x7C:            /* 0x9B7C */
          return 0x6588;
        case 0x7D:            /* 0x9B7D */
          return 0x5B7A;
        case 0x7E:            /* 0x9B7E */
          return 0x5B80;
        case 0x80:            /* 0x9B80 */
          return 0x5B83;
        case 0x81:            /* 0x9B81 */
          return 0x5BA6;
        case 0x82:            /* 0x9B82 */
          return 0x5BB8;
        case 0x83:            /* 0x9B83 */
          return 0x5BC3;
        case 0x84:            /* 0x9B84 */
          return 0x5BC7;
        case 0x85:            /* 0x9B85 */
          return 0x5BC9;
        case 0x86:            /* 0x9B86 */
          return 0x5BD4;
        case 0x87:            /* 0x9B87 */
          return 0x5BD0;
        case 0x88:            /* 0x9B88 */
          return 0x5BE4;
        case 0x89:            /* 0x9B89 */
          return 0x5BE6;
        case 0x8A:            /* 0x9B8A */
          return 0x5BE2;
        case 0x8B:            /* 0x9B8B */
          return 0x5BDE;
        case 0x8C:            /* 0x9B8C */
          return 0x5BE5;
        case 0x8D:            /* 0x9B8D */
          return 0x5BEB;
        case 0x8E:            /* 0x9B8E */
          return 0x5BF0;
        case 0x8F:            /* 0x9B8F */
          return 0x5BF6;
        case 0x90:            /* 0x9B90 */
          return 0x5BF3;
        case 0x91:            /* 0x9B91 */
          return 0x5C05;
        case 0x92:            /* 0x9B92 */
          return 0x5C07;
        case 0x93:            /* 0x9B93 */
          return 0x5C08;
        case 0x94:            /* 0x9B94 */
          return 0x5C0D;
        case 0x95:            /* 0x9B95 */
          return 0x5C13;
        case 0x96:            /* 0x9B96 */
          return 0x5C20;
        case 0x97:            /* 0x9B97 */
          return 0x5C22;
        case 0x98:            /* 0x9B98 */
          return 0x5C28;
        case 0x99:            /* 0x9B99 */
          return 0x5C38;
        case 0x9A:            /* 0x9B9A */
          return 0x5C39;
        case 0x9B:            /* 0x9B9B */
          return 0x5C41;
        case 0x9C:            /* 0x9B9C */
          return 0x5C46;
        case 0x9D:            /* 0x9B9D */
          return 0x5C4E;
        case 0x9E:            /* 0x9B9E */
          return 0x5C53;
        case 0x9F:            /* 0x9B9F */
          return 0x5C50;
        case 0xA0:            /* 0x9BA0 */
          return 0x5C4F;
        case 0xA1:            /* 0x9BA1 */
          return 0x5B71;
        case 0xA2:            /* 0x9BA2 */
          return 0x5C6C;
        case 0xA3:            /* 0x9BA3 */
          return 0x5C6E;
        case 0xA4:            /* 0x9BA4 */
          return 0x4E62;
        case 0xA5:            /* 0x9BA5 */
          return 0x5C76;
        case 0xA6:            /* 0x9BA6 */
          return 0x5C79;
        case 0xA7:            /* 0x9BA7 */
          return 0x5C8C;
        case 0xA8:            /* 0x9BA8 */
          return 0x5C91;
        case 0xA9:            /* 0x9BA9 */
          return 0x5C94;
        case 0xAA:            /* 0x9BAA */
          return 0x599B;
        case 0xAB:            /* 0x9BAB */
          return 0x5CAB;
        case 0xAC:            /* 0x9BAC */
          return 0x5CBB;
        case 0xAD:            /* 0x9BAD */
          return 0x5CB6;
        case 0xAE:            /* 0x9BAE */
          return 0x5CBC;
        case 0xAF:            /* 0x9BAF */
          return 0x5CB7;
        case 0xB0:            /* 0x9BB0 */
          return 0x5CC5;
        case 0xB1:            /* 0x9BB1 */
          return 0x5CBE;
        case 0xB2:            /* 0x9BB2 */
          return 0x5CC7;
        case 0xB3:            /* 0x9BB3 */
          return 0x5CD9;
        case 0xB4:            /* 0x9BB4 */
          return 0x5CE9;
        case 0xB5:            /* 0x9BB5 */
          return 0x5CFD;
        case 0xB6:            /* 0x9BB6 */
          return 0x5CFA;
        case 0xB7:            /* 0x9BB7 */
          return 0x5CED;
        case 0xB8:            /* 0x9BB8 */
          return 0x5D8C;
        case 0xB9:            /* 0x9BB9 */
          return 0x5CEA;
        case 0xBA:            /* 0x9BBA */
          return 0x5D0B;
        case 0xBB:            /* 0x9BBB */
          return 0x5D15;
        case 0xBC:            /* 0x9BBC */
          return 0x5D17;
        case 0xBD:            /* 0x9BBD */
          return 0x5D5C;
        case 0xBE:            /* 0x9BBE */
          return 0x5D1F;
        case 0xBF:            /* 0x9BBF */
          return 0x5D1B;
        case 0xC0:            /* 0x9BC0 */
          return 0x5D11;
        case 0xC1:            /* 0x9BC1 */
          return 0x5D14;
        case 0xC2:            /* 0x9BC2 */
          return 0x5D22;
        case 0xC3:            /* 0x9BC3 */
          return 0x5D1A;
        case 0xC4:            /* 0x9BC4 */
          return 0x5D19;
        case 0xC5:            /* 0x9BC5 */
          return 0x5D18;
        case 0xC6:            /* 0x9BC6 */
          return 0x5D4C;
        case 0xC7:            /* 0x9BC7 */
          return 0x5D52;
        case 0xC8:            /* 0x9BC8 */
          return 0x5D4E;
        case 0xC9:            /* 0x9BC9 */
          return 0x5D4B;
        case 0xCA:            /* 0x9BCA */
          return 0x5D6C;
        case 0xCB:            /* 0x9BCB */
          return 0x5D73;
        case 0xCC:            /* 0x9BCC */
          return 0x5D76;
        case 0xCD:            /* 0x9BCD */
          return 0x5D87;
        case 0xCE:            /* 0x9BCE */
          return 0x5D84;
        case 0xCF:            /* 0x9BCF */
          return 0x5D82;
        case 0xD0:            /* 0x9BD0 */
          return 0x5DA2;
        case 0xD1:            /* 0x9BD1 */
          return 0x5D9D;
        case 0xD2:            /* 0x9BD2 */
          return 0x5DAC;
        case 0xD3:            /* 0x9BD3 */
          return 0x5DAE;
        case 0xD4:            /* 0x9BD4 */
          return 0x5DBD;
        case 0xD5:            /* 0x9BD5 */
          return 0x5D90;
        case 0xD6:            /* 0x9BD6 */
          return 0x5DB7;
        case 0xD7:            /* 0x9BD7 */
          return 0x5DBC;
        case 0xD8:            /* 0x9BD8 */
          return 0x5DC9;
        case 0xD9:            /* 0x9BD9 */
          return 0x5DCD;
        case 0xDA:            /* 0x9BDA */
          return 0x5DD3;
        case 0xDB:            /* 0x9BDB */
          return 0x5DD2;
        case 0xDC:            /* 0x9BDC */
          return 0x5DD6;
        case 0xDD:            /* 0x9BDD */
          return 0x5DDB;
        case 0xDE:            /* 0x9BDE */
          return 0x5DEB;
        case 0xDF:            /* 0x9BDF */
          return 0x5DF2;
        case 0xE0:            /* 0x9BE0 */
          return 0x5DF5;
        case 0xE1:            /* 0x9BE1 */
          return 0x5E0B;
        case 0xE2:            /* 0x9BE2 */
          return 0x5E1A;
        case 0xE3:            /* 0x9BE3 */
          return 0x5E19;
        case 0xE4:            /* 0x9BE4 */
          return 0x5E11;
        case 0xE5:            /* 0x9BE5 */
          return 0x5E1B;
        case 0xE6:            /* 0x9BE6 */
          return 0x5E36;
        case 0xE7:            /* 0x9BE7 */
          return 0x5E37;
        case 0xE8:            /* 0x9BE8 */
          return 0x5E44;
        case 0xE9:            /* 0x9BE9 */
          return 0x5E43;
        case 0xEA:            /* 0x9BEA */
          return 0x5E40;
        case 0xEB:            /* 0x9BEB */
          return 0x5E4E;
        case 0xEC:            /* 0x9BEC */
          return 0x5E57;
        case 0xED:            /* 0x9BED */
          return 0x5E54;
        case 0xEE:            /* 0x9BEE */
          return 0x5E5F;
        case 0xEF:            /* 0x9BEF */
          return 0x5E62;
        case 0xF0:            /* 0x9BF0 */
          return 0x5E64;
        case 0xF1:            /* 0x9BF1 */
          return 0x5E47;
        case 0xF2:            /* 0x9BF2 */
          return 0x5E75;
        case 0xF3:            /* 0x9BF3 */
          return 0x5E76;
        case 0xF4:            /* 0x9BF4 */
          return 0x5E7A;
        case 0xF5:            /* 0x9BF5 */
          return 0x9EBC;
        case 0xF6:            /* 0x9BF6 */
          return 0x5E7F;
        case 0xF7:            /* 0x9BF7 */
          return 0x5EA0;
        case 0xF8:            /* 0x9BF8 */
          return 0x5EC1;
        case 0xF9:            /* 0x9BF9 */
          return 0x5EC2;
        case 0xFA:            /* 0x9BFA */
          return 0x5EC8;
        case 0xFB:            /* 0x9BFB */
          return 0x5ED0;
        case 0xFC:            /* 0x9BFC */
          return 0x5ECF;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 9B */

    /* Lead Byte: 0x9Cxx */
    case 0x9C:
      switch (low_byte)
      {
        case 0x40:            /* 0x9C40 */
          return 0x5ED6;
        case 0x41:            /* 0x9C41 */
          return 0x5EE3;
        case 0x42:            /* 0x9C42 */
          return 0x5EDD;
        case 0x43:            /* 0x9C43 */
          return 0x5EDA;
        case 0x44:            /* 0x9C44 */
          return 0x5EDB;
        case 0x45:            /* 0x9C45 */
          return 0x5EE2;
        case 0x46:            /* 0x9C46 */
          return 0x5EE1;
        case 0x47:            /* 0x9C47 */
          return 0x5EE8;
        case 0x48:            /* 0x9C48 */
          return 0x5EE9;
        case 0x49:            /* 0x9C49 */
          return 0x5EEC;
        case 0x4A:            /* 0x9C4A */
          return 0x5EF1;
        case 0x4B:            /* 0x9C4B */
          return 0x5EF3;
        case 0x4C:            /* 0x9C4C */
          return 0x5EF0;
        case 0x4D:            /* 0x9C4D */
          return 0x5EF4;
        case 0x4E:            /* 0x9C4E */
          return 0x5EF8;
        case 0x4F:            /* 0x9C4F */
          return 0x5EFE;
        case 0x50:            /* 0x9C50 */
          return 0x5F03;
        case 0x51:            /* 0x9C51 */
          return 0x5F09;
        case 0x52:            /* 0x9C52 */
          return 0x5F5D;
        case 0x53:            /* 0x9C53 */
          return 0x5F5C;
        case 0x54:            /* 0x9C54 */
          return 0x5F0B;
        case 0x55:            /* 0x9C55 */
          return 0x5F11;
        case 0x56:            /* 0x9C56 */
          return 0x5F16;
        case 0x57:            /* 0x9C57 */
          return 0x5F29;
        case 0x58:            /* 0x9C58 */
          return 0x5F2D;
        case 0x59:            /* 0x9C59 */
          return 0x5F38;
        case 0x5A:            /* 0x9C5A */
          return 0x5F41;
        case 0x5B:            /* 0x9C5B */
          return 0x5F48;
        case 0x5C:            /* 0x9C5C */
          return 0x5F4C;
        case 0x5D:            /* 0x9C5D */
          return 0x5F4E;
        case 0x5E:            /* 0x9C5E */
          return 0x5F2F;
        case 0x5F:            /* 0x9C5F */
          return 0x5F51;
        case 0x60:            /* 0x9C60 */
          return 0x5F56;
        case 0x61:            /* 0x9C61 */
          return 0x5F57;
        case 0x62:            /* 0x9C62 */
          return 0x5F59;
        case 0x63:            /* 0x9C63 */
          return 0x5F61;
        case 0x64:            /* 0x9C64 */
          return 0x5F6D;
        case 0x65:            /* 0x9C65 */
          return 0x5F73;
        case 0x66:            /* 0x9C66 */
          return 0x5F77;
        case 0x67:            /* 0x9C67 */
          return 0x5F83;
        case 0x68:            /* 0x9C68 */
          return 0x5F82;
        case 0x69:            /* 0x9C69 */
          return 0x5F7F;
        case 0x6A:            /* 0x9C6A */
          return 0x5F8A;
        case 0x6B:            /* 0x9C6B */
          return 0x5F88;
        case 0x6C:            /* 0x9C6C */
          return 0x5F91;
        case 0x6D:            /* 0x9C6D */
          return 0x5F87;
        case 0x6E:            /* 0x9C6E */
          return 0x5F9E;
        case 0x6F:            /* 0x9C6F */
          return 0x5F99;
        case 0x70:            /* 0x9C70 */
          return 0x5F98;
        case 0x71:            /* 0x9C71 */
          return 0x5FA0;
        case 0x72:            /* 0x9C72 */
          return 0x5FA8;
        case 0x73:            /* 0x9C73 */
          return 0x5FAD;
        case 0x74:            /* 0x9C74 */
          return 0x5FBC;
        case 0x75:            /* 0x9C75 */
          return 0x5FD6;
        case 0x76:            /* 0x9C76 */
          return 0x5FFB;
        case 0x77:            /* 0x9C77 */
          return 0x5FE4;
        case 0x78:            /* 0x9C78 */
          return 0x5FF8;
        case 0x79:            /* 0x9C79 */
          return 0x5FF1;
        case 0x7A:            /* 0x9C7A */
          return 0x5FDD;
        case 0x7B:            /* 0x9C7B */
          return 0x60B3;
        case 0x7C:            /* 0x9C7C */
          return 0x5FFF;
        case 0x7D:            /* 0x9C7D */
          return 0x6021;
        case 0x7E:            /* 0x9C7E */
          return 0x6060;
        case 0x80:            /* 0x9C80 */
          return 0x6019;
        case 0x81:            /* 0x9C81 */
          return 0x6010;
        case 0x82:            /* 0x9C82 */
          return 0x6029;
        case 0x83:            /* 0x9C83 */
          return 0x600E;
        case 0x84:            /* 0x9C84 */
          return 0x6031;
        case 0x85:            /* 0x9C85 */
          return 0x601B;
        case 0x86:            /* 0x9C86 */
          return 0x6015;
        case 0x87:            /* 0x9C87 */
          return 0x602B;
        case 0x88:            /* 0x9C88 */
          return 0x6026;
        case 0x89:            /* 0x9C89 */
          return 0x600F;
        case 0x8A:            /* 0x9C8A */
          return 0x603A;
        case 0x8B:            /* 0x9C8B */
          return 0x605A;
        case 0x8C:            /* 0x9C8C */
          return 0x6041;
        case 0x8D:            /* 0x9C8D */
          return 0x606A;
        case 0x8E:            /* 0x9C8E */
          return 0x6077;
        case 0x8F:            /* 0x9C8F */
          return 0x605F;
        case 0x90:            /* 0x9C90 */
          return 0x604A;
        case 0x91:            /* 0x9C91 */
          return 0x6046;
        case 0x92:            /* 0x9C92 */
          return 0x604D;
        case 0x93:            /* 0x9C93 */
          return 0x6063;
        case 0x94:            /* 0x9C94 */
          return 0x6043;
        case 0x95:            /* 0x9C95 */
          return 0x6064;
        case 0x96:            /* 0x9C96 */
          return 0x6042;
        case 0x97:            /* 0x9C97 */
          return 0x606C;
        case 0x98:            /* 0x9C98 */
          return 0x606B;
        case 0x99:            /* 0x9C99 */
          return 0x6059;
        case 0x9A:            /* 0x9C9A */
          return 0x6081;
        case 0x9B:            /* 0x9C9B */
          return 0x608D;
        case 0x9C:            /* 0x9C9C */
          return 0x60E7;
        case 0x9D:            /* 0x9C9D */
          return 0x6083;
        case 0x9E:            /* 0x9C9E */
          return 0x609A;
        case 0x9F:            /* 0x9C9F */
          return 0x6084;
        case 0xA0:            /* 0x9CA0 */
          return 0x609B;
        case 0xA1:            /* 0x9CA1 */
          return 0x6096;
        case 0xA2:            /* 0x9CA2 */
          return 0x6097;
        case 0xA3:            /* 0x9CA3 */
          return 0x6092;
        case 0xA4:            /* 0x9CA4 */
          return 0x60A7;
        case 0xA5:            /* 0x9CA5 */
          return 0x608B;
        case 0xA6:            /* 0x9CA6 */
          return 0x60E1;
        case 0xA7:            /* 0x9CA7 */
          return 0x60B8;
        case 0xA8:            /* 0x9CA8 */
          return 0x60E0;
        case 0xA9:            /* 0x9CA9 */
          return 0x60D3;
        case 0xAA:            /* 0x9CAA */
          return 0x60B4;
        case 0xAB:            /* 0x9CAB */
          return 0x5FF0;
        case 0xAC:            /* 0x9CAC */
          return 0x60BD;
        case 0xAD:            /* 0x9CAD */
          return 0x60C6;
        case 0xAE:            /* 0x9CAE */
          return 0x60B5;
        case 0xAF:            /* 0x9CAF */
          return 0x60D8;
        case 0xB0:            /* 0x9CB0 */
          return 0x614D;
        case 0xB1:            /* 0x9CB1 */
          return 0x6115;
        case 0xB2:            /* 0x9CB2 */
          return 0x6106;
        case 0xB3:            /* 0x9CB3 */
          return 0x60F6;
        case 0xB4:            /* 0x9CB4 */
          return 0x60F7;
        case 0xB5:            /* 0x9CB5 */
          return 0x6100;
        case 0xB6:            /* 0x9CB6 */
          return 0x60F4;
        case 0xB7:            /* 0x9CB7 */
          return 0x60FA;
        case 0xB8:            /* 0x9CB8 */
          return 0x6103;
        case 0xB9:            /* 0x9CB9 */
          return 0x6121;
        case 0xBA:            /* 0x9CBA */
          return 0x60FB;
        case 0xBB:            /* 0x9CBB */
          return 0x60F1;
        case 0xBC:            /* 0x9CBC */
          return 0x610D;
        case 0xBD:            /* 0x9CBD */
          return 0x610E;
        case 0xBE:            /* 0x9CBE */
          return 0x6147;
        case 0xBF:            /* 0x9CBF */
          return 0x613E;
        case 0xC0:            /* 0x9CC0 */
          return 0x6128;
        case 0xC1:            /* 0x9CC1 */
          return 0x6127;
        case 0xC2:            /* 0x9CC2 */
          return 0x614A;
        case 0xC3:            /* 0x9CC3 */
          return 0x613F;
        case 0xC4:            /* 0x9CC4 */
          return 0x613C;
        case 0xC5:            /* 0x9CC5 */
          return 0x612C;
        case 0xC6:            /* 0x9CC6 */
          return 0x6134;
        case 0xC7:            /* 0x9CC7 */
          return 0x613D;
        case 0xC8:            /* 0x9CC8 */
          return 0x6142;
        case 0xC9:            /* 0x9CC9 */
          return 0x6144;
        case 0xCA:            /* 0x9CCA */
          return 0x6173;
        case 0xCB:            /* 0x9CCB */
          return 0x6177;
        case 0xCC:            /* 0x9CCC */
          return 0x6158;
        case 0xCD:            /* 0x9CCD */
          return 0x6159;
        case 0xCE:            /* 0x9CCE */
          return 0x615A;
        case 0xCF:            /* 0x9CCF */
          return 0x616B;
        case 0xD0:            /* 0x9CD0 */
          return 0x6174;
        case 0xD1:            /* 0x9CD1 */
          return 0x616F;
        case 0xD2:            /* 0x9CD2 */
          return 0x6165;
        case 0xD3:            /* 0x9CD3 */
          return 0x6171;
        case 0xD4:            /* 0x9CD4 */
          return 0x615F;
        case 0xD5:            /* 0x9CD5 */
          return 0x615D;
        case 0xD6:            /* 0x9CD6 */
          return 0x6153;
        case 0xD7:            /* 0x9CD7 */
          return 0x6175;
        case 0xD8:            /* 0x9CD8 */
          return 0x6199;
        case 0xD9:            /* 0x9CD9 */
          return 0x6196;
        case 0xDA:            /* 0x9CDA */
          return 0x6187;
        case 0xDB:            /* 0x9CDB */
          return 0x61AC;
        case 0xDC:            /* 0x9CDC */
          return 0x6194;
        case 0xDD:            /* 0x9CDD */
          return 0x619A;
        case 0xDE:            /* 0x9CDE */
          return 0x618A;
        case 0xDF:            /* 0x9CDF */
          return 0x6191;
        case 0xE0:            /* 0x9CE0 */
          return 0x61AB;
        case 0xE1:            /* 0x9CE1 */
          return 0x61AE;
        case 0xE2:            /* 0x9CE2 */
          return 0x61CC;
        case 0xE3:            /* 0x9CE3 */
          return 0x61CA;
        case 0xE4:            /* 0x9CE4 */
          return 0x61C9;
        case 0xE5:            /* 0x9CE5 */
          return 0x61F7;
        case 0xE6:            /* 0x9CE6 */
          return 0x61C8;
        case 0xE7:            /* 0x9CE7 */
          return 0x61C3;
        case 0xE8:            /* 0x9CE8 */
          return 0x61C6;
        case 0xE9:            /* 0x9CE9 */
          return 0x61BA;
        case 0xEA:            /* 0x9CEA */
          return 0x61CB;
        case 0xEB:            /* 0x9CEB */
          return 0x7F79;
        case 0xEC:            /* 0x9CEC */
          return 0x61CD;
        case 0xED:            /* 0x9CED */
          return 0x61E6;
        case 0xEE:            /* 0x9CEE */
          return 0x61E3;
        case 0xEF:            /* 0x9CEF */
          return 0x61F6;
        case 0xF0:            /* 0x9CF0 */
          return 0x61FA;
        case 0xF1:            /* 0x9CF1 */
          return 0x61F4;
        case 0xF2:            /* 0x9CF2 */
          return 0x61FF;
        case 0xF3:            /* 0x9CF3 */
          return 0x61FD;
        case 0xF4:            /* 0x9CF4 */
          return 0x61FC;
        case 0xF5:            /* 0x9CF5 */
          return 0x61FE;
        case 0xF6:            /* 0x9CF6 */
          return 0x6200;
        case 0xF7:            /* 0x9CF7 */
          return 0x6208;
        case 0xF8:            /* 0x9CF8 */
          return 0x6209;
        case 0xF9:            /* 0x9CF9 */
          return 0x620D;
        case 0xFA:            /* 0x9CFA */
          return 0x620C;
        case 0xFB:            /* 0x9CFB */
          return 0x6214;
        case 0xFC:            /* 0x9CFC */
          return 0x621B;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 9C */

    /* Lead Byte: 0x9Dxx */
    case 0x9D:
      switch (low_byte)
      {
        case 0x40:            /* 0x9D40 */
          return 0x621E;
        case 0x41:            /* 0x9D41 */
          return 0x6221;
        case 0x42:            /* 0x9D42 */
          return 0x622A;
        case 0x43:            /* 0x9D43 */
          return 0x622E;
        case 0x44:            /* 0x9D44 */
          return 0x6230;
        case 0x45:            /* 0x9D45 */
          return 0x6232;
        case 0x46:            /* 0x9D46 */
          return 0x6233;
        case 0x47:            /* 0x9D47 */
          return 0x6241;
        case 0x48:            /* 0x9D48 */
          return 0x624E;
        case 0x49:            /* 0x9D49 */
          return 0x625E;
        case 0x4A:            /* 0x9D4A */
          return 0x6263;
        case 0x4B:            /* 0x9D4B */
          return 0x625B;
        case 0x4C:            /* 0x9D4C */
          return 0x6260;
        case 0x4D:            /* 0x9D4D */
          return 0x6268;
        case 0x4E:            /* 0x9D4E */
          return 0x627C;
        case 0x4F:            /* 0x9D4F */
          return 0x6282;
        case 0x50:            /* 0x9D50 */
          return 0x6289;
        case 0x51:            /* 0x9D51 */
          return 0x627E;
        case 0x52:            /* 0x9D52 */
          return 0x6292;
        case 0x53:            /* 0x9D53 */
          return 0x6293;
        case 0x54:            /* 0x9D54 */
          return 0x6296;
        case 0x55:            /* 0x9D55 */
          return 0x62D4;
        case 0x56:            /* 0x9D56 */
          return 0x6283;
        case 0x57:            /* 0x9D57 */
          return 0x6294;
        case 0x58:            /* 0x9D58 */
          return 0x62D7;
        case 0x59:            /* 0x9D59 */
          return 0x62D1;
        case 0x5A:            /* 0x9D5A */
          return 0x62BB;
        case 0x5B:            /* 0x9D5B */
          return 0x62CF;
        case 0x5C:            /* 0x9D5C */
          return 0x62FF;
        case 0x5D:            /* 0x9D5D */
          return 0x62C6;
        case 0x5E:            /* 0x9D5E */
          return 0x64D4;
        case 0x5F:            /* 0x9D5F */
          return 0x62C8;
        case 0x60:            /* 0x9D60 */
          return 0x62DC;
        case 0x61:            /* 0x9D61 */
          return 0x62CC;
        case 0x62:            /* 0x9D62 */
          return 0x62CA;
        case 0x63:            /* 0x9D63 */
          return 0x62C2;
        case 0x64:            /* 0x9D64 */
          return 0x62C7;
        case 0x65:            /* 0x9D65 */
          return 0x629B;
        case 0x66:            /* 0x9D66 */
          return 0x62C9;
        case 0x67:            /* 0x9D67 */
          return 0x630C;
        case 0x68:            /* 0x9D68 */
          return 0x62EE;
        case 0x69:            /* 0x9D69 */
          return 0x62F1;
        case 0x6A:            /* 0x9D6A */
          return 0x6327;
        case 0x6B:            /* 0x9D6B */
          return 0x6302;
        case 0x6C:            /* 0x9D6C */
          return 0x6308;
        case 0x6D:            /* 0x9D6D */
          return 0x62EF;
        case 0x6E:            /* 0x9D6E */
          return 0x62F5;
        case 0x6F:            /* 0x9D6F */
          return 0x6350;
        case 0x70:            /* 0x9D70 */
          return 0x633E;
        case 0x71:            /* 0x9D71 */
          return 0x634D;
        case 0x72:            /* 0x9D72 */
          return 0x641C;
        case 0x73:            /* 0x9D73 */
          return 0x634F;
        case 0x74:            /* 0x9D74 */
          return 0x6396;
        case 0x75:            /* 0x9D75 */
          return 0x638E;
        case 0x76:            /* 0x9D76 */
          return 0x6380;
        case 0x77:            /* 0x9D77 */
          return 0x63AB;
        case 0x78:            /* 0x9D78 */
          return 0x6376;
        case 0x79:            /* 0x9D79 */
          return 0x63A3;
        case 0x7A:            /* 0x9D7A */
          return 0x638F;
        case 0x7B:            /* 0x9D7B */
          return 0x6389;
        case 0x7C:            /* 0x9D7C */
          return 0x639F;
        case 0x7D:            /* 0x9D7D */
          return 0x63B5;
        case 0x7E:            /* 0x9D7E */
          return 0x636B;
        case 0x80:            /* 0x9D80 */
          return 0x6369;
        case 0x81:            /* 0x9D81 */
          return 0x63BE;
        case 0x82:            /* 0x9D82 */
          return 0x63E9;
        case 0x83:            /* 0x9D83 */
          return 0x63C0;
        case 0x84:            /* 0x9D84 */
          return 0x63C6;
        case 0x85:            /* 0x9D85 */
          return 0x63E3;
        case 0x86:            /* 0x9D86 */
          return 0x63C9;
        case 0x87:            /* 0x9D87 */
          return 0x63D2;
        case 0x88:            /* 0x9D88 */
          return 0x63F6;
        case 0x89:            /* 0x9D89 */
          return 0x63C4;
        case 0x8A:            /* 0x9D8A */
          return 0x6416;
        case 0x8B:            /* 0x9D8B */
          return 0x6434;
        case 0x8C:            /* 0x9D8C */
          return 0x6406;
        case 0x8D:            /* 0x9D8D */
          return 0x6413;
        case 0x8E:            /* 0x9D8E */
          return 0x6426;
        case 0x8F:            /* 0x9D8F */
          return 0x6436;
        case 0x90:            /* 0x9D90 */
          return 0x651D;
        case 0x91:            /* 0x9D91 */
          return 0x6417;
        case 0x92:            /* 0x9D92 */
          return 0x6428;
        case 0x93:            /* 0x9D93 */
          return 0x640F;
        case 0x94:            /* 0x9D94 */
          return 0x6467;
        case 0x95:            /* 0x9D95 */
          return 0x646F;
        case 0x96:            /* 0x9D96 */
          return 0x6476;
        case 0x97:            /* 0x9D97 */
          return 0x644E;
        case 0x98:            /* 0x9D98 */
          return 0x652A;
        case 0x99:            /* 0x9D99 */
          return 0x6495;
        case 0x9A:            /* 0x9D9A */
          return 0x6493;
        case 0x9B:            /* 0x9D9B */
          return 0x64A5;
        case 0x9C:            /* 0x9D9C */
          return 0x64A9;
        case 0x9D:            /* 0x9D9D */
          return 0x6488;
        case 0x9E:            /* 0x9D9E */
          return 0x64BC;
        case 0x9F:            /* 0x9D9F */
          return 0x64DA;
        case 0xA0:            /* 0x9DA0 */
          return 0x64D2;
        case 0xA1:            /* 0x9DA1 */
          return 0x64C5;
        case 0xA2:            /* 0x9DA2 */
          return 0x64C7;
        case 0xA3:            /* 0x9DA3 */
          return 0x64BB;
        case 0xA4:            /* 0x9DA4 */
          return 0x64D8;
        case 0xA5:            /* 0x9DA5 */
          return 0x64C2;
        case 0xA6:            /* 0x9DA6 */
          return 0x64F1;
        case 0xA7:            /* 0x9DA7 */
          return 0x64E7;
        case 0xA8:            /* 0x9DA8 */
          return 0x8209;
        case 0xA9:            /* 0x9DA9 */
          return 0x64E0;
        case 0xAA:            /* 0x9DAA */
          return 0x64E1;
        case 0xAB:            /* 0x9DAB */
          return 0x62AC;
        case 0xAC:            /* 0x9DAC */
          return 0x64E3;
        case 0xAD:            /* 0x9DAD */
          return 0x64EF;
        case 0xAE:            /* 0x9DAE */
          return 0x652C;
        case 0xAF:            /* 0x9DAF */
          return 0x64F6;
        case 0xB0:            /* 0x9DB0 */
          return 0x64F4;
        case 0xB1:            /* 0x9DB1 */
          return 0x64F2;
        case 0xB2:            /* 0x9DB2 */
          return 0x64FA;
        case 0xB3:            /* 0x9DB3 */
          return 0x6500;
        case 0xB4:            /* 0x9DB4 */
          return 0x64FD;
        case 0xB5:            /* 0x9DB5 */
          return 0x6518;
        case 0xB6:            /* 0x9DB6 */
          return 0x651C;
        case 0xB7:            /* 0x9DB7 */
          return 0x6505;
        case 0xB8:            /* 0x9DB8 */
          return 0x6524;
        case 0xB9:            /* 0x9DB9 */
          return 0x6523;
        case 0xBA:            /* 0x9DBA */
          return 0x652B;
        case 0xBB:            /* 0x9DBB */
          return 0x6534;
        case 0xBC:            /* 0x9DBC */
          return 0x6535;
        case 0xBD:            /* 0x9DBD */
          return 0x6537;
        case 0xBE:            /* 0x9DBE */
          return 0x6536;
        case 0xBF:            /* 0x9DBF */
          return 0x6538;
        case 0xC0:            /* 0x9DC0 */
          return 0x754B;
        case 0xC1:            /* 0x9DC1 */
          return 0x6548;
        case 0xC2:            /* 0x9DC2 */
          return 0x6556;
        case 0xC3:            /* 0x9DC3 */
          return 0x6555;
        case 0xC4:            /* 0x9DC4 */
          return 0x654D;
        case 0xC5:            /* 0x9DC5 */
          return 0x6558;
        case 0xC6:            /* 0x9DC6 */
          return 0x655E;
        case 0xC7:            /* 0x9DC7 */
          return 0x655D;
        case 0xC8:            /* 0x9DC8 */
          return 0x6572;
        case 0xC9:            /* 0x9DC9 */
          return 0x6578;
        case 0xCA:            /* 0x9DCA */
          return 0x6582;
        case 0xCB:            /* 0x9DCB */
          return 0x6583;
        case 0xCC:            /* 0x9DCC */
          return 0x8B8A;
        case 0xCD:            /* 0x9DCD */
          return 0x659B;
        case 0xCE:            /* 0x9DCE */
          return 0x659F;
        case 0xCF:            /* 0x9DCF */
          return 0x65AB;
        case 0xD0:            /* 0x9DD0 */
          return 0x65B7;
        case 0xD1:            /* 0x9DD1 */
          return 0x65C3;
        case 0xD2:            /* 0x9DD2 */
          return 0x65C6;
        case 0xD3:            /* 0x9DD3 */
          return 0x65C1;
        case 0xD4:            /* 0x9DD4 */
          return 0x65C4;
        case 0xD5:            /* 0x9DD5 */
          return 0x65CC;
        case 0xD6:            /* 0x9DD6 */
          return 0x65D2;
        case 0xD7:            /* 0x9DD7 */
          return 0x65DB;
        case 0xD8:            /* 0x9DD8 */
          return 0x65D9;
        case 0xD9:            /* 0x9DD9 */
          return 0x65E0;
        case 0xDA:            /* 0x9DDA */
          return 0x65E1;
        case 0xDB:            /* 0x9DDB */
          return 0x65F1;
        case 0xDC:            /* 0x9DDC */
          return 0x6772;
        case 0xDD:            /* 0x9DDD */
          return 0x660A;
        case 0xDE:            /* 0x9DDE */
          return 0x6603;
        case 0xDF:            /* 0x9DDF */
          return 0x65FB;
        case 0xE0:            /* 0x9DE0 */
          return 0x6773;
        case 0xE1:            /* 0x9DE1 */
          return 0x6635;
        case 0xE2:            /* 0x9DE2 */
          return 0x6636;
        case 0xE3:            /* 0x9DE3 */
          return 0x6634;
        case 0xE4:            /* 0x9DE4 */
          return 0x661C;
        case 0xE5:            /* 0x9DE5 */
          return 0x664F;
        case 0xE6:            /* 0x9DE6 */
          return 0x6644;
        case 0xE7:            /* 0x9DE7 */
          return 0x6649;
        case 0xE8:            /* 0x9DE8 */
          return 0x6641;
        case 0xE9:            /* 0x9DE9 */
          return 0x665E;
        case 0xEA:            /* 0x9DEA */
          return 0x665D;
        case 0xEB:            /* 0x9DEB */
          return 0x6664;
        case 0xEC:            /* 0x9DEC */
          return 0x6667;
        case 0xED:            /* 0x9DED */
          return 0x6668;
        case 0xEE:            /* 0x9DEE */
          return 0x665F;
        case 0xEF:            /* 0x9DEF */
          return 0x6662;
        case 0xF0:            /* 0x9DF0 */
          return 0x6670;
        case 0xF1:            /* 0x9DF1 */
          return 0x6683;
        case 0xF2:            /* 0x9DF2 */
          return 0x6688;
        case 0xF3:            /* 0x9DF3 */
          return 0x668E;
        case 0xF4:            /* 0x9DF4 */
          return 0x6689;
        case 0xF5:            /* 0x9DF5 */
          return 0x6684;
        case 0xF6:            /* 0x9DF6 */
          return 0x6698;
        case 0xF7:            /* 0x9DF7 */
          return 0x669D;
        case 0xF8:            /* 0x9DF8 */
          return 0x66C1;
        case 0xF9:            /* 0x9DF9 */
          return 0x66B9;
        case 0xFA:            /* 0x9DFA */
          return 0x66C9;
        case 0xFB:            /* 0x9DFB */
          return 0x66BE;
        case 0xFC:            /* 0x9DFC */
          return 0x66BC;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 9D */

    /* Lead Byte: 0x9Exx */
    case 0x9E:
      switch (low_byte)
      {
        case 0x40:            /* 0x9E40 */
          return 0x66C4;
        case 0x41:            /* 0x9E41 */
          return 0x66B8;
        case 0x42:            /* 0x9E42 */
          return 0x66D6;
        case 0x43:            /* 0x9E43 */
          return 0x66DA;
        case 0x44:            /* 0x9E44 */
          return 0x66E0;
        case 0x45:            /* 0x9E45 */
          return 0x663F;
        case 0x46:            /* 0x9E46 */
          return 0x66E6;
        case 0x47:            /* 0x9E47 */
          return 0x66E9;
        case 0x48:            /* 0x9E48 */
          return 0x66F0;
        case 0x49:            /* 0x9E49 */
          return 0x66F5;
        case 0x4A:            /* 0x9E4A */
          return 0x66F7;
        case 0x4B:            /* 0x9E4B */
          return 0x670F;
        case 0x4C:            /* 0x9E4C */
          return 0x6716;
        case 0x4D:            /* 0x9E4D */
          return 0x671E;
        case 0x4E:            /* 0x9E4E */
          return 0x6726;
        case 0x4F:            /* 0x9E4F */
          return 0x6727;
        case 0x50:            /* 0x9E50 */
          return 0x9738;
        case 0x51:            /* 0x9E51 */
          return 0x672E;
        case 0x52:            /* 0x9E52 */
          return 0x673F;
        case 0x53:            /* 0x9E53 */
          return 0x6736;
        case 0x54:            /* 0x9E54 */
          return 0x6741;
        case 0x55:            /* 0x9E55 */
          return 0x6738;
        case 0x56:            /* 0x9E56 */
          return 0x6737;
        case 0x57:            /* 0x9E57 */
          return 0x6746;
        case 0x58:            /* 0x9E58 */
          return 0x675E;
        case 0x59:            /* 0x9E59 */
          return 0x6760;
        case 0x5A:            /* 0x9E5A */
          return 0x6759;
        case 0x5B:            /* 0x9E5B */
          return 0x6763;
        case 0x5C:            /* 0x9E5C */
          return 0x6764;
        case 0x5D:            /* 0x9E5D */
          return 0x6789;
        case 0x5E:            /* 0x9E5E */
          return 0x6770;
        case 0x5F:            /* 0x9E5F */
          return 0x67A9;
        case 0x60:            /* 0x9E60 */
          return 0x677C;
        case 0x61:            /* 0x9E61 */
          return 0x676A;
        case 0x62:            /* 0x9E62 */
          return 0x678C;
        case 0x63:            /* 0x9E63 */
          return 0x678B;
        case 0x64:            /* 0x9E64 */
          return 0x67A6;
        case 0x65:            /* 0x9E65 */
          return 0x67A1;
        case 0x66:            /* 0x9E66 */
          return 0x6785;
        case 0x67:            /* 0x9E67 */
          return 0x67B7;
        case 0x68:            /* 0x9E68 */
          return 0x67EF;
        case 0x69:            /* 0x9E69 */
          return 0x67B4;
        case 0x6A:            /* 0x9E6A */
          return 0x67EC;
        case 0x6B:            /* 0x9E6B */
          return 0x67B3;
        case 0x6C:            /* 0x9E6C */
          return 0x67E9;
        case 0x6D:            /* 0x9E6D */
          return 0x67B8;
        case 0x6E:            /* 0x9E6E */
          return 0x67E4;
        case 0x6F:            /* 0x9E6F */
          return 0x67DE;
        case 0x70:            /* 0x9E70 */
          return 0x67DD;
        case 0x71:            /* 0x9E71 */
          return 0x67E2;
        case 0x72:            /* 0x9E72 */
          return 0x67EE;
        case 0x73:            /* 0x9E73 */
          return 0x67B9;
        case 0x74:            /* 0x9E74 */
          return 0x67CE;
        case 0x75:            /* 0x9E75 */
          return 0x67C6;
        case 0x76:            /* 0x9E76 */
          return 0x67E7;
        case 0x77:            /* 0x9E77 */
          return 0x6A9C;
        case 0x78:            /* 0x9E78 */
          return 0x681E;
        case 0x79:            /* 0x9E79 */
          return 0x6846;
        case 0x7A:            /* 0x9E7A */
          return 0x6829;
        case 0x7B:            /* 0x9E7B */
          return 0x6840;
        case 0x7C:            /* 0x9E7C */
          return 0x684D;
        case 0x7D:            /* 0x9E7D */
          return 0x6832;
        case 0x7E:            /* 0x9E7E */
          return 0x684E;
        case 0x80:            /* 0x9E80 */
          return 0x68B3;
        case 0x81:            /* 0x9E81 */
          return 0x682B;
        case 0x82:            /* 0x9E82 */
          return 0x6859;
        case 0x83:            /* 0x9E83 */
          return 0x6863;
        case 0x84:            /* 0x9E84 */
          return 0x6877;
        case 0x85:            /* 0x9E85 */
          return 0x687F;
        case 0x86:            /* 0x9E86 */
          return 0x689F;
        case 0x87:            /* 0x9E87 */
          return 0x688F;
        case 0x88:            /* 0x9E88 */
          return 0x68AD;
        case 0x89:            /* 0x9E89 */
          return 0x6894;
        case 0x8A:            /* 0x9E8A */
          return 0x689D;
        case 0x8B:            /* 0x9E8B */
          return 0x689B;
        case 0x8C:            /* 0x9E8C */
          return 0x6883;
        case 0x8D:            /* 0x9E8D */
          return 0x6AAE;
        case 0x8E:            /* 0x9E8E */
          return 0x68B9;
        case 0x8F:            /* 0x9E8F */
          return 0x6874;
        case 0x90:            /* 0x9E90 */
          return 0x68B5;
        case 0x91:            /* 0x9E91 */
          return 0x68A0;
        case 0x92:            /* 0x9E92 */
          return 0x68BA;
        case 0x93:            /* 0x9E93 */
          return 0x690F;
        case 0x94:            /* 0x9E94 */
          return 0x688D;
        case 0x95:            /* 0x9E95 */
          return 0x687E;
        case 0x96:            /* 0x9E96 */
          return 0x6901;
        case 0x97:            /* 0x9E97 */
          return 0x68CA;
        case 0x98:            /* 0x9E98 */
          return 0x6908;
        case 0x99:            /* 0x9E99 */
          return 0x68D8;
        case 0x9A:            /* 0x9E9A */
          return 0x6922;
        case 0x9B:            /* 0x9E9B */
          return 0x6926;
        case 0x9C:            /* 0x9E9C */
          return 0x68E1;
        case 0x9D:            /* 0x9E9D */
          return 0x690C;
        case 0x9E:            /* 0x9E9E */
          return 0x68CD;
        case 0x9F:            /* 0x9E9F */
          return 0x68D4;
        case 0xA0:            /* 0x9EA0 */
          return 0x68E7;
        case 0xA1:            /* 0x9EA1 */
          return 0x68D5;
        case 0xA2:            /* 0x9EA2 */
          return 0x6936;
        case 0xA3:            /* 0x9EA3 */
          return 0x6912;
        case 0xA4:            /* 0x9EA4 */
          return 0x6904;
        case 0xA5:            /* 0x9EA5 */
          return 0x68D7;
        case 0xA6:            /* 0x9EA6 */
          return 0x68E3;
        case 0xA7:            /* 0x9EA7 */
          return 0x6925;
        case 0xA8:            /* 0x9EA8 */
          return 0x68F9;
        case 0xA9:            /* 0x9EA9 */
          return 0x68E0;
        case 0xAA:            /* 0x9EAA */
          return 0x68EF;
        case 0xAB:            /* 0x9EAB */
          return 0x6928;
        case 0xAC:            /* 0x9EAC */
          return 0x692A;
        case 0xAD:            /* 0x9EAD */
          return 0x691A;
        case 0xAE:            /* 0x9EAE */
          return 0x6923;
        case 0xAF:            /* 0x9EAF */
          return 0x6921;
        case 0xB0:            /* 0x9EB0 */
          return 0x68C6;
        case 0xB1:            /* 0x9EB1 */
          return 0x6979;
        case 0xB2:            /* 0x9EB2 */
          return 0x6977;
        case 0xB3:            /* 0x9EB3 */
          return 0x695C;
        case 0xB4:            /* 0x9EB4 */
          return 0x6978;
        case 0xB5:            /* 0x9EB5 */
          return 0x696B;
        case 0xB6:            /* 0x9EB6 */
          return 0x6954;
        case 0xB7:            /* 0x9EB7 */
          return 0x697E;
        case 0xB8:            /* 0x9EB8 */
          return 0x696E;
        case 0xB9:            /* 0x9EB9 */
          return 0x6939;
        case 0xBA:            /* 0x9EBA */
          return 0x6974;
        case 0xBB:            /* 0x9EBB */
          return 0x693D;
        case 0xBC:            /* 0x9EBC */
          return 0x6959;
        case 0xBD:            /* 0x9EBD */
          return 0x6930;
        case 0xBE:            /* 0x9EBE */
          return 0x6961;
        case 0xBF:            /* 0x9EBF */
          return 0x695E;
        case 0xC0:            /* 0x9EC0 */
          return 0x695D;
        case 0xC1:            /* 0x9EC1 */
          return 0x6981;
        case 0xC2:            /* 0x9EC2 */
          return 0x696A;
        case 0xC3:            /* 0x9EC3 */
          return 0x69B2;
        case 0xC4:            /* 0x9EC4 */
          return 0x69AE;
        case 0xC5:            /* 0x9EC5 */
          return 0x69D0;
        case 0xC6:            /* 0x9EC6 */
          return 0x69BF;
        case 0xC7:            /* 0x9EC7 */
          return 0x69C1;
        case 0xC8:            /* 0x9EC8 */
          return 0x69D3;
        case 0xC9:            /* 0x9EC9 */
          return 0x69BE;
        case 0xCA:            /* 0x9ECA */
          return 0x69CE;
        case 0xCB:            /* 0x9ECB */
          return 0x5BE8;
        case 0xCC:            /* 0x9ECC */
          return 0x69CA;
        case 0xCD:            /* 0x9ECD */
          return 0x69DD;
        case 0xCE:            /* 0x9ECE */
          return 0x69BB;
        case 0xCF:            /* 0x9ECF */
          return 0x69C3;
        case 0xD0:            /* 0x9ED0 */
          return 0x69A7;
        case 0xD1:            /* 0x9ED1 */
          return 0x6A2E;
        case 0xD2:            /* 0x9ED2 */
          return 0x6991;
        case 0xD3:            /* 0x9ED3 */
          return 0x69A0;
        case 0xD4:            /* 0x9ED4 */
          return 0x699C;
        case 0xD5:            /* 0x9ED5 */
          return 0x6995;
        case 0xD6:            /* 0x9ED6 */
          return 0x69B4;
        case 0xD7:            /* 0x9ED7 */
          return 0x69DE;
        case 0xD8:            /* 0x9ED8 */
          return 0x69E8;
        case 0xD9:            /* 0x9ED9 */
          return 0x6A02;
        case 0xDA:            /* 0x9EDA */
          return 0x6A1B;
        case 0xDB:            /* 0x9EDB */
          return 0x69FF;
        case 0xDC:            /* 0x9EDC */
          return 0x6B0A;
        case 0xDD:            /* 0x9EDD */
          return 0x69F9;
        case 0xDE:            /* 0x9EDE */
          return 0x69F2;
        case 0xDF:            /* 0x9EDF */
          return 0x69E7;
        case 0xE0:            /* 0x9EE0 */
          return 0x6A05;
        case 0xE1:            /* 0x9EE1 */
          return 0x69B1;
        case 0xE2:            /* 0x9EE2 */
          return 0x6A1E;
        case 0xE3:            /* 0x9EE3 */
          return 0x69ED;
        case 0xE4:            /* 0x9EE4 */
          return 0x6A14;
        case 0xE5:            /* 0x9EE5 */
          return 0x69EB;
        case 0xE6:            /* 0x9EE6 */
          return 0x6A0A;
        case 0xE7:            /* 0x9EE7 */
          return 0x6A12;
        case 0xE8:            /* 0x9EE8 */
          return 0x6AC1;
        case 0xE9:            /* 0x9EE9 */
          return 0x6A23;
        case 0xEA:            /* 0x9EEA */
          return 0x6A13;
        case 0xEB:            /* 0x9EEB */
          return 0x6A44;
        case 0xEC:            /* 0x9EEC */
          return 0x6A0C;
        case 0xED:            /* 0x9EED */
          return 0x6A72;
        case 0xEE:            /* 0x9EEE */
          return 0x6A36;
        case 0xEF:            /* 0x9EEF */
          return 0x6A78;
        case 0xF0:            /* 0x9EF0 */
          return 0x6A47;
        case 0xF1:            /* 0x9EF1 */
          return 0x6A62;
        case 0xF2:            /* 0x9EF2 */
          return 0x6A59;
        case 0xF3:            /* 0x9EF3 */
          return 0x6A66;
        case 0xF4:            /* 0x9EF4 */
          return 0x6A48;
        case 0xF5:            /* 0x9EF5 */
          return 0x6A38;
        case 0xF6:            /* 0x9EF6 */
          return 0x6A22;
        case 0xF7:            /* 0x9EF7 */
          return 0x6A90;
        case 0xF8:            /* 0x9EF8 */
          return 0x6A8D;
        case 0xF9:            /* 0x9EF9 */
          return 0x6AA0;
        case 0xFA:            /* 0x9EFA */
          return 0x6A84;
        case 0xFB:            /* 0x9EFB */
          return 0x6AA2;
        case 0xFC:            /* 0x9EFC */
          return 0x6AA3;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 9E */

    /* Lead Byte: 0x9Fxx */
    case 0x9F:
      switch (low_byte)
      {
        case 0x40:            /* 0x9F40 */
          return 0x6A97;
        case 0x41:            /* 0x9F41 */
          return 0x8617;
        case 0x42:            /* 0x9F42 */
          return 0x6ABB;
        case 0x43:            /* 0x9F43 */
          return 0x6AC3;
        case 0x44:            /* 0x9F44 */
          return 0x6AC2;
        case 0x45:            /* 0x9F45 */
          return 0x6AB8;
        case 0x46:            /* 0x9F46 */
          return 0x6AB3;
        case 0x47:            /* 0x9F47 */
          return 0x6AAC;
        case 0x48:            /* 0x9F48 */
          return 0x6ADE;
        case 0x49:            /* 0x9F49 */
          return 0x6AD1;
        case 0x4A:            /* 0x9F4A */
          return 0x6ADF;
        case 0x4B:            /* 0x9F4B */
          return 0x6AAA;
        case 0x4C:            /* 0x9F4C */
          return 0x6ADA;
        case 0x4D:            /* 0x9F4D */
          return 0x6AEA;
        case 0x4E:            /* 0x9F4E */
          return 0x6AFB;
        case 0x4F:            /* 0x9F4F */
          return 0x6B05;
        case 0x50:            /* 0x9F50 */
          return 0x8616;
        case 0x51:            /* 0x9F51 */
          return 0x6AFA;
        case 0x52:            /* 0x9F52 */
          return 0x6B12;
        case 0x53:            /* 0x9F53 */
          return 0x6B16;
        case 0x54:            /* 0x9F54 */
          return 0x9B31;
        case 0x55:            /* 0x9F55 */
          return 0x6B1F;
        case 0x56:            /* 0x9F56 */
          return 0x6B38;
        case 0x57:            /* 0x9F57 */
          return 0x6B37;
        case 0x58:            /* 0x9F58 */
          return 0x76DC;
        case 0x59:            /* 0x9F59 */
          return 0x6B39;
        case 0x5A:            /* 0x9F5A */
          return 0x98EE;
        case 0x5B:            /* 0x9F5B */
          return 0x6B47;
        case 0x5C:            /* 0x9F5C */
          return 0x6B43;
        case 0x5D:            /* 0x9F5D */
          return 0x6B49;
        case 0x5E:            /* 0x9F5E */
          return 0x6B50;
        case 0x5F:            /* 0x9F5F */
          return 0x6B59;
        case 0x60:            /* 0x9F60 */
          return 0x6B54;
        case 0x61:            /* 0x9F61 */
          return 0x6B5B;
        case 0x62:            /* 0x9F62 */
          return 0x6B5F;
        case 0x63:            /* 0x9F63 */
          return 0x6B61;
        case 0x64:            /* 0x9F64 */
          return 0x6B78;
        case 0x65:            /* 0x9F65 */
          return 0x6B79;
        case 0x66:            /* 0x9F66 */
          return 0x6B7F;
        case 0x67:            /* 0x9F67 */
          return 0x6B80;
        case 0x68:            /* 0x9F68 */
          return 0x6B84;
        case 0x69:            /* 0x9F69 */
          return 0x6B83;
        case 0x6A:            /* 0x9F6A */
          return 0x6B8D;
        case 0x6B:            /* 0x9F6B */
          return 0x6B98;
        case 0x6C:            /* 0x9F6C */
          return 0x6B95;
        case 0x6D:            /* 0x9F6D */
          return 0x6B9E;
        case 0x6E:            /* 0x9F6E */
          return 0x6BA4;
        case 0x6F:            /* 0x9F6F */
          return 0x6BAA;
        case 0x70:            /* 0x9F70 */
          return 0x6BAB;
        case 0x71:            /* 0x9F71 */
          return 0x6BAF;
        case 0x72:            /* 0x9F72 */
          return 0x6BB2;
        case 0x73:            /* 0x9F73 */
          return 0x6BB1;
        case 0x74:            /* 0x9F74 */
          return 0x6BB3;
        case 0x75:            /* 0x9F75 */
          return 0x6BB7;
        case 0x76:            /* 0x9F76 */
          return 0x6BBC;
        case 0x77:            /* 0x9F77 */
          return 0x6BC6;
        case 0x78:            /* 0x9F78 */
          return 0x6BCB;
        case 0x79:            /* 0x9F79 */
          return 0x6BD3;
        case 0x7A:            /* 0x9F7A */
          return 0x6BDF;
        case 0x7B:            /* 0x9F7B */
          return 0x6BEC;
        case 0x7C:            /* 0x9F7C */
          return 0x6BEB;
        case 0x7D:            /* 0x9F7D */
          return 0x6BF3;
        case 0x7E:            /* 0x9F7E */
          return 0x6BEF;
        case 0x80:            /* 0x9F80 */
          return 0x9EBE;
        case 0x81:            /* 0x9F81 */
          return 0x6C08;
        case 0x82:            /* 0x9F82 */
          return 0x6C13;
        case 0x83:            /* 0x9F83 */
          return 0x6C14;
        case 0x84:            /* 0x9F84 */
          return 0x6C1B;
        case 0x85:            /* 0x9F85 */
          return 0x6C24;
        case 0x86:            /* 0x9F86 */
          return 0x6C23;
        case 0x87:            /* 0x9F87 */
          return 0x6C5E;
        case 0x88:            /* 0x9F88 */
          return 0x6C55;
        case 0x89:            /* 0x9F89 */
          return 0x6C62;
        case 0x8A:            /* 0x9F8A */
          return 0x6C6A;
        case 0x8B:            /* 0x9F8B */
          return 0x6C82;
        case 0x8C:            /* 0x9F8C */
          return 0x6C8D;
        case 0x8D:            /* 0x9F8D */
          return 0x6C9A;
        case 0x8E:            /* 0x9F8E */
          return 0x6C81;
        case 0x8F:            /* 0x9F8F */
          return 0x6C9B;
        case 0x90:            /* 0x9F90 */
          return 0x6C7E;
        case 0x91:            /* 0x9F91 */
          return 0x6C68;
        case 0x92:            /* 0x9F92 */
          return 0x6C73;
        case 0x93:            /* 0x9F93 */
          return 0x6C92;
        case 0x94:            /* 0x9F94 */
          return 0x6C90;
        case 0x95:            /* 0x9F95 */
          return 0x6CC4;
        case 0x96:            /* 0x9F96 */
          return 0x6CF1;
        case 0x97:            /* 0x9F97 */
          return 0x6CD3;
        case 0x98:            /* 0x9F98 */
          return 0x6CBD;
        case 0x99:            /* 0x9F99 */
          return 0x6CD7;
        case 0x9A:            /* 0x9F9A */
          return 0x6CC5;
        case 0x9B:            /* 0x9F9B */
          return 0x6CDD;
        case 0x9C:            /* 0x9F9C */
          return 0x6CAE;
        case 0x9D:            /* 0x9F9D */
          return 0x6CB1;
        case 0x9E:            /* 0x9F9E */
          return 0x6CBE;
        case 0x9F:            /* 0x9F9F */
          return 0x6CBA;
        case 0xA0:            /* 0x9FA0 */
          return 0x6CDB;
        case 0xA1:            /* 0x9FA1 */
          return 0x6CEF;
        case 0xA2:            /* 0x9FA2 */
          return 0x6CD9;
        case 0xA3:            /* 0x9FA3 */
          return 0x6CEA;
        case 0xA4:            /* 0x9FA4 */
          return 0x6D1F;
        case 0xA5:            /* 0x9FA5 */
          return 0x884D;
        case 0xA6:            /* 0x9FA6 */
          return 0x6D36;
        case 0xA7:            /* 0x9FA7 */
          return 0x6D2B;
        case 0xA8:            /* 0x9FA8 */
          return 0x6D3D;
        case 0xA9:            /* 0x9FA9 */
          return 0x6D38;
        case 0xAA:            /* 0x9FAA */
          return 0x6D19;
        case 0xAB:            /* 0x9FAB */
          return 0x6D35;
        case 0xAC:            /* 0x9FAC */
          return 0x6D33;
        case 0xAD:            /* 0x9FAD */
          return 0x6D12;
        case 0xAE:            /* 0x9FAE */
          return 0x6D0C;
        case 0xAF:            /* 0x9FAF */
          return 0x6D63;
        case 0xB0:            /* 0x9FB0 */
          return 0x6D93;
        case 0xB1:            /* 0x9FB1 */
          return 0x6D64;
        case 0xB2:            /* 0x9FB2 */
          return 0x6D5A;
        case 0xB3:            /* 0x9FB3 */
          return 0x6D79;
        case 0xB4:            /* 0x9FB4 */
          return 0x6D59;
        case 0xB5:            /* 0x9FB5 */
          return 0x6D8E;
        case 0xB6:            /* 0x9FB6 */
          return 0x6D95;
        case 0xB7:            /* 0x9FB7 */
          return 0x6FE4;
        case 0xB8:            /* 0x9FB8 */
          return 0x6D85;
        case 0xB9:            /* 0x9FB9 */
          return 0x6DF9;
        case 0xBA:            /* 0x9FBA */
          return 0x6E15;
        case 0xBB:            /* 0x9FBB */
          return 0x6E0A;
        case 0xBC:            /* 0x9FBC */
          return 0x6DB5;
        case 0xBD:            /* 0x9FBD */
          return 0x6DC7;
        case 0xBE:            /* 0x9FBE */
          return 0x6DE6;
        case 0xBF:            /* 0x9FBF */
          return 0x6DB8;
        case 0xC0:            /* 0x9FC0 */
          return 0x6DC6;
        case 0xC1:            /* 0x9FC1 */
          return 0x6DEC;
        case 0xC2:            /* 0x9FC2 */
          return 0x6DDE;
        case 0xC3:            /* 0x9FC3 */
          return 0x6DCC;
        case 0xC4:            /* 0x9FC4 */
          return 0x6DE8;
        case 0xC5:            /* 0x9FC5 */
          return 0x6DD2;
        case 0xC6:            /* 0x9FC6 */
          return 0x6DC5;
        case 0xC7:            /* 0x9FC7 */
          return 0x6DFA;
        case 0xC8:            /* 0x9FC8 */
          return 0x6DD9;
        case 0xC9:            /* 0x9FC9 */
          return 0x6DE4;
        case 0xCA:            /* 0x9FCA */
          return 0x6DD5;
        case 0xCB:            /* 0x9FCB */
          return 0x6DEA;
        case 0xCC:            /* 0x9FCC */
          return 0x6DEE;
        case 0xCD:            /* 0x9FCD */
          return 0x6E2D;
        case 0xCE:            /* 0x9FCE */
          return 0x6E6E;
        case 0xCF:            /* 0x9FCF */
          return 0x6E2E;
        case 0xD0:            /* 0x9FD0 */
          return 0x6E19;
        case 0xD1:            /* 0x9FD1 */
          return 0x6E72;
        case 0xD2:            /* 0x9FD2 */
          return 0x6E5F;
        case 0xD3:            /* 0x9FD3 */
          return 0x6E3E;
        case 0xD4:            /* 0x9FD4 */
          return 0x6E23;
        case 0xD5:            /* 0x9FD5 */
          return 0x6E6B;
        case 0xD6:            /* 0x9FD6 */
          return 0x6E2B;
        case 0xD7:            /* 0x9FD7 */
          return 0x6E76;
        case 0xD8:            /* 0x9FD8 */
          return 0x6E4D;
        case 0xD9:            /* 0x9FD9 */
          return 0x6E1F;
        case 0xDA:            /* 0x9FDA */
          return 0x6E43;
        case 0xDB:            /* 0x9FDB */
          return 0x6E3A;
        case 0xDC:            /* 0x9FDC */
          return 0x6E4E;
        case 0xDD:            /* 0x9FDD */
          return 0x6E24;
        case 0xDE:            /* 0x9FDE */
          return 0x6EFF;
        case 0xDF:            /* 0x9FDF */
          return 0x6E1D;
        case 0xE0:            /* 0x9FE0 */
          return 0x6E38;
        case 0xE1:            /* 0x9FE1 */
          return 0x6E82;
        case 0xE2:            /* 0x9FE2 */
          return 0x6EAA;
        case 0xE3:            /* 0x9FE3 */
          return 0x6E98;
        case 0xE4:            /* 0x9FE4 */
          return 0x6EC9;
        case 0xE5:            /* 0x9FE5 */
          return 0x6EB7;
        case 0xE6:            /* 0x9FE6 */
          return 0x6ED3;
        case 0xE7:            /* 0x9FE7 */
          return 0x6EBD;
        case 0xE8:            /* 0x9FE8 */
          return 0x6EAF;
        case 0xE9:            /* 0x9FE9 */
          return 0x6EC4;
        case 0xEA:            /* 0x9FEA */
          return 0x6EB2;
        case 0xEB:            /* 0x9FEB */
          return 0x6ED4;
        case 0xEC:            /* 0x9FEC */
          return 0x6ED5;
        case 0xED:            /* 0x9FED */
          return 0x6E8F;
        case 0xEE:            /* 0x9FEE */
          return 0x6EA5;
        case 0xEF:            /* 0x9FEF */
          return 0x6EC2;
        case 0xF0:            /* 0x9FF0 */
          return 0x6E9F;
        case 0xF1:            /* 0x9FF1 */
          return 0x6F41;
        case 0xF2:            /* 0x9FF2 */
          return 0x6F11;
        case 0xF3:            /* 0x9FF3 */
          return 0x704C;
        case 0xF4:            /* 0x9FF4 */
          return 0x6EEC;
        case 0xF5:            /* 0x9FF5 */
          return 0x6EF8;
        case 0xF6:            /* 0x9FF6 */
          return 0x6EFE;
        case 0xF7:            /* 0x9FF7 */
          return 0x6F3F;
        case 0xF8:            /* 0x9FF8 */
          return 0x6EF2;
        case 0xF9:            /* 0x9FF9 */
          return 0x6F31;
        case 0xFA:            /* 0x9FFA */
          return 0x6EEF;
        case 0xFB:            /* 0x9FFB */
          return 0x6F32;
        case 0xFC:            /* 0x9FFC */
          return 0x6ECC;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead 9F */

    /* Lead Byte: 0xE0xx */
    case 0xE0:
      switch (low_byte)
      {
        case 0x40:            /* 0xE040 */
          return 0x6F3E;
        case 0x41:            /* 0xE041 */
          return 0x6F13;
        case 0x42:            /* 0xE042 */
          return 0x6EF7;
        case 0x43:            /* 0xE043 */
          return 0x6F86;
        case 0x44:            /* 0xE044 */
          return 0x6F7A;
        case 0x45:            /* 0xE045 */
          return 0x6F78;
        case 0x46:            /* 0xE046 */
          return 0x6F81;
        case 0x47:            /* 0xE047 */
          return 0x6F80;
        case 0x48:            /* 0xE048 */
          return 0x6F6F;
        case 0x49:            /* 0xE049 */
          return 0x6F5B;
        case 0x4A:            /* 0xE04A */
          return 0x6FF3;
        case 0x4B:            /* 0xE04B */
          return 0x6F6D;
        case 0x4C:            /* 0xE04C */
          return 0x6F82;
        case 0x4D:            /* 0xE04D */
          return 0x6F7C;
        case 0x4E:            /* 0xE04E */
          return 0x6F58;
        case 0x4F:            /* 0xE04F */
          return 0x6F8E;
        case 0x50:            /* 0xE050 */
          return 0x6F91;
        case 0x51:            /* 0xE051 */
          return 0x6FC2;
        case 0x52:            /* 0xE052 */
          return 0x6F66;
        case 0x53:            /* 0xE053 */
          return 0x6FB3;
        case 0x54:            /* 0xE054 */
          return 0x6FA3;
        case 0x55:            /* 0xE055 */
          return 0x6FA1;
        case 0x56:            /* 0xE056 */
          return 0x6FA4;
        case 0x57:            /* 0xE057 */
          return 0x6FB9;
        case 0x58:            /* 0xE058 */
          return 0x6FC6;
        case 0x59:            /* 0xE059 */
          return 0x6FAA;
        case 0x5A:            /* 0xE05A */
          return 0x6FDF;
        case 0x5B:            /* 0xE05B */
          return 0x6FD5;
        case 0x5C:            /* 0xE05C */
          return 0x6FEC;
        case 0x5D:            /* 0xE05D */
          return 0x6FD4;
        case 0x5E:            /* 0xE05E */
          return 0x6FD8;
        case 0x5F:            /* 0xE05F */
          return 0x6FF1;
        case 0x60:            /* 0xE060 */
          return 0x6FEE;
        case 0x61:            /* 0xE061 */
          return 0x6FDB;
        case 0x62:            /* 0xE062 */
          return 0x7009;
        case 0x63:            /* 0xE063 */
          return 0x700B;
        case 0x64:            /* 0xE064 */
          return 0x6FFA;
        case 0x65:            /* 0xE065 */
          return 0x7011;
        case 0x66:            /* 0xE066 */
          return 0x7001;
        case 0x67:            /* 0xE067 */
          return 0x700F;
        case 0x68:            /* 0xE068 */
          return 0x6FFE;
        case 0x69:            /* 0xE069 */
          return 0x701B;
        case 0x6A:            /* 0xE06A */
          return 0x701A;
        case 0x6B:            /* 0xE06B */
          return 0x6F74;
        case 0x6C:            /* 0xE06C */
          return 0x701D;
        case 0x6D:            /* 0xE06D */
          return 0x7018;
        case 0x6E:            /* 0xE06E */
          return 0x701F;
        case 0x6F:            /* 0xE06F */
          return 0x7030;
        case 0x70:            /* 0xE070 */
          return 0x703E;
        case 0x71:            /* 0xE071 */
          return 0x7032;
        case 0x72:            /* 0xE072 */
          return 0x7051;
        case 0x73:            /* 0xE073 */
          return 0x7063;
        case 0x74:            /* 0xE074 */
          return 0x7099;
        case 0x75:            /* 0xE075 */
          return 0x7092;
        case 0x76:            /* 0xE076 */
          return 0x70AF;
        case 0x77:            /* 0xE077 */
          return 0x70F1;
        case 0x78:            /* 0xE078 */
          return 0x70AC;
        case 0x79:            /* 0xE079 */
          return 0x70B8;
        case 0x7A:            /* 0xE07A */
          return 0x70B3;
        case 0x7B:            /* 0xE07B */
          return 0x70AE;
        case 0x7C:            /* 0xE07C */
          return 0x70DF;
        case 0x7D:            /* 0xE07D */
          return 0x70CB;
        case 0x7E:            /* 0xE07E */
          return 0x70DD;
        case 0x80:            /* 0xE080 */
          return 0x70D9;
        case 0x81:            /* 0xE081 */
          return 0x7109;
        case 0x82:            /* 0xE082 */
          return 0x70FD;
        case 0x83:            /* 0xE083 */
          return 0x711C;
        case 0x84:            /* 0xE084 */
          return 0x7119;
        case 0x85:            /* 0xE085 */
          return 0x7165;
        case 0x86:            /* 0xE086 */
          return 0x7155;
        case 0x87:            /* 0xE087 */
          return 0x7188;
        case 0x88:            /* 0xE088 */
          return 0x7166;
        case 0x89:            /* 0xE089 */
          return 0x7162;
        case 0x8A:            /* 0xE08A */
          return 0x714C;
        case 0x8B:            /* 0xE08B */
          return 0x7156;
        case 0x8C:            /* 0xE08C */
          return 0x716C;
        case 0x8D:            /* 0xE08D */
          return 0x718F;
        case 0x8E:            /* 0xE08E */
          return 0x71FB;
        case 0x8F:            /* 0xE08F */
          return 0x7184;
        case 0x90:            /* 0xE090 */
          return 0x7195;
        case 0x91:            /* 0xE091 */
          return 0x71A8;
        case 0x92:            /* 0xE092 */
          return 0x71AC;
        case 0x93:            /* 0xE093 */
          return 0x71D7;
        case 0x94:            /* 0xE094 */
          return 0x71B9;
        case 0x95:            /* 0xE095 */
          return 0x71BE;
        case 0x96:            /* 0xE096 */
          return 0x71D2;
        case 0x97:            /* 0xE097 */
          return 0x71C9;
        case 0x98:            /* 0xE098 */
          return 0x71D4;
        case 0x99:            /* 0xE099 */
          return 0x71CE;
        case 0x9A:            /* 0xE09A */
          return 0x71E0;
        case 0x9B:            /* 0xE09B */
          return 0x71EC;
        case 0x9C:            /* 0xE09C */
          return 0x71E7;
        case 0x9D:            /* 0xE09D */
          return 0x71F5;
        case 0x9E:            /* 0xE09E */
          return 0x71FC;
        case 0x9F:            /* 0xE09F */
          return 0x71F9;
        case 0xA0:            /* 0xE0A0 */
          return 0x71FF;
        case 0xA1:            /* 0xE0A1 */
          return 0x720D;
        case 0xA2:            /* 0xE0A2 */
          return 0x7210;
        case 0xA3:            /* 0xE0A3 */
          return 0x721B;
        case 0xA4:            /* 0xE0A4 */
          return 0x7228;
        case 0xA5:            /* 0xE0A5 */
          return 0x722D;
        case 0xA6:            /* 0xE0A6 */
          return 0x722C;
        case 0xA7:            /* 0xE0A7 */
          return 0x7230;
        case 0xA8:            /* 0xE0A8 */
          return 0x7232;
        case 0xA9:            /* 0xE0A9 */
          return 0x723B;
        case 0xAA:            /* 0xE0AA */
          return 0x723C;
        case 0xAB:            /* 0xE0AB */
          return 0x723F;
        case 0xAC:            /* 0xE0AC */
          return 0x7240;
        case 0xAD:            /* 0xE0AD */
          return 0x7246;
        case 0xAE:            /* 0xE0AE */
          return 0x724B;
        case 0xAF:            /* 0xE0AF */
          return 0x7258;
        case 0xB0:            /* 0xE0B0 */
          return 0x7274;
        case 0xB1:            /* 0xE0B1 */
          return 0x727E;
        case 0xB2:            /* 0xE0B2 */
          return 0x7282;
        case 0xB3:            /* 0xE0B3 */
          return 0x7281;
        case 0xB4:            /* 0xE0B4 */
          return 0x7287;
        case 0xB5:            /* 0xE0B5 */
          return 0x7292;
        case 0xB6:            /* 0xE0B6 */
          return 0x7296;
        case 0xB7:            /* 0xE0B7 */
          return 0x72A2;
        case 0xB8:            /* 0xE0B8 */
          return 0x72A7;
        case 0xB9:            /* 0xE0B9 */
          return 0x72B9;
        case 0xBA:            /* 0xE0BA */
          return 0x72B2;
        case 0xBB:            /* 0xE0BB */
          return 0x72C3;
        case 0xBC:            /* 0xE0BC */
          return 0x72C6;
        case 0xBD:            /* 0xE0BD */
          return 0x72C4;
        case 0xBE:            /* 0xE0BE */
          return 0x72CE;
        case 0xBF:            /* 0xE0BF */
          return 0x72D2;
        case 0xC0:            /* 0xE0C0 */
          return 0x72E2;
        case 0xC1:            /* 0xE0C1 */
          return 0x72E0;
        case 0xC2:            /* 0xE0C2 */
          return 0x72E1;
        case 0xC3:            /* 0xE0C3 */
          return 0x72F9;
        case 0xC4:            /* 0xE0C4 */
          return 0x72F7;
        case 0xC5:            /* 0xE0C5 */
          return 0x500F;
        case 0xC6:            /* 0xE0C6 */
          return 0x7317;
        case 0xC7:            /* 0xE0C7 */
          return 0x730A;
        case 0xC8:            /* 0xE0C8 */
          return 0x731C;
        case 0xC9:            /* 0xE0C9 */
          return 0x7316;
        case 0xCA:            /* 0xE0CA */
          return 0x731D;
        case 0xCB:            /* 0xE0CB */
          return 0x7334;
        case 0xCC:            /* 0xE0CC */
          return 0x732F;
        case 0xCD:            /* 0xE0CD */
          return 0x7329;
        case 0xCE:            /* 0xE0CE */
          return 0x7325;
        case 0xCF:            /* 0xE0CF */
          return 0x733E;
        case 0xD0:            /* 0xE0D0 */
          return 0x734E;
        case 0xD1:            /* 0xE0D1 */
          return 0x734F;
        case 0xD2:            /* 0xE0D2 */
          return 0x9ED8;
        case 0xD3:            /* 0xE0D3 */
          return 0x7357;
        case 0xD4:            /* 0xE0D4 */
          return 0x736A;
        case 0xD5:            /* 0xE0D5 */
          return 0x7368;
        case 0xD6:            /* 0xE0D6 */
          return 0x7370;
        case 0xD7:            /* 0xE0D7 */
          return 0x7378;
        case 0xD8:            /* 0xE0D8 */
          return 0x7375;
        case 0xD9:            /* 0xE0D9 */
          return 0x737B;
        case 0xDA:            /* 0xE0DA */
          return 0x737A;
        case 0xDB:            /* 0xE0DB */
          return 0x73C8;
        case 0xDC:            /* 0xE0DC */
          return 0x73B3;
        case 0xDD:            /* 0xE0DD */
          return 0x73CE;
        case 0xDE:            /* 0xE0DE */
          return 0x73BB;
        case 0xDF:            /* 0xE0DF */
          return 0x73C0;
        case 0xE0:            /* 0xE0E0 */
          return 0x73E5;
        case 0xE1:            /* 0xE0E1 */
          return 0x73EE;
        case 0xE2:            /* 0xE0E2 */
          return 0x73DE;
        case 0xE3:            /* 0xE0E3 */
          return 0x74A2;
        case 0xE4:            /* 0xE0E4 */
          return 0x7405;
        case 0xE5:            /* 0xE0E5 */
          return 0x746F;
        case 0xE6:            /* 0xE0E6 */
          return 0x7425;
        case 0xE7:            /* 0xE0E7 */
          return 0x73F8;
        case 0xE8:            /* 0xE0E8 */
          return 0x7432;
        case 0xE9:            /* 0xE0E9 */
          return 0x743A;
        case 0xEA:            /* 0xE0EA */
          return 0x7455;
        case 0xEB:            /* 0xE0EB */
          return 0x743F;
        case 0xEC:            /* 0xE0EC */
          return 0x745F;
        case 0xED:            /* 0xE0ED */
          return 0x7459;
        case 0xEE:            /* 0xE0EE */
          return 0x7441;
        case 0xEF:            /* 0xE0EF */
          return 0x745C;
        case 0xF0:            /* 0xE0F0 */
          return 0x7469;
        case 0xF1:            /* 0xE0F1 */
          return 0x7470;
        case 0xF2:            /* 0xE0F2 */
          return 0x7463;
        case 0xF3:            /* 0xE0F3 */
          return 0x746A;
        case 0xF4:            /* 0xE0F4 */
          return 0x7476;
        case 0xF5:            /* 0xE0F5 */
          return 0x747E;
        case 0xF6:            /* 0xE0F6 */
          return 0x748B;
        case 0xF7:            /* 0xE0F7 */
          return 0x749E;
        case 0xF8:            /* 0xE0F8 */
          return 0x74A7;
        case 0xF9:            /* 0xE0F9 */
          return 0x74CA;
        case 0xFA:            /* 0xE0FA */
          return 0x74CF;
        case 0xFB:            /* 0xE0FB */
          return 0x74D4;
        case 0xFC:            /* 0xE0FC */
          return 0x73F1;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead E0 */

    /* Lead Byte: 0xE1xx */
    case 0xE1:
      switch (low_byte)
      {
        case 0x40:            /* 0xE140 */
          return 0x74E0;
        case 0x41:            /* 0xE141 */
          return 0x74E3;
        case 0x42:            /* 0xE142 */
          return 0x74E7;
        case 0x43:            /* 0xE143 */
          return 0x74E9;
        case 0x44:            /* 0xE144 */
          return 0x74EE;
        case 0x45:            /* 0xE145 */
          return 0x74F2;
        case 0x46:            /* 0xE146 */
          return 0x74F0;
        case 0x47:            /* 0xE147 */
          return 0x74F1;
        case 0x48:            /* 0xE148 */
          return 0x74F8;
        case 0x49:            /* 0xE149 */
          return 0x74F7;
        case 0x4A:            /* 0xE14A */
          return 0x7504;
        case 0x4B:            /* 0xE14B */
          return 0x7503;
        case 0x4C:            /* 0xE14C */
          return 0x7505;
        case 0x4D:            /* 0xE14D */
          return 0x750C;
        case 0x4E:            /* 0xE14E */
          return 0x750E;
        case 0x4F:            /* 0xE14F */
          return 0x750D;
        case 0x50:            /* 0xE150 */
          return 0x7515;
        case 0x51:            /* 0xE151 */
          return 0x7513;
        case 0x52:            /* 0xE152 */
          return 0x751E;
        case 0x53:            /* 0xE153 */
          return 0x7526;
        case 0x54:            /* 0xE154 */
          return 0x752C;
        case 0x55:            /* 0xE155 */
          return 0x753C;
        case 0x56:            /* 0xE156 */
          return 0x7544;
        case 0x57:            /* 0xE157 */
          return 0x754D;
        case 0x58:            /* 0xE158 */
          return 0x754A;
        case 0x59:            /* 0xE159 */
          return 0x7549;
        case 0x5A:            /* 0xE15A */
          return 0x755B;
        case 0x5B:            /* 0xE15B */
          return 0x7546;
        case 0x5C:            /* 0xE15C */
          return 0x755A;
        case 0x5D:            /* 0xE15D */
          return 0x7569;
        case 0x5E:            /* 0xE15E */
          return 0x7564;
        case 0x5F:            /* 0xE15F */
          return 0x7567;
        case 0x60:            /* 0xE160 */
          return 0x756B;
        case 0x61:            /* 0xE161 */
          return 0x756D;
        case 0x62:            /* 0xE162 */
          return 0x7578;
        case 0x63:            /* 0xE163 */
          return 0x7576;
        case 0x64:            /* 0xE164 */
          return 0x7586;
        case 0x65:            /* 0xE165 */
          return 0x7587;
        case 0x66:            /* 0xE166 */
          return 0x7574;
        case 0x67:            /* 0xE167 */
          return 0x758A;
        case 0x68:            /* 0xE168 */
          return 0x7589;
        case 0x69:            /* 0xE169 */
          return 0x7582;
        case 0x6A:            /* 0xE16A */
          return 0x7594;
        case 0x6B:            /* 0xE16B */
          return 0x759A;
        case 0x6C:            /* 0xE16C */
          return 0x759D;
        case 0x6D:            /* 0xE16D */
          return 0x75A5;
        case 0x6E:            /* 0xE16E */
          return 0x75A3;
        case 0x6F:            /* 0xE16F */
          return 0x75C2;
        case 0x70:            /* 0xE170 */
          return 0x75B3;
        case 0x71:            /* 0xE171 */
          return 0x75C3;
        case 0x72:            /* 0xE172 */
          return 0x75B5;
        case 0x73:            /* 0xE173 */
          return 0x75BD;
        case 0x74:            /* 0xE174 */
          return 0x75B8;
        case 0x75:            /* 0xE175 */
          return 0x75BC;
        case 0x76:            /* 0xE176 */
          return 0x75B1;
        case 0x77:            /* 0xE177 */
          return 0x75CD;
        case 0x78:            /* 0xE178 */
          return 0x75CA;
        case 0x79:            /* 0xE179 */
          return 0x75D2;
        case 0x7A:            /* 0xE17A */
          return 0x75D9;
        case 0x7B:            /* 0xE17B */
          return 0x75E3;
        case 0x7C:            /* 0xE17C */
          return 0x75DE;
        case 0x7D:            /* 0xE17D */
          return 0x75FE;
        case 0x7E:            /* 0xE17E */
          return 0x75FF;
        case 0x80:            /* 0xE180 */
          return 0x75FC;
        case 0x81:            /* 0xE181 */
          return 0x7601;
        case 0x82:            /* 0xE182 */
          return 0x75F0;
        case 0x83:            /* 0xE183 */
          return 0x75FA;
        case 0x84:            /* 0xE184 */
          return 0x75F2;
        case 0x85:            /* 0xE185 */
          return 0x75F3;
        case 0x86:            /* 0xE186 */
          return 0x760B;
        case 0x87:            /* 0xE187 */
          return 0x760D;
        case 0x88:            /* 0xE188 */
          return 0x7609;
        case 0x89:            /* 0xE189 */
          return 0x761F;
        case 0x8A:            /* 0xE18A */
          return 0x7627;
        case 0x8B:            /* 0xE18B */
          return 0x7620;
        case 0x8C:            /* 0xE18C */
          return 0x7621;
        case 0x8D:            /* 0xE18D */
          return 0x7622;
        case 0x8E:            /* 0xE18E */
          return 0x7624;
        case 0x8F:            /* 0xE18F */
          return 0x7634;
        case 0x90:            /* 0xE190 */
          return 0x7630;
        case 0x91:            /* 0xE191 */
          return 0x763B;
        case 0x92:            /* 0xE192 */
          return 0x7647;
        case 0x93:            /* 0xE193 */
          return 0x7648;
        case 0x94:            /* 0xE194 */
          return 0x7646;
        case 0x95:            /* 0xE195 */
          return 0x765C;
        case 0x96:            /* 0xE196 */
          return 0x7658;
        case 0x97:            /* 0xE197 */
          return 0x7661;
        case 0x98:            /* 0xE198 */
          return 0x7662;
        case 0x99:            /* 0xE199 */
          return 0x7668;
        case 0x9A:            /* 0xE19A */
          return 0x7669;
        case 0x9B:            /* 0xE19B */
          return 0x766A;
        case 0x9C:            /* 0xE19C */
          return 0x7667;
        case 0x9D:            /* 0xE19D */
          return 0x766C;
        case 0x9E:            /* 0xE19E */
          return 0x7670;
        case 0x9F:            /* 0xE19F */
          return 0x7672;
        case 0xA0:            /* 0xE1A0 */
          return 0x7676;
        case 0xA1:            /* 0xE1A1 */
          return 0x7678;
        case 0xA2:            /* 0xE1A2 */
          return 0x767C;
        case 0xA3:            /* 0xE1A3 */
          return 0x7680;
        case 0xA4:            /* 0xE1A4 */
          return 0x7683;
        case 0xA5:            /* 0xE1A5 */
          return 0x7688;
        case 0xA6:            /* 0xE1A6 */
          return 0x768B;
        case 0xA7:            /* 0xE1A7 */
          return 0x768E;
        case 0xA8:            /* 0xE1A8 */
          return 0x7696;
        case 0xA9:            /* 0xE1A9 */
          return 0x7693;
        case 0xAA:            /* 0xE1AA */
          return 0x7699;
        case 0xAB:            /* 0xE1AB */
          return 0x769A;
        case 0xAC:            /* 0xE1AC */
          return 0x76B0;
        case 0xAD:            /* 0xE1AD */
          return 0x76B4;
        case 0xAE:            /* 0xE1AE */
          return 0x76B8;
        case 0xAF:            /* 0xE1AF */
          return 0x76B9;
        case 0xB0:            /* 0xE1B0 */
          return 0x76BA;
        case 0xB1:            /* 0xE1B1 */
          return 0x76C2;
        case 0xB2:            /* 0xE1B2 */
          return 0x76CD;
        case 0xB3:            /* 0xE1B3 */
          return 0x76D6;
        case 0xB4:            /* 0xE1B4 */
          return 0x76D2;
        case 0xB5:            /* 0xE1B5 */
          return 0x76DE;
        case 0xB6:            /* 0xE1B6 */
          return 0x76E1;
        case 0xB7:            /* 0xE1B7 */
          return 0x76E5;
        case 0xB8:            /* 0xE1B8 */
          return 0x76E7;
        case 0xB9:            /* 0xE1B9 */
          return 0x76EA;
        case 0xBA:            /* 0xE1BA */
          return 0x862F;
        case 0xBB:            /* 0xE1BB */
          return 0x76FB;
        case 0xBC:            /* 0xE1BC */
          return 0x7708;
        case 0xBD:            /* 0xE1BD */
          return 0x7707;
        case 0xBE:            /* 0xE1BE */
          return 0x7704;
        case 0xBF:            /* 0xE1BF */
          return 0x7729;
        case 0xC0:            /* 0xE1C0 */
          return 0x7724;
        case 0xC1:            /* 0xE1C1 */
          return 0x771E;
        case 0xC2:            /* 0xE1C2 */
          return 0x7725;
        case 0xC3:            /* 0xE1C3 */
          return 0x7726;
        case 0xC4:            /* 0xE1C4 */
          return 0x771B;
        case 0xC5:            /* 0xE1C5 */
          return 0x7737;
        case 0xC6:            /* 0xE1C6 */
          return 0x7738;
        case 0xC7:            /* 0xE1C7 */
          return 0x7747;
        case 0xC8:            /* 0xE1C8 */
          return 0x775A;
        case 0xC9:            /* 0xE1C9 */
          return 0x7768;
        case 0xCA:            /* 0xE1CA */
          return 0x776B;
        case 0xCB:            /* 0xE1CB */
          return 0x775B;
        case 0xCC:            /* 0xE1CC */
          return 0x7765;
        case 0xCD:            /* 0xE1CD */
          return 0x777F;
        case 0xCE:            /* 0xE1CE */
          return 0x777E;
        case 0xCF:            /* 0xE1CF */
          return 0x7779;
        case 0xD0:            /* 0xE1D0 */
          return 0x778E;
        case 0xD1:            /* 0xE1D1 */
          return 0x778B;
        case 0xD2:            /* 0xE1D2 */
          return 0x7791;
        case 0xD3:            /* 0xE1D3 */
          return 0x77A0;
        case 0xD4:            /* 0xE1D4 */
          return 0x779E;
        case 0xD5:            /* 0xE1D5 */
          return 0x77B0;
        case 0xD6:            /* 0xE1D6 */
          return 0x77B6;
        case 0xD7:            /* 0xE1D7 */
          return 0x77B9;
        case 0xD8:            /* 0xE1D8 */
          return 0x77BF;
        case 0xD9:            /* 0xE1D9 */
          return 0x77BC;
        case 0xDA:            /* 0xE1DA */
          return 0x77BD;
        case 0xDB:            /* 0xE1DB */
          return 0x77BB;
        case 0xDC:            /* 0xE1DC */
          return 0x77C7;
        case 0xDD:            /* 0xE1DD */
          return 0x77CD;
        case 0xDE:            /* 0xE1DE */
          return 0x77D7;
        case 0xDF:            /* 0xE1DF */
          return 0x77DA;
        case 0xE0:            /* 0xE1E0 */
          return 0x77DC;
        case 0xE1:            /* 0xE1E1 */
          return 0x77E3;
        case 0xE2:            /* 0xE1E2 */
          return 0x77EE;
        case 0xE3:            /* 0xE1E3 */
          return 0x77FC;
        case 0xE4:            /* 0xE1E4 */
          return 0x780C;
        case 0xE5:            /* 0xE1E5 */
          return 0x7812;
        case 0xE6:            /* 0xE1E6 */
          return 0x7926;
        case 0xE7:            /* 0xE1E7 */
          return 0x7820;
        case 0xE8:            /* 0xE1E8 */
          return 0x792A;
        case 0xE9:            /* 0xE1E9 */
          return 0x7845;
        case 0xEA:            /* 0xE1EA */
          return 0x788E;
        case 0xEB:            /* 0xE1EB */
          return 0x7874;
        case 0xEC:            /* 0xE1EC */
          return 0x7886;
        case 0xED:            /* 0xE1ED */
          return 0x787C;
        case 0xEE:            /* 0xE1EE */
          return 0x789A;
        case 0xEF:            /* 0xE1EF */
          return 0x788C;
        case 0xF0:            /* 0xE1F0 */
          return 0x78A3;
        case 0xF1:            /* 0xE1F1 */
          return 0x78B5;
        case 0xF2:            /* 0xE1F2 */
          return 0x78AA;
        case 0xF3:            /* 0xE1F3 */
          return 0x78AF;
        case 0xF4:            /* 0xE1F4 */
          return 0x78D1;
        case 0xF5:            /* 0xE1F5 */
          return 0x78C6;
        case 0xF6:            /* 0xE1F6 */
          return 0x78CB;
        case 0xF7:            /* 0xE1F7 */
          return 0x78D4;
        case 0xF8:            /* 0xE1F8 */
          return 0x78BE;
        case 0xF9:            /* 0xE1F9 */
          return 0x78BC;
        case 0xFA:            /* 0xE1FA */
          return 0x78C5;
        case 0xFB:            /* 0xE1FB */
          return 0x78CA;
        case 0xFC:            /* 0xE1FC */
          return 0x78EC;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead E1 */

    /* Lead Byte: 0xE2xx */
    case 0xE2:
      switch (low_byte)
      {
        case 0x40:            /* 0xE240 */
          return 0x78E7;
        case 0x41:            /* 0xE241 */
          return 0x78DA;
        case 0x42:            /* 0xE242 */
          return 0x78FD;
        case 0x43:            /* 0xE243 */
          return 0x78F4;
        case 0x44:            /* 0xE244 */
          return 0x7907;
        case 0x45:            /* 0xE245 */
          return 0x7912;
        case 0x46:            /* 0xE246 */
          return 0x7911;
        case 0x47:            /* 0xE247 */
          return 0x7919;
        case 0x48:            /* 0xE248 */
          return 0x792C;
        case 0x49:            /* 0xE249 */
          return 0x792B;
        case 0x4A:            /* 0xE24A */
          return 0x7940;
        case 0x4B:            /* 0xE24B */
          return 0x7960;
        case 0x4C:            /* 0xE24C */
          return 0x7957;
        case 0x4D:            /* 0xE24D */
          return 0x795F;
        case 0x4E:            /* 0xE24E */
          return 0x795A;
        case 0x4F:            /* 0xE24F */
          return 0x7955;
        case 0x50:            /* 0xE250 */
          return 0x7953;
        case 0x51:            /* 0xE251 */
          return 0x797A;
        case 0x52:            /* 0xE252 */
          return 0x797F;
        case 0x53:            /* 0xE253 */
          return 0x798A;
        case 0x54:            /* 0xE254 */
          return 0x799D;
        case 0x55:            /* 0xE255 */
          return 0x79A7;
        case 0x56:            /* 0xE256 */
          return 0x9F4B;
        case 0x57:            /* 0xE257 */
          return 0x79AA;
        case 0x58:            /* 0xE258 */
          return 0x79AE;
        case 0x59:            /* 0xE259 */
          return 0x79B3;
        case 0x5A:            /* 0xE25A */
          return 0x79B9;
        case 0x5B:            /* 0xE25B */
          return 0x79BA;
        case 0x5C:            /* 0xE25C */
          return 0x79C9;
        case 0x5D:            /* 0xE25D */
          return 0x79D5;
        case 0x5E:            /* 0xE25E */
          return 0x79E7;
        case 0x5F:            /* 0xE25F */
          return 0x79EC;
        case 0x60:            /* 0xE260 */
          return 0x79E1;
        case 0x61:            /* 0xE261 */
          return 0x79E3;
        case 0x62:            /* 0xE262 */
          return 0x7A08;
        case 0x63:            /* 0xE263 */
          return 0x7A0D;
        case 0x64:            /* 0xE264 */
          return 0x7A18;
        case 0x65:            /* 0xE265 */
          return 0x7A19;
        case 0x66:            /* 0xE266 */
          return 0x7A20;
        case 0x67:            /* 0xE267 */
          return 0x7A1F;
        case 0x68:            /* 0xE268 */
          return 0x7980;
        case 0x69:            /* 0xE269 */
          return 0x7A31;
        case 0x6A:            /* 0xE26A */
          return 0x7A3B;
        case 0x6B:            /* 0xE26B */
          return 0x7A3E;
        case 0x6C:            /* 0xE26C */
          return 0x7A37;
        case 0x6D:            /* 0xE26D */
          return 0x7A43;
        case 0x6E:            /* 0xE26E */
          return 0x7A57;
        case 0x6F:            /* 0xE26F */
          return 0x7A49;
        case 0x70:            /* 0xE270 */
          return 0x7A61;
        case 0x71:            /* 0xE271 */
          return 0x7A62;
        case 0x72:            /* 0xE272 */
          return 0x7A69;
        case 0x73:            /* 0xE273 */
          return 0x9F9D;
        case 0x74:            /* 0xE274 */
          return 0x7A70;
        case 0x75:            /* 0xE275 */
          return 0x7A79;
        case 0x76:            /* 0xE276 */
          return 0x7A7D;
        case 0x77:            /* 0xE277 */
          return 0x7A88;
        case 0x78:            /* 0xE278 */
          return 0x7A97;
        case 0x79:            /* 0xE279 */
          return 0x7A95;
        case 0x7A:            /* 0xE27A */
          return 0x7A98;
        case 0x7B:            /* 0xE27B */
          return 0x7A96;
        case 0x7C:            /* 0xE27C */
          return 0x7AA9;
        case 0x7D:            /* 0xE27D */
          return 0x7AC8;
        case 0x7E:            /* 0xE27E */
          return 0x7AB0;
        case 0x80:            /* 0xE280 */
          return 0x7AB6;
        case 0x81:            /* 0xE281 */
          return 0x7AC5;
        case 0x82:            /* 0xE282 */
          return 0x7AC4;
        case 0x83:            /* 0xE283 */
          return 0x7ABF;
        case 0x84:            /* 0xE284 */
          return 0x9083;
        case 0x85:            /* 0xE285 */
          return 0x7AC7;
        case 0x86:            /* 0xE286 */
          return 0x7ACA;
        case 0x87:            /* 0xE287 */
          return 0x7ACD;
        case 0x88:            /* 0xE288 */
          return 0x7ACF;
        case 0x89:            /* 0xE289 */
          return 0x7AD5;
        case 0x8A:            /* 0xE28A */
          return 0x7AD3;
        case 0x8B:            /* 0xE28B */
          return 0x7AD9;
        case 0x8C:            /* 0xE28C */
          return 0x7ADA;
        case 0x8D:            /* 0xE28D */
          return 0x7ADD;
        case 0x8E:            /* 0xE28E */
          return 0x7AE1;
        case 0x8F:            /* 0xE28F */
          return 0x7AE2;
        case 0x90:            /* 0xE290 */
          return 0x7AE6;
        case 0x91:            /* 0xE291 */
          return 0x7AED;
        case 0x92:            /* 0xE292 */
          return 0x7AF0;
        case 0x93:            /* 0xE293 */
          return 0x7B02;
        case 0x94:            /* 0xE294 */
          return 0x7B0F;
        case 0x95:            /* 0xE295 */
          return 0x7B0A;
        case 0x96:            /* 0xE296 */
          return 0x7B06;
        case 0x97:            /* 0xE297 */
          return 0x7B33;
        case 0x98:            /* 0xE298 */
          return 0x7B18;
        case 0x99:            /* 0xE299 */
          return 0x7B19;
        case 0x9A:            /* 0xE29A */
          return 0x7B1E;
        case 0x9B:            /* 0xE29B */
          return 0x7B35;
        case 0x9C:            /* 0xE29C */
          return 0x7B28;
        case 0x9D:            /* 0xE29D */
          return 0x7B36;
        case 0x9E:            /* 0xE29E */
          return 0x7B50;
        case 0x9F:            /* 0xE29F */
          return 0x7B7A;
        case 0xA0:            /* 0xE2A0 */
          return 0x7B04;
        case 0xA1:            /* 0xE2A1 */
          return 0x7B4D;
        case 0xA2:            /* 0xE2A2 */
          return 0x7B0B;
        case 0xA3:            /* 0xE2A3 */
          return 0x7B4C;
        case 0xA4:            /* 0xE2A4 */
          return 0x7B45;
        case 0xA5:            /* 0xE2A5 */
          return 0x7B75;
        case 0xA6:            /* 0xE2A6 */
          return 0x7B65;
        case 0xA7:            /* 0xE2A7 */
          return 0x7B74;
        case 0xA8:            /* 0xE2A8 */
          return 0x7B67;
        case 0xA9:            /* 0xE2A9 */
          return 0x7B70;
        case 0xAA:            /* 0xE2AA */
          return 0x7B71;
        case 0xAB:            /* 0xE2AB */
          return 0x7B6C;
        case 0xAC:            /* 0xE2AC */
          return 0x7B6E;
        case 0xAD:            /* 0xE2AD */
          return 0x7B9D;
        case 0xAE:            /* 0xE2AE */
          return 0x7B98;
        case 0xAF:            /* 0xE2AF */
          return 0x7B9F;
        case 0xB0:            /* 0xE2B0 */
          return 0x7B8D;
        case 0xB1:            /* 0xE2B1 */
          return 0x7B9C;
        case 0xB2:            /* 0xE2B2 */
          return 0x7B9A;
        case 0xB3:            /* 0xE2B3 */
          return 0x7B8B;
        case 0xB4:            /* 0xE2B4 */
          return 0x7B92;
        case 0xB5:            /* 0xE2B5 */
          return 0x7B8F;
        case 0xB6:            /* 0xE2B6 */
          return 0x7B5D;
        case 0xB7:            /* 0xE2B7 */
          return 0x7B99;
        case 0xB8:            /* 0xE2B8 */
          return 0x7BCB;
        case 0xB9:            /* 0xE2B9 */
          return 0x7BC1;
        case 0xBA:            /* 0xE2BA */
          return 0x7BCC;
        case 0xBB:            /* 0xE2BB */
          return 0x7BCF;
        case 0xBC:            /* 0xE2BC */
          return 0x7BB4;
        case 0xBD:            /* 0xE2BD */
          return 0x7BC6;
        case 0xBE:            /* 0xE2BE */
          return 0x7BDD;
        case 0xBF:            /* 0xE2BF */
          return 0x7BE9;
        case 0xC0:            /* 0xE2C0 */
          return 0x7C11;
        case 0xC1:            /* 0xE2C1 */
          return 0x7C14;
        case 0xC2:            /* 0xE2C2 */
          return 0x7BE6;
        case 0xC3:            /* 0xE2C3 */
          return 0x7BE5;
        case 0xC4:            /* 0xE2C4 */
          return 0x7C60;
        case 0xC5:            /* 0xE2C5 */
          return 0x7C00;
        case 0xC6:            /* 0xE2C6 */
          return 0x7C07;
        case 0xC7:            /* 0xE2C7 */
          return 0x7C13;
        case 0xC8:            /* 0xE2C8 */
          return 0x7BF3;
        case 0xC9:            /* 0xE2C9 */
          return 0x7BF7;
        case 0xCA:            /* 0xE2CA */
          return 0x7C17;
        case 0xCB:            /* 0xE2CB */
          return 0x7C0D;
        case 0xCC:            /* 0xE2CC */
          return 0x7BF6;
        case 0xCD:            /* 0xE2CD */
          return 0x7C23;
        case 0xCE:            /* 0xE2CE */
          return 0x7C27;
        case 0xCF:            /* 0xE2CF */
          return 0x7C2A;
        case 0xD0:            /* 0xE2D0 */
          return 0x7C1F;
        case 0xD1:            /* 0xE2D1 */
          return 0x7C37;
        case 0xD2:            /* 0xE2D2 */
          return 0x7C2B;
        case 0xD3:            /* 0xE2D3 */
          return 0x7C3D;
        case 0xD4:            /* 0xE2D4 */
          return 0x7C4C;
        case 0xD5:            /* 0xE2D5 */
          return 0x7C43;
        case 0xD6:            /* 0xE2D6 */
          return 0x7C54;
        case 0xD7:            /* 0xE2D7 */
          return 0x7C4F;
        case 0xD8:            /* 0xE2D8 */
          return 0x7C40;
        case 0xD9:            /* 0xE2D9 */
          return 0x7C50;
        case 0xDA:            /* 0xE2DA */
          return 0x7C58;
        case 0xDB:            /* 0xE2DB */
          return 0x7C5F;
        case 0xDC:            /* 0xE2DC */
          return 0x7C64;
        case 0xDD:            /* 0xE2DD */
          return 0x7C56;
        case 0xDE:            /* 0xE2DE */
          return 0x7C65;
        case 0xDF:            /* 0xE2DF */
          return 0x7C6C;
        case 0xE0:            /* 0xE2E0 */
          return 0x7C75;
        case 0xE1:            /* 0xE2E1 */
          return 0x7C83;
        case 0xE2:            /* 0xE2E2 */
          return 0x7C90;
        case 0xE3:            /* 0xE2E3 */
          return 0x7CA4;
        case 0xE4:            /* 0xE2E4 */
          return 0x7CAD;
        case 0xE5:            /* 0xE2E5 */
          return 0x7CA2;
        case 0xE6:            /* 0xE2E6 */
          return 0x7CAB;
        case 0xE7:            /* 0xE2E7 */
          return 0x7CA1;
        case 0xE8:            /* 0xE2E8 */
          return 0x7CA8;
        case 0xE9:            /* 0xE2E9 */
          return 0x7CB3;
        case 0xEA:            /* 0xE2EA */
          return 0x7CB2;
        case 0xEB:            /* 0xE2EB */
          return 0x7CB1;
        case 0xEC:            /* 0xE2EC */
          return 0x7CAE;
        case 0xED:            /* 0xE2ED */
          return 0x7CB9;
        case 0xEE:            /* 0xE2EE */
          return 0x7CBD;
        case 0xEF:            /* 0xE2EF */
          return 0x7CC0;
        case 0xF0:            /* 0xE2F0 */
          return 0x7CC5;
        case 0xF1:            /* 0xE2F1 */
          return 0x7CC2;
        case 0xF2:            /* 0xE2F2 */
          return 0x7CD8;
        case 0xF3:            /* 0xE2F3 */
          return 0x7CD2;
        case 0xF4:            /* 0xE2F4 */
          return 0x7CDC;
        case 0xF5:            /* 0xE2F5 */
          return 0x7CE2;
        case 0xF6:            /* 0xE2F6 */
          return 0x9B3B;
        case 0xF7:            /* 0xE2F7 */
          return 0x7CEF;
        case 0xF8:            /* 0xE2F8 */
          return 0x7CF2;
        case 0xF9:            /* 0xE2F9 */
          return 0x7CF4;
        case 0xFA:            /* 0xE2FA */
          return 0x7CF6;
        case 0xFB:            /* 0xE2FB */
          return 0x7CFA;
        case 0xFC:            /* 0xE2FC */
          return 0x7D06;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead E2 */

    /* Lead Byte: 0xE3xx */
    case 0xE3:
      switch (low_byte)
      {
        case 0x40:            /* 0xE340 */
          return 0x7D02;
        case 0x41:            /* 0xE341 */
          return 0x7D1C;
        case 0x42:            /* 0xE342 */
          return 0x7D15;
        case 0x43:            /* 0xE343 */
          return 0x7D0A;
        case 0x44:            /* 0xE344 */
          return 0x7D45;
        case 0x45:            /* 0xE345 */
          return 0x7D4B;
        case 0x46:            /* 0xE346 */
          return 0x7D2E;
        case 0x47:            /* 0xE347 */
          return 0x7D32;
        case 0x48:            /* 0xE348 */
          return 0x7D3F;
        case 0x49:            /* 0xE349 */
          return 0x7D35;
        case 0x4A:            /* 0xE34A */
          return 0x7D46;
        case 0x4B:            /* 0xE34B */
          return 0x7D73;
        case 0x4C:            /* 0xE34C */
          return 0x7D56;
        case 0x4D:            /* 0xE34D */
          return 0x7D4E;
        case 0x4E:            /* 0xE34E */
          return 0x7D72;
        case 0x4F:            /* 0xE34F */
          return 0x7D68;
        case 0x50:            /* 0xE350 */
          return 0x7D6E;
        case 0x51:            /* 0xE351 */
          return 0x7D4F;
        case 0x52:            /* 0xE352 */
          return 0x7D63;
        case 0x53:            /* 0xE353 */
          return 0x7D93;
        case 0x54:            /* 0xE354 */
          return 0x7D89;
        case 0x55:            /* 0xE355 */
          return 0x7D5B;
        case 0x56:            /* 0xE356 */
          return 0x7D8F;
        case 0x57:            /* 0xE357 */
          return 0x7D7D;
        case 0x58:            /* 0xE358 */
          return 0x7D9B;
        case 0x59:            /* 0xE359 */
          return 0x7DBA;
        case 0x5A:            /* 0xE35A */
          return 0x7DAE;
        case 0x5B:            /* 0xE35B */
          return 0x7DA3;
        case 0x5C:            /* 0xE35C */
          return 0x7DB5;
        case 0x5D:            /* 0xE35D */
          return 0x7DC7;
        case 0x5E:            /* 0xE35E */
          return 0x7DBD;
        case 0x5F:            /* 0xE35F */
          return 0x7DAB;
        case 0x60:            /* 0xE360 */
          return 0x7E3D;
        case 0x61:            /* 0xE361 */
          return 0x7DA2;
        case 0x62:            /* 0xE362 */
          return 0x7DAF;
        case 0x63:            /* 0xE363 */
          return 0x7DDC;
        case 0x64:            /* 0xE364 */
          return 0x7DB8;
        case 0x65:            /* 0xE365 */
          return 0x7D9F;
        case 0x66:            /* 0xE366 */
          return 0x7DB0;
        case 0x67:            /* 0xE367 */
          return 0x7DD8;
        case 0x68:            /* 0xE368 */
          return 0x7DDD;
        case 0x69:            /* 0xE369 */
          return 0x7DE4;
        case 0x6A:            /* 0xE36A */
          return 0x7DDE;
        case 0x6B:            /* 0xE36B */
          return 0x7DFB;
        case 0x6C:            /* 0xE36C */
          return 0x7DF2;
        case 0x6D:            /* 0xE36D */
          return 0x7DE1;
        case 0x6E:            /* 0xE36E */
          return 0x7E05;
        case 0x6F:            /* 0xE36F */
          return 0x7E0A;
        case 0x70:            /* 0xE370 */
          return 0x7E23;
        case 0x71:            /* 0xE371 */
          return 0x7E21;
        case 0x72:            /* 0xE372 */
          return 0x7E12;
        case 0x73:            /* 0xE373 */
          return 0x7E31;
        case 0x74:            /* 0xE374 */
          return 0x7E1F;
        case 0x75:            /* 0xE375 */
          return 0x7E09;
        case 0x76:            /* 0xE376 */
          return 0x7E0B;
        case 0x77:            /* 0xE377 */
          return 0x7E22;
        case 0x78:            /* 0xE378 */
          return 0x7E46;
        case 0x79:            /* 0xE379 */
          return 0x7E66;
        case 0x7A:            /* 0xE37A */
          return 0x7E3B;
        case 0x7B:            /* 0xE37B */
          return 0x7E35;
        case 0x7C:            /* 0xE37C */
          return 0x7E39;
        case 0x7D:            /* 0xE37D */
          return 0x7E43;
        case 0x7E:            /* 0xE37E */
          return 0x7E37;
        case 0x80:            /* 0xE380 */
          return 0x7E32;
        case 0x81:            /* 0xE381 */
          return 0x7E3A;
        case 0x82:            /* 0xE382 */
          return 0x7E67;
        case 0x83:            /* 0xE383 */
          return 0x7E5D;
        case 0x84:            /* 0xE384 */
          return 0x7E56;
        case 0x85:            /* 0xE385 */
          return 0x7E5E;
        case 0x86:            /* 0xE386 */
          return 0x7E59;
        case 0x87:            /* 0xE387 */
          return 0x7E5A;
        case 0x88:            /* 0xE388 */
          return 0x7E79;
        case 0x89:            /* 0xE389 */
          return 0x7E6A;
        case 0x8A:            /* 0xE38A */
          return 0x7E69;
        case 0x8B:            /* 0xE38B */
          return 0x7E7C;
        case 0x8C:            /* 0xE38C */
          return 0x7E7B;
        case 0x8D:            /* 0xE38D */
          return 0x7E83;
        case 0x8E:            /* 0xE38E */
          return 0x7DD5;
        case 0x8F:            /* 0xE38F */
          return 0x7E7D;
        case 0x90:            /* 0xE390 */
          return 0x8FAE;
        case 0x91:            /* 0xE391 */
          return 0x7E7F;
        case 0x92:            /* 0xE392 */
          return 0x7E88;
        case 0x93:            /* 0xE393 */
          return 0x7E89;
        case 0x94:            /* 0xE394 */
          return 0x7E8C;
        case 0x95:            /* 0xE395 */
          return 0x7E92;
        case 0x96:            /* 0xE396 */
          return 0x7E90;
        case 0x97:            /* 0xE397 */
          return 0x7E93;
        case 0x98:            /* 0xE398 */
          return 0x7E94;
        case 0x99:            /* 0xE399 */
          return 0x7E96;
        case 0x9A:            /* 0xE39A */
          return 0x7E8E;
        case 0x9B:            /* 0xE39B */
          return 0x7E9B;
        case 0x9C:            /* 0xE39C */
          return 0x7E9C;
        case 0x9D:            /* 0xE39D */
          return 0x7F38;
        case 0x9E:            /* 0xE39E */
          return 0x7F3A;
        case 0x9F:            /* 0xE39F */
          return 0x7F45;
        case 0xA0:            /* 0xE3A0 */
          return 0x7F4C;
        case 0xA1:            /* 0xE3A1 */
          return 0x7F4D;
        case 0xA2:            /* 0xE3A2 */
          return 0x7F4E;
        case 0xA3:            /* 0xE3A3 */
          return 0x7F50;
        case 0xA4:            /* 0xE3A4 */
          return 0x7F51;
        case 0xA5:            /* 0xE3A5 */
          return 0x7F55;
        case 0xA6:            /* 0xE3A6 */
          return 0x7F54;
        case 0xA7:            /* 0xE3A7 */
          return 0x7F58;
        case 0xA8:            /* 0xE3A8 */
          return 0x7F5F;
        case 0xA9:            /* 0xE3A9 */
          return 0x7F60;
        case 0xAA:            /* 0xE3AA */
          return 0x7F68;
        case 0xAB:            /* 0xE3AB */
          return 0x7F69;
        case 0xAC:            /* 0xE3AC */
          return 0x7F67;
        case 0xAD:            /* 0xE3AD */
          return 0x7F78;
        case 0xAE:            /* 0xE3AE */
          return 0x7F82;
        case 0xAF:            /* 0xE3AF */
          return 0x7F86;
        case 0xB0:            /* 0xE3B0 */
          return 0x7F83;
        case 0xB1:            /* 0xE3B1 */
          return 0x7F88;
        case 0xB2:            /* 0xE3B2 */
          return 0x7F87;
        case 0xB3:            /* 0xE3B3 */
          return 0x7F8C;
        case 0xB4:            /* 0xE3B4 */
          return 0x7F94;
        case 0xB5:            /* 0xE3B5 */
          return 0x7F9E;
        case 0xB6:            /* 0xE3B6 */
          return 0x7F9D;
        case 0xB7:            /* 0xE3B7 */
          return 0x7F9A;
        case 0xB8:            /* 0xE3B8 */
          return 0x7FA3;
        case 0xB9:            /* 0xE3B9 */
          return 0x7FAF;
        case 0xBA:            /* 0xE3BA */
          return 0x7FB2;
        case 0xBB:            /* 0xE3BB */
          return 0x7FB9;
        case 0xBC:            /* 0xE3BC */
          return 0x7FAE;
        case 0xBD:            /* 0xE3BD */
          return 0x7FB6;
        case 0xBE:            /* 0xE3BE */
          return 0x7FB8;
        case 0xBF:            /* 0xE3BF */
          return 0x8B71;
        case 0xC0:            /* 0xE3C0 */
          return 0x7FC5;
        case 0xC1:            /* 0xE3C1 */
          return 0x7FC6;
        case 0xC2:            /* 0xE3C2 */
          return 0x7FCA;
        case 0xC3:            /* 0xE3C3 */
          return 0x7FD5;
        case 0xC4:            /* 0xE3C4 */
          return 0x7FD4;
        case 0xC5:            /* 0xE3C5 */
          return 0x7FE1;
        case 0xC6:            /* 0xE3C6 */
          return 0x7FE6;
        case 0xC7:            /* 0xE3C7 */
          return 0x7FE9;
        case 0xC8:            /* 0xE3C8 */
          return 0x7FF3;
        case 0xC9:            /* 0xE3C9 */
          return 0x7FF9;
        case 0xCA:            /* 0xE3CA */
          return 0x98DC;
        case 0xCB:            /* 0xE3CB */
          return 0x8006;
        case 0xCC:            /* 0xE3CC */
          return 0x8004;
        case 0xCD:            /* 0xE3CD */
          return 0x800B;
        case 0xCE:            /* 0xE3CE */
          return 0x8012;
        case 0xCF:            /* 0xE3CF */
          return 0x8018;
        case 0xD0:            /* 0xE3D0 */
          return 0x8019;
        case 0xD1:            /* 0xE3D1 */
          return 0x801C;
        case 0xD2:            /* 0xE3D2 */
          return 0x8021;
        case 0xD3:            /* 0xE3D3 */
          return 0x8028;
        case 0xD4:            /* 0xE3D4 */
          return 0x803F;
        case 0xD5:            /* 0xE3D5 */
          return 0x803B;
        case 0xD6:            /* 0xE3D6 */
          return 0x804A;
        case 0xD7:            /* 0xE3D7 */
          return 0x8046;
        case 0xD8:            /* 0xE3D8 */
          return 0x8052;
        case 0xD9:            /* 0xE3D9 */
          return 0x8058;
        case 0xDA:            /* 0xE3DA */
          return 0x805A;
        case 0xDB:            /* 0xE3DB */
          return 0x805F;
        case 0xDC:            /* 0xE3DC */
          return 0x8062;
        case 0xDD:            /* 0xE3DD */
          return 0x8068;
        case 0xDE:            /* 0xE3DE */
          return 0x8073;
        case 0xDF:            /* 0xE3DF */
          return 0x8072;
        case 0xE0:            /* 0xE3E0 */
          return 0x8070;
        case 0xE1:            /* 0xE3E1 */
          return 0x8076;
        case 0xE2:            /* 0xE3E2 */
          return 0x8079;
        case 0xE3:            /* 0xE3E3 */
          return 0x807D;
        case 0xE4:            /* 0xE3E4 */
          return 0x807F;
        case 0xE5:            /* 0xE3E5 */
          return 0x8084;
        case 0xE6:            /* 0xE3E6 */
          return 0x8086;
        case 0xE7:            /* 0xE3E7 */
          return 0x8085;
        case 0xE8:            /* 0xE3E8 */
          return 0x809B;
        case 0xE9:            /* 0xE3E9 */
          return 0x8093;
        case 0xEA:            /* 0xE3EA */
          return 0x809A;
        case 0xEB:            /* 0xE3EB */
          return 0x80AD;
        case 0xEC:            /* 0xE3EC */
          return 0x5190;
        case 0xED:            /* 0xE3ED */
          return 0x80AC;
        case 0xEE:            /* 0xE3EE */
          return 0x80DB;
        case 0xEF:            /* 0xE3EF */
          return 0x80E5;
        case 0xF0:            /* 0xE3F0 */
          return 0x80D9;
        case 0xF1:            /* 0xE3F1 */
          return 0x80DD;
        case 0xF2:            /* 0xE3F2 */
          return 0x80C4;
        case 0xF3:            /* 0xE3F3 */
          return 0x80DA;
        case 0xF4:            /* 0xE3F4 */
          return 0x80D6;
        case 0xF5:            /* 0xE3F5 */
          return 0x8109;
        case 0xF6:            /* 0xE3F6 */
          return 0x80EF;
        case 0xF7:            /* 0xE3F7 */
          return 0x80F1;
        case 0xF8:            /* 0xE3F8 */
          return 0x811B;
        case 0xF9:            /* 0xE3F9 */
          return 0x8129;
        case 0xFA:            /* 0xE3FA */
          return 0x8123;
        case 0xFB:            /* 0xE3FB */
          return 0x812F;
        case 0xFC:            /* 0xE3FC */
          return 0x814B;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead E3 */

    /* Lead Byte: 0xE4xx */
    case 0xE4:
      switch (low_byte)
      {
        case 0x40:            /* 0xE440 */
          return 0x968B;
        case 0x41:            /* 0xE441 */
          return 0x8146;
        case 0x42:            /* 0xE442 */
          return 0x813E;
        case 0x43:            /* 0xE443 */
          return 0x8153;
        case 0x44:            /* 0xE444 */
          return 0x8151;
        case 0x45:            /* 0xE445 */
          return 0x80FC;
        case 0x46:            /* 0xE446 */
          return 0x8171;
        case 0x47:            /* 0xE447 */
          return 0x816E;
        case 0x48:            /* 0xE448 */
          return 0x8165;
        case 0x49:            /* 0xE449 */
          return 0x8166;
        case 0x4A:            /* 0xE44A */
          return 0x8174;
        case 0x4B:            /* 0xE44B */
          return 0x8183;
        case 0x4C:            /* 0xE44C */
          return 0x8188;
        case 0x4D:            /* 0xE44D */
          return 0x818A;
        case 0x4E:            /* 0xE44E */
          return 0x8180;
        case 0x4F:            /* 0xE44F */
          return 0x8182;
        case 0x50:            /* 0xE450 */
          return 0x81A0;
        case 0x51:            /* 0xE451 */
          return 0x8195;
        case 0x52:            /* 0xE452 */
          return 0x81A4;
        case 0x53:            /* 0xE453 */
          return 0x81A3;
        case 0x54:            /* 0xE454 */
          return 0x815F;
        case 0x55:            /* 0xE455 */
          return 0x8193;
        case 0x56:            /* 0xE456 */
          return 0x81A9;
        case 0x57:            /* 0xE457 */
          return 0x81B0;
        case 0x58:            /* 0xE458 */
          return 0x81B5;
        case 0x59:            /* 0xE459 */
          return 0x81BE;
        case 0x5A:            /* 0xE45A */
          return 0x81B8;
        case 0x5B:            /* 0xE45B */
          return 0x81BD;
        case 0x5C:            /* 0xE45C */
          return 0x81C0;
        case 0x5D:            /* 0xE45D */
          return 0x81C2;
        case 0x5E:            /* 0xE45E */
          return 0x81BA;
        case 0x5F:            /* 0xE45F */
          return 0x81C9;
        case 0x60:            /* 0xE460 */
          return 0x81CD;
        case 0x61:            /* 0xE461 */
          return 0x81D1;
        case 0x62:            /* 0xE462 */
          return 0x81D9;
        case 0x63:            /* 0xE463 */
          return 0x81D8;
        case 0x64:            /* 0xE464 */
          return 0x81C8;
        case 0x65:            /* 0xE465 */
          return 0x81DA;
        case 0x66:            /* 0xE466 */
          return 0x81DF;
        case 0x67:            /* 0xE467 */
          return 0x81E0;
        case 0x68:            /* 0xE468 */
          return 0x81E7;
        case 0x69:            /* 0xE469 */
          return 0x81FA;
        case 0x6A:            /* 0xE46A */
          return 0x81FB;
        case 0x6B:            /* 0xE46B */
          return 0x81FE;
        case 0x6C:            /* 0xE46C */
          return 0x8201;
        case 0x6D:            /* 0xE46D */
          return 0x8202;
        case 0x6E:            /* 0xE46E */
          return 0x8205;
        case 0x6F:            /* 0xE46F */
          return 0x8207;
        case 0x70:            /* 0xE470 */
          return 0x820A;
        case 0x71:            /* 0xE471 */
          return 0x820D;
        case 0x72:            /* 0xE472 */
          return 0x8210;
        case 0x73:            /* 0xE473 */
          return 0x8216;
        case 0x74:            /* 0xE474 */
          return 0x8229;
        case 0x75:            /* 0xE475 */
          return 0x822B;
        case 0x76:            /* 0xE476 */
          return 0x8238;
        case 0x77:            /* 0xE477 */
          return 0x8233;
        case 0x78:            /* 0xE478 */
          return 0x8240;
        case 0x79:            /* 0xE479 */
          return 0x8259;
        case 0x7A:            /* 0xE47A */
          return 0x8258;
        case 0x7B:            /* 0xE47B */
          return 0x825D;
        case 0x7C:            /* 0xE47C */
          return 0x825A;
        case 0x7D:            /* 0xE47D */
          return 0x825F;
        case 0x7E:            /* 0xE47E */
          return 0x8264;
        case 0x80:            /* 0xE480 */
          return 0x8262;
        case 0x81:            /* 0xE481 */
          return 0x8268;
        case 0x82:            /* 0xE482 */
          return 0x826A;
        case 0x83:            /* 0xE483 */
          return 0x826B;
        case 0x84:            /* 0xE484 */
          return 0x822E;
        case 0x85:            /* 0xE485 */
          return 0x8271;
        case 0x86:            /* 0xE486 */
          return 0x8277;
        case 0x87:            /* 0xE487 */
          return 0x8278;
        case 0x88:            /* 0xE488 */
          return 0x827E;
        case 0x89:            /* 0xE489 */
          return 0x828D;
        case 0x8A:            /* 0xE48A */
          return 0x8292;
        case 0x8B:            /* 0xE48B */
          return 0x82AB;
        case 0x8C:            /* 0xE48C */
          return 0x829F;
        case 0x8D:            /* 0xE48D */
          return 0x82BB;
        case 0x8E:            /* 0xE48E */
          return 0x82AC;
        case 0x8F:            /* 0xE48F */
          return 0x82E1;
        case 0x90:            /* 0xE490 */
          return 0x82E3;
        case 0x91:            /* 0xE491 */
          return 0x82DF;
        case 0x92:            /* 0xE492 */
          return 0x82D2;
        case 0x93:            /* 0xE493 */
          return 0x82F4;
        case 0x94:            /* 0xE494 */
          return 0x82F3;
        case 0x95:            /* 0xE495 */
          return 0x82FA;
        case 0x96:            /* 0xE496 */
          return 0x8393;
        case 0x97:            /* 0xE497 */
          return 0x8303;
        case 0x98:            /* 0xE498 */
          return 0x82FB;
        case 0x99:            /* 0xE499 */
          return 0x82F9;
        case 0x9A:            /* 0xE49A */
          return 0x82DE;
        case 0x9B:            /* 0xE49B */
          return 0x8306;
        case 0x9C:            /* 0xE49C */
          return 0x82DC;
        case 0x9D:            /* 0xE49D */
          return 0x8309;
        case 0x9E:            /* 0xE49E */
          return 0x82D9;
        case 0x9F:            /* 0xE49F */
          return 0x8335;
        case 0xA0:            /* 0xE4A0 */
          return 0x8334;
        case 0xA1:            /* 0xE4A1 */
          return 0x8316;
        case 0xA2:            /* 0xE4A2 */
          return 0x8332;
        case 0xA3:            /* 0xE4A3 */
          return 0x8331;
        case 0xA4:            /* 0xE4A4 */
          return 0x8340;
        case 0xA5:            /* 0xE4A5 */
          return 0x8339;
        case 0xA6:            /* 0xE4A6 */
          return 0x8350;
        case 0xA7:            /* 0xE4A7 */
          return 0x8345;
        case 0xA8:            /* 0xE4A8 */
          return 0x832F;
        case 0xA9:            /* 0xE4A9 */
          return 0x832B;
        case 0xAA:            /* 0xE4AA */
          return 0x8317;
        case 0xAB:            /* 0xE4AB */
          return 0x8318;
        case 0xAC:            /* 0xE4AC */
          return 0x8385;
        case 0xAD:            /* 0xE4AD */
          return 0x839A;
        case 0xAE:            /* 0xE4AE */
          return 0x83AA;
        case 0xAF:            /* 0xE4AF */
          return 0x839F;
        case 0xB0:            /* 0xE4B0 */
          return 0x83A2;
        case 0xB1:            /* 0xE4B1 */
          return 0x8396;
        case 0xB2:            /* 0xE4B2 */
          return 0x8323;
        case 0xB3:            /* 0xE4B3 */
          return 0x838E;
        case 0xB4:            /* 0xE4B4 */
          return 0x8387;
        case 0xB5:            /* 0xE4B5 */
          return 0x838A;
        case 0xB6:            /* 0xE4B6 */
          return 0x837C;
        case 0xB7:            /* 0xE4B7 */
          return 0x83B5;
        case 0xB8:            /* 0xE4B8 */
          return 0x8373;
        case 0xB9:            /* 0xE4B9 */
          return 0x8375;
        case 0xBA:            /* 0xE4BA */
          return 0x83A0;
        case 0xBB:            /* 0xE4BB */
          return 0x8389;
        case 0xBC:            /* 0xE4BC */
          return 0x83A8;
        case 0xBD:            /* 0xE4BD */
          return 0x83F4;
        case 0xBE:            /* 0xE4BE */
          return 0x8413;
        case 0xBF:            /* 0xE4BF */
          return 0x83EB;
        case 0xC0:            /* 0xE4C0 */
          return 0x83CE;
        case 0xC1:            /* 0xE4C1 */
          return 0x83FD;
        case 0xC2:            /* 0xE4C2 */
          return 0x8403;
        case 0xC3:            /* 0xE4C3 */
          return 0x83D8;
        case 0xC4:            /* 0xE4C4 */
          return 0x840B;
        case 0xC5:            /* 0xE4C5 */
          return 0x83C1;
        case 0xC6:            /* 0xE4C6 */
          return 0x83F7;
        case 0xC7:            /* 0xE4C7 */
          return 0x8407;
        case 0xC8:            /* 0xE4C8 */
          return 0x83E0;
        case 0xC9:            /* 0xE4C9 */
          return 0x83F2;
        case 0xCA:            /* 0xE4CA */
          return 0x840D;
        case 0xCB:            /* 0xE4CB */
          return 0x8422;
        case 0xCC:            /* 0xE4CC */
          return 0x8420;
        case 0xCD:            /* 0xE4CD */
          return 0x83BD;
        case 0xCE:            /* 0xE4CE */
          return 0x8438;
        case 0xCF:            /* 0xE4CF */
          return 0x8506;
        case 0xD0:            /* 0xE4D0 */
          return 0x83FB;
        case 0xD1:            /* 0xE4D1 */
          return 0x846D;
        case 0xD2:            /* 0xE4D2 */
          return 0x842A;
        case 0xD3:            /* 0xE4D3 */
          return 0x843C;
        case 0xD4:            /* 0xE4D4 */
          return 0x855A;
        case 0xD5:            /* 0xE4D5 */
          return 0x8484;
        case 0xD6:            /* 0xE4D6 */
          return 0x8477;
        case 0xD7:            /* 0xE4D7 */
          return 0x846B;
        case 0xD8:            /* 0xE4D8 */
          return 0x84AD;
        case 0xD9:            /* 0xE4D9 */
          return 0x846E;
        case 0xDA:            /* 0xE4DA */
          return 0x8482;
        case 0xDB:            /* 0xE4DB */
          return 0x8469;
        case 0xDC:            /* 0xE4DC */
          return 0x8446;
        case 0xDD:            /* 0xE4DD */
          return 0x842C;
        case 0xDE:            /* 0xE4DE */
          return 0x846F;
        case 0xDF:            /* 0xE4DF */
          return 0x8479;
        case 0xE0:            /* 0xE4E0 */
          return 0x8435;
        case 0xE1:            /* 0xE4E1 */
          return 0x84CA;
        case 0xE2:            /* 0xE4E2 */
          return 0x8462;
        case 0xE3:            /* 0xE4E3 */
          return 0x84B9;
        case 0xE4:            /* 0xE4E4 */
          return 0x84BF;
        case 0xE5:            /* 0xE4E5 */
          return 0x849F;
        case 0xE6:            /* 0xE4E6 */
          return 0x84D9;
        case 0xE7:            /* 0xE4E7 */
          return 0x84CD;
        case 0xE8:            /* 0xE4E8 */
          return 0x84BB;
        case 0xE9:            /* 0xE4E9 */
          return 0x84DA;
        case 0xEA:            /* 0xE4EA */
          return 0x84D0;
        case 0xEB:            /* 0xE4EB */
          return 0x84C1;
        case 0xEC:            /* 0xE4EC */
          return 0x84C6;
        case 0xED:            /* 0xE4ED */
          return 0x84D6;
        case 0xEE:            /* 0xE4EE */
          return 0x84A1;
        case 0xEF:            /* 0xE4EF */
          return 0x8521;
        case 0xF0:            /* 0xE4F0 */
          return 0x84FF;
        case 0xF1:            /* 0xE4F1 */
          return 0x84F4;
        case 0xF2:            /* 0xE4F2 */
          return 0x8517;
        case 0xF3:            /* 0xE4F3 */
          return 0x8518;
        case 0xF4:            /* 0xE4F4 */
          return 0x852C;
        case 0xF5:            /* 0xE4F5 */
          return 0x851F;
        case 0xF6:            /* 0xE4F6 */
          return 0x8515;
        case 0xF7:            /* 0xE4F7 */
          return 0x8514;
        case 0xF8:            /* 0xE4F8 */
          return 0x84FC;
        case 0xF9:            /* 0xE4F9 */
          return 0x8540;
        case 0xFA:            /* 0xE4FA */
          return 0x8563;
        case 0xFB:            /* 0xE4FB */
          return 0x8558;
        case 0xFC:            /* 0xE4FC */
          return 0x8548;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead E4 */

    /* Lead Byte: 0xE5xx */
    case 0xE5:
      switch (low_byte)
      {
        case 0x40:            /* 0xE540 */
          return 0x8541;
        case 0x41:            /* 0xE541 */
          return 0x8602;
        case 0x42:            /* 0xE542 */
          return 0x854B;
        case 0x43:            /* 0xE543 */
          return 0x8555;
        case 0x44:            /* 0xE544 */
          return 0x8580;
        case 0x45:            /* 0xE545 */
          return 0x85A4;
        case 0x46:            /* 0xE546 */
          return 0x8588;
        case 0x47:            /* 0xE547 */
          return 0x8591;
        case 0x48:            /* 0xE548 */
          return 0x858A;
        case 0x49:            /* 0xE549 */
          return 0x85A8;
        case 0x4A:            /* 0xE54A */
          return 0x856D;
        case 0x4B:            /* 0xE54B */
          return 0x8594;
        case 0x4C:            /* 0xE54C */
          return 0x859B;
        case 0x4D:            /* 0xE54D */
          return 0x85EA;
        case 0x4E:            /* 0xE54E */
          return 0x8587;
        case 0x4F:            /* 0xE54F */
          return 0x859C;
        case 0x50:            /* 0xE550 */
          return 0x8577;
        case 0x51:            /* 0xE551 */
          return 0x857E;
        case 0x52:            /* 0xE552 */
          return 0x8590;
        case 0x53:            /* 0xE553 */
          return 0x85C9;
        case 0x54:            /* 0xE554 */
          return 0x85BA;
        case 0x55:            /* 0xE555 */
          return 0x85CF;
        case 0x56:            /* 0xE556 */
          return 0x85B9;
        case 0x57:            /* 0xE557 */
          return 0x85D0;
        case 0x58:            /* 0xE558 */
          return 0x85D5;
        case 0x59:            /* 0xE559 */
          return 0x85DD;
        case 0x5A:            /* 0xE55A */
          return 0x85E5;
        case 0x5B:            /* 0xE55B */
          return 0x85DC;
        case 0x5C:            /* 0xE55C */
          return 0x85F9;
        case 0x5D:            /* 0xE55D */
          return 0x860A;
        case 0x5E:            /* 0xE55E */
          return 0x8613;
        case 0x5F:            /* 0xE55F */
          return 0x860B;
        case 0x60:            /* 0xE560 */
          return 0x85FE;
        case 0x61:            /* 0xE561 */
          return 0x85FA;
        case 0x62:            /* 0xE562 */
          return 0x8606;
        case 0x63:            /* 0xE563 */
          return 0x8622;
        case 0x64:            /* 0xE564 */
          return 0x861A;
        case 0x65:            /* 0xE565 */
          return 0x8630;
        case 0x66:            /* 0xE566 */
          return 0x863F;
        case 0x67:            /* 0xE567 */
          return 0x864D;
        case 0x68:            /* 0xE568 */
          return 0x4E55;
        case 0x69:            /* 0xE569 */
          return 0x8654;
        case 0x6A:            /* 0xE56A */
          return 0x865F;
        case 0x6B:            /* 0xE56B */
          return 0x8667;
        case 0x6C:            /* 0xE56C */
          return 0x8671;
        case 0x6D:            /* 0xE56D */
          return 0x8693;
        case 0x6E:            /* 0xE56E */
          return 0x86A3;
        case 0x6F:            /* 0xE56F */
          return 0x86A9;
        case 0x70:            /* 0xE570 */
          return 0x86AA;
        case 0x71:            /* 0xE571 */
          return 0x868B;
        case 0x72:            /* 0xE572 */
          return 0x868C;
        case 0x73:            /* 0xE573 */
          return 0x86B6;
        case 0x74:            /* 0xE574 */
          return 0x86AF;
        case 0x75:            /* 0xE575 */
          return 0x86C4;
        case 0x76:            /* 0xE576 */
          return 0x86C6;
        case 0x77:            /* 0xE577 */
          return 0x86B0;
        case 0x78:            /* 0xE578 */
          return 0x86C9;
        case 0x79:            /* 0xE579 */
          return 0x8823;
        case 0x7A:            /* 0xE57A */
          return 0x86AB;
        case 0x7B:            /* 0xE57B */
          return 0x86D4;
        case 0x7C:            /* 0xE57C */
          return 0x86DE;
        case 0x7D:            /* 0xE57D */
          return 0x86E9;
        case 0x7E:            /* 0xE57E */
          return 0x86EC;
        case 0x80:            /* 0xE580 */
          return 0x86DF;
        case 0x81:            /* 0xE581 */
          return 0x86DB;
        case 0x82:            /* 0xE582 */
          return 0x86EF;
        case 0x83:            /* 0xE583 */
          return 0x8712;
        case 0x84:            /* 0xE584 */
          return 0x8706;
        case 0x85:            /* 0xE585 */
          return 0x8708;
        case 0x86:            /* 0xE586 */
          return 0x8700;
        case 0x87:            /* 0xE587 */
          return 0x8703;
        case 0x88:            /* 0xE588 */
          return 0x86FB;
        case 0x89:            /* 0xE589 */
          return 0x8711;
        case 0x8A:            /* 0xE58A */
          return 0x8709;
        case 0x8B:            /* 0xE58B */
          return 0x870D;
        case 0x8C:            /* 0xE58C */
          return 0x86F9;
        case 0x8D:            /* 0xE58D */
          return 0x870A;
        case 0x8E:            /* 0xE58E */
          return 0x8734;
        case 0x8F:            /* 0xE58F */
          return 0x873F;
        case 0x90:            /* 0xE590 */
          return 0x8737;
        case 0x91:            /* 0xE591 */
          return 0x873B;
        case 0x92:            /* 0xE592 */
          return 0x8725;
        case 0x93:            /* 0xE593 */
          return 0x8729;
        case 0x94:            /* 0xE594 */
          return 0x871A;
        case 0x95:            /* 0xE595 */
          return 0x8760;
        case 0x96:            /* 0xE596 */
          return 0x875F;
        case 0x97:            /* 0xE597 */
          return 0x8778;
        case 0x98:            /* 0xE598 */
          return 0x874C;
        case 0x99:            /* 0xE599 */
          return 0x874E;
        case 0x9A:            /* 0xE59A */
          return 0x8774;
        case 0x9B:            /* 0xE59B */
          return 0x8757;
        case 0x9C:            /* 0xE59C */
          return 0x8768;
        case 0x9D:            /* 0xE59D */
          return 0x876E;
        case 0x9E:            /* 0xE59E */
          return 0x8759;
        case 0x9F:            /* 0xE59F */
          return 0x8753;
        case 0xA0:            /* 0xE5A0 */
          return 0x8763;
        case 0xA1:            /* 0xE5A1 */
          return 0x876A;
        case 0xA2:            /* 0xE5A2 */
          return 0x8805;
        case 0xA3:            /* 0xE5A3 */
          return 0x87A2;
        case 0xA4:            /* 0xE5A4 */
          return 0x879F;
        case 0xA5:            /* 0xE5A5 */
          return 0x8782;
        case 0xA6:            /* 0xE5A6 */
          return 0x87AF;
        case 0xA7:            /* 0xE5A7 */
          return 0x87CB;
        case 0xA8:            /* 0xE5A8 */
          return 0x87BD;
        case 0xA9:            /* 0xE5A9 */
          return 0x87C0;
        case 0xAA:            /* 0xE5AA */
          return 0x87D0;
        case 0xAB:            /* 0xE5AB */
          return 0x96D6;
        case 0xAC:            /* 0xE5AC */
          return 0x87AB;
        case 0xAD:            /* 0xE5AD */
          return 0x87C4;
        case 0xAE:            /* 0xE5AE */
          return 0x87B3;
        case 0xAF:            /* 0xE5AF */
          return 0x87C7;
        case 0xB0:            /* 0xE5B0 */
          return 0x87C6;
        case 0xB1:            /* 0xE5B1 */
          return 0x87BB;
        case 0xB2:            /* 0xE5B2 */
          return 0x87EF;
        case 0xB3:            /* 0xE5B3 */
          return 0x87F2;
        case 0xB4:            /* 0xE5B4 */
          return 0x87E0;
        case 0xB5:            /* 0xE5B5 */
          return 0x880F;
        case 0xB6:            /* 0xE5B6 */
          return 0x880D;
        case 0xB7:            /* 0xE5B7 */
          return 0x87FE;
        case 0xB8:            /* 0xE5B8 */
          return 0x87F6;
        case 0xB9:            /* 0xE5B9 */
          return 0x87F7;
        case 0xBA:            /* 0xE5BA */
          return 0x880E;
        case 0xBB:            /* 0xE5BB */
          return 0x87D2;
        case 0xBC:            /* 0xE5BC */
          return 0x8811;
        case 0xBD:            /* 0xE5BD */
          return 0x8816;
        case 0xBE:            /* 0xE5BE */
          return 0x8815;
        case 0xBF:            /* 0xE5BF */
          return 0x8822;
        case 0xC0:            /* 0xE5C0 */
          return 0x8821;
        case 0xC1:            /* 0xE5C1 */
          return 0x8831;
        case 0xC2:            /* 0xE5C2 */
          return 0x8836;
        case 0xC3:            /* 0xE5C3 */
          return 0x8839;
        case 0xC4:            /* 0xE5C4 */
          return 0x8827;
        case 0xC5:            /* 0xE5C5 */
          return 0x883B;
        case 0xC6:            /* 0xE5C6 */
          return 0x8844;
        case 0xC7:            /* 0xE5C7 */
          return 0x8842;
        case 0xC8:            /* 0xE5C8 */
          return 0x8852;
        case 0xC9:            /* 0xE5C9 */
          return 0x8859;
        case 0xCA:            /* 0xE5CA */
          return 0x885E;
        case 0xCB:            /* 0xE5CB */
          return 0x8862;
        case 0xCC:            /* 0xE5CC */
          return 0x886B;
        case 0xCD:            /* 0xE5CD */
          return 0x8881;
        case 0xCE:            /* 0xE5CE */
          return 0x887E;
        case 0xCF:            /* 0xE5CF */
          return 0x889E;
        case 0xD0:            /* 0xE5D0 */
          return 0x8875;
        case 0xD1:            /* 0xE5D1 */
          return 0x887D;
        case 0xD2:            /* 0xE5D2 */
          return 0x88B5;
        case 0xD3:            /* 0xE5D3 */
          return 0x8872;
        case 0xD4:            /* 0xE5D4 */
          return 0x8882;
        case 0xD5:            /* 0xE5D5 */
          return 0x8897;
        case 0xD6:            /* 0xE5D6 */
          return 0x8892;
        case 0xD7:            /* 0xE5D7 */
          return 0x88AE;
        case 0xD8:            /* 0xE5D8 */
          return 0x8899;
        case 0xD9:            /* 0xE5D9 */
          return 0x88A2;
        case 0xDA:            /* 0xE5DA */
          return 0x888D;
        case 0xDB:            /* 0xE5DB */
          return 0x88A4;
        case 0xDC:            /* 0xE5DC */
          return 0x88B0;
        case 0xDD:            /* 0xE5DD */
          return 0x88BF;
        case 0xDE:            /* 0xE5DE */
          return 0x88B1;
        case 0xDF:            /* 0xE5DF */
          return 0x88C3;
        case 0xE0:            /* 0xE5E0 */
          return 0x88C4;
        case 0xE1:            /* 0xE5E1 */
          return 0x88D4;
        case 0xE2:            /* 0xE5E2 */
          return 0x88D8;
        case 0xE3:            /* 0xE5E3 */
          return 0x88D9;
        case 0xE4:            /* 0xE5E4 */
          return 0x88DD;
        case 0xE5:            /* 0xE5E5 */
          return 0x88F9;
        case 0xE6:            /* 0xE5E6 */
          return 0x8902;
        case 0xE7:            /* 0xE5E7 */
          return 0x88FC;
        case 0xE8:            /* 0xE5E8 */
          return 0x88F4;
        case 0xE9:            /* 0xE5E9 */
          return 0x88E8;
        case 0xEA:            /* 0xE5EA */
          return 0x88F2;
        case 0xEB:            /* 0xE5EB */
          return 0x8904;
        case 0xEC:            /* 0xE5EC */
          return 0x890C;
        case 0xED:            /* 0xE5ED */
          return 0x890A;
        case 0xEE:            /* 0xE5EE */
          return 0x8913;
        case 0xEF:            /* 0xE5EF */
          return 0x8943;
        case 0xF0:            /* 0xE5F0 */
          return 0x891E;
        case 0xF1:            /* 0xE5F1 */
          return 0x8925;
        case 0xF2:            /* 0xE5F2 */
          return 0x892A;
        case 0xF3:            /* 0xE5F3 */
          return 0x892B;
        case 0xF4:            /* 0xE5F4 */
          return 0x8941;
        case 0xF5:            /* 0xE5F5 */
          return 0x8944;
        case 0xF6:            /* 0xE5F6 */
          return 0x893B;
        case 0xF7:            /* 0xE5F7 */
          return 0x8936;
        case 0xF8:            /* 0xE5F8 */
          return 0x8938;
        case 0xF9:            /* 0xE5F9 */
          return 0x894C;
        case 0xFA:            /* 0xE5FA */
          return 0x891D;
        case 0xFB:            /* 0xE5FB */
          return 0x8960;
        case 0xFC:            /* 0xE5FC */
          return 0x895E;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead E5 */

    /* Lead Byte: 0xE6xx */
    case 0xE6:
      switch (low_byte)
      {
        case 0x40:            /* 0xE640 */
          return 0x8966;
        case 0x41:            /* 0xE641 */
          return 0x8964;
        case 0x42:            /* 0xE642 */
          return 0x896D;
        case 0x43:            /* 0xE643 */
          return 0x896A;
        case 0x44:            /* 0xE644 */
          return 0x896F;
        case 0x45:            /* 0xE645 */
          return 0x8974;
        case 0x46:            /* 0xE646 */
          return 0x8977;
        case 0x47:            /* 0xE647 */
          return 0x897E;
        case 0x48:            /* 0xE648 */
          return 0x8983;
        case 0x49:            /* 0xE649 */
          return 0x8988;
        case 0x4A:            /* 0xE64A */
          return 0x898A;
        case 0x4B:            /* 0xE64B */
          return 0x8993;
        case 0x4C:            /* 0xE64C */
          return 0x8998;
        case 0x4D:            /* 0xE64D */
          return 0x89A1;
        case 0x4E:            /* 0xE64E */
          return 0x89A9;
        case 0x4F:            /* 0xE64F */
          return 0x89A6;
        case 0x50:            /* 0xE650 */
          return 0x89AC;
        case 0x51:            /* 0xE651 */
          return 0x89AF;
        case 0x52:            /* 0xE652 */
          return 0x89B2;
        case 0x53:            /* 0xE653 */
          return 0x89BA;
        case 0x54:            /* 0xE654 */
          return 0x89BD;
        case 0x55:            /* 0xE655 */
          return 0x89BF;
        case 0x56:            /* 0xE656 */
          return 0x89C0;
        case 0x57:            /* 0xE657 */
          return 0x89DA;
        case 0x58:            /* 0xE658 */
          return 0x89DC;
        case 0x59:            /* 0xE659 */
          return 0x89DD;
        case 0x5A:            /* 0xE65A */
          return 0x89E7;
        case 0x5B:            /* 0xE65B */
          return 0x89F4;
        case 0x5C:            /* 0xE65C */
          return 0x89F8;
        case 0x5D:            /* 0xE65D */
          return 0x8A03;
        case 0x5E:            /* 0xE65E */
          return 0x8A16;
        case 0x5F:            /* 0xE65F */
          return 0x8A10;
        case 0x60:            /* 0xE660 */
          return 0x8A0C;
        case 0x61:            /* 0xE661 */
          return 0x8A1B;
        case 0x62:            /* 0xE662 */
          return 0x8A1D;
        case 0x63:            /* 0xE663 */
          return 0x8A25;
        case 0x64:            /* 0xE664 */
          return 0x8A36;
        case 0x65:            /* 0xE665 */
          return 0x8A41;
        case 0x66:            /* 0xE666 */
          return 0x8A5B;
        case 0x67:            /* 0xE667 */
          return 0x8A52;
        case 0x68:            /* 0xE668 */
          return 0x8A46;
        case 0x69:            /* 0xE669 */
          return 0x8A48;
        case 0x6A:            /* 0xE66A */
          return 0x8A7C;
        case 0x6B:            /* 0xE66B */
          return 0x8A6D;
        case 0x6C:            /* 0xE66C */
          return 0x8A6C;
        case 0x6D:            /* 0xE66D */
          return 0x8A62;
        case 0x6E:            /* 0xE66E */
          return 0x8A85;
        case 0x6F:            /* 0xE66F */
          return 0x8A82;
        case 0x70:            /* 0xE670 */
          return 0x8A84;
        case 0x71:            /* 0xE671 */
          return 0x8AA8;
        case 0x72:            /* 0xE672 */
          return 0x8AA1;
        case 0x73:            /* 0xE673 */
          return 0x8A91;
        case 0x74:            /* 0xE674 */
          return 0x8AA5;
        case 0x75:            /* 0xE675 */
          return 0x8AA6;
        case 0x76:            /* 0xE676 */
          return 0x8A9A;
        case 0x77:            /* 0xE677 */
          return 0x8AA3;
        case 0x78:            /* 0xE678 */
          return 0x8AC4;
        case 0x79:            /* 0xE679 */
          return 0x8ACD;
        case 0x7A:            /* 0xE67A */
          return 0x8AC2;
        case 0x7B:            /* 0xE67B */
          return 0x8ADA;
        case 0x7C:            /* 0xE67C */
          return 0x8AEB;
        case 0x7D:            /* 0xE67D */
          return 0x8AF3;
        case 0x7E:            /* 0xE67E */
          return 0x8AE7;
        case 0x80:            /* 0xE680 */
          return 0x8AE4;
        case 0x81:            /* 0xE681 */
          return 0x8AF1;
        case 0x82:            /* 0xE682 */
          return 0x8B14;
        case 0x83:            /* 0xE683 */
          return 0x8AE0;
        case 0x84:            /* 0xE684 */
          return 0x8AE2;
        case 0x85:            /* 0xE685 */
          return 0x8AF7;
        case 0x86:            /* 0xE686 */
          return 0x8ADE;
        case 0x87:            /* 0xE687 */
          return 0x8ADB;
        case 0x88:            /* 0xE688 */
          return 0x8B0C;
        case 0x89:            /* 0xE689 */
          return 0x8B07;
        case 0x8A:            /* 0xE68A */
          return 0x8B1A;
        case 0x8B:            /* 0xE68B */
          return 0x8AE1;
        case 0x8C:            /* 0xE68C */
          return 0x8B16;
        case 0x8D:            /* 0xE68D */
          return 0x8B10;
        case 0x8E:            /* 0xE68E */
          return 0x8B17;
        case 0x8F:            /* 0xE68F */
          return 0x8B20;
        case 0x90:            /* 0xE690 */
          return 0x8B33;
        case 0x91:            /* 0xE691 */
          return 0x97AB;
        case 0x92:            /* 0xE692 */
          return 0x8B26;
        case 0x93:            /* 0xE693 */
          return 0x8B2B;
        case 0x94:            /* 0xE694 */
          return 0x8B3E;
        case 0x95:            /* 0xE695 */
          return 0x8B28;
        case 0x96:            /* 0xE696 */
          return 0x8B41;
        case 0x97:            /* 0xE697 */
          return 0x8B4C;
        case 0x98:            /* 0xE698 */
          return 0x8B4F;
        case 0x99:            /* 0xE699 */
          return 0x8B4E;
        case 0x9A:            /* 0xE69A */
          return 0x8B49;
        case 0x9B:            /* 0xE69B */
          return 0x8B56;
        case 0x9C:            /* 0xE69C */
          return 0x8B5B;
        case 0x9D:            /* 0xE69D */
          return 0x8B5A;
        case 0x9E:            /* 0xE69E */
          return 0x8B6B;
        case 0x9F:            /* 0xE69F */
          return 0x8B5F;
        case 0xA0:            /* 0xE6A0 */
          return 0x8B6C;
        case 0xA1:            /* 0xE6A1 */
          return 0x8B6F;
        case 0xA2:            /* 0xE6A2 */
          return 0x8B74;
        case 0xA3:            /* 0xE6A3 */
          return 0x8B7D;
        case 0xA4:            /* 0xE6A4 */
          return 0x8B80;
        case 0xA5:            /* 0xE6A5 */
          return 0x8B8C;
        case 0xA6:            /* 0xE6A6 */
          return 0x8B8E;
        case 0xA7:            /* 0xE6A7 */
          return 0x8B92;
        case 0xA8:            /* 0xE6A8 */
          return 0x8B93;
        case 0xA9:            /* 0xE6A9 */
          return 0x8B96;
        case 0xAA:            /* 0xE6AA */
          return 0x8B99;
        case 0xAB:            /* 0xE6AB */
          return 0x8B9A;
        case 0xAC:            /* 0xE6AC */
          return 0x8C3A;
        case 0xAD:            /* 0xE6AD */
          return 0x8C41;
        case 0xAE:            /* 0xE6AE */
          return 0x8C3F;
        case 0xAF:            /* 0xE6AF */
          return 0x8C48;
        case 0xB0:            /* 0xE6B0 */
          return 0x8C4C;
        case 0xB1:            /* 0xE6B1 */
          return 0x8C4E;
        case 0xB2:            /* 0xE6B2 */
          return 0x8C50;
        case 0xB3:            /* 0xE6B3 */
          return 0x8C55;
        case 0xB4:            /* 0xE6B4 */
          return 0x8C62;
        case 0xB5:            /* 0xE6B5 */
          return 0x8C6C;
        case 0xB6:            /* 0xE6B6 */
          return 0x8C78;
        case 0xB7:            /* 0xE6B7 */
          return 0x8C7A;
        case 0xB8:            /* 0xE6B8 */
          return 0x8C82;
        case 0xB9:            /* 0xE6B9 */
          return 0x8C89;
        case 0xBA:            /* 0xE6BA */
          return 0x8C85;
        case 0xBB:            /* 0xE6BB */
          return 0x8C8A;
        case 0xBC:            /* 0xE6BC */
          return 0x8C8D;
        case 0xBD:            /* 0xE6BD */
          return 0x8C8E;
        case 0xBE:            /* 0xE6BE */
          return 0x8C94;
        case 0xBF:            /* 0xE6BF */
          return 0x8C7C;
        case 0xC0:            /* 0xE6C0 */
          return 0x8C98;
        case 0xC1:            /* 0xE6C1 */
          return 0x621D;
        case 0xC2:            /* 0xE6C2 */
          return 0x8CAD;
        case 0xC3:            /* 0xE6C3 */
          return 0x8CAA;
        case 0xC4:            /* 0xE6C4 */
          return 0x8CBD;
        case 0xC5:            /* 0xE6C5 */
          return 0x8CB2;
        case 0xC6:            /* 0xE6C6 */
          return 0x8CB3;
        case 0xC7:            /* 0xE6C7 */
          return 0x8CAE;
        case 0xC8:            /* 0xE6C8 */
          return 0x8CB6;
        case 0xC9:            /* 0xE6C9 */
          return 0x8CC8;
        case 0xCA:            /* 0xE6CA */
          return 0x8CC1;
        case 0xCB:            /* 0xE6CB */
          return 0x8CE4;
        case 0xCC:            /* 0xE6CC */
          return 0x8CE3;
        case 0xCD:            /* 0xE6CD */
          return 0x8CDA;
        case 0xCE:            /* 0xE6CE */
          return 0x8CFD;
        case 0xCF:            /* 0xE6CF */
          return 0x8CFA;
        case 0xD0:            /* 0xE6D0 */
          return 0x8CFB;
        case 0xD1:            /* 0xE6D1 */
          return 0x8D04;
        case 0xD2:            /* 0xE6D2 */
          return 0x8D05;
        case 0xD3:            /* 0xE6D3 */
          return 0x8D0A;
        case 0xD4:            /* 0xE6D4 */
          return 0x8D07;
        case 0xD5:            /* 0xE6D5 */
          return 0x8D0F;
        case 0xD6:            /* 0xE6D6 */
          return 0x8D0D;
        case 0xD7:            /* 0xE6D7 */
          return 0x8D10;
        case 0xD8:            /* 0xE6D8 */
          return 0x9F4E;
        case 0xD9:            /* 0xE6D9 */
          return 0x8D13;
        case 0xDA:            /* 0xE6DA */
          return 0x8CCD;
        case 0xDB:            /* 0xE6DB */
          return 0x8D14;
        case 0xDC:            /* 0xE6DC */
          return 0x8D16;
        case 0xDD:            /* 0xE6DD */
          return 0x8D67;
        case 0xDE:            /* 0xE6DE */
          return 0x8D6D;
        case 0xDF:            /* 0xE6DF */
          return 0x8D71;
        case 0xE0:            /* 0xE6E0 */
          return 0x8D73;
        case 0xE1:            /* 0xE6E1 */
          return 0x8D81;
        case 0xE2:            /* 0xE6E2 */
          return 0x8D99;
        case 0xE3:            /* 0xE6E3 */
          return 0x8DC2;
        case 0xE4:            /* 0xE6E4 */
          return 0x8DBE;
        case 0xE5:            /* 0xE6E5 */
          return 0x8DBA;
        case 0xE6:            /* 0xE6E6 */
          return 0x8DCF;
        case 0xE7:            /* 0xE6E7 */
          return 0x8DDA;
        case 0xE8:            /* 0xE6E8 */
          return 0x8DD6;
        case 0xE9:            /* 0xE6E9 */
          return 0x8DCC;
        case 0xEA:            /* 0xE6EA */
          return 0x8DDB;
        case 0xEB:            /* 0xE6EB */
          return 0x8DCB;
        case 0xEC:            /* 0xE6EC */
          return 0x8DEA;
        case 0xED:            /* 0xE6ED */
          return 0x8DEB;
        case 0xEE:            /* 0xE6EE */
          return 0x8DDF;
        case 0xEF:            /* 0xE6EF */
          return 0x8DE3;
        case 0xF0:            /* 0xE6F0 */
          return 0x8DFC;
        case 0xF1:            /* 0xE6F1 */
          return 0x8E08;
        case 0xF2:            /* 0xE6F2 */
          return 0x8E09;
        case 0xF3:            /* 0xE6F3 */
          return 0x8DFF;
        case 0xF4:            /* 0xE6F4 */
          return 0x8E1D;
        case 0xF5:            /* 0xE6F5 */
          return 0x8E1E;
        case 0xF6:            /* 0xE6F6 */
          return 0x8E10;
        case 0xF7:            /* 0xE6F7 */
          return 0x8E1F;
        case 0xF8:            /* 0xE6F8 */
          return 0x8E42;
        case 0xF9:            /* 0xE6F9 */
          return 0x8E35;
        case 0xFA:            /* 0xE6FA */
          return 0x8E30;
        case 0xFB:            /* 0xE6FB */
          return 0x8E34;
        case 0xFC:            /* 0xE6FC */
          return 0x8E4A;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead E6 */

    /* Lead Byte: 0xE7xx */
    case 0xE7:
      switch (low_byte)
      {
        case 0x40:            /* 0xE740 */
          return 0x8E47;
        case 0x41:            /* 0xE741 */
          return 0x8E49;
        case 0x42:            /* 0xE742 */
          return 0x8E4C;
        case 0x43:            /* 0xE743 */
          return 0x8E50;
        case 0x44:            /* 0xE744 */
          return 0x8E48;
        case 0x45:            /* 0xE745 */
          return 0x8E59;
        case 0x46:            /* 0xE746 */
          return 0x8E64;
        case 0x47:            /* 0xE747 */
          return 0x8E60;
        case 0x48:            /* 0xE748 */
          return 0x8E2A;
        case 0x49:            /* 0xE749 */
          return 0x8E63;
        case 0x4A:            /* 0xE74A */
          return 0x8E55;
        case 0x4B:            /* 0xE74B */
          return 0x8E76;
        case 0x4C:            /* 0xE74C */
          return 0x8E72;
        case 0x4D:            /* 0xE74D */
          return 0x8E7C;
        case 0x4E:            /* 0xE74E */
          return 0x8E81;
        case 0x4F:            /* 0xE74F */
          return 0x8E87;
        case 0x50:            /* 0xE750 */
          return 0x8E85;
        case 0x51:            /* 0xE751 */
          return 0x8E84;
        case 0x52:            /* 0xE752 */
          return 0x8E8B;
        case 0x53:            /* 0xE753 */
          return 0x8E8A;
        case 0x54:            /* 0xE754 */
          return 0x8E93;
        case 0x55:            /* 0xE755 */
          return 0x8E91;
        case 0x56:            /* 0xE756 */
          return 0x8E94;
        case 0x57:            /* 0xE757 */
          return 0x8E99;
        case 0x58:            /* 0xE758 */
          return 0x8EAA;
        case 0x59:            /* 0xE759 */
          return 0x8EA1;
        case 0x5A:            /* 0xE75A */
          return 0x8EAC;
        case 0x5B:            /* 0xE75B */
          return 0x8EB0;
        case 0x5C:            /* 0xE75C */
          return 0x8EC6;
        case 0x5D:            /* 0xE75D */
          return 0x8EB1;
        case 0x5E:            /* 0xE75E */
          return 0x8EBE;
        case 0x5F:            /* 0xE75F */
          return 0x8EC5;
        case 0x60:            /* 0xE760 */
          return 0x8EC8;
        case 0x61:            /* 0xE761 */
          return 0x8ECB;
        case 0x62:            /* 0xE762 */
          return 0x8EDB;
        case 0x63:            /* 0xE763 */
          return 0x8EE3;
        case 0x64:            /* 0xE764 */
          return 0x8EFC;
        case 0x65:            /* 0xE765 */
          return 0x8EFB;
        case 0x66:            /* 0xE766 */
          return 0x8EEB;
        case 0x67:            /* 0xE767 */
          return 0x8EFE;
        case 0x68:            /* 0xE768 */
          return 0x8F0A;
        case 0x69:            /* 0xE769 */
          return 0x8F05;
        case 0x6A:            /* 0xE76A */
          return 0x8F15;
        case 0x6B:            /* 0xE76B */
          return 0x8F12;
        case 0x6C:            /* 0xE76C */
          return 0x8F19;
        case 0x6D:            /* 0xE76D */
          return 0x8F13;
        case 0x6E:            /* 0xE76E */
          return 0x8F1C;
        case 0x6F:            /* 0xE76F */
          return 0x8F1F;
        case 0x70:            /* 0xE770 */
          return 0x8F1B;
        case 0x71:            /* 0xE771 */
          return 0x8F0C;
        case 0x72:            /* 0xE772 */
          return 0x8F26;
        case 0x73:            /* 0xE773 */
          return 0x8F33;
        case 0x74:            /* 0xE774 */
          return 0x8F3B;
        case 0x75:            /* 0xE775 */
          return 0x8F39;
        case 0x76:            /* 0xE776 */
          return 0x8F45;
        case 0x77:            /* 0xE777 */
          return 0x8F42;
        case 0x78:            /* 0xE778 */
          return 0x8F3E;
        case 0x79:            /* 0xE779 */
          return 0x8F4C;
        case 0x7A:            /* 0xE77A */
          return 0x8F49;
        case 0x7B:            /* 0xE77B */
          return 0x8F46;
        case 0x7C:            /* 0xE77C */
          return 0x8F4E;
        case 0x7D:            /* 0xE77D */
          return 0x8F57;
        case 0x7E:            /* 0xE77E */
          return 0x8F5C;
        case 0x80:            /* 0xE780 */
          return 0x8F62;
        case 0x81:            /* 0xE781 */
          return 0x8F63;
        case 0x82:            /* 0xE782 */
          return 0x8F64;
        case 0x83:            /* 0xE783 */
          return 0x8F9C;
        case 0x84:            /* 0xE784 */
          return 0x8F9F;
        case 0x85:            /* 0xE785 */
          return 0x8FA3;
        case 0x86:            /* 0xE786 */
          return 0x8FAD;
        case 0x87:            /* 0xE787 */
          return 0x8FAF;
        case 0x88:            /* 0xE788 */
          return 0x8FB7;
        case 0x89:            /* 0xE789 */
          return 0x8FDA;
        case 0x8A:            /* 0xE78A */
          return 0x8FE5;
        case 0x8B:            /* 0xE78B */
          return 0x8FE2;
        case 0x8C:            /* 0xE78C */
          return 0x8FEA;
        case 0x8D:            /* 0xE78D */
          return 0x8FEF;
        case 0x8E:            /* 0xE78E */
          return 0x9087;
        case 0x8F:            /* 0xE78F */
          return 0x8FF4;
        case 0x90:            /* 0xE790 */
          return 0x9005;
        case 0x91:            /* 0xE791 */
          return 0x8FF9;
        case 0x92:            /* 0xE792 */
          return 0x8FFA;
        case 0x93:            /* 0xE793 */
          return 0x9011;
        case 0x94:            /* 0xE794 */
          return 0x9015;
        case 0x95:            /* 0xE795 */
          return 0x9021;
        case 0x96:            /* 0xE796 */
          return 0x900D;
        case 0x97:            /* 0xE797 */
          return 0x901E;
        case 0x98:            /* 0xE798 */
          return 0x9016;
        case 0x99:            /* 0xE799 */
          return 0x900B;
        case 0x9A:            /* 0xE79A */
          return 0x9027;
        case 0x9B:            /* 0xE79B */
          return 0x9036;
        case 0x9C:            /* 0xE79C */
          return 0x9035;
        case 0x9D:            /* 0xE79D */
          return 0x9039;
        case 0x9E:            /* 0xE79E */
          return 0x8FF8;
        case 0x9F:            /* 0xE79F */
          return 0x904F;
        case 0xA0:            /* 0xE7A0 */
          return 0x9050;
        case 0xA1:            /* 0xE7A1 */
          return 0x9051;
        case 0xA2:            /* 0xE7A2 */
          return 0x9052;
        case 0xA3:            /* 0xE7A3 */
          return 0x900E;
        case 0xA4:            /* 0xE7A4 */
          return 0x9049;
        case 0xA5:            /* 0xE7A5 */
          return 0x903E;
        case 0xA6:            /* 0xE7A6 */
          return 0x9056;
        case 0xA7:            /* 0xE7A7 */
          return 0x9058;
        case 0xA8:            /* 0xE7A8 */
          return 0x905E;
        case 0xA9:            /* 0xE7A9 */
          return 0x9068;
        case 0xAA:            /* 0xE7AA */
          return 0x906F;
        case 0xAB:            /* 0xE7AB */
          return 0x9076;
        case 0xAC:            /* 0xE7AC */
          return 0x96A8;
        case 0xAD:            /* 0xE7AD */
          return 0x9072;
        case 0xAE:            /* 0xE7AE */
          return 0x9082;
        case 0xAF:            /* 0xE7AF */
          return 0x907D;
        case 0xB0:            /* 0xE7B0 */
          return 0x9081;
        case 0xB1:            /* 0xE7B1 */
          return 0x9080;
        case 0xB2:            /* 0xE7B2 */
          return 0x908A;
        case 0xB3:            /* 0xE7B3 */
          return 0x9089;
        case 0xB4:            /* 0xE7B4 */
          return 0x908F;
        case 0xB5:            /* 0xE7B5 */
          return 0x90A8;
        case 0xB6:            /* 0xE7B6 */
          return 0x90AF;
        case 0xB7:            /* 0xE7B7 */
          return 0x90B1;
        case 0xB8:            /* 0xE7B8 */
          return 0x90B5;
        case 0xB9:            /* 0xE7B9 */
          return 0x90E2;
        case 0xBA:            /* 0xE7BA */
          return 0x90E4;
        case 0xBB:            /* 0xE7BB */
          return 0x6248;
        case 0xBC:            /* 0xE7BC */
          return 0x90DB;
        case 0xBD:            /* 0xE7BD */
          return 0x9102;
        case 0xBE:            /* 0xE7BE */
          return 0x9112;
        case 0xBF:            /* 0xE7BF */
          return 0x9119;
        case 0xC0:            /* 0xE7C0 */
          return 0x9132;
        case 0xC1:            /* 0xE7C1 */
          return 0x9130;
        case 0xC2:            /* 0xE7C2 */
          return 0x914A;
        case 0xC3:            /* 0xE7C3 */
          return 0x9156;
        case 0xC4:            /* 0xE7C4 */
          return 0x9158;
        case 0xC5:            /* 0xE7C5 */
          return 0x9163;
        case 0xC6:            /* 0xE7C6 */
          return 0x9165;
        case 0xC7:            /* 0xE7C7 */
          return 0x9169;
        case 0xC8:            /* 0xE7C8 */
          return 0x9173;
        case 0xC9:            /* 0xE7C9 */
          return 0x9172;
        case 0xCA:            /* 0xE7CA */
          return 0x918B;
        case 0xCB:            /* 0xE7CB */
          return 0x9189;
        case 0xCC:            /* 0xE7CC */
          return 0x9182;
        case 0xCD:            /* 0xE7CD */
          return 0x91A2;
        case 0xCE:            /* 0xE7CE */
          return 0x91AB;
        case 0xCF:            /* 0xE7CF */
          return 0x91AF;
        case 0xD0:            /* 0xE7D0 */
          return 0x91AA;
        case 0xD1:            /* 0xE7D1 */
          return 0x91B5;
        case 0xD2:            /* 0xE7D2 */
          return 0x91B4;
        case 0xD3:            /* 0xE7D3 */
          return 0x91BA;
        case 0xD4:            /* 0xE7D4 */
          return 0x91C0;
        case 0xD5:            /* 0xE7D5 */
          return 0x91C1;
        case 0xD6:            /* 0xE7D6 */
          return 0x91C9;
        case 0xD7:            /* 0xE7D7 */
          return 0x91CB;
        case 0xD8:            /* 0xE7D8 */
          return 0x91D0;
        case 0xD9:            /* 0xE7D9 */
          return 0x91D6;
        case 0xDA:            /* 0xE7DA */
          return 0x91DF;
        case 0xDB:            /* 0xE7DB */
          return 0x91E1;
        case 0xDC:            /* 0xE7DC */
          return 0x91DB;
        case 0xDD:            /* 0xE7DD */
          return 0x91FC;
        case 0xDE:            /* 0xE7DE */
          return 0x91F5;
        case 0xDF:            /* 0xE7DF */
          return 0x91F6;
        case 0xE0:            /* 0xE7E0 */
          return 0x921E;
        case 0xE1:            /* 0xE7E1 */
          return 0x91FF;
        case 0xE2:            /* 0xE7E2 */
          return 0x9214;
        case 0xE3:            /* 0xE7E3 */
          return 0x922C;
        case 0xE4:            /* 0xE7E4 */
          return 0x9215;
        case 0xE5:            /* 0xE7E5 */
          return 0x9211;
        case 0xE6:            /* 0xE7E6 */
          return 0x925E;
        case 0xE7:            /* 0xE7E7 */
          return 0x9257;
        case 0xE8:            /* 0xE7E8 */
          return 0x9245;
        case 0xE9:            /* 0xE7E9 */
          return 0x9249;
        case 0xEA:            /* 0xE7EA */
          return 0x9264;
        case 0xEB:            /* 0xE7EB */
          return 0x9248;
        case 0xEC:            /* 0xE7EC */
          return 0x9295;
        case 0xED:            /* 0xE7ED */
          return 0x923F;
        case 0xEE:            /* 0xE7EE */
          return 0x924B;
        case 0xEF:            /* 0xE7EF */
          return 0x9250;
        case 0xF0:            /* 0xE7F0 */
          return 0x929C;
        case 0xF1:            /* 0xE7F1 */
          return 0x9296;
        case 0xF2:            /* 0xE7F2 */
          return 0x9293;
        case 0xF3:            /* 0xE7F3 */
          return 0x929B;
        case 0xF4:            /* 0xE7F4 */
          return 0x925A;
        case 0xF5:            /* 0xE7F5 */
          return 0x92CF;
        case 0xF6:            /* 0xE7F6 */
          return 0x92B9;
        case 0xF7:            /* 0xE7F7 */
          return 0x92B7;
        case 0xF8:            /* 0xE7F8 */
          return 0x92E9;
        case 0xF9:            /* 0xE7F9 */
          return 0x930F;
        case 0xFA:            /* 0xE7FA */
          return 0x92FA;
        case 0xFB:            /* 0xE7FB */
          return 0x9344;
        case 0xFC:            /* 0xE7FC */
          return 0x932E;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead E7 */

    /* Lead Byte: 0xE8xx */
    case 0xE8:
      switch (low_byte)
      {
        case 0x40:            /* 0xE840 */
          return 0x9319;
        case 0x41:            /* 0xE841 */
          return 0x9322;
        case 0x42:            /* 0xE842 */
          return 0x931A;
        case 0x43:            /* 0xE843 */
          return 0x9323;
        case 0x44:            /* 0xE844 */
          return 0x933A;
        case 0x45:            /* 0xE845 */
          return 0x9335;
        case 0x46:            /* 0xE846 */
          return 0x933B;
        case 0x47:            /* 0xE847 */
          return 0x935C;
        case 0x48:            /* 0xE848 */
          return 0x9360;
        case 0x49:            /* 0xE849 */
          return 0x937C;
        case 0x4A:            /* 0xE84A */
          return 0x936E;
        case 0x4B:            /* 0xE84B */
          return 0x9356;
        case 0x4C:            /* 0xE84C */
          return 0x93B0;
        case 0x4D:            /* 0xE84D */
          return 0x93AC;
        case 0x4E:            /* 0xE84E */
          return 0x93AD;
        case 0x4F:            /* 0xE84F */
          return 0x9394;
        case 0x50:            /* 0xE850 */
          return 0x93B9;
        case 0x51:            /* 0xE851 */
          return 0x93D6;
        case 0x52:            /* 0xE852 */
          return 0x93D7;
        case 0x53:            /* 0xE853 */
          return 0x93E8;
        case 0x54:            /* 0xE854 */
          return 0x93E5;
        case 0x55:            /* 0xE855 */
          return 0x93D8;
        case 0x56:            /* 0xE856 */
          return 0x93C3;
        case 0x57:            /* 0xE857 */
          return 0x93DD;
        case 0x58:            /* 0xE858 */
          return 0x93D0;
        case 0x59:            /* 0xE859 */
          return 0x93C8;
        case 0x5A:            /* 0xE85A */
          return 0x93E4;
        case 0x5B:            /* 0xE85B */
          return 0x941A;
        case 0x5C:            /* 0xE85C */
          return 0x9414;
        case 0x5D:            /* 0xE85D */
          return 0x9413;
        case 0x5E:            /* 0xE85E */
          return 0x9403;
        case 0x5F:            /* 0xE85F */
          return 0x9407;
        case 0x60:            /* 0xE860 */
          return 0x9410;
        case 0x61:            /* 0xE861 */
          return 0x9436;
        case 0x62:            /* 0xE862 */
          return 0x942B;
        case 0x63:            /* 0xE863 */
          return 0x9435;
        case 0x64:            /* 0xE864 */
          return 0x9421;
        case 0x65:            /* 0xE865 */
          return 0x943A;
        case 0x66:            /* 0xE866 */
          return 0x9441;
        case 0x67:            /* 0xE867 */
          return 0x9452;
        case 0x68:            /* 0xE868 */
          return 0x9444;
        case 0x69:            /* 0xE869 */
          return 0x945B;
        case 0x6A:            /* 0xE86A */
          return 0x9460;
        case 0x6B:            /* 0xE86B */
          return 0x9462;
        case 0x6C:            /* 0xE86C */
          return 0x945E;
        case 0x6D:            /* 0xE86D */
          return 0x946A;
        case 0x6E:            /* 0xE86E */
          return 0x9229;
        case 0x6F:            /* 0xE86F */
          return 0x9470;
        case 0x70:            /* 0xE870 */
          return 0x9475;
        case 0x71:            /* 0xE871 */
          return 0x9477;
        case 0x72:            /* 0xE872 */
          return 0x947D;
        case 0x73:            /* 0xE873 */
          return 0x945A;
        case 0x74:            /* 0xE874 */
          return 0x947C;
        case 0x75:            /* 0xE875 */
          return 0x947E;
        case 0x76:            /* 0xE876 */
          return 0x9481;
        case 0x77:            /* 0xE877 */
          return 0x947F;
        case 0x78:            /* 0xE878 */
          return 0x9582;
        case 0x79:            /* 0xE879 */
          return 0x9587;
        case 0x7A:            /* 0xE87A */
          return 0x958A;
        case 0x7B:            /* 0xE87B */
          return 0x9594;
        case 0x7C:            /* 0xE87C */
          return 0x9596;
        case 0x7D:            /* 0xE87D */
          return 0x9598;
        case 0x7E:            /* 0xE87E */
          return 0x9599;
        case 0x80:            /* 0xE880 */
          return 0x95A0;
        case 0x81:            /* 0xE881 */
          return 0x95A8;
        case 0x82:            /* 0xE882 */
          return 0x95A7;
        case 0x83:            /* 0xE883 */
          return 0x95AD;
        case 0x84:            /* 0xE884 */
          return 0x95BC;
        case 0x85:            /* 0xE885 */
          return 0x95BB;
        case 0x86:            /* 0xE886 */
          return 0x95B9;
        case 0x87:            /* 0xE887 */
          return 0x95BE;
        case 0x88:            /* 0xE888 */
          return 0x95CA;
        case 0x89:            /* 0xE889 */
          return 0x6FF6;
        case 0x8A:            /* 0xE88A */
          return 0x95C3;
        case 0x8B:            /* 0xE88B */
          return 0x95CD;
        case 0x8C:            /* 0xE88C */
          return 0x95CC;
        case 0x8D:            /* 0xE88D */
          return 0x95D5;
        case 0x8E:            /* 0xE88E */
          return 0x95D4;
        case 0x8F:            /* 0xE88F */
          return 0x95D6;
        case 0x90:            /* 0xE890 */
          return 0x95DC;
        case 0x91:            /* 0xE891 */
          return 0x95E1;
        case 0x92:            /* 0xE892 */
          return 0x95E5;
        case 0x93:            /* 0xE893 */
          return 0x95E2;
        case 0x94:            /* 0xE894 */
          return 0x9621;
        case 0x95:            /* 0xE895 */
          return 0x9628;
        case 0x96:            /* 0xE896 */
          return 0x962E;
        case 0x97:            /* 0xE897 */
          return 0x962F;
        case 0x98:            /* 0xE898 */
          return 0x9642;
        case 0x99:            /* 0xE899 */
          return 0x964C;
        case 0x9A:            /* 0xE89A */
          return 0x964F;
        case 0x9B:            /* 0xE89B */
          return 0x964B;
        case 0x9C:            /* 0xE89C */
          return 0x9677;
        case 0x9D:            /* 0xE89D */
          return 0x965C;
        case 0x9E:            /* 0xE89E */
          return 0x965E;
        case 0x9F:            /* 0xE89F */
          return 0x965D;
        case 0xA0:            /* 0xE8A0 */
          return 0x965F;
        case 0xA1:            /* 0xE8A1 */
          return 0x9666;
        case 0xA2:            /* 0xE8A2 */
          return 0x9672;
        case 0xA3:            /* 0xE8A3 */
          return 0x966C;
        case 0xA4:            /* 0xE8A4 */
          return 0x968D;
        case 0xA5:            /* 0xE8A5 */
          return 0x9698;
        case 0xA6:            /* 0xE8A6 */
          return 0x9695;
        case 0xA7:            /* 0xE8A7 */
          return 0x9697;
        case 0xA8:            /* 0xE8A8 */
          return 0x96AA;
        case 0xA9:            /* 0xE8A9 */
          return 0x96A7;
        case 0xAA:            /* 0xE8AA */
          return 0x96B1;
        case 0xAB:            /* 0xE8AB */
          return 0x96B2;
        case 0xAC:            /* 0xE8AC */
          return 0x96B0;
        case 0xAD:            /* 0xE8AD */
          return 0x96B4;
        case 0xAE:            /* 0xE8AE */
          return 0x96B6;
        case 0xAF:            /* 0xE8AF */
          return 0x96B8;
        case 0xB0:            /* 0xE8B0 */
          return 0x96B9;
        case 0xB1:            /* 0xE8B1 */
          return 0x96CE;
        case 0xB2:            /* 0xE8B2 */
          return 0x96CB;
        case 0xB3:            /* 0xE8B3 */
          return 0x96C9;
        case 0xB4:            /* 0xE8B4 */
          return 0x96CD;
        case 0xB5:            /* 0xE8B5 */
          return 0x894D;
        case 0xB6:            /* 0xE8B6 */
          return 0x96DC;
        case 0xB7:            /* 0xE8B7 */
          return 0x970D;
        case 0xB8:            /* 0xE8B8 */
          return 0x96D5;
        case 0xB9:            /* 0xE8B9 */
          return 0x96F9;
        case 0xBA:            /* 0xE8BA */
          return 0x9704;
        case 0xBB:            /* 0xE8BB */
          return 0x9706;
        case 0xBC:            /* 0xE8BC */
          return 0x9708;
        case 0xBD:            /* 0xE8BD */
          return 0x9713;
        case 0xBE:            /* 0xE8BE */
          return 0x970E;
        case 0xBF:            /* 0xE8BF */
          return 0x9711;
        case 0xC0:            /* 0xE8C0 */
          return 0x970F;
        case 0xC1:            /* 0xE8C1 */
          return 0x9716;
        case 0xC2:            /* 0xE8C2 */
          return 0x9719;
        case 0xC3:            /* 0xE8C3 */
          return 0x9724;
        case 0xC4:            /* 0xE8C4 */
          return 0x972A;
        case 0xC5:            /* 0xE8C5 */
          return 0x9730;
        case 0xC6:            /* 0xE8C6 */
          return 0x9739;
        case 0xC7:            /* 0xE8C7 */
          return 0x973D;
        case 0xC8:            /* 0xE8C8 */
          return 0x973E;
        case 0xC9:            /* 0xE8C9 */
          return 0x9744;
        case 0xCA:            /* 0xE8CA */
          return 0x9746;
        case 0xCB:            /* 0xE8CB */
          return 0x9748;
        case 0xCC:            /* 0xE8CC */
          return 0x9742;
        case 0xCD:            /* 0xE8CD */
          return 0x9749;
        case 0xCE:            /* 0xE8CE */
          return 0x975C;
        case 0xCF:            /* 0xE8CF */
          return 0x9760;
        case 0xD0:            /* 0xE8D0 */
          return 0x9764;
        case 0xD1:            /* 0xE8D1 */
          return 0x9766;
        case 0xD2:            /* 0xE8D2 */
          return 0x9768;
        case 0xD3:            /* 0xE8D3 */
          return 0x52D2;
        case 0xD4:            /* 0xE8D4 */
          return 0x976B;
        case 0xD5:            /* 0xE8D5 */
          return 0x9771;
        case 0xD6:            /* 0xE8D6 */
          return 0x9779;
        case 0xD7:            /* 0xE8D7 */
          return 0x9785;
        case 0xD8:            /* 0xE8D8 */
          return 0x977C;
        case 0xD9:            /* 0xE8D9 */
          return 0x9781;
        case 0xDA:            /* 0xE8DA */
          return 0x977A;
        case 0xDB:            /* 0xE8DB */
          return 0x9786;
        case 0xDC:            /* 0xE8DC */
          return 0x978B;
        case 0xDD:            /* 0xE8DD */
          return 0x978F;
        case 0xDE:            /* 0xE8DE */
          return 0x9790;
        case 0xDF:            /* 0xE8DF */
          return 0x979C;
        case 0xE0:            /* 0xE8E0 */
          return 0x97A8;
        case 0xE1:            /* 0xE8E1 */
          return 0x97A6;
        case 0xE2:            /* 0xE8E2 */
          return 0x97A3;
        case 0xE3:            /* 0xE8E3 */
          return 0x97B3;
        case 0xE4:            /* 0xE8E4 */
          return 0x97B4;
        case 0xE5:            /* 0xE8E5 */
          return 0x97C3;
        case 0xE6:            /* 0xE8E6 */
          return 0x97C6;
        case 0xE7:            /* 0xE8E7 */
          return 0x97C8;
        case 0xE8:            /* 0xE8E8 */
          return 0x97CB;
        case 0xE9:            /* 0xE8E9 */
          return 0x97DC;
        case 0xEA:            /* 0xE8EA */
          return 0x97ED;
        case 0xEB:            /* 0xE8EB */
          return 0x9F4F;
        case 0xEC:            /* 0xE8EC */
          return 0x97F2;
        case 0xED:            /* 0xE8ED */
          return 0x7ADF;
        case 0xEE:            /* 0xE8EE */
          return 0x97F6;
        case 0xEF:            /* 0xE8EF */
          return 0x97F5;
        case 0xF0:            /* 0xE8F0 */
          return 0x980F;
        case 0xF1:            /* 0xE8F1 */
          return 0x980C;
        case 0xF2:            /* 0xE8F2 */
          return 0x9838;
        case 0xF3:            /* 0xE8F3 */
          return 0x9824;
        case 0xF4:            /* 0xE8F4 */
          return 0x9821;
        case 0xF5:            /* 0xE8F5 */
          return 0x9837;
        case 0xF6:            /* 0xE8F6 */
          return 0x983D;
        case 0xF7:            /* 0xE8F7 */
          return 0x9846;
        case 0xF8:            /* 0xE8F8 */
          return 0x984F;
        case 0xF9:            /* 0xE8F9 */
          return 0x984B;
        case 0xFA:            /* 0xE8FA */
          return 0x986B;
        case 0xFB:            /* 0xE8FB */
          return 0x986F;
        case 0xFC:            /* 0xE8FC */
          return 0x9870;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead E8 */

    /* Lead Byte: 0xE9xx */
    case 0xE9:
      switch (low_byte)
      {
        case 0x40:            /* 0xE940 */
          return 0x9871;
        case 0x41:            /* 0xE941 */
          return 0x9874;
        case 0x42:            /* 0xE942 */
          return 0x9873;
        case 0x43:            /* 0xE943 */
          return 0x98AA;
        case 0x44:            /* 0xE944 */
          return 0x98AF;
        case 0x45:            /* 0xE945 */
          return 0x98B1;
        case 0x46:            /* 0xE946 */
          return 0x98B6;
        case 0x47:            /* 0xE947 */
          return 0x98C4;
        case 0x48:            /* 0xE948 */
          return 0x98C3;
        case 0x49:            /* 0xE949 */
          return 0x98C6;
        case 0x4A:            /* 0xE94A */
          return 0x98E9;
        case 0x4B:            /* 0xE94B */
          return 0x98EB;
        case 0x4C:            /* 0xE94C */
          return 0x9903;
        case 0x4D:            /* 0xE94D */
          return 0x9909;
        case 0x4E:            /* 0xE94E */
          return 0x9912;
        case 0x4F:            /* 0xE94F */
          return 0x9914;
        case 0x50:            /* 0xE950 */
          return 0x9918;
        case 0x51:            /* 0xE951 */
          return 0x9921;
        case 0x52:            /* 0xE952 */
          return 0x991D;
        case 0x53:            /* 0xE953 */
          return 0x991E;
        case 0x54:            /* 0xE954 */
          return 0x9924;
        case 0x55:            /* 0xE955 */
          return 0x9920;
        case 0x56:            /* 0xE956 */
          return 0x992C;
        case 0x57:            /* 0xE957 */
          return 0x992E;
        case 0x58:            /* 0xE958 */
          return 0x993D;
        case 0x59:            /* 0xE959 */
          return 0x993E;
        case 0x5A:            /* 0xE95A */
          return 0x9942;
        case 0x5B:            /* 0xE95B */
          return 0x9949;
        case 0x5C:            /* 0xE95C */
          return 0x9945;
        case 0x5D:            /* 0xE95D */
          return 0x9950;
        case 0x5E:            /* 0xE95E */
          return 0x994B;
        case 0x5F:            /* 0xE95F */
          return 0x9951;
        case 0x60:            /* 0xE960 */
          return 0x9952;
        case 0x61:            /* 0xE961 */
          return 0x994C;
        case 0x62:            /* 0xE962 */
          return 0x9955;
        case 0x63:            /* 0xE963 */
          return 0x9997;
        case 0x64:            /* 0xE964 */
          return 0x9998;
        case 0x65:            /* 0xE965 */
          return 0x99A5;
        case 0x66:            /* 0xE966 */
          return 0x99AD;
        case 0x67:            /* 0xE967 */
          return 0x99AE;
        case 0x68:            /* 0xE968 */
          return 0x99BC;
        case 0x69:            /* 0xE969 */
          return 0x99DF;
        case 0x6A:            /* 0xE96A */
          return 0x99DB;
        case 0x6B:            /* 0xE96B */
          return 0x99DD;
        case 0x6C:            /* 0xE96C */
          return 0x99D8;
        case 0x6D:            /* 0xE96D */
          return 0x99D1;
        case 0x6E:            /* 0xE96E */
          return 0x99ED;
        case 0x6F:            /* 0xE96F */
          return 0x99EE;
        case 0x70:            /* 0xE970 */
          return 0x99F1;
        case 0x71:            /* 0xE971 */
          return 0x99F2;
        case 0x72:            /* 0xE972 */
          return 0x99FB;
        case 0x73:            /* 0xE973 */
          return 0x99F8;
        case 0x74:            /* 0xE974 */
          return 0x9A01;
        case 0x75:            /* 0xE975 */
          return 0x9A0F;
        case 0x76:            /* 0xE976 */
          return 0x9A05;
        case 0x77:            /* 0xE977 */
          return 0x99E2;
        case 0x78:            /* 0xE978 */
          return 0x9A19;
        case 0x79:            /* 0xE979 */
          return 0x9A2B;
        case 0x7A:            /* 0xE97A */
          return 0x9A37;
        case 0x7B:            /* 0xE97B */
          return 0x9A45;
        case 0x7C:            /* 0xE97C */
          return 0x9A42;
        case 0x7D:            /* 0xE97D */
          return 0x9A40;
        case 0x7E:            /* 0xE97E */
          return 0x9A43;
        case 0x80:            /* 0xE980 */
          return 0x9A3E;
        case 0x81:            /* 0xE981 */
          return 0x9A55;
        case 0x82:            /* 0xE982 */
          return 0x9A4D;
        case 0x83:            /* 0xE983 */
          return 0x9A5B;
        case 0x84:            /* 0xE984 */
          return 0x9A57;
        case 0x85:            /* 0xE985 */
          return 0x9A5F;
        case 0x86:            /* 0xE986 */
          return 0x9A62;
        case 0x87:            /* 0xE987 */
          return 0x9A65;
        case 0x88:            /* 0xE988 */
          return 0x9A64;
        case 0x89:            /* 0xE989 */
          return 0x9A69;
        case 0x8A:            /* 0xE98A */
          return 0x9A6B;
        case 0x8B:            /* 0xE98B */
          return 0x9A6A;
        case 0x8C:            /* 0xE98C */
          return 0x9AAD;
        case 0x8D:            /* 0xE98D */
          return 0x9AB0;
        case 0x8E:            /* 0xE98E */
          return 0x9ABC;
        case 0x8F:            /* 0xE98F */
          return 0x9AC0;
        case 0x90:            /* 0xE990 */
          return 0x9ACF;
        case 0x91:            /* 0xE991 */
          return 0x9AD1;
        case 0x92:            /* 0xE992 */
          return 0x9AD3;
        case 0x93:            /* 0xE993 */
          return 0x9AD4;
        case 0x94:            /* 0xE994 */
          return 0x9ADE;
        case 0x95:            /* 0xE995 */
          return 0x9ADF;
        case 0x96:            /* 0xE996 */
          return 0x9AE2;
        case 0x97:            /* 0xE997 */
          return 0x9AE3;
        case 0x98:            /* 0xE998 */
          return 0x9AE6;
        case 0x99:            /* 0xE999 */
          return 0x9AEF;
        case 0x9A:            /* 0xE99A */
          return 0x9AEB;
        case 0x9B:            /* 0xE99B */
          return 0x9AEE;
        case 0x9C:            /* 0xE99C */
          return 0x9AF4;
        case 0x9D:            /* 0xE99D */
          return 0x9AF1;
        case 0x9E:            /* 0xE99E */
          return 0x9AF7;
        case 0x9F:            /* 0xE99F */
          return 0x9AFB;
        case 0xA0:            /* 0xE9A0 */
          return 0x9B06;
        case 0xA1:            /* 0xE9A1 */
          return 0x9B18;
        case 0xA2:            /* 0xE9A2 */
          return 0x9B1A;
        case 0xA3:            /* 0xE9A3 */
          return 0x9B1F;
        case 0xA4:            /* 0xE9A4 */
          return 0x9B22;
        case 0xA5:            /* 0xE9A5 */
          return 0x9B23;
        case 0xA6:            /* 0xE9A6 */
          return 0x9B25;
        case 0xA7:            /* 0xE9A7 */
          return 0x9B27;
        case 0xA8:            /* 0xE9A8 */
          return 0x9B28;
        case 0xA9:            /* 0xE9A9 */
          return 0x9B29;
        case 0xAA:            /* 0xE9AA */
          return 0x9B2A;
        case 0xAB:            /* 0xE9AB */
          return 0x9B2E;
        case 0xAC:            /* 0xE9AC */
          return 0x9B2F;
        case 0xAD:            /* 0xE9AD */
          return 0x9B32;
        case 0xAE:            /* 0xE9AE */
          return 0x9B44;
        case 0xAF:            /* 0xE9AF */
          return 0x9B43;
        case 0xB0:            /* 0xE9B0 */
          return 0x9B4F;
        case 0xB1:            /* 0xE9B1 */
          return 0x9B4D;
        case 0xB2:            /* 0xE9B2 */
          return 0x9B4E;
        case 0xB3:            /* 0xE9B3 */
          return 0x9B51;
        case 0xB4:            /* 0xE9B4 */
          return 0x9B58;
        case 0xB5:            /* 0xE9B5 */
          return 0x9B74;
        case 0xB6:            /* 0xE9B6 */
          return 0x9B93;
        case 0xB7:            /* 0xE9B7 */
          return 0x9B83;
        case 0xB8:            /* 0xE9B8 */
          return 0x9B91;
        case 0xB9:            /* 0xE9B9 */
          return 0x9B96;
        case 0xBA:            /* 0xE9BA */
          return 0x9B97;
        case 0xBB:            /* 0xE9BB */
          return 0x9B9F;
        case 0xBC:            /* 0xE9BC */
          return 0x9BA0;
        case 0xBD:            /* 0xE9BD */
          return 0x9BA8;
        case 0xBE:            /* 0xE9BE */
          return 0x9BB4;
        case 0xBF:            /* 0xE9BF */
          return 0x9BC0;
        case 0xC0:            /* 0xE9C0 */
          return 0x9BCA;
        case 0xC1:            /* 0xE9C1 */
          return 0x9BB9;
        case 0xC2:            /* 0xE9C2 */
          return 0x9BC6;
        case 0xC3:            /* 0xE9C3 */
          return 0x9BCF;
        case 0xC4:            /* 0xE9C4 */
          return 0x9BD1;
        case 0xC5:            /* 0xE9C5 */
          return 0x9BD2;
        case 0xC6:            /* 0xE9C6 */
          return 0x9BE3;
        case 0xC7:            /* 0xE9C7 */
          return 0x9BE2;
        case 0xC8:            /* 0xE9C8 */
          return 0x9BE4;
        case 0xC9:            /* 0xE9C9 */
          return 0x9BD4;
        case 0xCA:            /* 0xE9CA */
          return 0x9BE1;
        case 0xCB:            /* 0xE9CB */
          return 0x9C3A;
        case 0xCC:            /* 0xE9CC */
          return 0x9BF2;
        case 0xCD:            /* 0xE9CD */
          return 0x9BF1;
        case 0xCE:            /* 0xE9CE */
          return 0x9BF0;
        case 0xCF:            /* 0xE9CF */
          return 0x9C15;
        case 0xD0:            /* 0xE9D0 */
          return 0x9C14;
        case 0xD1:            /* 0xE9D1 */
          return 0x9C09;
        case 0xD2:            /* 0xE9D2 */
          return 0x9C13;
        case 0xD3:            /* 0xE9D3 */
          return 0x9C0C;
        case 0xD4:            /* 0xE9D4 */
          return 0x9C06;
        case 0xD5:            /* 0xE9D5 */
          return 0x9C08;
        case 0xD6:            /* 0xE9D6 */
          return 0x9C12;
        case 0xD7:            /* 0xE9D7 */
          return 0x9C0A;
        case 0xD8:            /* 0xE9D8 */
          return 0x9C04;
        case 0xD9:            /* 0xE9D9 */
          return 0x9C2E;
        case 0xDA:            /* 0xE9DA */
          return 0x9C1B;
        case 0xDB:            /* 0xE9DB */
          return 0x9C25;
        case 0xDC:            /* 0xE9DC */
          return 0x9C24;
        case 0xDD:            /* 0xE9DD */
          return 0x9C21;
        case 0xDE:            /* 0xE9DE */
          return 0x9C30;
        case 0xDF:            /* 0xE9DF */
          return 0x9C47;
        case 0xE0:            /* 0xE9E0 */
          return 0x9C32;
        case 0xE1:            /* 0xE9E1 */
          return 0x9C46;
        case 0xE2:            /* 0xE9E2 */
          return 0x9C3E;
        case 0xE3:            /* 0xE9E3 */
          return 0x9C5A;
        case 0xE4:            /* 0xE9E4 */
          return 0x9C60;
        case 0xE5:            /* 0xE9E5 */
          return 0x9C67;
        case 0xE6:            /* 0xE9E6 */
          return 0x9C76;
        case 0xE7:            /* 0xE9E7 */
          return 0x9C78;
        case 0xE8:            /* 0xE9E8 */
          return 0x9CE7;
        case 0xE9:            /* 0xE9E9 */
          return 0x9CEC;
        case 0xEA:            /* 0xE9EA */
          return 0x9CF0;
        case 0xEB:            /* 0xE9EB */
          return 0x9D09;
        case 0xEC:            /* 0xE9EC */
          return 0x9D08;
        case 0xED:            /* 0xE9ED */
          return 0x9CEB;
        case 0xEE:            /* 0xE9EE */
          return 0x9D03;
        case 0xEF:            /* 0xE9EF */
          return 0x9D06;
        case 0xF0:            /* 0xE9F0 */
          return 0x9D2A;
        case 0xF1:            /* 0xE9F1 */
          return 0x9D26;
        case 0xF2:            /* 0xE9F2 */
          return 0x9DAF;
        case 0xF3:            /* 0xE9F3 */
          return 0x9D23;
        case 0xF4:            /* 0xE9F4 */
          return 0x9D1F;
        case 0xF5:            /* 0xE9F5 */
          return 0x9D44;
        case 0xF6:            /* 0xE9F6 */
          return 0x9D15;
        case 0xF7:            /* 0xE9F7 */
          return 0x9D12;
        case 0xF8:            /* 0xE9F8 */
          return 0x9D41;
        case 0xF9:            /* 0xE9F9 */
          return 0x9D3F;
        case 0xFA:            /* 0xE9FA */
          return 0x9D3E;
        case 0xFB:            /* 0xE9FB */
          return 0x9D46;
        case 0xFC:            /* 0xE9FC */
          return 0x9D48;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead E9 */

    /* Lead Byte: 0xEAxx */
    case 0xEA:
      switch (low_byte)
      {
        case 0x40:            /* 0xEA40 */
          return 0x9D5D;
        case 0x41:            /* 0xEA41 */
          return 0x9D5E;
        case 0x42:            /* 0xEA42 */
          return 0x9D64;
        case 0x43:            /* 0xEA43 */
          return 0x9D51;
        case 0x44:            /* 0xEA44 */
          return 0x9D50;
        case 0x45:            /* 0xEA45 */
          return 0x9D59;
        case 0x46:            /* 0xEA46 */
          return 0x9D72;
        case 0x47:            /* 0xEA47 */
          return 0x9D89;
        case 0x48:            /* 0xEA48 */
          return 0x9D87;
        case 0x49:            /* 0xEA49 */
          return 0x9DAB;
        case 0x4A:            /* 0xEA4A */
          return 0x9D6F;
        case 0x4B:            /* 0xEA4B */
          return 0x9D7A;
        case 0x4C:            /* 0xEA4C */
          return 0x9D9A;
        case 0x4D:            /* 0xEA4D */
          return 0x9DA4;
        case 0x4E:            /* 0xEA4E */
          return 0x9DA9;
        case 0x4F:            /* 0xEA4F */
          return 0x9DB2;
        case 0x50:            /* 0xEA50 */
          return 0x9DC4;
        case 0x51:            /* 0xEA51 */
          return 0x9DC1;
        case 0x52:            /* 0xEA52 */
          return 0x9DBB;
        case 0x53:            /* 0xEA53 */
          return 0x9DB8;
        case 0x54:            /* 0xEA54 */
          return 0x9DBA;
        case 0x55:            /* 0xEA55 */
          return 0x9DC6;
        case 0x56:            /* 0xEA56 */
          return 0x9DCF;
        case 0x57:            /* 0xEA57 */
          return 0x9DC2;
        case 0x58:            /* 0xEA58 */
          return 0x9DD9;
        case 0x59:            /* 0xEA59 */
          return 0x9DD3;
        case 0x5A:            /* 0xEA5A */
          return 0x9DF8;
        case 0x5B:            /* 0xEA5B */
          return 0x9DE6;
        case 0x5C:            /* 0xEA5C */
          return 0x9DED;
        case 0x5D:            /* 0xEA5D */
          return 0x9DEF;
        case 0x5E:            /* 0xEA5E */
          return 0x9DFD;
        case 0x5F:            /* 0xEA5F */
          return 0x9E1A;
        case 0x60:            /* 0xEA60 */
          return 0x9E1B;
        case 0x61:            /* 0xEA61 */
          return 0x9E1E;
        case 0x62:            /* 0xEA62 */
          return 0x9E75;
        case 0x63:            /* 0xEA63 */
          return 0x9E79;
        case 0x64:            /* 0xEA64 */
          return 0x9E7D;
        case 0x65:            /* 0xEA65 */
          return 0x9E81;
        case 0x66:            /* 0xEA66 */
          return 0x9E88;
        case 0x67:            /* 0xEA67 */
          return 0x9E8B;
        case 0x68:            /* 0xEA68 */
          return 0x9E8C;
        case 0x69:            /* 0xEA69 */
          return 0x9E92;
        case 0x6A:            /* 0xEA6A */
          return 0x9E95;
        case 0x6B:            /* 0xEA6B */
          return 0x9E91;
        case 0x6C:            /* 0xEA6C */
          return 0x9E9D;
        case 0x6D:            /* 0xEA6D */
          return 0x9EA5;
        case 0x6E:            /* 0xEA6E */
          return 0x9EA9;
        case 0x6F:            /* 0xEA6F */
          return 0x9EB8;
        case 0x70:            /* 0xEA70 */
          return 0x9EAA;
        case 0x71:            /* 0xEA71 */
          return 0x9EAD;
        case 0x72:            /* 0xEA72 */
          return 0x9761;
        case 0x73:            /* 0xEA73 */
          return 0x9ECC;
        case 0x74:            /* 0xEA74 */
          return 0x9ECE;
        case 0x75:            /* 0xEA75 */
          return 0x9ECF;
        case 0x76:            /* 0xEA76 */
          return 0x9ED0;
        case 0x77:            /* 0xEA77 */
          return 0x9ED4;
        case 0x78:            /* 0xEA78 */
          return 0x9EDC;
        case 0x79:            /* 0xEA79 */
          return 0x9EDE;
        case 0x7A:            /* 0xEA7A */
          return 0x9EDD;
        case 0x7B:            /* 0xEA7B */
          return 0x9EE0;
        case 0x7C:            /* 0xEA7C */
          return 0x9EE5;
        case 0x7D:            /* 0xEA7D */
          return 0x9EE8;
        case 0x7E:            /* 0xEA7E */
          return 0x9EEF;
        case 0x80:            /* 0xEA80 */
          return 0x9EF4;
        case 0x81:            /* 0xEA81 */
          return 0x9EF6;
        case 0x82:            /* 0xEA82 */
          return 0x9EF7;
        case 0x83:            /* 0xEA83 */
          return 0x9EF9;
        case 0x84:            /* 0xEA84 */
          return 0x9EFB;
        case 0x85:            /* 0xEA85 */
          return 0x9EFC;
        case 0x86:            /* 0xEA86 */
          return 0x9EFD;
        case 0x87:            /* 0xEA87 */
          return 0x9F07;
        case 0x88:            /* 0xEA88 */
          return 0x9F08;
        case 0x89:            /* 0xEA89 */
          return 0x76B7;
        case 0x8A:            /* 0xEA8A */
          return 0x9F15;
        case 0x8B:            /* 0xEA8B */
          return 0x9F21;
        case 0x8C:            /* 0xEA8C */
          return 0x9F2C;
        case 0x8D:            /* 0xEA8D */
          return 0x9F3E;
        case 0x8E:            /* 0xEA8E */
          return 0x9F4A;
        case 0x8F:            /* 0xEA8F */
          return 0x9F52;
        case 0x90:            /* 0xEA90 */
          return 0x9F54;
        case 0x91:            /* 0xEA91 */
          return 0x9F63;
        case 0x92:            /* 0xEA92 */
          return 0x9F5F;
        case 0x93:            /* 0xEA93 */
          return 0x9F60;
        case 0x94:            /* 0xEA94 */
          return 0x9F61;
        case 0x95:            /* 0xEA95 */
          return 0x9F66;
        case 0x96:            /* 0xEA96 */
          return 0x9F67;
        case 0x97:            /* 0xEA97 */
          return 0x9F6C;
        case 0x98:            /* 0xEA98 */
          return 0x9F6A;
        case 0x99:            /* 0xEA99 */
          return 0x9F77;
        case 0x9A:            /* 0xEA9A */
          return 0x9F72;
        case 0x9B:            /* 0xEA9B */
          return 0x9F76;
        case 0x9C:            /* 0xEA9C */
          return 0x9F95;
        case 0x9D:            /* 0xEA9D */
          return 0x9F9C;
        case 0x9E:            /* 0xEA9E */
          return 0x9FA0;
        case 0x9F:            /* 0xEA9F */
          return 0x582F;
        case 0xA0:            /* 0xEAA0 */
          return 0x69C7;
        case 0xA1:            /* 0xEAA1 */
          return 0x9059;
        case 0xA2:            /* 0xEAA2 */
          return 0x7464;
        case 0xA3:            /* 0xEAA3 */
          return 0x51DC;
        case 0xA4:            /* 0xEAA4 */
          return 0x7199;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead EA */

    /* Lead Byte: 0xEDxx */
    case 0xED:
      switch (low_byte)
      {
        case 0x40:            /* 0xED40 */
          return 0x7E8A;
        case 0x41:            /* 0xED41 */
          return 0x891C;
        case 0x42:            /* 0xED42 */
          return 0x9348;
        case 0x43:            /* 0xED43 */
          return 0x9288;
        case 0x44:            /* 0xED44 */
          return 0x84DC;
        case 0x45:            /* 0xED45 */
          return 0x4FC9;
        case 0x46:            /* 0xED46 */
          return 0x70BB;
        case 0x47:            /* 0xED47 */
          return 0x6631;
        case 0x48:            /* 0xED48 */
          return 0x68C8;
        case 0x49:            /* 0xED49 */
          return 0x92F9;
        case 0x4A:            /* 0xED4A */
          return 0x66FB;
        case 0x4B:            /* 0xED4B */
          return 0x5F45;
        case 0x4C:            /* 0xED4C */
          return 0x4E28;
        case 0x4D:            /* 0xED4D */
          return 0x4EE1;
        case 0x4E:            /* 0xED4E */
          return 0x4EFC;
        case 0x4F:            /* 0xED4F */
          return 0x4F00;
        case 0x50:            /* 0xED50 */
          return 0x4F03;
        case 0x51:            /* 0xED51 */
          return 0x4F39;
        case 0x52:            /* 0xED52 */
          return 0x4F56;
        case 0x53:            /* 0xED53 */
          return 0x4F92;
        case 0x54:            /* 0xED54 */
          return 0x4F8A;
        case 0x55:            /* 0xED55 */
          return 0x4F9A;
        case 0x56:            /* 0xED56 */
          return 0x4F94;
        case 0x57:            /* 0xED57 */
          return 0x4FCD;
        case 0x58:            /* 0xED58 */
          return 0x5040;
        case 0x59:            /* 0xED59 */
          return 0x5022;
        case 0x5A:            /* 0xED5A */
          return 0x4FFF;
        case 0x5B:            /* 0xED5B */
          return 0x501E;
        case 0x5C:            /* 0xED5C */
          return 0x5046;
        case 0x5D:            /* 0xED5D */
          return 0x5070;
        case 0x5E:            /* 0xED5E */
          return 0x5042;
        case 0x5F:            /* 0xED5F */
          return 0x5094;
        case 0x60:            /* 0xED60 */
          return 0x50F4;
        case 0x61:            /* 0xED61 */
          return 0x50D8;
        case 0x62:            /* 0xED62 */
          return 0x514A;
        case 0x63:            /* 0xED63 */
          return 0x5164;
        case 0x64:            /* 0xED64 */
          return 0x519D;
        case 0x65:            /* 0xED65 */
          return 0x51BE;
        case 0x66:            /* 0xED66 */
          return 0x51EC;
        case 0x67:            /* 0xED67 */
          return 0x5215;
        case 0x68:            /* 0xED68 */
          return 0x529C;
        case 0x69:            /* 0xED69 */
          return 0x52A6;
        case 0x6A:            /* 0xED6A */
          return 0x52C0;
        case 0x6B:            /* 0xED6B */
          return 0x52DB;
        case 0x6C:            /* 0xED6C */
          return 0x5300;
        case 0x6D:            /* 0xED6D */
          return 0x5307;
        case 0x6E:            /* 0xED6E */
          return 0x5324;
        case 0x6F:            /* 0xED6F */
          return 0x5372;
        case 0x70:            /* 0xED70 */
          return 0x5393;
        case 0x71:            /* 0xED71 */
          return 0x53B2;
        case 0x72:            /* 0xED72 */
          return 0x53DD;
        case 0x73:            /* 0xED73 */
          return 0xFA0E;
        case 0x74:            /* 0xED74 */
          return 0x549C;
        case 0x75:            /* 0xED75 */
          return 0x548A;
        case 0x76:            /* 0xED76 */
          return 0x54A9;
        case 0x77:            /* 0xED77 */
          return 0x54FF;
        case 0x78:            /* 0xED78 */
          return 0x5586;
        case 0x79:            /* 0xED79 */
          return 0x5759;
        case 0x7A:            /* 0xED7A */
          return 0x5765;
        case 0x7B:            /* 0xED7B */
          return 0x57AC;
        case 0x7C:            /* 0xED7C */
          return 0x57C8;
        case 0x7D:            /* 0xED7D */
          return 0x57C7;
        case 0x7E:            /* 0xED7E */
          return 0xFA0F;
        case 0x80:            /* 0xED80 */
          return 0xFA10;
        case 0x81:            /* 0xED81 */
          return 0x589E;
        case 0x82:            /* 0xED82 */
          return 0x58B2;
        case 0x83:            /* 0xED83 */
          return 0x590B;
        case 0x84:            /* 0xED84 */
          return 0x5953;
        case 0x85:            /* 0xED85 */
          return 0x595B;
        case 0x86:            /* 0xED86 */
          return 0x595D;
        case 0x87:            /* 0xED87 */
          return 0x5963;
        case 0x88:            /* 0xED88 */
          return 0x59A4;
        case 0x89:            /* 0xED89 */
          return 0x59BA;
        case 0x8A:            /* 0xED8A */
          return 0x5B56;
        case 0x8B:            /* 0xED8B */
          return 0x5BC0;
        case 0x8C:            /* 0xED8C */
          return 0x752F;
        case 0x8D:            /* 0xED8D */
          return 0x5BD8;
        case 0x8E:            /* 0xED8E */
          return 0x5BEC;
        case 0x8F:            /* 0xED8F */
          return 0x5C1E;
        case 0x90:            /* 0xED90 */
          return 0x5CA6;
        case 0x91:            /* 0xED91 */
          return 0x5CBA;
        case 0x92:            /* 0xED92 */
          return 0x5CF5;
        case 0x93:            /* 0xED93 */
          return 0x5D27;
        case 0x94:            /* 0xED94 */
          return 0x5D53;
        case 0x95:            /* 0xED95 */
          return 0xFA11;
        case 0x96:            /* 0xED96 */
          return 0x5D42;
        case 0x97:            /* 0xED97 */
          return 0x5D6D;
        case 0x98:            /* 0xED98 */
          return 0x5DB8;
        case 0x99:            /* 0xED99 */
          return 0x5DB9;
        case 0x9A:            /* 0xED9A */
          return 0x5DD0;
        case 0x9B:            /* 0xED9B */
          return 0x5F21;
        case 0x9C:            /* 0xED9C */
          return 0x5F34;
        case 0x9D:            /* 0xED9D */
          return 0x5F67;
        case 0x9E:            /* 0xED9E */
          return 0x5FB7;
        case 0x9F:            /* 0xED9F */
          return 0x5FDE;
        case 0xA0:            /* 0xEDA0 */
          return 0x605D;
        case 0xA1:            /* 0xEDA1 */
          return 0x6085;
        case 0xA2:            /* 0xEDA2 */
          return 0x608A;
        case 0xA3:            /* 0xEDA3 */
          return 0x60DE;
        case 0xA4:            /* 0xEDA4 */
          return 0x60D5;
        case 0xA5:            /* 0xEDA5 */
          return 0x6120;
        case 0xA6:            /* 0xEDA6 */
          return 0x60F2;
        case 0xA7:            /* 0xEDA7 */
          return 0x6111;
        case 0xA8:            /* 0xEDA8 */
          return 0x6137;
        case 0xA9:            /* 0xEDA9 */
          return 0x6130;
        case 0xAA:            /* 0xEDAA */
          return 0x6198;
        case 0xAB:            /* 0xEDAB */
          return 0x6213;
        case 0xAC:            /* 0xEDAC */
          return 0x62A6;
        case 0xAD:            /* 0xEDAD */
          return 0x63F5;
        case 0xAE:            /* 0xEDAE */
          return 0x6460;
        case 0xAF:            /* 0xEDAF */
          return 0x649D;
        case 0xB0:            /* 0xEDB0 */
          return 0x64CE;
        case 0xB1:            /* 0xEDB1 */
          return 0x654E;
        case 0xB2:            /* 0xEDB2 */
          return 0x6600;
        case 0xB3:            /* 0xEDB3 */
          return 0x6615;
        case 0xB4:            /* 0xEDB4 */
          return 0x663B;
        case 0xB5:            /* 0xEDB5 */
          return 0x6609;
        case 0xB6:            /* 0xEDB6 */
          return 0x662E;
        case 0xB7:            /* 0xEDB7 */
          return 0x661E;
        case 0xB8:            /* 0xEDB8 */
          return 0x6624;
        case 0xB9:            /* 0xEDB9 */
          return 0x6665;
        case 0xBA:            /* 0xEDBA */
          return 0x6657;
        case 0xBB:            /* 0xEDBB */
          return 0x6659;
        case 0xBC:            /* 0xEDBC */
          return 0xFA12;
        case 0xBD:            /* 0xEDBD */
          return 0x6673;
        case 0xBE:            /* 0xEDBE */
          return 0x6699;
        case 0xBF:            /* 0xEDBF */
          return 0x66A0;
        case 0xC0:            /* 0xEDC0 */
          return 0x66B2;
        case 0xC1:            /* 0xEDC1 */
          return 0x66BF;
        case 0xC2:            /* 0xEDC2 */
          return 0x66FA;
        case 0xC3:            /* 0xEDC3 */
          return 0x670E;
        case 0xC4:            /* 0xEDC4 */
          return 0xF929;
        case 0xC5:            /* 0xEDC5 */
          return 0x6766;
        case 0xC6:            /* 0xEDC6 */
          return 0x67BB;
        case 0xC7:            /* 0xEDC7 */
          return 0x6852;
        case 0xC8:            /* 0xEDC8 */
          return 0x67C0;
        case 0xC9:            /* 0xEDC9 */
          return 0x6801;
        case 0xCA:            /* 0xEDCA */
          return 0x6844;
        case 0xCB:            /* 0xEDCB */
          return 0x68CF;
        case 0xCC:            /* 0xEDCC */
          return 0xFA13;
        case 0xCD:            /* 0xEDCD */
          return 0x6968;
        case 0xCE:            /* 0xEDCE */
          return 0xFA14;
        case 0xCF:            /* 0xEDCF */
          return 0x6998;
        case 0xD0:            /* 0xEDD0 */
          return 0x69E2;
        case 0xD1:            /* 0xEDD1 */
          return 0x6A30;
        case 0xD2:            /* 0xEDD2 */
          return 0x6A6B;
        case 0xD3:            /* 0xEDD3 */
          return 0x6A46;
        case 0xD4:            /* 0xEDD4 */
          return 0x6A73;
        case 0xD5:            /* 0xEDD5 */
          return 0x6A7E;
        case 0xD6:            /* 0xEDD6 */
          return 0x6AE2;
        case 0xD7:            /* 0xEDD7 */
          return 0x6AE4;
        case 0xD8:            /* 0xEDD8 */
          return 0x6BD6;
        case 0xD9:            /* 0xEDD9 */
          return 0x6C3F;
        case 0xDA:            /* 0xEDDA */
          return 0x6C5C;
        case 0xDB:            /* 0xEDDB */
          return 0x6C86;
        case 0xDC:            /* 0xEDDC */
          return 0x6C6F;
        case 0xDD:            /* 0xEDDD */
          return 0x6CDA;
        case 0xDE:            /* 0xEDDE */
          return 0x6D04;
        case 0xDF:            /* 0xEDDF */
          return 0x6D87;
        case 0xE0:            /* 0xEDE0 */
          return 0x6D6F;
        case 0xE1:            /* 0xEDE1 */
          return 0x6D96;
        case 0xE2:            /* 0xEDE2 */
          return 0x6DAC;
        case 0xE3:            /* 0xEDE3 */
          return 0x6DCF;
        case 0xE4:            /* 0xEDE4 */
          return 0x6DF8;
        case 0xE5:            /* 0xEDE5 */
          return 0x6DF2;
        case 0xE6:            /* 0xEDE6 */
          return 0x6DFC;
        case 0xE7:            /* 0xEDE7 */
          return 0x6E39;
        case 0xE8:            /* 0xEDE8 */
          return 0x6E5C;
        case 0xE9:            /* 0xEDE9 */
          return 0x6E27;
        case 0xEA:            /* 0xEDEA */
          return 0x6E3C;
        case 0xEB:            /* 0xEDEB */
          return 0x6EBF;
        case 0xEC:            /* 0xEDEC */
          return 0x6F88;
        case 0xED:            /* 0xEDED */
          return 0x6FB5;
        case 0xEE:            /* 0xEDEE */
          return 0x6FF5;
        case 0xEF:            /* 0xEDEF */
          return 0x7005;
        case 0xF0:            /* 0xEDF0 */
          return 0x7007;
        case 0xF1:            /* 0xEDF1 */
          return 0x7028;
        case 0xF2:            /* 0xEDF2 */
          return 0x7085;
        case 0xF3:            /* 0xEDF3 */
          return 0x70AB;
        case 0xF4:            /* 0xEDF4 */
          return 0x710F;
        case 0xF5:            /* 0xEDF5 */
          return 0x7104;
        case 0xF6:            /* 0xEDF6 */
          return 0x715C;
        case 0xF7:            /* 0xEDF7 */
          return 0x7146;
        case 0xF8:            /* 0xEDF8 */
          return 0x7147;
        case 0xF9:            /* 0xEDF9 */
          return 0xFA15;
        case 0xFA:            /* 0xEDFA */
          return 0x71C1;
        case 0xFB:            /* 0xEDFB */
          return 0x71FE;
        case 0xFC:            /* 0xEDFC */
          return 0x72B1;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead ED */

    /* Lead Byte: 0xEExx */
    case 0xEE:
      switch (low_byte)
      {
        case 0x40:            /* 0xEE40 */
          return 0x72BE;
        case 0x41:            /* 0xEE41 */
          return 0x7324;
        case 0x42:            /* 0xEE42 */
          return 0xFA16;
        case 0x43:            /* 0xEE43 */
          return 0x7377;
        case 0x44:            /* 0xEE44 */
          return 0x73BD;
        case 0x45:            /* 0xEE45 */
          return 0x73C9;
        case 0x46:            /* 0xEE46 */
          return 0x73D6;
        case 0x47:            /* 0xEE47 */
          return 0x73E3;
        case 0x48:            /* 0xEE48 */
          return 0x73D2;
        case 0x49:            /* 0xEE49 */
          return 0x7407;
        case 0x4A:            /* 0xEE4A */
          return 0x73F5;
        case 0x4B:            /* 0xEE4B */
          return 0x7426;
        case 0x4C:            /* 0xEE4C */
          return 0x742A;
        case 0x4D:            /* 0xEE4D */
          return 0x7429;
        case 0x4E:            /* 0xEE4E */
          return 0x742E;
        case 0x4F:            /* 0xEE4F */
          return 0x7462;
        case 0x50:            /* 0xEE50 */
          return 0x7489;
        case 0x51:            /* 0xEE51 */
          return 0x749F;
        case 0x52:            /* 0xEE52 */
          return 0x7501;
        case 0x53:            /* 0xEE53 */
          return 0x756F;
        case 0x54:            /* 0xEE54 */
          return 0x7682;
        case 0x55:            /* 0xEE55 */
          return 0x769C;
        case 0x56:            /* 0xEE56 */
          return 0x769E;
        case 0x57:            /* 0xEE57 */
          return 0x769B;
        case 0x58:            /* 0xEE58 */
          return 0x76A6;
        case 0x59:            /* 0xEE59 */
          return 0xFA17;
        case 0x5A:            /* 0xEE5A */
          return 0x7746;
        case 0x5B:            /* 0xEE5B */
          return 0x52AF;
        case 0x5C:            /* 0xEE5C */
          return 0x7821;
        case 0x5D:            /* 0xEE5D */
          return 0x784E;
        case 0x5E:            /* 0xEE5E */
          return 0x7864;
        case 0x5F:            /* 0xEE5F */
          return 0x787A;
        case 0x60:            /* 0xEE60 */
          return 0x7930;
        case 0x61:            /* 0xEE61 */
          return 0xFA18;
        case 0x62:            /* 0xEE62 */
          return 0xFA19;
        case 0x63:            /* 0xEE63 */
          return 0xFA1A;
        case 0x64:            /* 0xEE64 */
          return 0x7994;
        case 0x65:            /* 0xEE65 */
          return 0xFA1B;
        case 0x66:            /* 0xEE66 */
          return 0x799B;
        case 0x67:            /* 0xEE67 */
          return 0x7AD1;
        case 0x68:            /* 0xEE68 */
          return 0x7AE7;
        case 0x69:            /* 0xEE69 */
          return 0xFA1C;
        case 0x6A:            /* 0xEE6A */
          return 0x7AEB;
        case 0x6B:            /* 0xEE6B */
          return 0x7B9E;
        case 0x6C:            /* 0xEE6C */
          return 0xFA1D;
        case 0x6D:            /* 0xEE6D */
          return 0x7D48;
        case 0x6E:            /* 0xEE6E */
          return 0x7D5C;
        case 0x6F:            /* 0xEE6F */
          return 0x7DB7;
        case 0x70:            /* 0xEE70 */
          return 0x7DA0;
        case 0x71:            /* 0xEE71 */
          return 0x7DD6;
        case 0x72:            /* 0xEE72 */
          return 0x7E52;
        case 0x73:            /* 0xEE73 */
          return 0x7F47;
        case 0x74:            /* 0xEE74 */
          return 0x7FA1;
        case 0x75:            /* 0xEE75 */
          return 0xFA1E;
        case 0x76:            /* 0xEE76 */
          return 0x8301;
        case 0x77:            /* 0xEE77 */
          return 0x8362;
        case 0x78:            /* 0xEE78 */
          return 0x837F;
        case 0x79:            /* 0xEE79 */
          return 0x83C7;
        case 0x7A:            /* 0xEE7A */
          return 0x83F6;
        case 0x7B:            /* 0xEE7B */
          return 0x8448;
        case 0x7C:            /* 0xEE7C */
          return 0x84B4;
        case 0x7D:            /* 0xEE7D */
          return 0x8553;
        case 0x7E:            /* 0xEE7E */
          return 0x8559;
        case 0x80:            /* 0xEE80 */
          return 0x856B;
        case 0x81:            /* 0xEE81 */
          return 0xFA1F;
        case 0x82:            /* 0xEE82 */
          return 0x85B0;
        case 0x83:            /* 0xEE83 */
          return 0xFA20;
        case 0x84:            /* 0xEE84 */
          return 0xFA21;
        case 0x85:            /* 0xEE85 */
          return 0x8807;
        case 0x86:            /* 0xEE86 */
          return 0x88F5;
        case 0x87:            /* 0xEE87 */
          return 0x8A12;
        case 0x88:            /* 0xEE88 */
          return 0x8A37;
        case 0x89:            /* 0xEE89 */
          return 0x8A79;
        case 0x8A:            /* 0xEE8A */
          return 0x8AA7;
        case 0x8B:            /* 0xEE8B */
          return 0x8ABE;
        case 0x8C:            /* 0xEE8C */
          return 0x8ADF;
        case 0x8D:            /* 0xEE8D */
          return 0xFA22;
        case 0x8E:            /* 0xEE8E */
          return 0x8AF6;
        case 0x8F:            /* 0xEE8F */
          return 0x8B53;
        case 0x90:            /* 0xEE90 */
          return 0x8B7F;
        case 0x91:            /* 0xEE91 */
          return 0x8CF0;
        case 0x92:            /* 0xEE92 */
          return 0x8CF4;
        case 0x93:            /* 0xEE93 */
          return 0x8D12;
        case 0x94:            /* 0xEE94 */
          return 0x8D76;
        case 0x95:            /* 0xEE95 */
          return 0xFA23;
        case 0x96:            /* 0xEE96 */
          return 0x8ECF;
        case 0x97:            /* 0xEE97 */
          return 0xFA24;
        case 0x98:            /* 0xEE98 */
          return 0xFA25;
        case 0x99:            /* 0xEE99 */
          return 0x9067;
        case 0x9A:            /* 0xEE9A */
          return 0x90DE;
        case 0x9B:            /* 0xEE9B */
          return 0xFA26;
        case 0x9C:            /* 0xEE9C */
          return 0x9115;
        case 0x9D:            /* 0xEE9D */
          return 0x9127;
        case 0x9E:            /* 0xEE9E */
          return 0x91DA;
        case 0x9F:            /* 0xEE9F */
          return 0x91D7;
        case 0xA0:            /* 0xEEA0 */
          return 0x91DE;
        case 0xA1:            /* 0xEEA1 */
          return 0x91ED;
        case 0xA2:            /* 0xEEA2 */
          return 0x91EE;
        case 0xA3:            /* 0xEEA3 */
          return 0x91E4;
        case 0xA4:            /* 0xEEA4 */
          return 0x91E5;
        case 0xA5:            /* 0xEEA5 */
          return 0x9206;
        case 0xA6:            /* 0xEEA6 */
          return 0x9210;
        case 0xA7:            /* 0xEEA7 */
          return 0x920A;
        case 0xA8:            /* 0xEEA8 */
          return 0x923A;
        case 0xA9:            /* 0xEEA9 */
          return 0x9240;
        case 0xAA:            /* 0xEEAA */
          return 0x923C;
        case 0xAB:            /* 0xEEAB */
          return 0x924E;
        case 0xAC:            /* 0xEEAC */
          return 0x9259;
        case 0xAD:            /* 0xEEAD */
          return 0x9251;
        case 0xAE:            /* 0xEEAE */
          return 0x9239;
        case 0xAF:            /* 0xEEAF */
          return 0x9267;
        case 0xB0:            /* 0xEEB0 */
          return 0x92A7;
        case 0xB1:            /* 0xEEB1 */
          return 0x9277;
        case 0xB2:            /* 0xEEB2 */
          return 0x9278;
        case 0xB3:            /* 0xEEB3 */
          return 0x92E7;
        case 0xB4:            /* 0xEEB4 */
          return 0x92D7;
        case 0xB5:            /* 0xEEB5 */
          return 0x92D9;
        case 0xB6:            /* 0xEEB6 */
          return 0x92D0;
        case 0xB7:            /* 0xEEB7 */
          return 0xFA27;
        case 0xB8:            /* 0xEEB8 */
          return 0x92D5;
        case 0xB9:            /* 0xEEB9 */
          return 0x92E0;
        case 0xBA:            /* 0xEEBA */
          return 0x92D3;
        case 0xBB:            /* 0xEEBB */
          return 0x9325;
        case 0xBC:            /* 0xEEBC */
          return 0x9321;
        case 0xBD:            /* 0xEEBD */
          return 0x92FB;
        case 0xBE:            /* 0xEEBE */
          return 0xFA28;
        case 0xBF:            /* 0xEEBF */
          return 0x931E;
        case 0xC0:            /* 0xEEC0 */
          return 0x92FF;
        case 0xC1:            /* 0xEEC1 */
          return 0x931D;
        case 0xC2:            /* 0xEEC2 */
          return 0x9302;
        case 0xC3:            /* 0xEEC3 */
          return 0x9370;
        case 0xC4:            /* 0xEEC4 */
          return 0x9357;
        case 0xC5:            /* 0xEEC5 */
          return 0x93A4;
        case 0xC6:            /* 0xEEC6 */
          return 0x93C6;
        case 0xC7:            /* 0xEEC7 */
          return 0x93DE;
        case 0xC8:            /* 0xEEC8 */
          return 0x93F8;
        case 0xC9:            /* 0xEEC9 */
          return 0x9431;
        case 0xCA:            /* 0xEECA */
          return 0x9445;
        case 0xCB:            /* 0xEECB */
          return 0x9448;
        case 0xCC:            /* 0xEECC */
          return 0x9592;
        case 0xCD:            /* 0xEECD */
          return 0xF9DC;
        case 0xCE:            /* 0xEECE */
          return 0xFA29;
        case 0xCF:            /* 0xEECF */
          return 0x969D;
        case 0xD0:            /* 0xEED0 */
          return 0x96AF;
        case 0xD1:            /* 0xEED1 */
          return 0x9733;
        case 0xD2:            /* 0xEED2 */
          return 0x973B;
        case 0xD3:            /* 0xEED3 */
          return 0x9743;
        case 0xD4:            /* 0xEED4 */
          return 0x974D;
        case 0xD5:            /* 0xEED5 */
          return 0x974F;
        case 0xD6:            /* 0xEED6 */
          return 0x9751;
        case 0xD7:            /* 0xEED7 */
          return 0x9755;
        case 0xD8:            /* 0xEED8 */
          return 0x9857;
        case 0xD9:            /* 0xEED9 */
          return 0x9865;
        case 0xDA:            /* 0xEEDA */
          return 0xFA2A;
        case 0xDB:            /* 0xEEDB */
          return 0xFA2B;
        case 0xDC:            /* 0xEEDC */
          return 0x9927;
        case 0xDD:            /* 0xEEDD */
          return 0xFA2C;
        case 0xDE:            /* 0xEEDE */
          return 0x999E;
        case 0xDF:            /* 0xEEDF */
          return 0x9A4E;
        case 0xE0:            /* 0xEEE0 */
          return 0x9AD9;
        case 0xE1:            /* 0xEEE1 */
          return 0x9ADC;
        case 0xE2:            /* 0xEEE2 */
          return 0x9B75;
        case 0xE3:            /* 0xEEE3 */
          return 0x9B72;
        case 0xE4:            /* 0xEEE4 */
          return 0x9B8F;
        case 0xE5:            /* 0xEEE5 */
          return 0x9BB1;
        case 0xE6:            /* 0xEEE6 */
          return 0x9BBB;
        case 0xE7:            /* 0xEEE7 */
          return 0x9C00;
        case 0xE8:            /* 0xEEE8 */
          return 0x9D70;
        case 0xE9:            /* 0xEEE9 */
          return 0x9D6B;
        case 0xEA:            /* 0xEEEA */
          return 0xFA2D;
        case 0xEB:            /* 0xEEEB */
          return 0x9E19;
        case 0xEC:            /* 0xEEEC */
          return 0x9ED1;
        case 0xEF:            /* 0xEEEF */
          return 0x2170;
        case 0xF0:            /* 0xEEF0 */
          return 0x2171;
        case 0xF1:            /* 0xEEF1 */
          return 0x2172;
        case 0xF2:            /* 0xEEF2 */
          return 0x2173;
        case 0xF3:            /* 0xEEF3 */
          return 0x2174;
        case 0xF4:            /* 0xEEF4 */
          return 0x2175;
        case 0xF5:            /* 0xEEF5 */
          return 0x2176;
        case 0xF6:            /* 0xEEF6 */
          return 0x2177;
        case 0xF7:            /* 0xEEF7 */
          return 0x2178;
        case 0xF8:            /* 0xEEF8 */
          return 0x2179;
        case 0xF9:            /* 0xEEF9 */
          return 0xFFE2;
        case 0xFA:            /* 0xEEFA */
          return 0xFFE4;
        case 0xFB:            /* 0xEEFB */
          return 0xFF07;
        case 0xFC:            /* 0xEEFC */
          return 0xFF02;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead EE */

    /* Lead Byte: 0xFAxx */
    case 0xFA:
      switch (low_byte)
      {
        case 0x40:            /* 0xFA40 */
          return 0x2170;
        case 0x41:            /* 0xFA41 */
          return 0x2171;
        case 0x42:            /* 0xFA42 */
          return 0x2172;
        case 0x43:            /* 0xFA43 */
          return 0x2173;
        case 0x44:            /* 0xFA44 */
          return 0x2174;
        case 0x45:            /* 0xFA45 */
          return 0x2175;
        case 0x46:            /* 0xFA46 */
          return 0x2176;
        case 0x47:            /* 0xFA47 */
          return 0x2177;
        case 0x48:            /* 0xFA48 */
          return 0x2178;
        case 0x49:            /* 0xFA49 */
          return 0x2179;
        case 0x4A:            /* 0xFA4A */
          return 0x2160;
        case 0x4B:            /* 0xFA4B */
          return 0x2161;
        case 0x4C:            /* 0xFA4C */
          return 0x2162;
        case 0x4D:            /* 0xFA4D */
          return 0x2163;
        case 0x4E:            /* 0xFA4E */
          return 0x2164;
        case 0x4F:            /* 0xFA4F */
          return 0x2165;
        case 0x50:            /* 0xFA50 */
          return 0x2166;
        case 0x51:            /* 0xFA51 */
          return 0x2167;
        case 0x52:            /* 0xFA52 */
          return 0x2168;
        case 0x53:            /* 0xFA53 */
          return 0x2169;
        case 0x54:            /* 0xFA54 */
          return 0xFFE2;
        case 0x55:            /* 0xFA55 */
          return 0xFFE4;
        case 0x56:            /* 0xFA56 */
          return 0xFF07;
        case 0x57:            /* 0xFA57 */
          return 0xFF02;
        case 0x58:            /* 0xFA58 */
          return 0x3231;
        case 0x59:            /* 0xFA59 */
          return 0x2116;
        case 0x5A:            /* 0xFA5A */
          return 0x2121;
        case 0x5B:            /* 0xFA5B */
          return 0x2235;
        case 0x5C:            /* 0xFA5C */
          return 0x7E8A;
        case 0x5D:            /* 0xFA5D */
          return 0x891C;
        case 0x5E:            /* 0xFA5E */
          return 0x9348;
        case 0x5F:            /* 0xFA5F */
          return 0x9288;
        case 0x60:            /* 0xFA60 */
          return 0x84DC;
        case 0x61:            /* 0xFA61 */
          return 0x4FC9;
        case 0x62:            /* 0xFA62 */
          return 0x70BB;
        case 0x63:            /* 0xFA63 */
          return 0x6631;
        case 0x64:            /* 0xFA64 */
          return 0x68C8;
        case 0x65:            /* 0xFA65 */
          return 0x92F9;
        case 0x66:            /* 0xFA66 */
          return 0x66FB;
        case 0x67:            /* 0xFA67 */
          return 0x5F45;
        case 0x68:            /* 0xFA68 */
          return 0x4E28;
        case 0x69:            /* 0xFA69 */
          return 0x4EE1;
        case 0x6A:            /* 0xFA6A */
          return 0x4EFC;
        case 0x6B:            /* 0xFA6B */
          return 0x4F00;
        case 0x6C:            /* 0xFA6C */
          return 0x4F03;
        case 0x6D:            /* 0xFA6D */
          return 0x4F39;
        case 0x6E:            /* 0xFA6E */
          return 0x4F56;
        case 0x6F:            /* 0xFA6F */
          return 0x4F92;
        case 0x70:            /* 0xFA70 */
          return 0x4F8A;
        case 0x71:            /* 0xFA71 */
          return 0x4F9A;
        case 0x72:            /* 0xFA72 */
          return 0x4F94;
        case 0x73:            /* 0xFA73 */
          return 0x4FCD;
        case 0x74:            /* 0xFA74 */
          return 0x5040;
        case 0x75:            /* 0xFA75 */
          return 0x5022;
        case 0x76:            /* 0xFA76 */
          return 0x4FFF;
        case 0x77:            /* 0xFA77 */
          return 0x501E;
        case 0x78:            /* 0xFA78 */
          return 0x5046;
        case 0x79:            /* 0xFA79 */
          return 0x5070;
        case 0x7A:            /* 0xFA7A */
          return 0x5042;
        case 0x7B:            /* 0xFA7B */
          return 0x5094;
        case 0x7C:            /* 0xFA7C */
          return 0x50F4;
        case 0x7D:            /* 0xFA7D */
          return 0x50D8;
        case 0x7E:            /* 0xFA7E */
          return 0x514A;
        case 0x80:            /* 0xFA80 */
          return 0x5164;
        case 0x81:            /* 0xFA81 */
          return 0x519D;
        case 0x82:            /* 0xFA82 */
          return 0x51BE;
        case 0x83:            /* 0xFA83 */
          return 0x51EC;
        case 0x84:            /* 0xFA84 */
          return 0x5215;
        case 0x85:            /* 0xFA85 */
          return 0x529C;
        case 0x86:            /* 0xFA86 */
          return 0x52A6;
        case 0x87:            /* 0xFA87 */
          return 0x52C0;
        case 0x88:            /* 0xFA88 */
          return 0x52DB;
        case 0x89:            /* 0xFA89 */
          return 0x5300;
        case 0x8A:            /* 0xFA8A */
          return 0x5307;
        case 0x8B:            /* 0xFA8B */
          return 0x5324;
        case 0x8C:            /* 0xFA8C */
          return 0x5372;
        case 0x8D:            /* 0xFA8D */
          return 0x5393;
        case 0x8E:            /* 0xFA8E */
          return 0x53B2;
        case 0x8F:            /* 0xFA8F */
          return 0x53DD;
        case 0x90:            /* 0xFA90 */
          return 0xFA0E;
        case 0x91:            /* 0xFA91 */
          return 0x549C;
        case 0x92:            /* 0xFA92 */
          return 0x548A;
        case 0x93:            /* 0xFA93 */
          return 0x54A9;
        case 0x94:            /* 0xFA94 */
          return 0x54FF;
        case 0x95:            /* 0xFA95 */
          return 0x5586;
        case 0x96:            /* 0xFA96 */
          return 0x5759;
        case 0x97:            /* 0xFA97 */
          return 0x5765;
        case 0x98:            /* 0xFA98 */
          return 0x57AC;
        case 0x99:            /* 0xFA99 */
          return 0x57C8;
        case 0x9A:            /* 0xFA9A */
          return 0x57C7;
        case 0x9B:            /* 0xFA9B */
          return 0xFA0F;
        case 0x9C:            /* 0xFA9C */
          return 0xFA10;
        case 0x9D:            /* 0xFA9D */
          return 0x589E;
        case 0x9E:            /* 0xFA9E */
          return 0x58B2;
        case 0x9F:            /* 0xFA9F */
          return 0x590B;
        case 0xA0:            /* 0xFAA0 */
          return 0x5953;
        case 0xA1:            /* 0xFAA1 */
          return 0x595B;
        case 0xA2:            /* 0xFAA2 */
          return 0x595D;
        case 0xA3:            /* 0xFAA3 */
          return 0x5963;
        case 0xA4:            /* 0xFAA4 */
          return 0x59A4;
        case 0xA5:            /* 0xFAA5 */
          return 0x59BA;
        case 0xA6:            /* 0xFAA6 */
          return 0x5B56;
        case 0xA7:            /* 0xFAA7 */
          return 0x5BC0;
        case 0xA8:            /* 0xFAA8 */
          return 0x752F;
        case 0xA9:            /* 0xFAA9 */
          return 0x5BD8;
        case 0xAA:            /* 0xFAAA */
          return 0x5BEC;
        case 0xAB:            /* 0xFAAB */
          return 0x5C1E;
        case 0xAC:            /* 0xFAAC */
          return 0x5CA6;
        case 0xAD:            /* 0xFAAD */
          return 0x5CBA;
        case 0xAE:            /* 0xFAAE */
          return 0x5CF5;
        case 0xAF:            /* 0xFAAF */
          return 0x5D27;
        case 0xB0:            /* 0xFAB0 */
          return 0x5D53;
        case 0xB1:            /* 0xFAB1 */
          return 0xFA11;
        case 0xB2:            /* 0xFAB2 */
          return 0x5D42;
        case 0xB3:            /* 0xFAB3 */
          return 0x5D6D;
        case 0xB4:            /* 0xFAB4 */
          return 0x5DB8;
        case 0xB5:            /* 0xFAB5 */
          return 0x5DB9;
        case 0xB6:            /* 0xFAB6 */
          return 0x5DD0;
        case 0xB7:            /* 0xFAB7 */
          return 0x5F21;
        case 0xB8:            /* 0xFAB8 */
          return 0x5F34;
        case 0xB9:            /* 0xFAB9 */
          return 0x5F67;
        case 0xBA:            /* 0xFABA */
          return 0x5FB7;
        case 0xBB:            /* 0xFABB */
          return 0x5FDE;
        case 0xBC:            /* 0xFABC */
          return 0x605D;
        case 0xBD:            /* 0xFABD */
          return 0x6085;
        case 0xBE:            /* 0xFABE */
          return 0x608A;
        case 0xBF:            /* 0xFABF */
          return 0x60DE;
        case 0xC0:            /* 0xFAC0 */
          return 0x60D5;
        case 0xC1:            /* 0xFAC1 */
          return 0x6120;
        case 0xC2:            /* 0xFAC2 */
          return 0x60F2;
        case 0xC3:            /* 0xFAC3 */
          return 0x6111;
        case 0xC4:            /* 0xFAC4 */
          return 0x6137;
        case 0xC5:            /* 0xFAC5 */
          return 0x6130;
        case 0xC6:            /* 0xFAC6 */
          return 0x6198;
        case 0xC7:            /* 0xFAC7 */
          return 0x6213;
        case 0xC8:            /* 0xFAC8 */
          return 0x62A6;
        case 0xC9:            /* 0xFAC9 */
          return 0x63F5;
        case 0xCA:            /* 0xFACA */
          return 0x6460;
        case 0xCB:            /* 0xFACB */
          return 0x649D;
        case 0xCC:            /* 0xFACC */
          return 0x64CE;
        case 0xCD:            /* 0xFACD */
          return 0x654E;
        case 0xCE:            /* 0xFACE */
          return 0x6600;
        case 0xCF:            /* 0xFACF */
          return 0x6615;
        case 0xD0:            /* 0xFAD0 */
          return 0x663B;
        case 0xD1:            /* 0xFAD1 */
          return 0x6609;
        case 0xD2:            /* 0xFAD2 */
          return 0x662E;
        case 0xD3:            /* 0xFAD3 */
          return 0x661E;
        case 0xD4:            /* 0xFAD4 */
          return 0x6624;
        case 0xD5:            /* 0xFAD5 */
          return 0x6665;
        case 0xD6:            /* 0xFAD6 */
          return 0x6657;
        case 0xD7:            /* 0xFAD7 */
          return 0x6659;
        case 0xD8:            /* 0xFAD8 */
          return 0xFA12;
        case 0xD9:            /* 0xFAD9 */
          return 0x6673;
        case 0xDA:            /* 0xFADA */
          return 0x6699;
        case 0xDB:            /* 0xFADB */
          return 0x66A0;
        case 0xDC:            /* 0xFADC */
          return 0x66B2;
        case 0xDD:            /* 0xFADD */
          return 0x66BF;
        case 0xDE:            /* 0xFADE */
          return 0x66FA;
        case 0xDF:            /* 0xFADF */
          return 0x670E;
        case 0xE0:            /* 0xFAE0 */
          return 0xF929;
        case 0xE1:            /* 0xFAE1 */
          return 0x6766;
        case 0xE2:            /* 0xFAE2 */
          return 0x67BB;
        case 0xE3:            /* 0xFAE3 */
          return 0x6852;
        case 0xE4:            /* 0xFAE4 */
          return 0x67C0;
        case 0xE5:            /* 0xFAE5 */
          return 0x6801;
        case 0xE6:            /* 0xFAE6 */
          return 0x6844;
        case 0xE7:            /* 0xFAE7 */
          return 0x68CF;
        case 0xE8:            /* 0xFAE8 */
          return 0xFA13;
        case 0xE9:            /* 0xFAE9 */
          return 0x6968;
        case 0xEA:            /* 0xFAEA */
          return 0xFA14;
        case 0xEB:            /* 0xFAEB */
          return 0x6998;
        case 0xEC:            /* 0xFAEC */
          return 0x69E2;
        case 0xED:            /* 0xFAED */
          return 0x6A30;
        case 0xEE:            /* 0xFAEE */
          return 0x6A6B;
        case 0xEF:            /* 0xFAEF */
          return 0x6A46;
        case 0xF0:            /* 0xFAF0 */
          return 0x6A73;
        case 0xF1:            /* 0xFAF1 */
          return 0x6A7E;
        case 0xF2:            /* 0xFAF2 */
          return 0x6AE2;
        case 0xF3:            /* 0xFAF3 */
          return 0x6AE4;
        case 0xF4:            /* 0xFAF4 */
          return 0x6BD6;
        case 0xF5:            /* 0xFAF5 */
          return 0x6C3F;
        case 0xF6:            /* 0xFAF6 */
          return 0x6C5C;
        case 0xF7:            /* 0xFAF7 */
          return 0x6C86;
        case 0xF8:            /* 0xFAF8 */
          return 0x6C6F;
        case 0xF9:            /* 0xFAF9 */
          return 0x6CDA;
        case 0xFA:            /* 0xFAFA */
          return 0x6D04;
        case 0xFB:            /* 0xFAFB */
          return 0x6D87;
        case 0xFC:            /* 0xFAFC */
          return 0x6D6F;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead FA */

    /* Lead Byte: 0xFBxx */
    case 0xFB:
      switch (low_byte)
      {
        case 0x40:            /* 0xFB40 */
          return 0x6D96;
        case 0x41:            /* 0xFB41 */
          return 0x6DAC;
        case 0x42:            /* 0xFB42 */
          return 0x6DCF;
        case 0x43:            /* 0xFB43 */
          return 0x6DF8;
        case 0x44:            /* 0xFB44 */
          return 0x6DF2;
        case 0x45:            /* 0xFB45 */
          return 0x6DFC;
        case 0x46:            /* 0xFB46 */
          return 0x6E39;
        case 0x47:            /* 0xFB47 */
          return 0x6E5C;
        case 0x48:            /* 0xFB48 */
          return 0x6E27;
        case 0x49:            /* 0xFB49 */
          return 0x6E3C;
        case 0x4A:            /* 0xFB4A */
          return 0x6EBF;
        case 0x4B:            /* 0xFB4B */
          return 0x6F88;
        case 0x4C:            /* 0xFB4C */
          return 0x6FB5;
        case 0x4D:            /* 0xFB4D */
          return 0x6FF5;
        case 0x4E:            /* 0xFB4E */
          return 0x7005;
        case 0x4F:            /* 0xFB4F */
          return 0x7007;
        case 0x50:            /* 0xFB50 */
          return 0x7028;
        case 0x51:            /* 0xFB51 */
          return 0x7085;
        case 0x52:            /* 0xFB52 */
          return 0x70AB;
        case 0x53:            /* 0xFB53 */
          return 0x710F;
        case 0x54:            /* 0xFB54 */
          return 0x7104;
        case 0x55:            /* 0xFB55 */
          return 0x715C;
        case 0x56:            /* 0xFB56 */
          return 0x7146;
        case 0x57:            /* 0xFB57 */
          return 0x7147;
        case 0x58:            /* 0xFB58 */
          return 0xFA15;
        case 0x59:            /* 0xFB59 */
          return 0x71C1;
        case 0x5A:            /* 0xFB5A */
          return 0x71FE;
        case 0x5B:            /* 0xFB5B */
          return 0x72B1;
        case 0x5C:            /* 0xFB5C */
          return 0x72BE;
        case 0x5D:            /* 0xFB5D */
          return 0x7324;
        case 0x5E:            /* 0xFB5E */
          return 0xFA16;
        case 0x5F:            /* 0xFB5F */
          return 0x7377;
        case 0x60:            /* 0xFB60 */
          return 0x73BD;
        case 0x61:            /* 0xFB61 */
          return 0x73C9;
        case 0x62:            /* 0xFB62 */
          return 0x73D6;
        case 0x63:            /* 0xFB63 */
          return 0x73E3;
        case 0x64:            /* 0xFB64 */
          return 0x73D2;
        case 0x65:            /* 0xFB65 */
          return 0x7407;
        case 0x66:            /* 0xFB66 */
          return 0x73F5;
        case 0x67:            /* 0xFB67 */
          return 0x7426;
        case 0x68:            /* 0xFB68 */
          return 0x742A;
        case 0x69:            /* 0xFB69 */
          return 0x7429;
        case 0x6A:            /* 0xFB6A */
          return 0x742E;
        case 0x6B:            /* 0xFB6B */
          return 0x7462;
        case 0x6C:            /* 0xFB6C */
          return 0x7489;
        case 0x6D:            /* 0xFB6D */
          return 0x749F;
        case 0x6E:            /* 0xFB6E */
          return 0x7501;
        case 0x6F:            /* 0xFB6F */
          return 0x756F;
        case 0x70:            /* 0xFB70 */
          return 0x7682;
        case 0x71:            /* 0xFB71 */
          return 0x769C;
        case 0x72:            /* 0xFB72 */
          return 0x769E;
        case 0x73:            /* 0xFB73 */
          return 0x769B;
        case 0x74:            /* 0xFB74 */
          return 0x76A6;
        case 0x75:            /* 0xFB75 */
          return 0xFA17;
        case 0x76:            /* 0xFB76 */
          return 0x7746;
        case 0x77:            /* 0xFB77 */
          return 0x52AF;
        case 0x78:            /* 0xFB78 */
          return 0x7821;
        case 0x79:            /* 0xFB79 */
          return 0x784E;
        case 0x7A:            /* 0xFB7A */
          return 0x7864;
        case 0x7B:            /* 0xFB7B */
          return 0x787A;
        case 0x7C:            /* 0xFB7C */
          return 0x7930;
        case 0x7D:            /* 0xFB7D */
          return 0xFA18;
        case 0x7E:            /* 0xFB7E */
          return 0xFA19;
        case 0x80:            /* 0xFB80 */
          return 0xFA1A;
        case 0x81:            /* 0xFB81 */
          return 0x7994;
        case 0x82:            /* 0xFB82 */
          return 0xFA1B;
        case 0x83:            /* 0xFB83 */
          return 0x799B;
        case 0x84:            /* 0xFB84 */
          return 0x7AD1;
        case 0x85:            /* 0xFB85 */
          return 0x7AE7;
        case 0x86:            /* 0xFB86 */
          return 0xFA1C;
        case 0x87:            /* 0xFB87 */
          return 0x7AEB;
        case 0x88:            /* 0xFB88 */
          return 0x7B9E;
        case 0x89:            /* 0xFB89 */
          return 0xFA1D;
        case 0x8A:            /* 0xFB8A */
          return 0x7D48;
        case 0x8B:            /* 0xFB8B */
          return 0x7D5C;
        case 0x8C:            /* 0xFB8C */
          return 0x7DB7;
        case 0x8D:            /* 0xFB8D */
          return 0x7DA0;
        case 0x8E:            /* 0xFB8E */
          return 0x7DD6;
        case 0x8F:            /* 0xFB8F */
          return 0x7E52;
        case 0x90:            /* 0xFB90 */
          return 0x7F47;
        case 0x91:            /* 0xFB91 */
          return 0x7FA1;
        case 0x92:            /* 0xFB92 */
          return 0xFA1E;
        case 0x93:            /* 0xFB93 */
          return 0x8301;
        case 0x94:            /* 0xFB94 */
          return 0x8362;
        case 0x95:            /* 0xFB95 */
          return 0x837F;
        case 0x96:            /* 0xFB96 */
          return 0x83C7;
        case 0x97:            /* 0xFB97 */
          return 0x83F6;
        case 0x98:            /* 0xFB98 */
          return 0x8448;
        case 0x99:            /* 0xFB99 */
          return 0x84B4;
        case 0x9A:            /* 0xFB9A */
          return 0x8553;
        case 0x9B:            /* 0xFB9B */
          return 0x8559;
        case 0x9C:            /* 0xFB9C */
          return 0x856B;
        case 0x9D:            /* 0xFB9D */
          return 0xFA1F;
        case 0x9E:            /* 0xFB9E */
          return 0x85B0;
        case 0x9F:            /* 0xFB9F */
          return 0xFA20;
        case 0xA0:            /* 0xFBA0 */
          return 0xFA21;
        case 0xA1:            /* 0xFBA1 */
          return 0x8807;
        case 0xA2:            /* 0xFBA2 */
          return 0x88F5;
        case 0xA3:            /* 0xFBA3 */
          return 0x8A12;
        case 0xA4:            /* 0xFBA4 */
          return 0x8A37;
        case 0xA5:            /* 0xFBA5 */
          return 0x8A79;
        case 0xA6:            /* 0xFBA6 */
          return 0x8AA7;
        case 0xA7:            /* 0xFBA7 */
          return 0x8ABE;
        case 0xA8:            /* 0xFBA8 */
          return 0x8ADF;
        case 0xA9:            /* 0xFBA9 */
          return 0xFA22;
        case 0xAA:            /* 0xFBAA */
          return 0x8AF6;
        case 0xAB:            /* 0xFBAB */
          return 0x8B53;
        case 0xAC:            /* 0xFBAC */
          return 0x8B7F;
        case 0xAD:            /* 0xFBAD */
          return 0x8CF0;
        case 0xAE:            /* 0xFBAE */
          return 0x8CF4;
        case 0xAF:            /* 0xFBAF */
          return 0x8D12;
        case 0xB0:            /* 0xFBB0 */
          return 0x8D76;
        case 0xB1:            /* 0xFBB1 */
          return 0xFA23;
        case 0xB2:            /* 0xFBB2 */
          return 0x8ECF;
        case 0xB3:            /* 0xFBB3 */
          return 0xFA24;
        case 0xB4:            /* 0xFBB4 */
          return 0xFA25;
        case 0xB5:            /* 0xFBB5 */
          return 0x9067;
        case 0xB6:            /* 0xFBB6 */
          return 0x90DE;
        case 0xB7:            /* 0xFBB7 */
          return 0xFA26;
        case 0xB8:            /* 0xFBB8 */
          return 0x9115;
        case 0xB9:            /* 0xFBB9 */
          return 0x9127;
        case 0xBA:            /* 0xFBBA */
          return 0x91DA;
        case 0xBB:            /* 0xFBBB */
          return 0x91D7;
        case 0xBC:            /* 0xFBBC */
          return 0x91DE;
        case 0xBD:            /* 0xFBBD */
          return 0x91ED;
        case 0xBE:            /* 0xFBBE */
          return 0x91EE;
        case 0xBF:            /* 0xFBBF */
          return 0x91E4;
        case 0xC0:            /* 0xFBC0 */
          return 0x91E5;
        case 0xC1:            /* 0xFBC1 */
          return 0x9206;
        case 0xC2:            /* 0xFBC2 */
          return 0x9210;
        case 0xC3:            /* 0xFBC3 */
          return 0x920A;
        case 0xC4:            /* 0xFBC4 */
          return 0x923A;
        case 0xC5:            /* 0xFBC5 */
          return 0x9240;
        case 0xC6:            /* 0xFBC6 */
          return 0x923C;
        case 0xC7:            /* 0xFBC7 */
          return 0x924E;
        case 0xC8:            /* 0xFBC8 */
          return 0x9259;
        case 0xC9:            /* 0xFBC9 */
          return 0x9251;
        case 0xCA:            /* 0xFBCA */
          return 0x9239;
        case 0xCB:            /* 0xFBCB */
          return 0x9267;
        case 0xCC:            /* 0xFBCC */
          return 0x92A7;
        case 0xCD:            /* 0xFBCD */
          return 0x9277;
        case 0xCE:            /* 0xFBCE */
          return 0x9278;
        case 0xCF:            /* 0xFBCF */
          return 0x92E7;
        case 0xD0:            /* 0xFBD0 */
          return 0x92D7;
        case 0xD1:            /* 0xFBD1 */
          return 0x92D9;
        case 0xD2:            /* 0xFBD2 */
          return 0x92D0;
        case 0xD3:            /* 0xFBD3 */
          return 0xFA27;
        case 0xD4:            /* 0xFBD4 */
          return 0x92D5;
        case 0xD5:            /* 0xFBD5 */
          return 0x92E0;
        case 0xD6:            /* 0xFBD6 */
          return 0x92D3;
        case 0xD7:            /* 0xFBD7 */
          return 0x9325;
        case 0xD8:            /* 0xFBD8 */
          return 0x9321;
        case 0xD9:            /* 0xFBD9 */
          return 0x92FB;
        case 0xDA:            /* 0xFBDA */
          return 0xFA28;
        case 0xDB:            /* 0xFBDB */
          return 0x931E;
        case 0xDC:            /* 0xFBDC */
          return 0x92FF;
        case 0xDD:            /* 0xFBDD */
          return 0x931D;
        case 0xDE:            /* 0xFBDE */
          return 0x9302;
        case 0xDF:            /* 0xFBDF */
          return 0x9370;
        case 0xE0:            /* 0xFBE0 */
          return 0x9357;
        case 0xE1:            /* 0xFBE1 */
          return 0x93A4;
        case 0xE2:            /* 0xFBE2 */
          return 0x93C6;
        case 0xE3:            /* 0xFBE3 */
          return 0x93DE;
        case 0xE4:            /* 0xFBE4 */
          return 0x93F8;
        case 0xE5:            /* 0xFBE5 */
          return 0x9431;
        case 0xE6:            /* 0xFBE6 */
          return 0x9445;
        case 0xE7:            /* 0xFBE7 */
          return 0x9448;
        case 0xE8:            /* 0xFBE8 */
          return 0x9592;
        case 0xE9:            /* 0xFBE9 */
          return 0xF9DC;
        case 0xEA:            /* 0xFBEA */
          return 0xFA29;
        case 0xEB:            /* 0xFBEB */
          return 0x969D;
        case 0xEC:            /* 0xFBEC */
          return 0x96AF;
        case 0xED:            /* 0xFBED */
          return 0x9733;
        case 0xEE:            /* 0xFBEE */
          return 0x973B;
        case 0xEF:            /* 0xFBEF */
          return 0x9743;
        case 0xF0:            /* 0xFBF0 */
          return 0x974D;
        case 0xF1:            /* 0xFBF1 */
          return 0x974F;
        case 0xF2:            /* 0xFBF2 */
          return 0x9751;
        case 0xF3:            /* 0xFBF3 */
          return 0x9755;
        case 0xF4:            /* 0xFBF4 */
          return 0x9857;
        case 0xF5:            /* 0xFBF5 */
          return 0x9865;
        case 0xF6:            /* 0xFBF6 */
          return 0xFA2A;
        case 0xF7:            /* 0xFBF7 */
          return 0xFA2B;
        case 0xF8:            /* 0xFBF8 */
          return 0x9927;
        case 0xF9:            /* 0xFBF9 */
          return 0xFA2C;
        case 0xFA:            /* 0xFBFA */
          return 0x999E;
        case 0xFB:            /* 0xFBFB */
          return 0x9A4E;
        case 0xFC:            /* 0xFBFC */
          return 0x9AD9;
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead FB */

    /* Lead Byte: 0xFCxx */
    case 0xFC:
      switch (low_byte)
      {
        case 0x40:            /* 0xFC40 */
          return 0x9ADC;
        case 0x41:            /* 0xFC41 */
          return 0x9B75;
        case 0x42:            /* 0xFC42 */
          return 0x9B72;
        case 0x43:            /* 0xFC43 */
          return 0x9B8F;
        case 0x44:            /* 0xFC44 */
          return 0x9BB1;
        case 0x45:            /* 0xFC45 */
          return 0x9BBB;
        case 0x46:            /* 0xFC46 */
          return 0x9C00;
        case 0x47:            /* 0xFC47 */
          return 0x9D70;
        case 0x48:            /* 0xFC48 */
          return 0x9D6B;
        case 0x49:            /* 0xFC49 */
          return 0xFA2D;
        case 0x4A:            /* 0xFC4A */
          return 0x9E19;
        case 0x4B:            /* 0xFC4B */
          return 0x9ED1;
// Auto-generated code END
        default:
          return FS_SJIS_UNKNOWN;
      } /* Lead FB */

    default:               /* High byte unrecognized */
      return FS_SJIS_UNKNOWN;
  } // switch (high_byte)
#endif /* FEATURE_EFS_CONVERT_SHIFT_JIS */

  /* Unreached! */
}

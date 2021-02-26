/*
===========================================================================

FILE:         hal_mdp_source.h

===========================================================================
  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/* -----------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------- */

#include "hal_mdp_i.h"
#include "hal_mdp.h"


#ifdef __cplusplus
extern "C" {
#endif

static  uint32 aDirLUTs[200]=
{
  0X3BF93B36, 0XF4396D80, 0XF2341CFB, 0X1F3E7D00, 0X15FF4E1E, 0XF26E66FF, 0XFA481179, 0X1F3E3E00,
  0X000E5309, 0X00A551FC, 0X125206F8, 0X1F9E3F00, 0X4EF9151E, 0XF8226680, 0XF0483779, 0X1FFF7CFF,
  0X21F82110, 0XF052677F, 0XFA6D2977, 0X1F1F7C7F, 0X06FD2A03, 0XF485587F, 0X1A8016F9, 0X1F3F3D00,
  0X53FC0009, 0XFC0D5180, 0XF05252F8, 0X01C2407C, 0X2AF90603, 0XF42D5800, 0XF88042F9, 0X1FA37D7F,
  0X0BF80BFE, 0XF0595100, 0X16A42CFE, 0X1F02FC00, 0X3B353BF9, 0XF4396C79, 0X01F71C80, 0X06A03D79,
  0X144A24F9, 0XF26D667D, 0XFFFD2580, 0X03C03CF8, 0X00530EFC, 0X00A55189, 0XF80D2900, 0X013FBC78,
  0X241E14F9, 0XF24B6678, 0X01FD3680, 0X095FFCFD, 0X072807FC, 0XF48059FB, 0XFA0F4000, 0X051F7D7B,
  0XFA2AFDFF, 0X0CB24183, 0XF22D4000, 0X01BEBE79, 0X0E0900FC, 0XF05251F8, 0XFC0D5280, 0X0A7F0009,
  0XFD0DFAFF, 0XF88041F9, 0XF42D5900, 0X055E4303, 0XF80BF800, 0X16A42BFE, 0XF0595200, 0X017E05FE,
  0X26FB462A, 0XF2526B80, 0XF43F177A, 0X1F1E7D80, 0X08055213, 0XF68A5E7E, 0X044F0B79, 0X1F5E3E80,
  0X36F81C18, 0XF4376800, 0XF45A2FF9, 0X1F7FBC00, 0X12FA270A, 0XF06A6100, 0X06771F79, 0X1F1F7C80,
  0X3DFB0207, 0XF81B5700, 0XF2694BF8, 0X0062FEFE, 0X19F80900, 0XF2425600, 0X0294387B, 0X1F433C80,
  0X26402FF9, 0XF25269FA, 0X01FA2200, 0X05403CF9, 0X085018FB, 0XF68A5C82, 0XFC042880, 0X027FFC78,
  0X13240DFA, 0XF06660F9, 0XFE053D00, 0X073FBCFC, 0XFF2A01FE, 0XFE9A4EFF, 0XF61C4100, 0X033F3DFA,
  0X030BFDFE, 0XF2694BF8, 0XF81B5700, 0X07BEC107, 0XFA0CF900, 0X0294377B, 0XF2425700, 0X033E0480,
  0X46F8262A, 0XF62E6B80, 0XF23F297A, 0X1F7EBC80, 0X1CFB3618, 0XF05F6800, 0XFC5A1BF9, 0X1F1EBD00,
  0X02033D07, 0XFA97577E, 0X16690DF8, 0X1F7E7E00, 0X52FA0813, 0XFA165E00, 0XF04F4579, 0X00A0BDFE,
  0X27F8120A, 0XF23E6100, 0XFA773579, 0X1F40FC00, 0X09FA1900, 0XF2705600, 0X1894217B, 0X1F007C80,
  0X2F2A26F9, 0XF24469F9, 0X01FA2900, 0X08003CFA, 0X0D3913FA, 0XF27A60FC, 0XFC053300, 0X049FFC79,
  0XFD3D03FE, 0X04AE4B87, 0XF61B3480, 0X017F3CF8, 0X181308FB, 0XF0515CF8, 0XFE044500, 0X0A1FBD82,
  0X0119FFFE, 0XF6824EFA, 0XF81C4D00, 0X055EFFFF, 0XF919FA00, 0X12AE3700, 0XF0424A00, 0X019E40FB,
  0X2FF92F21, 0XF2466B00, 0XF44E2379, 0X1F3EBC80, 0X0DFF3B0F, 0XF27B61FF, 0X06631478, 0X1F3EBD80,
  0X3BF90D0F, 0XF6286180, 0XF4633DF8, 0X1FE0FCFF, 0X16F81604, 0XF0565E00, 0X06882B7A, 0X1F00FC00,
  0X1C321CF9, 0XF05D67FA, 0X01FE2E80, 0X06403C7A, 0X033C0BFC, 0XFA945780, 0XFA0E3480, 0X02FF7CF9,
  0X0B1703FC, 0XF26957F9, 0XFA0E4A00, 0X079F7E80, 0XFB19FBFE, 0XFE9D44FC, 0XF2304EFF, 0X033E7FFC
};

static uint32 aCircLUTs[9][60]={
  {
    0x2DF32DF3, 0x002CC180, 0x01F21600, 0x1E600000, 0xFDFD24F3, 0xF087AF00,
    0x01FC1180, 0x1E600000, 0xF21111F9, 0xE4FC7E79, 0xF20F0780, 0x1F200000,
    0xFAF1FAF7, 0xEE7AA07F, 0xF61E3D7F, 0x1E3EFBFF, 0xF1F4F4FD, 0xE2E571FD,
    0xE84E277F, 0x1E9E7CFF, 0xF2F2F2FF, 0xE49D4EFF, 0xE49D4EFF, 0x1E5CB97F,
    0x0FF72BF2, 0xF854BE80, 0x01F51480, 0x1E400000, 0xF4061BF6, 0xE8C199FD,
    0xFA040C80, 0x1EC00000, 0xF3F3F8FB, 0xE8AB8A00, 0xF2323180, 0x1E5EFC80,
    0xFDF50AF5, 0xF083AC00, 0xFC082400, 0x1E403E00, 0xF1FFFFFB, 0xE2F97CFB,
    0xEC2B15FF, 0x1EDF3E7F, 0xF1F1F1FF, 0xE2C4617F, 0xE6753AFF, 0x1E7D7AFF,
    0x0EF20EF3, 0xF852B980, 0x01FE2900, 0x1E403E00, 0xF3F904F7, 0xE6BF987D,
    0xF41A1E7F, 0x1E7F7E7F, 0xF2F2F2FD, 0xEA917800, 0xEE514880, 0x1E5DFA80
  },
  {
    0x3CFC3CFC, 0xF63B9680, 0x01FB1D80, 0x1F803D80, 0x0C0C36F5, 0xEA888E7B,
    0xF60B1A80, 0x1EBFBD80, 0xF82323F6, 0xF0DC6E76, 0xEC211080, 0x1EDF3E00,
    0x09F909F6, 0xEC7B857E, 0xF02E3D80, 0x1F3E3B7E, 0xF70101FA, 0xEECB65FA,
    0xEA582C00, 0x1EBDBB00, 0xF4F4F4FC, 0xE89C4E7C, 0xE89C4E7C, 0x1E9D3A7C,
    0x20023AF7, 0xEE6098FD, 0xFA031DFF, 0x1EFFBD7F, 0xFF172DF5, 0xEAB280F9,
    0xF2151580, 0x1EBF7E00, 0xFDFD05F7, 0xEAA4777C, 0xEC433600, 0x1EBDFB00,
    0x0B001CF5, 0xEA848DFD, 0xF41A2A80, 0x1EFEFC00, 0xF70F0FF8, 0xEEDA6C78,
    0xEA3B1D80, 0x1EBE7C80, 0xF5F8F8FA, 0xEAB85C7A, 0xE87B3DFE, 0x1E9D3A7E,
    0x20FB20F5, 0xF05C95FF, 0xF80C2E00, 0x1F7F3C7F, 0xFF0716F6, 0xEAAF7F7A,
    0xEE2A2480, 0x1EBEBC80, 0xFAF6FAF8, 0xE8936CFD, 0xE86049FF, 0x1EDD3A7D
  },
  {
    0x470D470D, 0xF2466AFB, 0xF60C2300, 0x01BEFCFB, 0x1F1F42FF, 0xF47A66F8,
    0xF021227F, 0x1FFE7C7C, 0x053333F9, 0x0AAC5579, 0xF2351AFE, 0x1F3E3C7E,
    0x1D091DFA, 0xF472617A, 0xF03E397E, 0x013E3D7A, 0x051515F8, 0x0AA150F8,
    0xFA5B2DFC, 0x1FBE7CFC, 0x010101F9, 0x028542F9, 0x028542F9, 0x002040F9,
    0x32164606, 0xF25E6A7A, 0xF4142200, 0x00DEBCFC, 0x10293CFB, 0xFC945FF8,
    0xF02B1F7E, 0x1F7E3C7D, 0x0E0E19F8, 0xFA8D5B78, 0xF24F35FC, 0x003E3CFA,
    0x1F132EFC, 0xF4796678, 0xEE302DFE, 0x007E3C7A, 0x062323F9, 0x0CA85479,
    0xF646237D, 0x1F7E3C7D, 0x020808F8, 0x04984B78, 0xFC743A7A, 0x1FDEFDFA,
    0x310C3100, 0xF25E68FA, 0xF2212F00, 0x019E7CFA, 0x111C2AFA, 0xFE905DF8,
    0xF23A28FE, 0x1FFE3C7C, 0x0D050DF8, 0xFA805379, 0xF660407C, 0x00BEFEF9
  },
  {
    0x47184718, 0xF649517A, 0xF41A24FE, 0x031EBDFA, 0x2929440B, 0x06674BFC,
    0xF82821FE, 0x017EFE7B, 0x12393901, 0x24844201, 0x02371BFC, 0x003F3E7C, 
    0x27152702, 0x046248FB, 0xFC3F317C, 0x02BF817B, 0x101F1FFD, 0x207F3FFD,
    0x0C542A7B, 0x00C0007B, 0x0C0C0CFB, 0x186D36FB, 0x186D36FB, 0x0183067B, 
    0x38214612, 0xFE564FFB, 0xF61F227E, 0x025EFE7B, 0x1D313F05, 0x127747FE,
    0xFC301F7D, 0x00BEFE7C, 0x1B1B2400, 0x107044FC, 0x02492DFB, 0x01A000FB, 
    0x291F3406, 0x06664AFB, 0xFA33297D, 0x021F3FFB, 0x122C2CFF, 0x248340FF,
    0x064522FB, 0x007FBF7B, 0x0F1515FC, 0x1E773AFC, 0x126130FB, 0x012142FB, 
    0x3718370B, 0xFE554E7B, 0xF62A2AFD, 0x031EFFFB, 0x1C253102, 0x127447FD,
    0x003C267C, 0x013F7F7B, 0x181118FD, 0x0E6A417B, 0x0656357B, 0x0220C3FB
  },
  {
    0x44204420, 0x0243407D, 0xFA1F21FD, 0x041F40FD, 0x2D2D4012, 0x145A3D00,
    0x002D207C, 0x025FC07C, 0x19383808, 0x326C3608, 0x10371BFC, 0x0100007C,
    0x2A1C2A09, 0x125439FF, 0x083C2A7C, 0x038104FF, 0x17242402, 0x2E673282, 
    0x1C4B25FD, 0x01C1C3FD, 0x131313FF, 0x265B2DFF, 0x265B2DFF, 0x0264C9FF, 
    0x38264319, 0x0A4E3E7F, 0xFE25217D, 0x033F80FD, 0x22333D0D, 0x22633904,
    0x08321E7C, 0x01A0007C, 0x20202705, 0x205F3680, 0x104428FC, 0x0281447E,
    0x2C24350E, 0x14573C00, 0x0434257C, 0x02E0027D, 0x182E2E05, 0x306A3505,
    0x164120FC, 0x0160C1FC, 0x151B1B00, 0x2A613080, 0x225329FE, 0x022346FE,
    0x371E3713, 0x0A4D3D7E, 0x002D26FC, 0x03C002FE, 0x21293209, 0x22613882,
    0x0C3A237C, 0x0220827D, 0x1E181E02, 0x1C59347F, 0x164B2CFD, 0x0302C77F
  },
  {
    0x3C253C25, 0x163B2D84, 0x08241DFE, 0x04A10584, 0x2D2D381A, 0x28472B0A,
    0x142D1C7F, 0x03418501, 0x1F333311, 0x3E512791, 0x22341A00, 0x02220400,
    0x2A212A12, 0x24432986, 0x1A352180, 0x04234906, 0x1D26260B, 0x3A4D258B,
    0x2C3E1F02, 0x02C40802, 0x1A1A1A06, 0x34462306, 0x34462306, 0x03468D06,
    0x34293A20, 0x1E412C07, 0x0E291D7E, 0x04014503, 0x26303616, 0x324C2A0D,
    0x1A301B7F, 0x02C1C480, 0x2323280F, 0x30482888, 0x22392001, 0x0363C884,
    0x2C263116, 0x26452A08, 0x18311F7F, 0x03C28703, 0x1E2C2C0E, 0x3C4F268E,
    0x28391C81, 0x02830601, 0x1C202009, 0x38492489, 0x30412084, 0x03054A84,
    0x3323331B, 0x1C402B85, 0x102D207F, 0x04620705, 0x252A2F12, 0x324A280B,
    0x1E351D80, 0x0322C682, 0x221E220C, 0x2C452606, 0x263E2282, 0x03C4CB06
  },
  {
    0x37263726, 0x1E362689, 0x12251B00, 0x4C24789, 0x2B2B341D, 0x2E3F248E,
    0x1C2C1A81, 0x3A2C706, 0x21303015, 0x42452295, 0x2A301803, 0x2A34683,
    0x29222916, 0x2C3C228A, 0x22321E03, 0x4444B0A, 0x1F262610, 0x3E412090,
    0x32371B86, 0x3250A06, 0x1C1C1C0B, 0x383D1E8B, 0x383D1E8B, 0x3870E0B,
    0x31293621, 0x263B258B, 0x16291B00, 0x4228787, 0x262E3219, 0x38422312,
    0x242E1902, 0x3230704, 0x24242713, 0x343E228D, 0x2A341C85, 0x3A4CA88,
    0x2A262E19, 0x2E3E238C, 0x202F1C82, 0x3E38908, 0x202B2B12, 0x40432192,
    0x2E341A05, 0x2E40805, 0x1E21210D, 0x3C3F1E8D, 0x363A1D09, 0x3660C09,
    0x3024301D, 0x2639240A, 0x1A2C1C81, 0x483498A, 0x25292D16, 0x3640238F,
    0x26311B03, 0x363C886, 0x221F2210, 0x323C210B, 0x2E371E07, 0x3E5CC8B
  },
  {
    0x32263226, 0x2832210D, 0x1A261902, 0x4C34A0D, 0x2A2A301F, 0x34361F93,
    0x26291785, 0x3E3C98A, 0x212D2D18, 0x423C1E18, 0x302E1707, 0x3044887,
    0x28222819, 0x32341E0F, 0x2A2D1A08, 0x4454C8F, 0x20252513, 0x403A1C13,
    0x3632190B, 0x365CB8B, 0x1E1E1E0F, 0x3C351A8F, 0x3C351A8F, 0x3C78F0F,
    0x2E283122, 0x2E352010, 0x20281883, 0x443898C, 0x252B2F1B, 0x3C3A1E15,
    0x2A2C1806, 0x3640909, 0x24242616, 0x38371D11, 0x30301989, 0x3E58C0D,
    0x29262C1C, 0x34361E11, 0x282C1906, 0x4048B0C, 0x21292916, 0x423A1D16,
    0x342F1789, 0x3450A09, 0x1F212111, 0x3E371B91, 0x3A341A0D, 0x3A68D0D,
    0x2D242D1F, 0x2C341F0E, 0x222A1A05, 0x4844B0E, 0x25272A19, 0x3A381D93,
    0x2E2E1888, 0x3A4CA8B, 0x23202314, 0x36351C8F, 0x32321A8B, 0x4064D8F
  },
  {
    0x2C252C25, 0x302D1A13, 0x26261689, 0x4A4CC13, 0x27272B20, 0x3A2F1997,
    0x2E27158B, 0x4054B91, 0x2229291C, 0x4431179C, 0x3829148E, 0x3858B0E,
    0x2522251C, 0x382D1994, 0x3229168E, 0x4464E14, 0x20242418, 0x40311798,
    0x3A2C1611, 0x3A68D11, 0x1F1F1F14, 0x3E2E1714, 0x3E2E1714, 0x3E7CF94,
    0x2A262C22, 0x342E1A15, 0x2A26160A, 0x4450C12, 0x24282A1E, 0x3E301919,
    0x3228150D, 0x3C58B8F, 0x2323251A, 0x3C2E1816, 0x362A1610, 0x4068D93,
    0x2624281E, 0x382E1996, 0x3028160D, 0x425CD12, 0x2126261A, 0x4231189A,
    0x382B158F, 0x3860C0F, 0x20212116, 0x402F1696, 0x3C2D1693, 0x3C74E93,
    0x29232920, 0x342D1A14, 0x2C27168B, 0x4658D14, 0x2425271C, 0x3E2F1898,
    0x3429158E, 0x3E60C91, 0x22212218, 0x3A2E1814, 0x382B1711, 0x4270E94
  }
};

static uint32 aSepLUTs[10][60]={
    {0x0, 0x8000, 0x0, 0x80000000, 0xDF000000, 0xF06EDD80, 0x0, 0x40000000, 0xE9000000, 0xD3178B80, 0x0, 0x20000000, 0xE3F9E302, 0xF25FC001, 0xFC182F80, 0x3F3FBC81, 0xECEEEE01, 0xD8F27801, 0xF63D1E80, 0x3F7F3E00, 0xF3F3F301, 0xE6994C81, 0xE6994C81, 0x1E7CF981, 0xF3000000, 0xFC177B00, 0x10080, 0x80000000, 0xE1000000, 0xE4BEB780, 0x0, 0x20000000, 0xE5F4E802, 0xE8A49F81, 0xFA292780, 0x3F3F7D00, 0xE0FBEA02, 0xF06BD580, 0xFE0B1400, 0x3FBFFE80, 0xEAF2F201, 0xD50E8701, 0xFC190C80, 0x3FDFBF00, 0xF0EFEF01, 0xE0C86401, 0xEE683401, 0x3EFE3C01, 0xE8FEE801, 0xFA2CEE80, 0x41600, 0x3FC03E80, 0xE2F7EE02, 0xE4B8B101, 0xFE111000, 0x3FBFFF00, 0xEAF4EA02, 0xEC888381, 0xF6474400, 0x3E9EFB01},
    {0x0, 0x8000, 0x0, 0x80000000, 0x0, 0x80C000, 0x0, 0x0, 0x0, 0x1008000, 0x0, 0x0, 0x0, 0x609000, 0x203000, 0x0, 0x0, 0xC06000, 0x402000, 0x0, 0x0, 0x804000, 0x804000, 0x0, 0x0, 0x40E000, 0x0, 0x0, 0x0, 0xC0A000, 0x0, 0x0, 0x0, 0x907800, 0x302800, 0x0, 0x0, 0x70A800, 0x101800, 0x0, 0x0, 0xE07000, 0x201000, 0x0, 0x0, 0xA05000, 0x603000, 0x0, 0x0, 0x38C400, 0x81C00, 0x0, 0x0, 0xA88C00, 0x181400, 0x0, 0x0, 0x786400, 0x483C00, 0x0},
    {0x29052905, 0x29A400, 0x51480, 0xA00000, 0x150F2103, 0x7B8500, 0xF1080, 0x600000, 0x1A1A00, 0xCD6580, 0x1A0D00, 0x0, 0x11081101, 0x646B80, 0x2E3200, 0x1000000, 0xD0D00, 0xA65300, 0x4D2680, 0x0, 0x0, 0x804000, 0x804000, 0x0, 0x1E0A2504, 0x529500, 0xA1280, 0x800000, 0xA141D01, 0xA57580, 0x150F00, 0x200000, 0x80A0F01, 0x855F80, 0x3E2C00, 0x800000, 0x130B1902, 0x707880, 0x1F2100, 0xA00000, 0x131300, 0xBA5D00, 0x331980, 0x0, 0x70700, 0x934880, 0x673380, 0x0, 0x1C081C03, 0x4A8680, 0x142500, 0x1000000, 0x90F1601, 0x956A80, 0x291D80, 0x600000, 0x7050700, 0x765500, 0x523B00, 0xA00000},
    {0x390E390E, 0x397200, 0xE1C80, 0x1C00000, 0x1D1C3107, 0x736300, 0x1D1900, 0xE00000, 0x2B2B00, 0xAB5480, 0x2B1580, 0x0, 0x190E1904, 0x645680, 0x393200, 0x1C00000, 0x161600, 0x954980, 0x562B00, 0x0, 0x0, 0x804000, 0x804000, 0x0, 0x2B15350B, 0x566A80, 0x151A80, 0x1600000, 0xE232E03, 0x8F5C80, 0x241780, 0x600000, 0xC121702, 0x7C5100, 0x472E80, 0xE00000, 0x1B152505, 0x6B5D00, 0x2B2580, 0x1600000, 0x202000, 0xA05000, 0x402000, 0x0, 0xB0B00, 0x8B4480, 0x6B3580, 0x0, 0x28102808, 0x506400, 0x202800, 0x2000000, 0xD1B2303, 0x855680, 0x352300, 0xA00000, 0xB090B01, 0x744B00, 0x593A00, 0x1200000},
    {0x3F183F18, 0x3E5380, 0x171F00, 0x3000000, 0x1F273B0C, 0x684E80, 0x271D80, 0x1800000, 0x373700, 0x924900, 0x371B80, 0x0, 0x1D141D06, 0x624980, 0x413100, 0x2800000, 0x1C1C00, 0x894380, 0x5C2E00, 0x0, 0x0, 0x804000, 0x804000, 0x0, 0x2F1F3D12, 0x535100, 0x1F1E80, 0x2400000, 0xF2F3906, 0x7D4C00, 0x2F1C80, 0xC00000, 0xE181D03, 0x754700, 0x4E2F80, 0x1400000, 0x1F1D2C09, 0x664B80, 0x342700, 0x2000000, 0x292900, 0x8E4700, 0x492480, 0x0, 0xE0E00, 0x854180, 0x6E3700, 0x0, 0x2D172D0D, 0x514F80, 0x2A2880, 0x2E00000, 0xF232B04, 0x794A00, 0x3E2600, 0x1000000, 0xE0C0E01, 0x724480, 0x5E3900, 0x1800000},
    {0x40204020, 0x404000, 0x202000, 0x4000000, 0x20304010, 0x604000, 0x302000, 0x2000000, 0x404000, 0x804000, 0x402000, 0x0, 0x20182008, 0x604000, 0x483000, 0x3000000, 0x202000, 0x804000, 0x603000, 0x0, 0x0, 0x804000, 0x804000, 0x0, 0x30284018, 0x504000, 0x282000, 0x3000000, 0x10384008, 0x704000, 0x382000, 0x1000000, 0x101C2004, 0x704000, 0x543000, 0x1800000, 0x2024300C, 0x604000, 0x3C2800, 0x2800000, 0x303000, 0x804000, 0x502800, 0x0, 0x101000, 0x804000, 0x703800, 0x0, 0x301E3012, 0x504000, 0x322800, 0x3C00000, 0x102A3006, 0x704000, 0x462800, 0x1400000, 0x100E1002, 0x704000, 0x623800, 0x1C00000},
    {0x3D2E3D2E, 0x3E2880, 0x2F1F00, 0x5C00000, 0x29363D1F, 0x14472908, 0x10361E80, 0x3E00000, 0x143E3E0F, 0x2852290F, 0x1E3E1F00, 0x1E00000, 0x29242915, 0x14482805, 0x12402401, 0x4824505, 0x1429290A, 0x2852290A, 0x24482403, 0x2428503, 0x14141405, 0x28532985, 0x28532985, 0x2850A05, 0x33323D27, 0xA432884, 0x8321E80, 0x4E00000, 0x1F3A3E17, 0x1E4D288B, 0x163A1F00, 0x2E00000, 0x1F27290F, 0x1E4D2888, 0x1A432402, 0x3628504, 0x292D331A, 0x14472807, 0x103A2181, 0x4414283, 0x1433330D, 0x2852290D, 0x22432181, 0x2214281, 0x141F1F08, 0x28522908, 0x264D2684, 0x263C784, 0x332A3320, 0xA432903, 0x8362180, 0x5410283, 0x1F303313, 0x1E4D288A, 0x183F2181, 0x3214282, 0x1F1D1F0C, 0x1E4D2886, 0x1C482683, 0x3A38786},
    {0x39393939, 0x391C00, 0x391C80, 0x7200000, 0x2B39392B, 0x1C391C0E, 0x1C391C80, 0x5600000, 0x1C39391C, 0x383A1C1C, 0x383A1D00, 0x3800000, 0x2B2B2B20, 0x1C391B8B, 0x1C391C84, 0x563870B, 0x1C2B2B15, 0x383A1C15, 0x383A1D07, 0x3838707, 0x1C1C1C0E, 0x383A1D0E, 0x383A1D0E, 0x3870E0E, 0x31393932, 0xE391C87, 0xE391C80, 0x6400000, 0x24393924, 0x2A391C15, 0x2A391C80, 0x4800000, 0x242B2B1B, 0x2A391C10, 0x2A391C85, 0x4838709, 0x2B323125, 0x1C391C8D, 0x1C391C82, 0x561C385, 0x1C323219, 0x383A1C19, 0x383A1D03, 0x381C383, 0x1C242412, 0x38391C92, 0x38391C8B, 0x3854A8B, 0x3232322B, 0xE391C86, 0xE391C81, 0x641C386, 0x2432311F, 0x2A391C93, 0x2A391C83, 0x481C384, 0x24242416, 0x2A391C8D, 0x2A391C88, 0x4854A8D},
    {0x2A2A2A2A, 0x2A2A1415, 0x2A2A150A, 0x5454A95, 0x252A2A25, 0x342A141A, 0x342A150D, 0x4A54A92, 0x202A2A1F, 0x402A141F, 0x3E2A1510, 0x3E54A90, 0x25252520, 0x342A1417, 0x342A1510, 0x4A68D17, 0x2025251C, 0x4029149C, 0x3E291494, 0x3E68D14, 0x1F1F1F18, 0x3E2A1518, 0x3E2A1518, 0x3E7CF98, 0x272A2A27, 0x302A1497, 0x2E2A150C, 0x4E54A94, 0x222A2A22, 0x3A2A141D, 0x3A2A150E, 0x4454A91, 0x2225251E, 0x3A2A1419, 0x3A2A1512, 0x4468D15, 0x25272722, 0x342A1498, 0x342A150F, 0x4A5CC15, 0x2027271D, 0x402A151D, 0x3E2A1512, 0x3E5CB92, 0x1F22221A, 0x3E2A141A, 0x3E2A1516, 0x3E74E96, 0x27272725, 0x302A1516, 0x2E2A150D, 0x4E5CC16, 0x22272720, 0x3A2A151B, 0x3A2A1510, 0x445CB93, 0x2222221C, 0x3A2A1417, 0x3A2A1514, 0x4474E97},
    {0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020, 0x20202020, 0x40201020, 0x40201020, 0x4081020}
};

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_DestScaler_BaseOffset()
*/
/*!
* \brief
*     Returns the offset of Destination scaler register block from first layer's SSPP address
*
* \param [in]   eDestPipe         - Destination pipe id
*
* \retval uint32
*
****************************************************************************/
static uintPtr HAL_MDP_Get_DestScaler_BaseOffset(HAL_MDP_DestinationPipeId   eDestPipe)
{
   uintPtr      uRegOffset        = 0;

   uRegOffset = uMDPDestScalerRegBaseOffset[eDestPipe];

   return uRegOffset;
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_QSEED_LUTConfig()
*/
/*!
* \brief
*     Configures the QSEED LUT
*
* \param [in] uRegOffset            - Scalar register offset
* \param [in] psScalarConfig        - QSEED LUT configuration
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_QSEED_LUTConfig(uintPtr                   uScalarRegOffset,
                                    HAL_MDP_ScalarConfigType *psScalarConfig)
{
  uint32 COEF_Y_DIR_INFO[4][5][2] = 
  {
    { { 18,  0x000  },{  6,  0x320  },{  6,  0x380  },{  6, 0x500 },{  6, 0x560 } },
    { { 12,  0x120  },{  3,  0x3E0  },{  3,  0x410  },{  3, 0x5C0 },{  3, 0x5F0 } },
    { { 12,  0x1E0  },{  3,  0x440  },{  3,  0x470  },{  3, 0x620 },{  3, 0x650 } },
    { {  8,  0x2A0  },{  3,  0x4A0  },{  3,  0x4D0  },{  3, 0x680 },{  3, 0x6B0 } },
  };
  bool32  enableWr[5] =
  {
    psScalarConfig->b2dWriteEnable,
    psScalarConfig->bYCircWriteEnable,
    psScalarConfig->bYSepWriteEnable,
    psScalarConfig->bUVCircWriteEnable,
    psScalarConfig->bUVSepWriteEnable
  };

  uint32*  wrLut[5] = 
  {
    aDirLUTs, NULL, NULL, NULL, NULL
  };
  uint32  lutValue[5] = 
  {
    psScalarConfig->e2dLutIndex,
    psScalarConfig->eYCircLutIndex,
    psScalarConfig->eYSepLutIndex,
    psScalarConfig->eUVCircLutIndex,
    psScalarConfig->eUVSepLutIndex
  };
  uint32     uLUTRegionId        = 0;
  uint32     uFilterId           = 0;
  uintPtr    uQseed3CoefBase     = 0;
  uint32     uRegIndex           = 0;
  uint32     uLutIndex           = 0;
  uint32     uCoefTabSize        = 0;
  uintPtr    uRegAddr            = 0;

  uint32     uLutCtrl            = 0;
  
  if ( psScalarConfig->b2dWriteEnable 
    || psScalarConfig->bYCircWriteEnable
    || psScalarConfig->bYSepWriteEnable
    || psScalarConfig->bUVCircWriteEnable
    || psScalarConfig->bUVSepWriteEnable )
  {
    if((psScalarConfig->bYCircWriteEnable) && 
       (psScalarConfig->eYCircLutIndex >= HAL_MDP_QSEED_LUT_INDEX_0) &&
       (psScalarConfig->eYCircLutIndex <  HAL_MDP_QSEED_LUT_INDEX_9))
    {
      wrLut[1] = (uint32 *)&aCircLUTs[psScalarConfig->eYCircLutIndex - HAL_MDP_QSEED_LUT_INDEX_0];
    }
    if((psScalarConfig->bYSepWriteEnable) &&
       (psScalarConfig->eYSepLutIndex >= HAL_MDP_QSEED_LUT_INDEX_0) &&
       (psScalarConfig->eYSepLutIndex <  HAL_MDP_QSEED_LUT_INDEX_MAX))
    {
      wrLut[2] = (uint32 *)&aSepLUTs[psScalarConfig->eYSepLutIndex - HAL_MDP_QSEED_LUT_INDEX_0];
    }
    if((psScalarConfig->bUVCircWriteEnable) && 
       (psScalarConfig->eUVCircLutIndex >= HAL_MDP_QSEED_LUT_INDEX_0) &&
       (psScalarConfig->eUVCircLutIndex <  HAL_MDP_QSEED_LUT_INDEX_9 ))
    {
      wrLut[3] = (uint32 *)&aCircLUTs[psScalarConfig->eUVCircLutIndex - HAL_MDP_QSEED_LUT_INDEX_0];
    }
    if((psScalarConfig->bUVSepWriteEnable) && 
       (psScalarConfig->eUVSepLutIndex >= HAL_MDP_QSEED_LUT_INDEX_0) &&
       (psScalarConfig->eUVSepLutIndex < HAL_MDP_QSEED_LUT_INDEX_MAX))
    {
      wrLut[4] = (uint32 *)&aSepLUTs[psScalarConfig->eUVSepLutIndex - HAL_MDP_QSEED_LUT_INDEX_0];
    }
    for (uFilterId = 0; uFilterId < (sizeof(enableWr)/sizeof(enableWr[0])); uFilterId++)
    { 
      if ((enableWr[uFilterId] == 0) || (lutValue[uFilterId] < HAL_MDP_QSEED_LUT_INDEX_0))
      {
          continue;
      }
      uLutIndex = 0;
      for (uLUTRegionId = 0; uLUTRegionId < 4; uLUTRegionId++)
      {
        uQseed3CoefBase    = HWIO_MDP_VP_0_VIG_0_QSEED3_COEF_LUT_COEF_Y_DIR_A_IDXi_DATAj_ADDR(0,0) + uScalarRegOffset + COEF_Y_DIR_INFO[uLUTRegionId][uFilterId][1];
        uCoefTabSize       = COEF_Y_DIR_INFO[uLUTRegionId][uFilterId][0] << 2;
        uRegAddr = uQseed3CoefBase;
        for (uRegIndex = 0; uRegIndex < uCoefTabSize; uRegIndex++, uRegAddr += 4, uLutIndex++)
        {
          out_dword(uRegAddr , wrLut[uFilterId][uLutIndex]);
        }
      }
    } 
    
  } 
  // Enable SWAP
  if(psScalarConfig->bLutSwapEnable)
  {
    uLutCtrl   = in_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_COEF_LUT_CTRL_ADDR + uScalarRegOffset);
    uLutCtrl   = HWIO_OUT_FLD(uLutCtrl, MDP_VP_0_VIG_0_QSEED3_COEF_LUT_CTRL, SWAP, 1);
    out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_COEF_LUT_CTRL_ADDR + uScalarRegOffset, uLutCtrl); 
  }
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_QSEED_DE_Config()
*/
/*!
* \brief
*     Program  qseed DE registers
*
* \param [in]   uRegOffset              - Register offset
* \param [in]   psScalarConfig          - Scalar configuration
*
* \retval void
*
****************************************************************************/
void HAL_MDP_QSEED_DE_Config(uintPtr                          uRegOffset,
                             HAL_MDP_ScalarConfigType        *psScalarConfig)
{
  uint32        uDeSharpen          = 0;
  uint32        uDeSharpenCtl       = 0;
  uint32        uDeShapeCtl         = 0;
  uint32        uDeThreshold        = 0;
  uint32        uDeAdjustData0      = 0;
  uint32        uDeAdjustData1      = 0;
  uint32        uDeAdjustData2      = 0;

  if (TRUE == psScalarConfig->sDetailEnhancer.bDetailEnhancerEnable)
  {
    uDeSharpen   = HWIO_OUT_FLD(uDeSharpen,
                     MDP_VP_0_VIG_0_QSEED3_DE_SHARPEN,
                     DE_SHARPEN_LEVEL1,
                     psScalarConfig->sDetailEnhancer.iDeSharpenLevel1);
    uDeSharpen   = HWIO_OUT_FLD(uDeSharpen,
                     MDP_VP_0_VIG_0_QSEED3_DE_SHARPEN,
                     DE_SHARPEN_LEVEL2,
                     psScalarConfig->sDetailEnhancer.iDeSharpenLevel2);

    uDeSharpenCtl = HWIO_OUT_FLD(uDeSharpenCtl,
                     MDP_VP_0_VIG_0_QSEED3_DE_SHARPEN_CTL,
                     DE_LIMIT,
                     psScalarConfig->sDetailEnhancer.uDeLimit);
    uDeSharpenCtl = HWIO_OUT_FLD(uDeSharpenCtl,
                     MDP_VP_0_VIG_0_QSEED3_DE_SHARPEN_CTL,
                     DE_PREC,
                     psScalarConfig->sDetailEnhancer.uDePrecisionShift);
    uDeSharpenCtl = HWIO_OUT_FLD(uDeSharpenCtl,
                     MDP_VP_0_VIG_0_QSEED3_DE_SHARPEN_CTL,
                     DE_CLIP,
                     psScalarConfig->sDetailEnhancer.uDeClipShift);

    uDeShapeCtl   = HWIO_OUT_FLD(uDeShapeCtl,
                     MDP_VP_0_VIG_0_QSEED3_DE_SHAPE_CTL,
                     THR_QUIET,
                     psScalarConfig->sDetailEnhancer.uDeThresholdQuiet);
    uDeShapeCtl   = HWIO_OUT_FLD(uDeShapeCtl,
                     MDP_VP_0_VIG_0_QSEED3_DE_SHAPE_CTL,
                     THR_DIEOUT,
                     psScalarConfig->sDetailEnhancer.uDeThresholdDieout);

    uDeThreshold   = HWIO_OUT_FLD(uDeThreshold,
                     MDP_VP_0_VIG_0_QSEED3_DE_THRESHOLD,
                     THR_LOW,
                     psScalarConfig->sDetailEnhancer.uDeThresholdLow);
    uDeThreshold   = HWIO_OUT_FLD(uDeThreshold,
                     MDP_VP_0_VIG_0_QSEED3_DE_THRESHOLD,
                     THR_HIGH,
                     psScalarConfig->sDetailEnhancer.uDeThresholdHigh);

    uDeAdjustData0   = HWIO_OUT_FLD(uDeAdjustData0,
                     MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_0,
                     ADJUST_A0,
                     psScalarConfig->sDetailEnhancer.iDeAdjustA[0]);
    uDeAdjustData0   = HWIO_OUT_FLD(uDeAdjustData0,
                     MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_0,
                     ADJUST_A1,
                     psScalarConfig->sDetailEnhancer.iDeAdjustA[1]);
    uDeAdjustData0   = HWIO_OUT_FLD(uDeAdjustData0,
                     MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_0,
                     ADJUST_A2,
                     psScalarConfig->sDetailEnhancer.iDeAdjustA[2]);

    uDeAdjustData1   = HWIO_OUT_FLD(uDeAdjustData1,
                     MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_1,
                     ADJUST_B0,
                     psScalarConfig->sDetailEnhancer.iDeAdjustB[0]);
    uDeAdjustData1   = HWIO_OUT_FLD(uDeAdjustData1,
                     MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_1,
                     ADJUST_B1,
                     psScalarConfig->sDetailEnhancer.iDeAdjustB[1]);
    uDeAdjustData1   = HWIO_OUT_FLD(uDeAdjustData1,
                     MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_1,
                     ADJUST_B2,
                     psScalarConfig->sDetailEnhancer.iDeAdjustB[2]);

    uDeAdjustData2   = HWIO_OUT_FLD(uDeAdjustData2,
                     MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_2,
                     ADJUST_C0,
                     psScalarConfig->sDetailEnhancer.iDeAdjustC[0]);
    uDeAdjustData2   = HWIO_OUT_FLD(uDeAdjustData2,
                     MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_2,
                     ADJUST_C1,
                     psScalarConfig->sDetailEnhancer.iDeAdjustC[1]);
    uDeAdjustData2   = HWIO_OUT_FLD(uDeAdjustData2,
                     MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_2,
                     ADJUST_C2,
                     psScalarConfig->sDetailEnhancer.iDeAdjustC[2]);

    out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_DE_SHARPEN_ADDR       + uRegOffset, uDeSharpen);
    out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_DE_SHARPEN_CTL_ADDR   + uRegOffset, uDeSharpenCtl);
    out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_DE_SHAPE_CTL_ADDR     + uRegOffset, uDeShapeCtl);
    out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_DE_THRESHOLD_ADDR     + uRegOffset, uDeThreshold);
    out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_0_ADDR + uRegOffset, uDeAdjustData0);
    out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_1_ADDR + uRegOffset, uDeAdjustData1);
    out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_DE_ADJUST_DATA_2_ADDR + uRegOffset, uDeAdjustData2);
  }
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_QSEED_Config()
*/
/*!
* \brief
*     Program  qseed registers
*
* \param [in]   uRegOffset              - Register offset
* \param [in]   psScalarConfig          - Scalar configuration
*
* \retval void
*
****************************************************************************/
static void HAL_MDP_QSEED_Config(uintPtr                          uRegOffset,
                                 HAL_MDP_ScalarConfigType        *psScalarConfig)
{
  uint32        uOpMode             = 0;
  uint32        uPreload            = 0;
  uint32        uSrcSizeY           = 0;
  uint32        uSrcSizeUV          = 0;
  uint32        uDstSize            = 0;
    
    /* Always enable scaler even in the unity scaling case*/
    uOpMode = HWIO_OUT_FLD(uOpMode,
                           MDP_VP_0_VIG_0_QSEED3_OP_MODE,
                           EN,
                           psScalarConfig->bScalarEnable);
    if (TRUE == psScalarConfig->bScalarEnable)
    {
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_VP_0_VIG_0_QSEED3_OP_MODE,
                             DIR_EN,
                             psScalarConfig->bDirectionDetectionEnable);
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_VP_0_VIG_0_QSEED3_OP_MODE,
                             DE_EN,
                             psScalarConfig->sDetailEnhancer.bDetailEnhancerEnable);
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_VP_0_VIG_0_QSEED3_OP_MODE,
                             ALPHA_EN,
                             psScalarConfig->bAlpha);
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_VP_0_VIG_0_QSEED3_OP_MODE,
                             COLOR_SPACE,
                             psScalarConfig->bYuvColorSpace);
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_VP_0_VIG_0_QSEED3_OP_MODE,
                             BIT_WIDTH,
                             psScalarConfig->bBitWidth);

      /* Scaling filter of component y/rgb */
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_VP_0_VIG_0_QSEED3_OP_MODE,
                             Y_CFG,
                             psScalarConfig->uC0Filter);
      /* Scaling filter of component uv */
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_VP_0_VIG_0_QSEED3_OP_MODE,
                             UV_CFG,
                             psScalarConfig->uC12Filter);
      
      /* Scaling filter of alpha component */
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_VP_0_VIG_0_QSEED3_OP_MODE,
                             A_CFG,
                             psScalarConfig->uC3Filter);
      /* Selection of blend coefficients */
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_VP_0_VIG_0_QSEED3_OP_MODE,
                             BLEND_CFG,
                             psScalarConfig->uBlendFilter);

      uPreload = HWIO_OUT_FLD(uPreload,
                              MDP_VP_0_VIG_0_QSEED3_PRELOAD,
                              Y_PRELOAD_H,
                              psScalarConfig->iC03HorizontalPreload);
      uPreload = HWIO_OUT_FLD(uPreload,
                              MDP_VP_0_VIG_0_QSEED3_PRELOAD,
                              Y_PRELOAD_V,
                              psScalarConfig->iC03VerticalPreload);
      uPreload = HWIO_OUT_FLD(uPreload,
                              MDP_VP_0_VIG_0_QSEED3_PRELOAD,
                              UV_PRELOAD_H,
                              psScalarConfig->iC12HorizontalPreload);
      uPreload = HWIO_OUT_FLD(uPreload,
                              MDP_VP_0_VIG_0_QSEED3_PRELOAD,
                              UV_PRELOAD_V,
                              psScalarConfig->iC12VerticalPreload);

      uSrcSizeY = HWIO_OUT_FLD(uSrcSizeY,
                               MDP_VP_0_VIG_0_QSEED3_SRC_SIZE_Y_RGB_A,
                               Y_RGB_A_SRC_W,
                               psScalarConfig->uC03SrcWidth);
      uSrcSizeY = HWIO_OUT_FLD(uSrcSizeY,
                               MDP_VP_0_VIG_0_QSEED3_SRC_SIZE_Y_RGB_A,
                               Y_RGB_A_SRC_H,
                               psScalarConfig->uC03SrcHeight);
      uSrcSizeUV = HWIO_OUT_FLD(uSrcSizeUV,
                               MDP_VP_0_VIG_0_QSEED3_SRC_SIZE_UV,
                               UV_SRC_W,
                               psScalarConfig->uC12SrcWidth);
      uSrcSizeUV = HWIO_OUT_FLD(uSrcSizeUV,
                                MDP_VP_0_VIG_0_QSEED3_SRC_SIZE_UV,
                                UV_SRC_H,
                                psScalarConfig->uC12SrcHeight);

      uDstSize = HWIO_OUT_FLD(uDstSize,
                              MDP_VP_0_VIG_0_QSEED3_DST_SIZE,
                              DST_W,
                              psScalarConfig->uDstWidth);
      uDstSize = HWIO_OUT_FLD(uDstSize,
                              MDP_VP_0_VIG_0_QSEED3_DST_SIZE,
                              DST_H,
                              psScalarConfig->uDstHeight);
      
      HAL_MDP_QSEED_LUTConfig(uRegOffset, psScalarConfig);

      if(TRUE == psScalarConfig->sDetailEnhancer.bDetailEnhancerEnable)
      {
        HAL_MDP_QSEED_DE_Config(uRegOffset,psScalarConfig);
      }

      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_PHASE_INIT_Y_H_ADDR   + uRegOffset, psScalarConfig->uC03HorizontalInitPhase);
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_PHASE_INIT_Y_V_ADDR   + uRegOffset, psScalarConfig->uC03VerticalInitPhase);
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_PHASE_INIT_UV_H_ADDR  + uRegOffset, psScalarConfig->uC12HorizontalInitPhase);
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_PHASE_INIT_UV_V_ADDR  + uRegOffset, psScalarConfig->uC12VerticalInitPhase);
      
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_PHASE_STEP_Y_H_ADDR   + uRegOffset,psScalarConfig->uC03HorizontalPhaseStep);
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_PHASE_STEP_Y_V_ADDR   + uRegOffset,psScalarConfig->uC03VerticalPhaseStep);
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_PHASE_STEP_UV_H_ADDR  + uRegOffset,psScalarConfig->uC12HorizontalPhaseStep);
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_PHASE_STEP_UV_V_ADDR  + uRegOffset,psScalarConfig->uC12VerticalPhaseStep);
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_PRELOAD_ADDR          + uRegOffset,uPreload);
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_SRC_SIZE_Y_RGB_A_ADDR + uRegOffset,uSrcSizeY);
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_SRC_SIZE_UV_ADDR      + uRegOffset,uSrcSizeUV);
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_DST_SIZE_ADDR         + uRegOffset,uDstSize);
    }
    out_dword(HWIO_MDP_VP_0_VIG_0_QSEED3_OP_MODE_ADDR          + uRegOffset, uOpMode);

}



/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ScalerLUTConfig()
*/
/*!
* \brief
*     Configures the SSPP scaler LUT
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] psScalarConfig        - QSEED LUT configuration
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_SSPP_ScalerLUTConfig(HAL_MDP_SourcePipeId      eSourcePipe, 
                                  HAL_MDP_ScalarConfigType *psScalarConfig)
{
  uintPtr uLayerRegOffset = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);

  HAL_MDP_QSEED_LUTConfig(uLayerRegOffset,psScalarConfig);
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ScalerDEConfig()
*/
/*!
* \brief
*     Program  scaler DE registers
*
* \param [in]   eSourcePipe             - Source pipe id
* \param [in]   eLayerType              - Layer Type, Only RGB, VG and DMA are supported
* \param [in]   psScalarConfig          - Scalar configuration
*
* \retval void
*
****************************************************************************/
void HAL_MDP_SSPP_ScalerDEConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                 HAL_MDP_LayerType                eLayerType,
                                 HAL_MDP_ScalarConfigType        *psScalarConfig)
{
  uintPtr       uLayerRegOffset     = 0;

  if(HAL_MDP_LAYER_TYPE_VIG == eLayerType)
  {
    uLayerRegOffset   = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);
    HAL_MDP_QSEED_DE_Config(uLayerRegOffset,psScalarConfig);
  }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ScalerConfig()
*/
/*!
* \brief
*     Program  scaler registers
*
* \param [in]   eSourcePipe             - Source pipe id
* \param [in]   eLayerType              - Layer Type, Only RGB, VG and DMA are supported
* \param [in]   psScalarConfig          - Scalar configuration
*
* \retval void
*
****************************************************************************/
void HAL_MDP_SSPP_ScalerConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                               HAL_MDP_LayerType                eLayerType,
                               HAL_MDP_ScalarConfigType        *psScalarConfig)
{
  uintPtr       uLayerRegOffset     = 0;

  if(HAL_MDP_LAYER_TYPE_VIG == eLayerType)
  {
    uLayerRegOffset   = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);
    HAL_MDP_QSEED_Config(uLayerRegOffset,psScalarConfig);
  }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ScalerLUTConfig()
*/
/*!
* \brief
*     Configures the DSPP scaler LUT
*
* \param [in] eDestPipe             - Destination Pipe ID
* \param [in] psScalarConfig        - QSEED LUT configuration
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_ScalerLUTConfig(HAL_MDP_DestinationPipeId   eDestPipe, 
                                  HAL_MDP_ScalarConfigType   *psScalarConfig)
{
  uintPtr uRegOffset = HAL_MDP_Get_DestScaler_BaseOffset(eDestPipe);

  HAL_MDP_QSEED_LUTConfig(uRegOffset,psScalarConfig);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_ScalerDEConfig()
*/
/*!
* \brief
*     Program  destination scaler DE registers
*     populated from HAL_MDP_UTILITY_CalcDisplayConfig()
*
* \param [in]   eDestPipe               - Destination pipe id
* \param [in]   psScalarConfig          - Values populated from 
*                                         HAL_MDP_UTILITY_CalcDisplayConfig()
*
* \retval void
*
****************************************************************************/
void HAL_MDP_DSPP_ScalerDEConfig(HAL_MDP_DestinationPipeId        eDestPipe,
                                 HAL_MDP_ScalarConfigType        *psScalarConfig)
{
  uintPtr uDestScalarRegOffset = HAL_MDP_Get_DestScaler_BaseOffset(eDestPipe);

  HAL_MDP_QSEED_DE_Config(uDestScalarRegOffset,psScalarConfig);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_ScalerConfig()
*/
/*!
* \brief
*     Program  scaler registers
*
* \param [in]   eDestPipe               - Destination pipe id
* \param [in]   psScalarConfig          - Scalar configuration
*
* \retval void
*
****************************************************************************/
void HAL_MDP_DSPP_ScalerConfig(HAL_MDP_DestinationPipeId        eDestPipe,
                               HAL_MDP_ScalarConfigType        *psScalarConfig)
{
  uintPtr uRegOffset      = HAL_MDP_Get_DestScaler_BaseOffset(eDestPipe);
  static uint32  uOpMode  = 0;

  if( (HAL_MDP_DESTINATION_PIPE_0 == eDestPipe) || (HAL_MDP_DESTINATION_PIPE_1 == eDestPipe))
  {
    HAL_MDP_QSEED_Config(uRegOffset,psScalarConfig);

    //uOpMode  = in_dword(HWIO_MDP_DEST_SCALER_0_OP_MODE_ADDR);
    if(HAL_MDP_DESTINATION_PIPE_0 == eDestPipe)
    {
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_DEST_SCALER_0_OP_MODE,
                             SCALER_0_EN,
                             psScalarConfig->bScalarEnable);
    }
    else
    {
      uOpMode = HWIO_OUT_FLD(uOpMode,
                             MDP_DEST_SCALER_0_OP_MODE,
                             SCALER_1_EN,
                             psScalarConfig->bScalarEnable);
    }
    uOpMode = HWIO_OUT_FLD(uOpMode,
                           MDP_DEST_SCALER_0_OP_MODE,
                           SCALER_01_DUAL_MODE,
                           psScalarConfig->bDualScalarMode);
    out_dword(HWIO_MDP_DEST_SCALER_0_OP_MODE_ADDR, uOpMode);
  }

}




#ifdef __cplusplus
}
#endif

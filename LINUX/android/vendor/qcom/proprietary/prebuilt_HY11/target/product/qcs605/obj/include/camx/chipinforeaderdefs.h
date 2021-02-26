//-------------------------------------------------------------------------
// Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  chipinforeaderdefs.h
// @brief This module is in charge of reading the necessary HW/chip version
//        register(s) at start up to populate a registry of information
//        usable by other SW components.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
// 01/15/17          etsairi       Initial version
//------------------------------------------------------------------------

#ifndef _CHIP_INFO_READER_DEFS_H_
#define _CHIP_INFO_READER_DEFS_H_

/*------------------------------------------------------------------------
*       Include Files
* ----------------------------------------------------------------------- */

typedef enum
{
    TITAN_180 = 0x00010800,
    TITAN_175 = 0x00010705,
    TITAN_170 = 0x00010700,
    TITAN_160 = 0x00010600,
    TITAN_150 = 0x00010500,
    TITAN_140 = 0x00010400,
    TITAN_120 = 0x00010200,
    TITAN_480 = 0x00040800,
} TitanVersion;

/* Derived information combining Titan Tiering version and HW version */
typedef enum
{
    TITAN_VERSION_FIRST = 0,

    TITAN_170_V1 = 1,        // SDM845 (Napali)
    TITAN_160_V1 = 2,        // SDM840 (NapaliQ)
    TITAN_175_V1 = 3,        // SDM855 (Hana)
    TITAN_170_V2 = 4,        // SDM845 (Napali) v2
    TITAN_150_V1 = 5,        // SDM640 (Talos)
    TITAN_170_V2_ONEIPE = 6, // SDM670 (Warlock 1 core)
    TITAN_480_V1 = 7,        // SDM865 (Kona)
    TITAN_175_V2 = 8,         // SDM855 (Hana) v2
    TITAN_VERSION_MAX,

    // ALIASES
    SANDBOX = TITAN_175_V1,    // SANDBOX is initially the same as T175, need to have its own value after it diverges
    SPECTRA_480 = TITAN_480_V1
} ChipVersion;

typedef enum
{
    TITAN_VER_FIRST = TITAN_VERSION_FIRST,
    NAPALI = TITAN_170_V1,
    NAPALI_Q = TITAN_160_V1,
    HANA = TITAN_175_V1,
    NAPALI_V2 = TITAN_170_V2,
    TALOS = TITAN_150_V1,
    WARLOCK_SINGLE_CORE = TITAN_170_V2_ONEIPE,
    TITAN_VER_MAX = TITAN_VERSION_MAX,
    VER_SANDBOX = SANDBOX
} ChipVersionStr;

#endif //End _CHIP_INFO_READER_DEFS_H_

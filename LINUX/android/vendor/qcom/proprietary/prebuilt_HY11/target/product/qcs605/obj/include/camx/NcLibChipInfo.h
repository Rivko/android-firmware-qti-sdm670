// NOWHINE ENTIRE FILE 
//----------------------------------------------------------------------------
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//---------------------------------------------------------------------------

#pragma once

#include "chipinforeaderdefs.h"
#include "CommonDefs.h"

/// @brief Holds the necessary HW/chip version.
///        Needs to be set at start and then can be used by other SW components.
class NcLibChipInfo
{
public:
    /// @brief  Required initializer
    /// @param  titanVersion The version of the camera platform
    /// @param  hwVersion    The version of the camera CPAS 
    /// @return None
    static void Set(uint32_t titanVersion, uint32_t hwVersion);

    /// @brief  Camera platform version accessor
    /// @return The version of the camera platform
    static inline TitanVersion GetTitanVersion()
    {
        NCLIB_ASSERT(s_titanVersion != INVALID_TITAN_VERSION);
        return s_titanVersion;
    }

    /// @brief  Camera CPAS version accessor
    /// @return The version of the camera CPAS 
    static inline uint32_t GetHwVersion()
    {
        NCLIB_ASSERT(s_hwVersion != INVALID_CHIP_VERSION);
        return s_hwVersion;
    }

    /// @brief  Tests whether running on Napali
    /// @return True if running on Napali
    static inline bool IsNapali() { return GetTitanVersion() == TITAN_170; }

    /// @brief  Tests whether running on Hana
    /// @return True if running on Hana
    static inline bool IsHana()   { return GetTitanVersion() == TITAN_175; }

    /// @brief  Tests whether running on Kona
    /// @return True if running on Kona
    static inline bool IsKona()   { return GetTitanVersion() == TITAN_480; }

    /// @brief  Tests whether running on Talos
    /// @return True if running on Talos
    static inline bool IsTalos()  { return GetTitanVersion() == TITAN_150; }

private:
    NcLibChipInfo() = delete;

private:
    static const TitanVersion INVALID_TITAN_VERSION = (TitanVersion)0;
    static const ChipVersion  INVALID_CHIP_VERSION = TITAN_VERSION_MAX;

    static TitanVersion s_titanVersion;
    static uint32_t s_hwVersion;
};

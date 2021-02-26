////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chiiqmoduledefines.h
///
/// @brief Qualcomm Technologies, Inc. IQ modules data structure definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIIQMODULEDEFINES_H
#define CHIIQMODULEDEFINES_H

#include "camxcdktypes.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Data structure that are exposed to OEM must be packed to have the expected layout
#pragma pack(push, CDK_PACK_SIZE)

struct abf34_enable_sectionStruct
{
    UINT32    reserved;
    INT32     abf_enable;
    INT32     sbpc_enable;
};

struct chromatix_abf34_reserveType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cross_plane_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable Gr/Gb cross processing in bilateral filtering
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32            cross_plane_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_anchor_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   anchor tables from center to frame boundary
    /// Type:          float
    /// Default Value: 0,0,0,0,0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Length:        5
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_anchor_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_anchor - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0,0,0,0,0
        /// Range:         [0.0, 1.0]
        /// Length:        5
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_anchor[5];
    } radial_anchor_tab;
};

struct abf40_enable_sectionStruct
{
    UINT32 reserved;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bilateral_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bilateral filtering, tier 1 enable
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  bilateral_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// minmax_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable built-in min-max pixel filter, tier 1 enable
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         if BPC module is on, this feature is likely off
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  minmax_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dirsmth_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable Directional Smoothing filter, tier 1 enable
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  dirsmth_en;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_abf40_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_abf40_reserveType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cross_plane_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable Gr/Gb cross processing in bilateral filtering
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  cross_plane_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dark_desat_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable dark desaturation feature, tier 2 enable
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Effective when bilateral_en is on.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  dark_desat_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dark_smooth_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable dark area smoothing feature, tier 2 enable
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Effective when bilateral_en is on.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  dark_smooth_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// act_adj_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable activity based adjustment feature, tier 2 enable
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         It is effective only when both bilateral_en and dirsmth_en are set to 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  act_adj_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blk_opt
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   block matching pattern for R/B
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 2]
    /// Bit Depth:     2u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32             blk_opt;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_anchor_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   anchor tables from center to frame boundary
    /// Type:          float
    /// Default Value: 0,0.4,0.6,0.8,1.0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Length:        5
    /// Conversion:    No Conversion needed
    /// Notes:         rnr_anchor[0] is 0 by default. normally rnr_anchor[4] is 1.0.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_anchor_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_anchor - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0,0.4,0.6,0.8,1.0
        /// Range:         [0.0, 1.0]
        /// Length:        5
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_anchor[5];
    } radial_anchor_tab;
};

struct abf34_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bpc_fmax
    ///
    /// Tuning Level:  OftenTune
    /// Description:   upper offset factor used in single bpc applied to center pixel
    /// Type:          uint
    /// Default Value: 16
    /// Range:         [0, 63]
    /// Bit Depth:     6u, Q6
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  bpc_fmax;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bpc_fmin
    ///
    /// Tuning Level:  OftenTune
    /// Description:   lower offset factor used in single bpc applied to center pixel
    /// Type:          uint
    /// Default Value: 16
    /// Range:         [0, 63]
    /// Bit Depth:     6u, Q6
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  bpc_fmin;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bpc_maxshft
    ///
    /// Tuning Level:  OftenTune
    /// Description:   upper offset (as right-shifting) used in single bpc applied to neighbor pixels
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [0, 15]
    /// Bit Depth:     4u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  bpc_maxshft;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bpc_minshft
    ///
    /// Tuning Level:  OftenTune
    /// Description:   lower offset (as right-shifting) used in single bpc applied to neighbor pixels
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [0, 15]
    /// Bit Depth:     4u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  bpc_minshft;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bpc_offset
    ///
    /// Tuning Level:  OftenTune
    /// Description:   common absolute offset value used in single bpc applied to both center pixel and neighboring pixel
    /// Type:          uint
    /// Default Value: 64
    /// Range:         [0, 4095]
    /// Bit Depth:     12u, Q0
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  bpc_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blk_pix_matching_rb
    ///
    /// Tuning Level:  OftenTune
    /// Description:   R and B channel block matching or pixel matching selection. Value 0 means block matching in determining
    ///                filter coefficient. Value 4 means pixel matching
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 4]
    /// Bit Depth:     3u
    /// Notes:         may use 4 for ultra low light cases
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  blk_pix_matching_rb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blk_pix_matching_g
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Gr and Gb channel block matching or pixel matching selection. Value 0 means block matching in determining
    ///                filter coefficient. Value 4 means pixel matching
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 4]
    /// Bit Depth:     3u
    /// Notes:         may use 4 for ultra low light cases
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  blk_pix_matching_g;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_stdlut_level_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   noise profile in standard deviation
    /// Type:          float
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [0.0, 512.0]
    /// Bit Depth:     float
    /// Length:        65
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_stdlut_level_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_stdlut_level - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [0.0, 512.0]
        /// Length:        65
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_stdlut_level[65];
    } noise_stdlut_level_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// distance_ker_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   bit shifting value corresponding to distance weight; [0][] - R/B, [1][] - Gr/Gb
    /// Type:          uint
    /// Default Value: 3,2,2,3,2,2
    /// Range:         [1,4]
    /// Bit Depth:     [][0]:3bit; othes:2bit
    /// Length:        6
    /// Notes:         non-zero value n means left shift (n-1); zero value means weight of 0. Range 0: 1-4, 1: 1-2, 2: 1-2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct distance_ker_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// distance_ker - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 3,2,2,3,2,2
        /// Range:         [1,4]
        /// Length:        6
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT distance_ker[6];
    } distance_ker_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// curve_offset_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset that is subtracted from normalized difference before generating bilateral coefficients
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 127]
    /// Bit Depth:     7
    /// Length:        4
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct curve_offset_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// curve_offset - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0
        /// Range:         [0, 127]
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT curve_offset[4];
    } curve_offset_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_prsv_anchor_lo
    ///
    /// Tuning Level:  OftenTune
    /// Description:   anchor point below which noise_preserve will be noise_prsv_lo
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Notes:         Range of 0.0 to 1.0 corresponds to intensity level 0 to 1023
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  noise_prsv_anchor_lo;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_prsv_anchor_hi
    ///
    /// Tuning Level:  OftenTune
    /// Description:   anchor point above which noise_preserve will be a flat maximum value
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Notes:         Range of 0.0 to 1.0 corresponds to intensity level 0 to 1023
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  noise_prsv_anchor_hi;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_prsv_lo_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   minimum value for noise preserve, [0] - R/B, [1] - Gr/Gb
    /// Type:          float
    /// Default Value: 0.0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Length:        2
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_prsv_lo_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_prsv_lo - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0.0
        /// Range:         [0.0, 1.0]
        /// Length:        2
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_prsv_lo[2];
    } noise_prsv_lo_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_prsv_hi_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   maximum value for noise preserve, [0] - R/B, [1] - Gr/Gb
    /// Type:          float
    /// Default Value: 0.0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Length:        2
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_prsv_hi_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_prsv_hi - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0.0
        /// Range:         [0.0, 1.0]
        /// Length:        2
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_prsv_hi[2];
    } noise_prsv_hi_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_gain_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   base value for each anchor, [0][] for loc_fac0, [1][] for loc_fac1
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Length:        10
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_gain_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_gain - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0
        /// Range:         [0.0, 1.0]
        /// Length:        10
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_gain[10];
    } radial_gain_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// edge_softness
    ///
    /// Tuning Level:  OftenTune
    /// Description:   control bilateral filter strength together with noise_std_lut
    /// Type:          float
    /// Default Value: 3
    /// Range:         [0.0, 256.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  edge_softness;
};

struct abf40_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// minmax_maxshft
    ///
    /// Tuning Level:  OftenTune
    /// Description:   upper/lower offset (as right-shifting) used in single bpc applied to neighbor pixels
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [0, 15]
    /// Bit Depth:     4u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        minmax_maxshft;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// minmax_minshft
    ///
    /// Tuning Level:  OftenTune
    /// Description:   upper/lower offset (as right-shifting) used in single bpc applied to neighbor pixels
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [0, 15]
    /// Bit Depth:     4u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        minmax_minshft;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// minmax_offset
    ///
    /// Tuning Level:  OftenTune
    /// Description:   common absolute offset value used in single bpc applied to both center pixel and neighboring pixel
    /// Type:          uint
    /// Default Value: 64
    /// Range:         [0, 4095]
    /// Bit Depth:     12u
    /// Conversion:    No Conversion needed
    /// Notes:         Q0 precision
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        minmax_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// minmax_bls
    ///
    /// Tuning Level:  OftenTune
    /// Description:   black level used in single bpc applied to both center pixel and neighboring pixel
    /// Type:          int
    /// Default Value: 64
    /// Range:         [0, 4095]
    /// Bit Depth:     12 bit unsigned for Titan
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        minmax_bls;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blkpix_lev_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   block matching or pixel matching selection. Value 0 means block matching in determining filter
    ///                coefficient. Value 4 means pixel matching
    /// Type:          uint
    /// Default Value: 0 0
    /// Range:         [0, 4]
    /// Bit Depth:     3u
    /// Length:        2
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct blkpix_lev_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// blkpix_lev - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0 0
        /// Range:         [0, 4]
        /// Length:        2
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT blkpix_lev[2];
    } blkpix_lev_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_std_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   noise profile (std) that will be used together with edge_softness to generate noise_std_dev_lut in packed
    ///                format
    /// Type:          float
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [0.0, 512.0]
    /// Bit Depth:     float
    /// Length:        65
    /// Notes:         The above conversion starts with 8996 ABF noise profile
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_std_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_std_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [0.0, 512.0]
        /// Length:        65
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_std_lut[65];
    } noise_std_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// curve_offset_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset that is subtracted from normalized difference before generating bilateral coefficients
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 127]
    /// Bit Depth:      7u
    /// Length:        4
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct curve_offset_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// curve_offset - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0
        /// Range:         [0, 127]
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT curve_offset[4];
    } curve_offset_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dist_ker_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   bit shifting value corresponding to distance weight; [0-2][] – 3 smoothing levels, [][0-2,R/B, 3-5,Gr/Gb]
    ///                non-zero value n means 1-shift-left-(n-1); zero value means weight of 0.
    /// Type:          uint
    /// Default Value: 3 2 1 3 2 1 3 2 1 3 2 1 3 2 1 3 2 1
    /// Range:         [[1,4],[0,2],[0,1],[1,4],[0,2],[0,1],[1,4],[0,2],[0,1],[1,4],[0,2],[0,1],[1,4],[0,2],[0,1],[1,4],[0,2],[0,1]]
    /// Bit Depth:     [][0]:3bit; othes:2bit
    /// Length:        3x6
    /// Notes:         range:[0]:1-4; [1]:0-2; [2]:0-2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct dist_ker_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dist_ker - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 3 2 1 3 2 1 3 2 1 3 2 1 3 2 1 3 2 1
        /// Range:         [[1,4],[0,2],[0,1],[1,4],[0,2],[0,1],[1,4],[0,2],[0,1],[1,4],[0,2],[0,1],[1,4],[0,2],[0,1],[1,4],[0,2],[0,1]]
        /// Length:        18
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT dist_ker[18];
    } dist_ker_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// edge_softness_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   control bilateral filter strength together with noise_std_lut
    /// Type:          float
    /// Default Value: 3.0, 3.0, 3.0, 3.0
    /// Range:         [0.0, 15.99]
    /// Bit Depth:     float
    /// Length:        4
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct edge_softness_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// edge_softness - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 3.0, 3.0, 3.0, 3.0
        /// Range:         [0.0, 15.99]
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT edge_softness[4];
    } edge_softness_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// denoise_strength_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   higher means more denoising, one for each channel
    /// Type:          float
    /// Default Value: 1.0, 1.0, 1.0, 1.0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:
    /// Length:        4
    /// Notes:         Final_slope = (1 - noise_prsv * radial_adj * act_adj) * denoise_scale
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct denoise_strength_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// denoise_strength - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 1.0, 1.0, 1.0, 1.0
        /// Range:         [0.0, 1.0]
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT denoise_strength[4];
    } denoise_strength_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_prsv_anchor_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   anchor points for noise_preserve
    /// Type:          float
    /// Default Value: 0.0, 0.06227106, 0.12478632, 0.85470085, 1.0
    /// Range:         [0, 1.0]
    /// Bit Depth:     float
    /// Length:        5
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_prsv_anchor_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_prsv_anchor - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0.0, 0.06227106, 0.12478632, 0.85470085, 1.0
        /// Range:         [0, 1.0]
        /// Length:        5
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_prsv_anchor[5];
    } noise_prsv_anchor_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_prsv_base_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   base value for noise preserve, [0][] - R/B, [1][] - Gr/Gb
    /// Type:          float
    /// Default Value: 0.19607843, 0.2, 0.20392157, 0.20784314, 0.21176471, 0.19607843, 0.2, 0.20392157, 0.20784314, 0.21176471
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Length:        10
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_prsv_base_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_prsv_base - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0.19607843, 0.2, 0.20392157, 0.20784314, 0.21176471, 0.19607843, 0.2, 0.20392157, 0.20784314, 0.21176471
        /// Range:         [0.0, 1.0]
        /// Length:        10
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_prsv_base[10];
    } noise_prsv_base_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_edge_softness_adj_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   base value for rnr, used for bilateral
    /// Type:          float
    /// Default Value: 1.0, 1.015625, 1.171875, 1.875, 2.78125
    /// Range:         [1.0, 15.99]
    /// Bit Depth:     float
    /// Length:        5
    /// Notes:         increasing table
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_edge_softness_adj_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_edge_softness_adj - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 1.0, 1.015625, 1.171875, 1.875, 2.78125
        /// Range:         [1.0, 15.99]
        /// Length:        5
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_edge_softness_adj[5];
    } radial_edge_softness_adj_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_noise_prsv_adj_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   base value for rnr, used for soft-thresholding
    /// Type:          float
    /// Default Value: 1.0, 0.9765625, 0.8203125, 0.5859375, 0.09375
    /// Range:         [0, 1.0]
    /// Bit Depth:     float
    /// Length:        5
    /// Notes:         decreasing table
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_noise_prsv_adj_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_noise_prsv_adj - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 1.0, 0.9765625, 0.8203125, 0.5859375, 0.09375
        /// Range:         [0, 1.0]
        /// Length:        5
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_noise_prsv_adj[5];
    } radial_noise_prsv_adj_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// act_fac_lut_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   LUT for activity adjust
    /// Type:          uint
    /// Default Value: 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256
    /// Range:         [0, 256]
    /// Bit Depth:     9u
    /// Length:        32
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct act_fac_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// act_fac_lut - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256 256
        /// Range:         [0, 256]
        /// Length:        32
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT act_fac_lut[32];
    } act_fac_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// act_fac0
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for activity adjust
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 8191]
    /// Bit Depth:     13u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        act_fac0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// act_fac1
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for activity adjust
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 8191]
    /// Bit Depth:     13u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        act_fac1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// act_thd0
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for activity adjust
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [1, 8191]
    /// Bit Depth:     13u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        act_thd0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// act_thd1
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for activity adjust
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 4095]
    /// Bit Depth:     13u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        act_thd1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// act_smth_thd0
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for activity adjust
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        act_smth_thd0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// act_smth_thd1
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for activity adjust
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        act_smth_thd1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dark_thd
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for activity adjust
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 4095]
    /// Bit Depth:     12u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        dark_thd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dark_fac_lut_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for dark_factor adjustment
    /// Type:          uint
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [0, 256]
    /// Bit Depth:     9u
    /// Length:        42
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct dark_fac_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dark_fac_lut - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [0, 256]
        /// Length:        42
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT dark_fac_lut[42];
    } dark_fac_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// edge_detect_thd
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for edge_detection
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 15]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        edge_detect_thd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// edge_count_low
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for edge_detection
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 16]
    /// Bit Depth:     7u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        edge_count_low;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// edge_detect_noise_scaler
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for edge_detection
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0, 63.99]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        edge_detect_noise_scaler;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// edge_smooth_strength
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for edge_detection
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0, 1.0]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                        edge_smooth_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// edge_smooth_scaler_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   parameter for edge_detection
    /// Type:          float
    /// Default Value: 0 0 0 0
    /// Range:         [0, 15.99]
    /// Bit Depth:     float
    /// Length:        4
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct edge_smooth_scaler_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// edge_smooth_scaler - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0 0 0 0
        /// Range:         [0, 15.99]
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT edge_smooth_scaler[4];
    } edge_smooth_scaler_tab;
};

struct anr10_rgn_dataType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// luma_chroma_filter_config
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct luma_chroma_filter_configStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// threshold_lut_control_avg_block_size
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   yuv average block size for controling the threshold lut entry. . 0-Y3x3/UV1x1 threshold avg support.
        ///                1-Y5x5/UV3x3 threshold avg support. 2-Y9x9/UV5x5 threshold avg support.
        /// Type:          uint
        /// Default Value: 2
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT threshold_lut_control_avg_block_size;
    } luma_chroma_filter_config;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// luma_filter_config
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct luma_filter_configStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// filter_isotropic_min_filter_size
        ///
        /// Tuning Level:  OftenTune
        /// Description:   0 - normal mode, each distance depends on distences shorter than him and himself. ( == Isotropic mode 1x1
        ///                (minimum filter size) ). 1 - Isotropic mode 3x3 (minimum filter size). 2 - Isotropic mode 5x5 (minimum
        ///                filter size). 3 - Isotropic mode 7x7 (minimum filter size). 4 - Isotropic mode 9x9 (minimum filter size).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT filter_isotropic_min_filter_size;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// filter_manual_derivatives_flags
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Condition flags for calculating the luma filtering. Di_j == 0 means that filtering of length i is
        ///                controlled by detection of length j. (0-consider, 1-ignore). D3_3 StartBit:0 Default:0. D3_5 StartBit:1
        ///                Default:1. D3_5X StartBit:2 Default:1. D3_7 StartBit:3 Default:1. D3_9 StartBit:4 Default:1. D3_9X
        ///                StartBit:5 Default:1. D5_3 StartBit:6 Default:0. D5_5 StartBit:7 Default:0. D5_5X StartBit:8 Default:0.
        ///                D5_7 StartBit:9 Default:1. D5_9 StartBit:10 Default:1. D5_9X StartBit:11 Default:1. D7_3 StartBit:12
        ///                Default:0. D7_5 StartBit:13 Default:0. D7_5X StartBit:14 Default:0. D7_7 StartBit:15 Default:0. D7_9
        ///                StartBit:16 Default:1. D7_9X StartBit:17 Default:1. D9_3 StartBit:18 Default:0. D9_5 StartBit:19
        ///                Default:0. D9_5X StartBit:20 Default:0. D9_7 StartBit:21 Default:0. D9_9 StartBit:22 Default:0. D9_9X
        ///                StartBit:23 Default:0.
        /// Type:          uint
        /// Default Value: 9654078
        /// Range:         [0, 16777215]
        /// Bit Depth:     24u
        /// Conversion:    No Conversion needed
        /// Notes:         Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT filter_manual_derivatives_flags;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dcind_isotropic_min_size
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   0 - normal mode, each distance depends on distences shorter than him and himself. ( == Isotropic mode 1x1
        ///                (minimum dc indication size) ). 1 - Isotropic mode 3x3 (minimum dc indication size). 2 - Isotropic mode
        ///                5x5 (minimum dc indication size). 3 - Isotropic mode 7x7 (minimum dc indication size). 4 - Isotropic mode
        ///                9x9 (minimum dc indication size).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT dcind_isotropic_min_size;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dcind_manual_derivatives_flags
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Condition flags for calculating the luma dc indications. Di_j == 0 means that dc indication of length i is
        ///                controlled by detection of length j. (0-consider, 1-ignore). D3_3 StartBit:0 Default:0. D3_5 StartBit:1
        ///                Default:1. D3_5X StartBit:2 Default:1. D3_7 StartBit:3 Default:1. D3_9 StartBit:4 Default:1. D3_9X
        ///                StartBit:5 Default:1. D5_3 StartBit:6 Default:0. D5_5 StartBit:7 Default:0. D5_5X StartBit:8 Default:0.
        ///                D5_7 StartBit:9 Default:1. D5_9 StartBit:10 Default:1. D5_9X StartBit:11 Default:1. D7_3 StartBit:12
        ///                Default:0. D7_5 StartBit:13 Default:0. D7_5X StartBit:14 Default:0. D7_7 StartBit:15 Default:0. D7_9
        ///                StartBit:16 Default:1. D7_9X StartBit:17 Default:1. D9_3 StartBit:18 Default:0. D9_5 StartBit:19
        ///                Default:0. D9_5X StartBit:20 Default:0. D9_7 StartBit:21 Default:0. D9_9 StartBit:22 Default:0. D9_9X
        ///                StartBit:23 Default:0.
        /// Type:          uint
        /// Default Value: 9654078
        /// Range:         [0, 16777215]
        /// Bit Depth:     24u
        /// Conversion:    No Conversion needed
        /// Notes:         Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT dcind_manual_derivatives_flags;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// second_derivative_max_influence_radius_filtering
        ///
        /// Tuning Level:  NeverTune
        /// Description:   Controls the influence radius of the 2nd derivative on the filtering (consider der2 evaluation upto this
        ///                radius). Demux to registers: FULL_IPU_CYLPF_YFilter_Der2FlagsDCInd_D3_*.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT second_derivative_max_influence_radius_filtering;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// second_derivative_max_influence_radius_dc_indication
        ///
        /// Tuning Level:  NeverTune
        /// Description:   Controls the influence radius of the 2nd derivative on the DC Indications (consider der2 evaluation upto
        ///                this radius). Demux to registers: FULL_IPU_CYLPF_YFilter_Der2FlagsFilt_D3_3*.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT second_derivative_max_influence_radius_dc_indication;
    } luma_filter_config;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chroma_filter_config
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct chroma_filter_configStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// filter_isotropic_min_filter_size
        ///
        /// Tuning Level:  OftenTune
        /// Description:   0 - normal mode, each distance depends on distences shorter than him and himself. ( == Isotropic mode 1x1
        ///                (minimum filter size) ). 1 - Isotropic mode 3x3 (minimum filter size). 2 - Isotropic mode 5x5 (minimum
        ///                filter size). 3 - Isotropic mode 7x7 (minimum filter size).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT filter_isotropic_min_filter_size;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// filter_manual_derivatives_flags
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Condition flags for calculating the chroma filtering. Di_j == 0 means that filtering of length i is
        ///                controlled by detection of length j. (0-consider, 1-ignore). D3_3 StartBit:0 Default:0. D3_5 StartBit:1
        ///                Default:1. D3_5X StartBit:2 Default:1. D3_7 StartBit:3 Default:1. D5_3 StartBit:6 Default:0. D5_5
        ///                StartBit:7 Default:0. D5_5X StartBit:8 Default:1. D5_7 StartBit:9 Default:1. D7_3 StartBit:12 Default:0.
        ///                D7_5 StartBit:13 Default:0. D7_5X StartBit:14 Default:1. D7_7 StartBit:15 Default:0.
        /// Type:          uint
        /// Default Value: 20286
        /// Range:         [0, 65535]
        /// Bit Depth:     16u
        /// Conversion:    No Conversion needed
        /// Notes:         Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT filter_manual_derivatives_flags;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dcind_isotropic_min_size
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   0 - normal mode, each distance depends on distences shorter than him and himself. ( == Isotropic mode 1x1
        ///                (minimum dc indication size) ). 1 - Isotropic mode 3x3 (minimum dc indication size). 2 - Isotropic mode
        ///                5x5 (minimum dc indication size). 3 - Isotropic mode 7x7 (minimum dc indication size). 4 - Isotropic mode
        ///                9x9 (minimum dc indication size). 5 - Isotropic mode 11x11 (minimum dc indication size).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT dcind_isotropic_min_size;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dcind_manual_derivatives_flags
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Condition flags for calculating the chroma dc indications. Di_j == 0 means that dc indication of length i
        ///                is controlled by detection of length j. (0-consider, 1-ignore). D3_3 StartBit:0 Default:0. D3_5 StartBit:1
        ///                Default:1. D3_5X StartBit:2 Default:1. D3_7 StartBit:3 Default:1. D5_3 StartBit:6 Default:0. D5_5
        ///                StartBit:7 Default:0. D5_5X StartBit:8 Default:1. D5_7 StartBit:9 Default:1. D7_3 StartBit:12 Default:0.
        ///                D7_5 StartBit:13 Default:0. D7_5X StartBit:14 Default:1. D7_7 StartBit:15 Default:0.
        /// Type:          uint
        /// Default Value: 20286
        /// Range:         [0, 65535]
        /// Bit Depth:     16u
        /// Conversion:    No Conversion needed
        /// Notes:         Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT dcind_manual_derivatives_flags;
    } chroma_filter_config;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// luma_filter_kernel
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct luma_filter_kernelStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// edge_kernel_size
        ///
        /// Tuning Level:  MustTune
        /// Description:   size of edge filter, configured both for autoconf and manual configuration. 0 - 1x1 ( luma filter bypass).
        ///                1 - 3x3. 2 - 5x5. 3 - 7x7. 4 - 9x9.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT edge_kernel_size;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// automatic_definition_granularity
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   luma filter kernel non manual mode, ratio between center and sides. Lower granularity is more smoothing
        ///                kernel (more rectangle). Higher granularity is less smoothing kernel (more picky).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Floor Interpolation: Do Linear Interpolation and then Floor.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT automatic_definition_granularity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_1x1_center_coefficient
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter middle pixel weight.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [4, 47]
        /// Bit Depth:     6u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_1x1_center_coefficient;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_3x3_horver_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 3x3 horizontal/vertical pixel shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 -
        ///                Weight 2. 2 - Weight 4. 3 - Weight 8.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_3x3_horver_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_3x3_diag_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 3x3 diagonal pixel shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 -
        ///                Weight 4. 3 - Weight 8.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_3x3_diag_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_5x5_horver_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 5x5 horizontal/vertical shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2
        ///                - Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_5x5_horver_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_5x5_diag_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 5x5 diagonal pixel weights. 0 - Weight 1. 1 - Weight 2. 2 - Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_5x5_diag_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_5x5_complement_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 5x5 off-diagonal (complementary) shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 -
        ///                Weight 2. 2 - Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_5x5_complement_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_7x7_horver_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter horizontal/vertical pixel shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2.
        ///                2 - Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_7x7_horver_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_7x7_diag_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter diagonal pixel shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 - Weight
        ///                4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_7x7_diag_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_7x7_complement_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 7x7 off-diagonal shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 -
        ///                Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_7x7_complement_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_9x9_horver_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter horizontal/vertical shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 -
        ///                Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_9x9_horver_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_9x9_diag_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 9x9 diagonal shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 - Weight
        ///                4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_9x9_diag_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_edge_kernel_9x9_complement_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 9x9 off-diagonal shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 -
        ///                Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_edge_kernel_9x9_complement_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_blend_weight
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Max Blend Weight of the flat kernel in regards to Edge kernel. (blend is linear as a function of pixel
        ///                smoothness).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_blend_weight;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_size
        ///
        /// Tuning Level:  OftenTune
        /// Description:   0 - 1x1 (luma filter bypass). 1 - 3x3. 2 - 5x5. 3 - 7x7. 4 - 9x9.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_size;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_1x1_center_coefficient
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter middle pixel weight.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [4, 47]
        /// Bit Depth:     6u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_1x1_center_coefficient;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_3x3_horver_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 3x3 horizontal/vertical pixel shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 -
        ///                Weight 2. 2 - Weight 4. 3 - Weight 8.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_3x3_horver_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_3x3_diag_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 3x3 diagonal pixel shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 -
        ///                Weight 4. 3 - Weight 8.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_3x3_diag_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_5x5_horver_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 5x5 horizontal/vertical shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2
        ///                - Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_5x5_horver_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_5x5_diag_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 5x5 diagonal pixel shift weights. Actual weight is 2^shift . 0 - Weight 1. 1 - Weight 2. 2 -
        ///                Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_5x5_diag_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_5x5_complement_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 5x5 off-diagonal (complementary) shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 -
        ///                Weight 2. 2 - Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_5x5_complement_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_7x7_horver_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 7x7 horizontal/vertical pixel shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 -
        ///                Weight 2. 2 - Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_7x7_horver_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_7x7_diag_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 7x7 diagonal pixel shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 -
        ///                Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_7x7_diag_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_7x7_complement_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 7x7 off-diagonal shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 -
        ///                Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_7x7_complement_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_9x9_horver_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter horizontal/vertical shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 -
        ///                Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_9x9_horver_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_9x9_diag_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 9x9 diagonal shift weights. Actual weight is 2^shift. 0 - Weight 1. 1 - Weight 2. 2 - Weight
        ///                4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_9x9_diag_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// manual_flat_kernel_9x9_complement_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   luma filter 9x9 off-diagonal shift weights. Actual weight is 2^shift . 0 - Weight 1. 1 - Weight 2. 2 -
        ///                Weight 4.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT manual_flat_kernel_9x9_complement_shift;
    } luma_filter_kernel;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chroma_filter_kernel
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct chroma_filter_kernelStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// kernel_size
        ///
        /// Tuning Level:  MustTune
        /// Description:   0 - 1x1 (YLPF1 bypass). 1 - 3x3. 2 - 5x5. 3 - 7x7.
        /// Type:          uint
        /// Default Value: 3
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT kernel_size;
    } chroma_filter_kernel;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// luma_peak_management
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct luma_peak_managementStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_hard_decision_environment_activity
        ///
        /// Tuning Level:  OftenTune
        /// Description:   The maximum environment activity of the pixel (the difference between the maximum and minimum Y component
        ///                values in the pixel neighborhood, . excluding the candidate pixel) is less than LUMA_DCTHRSH. Decreasing
        ///                this parameter results in fewer peak identifications. 0 = Condition is disabled.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_hard_decision_environment_activity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_hard_decision_distance_from_average
        ///
        /// Tuning Level:  OftenTune
        /// Description:   The difference between the Y component of the candidate pixel and the average Y component value of its .
        ///                neighboring pixels (excluding candidate pixel) is at least LUMA_DCLEVEL. . Increasing this parameter
        ///                results in fewer peak identifications. 255 = Condition is disabled.
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_hard_decision_distance_from_average;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_soft_decision_distance_from_average_lower_limit
        ///
        /// Tuning Level:  OftenTune
        /// Description:   soft decision function for distance_from_avg vs activity. . distance_from_avg (for peak) is greater or
        ///                equal than min ( max ( ( (slope * activity ) + offset , lower_limit ) , 255);.
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_soft_decision_distance_from_average_lower_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_soft_decision_distance_from_average_offset
        ///
        /// Tuning Level:  OftenTune
        /// Description:   soft decision function for distance_from_avg vs activity. . distance_from_avg (for peak) is greater or
        ///                equal than min ( max ( ( (slope * activity ) + offset , lower_limit ) , 255);. 255 is disabled.
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_soft_decision_distance_from_average_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_soft_decision_distance_from_average_slope
        ///
        /// Tuning Level:  OftenTune
        /// Description:   soft decision function for distance_from_avg vs activity. . distance_from_avg (for peak) is greater or
        ///                equal than min ( max ( ( (slope * activity ) + offset , lower_limit ) , 255);.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1023]
        /// Bit Depth:     10u, Q5
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_soft_decision_distance_from_average_slope;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_extreme_decision_distance_from_maxmin
        ///
        /// Tuning Level:  OftenTune
        /// Description:   The maximum difference between the Y component of the candidate pixel and the Y component of its
        ///                neighboring pixels is at least detect_extreme_decision_distance_from_maxmin. . Increasing this parameter
        ///                results in fewer peak identifications. Max( pixel_val - env_max, env_min-pixel_val ). 255 = Condition is
        ///                disabled.
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_extreme_decision_distance_from_maxmin;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_dcsize_decision_sensitivity
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Rank peak identification flags a peak when a sufficient number of neighbors around a . pixel are
        ///                identified as flat (regardless of whether the current pixel is flat or not). The following parameters
        ///                control the minimum number of flat neighbors for identifying a peak in each length. 128 is all neighbores.
        ///                129 is disable.
        /// Type:          uint
        /// Default Value: 129
        /// Range:         [0, 129]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_dcsize_decision_sensitivity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// correction_mode
        ///
        /// Tuning Level:  OftenTune
        /// Description:   0 - Correct using area smart selection. (closest pixel value in the 3x3 environment). 1 - Correct using
        ///                flattest direction. area smart is recommended. This parameter should be modified only in FULL pass. DC4,
        ///                DC16,DC64 Passes should always use area smart selection.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT correction_mode;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// correction_area_smart_min_inner_distance
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Area smart selection can be made more robust to larger peaks by increasing correction_nl_mode_thr. This
        ///                value will result taking a pixel value closer to the median of 3x3. 0 - take the closest pixel value. 3-
        ///                take the median.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT correction_area_smart_min_inner_distance;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// correction_isotropic_activity_threshold
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Non edge-preserving is enabled based on following thresholds. Peaks with environment activity below these
        ///                thresholds are corrected using non edge-preserving filtering. Peaks with environment activity above these
        ///                thresholds are corrected using edge preserving filtering. (either area smart or flattest direction based
        ///                on user settings of correction_mode ).
        /// Type:          uint
        /// Default Value: 3
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT correction_isotropic_activity_threshold;
    } luma_peak_management;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chroma_peak_management
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct chroma_peak_managementStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_hard_decision_environment_activity
        ///
        /// Tuning Level:  OftenTune
        /// Description:   The maximum environment activity of the pixel (the difference between the maximum and minimum UV component
        ///                values in the pixel neighborhood, . excluding the candidate pixel) is less than DCTHRSH. Decreasing this
        ///                parameter results in fewer peak identifications. 0 = Condition is disabled.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_hard_decision_environment_activity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_hard_decision_distance_from_average
        ///
        /// Tuning Level:  OftenTune
        /// Description:   The difference between the Y component of the candidate pixel and the average UV component value of its .
        ///                neighboring pixels (excluding candidate pixel) is at least DCLEVEL. . Increasing this parameter results in
        ///                fewer peak identifications. 255 = Condition is disabled.
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_hard_decision_distance_from_average;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_soft_decision_distance_from_average_lower_limit
        ///
        /// Tuning Level:  OftenTune
        /// Description:   soft decision function for distance_from_avg vs activity. . distance_from_avg (for peak) is greater or
        ///                equal than min ( max ( ( (slope * activity ) + offset , lower_limit ) , 255);.
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_soft_decision_distance_from_average_lower_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_soft_decision_distance_from_average_offset
        ///
        /// Tuning Level:  OftenTune
        /// Description:   soft decision function for distance_from_avg vs activity. . distance_from_avg (for peak) is greater or
        ///                equal than min ( max ( ( (slope * activity ) + offset , lower_limit ) , 255);.
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_soft_decision_distance_from_average_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_soft_decision_distance_from_average_slope
        ///
        /// Tuning Level:  OftenTune
        /// Description:   soft decision function for distance_from_avg vs activity. . distance_from_avg (for peak) is greater or
        ///                equal than min ( max ( ( (slope * activity ) + offset , lower_limit ) , 255);.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1023]
        /// Bit Depth:     10u, Q5
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_soft_decision_distance_from_average_slope;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_extreme_decision_distance_from_maxmin
        ///
        /// Tuning Level:  OftenTune
        /// Description:   The maximum difference between the Y component of the candidate pixel and the Y component of its
        ///                neighboring pixels is at least detect_extreme_decision_distance_from_maxmin. . Increasing this parameter
        ///                results in fewer peak identifications. Max( pixel_val - env_max, env_min-pixel_val ). 255 = Condition is
        ///                disabled.
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_extreme_decision_distance_from_maxmin;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_dcsize_decision_sensitivity
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Rank peak identification flags a peak when a sufficient number of neighbors around a . pixel are
        ///                identified as flat (regardless of whether the current pixel is flat or not). The following parameters
        ///                control the minimum number of flat neighbors for identifying a peak in each length. 128 is all neighbores.
        ///                129 is disable.
        /// Type:          uint
        /// Default Value: 129
        /// Range:         [0, 129]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_dcsize_decision_sensitivity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// correction_mode
        ///
        /// Tuning Level:  OftenTune
        /// Description:   0 - Correct using area smart selection. (closest pixel value in the 3x3 environment). 1 - Correct using
        ///                flattest direction. area smart is recommended. This parameter should be modified only in FULL pass. DC4,
        ///                DC16,DC64 Passes should always use area smart selection.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT correction_mode;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// correction_area_smart_min_inner_distance
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Area smart selection can be made more robust to larger peaks by increasing correction_nl_mode_thr. This
        ///                value will result taking a pixel value closer to the median of 3x3. 0 - take the closest pixel value. 3-
        ///                take the median.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT correction_area_smart_min_inner_distance;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// correction_isotropic_activity_threshold
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Non edge-preserving is enabled based on following thresholds. Peaks with environment activity below these
        ///                thresholds are corrected using non edge-preserving filtering. Peaks with environment activity above these
        ///                thresholds are corrected using edge preserving filtering. (either area smart or flattest direction based
        ///                on user settings of correction_mode ).
        /// Type:          uint
        /// Default Value: 3
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT correction_isotropic_activity_threshold;
    } chroma_peak_management;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// inter_length_thr_modification
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct inter_length_thr_modificationStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_input_indication_thr_modification_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   The strength is the maximal thresh gain factor on current PASS assuming fully flat pixel detected by input
        ///                indication from the lower pass. The thresh gain factor for every pixel will be based on the maximal value
        ///                and the flatness of the pixel detected by indication of the lower pass. . 7Q4, example 16 == X1. Actual
        ///                Possible Values are: [16,17,18,20,24,28,30,32,40,48,56,64,72,80] ( == X [ 1, 1.0625, 1.125, 1.25, 1.5,
        ///                1.75, 1.875, 2, 2.5, 3, 3.5, 4, 4.5, 5 ] ). all other values will be quantized later to one of the Actual
        ///                Possible Values.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [16, 80]
        /// Bit Depth:     7u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT luma_input_indication_thr_modification_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_input_indication_thr_modification_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   The strength is the maximal thresh gain factor on current PASS assuming fully flat pixel detected by input
        ///                indication from the lower pass. The thresh gain factor for every pixel will be based on the maximal value
        ///                and the flatness of the pixel detected by indication of the lower pass. . 7Q4, example 16 == X1. Actual
        ///                Possible Values are: [16,17,18,20,24,28,30,32,40,48,56,64,72,80] ( == X [ 1, 1.0625, 1.125, 1.25, 1.5,
        ///                1.75, 1.875, 2, 2.5, 3, 3.5, 4, 4.5, 5 ] ). all other values will be quantized later to one of the Actual
        ///                Possible Values.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [16, 80]
        /// Bit Depth:     7u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT chroma_input_indication_thr_modification_scale;
    } inter_length_thr_modification;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// inter_length_output_indication
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct inter_length_output_indicationStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_center_binarization_minflatval
        ///
        /// Tuning Level:  NeverTune
        /// Description:   in case center pixel is flatter than center_binarization_minflatval it will indicate the pixel as flat
        ///                without any regard to neighbores. If not, the decision will be done according to neighbores decision.
        /// Type:          uint
        /// Default Value: 2
        /// Range:         [1, 5]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT luma_center_binarization_minflatval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_neighbours_binarization_minflatval
        ///
        /// Tuning Level:  NeverTune
        /// Description:   neighbors are counted as flat if they are flatter than neighbours_binarization_minflatval.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [1, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT luma_neighbours_binarization_minflatval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_neighbours_parallel_dist
        ///
        /// Tuning Level:  NeverTune
        /// Description:   perpendicular neighbourhood size can be 3 or 4. 1- size =3 . 2- size =5.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [1, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT luma_neighbours_parallel_dist;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_neighbours_perpendicular_dist
        ///
        /// Tuning Level:  NeverTune
        /// Description:   perpendicular neighbourhood size can be 1 or 3. 0- size =1 . 1- size =3.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT luma_neighbours_perpendicular_dist;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_neighbours_agreement_sensitivity
        ///
        /// Tuning Level:  NeverTune
        /// Description:   if percentage of neighbores are determind as flat (according to comparison with
        ///                neighbours_binarization_minflatval) is greater than neighbours_agreement_sensitivity, center pixel will be
        ///                indicated as flat for higher pass.
        /// Type:          uint
        /// Default Value: 80
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT luma_neighbours_agreement_sensitivity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_center_binarization_minflatval
        ///
        /// Tuning Level:  NeverTune
        /// Description:   in case center pixel is flatter than center_binarization_minflatval it will indicate the pixel as flat
        ///                without any regard to neighbores. If not, the decision will be done according to neighbores decision.
        /// Type:          uint
        /// Default Value: 2
        /// Range:         [1, 6]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT chroma_center_binarization_minflatval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_neighbours_binarization_minflatval
        ///
        /// Tuning Level:  NeverTune
        /// Description:   neighbors are counted as flat if they are flatter than neighbours_binarization_minflatval.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [1, 5]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT chroma_neighbours_binarization_minflatval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_neighbours_perpendicular_dist
        ///
        /// Tuning Level:  NeverTune
        /// Description:   perpendicular neighbourhood size can be 1 or 3. 0- size =1 . 1- size =3.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT chroma_neighbours_perpendicular_dist;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_neighbours_agreement_sensitivity
        ///
        /// Tuning Level:  NeverTune
        /// Description:   if percentage of neighbores are determind as flat (according to comparison with
        ///                neighbours_binarization_minflatval) is greater than neighbours_agreement_sensitivity, center pixel will be
        ///                indicated as flat for higher pass.
        /// Type:          uint
        /// Default Value: 80
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT chroma_neighbours_agreement_sensitivity;
    } inter_length_output_indication;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// grey_treatment
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct grey_treatmentStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// enable_grey_treatment_dcblend2_chroma_modification
        ///
        /// Tuning Level:  MustTune
        /// Description:   dcblend2 will blend as a function of max(greyDCSize,cDCSize) instead of just cDCSize in case that the
        ///                chromaticity of the lower pass is lower than the chromaticity of the current pass for the current pixel.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT enable_grey_treatment_dcblend2_chroma_modification;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_grey_condition_chromaticity_radius
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   chromaticity is estimated using : 0 - 3x3 circle, 1- 5x5 circle.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_grey_condition_chromaticity_radius;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_grey_condition_chromaticity_thr_low
        ///
        /// Tuning Level:  MustTune
        /// Description:   Define the conditions for identifying areas as grey - chroma threshold p1 (100% weight), Linear in between
        ///                p1,p2 (start,end).
        /// Type:          uint
        /// Default Value: 2
        /// Range:         [0, 182]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_grey_condition_chromaticity_thr_low;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_grey_condition_chromaticity_thr_high
        ///
        /// Tuning Level:  MustTune
        /// Description:   Define the conditions for identifying areas as grey - chroma threshold p2 (0% weight), Linear in between
        ///                p1,p2 (start,end).
        /// Type:          uint
        /// Default Value: 3
        /// Range:         [0, 182]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_grey_condition_chromaticity_thr_high;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_grey_condition_y_max_derivative_radius
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   y max derivative is calculated using : 0- 3x3 , 1- 5x5.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_grey_condition_y_max_derivative_radius;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_grey_condition_y_max_derivative_thr_low
        ///
        /// Tuning Level:  MustTune
        /// Description:   Define the conditions for identifying areas as grey - derivative threshold p1 (0% weight), Linear in
        ///                between p1,p2 (start,end).
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_grey_condition_y_max_derivative_thr_low;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_grey_condition_y_max_derivative_thr_high
        ///
        /// Tuning Level:  MustTune
        /// Description:   Define the conditions for identifying areas as grey - derivative threshold p2 (100% weight), Linear in
        ///                between p1,p2 (start,end).
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_grey_condition_y_max_derivative_thr_high;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_greydcsize_neighbors_chromaticity_thr
        ///
        /// Tuning Level:  MustTune
        /// Description:   pixels Neighborhood should have chromaticity lower than this threshold in order to be defined as grey for
        ///                the calculation of the greydcsize.
        /// Type:          uint
        /// Default Value: 3
        /// Range:         [0, 182]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_greydcsize_neighbors_chromaticity_thr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_greydcsize_center_detail_chromaticity_thr
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   if pixel has a higher chromaticity than this threshold it will be defined a chromatc detail and will set
        ///                greydcsize to zero.
        /// Type:          uint
        /// Default Value: 30
        /// Range:         [0, 182]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT detect_greydcsize_center_detail_chromaticity_thr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// thr_modification_target_y
        ///
        /// Tuning Level:  MustTune
        /// Description:   Increase the chroma filter thresholds prior to the CLPF filtering algorithm. If target_y,u,v ==0 then it
        ///                wont change the thresholds, actually if the target threshold is less than base threshold, then no change
        ///                will be made.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT thr_modification_target_y;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// thr_modification_target_u
        ///
        /// Tuning Level:  MustTune
        /// Description:   Increase the chroma filter thresholds prior to the CLPF filtering algorithm. If target_y,u,v ==0 then it
        ///                wont change the thresholds, actually if the target threshold is less than base threshold, then no change
        ///                will be made.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT thr_modification_target_u;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// thr_modification_target_v
        ///
        /// Tuning Level:  MustTune
        /// Description:   Increase the chroma filter thresholds prior to the CLPF filtering algorithm. If target_y,u,v ==0 then it
        ///                wont change the thresholds, actually if the target threshold is less than base threshold, then no change
        ///                will be made.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT thr_modification_target_v;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// isotropic_filter_blend_factor_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   controls how much blend will be done with the isotropic chroma filtering upto 7x7, depends on GreyDCSize
        ///                and Isotropic_Filter_Size. blend (with isotropic filtering 7x7) factor fucntion scale as a function of
        ///                grey edge detection. Function values are 0:64 and as a function of grey edge detection which is also 0:64.
        ///                scale values are Q4, 7Q4, example 16 == X1 slope. to disable blend with isotropic filter :
        ///                isotropic_filter_blend_factor_scale == isotropic_filter_blend_factor_offset == 0 .
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT isotropic_filter_blend_factor_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// isotropic_filter_blend_factor_offset
        ///
        /// Tuning Level:  MustTune
        /// Description:   controls how much blend will be done with the isotropic chroma filtering upto 7x7, depends on GreyDCSize
        ///                and Isotropic_Filter_Size. blend (with isotropic filtering 7x7) factor fucntion scale as a function of
        ///                grey edge detection. Function values are 0:64 and as a function of grey edge detection which is also 0:64.
        ///                to disable blend with isotropic filter : isotropic_filter_blend_factor_scale ==
        ///                isotropic_filter_blend_factor_offset == 0 .
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-64, 63]
        /// Bit Depth:     7s
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT isotropic_filter_blend_factor_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// isotropic_filter_size
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   0- isotropic filter is 3x3. 1- isotropic filter is 5x5. 2- isotropic filter is adaptive (1x1/3x3/5x5/7x7)
        ///                depends on GreyDCSize (0/1/2/3). 3- isotropic filter is 7x7.
        /// Type:          uint
        /// Default Value: 2
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT isotropic_filter_size;
    } grey_treatment;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chroma_filter_extension
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct chroma_filter_extensionStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_override_detail_condition_chromaticity_thr
        ///
        /// Tuning Level:  MustTune
        /// Description:   The maximum chromaticity value for which details are disabled.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 182]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_override_detail_condition_chromaticity_thr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_override_detail_condition_y_max_derivative_thr
        ///
        /// Tuning Level:  MustTune
        /// Description:   The minimum luma derivative value for which details are disabled.
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_override_detail_condition_y_max_derivative_thr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_corner_detail_sensitivity_y
        ///
        /// Tuning Level:  MustTune
        /// Description:   The local detail sensitivity controls the local conditions logic: . High value - more local details are
        ///                identified in the image ( = less filtering is performed) . Low value - less local details are identified
        ///                in the image ( = more filtering is performed). this will multiply the chroma x modifers (scale and
        ///                offset). Q4 example 16 == X1. 65 to disable median filter.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 65]
        /// Bit Depth:     7u, Q5
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_corner_detail_sensitivity_y;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_corner_detail_sensitivity_uv
        ///
        /// Tuning Level:  MustTune
        /// Description:
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 64]
        /// Bit Depth:     7u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_corner_detail_sensitivity_uv;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_triple_chromaticities_detail_thr_up_down
        ///
        /// Tuning Level:  MustTune
        /// Description:   Threshold for chroma detail detection, chromaticity difference is calculated from chromaticity 8bits.
        ///                Higher chromaticity difference than this value is a detail.
        /// Type:          uint
        /// Default Value: 8
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_triple_chromaticities_detail_thr_up_down;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_triple_chromaticities_detail_thr_external
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Threshold for chroma detail detection, chromaticity difference is calculated from chromaticity 8bits.
        ///                Higher chromaticity difference than this value could be a detail depends on UD Th comparison.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_triple_chromaticities_detail_thr_external;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_isotropic_self_decision_enforce_detail
        ///
        /// Tuning Level:  OftenTune
        /// Description:   1- if the pixel detected a detail than median wont work on the pixel in the problematic directions, or all
        ///                directions for isotropic. 0 - pixel self detail wont enforce median not working. Now it depends on the
        ///                environment decision.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_isotropic_self_decision_enforce_detail;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_isotropic_neighbors_detail_sensitivity
        ///
        /// Tuning Level:  MustTune
        /// Description:   The environment detail sensitivity values determine the importance of the local indications environment in
        ///                generating a detail flag. . High value - few local detail indications are required to generate detail flag
        ///                ( = more details, less filtering). Low value - many local detail indications are required to generate
        ///                detail flag ( = less details, more filtering).
        /// Type:          uint
        /// Default Value: 64
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_isotropic_neighbors_detail_sensitivity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_isotropic_neighbors_detail_sensitivity_far_scale
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   scale on the sensativity to get sensativity for bigger environment .
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_isotropic_neighbors_detail_sensitivity_far_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_directional_self_decision_enforce_detail
        ///
        /// Tuning Level:  OftenTune
        /// Description:   1- if the pixel detected a detail than median wont work on the pixel in the problematic directions, or all
        ///                directions for isotropic. 0 - pixel self detail wont enforce median not working. Now it depends on the
        ///                environment decision.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_directional_self_decision_enforce_detail;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_directional_neighbors_detail_sensitivity
        ///
        /// Tuning Level:  MustTune
        /// Description:   The environment detail sensitivity values determine the importance of the local indications environment in
        ///                generating a detail flag. . High value - few local detail indications are required to generate detail flag
        ///                ( = more details, less filtering). Low value - many local detail indications are required to generate
        ///                detail flag ( = less details, more filtering).
        /// Type:          uint
        /// Default Value: 32
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_directional_neighbors_detail_sensitivity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// median_detect_directional_neighbors_detail_sensitivity_far_scale
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   scale on the sensativity to get sensativity for bigger environment .
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT median_detect_directional_neighbors_detail_sensitivity_far_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// bilateral_decision_minimalsize
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   if both bilateral and median are on. The mux that chooses the final result from one of the two is
        ///                controled by Bilateral_Decision_MinimalSize. If ( chromaOutputDCindications_CDCMinSize is greater or equal
        ///                than Bilateral_Decision_MinimalSize) then take bilateral output, otherwise thake median output.
        ///                chromaOutputDCindications_CDCMinSize is in range 0:5 . . 6 is bypass. if the pixel is flatter than this
        ///                value then use the bilateral filter on it, otherwise just use median filter.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [0, 6]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT bilateral_decision_minimalsize;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// bilateral_filtersize
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   0- bilateral size 1x1 (disabled). 1- bilateral size 3x3. 2- bilateral size 5x5.
        /// Type:          uint
        /// Default Value: 2
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT bilateral_filtersize;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_flat_indication_extension_threshold_9x9
        ///
        /// Tuning Level:  NeverTune
        /// Description:   extension of the flatness indication from 0:3 (1x1 : 7x7) to 0:5 (1x1 : 11x11).
        /// Type:          uint
        /// Default Value: 6
        /// Range:         [0, 15]
        /// Bit Depth:     4u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT chroma_flat_indication_extension_threshold_9x9;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_flat_indication_extension_threshold_11x11
        ///
        /// Tuning Level:  NeverTune
        /// Description:
        /// Type:          uint
        /// Default Value: 6
        /// Range:         [0, 15]
        /// Bit Depth:     4u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT chroma_flat_indication_extension_threshold_11x11;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_grey_indication_extension_threshold_9x9
        ///
        /// Tuning Level:  NeverTune
        /// Description:
        /// Type:          uint
        /// Default Value: 21
        /// Range:         [0, 31]
        /// Bit Depth:     5u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT chroma_grey_indication_extension_threshold_9x9;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_grey_indication_extension_threshold_11x11
        ///
        /// Tuning Level:  NeverTune
        /// Description:
        /// Type:          uint
        /// Default Value: 42
        /// Range:         [0, 63]
        /// Bit Depth:     6u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT chroma_grey_indication_extension_threshold_11x11;
    } chroma_filter_extension;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// luma_smoothing_treatment
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct luma_smoothing_treatmentStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// edge_smoothing_binarization_minflatval
        ///
        /// Tuning Level:  MustTune
        /// Description:   min flat val in parallel to the edge in order to smooth it. Flat enough means a large edge and could be
        ///                smoothed. 5 - to bypasse.
        /// Type:          uint
        /// Default Value: 3
        /// Range:         [0, 5]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT edge_smoothing_binarization_minflatval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// edge_smoothing_binarization_maxedgeval
        ///
        /// Tuning Level:  MustTune
        /// Description:   max edge val in perpendicular to the edge in order to smooth the edge. Edge small enough means a strong
        ///                edge that could be smoothed.
        /// Type:          uint
        /// Default Value: 2
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT edge_smoothing_binarization_maxedgeval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// edge_smoothing_agreement_number
        ///
        /// Tuning Level:  MustTune
        /// Description:   how many neighbores out of 5 should fulfill the parrallel minflatval condition and the perpendicular
        ///                maxedgeval condition. This affects also how many neighbores out of 3 should fulfill the conditions.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [2, 5]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT edge_smoothing_agreement_number;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_smoothing_filter_size
        ///
        /// Tuning Level:  OftenTune
        /// Description:   1- 3x3 , 2-5x5.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [1, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_smoothing_filter_size;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_isotropic_neighbours_binarization_minflatval
        ///
        /// Tuning Level:  OftenTune
        /// Description:   binarization_minflatval, lower value means more areas will be defined as flat which in turn can give
        ///                filtering in more areas. range [1,4].
        /// Type:          uint
        /// Default Value: 3
        /// Range:         [1, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_isotropic_neighbours_binarization_minflatval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_isotropic_neighbours_binarization_minedgeval
        ///
        /// Tuning Level:  OftenTune
        /// Description:   binarization_minedgeval, lower value means more areas will be defined as edge which in turn can give
        ///                filtering in more areas. range [0,3].
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_isotropic_neighbours_binarization_minedgeval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_isotropic_neighbours_binarization_maxedgeval
        ///
        /// Tuning Level:  OftenTune
        /// Description:   binarization_maxedgeval, higher value means more areas will be defined as edge which in turn can give
        ///                filtering in more areas. range [0,3].
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [0, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_isotropic_neighbours_binarization_maxedgeval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_isotropic_neighbours_agreement_sensitivity
        ///
        /// Tuning Level:  OftenTune
        /// Description:   agreement_sensitivity percentage (out of 128) will set the effective neighbours percetage needs to pass
        ///                all condtions. 86/128 value is 6/9 or 16/25 depends on transition_smoothing_filter_size. 129 to disable
        ///                feature.
        /// Type:          uint
        /// Default Value: 86
        /// Range:         [86, 129]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_isotropic_neighbours_agreement_sensitivity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_isotropic_neighbours_agreement_flat_vs_voters
        ///
        /// Tuning Level:  OftenTune
        /// Description:   percentage (out of 128) of flats voters out of all voters, rest of needed voters should be edges. total
        ///                number of needed voters is determind by transition_isotropic_neighbours_agreement_sensitivity.
        /// Type:          uint
        /// Default Value: 92
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_isotropic_neighbours_agreement_flat_vs_voters;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// flat_isotropic_3x3_neighbours_binarization_minflatval
        ///
        /// Tuning Level:  MustTune
        /// Description:   mininum flat value to define flat for 3x3 additional isotropic filtering feature.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [1, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT flat_isotropic_3x3_neighbours_binarization_minflatval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// flat_isotropic_3x3_neighbours_agreement_sensitivity
        ///
        /// Tuning Level:  MustTune
        /// Description:   agreement_sensitivity of the neighbours, size is 3x3 / 5x5 according to transition_smoothing_filter_size.
        ///                Sensativity 128 requires 100% of neighbours to be flat according to upper threshold. Sensativity 129 is
        ///                disable feature.
        /// Type:          uint
        /// Default Value: 129
        /// Range:         [0, 129]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT flat_isotropic_3x3_neighbours_agreement_sensitivity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// flat_isotropic_5x5_neighbours_agreement_sensitivity
        ///
        /// Tuning Level:  MustTune
        /// Description:   agreement_sensitivity of the neighbours 5x5 according only if transition_smoothing_filter_size ==2,
        ///                otherwise not operational.. Sensativity 128 requires 100% of neighbours to be flat according to upper
        ///                threshold. Sensativity 129 is disable feature. binarization_minflatval is always 4 for this condition.
        /// Type:          uint
        /// Default Value: 129
        /// Range:         [0, 129]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT flat_isotropic_5x5_neighbours_agreement_sensitivity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// smoothing_kernel_3x3_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   3x3 (including center) coefficients weight = 2^kernel_3x3_shift.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT smoothing_kernel_3x3_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// smoothing_kernel_5x5_horver_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   5x5 horizontal, vertical and off-diagonals only coefficients weight = 2^kernel_5x5_horver_shift. 5x5 main
        ///                diagonals are weight = 1.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT smoothing_kernel_5x5_horver_shift;
    } luma_smoothing_treatment;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chroma_smoothing_treatment
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct chroma_smoothing_treatmentStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// edge_smoothing_binarization_minflatval
        ///
        /// Tuning Level:  MustTune
        /// Description:   min flat val in parallel to the edge in order to smooth it. Flat enough means a large edge and could be
        ///                smoothed. 6 - to bypasse.
        /// Type:          uint
        /// Default Value: 3
        /// Range:         [0, 6]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT edge_smoothing_binarization_minflatval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// edge_smoothing_binarization_maxedgeval
        ///
        /// Tuning Level:  MustTune
        /// Description:   max edge val in perpendicular to the edge in order to smooth the edge. Edge small enough means a strong
        ///                edge that could be smoothed.
        /// Type:          uint
        /// Default Value: 2
        /// Range:         [0, 5]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT edge_smoothing_binarization_maxedgeval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// edge_smoothing_agreement_number
        ///
        /// Tuning Level:  MustTune
        /// Description:   how many neighbores out of 3 should fulfill the parrallel minflatval condition and the perpendicular
        ///                maxedgeval condition.
        /// Type:          uint
        /// Default Value: 2
        /// Range:         [2, 3]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT edge_smoothing_agreement_number;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_isotropic_neighbours_binarization_minflatval
        ///
        /// Tuning Level:  OftenTune
        /// Description:   binarization_minflatval, lower value means more areas will be defined as flat which in turn can give
        ///                filtering in more areas.
        /// Type:          uint
        /// Default Value: 2
        /// Range:         [1, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_isotropic_neighbours_binarization_minflatval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_isotropic_neighbours_binarization_minedgeval
        ///
        /// Tuning Level:  OftenTune
        /// Description:   binarization_minedgeval, lower value means more areas will be defined as edge which in turn can give
        ///                filtering in more areas.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_isotropic_neighbours_binarization_minedgeval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_isotropic_neighbours_binarization_maxedgeval
        ///
        /// Tuning Level:  OftenTune
        /// Description:   binarization_maxedgeval, higher value means more areas will be defined as edge which in turn can give
        ///                filtering in more areas.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_isotropic_neighbours_binarization_maxedgeval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_isotropic_neighbours_agreement_sensitivity
        ///
        /// Tuning Level:  OftenTune
        /// Description:   agreement_sensitivity percentage (out of 128) will set the effective neighbours percetage needs to pass
        ///                all condtions. 86/128 value is 6/9 or 16/25 depends on transition_smoothing_filter_size. 129 to disable
        ///                feature.
        /// Type:          uint
        /// Default Value: 86
        /// Range:         [86, 129]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_isotropic_neighbours_agreement_sensitivity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_isotropic_neighbours_agreement_flat_vs_voters
        ///
        /// Tuning Level:  OftenTune
        /// Description:   percentage (out of 128) of flats voters out of all voters, rest of needed voters should be edges. total
        ///                number of needed voters is determind by transition_isotropic_neighbours_agreement_sensitivity.
        /// Type:          uint
        /// Default Value: 92
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT transition_isotropic_neighbours_agreement_flat_vs_voters;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// flat_isotropic_3x3_neighbours_binarization_minflatval
        ///
        /// Tuning Level:  MustTune
        /// Description:   mininum flat value to define flat for 3x3 additional isotropic filtering feature.
        /// Type:          uint
        /// Default Value: 5
        /// Range:         [1, 5]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT flat_isotropic_3x3_neighbours_binarization_minflatval;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// flat_isotropic_3x3_neighbours_agreement_sensitivity
        ///
        /// Tuning Level:  MustTune
        /// Description:   agreement_sensitivity of the neighbours, size is 3x3. Sensativity 128 requires 100% of neighbours to be
        ///                flat according to upper threshold.. Sensativity 129 is disable feature.
        /// Type:          uint
        /// Default Value: 102
        /// Range:         [0, 129]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT flat_isotropic_3x3_neighbours_agreement_sensitivity;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// smoothing_kernel_1x1_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   center coefficient weight = 2^kernel_1x1_shift.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT smoothing_kernel_1x1_shift;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// smoothing_kernel_3x3_shift
        ///
        /// Tuning Level:  OftenTune
        /// Description:   3x3 (not including center) coefficients weight = 2^kernel_3x3_shift.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT smoothing_kernel_3x3_shift;
    } chroma_smoothing_treatment;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lnr
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct lnrStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// elliptic_xc
        ///
        /// Tuning Level:  MustTune
        /// Description:   XC in the formula: . R(x,y) = (A*(x-XC))^2 + (B*(y-YC))^2. logic value 0 is start of the image, 16384 is
        ///                the end of the image.
        /// Type:          uint
        /// Default Value: 8192
        /// Range:         [0, 16384]
        /// Bit Depth:     15u, Q14
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                  elliptic_xc;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// elliptic_yc
        ///
        /// Tuning Level:  MustTune
        /// Description:   YC in the formula:. R(x,y) = (A*(x-XC))^2 + (B*(y-YC))^2. logic value 0 is start of the image, 16384 is
        ///                the end of the image.
        /// Type:          uint
        /// Default Value: 8192
        /// Range:         [0, 16384]
        /// Bit Depth:     15u, Q14
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                  elliptic_yc;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// elliptic_a
        ///
        /// Tuning Level:  MustTune
        /// Description:   A in the formula:. R(x,y) = (A*(x-XC))^2 + (B*(y-YC))^2. B will be detemind by B = (2^16 - A). That way we
        ///                keep A,B normilized for a valid linear interpulation.
        /// Type:          uint
        /// Default Value: 32768
        /// Range:         [6553, 58982]
        /// Bit Depth:     16u, Q16
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                  elliptic_a;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_lut_thr_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma filter Y threshold modifier LUT. Modifier threshold as a function of radius^2 and elipses. 17
        ///                entries, each in (0,4,7) bits. . The actual Y threshold will be YbaseThr*luma_filter_lut_thr_y[elliptic
        ///                radius].
        /// Default Value: 128
        /// Range:         [0, 2047]
        /// Bit Depth:     11u, Q7
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_filter_lut_thr_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// luma_filter_lut_thr_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 128
            /// Range:         [0, 2047]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT luma_filter_lut_thr_y[17];
        } luma_filter_lut_thr_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_lut_thr_uv_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma filter uv threshold modifier LUT. Modifier threshold as a function of radius^2 and elipses. 17
        ///                entries, each in (0,4,7) bits. . The actual uv threshold will be
        ///                U/VbaseThr*luma_filter_lut_thr_uv[elliptic radius].
        /// Default Value: 128
        /// Range:         [0, 2047]
        /// Bit Depth:     11u, Q7
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_filter_lut_thr_uv_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// luma_filter_lut_thr_uv - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 128
            /// Range:         [0, 2047]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT luma_filter_lut_thr_uv[17];
        } luma_filter_lut_thr_uv_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_lut_thr_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma filter Y threshold modifier LUT. Modifier threshold as a function of radius^2 and elipses. 17
        ///                entries, each in (0,4,7) bits. . The actual Y threshold will be YbaseThr*chroma_filter_lut_thr_y[elliptic
        ///                radius].
        /// Default Value: 128
        /// Range:         [0, 2047]
        /// Bit Depth:     11u, Q7
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_filter_lut_thr_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// chroma_filter_lut_thr_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 128
            /// Range:         [0, 2047]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT chroma_filter_lut_thr_y[17];
        } chroma_filter_lut_thr_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_lut_thr_uv_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma filter uv threshold modifier LUT. Modifier threshold as a function of radius^2 and elipses. 17
        ///                entries, each in (0,4,7) bits. . The actual uv threshold will be
        ///                U/VbaseThr*chroma_filter_lut_thr_uv[elliptic radius].
        /// Default Value: 128
        /// Range:         [0, 2047]
        /// Bit Depth:     11u, Q7
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_filter_lut_thr_uv_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// chroma_filter_lut_thr_uv - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 128
            /// Range:         [0, 2047]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT chroma_filter_lut_thr_uv[17];
        } chroma_filter_lut_thr_uv_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// strength_modifier_radius_blend_lut_tab
        ///
        /// Tuning Level:  OftenTune
        /// Description:   LNR Strength Modifier LUT. . Blend factor as a function of radius, the blend factor will be used to blend
        ///                between original dcblend2_lut (or flat weight) and the dcblend2_lut * target_factor. 17 Entries, values in
        ///                0:128 with 7 fraction bits. 128 = don’t change ThMult. 0 = blend fully torwards .
        /// Default Value: 128
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct strength_modifier_radius_blend_lut_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// strength_modifier_radius_blend_lut - Array
            ///
            /// Tuning Level:  OftenTune
            /// Default Value: 128
            /// Range:         [0, 128]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT strength_modifier_radius_blend_lut[17];
        } strength_modifier_radius_blend_lut_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_lnr_dcblend2_target_factor
        ///
        /// Tuning Level:  OftenTune
        /// Description:   LNR Luma blend factor (128 is factor 1).
        /// Type:          uint
        /// Default Value: 128
        /// Range:         [128, 2048]
        /// Bit Depth:     12u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                  luma_lnr_dcblend2_target_factor;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_lnr_flat_kernel_weight_target_factor
        ///
        /// Tuning Level:  OftenTune
        /// Description:   LNR Luma flat blend factor (128 is factor 1). Factor on the weight of the flat kernel for lnr in its
        ///                maximal target strenght (lut blend ==0).
        /// Type:          uint
        /// Default Value: 128
        /// Range:         [128, 2048]
        /// Bit Depth:     12u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                  luma_lnr_flat_kernel_weight_target_factor;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_lnr_cnr_dcblend2_target_factor
        ///
        /// Tuning Level:  OftenTune
        /// Description:   LNR CNR Chroma blend factor (128 is factor 1) (max factor). This target factor also serves as the maximal
        ///                scale factor that lnr and cnr can apply together.
        /// Type:          uint
        /// Default Value: 128
        /// Range:         [128, 2048]
        /// Bit Depth:     12u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                  chroma_lnr_cnr_dcblend2_target_factor;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// automatic_influence_luma_luts
        ///
        /// Tuning Level:  OftenTune
        /// Description:   if use_luts_from_full_pass_configuration == 1 then. luma_filter_luts = ( full_pass_luma_filter_luts *
        ///                automatic_influence_luma_luts + (128-automatic_influence_luma_luts ) * [128,128,…,128] ) is greater thanis
        ///                greater than 7.
        /// Type:          uint
        /// Default Value: 128
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                  automatic_influence_luma_luts;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// automatic_influence_chroma_luts
        ///
        /// Tuning Level:  OftenTune
        /// Description:   if use_luts_from_full_pass_configuration == 1 then. chroma_filter_luts = ( full_pass_chroma_filter_luts *
        ///                automatic_influence_chroma_luts + (128-automatic_influence_chroma_luts ) * [128,128,…,128] ) is greater
        ///                thanis greater than 7.
        /// Type:          uint
        /// Default Value: 128
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                  automatic_influence_chroma_luts;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// automatic_influence_modifier_radius_blend_lut
        ///
        /// Tuning Level:  OftenTune
        /// Description:   if use_luts_from_full_pass_configuration == 1 then. strength_modifier_radius_blend_lut = (
        ///                full_pass_strength_modifier_radius_blend_lut * automatic_influence_chroma_luts +
        ///                (128-automatic_influence_chroma_luts ) * [128,128,…,128] ) is greater thanis greater than 7.
        /// Type:          uint
        /// Default Value: 128
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                  automatic_influence_modifier_radius_blend_lut;
    } lnr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cnr
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct cnrStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_angle_start_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Angle(U,V) (hue) low limit.
        /// Default Value: 48
        /// Range:         [0, 1440]
        /// Bit Depth:     11u, Q2
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct detect_angle_start_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// detect_angle_start - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 48
            /// Range:         [0, 1440]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT detect_angle_start[5];
        } detect_angle_start_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_angle_end_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Angle(U,V) (hue) high limit.
        /// Default Value: 256
        /// Range:         [0, 1440]
        /// Bit Depth:     11u, Q2
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct detect_angle_end_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// detect_angle_end - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 256
            /// Range:         [0, 1440]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT detect_angle_end[5];
        } detect_angle_end_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_chromaticity_start_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Color chromaticity low limit. 1024 is maximal chromaticity chosen according to calc mode.
        /// Default Value: 0
        /// Range:         [0, 1024]
        /// Bit Depth:     11u
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct detect_chromaticity_start_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// detect_chromaticity_start - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [0, 1024]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT detect_chromaticity_start[5];
        } detect_chromaticity_start_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_chromaticity_end_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Color chromaticity high limit. 1024 is maximal chromaticity chosen according to calc mode.
        /// Default Value: 1024
        /// Range:         [0, 1024]
        /// Bit Depth:     11u
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct detect_chromaticity_end_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// detect_chromaticity_end - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 1024
            /// Range:         [0, 1024]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT detect_chromaticity_end[5];
        } detect_chromaticity_end_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_luma_start_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Y component low limit.
        /// Default Value: 100
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct detect_luma_start_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// detect_luma_start - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 100
            /// Range:         [0, 1023]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT detect_luma_start[5];
        } detect_luma_start_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_luma_end_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Y component high limit.
        /// Default Value: 880
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct detect_luma_end_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// detect_luma_end - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 880
            /// Range:         [0, 1023]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT detect_luma_end[5];
        } detect_luma_end_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_color0_skin_saturation_min_y_min
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Minumum saturation value for skin when Y=Y_min.
        /// Type:          uint
        /// Default Value: 328
        /// Range:         [0, 1024]
        /// Bit Depth:     11u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                       detect_color0_skin_saturation_min_y_min;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_color0_skin_saturation_max_y_min
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Maximum saturation value for skin when Y=Y_min.
        /// Type:          uint
        /// Default Value: 1024
        /// Range:         [0, 1024]
        /// Bit Depth:     11u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                       detect_color0_skin_saturation_max_y_min;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_color0_skin_saturation_min_y_max
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Minumum saturation value for skin when Y=Y_max.
        /// Type:          uint
        /// Default Value: 82
        /// Range:         [0, 1024]
        /// Bit Depth:     11u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                       detect_color0_skin_saturation_min_y_max;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// detect_color0_skin_saturation_max_y_max
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Maximum saturation value for skin when Y=Y_max.
        /// Type:          uint
        /// Default Value: 819
        /// Range:         [0, 1024]
        /// Bit Depth:     11u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                       detect_color0_skin_saturation_max_y_max;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// boundary_weight_tab
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Weight value at Color Range Borders.
        /// Default Value: 1
        /// Range:         [1, 15]
        /// Bit Depth:     4u, Q4
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct boundary_weight_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// boundary_weight - Array
            ///
            /// Tuning Level:  OftenTune
            /// Default Value: 1
            /// Range:         [1, 15]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT boundary_weight[5];
        } boundary_weight_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// transition_ratio_tab
        ///
        /// Tuning Level:  OftenTune
        /// Description:   smooth range transition ratio (with regards to color range) of each color region.
        /// Default Value: 13
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q8
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct transition_ratio_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// transition_ratio - Array
            ///
            /// Tuning Level:  OftenTune
            /// Default Value: 13
            /// Range:         [0, 128]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT transition_ratio[5];
        } transition_ratio_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// color0_transition_ratio_external
        ///
        /// Tuning Level:  OftenTune
        /// Description:   smooth range transition ratio (with regards to color 0 range) of each external color regions. q_ext =
        ///                transition_ratio_external / boundery_probability[0].
        /// Type:          uint
        /// Default Value: 13
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q8
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                       color0_transition_ratio_external;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_threshold_scale_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Gain factor YLPF Y-Base Threshold.
        /// Default Value: 128
        /// Range:         [0, 2047]
        /// Bit Depth:     11u, Q7
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_filter_threshold_scale_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// luma_filter_threshold_scale_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 128
            /// Range:         [0, 2047]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT luma_filter_threshold_scale_y[5];
        } luma_filter_threshold_scale_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_threshold_scale_uv_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Gain factor for YLPF U/V-Base Thresholds .
        /// Default Value: 128
        /// Range:         [0, 2047]
        /// Bit Depth:     11u, Q7
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_filter_threshold_scale_uv_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// luma_filter_threshold_scale_uv - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 128
            /// Range:         [0, 2047]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT luma_filter_threshold_scale_uv[5];
        } luma_filter_threshold_scale_uv_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_offset_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Offset for YLPF Y-Base Threshold .
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Bit Depth:     11s
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_filter_offset_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// luma_filter_offset_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-1024, 1023]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT luma_filter_offset_y[5];
        } luma_filter_offset_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_offset_u_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Offset for YLPF U-Base Threshold .
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_filter_offset_u_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// luma_filter_offset_u - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-256, 255]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT luma_filter_offset_u[5];
        } luma_filter_offset_u_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_offset_v_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Offset for YLPF V-Base Threshold.
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_filter_offset_v_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// luma_filter_offset_v - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-256, 255]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT luma_filter_offset_v[5];
        } luma_filter_offset_v_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_threshold_scale_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Gain factor for CLPF Y-Base Threshold.
        /// Default Value: 128
        /// Range:         [0, 2047]
        /// Bit Depth:     11u, Q7
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_filter_threshold_scale_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// chroma_filter_threshold_scale_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 128
            /// Range:         [0, 2047]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT chroma_filter_threshold_scale_y[5];
        } chroma_filter_threshold_scale_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_threshold_scale_uv_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Gain factor for CLPF U/V- Base Threshold .
        /// Default Value: 128
        /// Range:         [0, 2047]
        /// Bit Depth:     11u, Q7
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_filter_threshold_scale_uv_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// chroma_filter_threshold_scale_uv - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 128
            /// Range:         [0, 2047]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT chroma_filter_threshold_scale_uv[5];
        } chroma_filter_threshold_scale_uv_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_offset_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Offset for CLPF Y-Base Threshold.
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Bit Depth:     11s
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_filter_offset_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// chroma_filter_offset_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-1024, 1023]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT chroma_filter_offset_y[5];
        } chroma_filter_offset_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_offset_u_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Offset for CLPF U-Base Threshold.
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_filter_offset_u_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// chroma_filter_offset_u - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-256, 255]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT chroma_filter_offset_u[5];
        } chroma_filter_offset_u_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_offset_v_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Offset for CLPF V-Base Threshold.
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_filter_offset_v_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// chroma_filter_offset_v - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-256, 255]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT chroma_filter_offset_v[5];
        } chroma_filter_offset_v_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_base_far_modifier_y
        ///
        /// Tuning Level:  OftenTune
        /// Description:   YLPF Blend factor for CNR gain level for the Far base Luma thresholds. 0-no CNR modification on far
        ///                thresholds at all. 4-full CNR modification on far with same scale and offset as in close. in between we
        ///                blend.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                       luma_filter_base_far_modifier_y;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_base_far_modifier_uv
        ///
        /// Tuning Level:  OftenTune
        /// Description:   YLPF Blend factor for CNR gain level for the Far base Chroma thresholds. 0-no CNR modification on far
        ///                thresholds at all. 4-full CNR modification on far with same scale and offset as in close. in between we
        ///                blend.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                       luma_filter_base_far_modifier_uv;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_base_far_modifier_y
        ///
        /// Tuning Level:  OftenTune
        /// Description:   CLPF Blend factor for CNR gain level for the Far base Luma thresholds. 0-no CNR modification on far
        ///                thresholds at all. 4-full CNR modification on far with same scale and offset as in close. in between we
        ///                blend.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                       chroma_filter_base_far_modifier_y;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_base_far_modifier_uv
        ///
        /// Tuning Level:  OftenTune
        /// Description:   CLPF Blend factor for CNR gain level for the Far base Chroma thresholds. 0-no CNR modification on far
        ///                thresholds at all. 4-full CNR modification on far with same scale and offset as in close. in between we
        ///                blend.
        /// Type:          uint
        /// Default Value: 4
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                                       chroma_filter_base_far_modifier_uv;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_dcblend2_weight_scale_tab
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Scale Factor for user defined DCBlend2 Y_Dc blend factor. (2048 == X16).
        /// Default Value: 128
        /// Range:         [0, 2048]
        /// Bit Depth:     12u, Q7
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_dcblend2_weight_scale_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// luma_dcblend2_weight_scale - Array
            ///
            /// Tuning Level:  OftenTune
            /// Default Value: 128
            /// Range:         [0, 2048]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT luma_dcblend2_weight_scale[5];
        } luma_dcblend2_weight_scale_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_dcblend2_weight_restricted_scale_tab
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Scale Factor for user defined DCBlend2 C_Dc blend factor. (2048 == X16). this scale is limited by
        ///                chroma_lnr_cnr_dcblend2_target_factor with the following: . chroma_dcblend2_weight_scale (actual cnr scale
        ///                influence of dcblend 2 blend factor ) = MIN( MAX(chroma_dcblend2_weight_restricted_scale , 2*128 -
        ///                chroma_lnr_cnr_dcblend2_target_factor ) , chroma_lnr_cnr_dcblend2_target_factor ).
        /// Default Value: 128
        /// Range:         [0, 2048]
        /// Bit Depth:     12u, Q7
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_dcblend2_weight_restricted_scale_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// chroma_dcblend2_weight_restricted_scale - Array
            ///
            /// Tuning Level:  OftenTune
            /// Default Value: 128
            /// Range:         [0, 2048]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT chroma_dcblend2_weight_restricted_scale[5];
        } chroma_dcblend2_weight_restricted_scale_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_flat_kernel_blend_weight_scale_tab
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Scale Factor for user defined flat kernel blend factor. (2048 == X16).
        /// Default Value: 128
        /// Range:         [0, 2048]
        /// Bit Depth:     12u, Q7
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_flat_kernel_blend_weight_scale_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// luma_flat_kernel_blend_weight_scale - Array
            ///
            /// Tuning Level:  OftenTune
            /// Default Value: 128
            /// Range:         [0, 2048]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT luma_flat_kernel_blend_weight_scale[5];
        } luma_flat_kernel_blend_weight_scale_tab;
    } cnr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// luma_filter_detection_thresholds
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct luma_filter_detection_thresholdsStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_per_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, _Y threshold LutY.
        /// Default Value: 0
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct y_threshold_per_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// y_threshold_per_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [0, 1023]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT y_threshold_per_y[17];
        } y_threshold_per_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_per_uv_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut
        ///                interpulation are limited with control_per_uv_limit. u_limited = min(u, control_per_uv_limit), v_limited =
        ///                min(u, control_per_uv_limit). Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) ).
        /// Default Value: 0
        /// Range:         [-128, 127]
        /// Bit Depth:     8s
        /// Length:        8
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct y_threshold_per_uv_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// y_threshold_per_uv - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-128, 127]
            /// Length:        8
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT y_threshold_per_uv[8];
        } y_threshold_per_uv_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_top_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, _Y threshold Top Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 1023
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_top_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_bottom_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, _Y threshold Bottom, Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_bottom_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_close3_mod_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   Luma filter, 3x3 luma derivative , 6 bits unsigned (0,2,4).
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_close3_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_close3_mod_offset
        ///
        /// Tuning Level:  MustTune
        /// Description:   Luma filter, 3x3 luma derivative , 11 bits signed (1,10,0).
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Bit Depth:     11s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_close3_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_der2_close3_mod_scale
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Luma filter, 3x3 luma second order derivative scale, 6 bits unsigned (0,2,4).
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_der2_close3_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_der2_close3_mod_offset
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Luma filter, 3x3 luma second order derivative offset 11 bits signed (1,10,0).
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Bit Depth:     11s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_der2_close3_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_far_mod_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, 7x7/9x9 _Ythr modifier Far(7,9) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_far_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_far_mod_offset
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, 7x7/9x9 _Ythr modifier Far(7,9) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Bit Depth:     11s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_far_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_close_external_mod_scale
        ///
        /// Tuning Level:  NeverTune
        /// Description:   luma_filter, 3x3/5x5 _Ythr modifier CloseX (X==External) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_close_external_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_close_external_mod_offset
        ///
        /// Tuning Level:  NeverTune
        /// Description:   luma_filter, 3x3/5x5 _Ythr modifier CloseX (X==External) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Bit Depth:     11s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_close_external_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_far_external_mod_scale
        ///
        /// Tuning Level:  NeverTune
        /// Description:   luma_filter, 7x7/9x9 _Ythr modifier FarX (X==External) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_far_external_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_far_external_mod_offset
        ///
        /// Tuning Level:  NeverTune
        /// Description:   luma_filter, 7x7/9x9 _Ythr modifier FarX (X==External) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Bit Depth:     11s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_far_external_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_per_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, _U threshold LutY.
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct u_threshold_per_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// u_threshold_per_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [0, 255]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT u_threshold_per_y[17];
        } u_threshold_per_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_per_uv_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut
        ///                interpulation are limited with control_per_uv_limit. u_limited = min(u, control_per_uv_limit), v_limited =
        ///                min(u, control_per_uv_limit). Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) ).
        /// Default Value: 0
        /// Range:         [-32, 31]
        /// Bit Depth:     6s
        /// Length:        8
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct u_threshold_per_uv_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// u_threshold_per_uv - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-32, 31]
            /// Length:        8
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT u_threshold_per_uv[8];
        } u_threshold_per_uv_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_top_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, _U threshold Top Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_top_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_bottom_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, _U threshold Bottom, Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_bottom_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_far_mod_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, 7x7/9x9(luma) _Uthr modifier Far(5) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_far_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_far_mod_offset
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, 7x7/9x9(luma) _Uthr modifier Far(5) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_far_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_far_external_mod_scale
        ///
        /// Tuning Level:  NeverTune
        /// Description:   luma_filter, 7x7/9x9 _Uthr modifier FarX (X==External) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_far_external_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_far_external_mod_offset
        ///
        /// Tuning Level:  NeverTune
        /// Description:   luma_filter, 7x7/9x9 _Uthr modifier FarX (X==External) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_far_external_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_per_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, _V threshold LutY.
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct v_threshold_per_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// v_threshold_per_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [0, 255]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT v_threshold_per_y[17];
        } v_threshold_per_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_per_uv_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut
        ///                interpulation are limited with control_per_uv_limit. u_limited = min(u, control_per_uv_limit), v_limited =
        ///                min(u, control_per_uv_limit). Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) ).
        /// Default Value: 0
        /// Range:         [-32, 31]
        /// Bit Depth:     6s
        /// Length:        8
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct v_threshold_per_uv_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// v_threshold_per_uv - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-32, 31]
            /// Length:        8
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT v_threshold_per_uv[8];
        } v_threshold_per_uv_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_top_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, _V threshold Top Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_top_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_bottom_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, _V threshold Bottom, Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_bottom_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_far_mod_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, 7x7/9x9(luma) _Vthr modifier Far(5) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_far_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_far_mod_offset
        ///
        /// Tuning Level:  MustTune
        /// Description:   luma_filter, 7x7/9x9(luma) _Vthr modifier Far(5) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_far_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_far_external_mod_scale
        ///
        /// Tuning Level:  NeverTune
        /// Description:   luma_filter, 7x7/9x9 _Vthr modifier FarX (X==External) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_far_external_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_far_external_mod_offset
        ///
        /// Tuning Level:  NeverTune
        /// Description:   luma_filter, 7x7/9x9 _Vthr modifier FarX (X==External) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_far_external_mod_offset;
    } luma_filter_detection_thresholds;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chroma_filter_detection_thresholds
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct chroma_filter_detection_thresholdsStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_per_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, _Y threshold LutY.
        /// Default Value: 0
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct y_threshold_per_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// y_threshold_per_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [0, 1023]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT y_threshold_per_y[17];
        } y_threshold_per_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_per_uv_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut
        ///                interpulation are limited with control_per_uv_limit. u_limited = min(u, control_per_uv_limit), v_limited =
        ///                min(u, control_per_uv_limit). Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) ).
        /// Default Value: 0
        /// Range:         [-128, 127]
        /// Bit Depth:     8s
        /// Length:        8
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct y_threshold_per_uv_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// y_threshold_per_uv - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-128, 127]
            /// Length:        8
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT y_threshold_per_uv[8];
        } y_threshold_per_uv_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_top_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, _Y threshold Top Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 1023
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_top_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_bottom_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, _Y threshold Bottom, Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1023]
        /// Bit Depth:     10u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_bottom_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_far_mod_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, 5x5 _Ythr modifier Far(5 chroma blocks but 7x7/9x9 for y pixels) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_far_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_threshold_far_mod_offset
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, 5x5 _Ythr modifier Far(5 chroma blocks but 7x7/9x9 for y pixels) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Bit Depth:     11s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  y_threshold_far_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_per_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, _U threshold LutY.
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct u_threshold_per_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// u_threshold_per_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [0, 255]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT u_threshold_per_y[17];
        } u_threshold_per_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_per_uv_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut
        ///                interpulation are limited with control_per_uv_limit. u_limited = min(u, control_per_uv_limit), v_limited =
        ///                min(u, control_per_uv_limit). Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) ).
        /// Default Value: 0
        /// Range:         [-32, 31]
        /// Bit Depth:     6s
        /// Length:        8
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct u_threshold_per_uv_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// u_threshold_per_uv - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-32, 31]
            /// Length:        8
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT u_threshold_per_uv[8];
        } u_threshold_per_uv_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_top_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, _U threshold Top Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_top_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_bottom_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, _U threshold Bottom, Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_bottom_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_far_mod_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, 5x5 _Uthr modifier Far(5) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_far_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_far_mod_offset
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, 5x5 _Uthr modifier Far(5) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_far_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_distant_mod_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter,_Uthr modifier Dist (7) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_distant_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// u_threshold_distant_mod_offset
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter,_Uthr modifier Dist (7) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  u_threshold_distant_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_per_y_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, _V threshold LutY.
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Length:        17
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct v_threshold_per_y_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// v_threshold_per_y - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [0, 255]
            /// Length:        17
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT v_threshold_per_y[17];
        } v_threshold_per_y_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_per_uv_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut
        ///                interpulation are limited with control_per_uv_limit. u_limited = min(u, control_per_uv_limit), v_limited =
        ///                min(u, control_per_uv_limit). Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) ).
        /// Default Value: 0
        /// Range:         [-32, 31]
        /// Bit Depth:     6s
        /// Length:        8
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct v_threshold_per_uv_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// v_threshold_per_uv - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0
            /// Range:         [-32, 31]
            /// Length:        8
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT v_threshold_per_uv[8];
        } v_threshold_per_uv_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_top_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, _V threshold Top Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_top_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_bottom_limit
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, _V threshold Bottom, Limit of (LutY+LutUV).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 255]
        /// Bit Depth:     8u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_bottom_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_far_mod_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, 5x5 _Vthr modifier Far(5) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_far_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_far_mod_offset
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, 5x5 _Vthr modifier Far(5) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_far_mod_offset;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_distant_mod_scale
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, 7x7 _Vthr modifier Dist (7) scale/offset factors.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 63]
        /// Bit Depth:     6u, Q4
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_distant_mod_scale;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// v_threshold_distant_mod_offset
        ///
        /// Tuning Level:  MustTune
        /// Description:   chroma_filter, 7x7 _Vthr modifier Dist (7) scale/offset factors.
        /// Type:          int
        /// Default Value: 0
        /// Range:         [-256, 255]
        /// Bit Depth:     9s
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT                  v_threshold_distant_mod_offset;
    } chroma_filter_detection_thresholds;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dcblend2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct dcblend2Struct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dcblend2_luma_strength_function_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Controls the strength of DCBlend2. Lut of how much to blend as a function YDCSize. Higher values reduce
        ///                luminance noise significantly, at the expense of texture, granularity and sharpness. Lower values maintain
        ///                the luminance noise levels, while preserving texture, granularity and sharpness.
        /// Default Value: 0,0,0,0,0
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Length:        5
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct dcblend2_luma_strength_function_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// dcblend2_luma_strength_function - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0,0,0,0,0
            /// Range:         [0, 128]
            /// Length:        5
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT dcblend2_luma_strength_function[5];
        } dcblend2_luma_strength_function_tab;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dcblend2_chroma_strength_function_tab
        ///
        /// Tuning Level:  MustTune
        /// Description:   Controls the strength of DCBlend2. Lut of how much to blend as a function CDCSize. (in case of filter2 ind
        ///                extenstion interp is off, these values will be interpulated to [0,3] instead of [0,5]). Higher values
        ///                reduce chromatic noise significantly, at the expense of texture, granularity and sharpness. Lower values
        ///                maintain the chromatic noise levels, while preserving texture, granularity and sharpness.
        /// Default Value: 0,0,0,0,0,0
        /// Range:         [0, 128]
        /// Bit Depth:     8u, Q7
        /// Length:        6
        /// Conversion:    No Conversion needed
        /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
        ///                DC64. Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct dcblend2_chroma_strength_function_tabStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// dcblend2_chroma_strength_function - Array
            ///
            /// Tuning Level:  MustTune
            /// Default Value: 0,0,0,0,0,0
            /// Range:         [0, 128]
            /// Length:        6
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FLOAT dcblend2_chroma_strength_function[6];
        } dcblend2_chroma_strength_function_tab;
    } dcblend2;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// mod_anr10_pass_dataType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mod_anr10_pass_dataType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// pass_trigger
    ///
    /// Default Value: PASS_FULL(0),PASS_DC4(1),PASS_DC16(2),PASS_DC64(3)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 trigger_pass;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anr10_rgn_data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    anr10_rgn_dataType anr10_rgn_data;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// anr10_cct_dataType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct anr10_cct_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cct_data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct cct_dataStruct
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// mod_anr10_pass_data - Array
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        mod_anr10_pass_dataType mod_anr10_pass_data[4];
    } cct_data;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// anr10_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct anr10_enable_sectionStruct
{
    UINT32 reserved;
    INT32  enable_luma_noise_reduction;
    INT32  enable_chroma_noise_reduction;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// mod_anr10_pass_reserve_dataType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mod_anr10_pass_reserve_dataType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// pass_trigger
    ///
    /// Default Value: PASS_FULL(0),PASS_DC4(1),PASS_DC16(2),PASS_DC64(3)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 pass_trigger;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// pass_data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct pass_dataStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// top
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct topStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_luma_noise_reduction_pass
            ///
            /// Tuning Level:  MustTune
            /// Description:   Enable Luma filtering for current PASS.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_luma_noise_reduction_pass;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_chroma_noise_reduction_pass
            ///
            /// Tuning Level:  MustTune
            /// Description:   Enable Chroma filtering for current PASS.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_chroma_noise_reduction_pass;
        } top;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// power_control
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct power_controlStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_chroma_filter_extension
            ///
            /// Tuning Level:  OftenTune
            /// Description:   1 - Standard mode. . 0 - Low power mode, disable chroma filter extension: turn off median, second
            ///                bilateral and chroma indicaton extention.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_chroma_filter_extension;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_luma_smoothing_treatment_and_peak_treatment
            ///
            /// Tuning Level:  OftenTune
            /// Description:   if set to 0, luma_smoothing_treatment_and_peak_treatment will be turned off.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_luma_smoothing_treatment_and_peak_treatment;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_chroma_smoothing_treatment_and_peak_treatment
            ///
            /// Tuning Level:  OftenTune
            /// Description:   if set to 0, chroma_smoothing_treatment_and_peak_treatment will be turned off.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_chroma_smoothing_treatment_and_peak_treatment;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_luma_chroma_filter_all_thresholds_per_uv
            ///
            /// Tuning Level:  OftenTune
            /// Description:   if set to 0, y,u,v thresholds will be dependent only on Ylut (function of y) and not on Uvlut (function of
            ///                u,v).
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_luma_chroma_filter_all_thresholds_per_uv;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_luma_chroma_filter_uv_thresholds_per_yuv
            ///
            /// Tuning Level:  OftenTune
            /// Description:   if set to 0, u,v thresholds will be a constant and will not be dependent on y,u,v. Constant will be taken
            ///                from first cell of lut_per_y.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_luma_chroma_filter_uv_thresholds_per_yuv;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_luma_filter_uv_thresholds
            ///
            /// Tuning Level:  OftenTune
            /// Description:   if set to 0, luma filter will be dependent only on y derivatives and not u,v derivativs (no need to
            ///                compare them).
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_luma_filter_uv_thresholds;
        } power_control;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_config
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_filter_configStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// filter_decision_mode
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   0 - normal mode (depends on iisotropic_min, enable_external). 1 - manual settings ( manual mode, config
            ///                FiltDerFlags register as user like. (can be configured to disconnect the link betwee 5 depnds on 3 for
            ///                example ... )).
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 filter_decision_mode;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// filter_enable_external_derivatives
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   is consider dx derivatives for y filtering. 0: don’t use. 1:use.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 filter_enable_external_derivatives;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// dcind_decision_mode
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   0 - normal mode. 1 - manual settings ( manual mode, config dcindDerFlags register as user like ).
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 dcind_decision_mode;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// dcind_enable_external_derivatives
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   is consider dx derivatives for y dc indications. 0: don’t use. 1:use.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 dcind_enable_external_derivatives;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_use_second_derivative_for_luma_3x3
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   use 2nd derivative for 3x3 comparison decision for luma filtering and dcind and preform OR with regular
            ///                derivative comparison of regular derivatives.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_use_second_derivative_for_luma_3x3;
        } luma_filter_config;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_config
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_filter_configStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// filter_decision_mode
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   0 - normal mode. 1 - manual settings ( manual mode, config FiltDerFlags register as user like. (can be
            ///                configured to disconnect the link betwee 5 depnds on 3 for example ... )).
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 filter_decision_mode;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// dcind_decision_mode
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   0 - normal mode. 1 - manual settings ( manual mode, config dcindDerFlags register as user like ).
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 dcind_decision_mode;
        } chroma_filter_config;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_kernel
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_filter_kernelStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// kernel_definition_mode
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   0 - use abstraction (auto config) for deciding on kernels based on granularity and filter size. 1- use
            ///                manual configured kernels by the user inside the kernels struct. User MUST set edge kernel, flat weight
            ///                and flat kernel if weight is greater than zero.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 kernel_definition_mode;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// manual_edge_kernel_complement_mode
            ///
            /// Tuning Level:  NeverTune
            /// Description:   0: dont include pixels in between main directions, 1 include them only if all directions agree. 2- include
            ///                them only if the two relevant directions (the direction the pixel is in between) agree.
            /// Type:          uint
            /// Default Value: 2
            /// Range:         [0, 2]
            /// Bit Depth:     2u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 manual_edge_kernel_complement_mode;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// manual_flat_kernel_complement_mode
            ///
            /// Tuning Level:  NeverTune
            /// Description:   0: dont include pixels in between main directions, 1 include them only if all directions agree. 2- include
            ///                them only if the two relevant directions (the direction the pixel is in between) agree.
            /// Type:          uint
            /// Default Value: 2
            /// Range:         [0, 2]
            /// Bit Depth:     2u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 manual_flat_kernel_complement_mode;
        } luma_filter_kernel;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_peak_management
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_peak_managementStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_luma_peak_management
            ///
            /// Tuning Level:  OftenTune
            /// Description:   enable luma peak treatment.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_luma_peak_management;
        } luma_peak_management;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_peak_management
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_peak_managementStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_chroma_peak_management
            ///
            /// Tuning Level:  OftenTune
            /// Description:   enable chroma peak treatment.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_chroma_peak_management;
        } chroma_peak_management;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// inter_length_output_indication
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct inter_length_output_indicationStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// luma_neighbours_impact_enable
            ///
            /// Tuning Level:  NeverTune
            /// Description:   if set to 1, pixel can be dependant on its neighbores for indicating as flat for higher pass.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 luma_neighbours_impact_enable;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// chroma_neighbours_impact_enable
            ///
            /// Tuning Level:  NeverTune
            /// Description:   if set to 1, pixel can be dependant on its neighbores for indicating as flat for higher pass.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 chroma_neighbours_impact_enable;
        } inter_length_output_indication;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// grey_treatment
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct grey_treatmentStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_grey_treatment_thr_modification
            ///
            /// Tuning Level:  MustTune
            /// Description:   increase threshold torwards the Thr_Modification_Target_*_DER values. If fully grey, thresholds will
            ///                assume the target.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_grey_treatment_thr_modification;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_grey_treatment_isotropic_filter_blend
            ///
            /// Tuning Level:  MustTune
            /// Description:   blend with isotropic filtering.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_grey_treatment_isotropic_filter_blend;
        } grey_treatment;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_extension
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_filter_extensionStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_chroma_filter_extension_median
            ///
            /// Tuning Level:  MustTune
            /// Description:   enable current pass CLPF Edge filtering bypass.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_chroma_filter_extension_median;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_chroma_filter_extension_bilateral
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   enable for the Chroma Bilateral filter block .
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_chroma_filter_extension_bilateral;
        } chroma_filter_extension;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_smoothing_treatment
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_smoothing_treatmentStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_luma_edge_smoothing
            ///
            /// Tuning Level:  MustTune
            /// Description:   If set to 1, Luma edge smoothing is enabled.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_luma_edge_smoothing;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_luma_transition_smoothing
            ///
            /// Tuning Level:  MustTune
            /// Description:   If set to 1, Luma transition smoothing is enabled.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_luma_transition_smoothing;
        } luma_smoothing_treatment;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_smoothing_treatment
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_smoothing_treatmentStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_chroma_edge_smoothing
            ///
            /// Tuning Level:  MustTune
            /// Description:   If set to 1, Chroma edge smoothing is enabled.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_chroma_edge_smoothing;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_chroma_transition_smoothing
            ///
            /// Tuning Level:  MustTune
            /// Description:   If set to 1, chroma transition smoothing is enabled.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_chroma_transition_smoothing;
        } chroma_smoothing_treatment;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// lnr
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct lnrStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_lnr
            ///
            /// Tuning Level:  MustTune
            /// Description:
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_lnr;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// use_luts_from_full_pass_configuration
            ///
            /// Tuning Level:  OftenTune
            /// Description:   automatic mode : if set to 1, take the luts from the Full pass configuration and compensate with influence
            ///                factor. . manual mode : if set to 0, take the luts from the current pass configuration. . influence factor
            ///                == 1 take the same lut, influence factor == 0 take lut ofall 128, in between interpulate .
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 use_luts_from_full_pass_configuration;
        } lnr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// cnr
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct cnrStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_cnr
            ///
            /// Tuning Level:  MustTune
            /// Description:   0 - enable operation. 1 - bypass unit.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32                            enable_cnr;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// input_select
            ///
            /// Tuning Level:  MustTune
            /// Description:   0 - select image from CYLPF. 1 - select DCNF image .
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32                            input_select;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// number_of_colors
            ///
            /// Tuning Level:  MustTune
            /// Description:   Defines the actual number of treated colors.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 5]
            /// Bit Depth:     3u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32                            number_of_colors;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// detect_chromaticity_calc_mode_tab
            ///
            /// Tuning Level:  OftenTune
            /// Description:   0 - chromaticity is calculated by (abs(U) + abs(V))/2. 1 - chromaticity is calculated by Radius =
            ///                (U^2+V^2)^0.5.
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Length:        5
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            struct detect_chromaticity_calc_mode_tabStruct
            {

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                /// detect_chromaticity_calc_mode - Array
                ///
                /// Tuning Level:  OftenTune
                /// Default Value: 1
                /// Range:         [0, 1]
                /// Length:        5
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                UINT32 detect_chromaticity_calc_mode[5];
            } detect_chromaticity_calc_mode_tab;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// detect_color0_saturation_mode
            ///
            /// Tuning Level:  OftenTune
            /// Description:   0 - Color#0 saturation is calculated by Saturation = Chromaticity/Y (HSY color space). Taken from
            ///                detect_color0_skin_saturation_*. 1 - Color#0 saturation is calculated by Saturation = Chromaticity (YUV
            ///                color space). Taken from detect_chromaticity_start[0], detect_chromaticity_end[0].
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32                            detect_color0_saturation_mode;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// detect_color0_external_enable
            ///
            /// Tuning Level:  OftenTune
            /// Description:   0 - disable external color regions treatment. 1 - enable color regions treatment to U is less or equal
            ///                than 0 and V is greater or equal than 0.
            /// Type:          uint
            /// Default Value: 0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32                            detect_color0_external_enable;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// face_detection_dependency_tab
            ///
            /// Tuning Level:  MustTune
            /// Description:   is dependent of Face Detection for this color. Demand FD in addition to color detection.
            /// Default Value: 0,0,0,0,0
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Length:        5
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            struct face_detection_dependency_tabStruct
            {

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                /// face_detection_dependency - Array
                ///
                /// Tuning Level:  MustTune
                /// Default Value: 0,0,0,0,0
                /// Range:         [0, 1]
                /// Length:        5
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                UINT32 face_detection_dependency[5];
            } face_detection_dependency_tab;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// face_detection_boundary
            ///
            /// Tuning Level:  OftenTune
            /// Description:   actual face radius will be face_detection_boundary * face_detected_radius.
            /// Type:          uint
            /// Default Value: 1229
            /// Range:         [512, 8192]
            /// Bit Depth:     14u, Q10
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32                            face_detection_boundary;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// face_detection_transition
            ///
            /// Tuning Level:  OftenTune
            /// Description:   actual face transition radius (from this point and outer face weight == 0 ) will be
            ///                face_detection_transition * face_detected_radius . restriction : face_detection_boundary is less or equal
            ///                than face_detection_transition is less than = 2 * face_detection_boundary (will be forced otherwise).
            /// Type:          uint
            /// Default Value: 2048
            /// Range:         [1024, 8192]
            /// Bit Depth:     14u, Q10
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32                            face_detection_transition;
        } cnr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// luma_filter_detection_thresholds
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct luma_filter_detection_thresholdsStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// control_per_uv_limit
            ///
            /// Tuning Level:  OftenTune
            /// Description:   CTb points location in the UV space, 127 is at external limits, 64 is half limit, … . ( HW implementation
            ///                is : Limit value for UV-Average input for Luma base threshold caluclation ).
            /// Type:          uint
            /// Default Value: 64
            /// Range:         [32, 127]
            /// Bit Depth:     7u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 control_per_uv_limit;
        } luma_filter_detection_thresholds;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// chroma_filter_detection_thresholds
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct chroma_filter_detection_thresholdsStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// control_per_uv_limit
            ///
            /// Tuning Level:  OftenTune
            /// Description:   CTb points location in the UV space, 127 is at external limits, 64 is half limit, … . ( HW implementation
            ///                is : Limit value for UV-Average input for Luma base threshold caluclation ).
            /// Type:          uint
            /// Default Value: 64
            /// Range:         [32, 127]
            /// Bit Depth:     7u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 control_per_uv_limit;
        } chroma_filter_detection_thresholds;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dcblend1
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct dcblend1Struct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_dcblend1_chroma
            ///
            /// Tuning Level:  MustTune
            /// Description:   enable DCblend1 between lower pass and current one (lower pass has to be processed).
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_dcblend1_chroma;
        } dcblend1;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dcblend2
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct dcblend2Struct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_dcblend2_luma
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   enable DCblend2 between lower pass and current one (lower pass has to be processed).
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_dcblend2_luma;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// enable_dcblend2_chroma
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   enable DCblend2 between lower pass and current one (lower pass has to be processed).
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 enable_dcblend2_chroma;
        } dcblend2;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// dithering
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct ditheringStruct
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// dithering_y_en
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   Dithering enable for luma image.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 dithering_y_en;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// dithering_c_en
            ///
            /// Tuning Level:  RarelyTune
            /// Description:   Dithering enable for chroma image.
            /// Type:          uint
            /// Default Value: 1
            /// Range:         [0, 1]
            /// Bit Depth:     1u
            /// Conversion:    No Conversion needed
            /// Notes:         In tuning_parameter_level, first parmater is for FULL pass, second for DC4, third for DC16 and last is for
            ///                DC64. Constant (per pass): no dynamic configuration even when control variable change.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            UINT32 dithering_c_en;
        } dithering;
    } pass_data;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_anr10_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_anr10_reserveType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mod_anr10_pass_reserve_data - Array
    /// Length:        4
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    mod_anr10_pass_reserve_dataType mod_anr10_pass_reserve_data[4];
};

struct asf30_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_hpf_symmetric_coeff_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   1st layer 7x7 sharpening coefficients
    /// Type:          int
    /// Default Value: 0, 0, 0, -1, -2, -25, -86, -173, -180, 1968
    /// Range:         [[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-1023, 1023],[-2047, 2047]]
    /// Bit Depth:     12 bits([9], center), 11 bits([8]), 10 bits(otherwise). All are Q10 numbers
    /// Length:        10
    /// Conversion:    no conversion needed
    /// Notes:         don’t interpolate
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_1_hpf_symmetric_coeff_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_1_hpf_symmetric_coeff - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0, 0, 0, -1, -2, -25, -86, -173, -180, 1968
        /// Range:         [[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-1023, 1023],[-2047, 2047]]
        /// Length:        10
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_1_hpf_symmetric_coeff[10];
    } layer_1_hpf_symmetric_coeff_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_lpf_symmetric_coeff_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Description: 1st layer 7x7 low-pass filter coefficients
    /// Type:          int
    /// Default Value: 4 8 12 16 16 24 32 36 48 64
    /// Range:         [[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-1023, 1023],[-2047, 2047]]
    /// Bit Depth:      12 bits([9], center), 11 bits([8]), 10 bits(otherwise). All are Q10 numbers
    /// Length:        10
    /// Conversion:    no conversion needed
    /// Notes:         don’t interpolate
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_1_lpf_symmetric_coeff_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_1_lpf_symmetric_coeff - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 4 8 12 16 16 24 32 36 48 64
        /// Range:         [[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-511, 511],[-1023, 1023],[-2047, 2047]]
        /// Length:        10
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_1_lpf_symmetric_coeff[10];
    } layer_1_lpf_symmetric_coeff_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_activity_band_pass_coeff_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   1st layer 5x5 activity band-pass filter (one-eighth coefficients)
    /// Type:          int
    /// Default Value: -4, -16, -24, 0, 32, 112
    /// Range:         [[-127, 127],[-127, 127],[-127,127],[-127, 127],[-255, 255],[-511, 511]]
    /// Bit Depth:     10 bits([5], center), 9 bits([4]), 8 bits(otherwise). All are Q8 numbers
    /// Length:        6
    /// Conversion:    no conversion needed
    /// Notes:         don’t interpolate
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_1_activity_band_pass_coeff_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_1_activity_band_pass_coeff - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: -4, -16, -24, 0, 32, 112
        /// Range:         [[-127, 127],[-127, 127],[-127,127],[-127, 127],[-255, 255],[-511, 511]]
        /// Length:        6
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_1_activity_band_pass_coeff[6];
    } layer_1_activity_band_pass_coeff_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_activity_normalization_lut_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   level-based normalization LUT to calculate normalized activity
    /// Type:          float
    /// Default Value: 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
    /// Range:         [0.0, 0.99609375]
    /// Bit Depth:     8 bits. Unsigned Q8 numbers
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_1_activity_normalization_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_1_activity_normalization_lut - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
        /// Range:         [0.0, 0.99609375]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_1_activity_normalization_lut[64];
    } layer_1_activity_normalization_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_weight_modulation_lut_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   normalized activity-based LUT to control the slope(output/input) when sharpened value magnitude is less
    ///                than the soft threshold
    /// Type:          float
    /// Default Value: 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996
    /// Range:         [0.0, 0.99609375]
    /// Bit Depth:     8 bits. Unsigned Q8 numbers
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_1_weight_modulation_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_1_weight_modulation_lut - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996
        /// Range:         [0.0, 0.99609375]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_1_weight_modulation_lut[64];
    } layer_1_weight_modulation_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_soft_threshold_lut_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   level-based soft-thresholding LUT to determine the final sharpened value
    /// Type:          uint
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [0, 255]
    /// Bit Depth:     8 bits. Unsigned Q0 numbers
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_1_soft_threshold_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_1_soft_threshold_lut - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [0, 255]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_1_soft_threshold_lut[64];
    } layer_1_soft_threshold_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_gain_positive_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   level-based sharpening gain LUT for positive halos
    /// Type:          float
    /// Default Value: 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65
    /// Range:         [0.0, 7.96875]
    /// Bit Depth:     8 bits. Unsigned Q5 numbers
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_1_gain_positive_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_1_gain_positive_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65
        /// Range:         [0.0, 7.96875]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_1_gain_positive_lut[64];
    } layer_1_gain_positive_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_gain_negative_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   level-based sharpening gain LUT for negative halos
    /// Type:          float
    /// Default Value: 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65
    /// Range:         [0.0, 7.96875]
    /// Bit Depth:     8 bits. Unsigned Q5 numbers
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_1_gain_negative_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_1_gain_negative_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65 1.65
        /// Range:         [0.0, 7.96875]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_1_gain_negative_lut[64];
    } layer_1_gain_negative_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_gain_weight_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   normalized activity-based sharpening gain LUT
    /// Type:          float
    /// Default Value: 0.5000 0.5027 0.5108 0.5240 0.5419 0.5639 0.5893 0.6175 0.6475 0.6787 0.7103 0.7416 0.7719 0.8008 0.8278 0.8526 0.8752 0.8953 0.9130 0.9283 0.9414 0.9525 0.9617 0.9692 0.9753 0.9802 0.9841 0.9871 0.9894 0.9912 0.9925 0.9935 0.9943 0.9948 0.9952 0.9954 0.9956 0.9957 0.9958 0.9959 0.9959 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960
    /// Range:         [0.0, 0.99609375]
    /// Bit Depth:     8 bits. Unsigned Q8 numbers
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_1_gain_weight_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_1_gain_weight_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0.5000 0.5027 0.5108 0.5240 0.5419 0.5639 0.5893 0.6175 0.6475 0.6787 0.7103 0.7416 0.7719 0.8008 0.8278 0.8526 0.8752 0.8953 0.9130 0.9283 0.9414 0.9525 0.9617 0.9692 0.9753 0.9802 0.9841 0.9871 0.9894 0.9912 0.9925 0.9935 0.9943 0.9948 0.9952 0.9954 0.9956 0.9957 0.9958 0.9959 0.9959 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960
        /// Range:         [0.0, 0.99609375]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_1_gain_weight_lut[64];
    } layer_1_gain_weight_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_gamma_corrected_luma_target
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   luma target after gamma to control level-based sharpening.
    /// Type:          int
    /// Default Value: 128
    /// Range:         [0, 255]
    /// Bit Depth:     8 bits Q0. unsigned
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_1_gamma_corrected_luma_target;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_gain_cap
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   upper cap of sharpening gain
    /// Type:          float
    /// Default Value: 7.9
    /// Range:         [0.0, 7.96875]
    /// Bit Depth:     8 bits. Unsigned Q5 numbers
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_1_gain_cap;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_clamp_ul
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Manual fixed positive clamping level sharpening
    /// Type:          int
    /// Default Value: 255
    /// Range:         [-255, 255]
    /// Bit Depth:     9 bits. Signed Q0 number
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_1_clamp_ul;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_clamp_ll
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Manual fixed negative clamping level sharpening
    /// Type:          int
    /// Default Value: -255
    /// Range:         [-255, 255]
    /// Bit Depth:     9 bits. Signed Q0 number
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_1_clamp_ll;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_norm_scale
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   scale factor of 5x5 band-pass filter output (activity)
    /// Type:          float
    /// Default Value: 3.98
    /// Range:         [0.0, 3.984375]
    /// Bit Depth:     8 bits. unsigned Q6 number
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_1_norm_scale;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_activity_clamp_threshold
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   static clamp of 5x5 band-pass filter output (activity)
    /// Type:          int
    /// Default Value: 255
    /// Range:         [0, 255]
    /// Bit Depth:     8 bits. unsigned Q0 number
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_1_activity_clamp_threshold;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_l2_norm_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable L2 norm for 5x5 band-pass filter output (activity). 0: L1 norm (absolute value), 1: L2 norm
    ///                (squared value)
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1 bit.
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_1_l2_norm_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_median_blend_upper_offset
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   upper offset of median blend
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0, 0.9375]
    /// Bit Depth:     4 bits. Unsigned Q4.
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_1_median_blend_upper_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_median_blend_lower_offset
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   lower offset of median blend. Has to be smaller than upper offset.
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0, 0.9375]
    /// Bit Depth:     4 bits. Unsigned Q4.
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_1_median_blend_lower_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_sp
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   3x3 median filter smoothing percentage
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 100]
    /// Bit Depth:     5 bits. Unsigned Q4.
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_1_sp;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_hpf_symmetric_coeff_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   2nd layer 9x9 sharpening coefficients (one-eighth coefficients, every other pixels and lines are skipped)
    /// Type:          int
    /// Default Value: -1, -4, -6, -16, -24, 220
    /// Range:         [[-127,127],[-127,127],[-127,127],[-127,127],[-255, 255],[-511, 511]]
    /// Bit Depth:     10 bits([5], center), 9 bits([4]), 8 bits(otherwise). All are Q8 numbers.
    /// Length:        6
    /// Conversion:    no conversion needed
    /// Notes:         don’t interpolate
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_2_hpf_symmetric_coeff_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_2_hpf_symmetric_coeff - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: -1, -4, -6, -16, -24, 220
        /// Range:         [[-127,127],[-127,127],[-127,127],[-127,127],[-255, 255],[-511, 511]]
        /// Length:        6
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_2_hpf_symmetric_coeff[6];
    } layer_2_hpf_symmetric_coeff_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_lpf_symmetric_coeff_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   2nd layer 9x9 low-pass filter coefficients (one-eighth coefficients, every other pixels and lines are
    ///                skipped)
    /// Type:          int
    /// Default Value: 4, 16, 24, 64, 96, 144
    /// Range:         [[-127,127],[-127,127],[-127,127],[-127,127],[-255, 255],[-511, 511]]
    /// Bit Depth:     10 bits([5], center), 9 bits([4]), 8 bits(otherwise). All are Q8 numbers.
    /// Length:        6
    /// Conversion:    no conversion needed
    /// Notes:         don’t interpolate
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_2_lpf_symmetric_coeff_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_2_lpf_symmetric_coeff - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 4, 16, 24, 64, 96, 144
        /// Range:         [[-127,127],[-127,127],[-127,127],[-127,127],[-255, 255],[-511, 511]]
        /// Length:        6
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_2_lpf_symmetric_coeff[6];
    } layer_2_lpf_symmetric_coeff_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_activity_band_pass_coeff_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   2nd layer 5x5 activity band-pass filter (one-eighth coefficients)
    /// Type:          int
    /// Default Value: -4, -16, -24, 0, 32, 112
    /// Range:         [[-127,127],[-127,127],[-127,127],[-127,127],[-255, 255],[-511, 511]]
    /// Bit Depth:     10 bits([5], center), 9 bits([4]), 8 bits(otherwise). All are Q8 numbers.
    /// Length:        6
    /// Conversion:    no conversion needed
    /// Notes:         don’t interpolate
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_2_activity_band_pass_coeff_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_2_activity_band_pass_coeff - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: -4, -16, -24, 0, 32, 112
        /// Range:         [[-127,127],[-127,127],[-127,127],[-127,127],[-255, 255],[-511, 511]]
        /// Length:        6
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_2_activity_band_pass_coeff[6];
    } layer_2_activity_band_pass_coeff_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_activity_normalization_lut_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   level-based normalization LUT to calculate normalized activity
    /// Type:          float
    /// Default Value: 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
    /// Range:         [0, 0.99609375]
    /// Bit Depth:     8 bits. Unsigned Q8 numbers.
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_2_activity_normalization_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_2_activity_normalization_lut - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
        /// Range:         [0, 0.99609375]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_2_activity_normalization_lut[64];
    } layer_2_activity_normalization_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_weight_modulation_lut_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   normalized activity-based LUT to control the slope(output/input) when sharpened value magnitude is less
    ///                than the soft threshold
    /// Type:          float
    /// Default Value: 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996
    /// Range:         [0, 0.99609375]
    /// Bit Depth:     8 bits. Unsigned Q8 numbers.
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_2_weight_modulation_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_2_weight_modulation_lut - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996 0.996 0.996 0.996 0.996 0.996 0.996  0.996 0.996
        /// Range:         [0, 0.99609375]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_2_weight_modulation_lut[64];
    } layer_2_weight_modulation_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_soft_threshold_lut_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   level-based soft-thresholding LUT to determine the final sharpened value
    /// Type:          uint
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [0, 255]
    /// Bit Depth:     8 bits. Unsigned Q0 numbers.
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_2_soft_threshold_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_2_soft_threshold_lut - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [0, 255]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_2_soft_threshold_lut[64];
    } layer_2_soft_threshold_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_gain_positive_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   level-based sharpening gain LUT for positive halos
    /// Type:          float
    /// Default Value: 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55
    /// Range:         [0, 7.96875]
    /// Bit Depth:     8 bits. Unsigned Q5 numbers.
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_2_gain_positive_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_2_gain_positive_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55
        /// Range:         [0, 7.96875]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_2_gain_positive_lut[64];
    } layer_2_gain_positive_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_gain_negative_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   level-based sharpening gain LUT for negative halos
    /// Type:          float
    /// Default Value: 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55
    /// Range:         [0, 7.96875]
    /// Bit Depth:     8 bits. Unsigned Q5 numbers.
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_2_gain_negative_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_2_gain_negative_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55 0.55
        /// Range:         [0, 7.96875]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_2_gain_negative_lut[64];
    } layer_2_gain_negative_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_gain_weight_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   normalized activity-based sharpening gain LUT
    /// Type:          float
    /// Default Value: 0.5000 0.5027 0.5108 0.5240 0.5419 0.5639 0.5893 0.6175 0.6475 0.6787 0.7103 0.7416 0.7719 0.8008 0.8278 0.8526 0.8752 0.8953 0.9130 0.9283 0.9414 0.9525 0.9617 0.9692 0.9753 0.9802 0.9841 0.9871 0.9894 0.9912 0.9925 0.9935 0.9943 0.9948 0.9952 0.9954 0.9956 0.9957 0.9958 0.9959 0.9959 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960
    /// Range:         [0, 0.99609375]
    /// Bit Depth:     8 bits. Unsigned Q8 numbers.
    /// Length:        64
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct layer_2_gain_weight_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// layer_2_gain_weight_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0.5000 0.5027 0.5108 0.5240 0.5419 0.5639 0.5893 0.6175 0.6475 0.6787 0.7103 0.7416 0.7719 0.8008 0.8278 0.8526 0.8752 0.8953 0.9130 0.9283 0.9414 0.9525 0.9617 0.9692 0.9753 0.9802 0.9841 0.9871 0.9894 0.9912 0.9925 0.9935 0.9943 0.9948 0.9952 0.9954 0.9956 0.9957 0.9958 0.9959 0.9959 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960 0.9960
        /// Range:         [0, 0.99609375]
        /// Length:        64
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT layer_2_gain_weight_lut[64];
    } layer_2_gain_weight_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_clamp_ul
    ///
    /// Tuning Level:  OftenTune
    /// Description:   normalized activity-based sharpening gain LUT
    /// Type:          int
    /// Default Value: 255
    /// Range:         [-255, 255]
    /// Bit Depth:     9 bits. Signed Q0 number
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_2_clamp_ul;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_clamp_ll
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Manual fixed negative clamping level sharpening
    /// Type:          int
    /// Default Value: -255
    /// Range:         [-255, 255]
    /// Bit Depth:     9 bits. Signed Q0 number
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_2_clamp_ll;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_norm_scale
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   scale factor of 5x5 band-pass filter output (activity)
    /// Type:          float
    /// Default Value: 3.98
    /// Range:         [0, 3.984375]
    /// Bit Depth:     8 bits. unsigned Q6 number
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_2_norm_scale;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_activity_clamp_threshold
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   static clamp of 5x5 band-pass filter output (activity)
    /// Type:          int
    /// Default Value: 255
    /// Range:         [0, 255]
    /// Bit Depth:     8 bits. Unsigned Q0 number
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_2_activity_clamp_threshold;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_l2_norm_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable L2 norm for 5x5 band-pass filter output (activity).0: L1 norm (absolute value), 1: L2 norm (squared
    ///                value)
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1 bit.
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  layer_2_l2_norm_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_activity_adj_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   correction factor for activity based on radial distance
    /// Type:          float
    /// Default Value: 1.0, 1.0, 1.0, 1.0
    /// Range:         [0, 7.96875]
    /// Bit Depth:     8 bits. Unsigned Q5
    /// Length:        4
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_activity_adj_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_activity_adj - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 1.0, 1.0, 1.0, 1.0
        /// Range:         [0, 7.96875]
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_activity_adj[4];
    } radial_activity_adj_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_gain_adj_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   correction factor for gain based on radial distance
    /// Type:          float
    /// Default Value: 1.0, 1.0, 0.8, 0.6
    /// Range:         [0, 7.96875]
    /// Bit Depth:     8 bits. Unsigned Q5
    /// Length:        4
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_gain_adj_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_gain_adj - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 1.0, 1.0, 0.8, 0.6
        /// Range:         [0, 7.96875]
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_gain_adj[4];
    } radial_gain_adj_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_hue_min
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Min hue value for skin detection. Instead of negative (Cb/Cr), absolute value |Cb/Cr| is used in HW
    /// Type:          float
    /// Default Value: -1.7
    /// Range:         [-3.0, -1.0]
    /// Bit Depth:     10bit, Q8, unsigned
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_hue_min;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_hue_max
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Max hue value for skin detection. Instead of negative (Cb/Cr), absolute value |Cb/Cr| is used in HW
    /// Type:          float
    /// Default Value: -0.5
    /// Range:         [-0.99609375, 0.99609375]
    /// Bit Depth:     8bit, Q8, unsigned
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_hue_max;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_y_min
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Minimum Y value for skin.
    /// Type:          float
    /// Default Value: 0.1
    /// Range:         [0, 0.99609375]
    /// Bit Depth:     8bit, Q8, unsigned
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_y_min;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_y_max
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Maximum Y value for skin.
    /// Type:          float
    /// Default Value: 0.9
    /// Range:         [0, 0.99609375]
    /// Bit Depth:     8bit, unsigned Q8
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_y_max;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_saturation_min_ymax
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Minimum saturation value for skin when Y=Y_max.
    /// Type:          float
    /// Default Value: 0.05
    /// Range:         [0.0, 0.4]
    /// Bit Depth:     8bit, unsigned Q8
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_saturation_min_ymax;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_saturation_max_ymax
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Maximum saturation value for skin when Y=Y_max.
    /// Type:          float
    /// Default Value: 0.25
    /// Range:         [0.1, 0.5]
    /// Bit Depth:     8bit, unsigned Q8
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_saturation_max_ymax;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_saturation_min_ymin
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Minimum saturation value for skin when Y=Y_min.
    /// Type:          float
    /// Default Value: 0.25
    /// Range:         [0.0, 0.5]
    /// Bit Depth:     8bit, unsigned Q8
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_saturation_min_ymin;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_saturation_max_ymin
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Maximum saturation value for skin when Y=Y_min.
    /// Type:          float
    /// Default Value: 0.65
    /// Range:         [0.2, 0.99609375]
    /// Bit Depth:     8bit, unsigned Q8
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_saturation_max_ymin;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_boundary_probability
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Denoting the skin-tone probability at the boundary of skin-tone range.
    /// Type:          uint
    /// Default Value: 8
    /// Range:         [1, 15]
    /// Bit Depth:     4 bits. Unsigned Q4
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_boundary_probability;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_percent
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   The portion of skin_tone region with skin-tone probability 1.
    /// Type:          float
    /// Default Value: 12
    /// Range:         [0, 100]
    /// Bit Depth:     Not applicable
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_percent;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_nonskin_to_skin_qratio
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Ratio between Q_nonSkin and Q_skin.
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0.1, 10.0]
    /// Bit Depth:     Not applicable
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  skin_nonskin_to_skin_qratio;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// flat_thresold
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Apply edge smoothing only when maximum AAD (average absolute difference) is larger or equal to
    ///                FlatThreshold.
    /// Type:          int
    /// Default Value: 8
    /// Range:         [0, 25]
    /// Bit Depth:     8uQ0
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  flat_thresold;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// max_smoothing_clamp
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   edge smoothing value change is clamped by max_smoothing_clamp.
    /// Type:          int
    /// Default Value: 8
    /// Range:         [0, 25]
    /// Bit Depth:     8uQ0
    /// Conversion:    no conversion needed
    /// Notes:         previously called texture_threshold
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  max_smoothing_clamp;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// corner_threshold
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Separate corner from edge.
    /// Type:          float
    /// Default Value: 2
    /// Range:         [0, 63.9990234375]
    /// Bit Depth:     16 bits. unsigned Q10
    /// Conversion:    no conversion needed
    /// Notes:         previously called similarity_threshold
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  corner_threshold;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// smoothing_strength
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Blending factor between original pixel and edge smoothed pixel.
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0, 0.9990234375]
    /// Bit Depth:     10 bits. unsigned Q10
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  smoothing_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gain_contrast_positive_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Contrast-based positive halo sharpening gain
    /// Type:          float
    /// Default Value: 1.0000 1.0000 0.9375 0.8750 0.8125 0.7500 0.6875 0.6250 0.5625 0.5000 0.4375 0.3750 0.3125 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500
    /// Range:         [0.00390625, 1]
    /// Bit Depth:     9 bits. unsigned Q8
    /// Length:        32
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gain_contrast_positive_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gain_contrast_positive - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1.0000 1.0000 0.9375 0.8750 0.8125 0.7500 0.6875 0.6250 0.5625 0.5000 0.4375 0.3750 0.3125 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500
        /// Range:         [0.00390625, 1]
        /// Length:        32
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gain_contrast_positive[32];
    } gain_contrast_positive_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gain_contrast_negative_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Contrast-based negative halo sharpening gain LUT
    /// Type:          float
    /// Default Value: 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 0.9375 0.8750 0.8125 0.7500 0.6875 0.6250 0.5625 0.5000 0.4375 0.3750 0.3125 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500
    /// Range:         [0.00390625, 1]
    /// Bit Depth:     9 bits. unsigned Q8
    /// Length:        32
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gain_contrast_negative_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gain_contrast_negative - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 0.9375 0.8750 0.8125 0.7500 0.6875 0.6250 0.5625 0.5000 0.4375 0.3750 0.3125 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500
        /// Range:         [0.00390625, 1]
        /// Length:        32
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gain_contrast_negative[32];
    } gain_contrast_negative_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_gain_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Skin-detection-based sharpening gain
    /// Type:          float
    /// Default Value: 1.0000 1.0000 1.0000 0.9531 0.9063 0.8594 0.8125 0.7656 0.7188 0.7188 0.7188 0.7188 0.7188 0.7188 0.7188 0.7188 0.7188
    /// Range:         [0.00390625, 1]
    /// Bit Depth:     9 bits. unsigned Q8
    /// Length:        17
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct skin_gain_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// skin_gain - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1.0000 1.0000 1.0000 0.9531 0.9063 0.8594 0.8125 0.7656 0.7188 0.7188 0.7188 0.7188 0.7188 0.7188 0.7188 0.7188 0.7188
        /// Range:         [0.00390625, 1]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT skin_gain[17];
    } skin_gain_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_activity_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Skin-detection-based activity gain
    /// Type:          float
    /// Default Value: 1.0000 0.9688 0.9375 0.9063 0.8750 0.8438 0.8125 0.7813 0.7500 0.7188 0.6875 0.6563 0.6250 0.5938 0.5625 0.5313 0.5000
    /// Range:         [0.00390625, 1]
    /// Bit Depth:     9 bits. unsigned Q8
    /// Length:        17
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct skin_activity_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// skin_activity - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1.0000 0.9688 0.9375 0.9063 0.8750 0.8438 0.8125 0.7813 0.7500 0.7188 0.6875 0.6563 0.6250 0.5938 0.5625 0.5313 0.5000
        /// Range:         [0.00390625, 1]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT skin_activity[17];
    } skin_activity_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gain_chroma_positive_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Chroma gradient-based positive halo sharpening gain
    /// Type:          float
    /// Default Value: 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 0.9375 0.8750 0.8125 0.7500 0.6875 0.6250 0.5625 0.5000 0.4375 0.3750 0.3125 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500
    /// Range:         [0.00390625, 1]
    /// Bit Depth:     9 bits. unsigned Q8
    /// Length:        32
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gain_chroma_positive_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gain_chroma_positive - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 0.9375 0.8750 0.8125 0.7500 0.6875 0.6250 0.5625 0.5000 0.4375 0.3750 0.3125 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500 0.2500
        /// Range:         [0.00390625, 1]
        /// Length:        32
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gain_chroma_positive[32];
    } gain_chroma_positive_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gain_chroma_negative_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Chroma gradient-based negative halo sharpening gain LUT
    /// Type:          float
    /// Default Value: 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000
    /// Range:         [0.00390625, 1]
    /// Bit Depth:     9 bits. unsigned Q8
    /// Length:        32
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gain_chroma_negative_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gain_chroma_negative - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000 1.0000
        /// Range:         [0.00390625, 1]
        /// Length:        32
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gain_chroma_negative[32];
    } gain_chroma_negative_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// face_boundary
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   full skin detection is applied when radius less-or-equal to face_boundary * face_radius(from face
    ///                detection).
    /// Type:          float
    /// Default Value: 1.2
    /// Range:         [0, 8]
    /// Bit Depth:     not applicable
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  face_boundary;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// face_transition
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   No skin detection is applied when radius less-or-equal to face_transition * face_radius(from face
    ///                detection).
    /// Type:          float
    /// Default Value: 2
    /// Range:         [0, 8]
    /// Bit Depth:     not applicable
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                  face_transition;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// asf30_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct asf30_enable_sectionStruct
{
    UINT32 reserved;

    INT32  asf_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of skin color based noise reduction
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         if face_enable = 1 and skin_enable = 1, skin color noise reduction is applied on face regions; if
    ///                face_enable = 0 and skin_enable=1, skin color noise reduction is applied on both face regions and non-face
    ///                regions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  skin_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chroma_gradient_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of chroma gradient statistics
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  chroma_gradient_enable;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_asf30_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_asf30_reserveType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_anchor_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   anchor table (distance percentage)for radius based noise reduction
    /// Type:          float
    /// Default Value: 0.25, 0.5, 0.75, 1.0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Length:        4
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_anchor_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_anchor - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0.25, 0.5, 0.75, 1.0
        /// Range:         [0.0, 1.0]
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_anchor[4];
    } radial_anchor_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// face_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of face guided skin color based noise reduction
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         if face_enable = 1 and skin_enable = 1, skin color noise reduction is applied on face regions; if
    ///                face_enable = 0 and skin_enable=1, skin color noise reduction is applied on both face regions and non-face
    ///                regions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  face_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// edge_alignment_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable edge alignment
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  edge_alignment_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_1_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of 1st layer 7x7 ASF
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  layer_1_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// layer_2_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of 2nd layer 13x13 ASF
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  layer_2_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of radial distance based sharpening
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  radial_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// contrast_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of contrast based sharpening.
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  contrast_enable;
};

struct bls12_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// offset
    ///
    /// Tuning Level:  MustTune
    /// Description:   Residual black level to be removed from pixels
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Notes:         BLS removes the residual black level from linearization (and pedestal correction) that is purposed left
    ///                there to avoid low light color tint due to non-zero noise mean after zero-clamping in the pipeline if
    ///                black level is totally removed. After linearization, black levels of the channels are supposed to be
    ///                equal. Same BLS is used on the main pipe and on the statistics track. Effect: Increasing this value alone
    ///                will make image dimmer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// threshold_r
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Hard thresholds of R/Gr/Gb/B channels
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Notes:         Pixel values are 0 if below the thresholds, else remain unchanged if equal or above the thresholds.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT threshold_r;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// threshold_gr
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Hard thresholds of R/Gr/Gb/B channels
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Notes:         Pixel values are 0 if below the thresholds, else remain unchanged if equal or above the thresholds.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT threshold_gr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// threshold_gb
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Hard thresholds of R/Gr/Gb/B channels
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Notes:         Pixel values are 0 if below the thresholds, else remain unchanged if equal or above the thresholds.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT threshold_gb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// threshold_b
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Hard thresholds of R/Gr/Gb/B channels
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Notes:         Pixel values are 0 if below the thresholds, else remain unchanged if equal or above the thresholds.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT threshold_b;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// bls12_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct bls12_enable_sectionStruct
{
    UINT32 reserved;
    INT32  bls_enable;
};

struct bpcbcc50_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hot_pixel_correction_disable
    ///
    /// Tuning Level:  OftenTune
    /// Description:   hot pixel correction enable/disable
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed.
    /// Notes:         Enable bit.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT hot_pixel_correction_disable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cold_pixel_correction_disable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   cold pixel correction enable/disable.
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed.
    /// Notes:         Enable bit
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT cold_pixel_correction_disable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// same_channel_recovery
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Use only same channel information for detection.
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Enable bit
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT same_channel_recovery;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fmax
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Bad pixel maximum threshold factor.
    /// Type:          int
    /// Default Value: 64
    /// Range:         [0, 127]
    /// Bit Depth:     7uQ6
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT fmax;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fmin
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Bad pixel minimum threshold factor.
    /// Type:          int
    /// Default Value: 32
    /// Range:         [0, 127]
    /// Bit Depth:     7uQ6
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT fmin;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bpc_offset
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset used in single bad pixel detection for regular bayer or T1 in HDR mode.
    /// Type:          int
    /// Default Value: 128
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT bpc_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bcc_offset
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset used in couplet bad pixel detection for regular bayer or T1 in HDR mode.
    /// Type:          int
    /// Default Value: 128
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT bcc_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// correction_threshold
    ///
    /// Tuning Level:  OftenTune
    /// Description:   if departure of correction value from the original value is larger than this threshold, the pixel will be
    ///                corrected using the correction value.
    /// Type:          int
    /// Default Value: 128
    /// Range:         [0, 8191]
    /// Bit Depth:     13u
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT correction_threshold;
};

struct cac22_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cac_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   CAC enable bit
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         0x1: enable cac. 0x0: disable cac.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT cac_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// y_spot_thr
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   the threshold on brightness difference between one pixel and its surrounding pixels for luma spot
    ///                detection.
    /// Type:          uint
    /// Default Value: 10
    /// Range:         [0, 63]
    /// Bit Depth:     6u, q6
    /// Notes:         By decreasing the threshold, more points will be detected.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT y_spot_thr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// y_saturation_thr
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   the brightness saturation threshold for luma spot detection.
    /// Type:          uint
    /// Default Value: 960
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Notes:         By decreasing the threshold, more points will be detected.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT y_saturation_thr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c_spot_thr
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   the threshold on color difference between one pixel and its surrounding pixels for color spot detection.
    /// Type:          uint
    /// Default Value: 40
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Notes:         By decreasing the threshold, more points will be detected.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT c_spot_thr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c_saturation_thr
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   the color saturation threshold for color spot detection.
    /// Type:          uint
    /// Default Value: 20
    /// Range:         [0, 511]
    /// Bit Depth:     9u
    /// Notes:         By decreasing the threshold, more points will be detected.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT c_saturation_thr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// cac22_enable_section
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct cac22_enable_sectionStruct
{
    UINT32 reserved;
    INT32  cac_global_enable;
};

struct chromatix_cc12_reserveType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// q_factor
    ///
    /// Tuning Level:  NeverTune
    /// Description:   q factor to adjust precision of color correction matrix
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 3]
    /// Bit Depth:     2bit
    /// Notes:         range 0x0-0x3 maps to Q7-Q10 for CC matrix elements
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 q_factor;
};

struct cc12_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   3x3 color correction matrix elements
    /// Type:          float
    /// Default Value: 1, 0, 0, 0, 1, 0, 0, 0, 1
    /// Range:         [-15.99, 15.99]
    /// Bit Depth:     12bit, {Q7,Q8,Q9,Q10}, signed
    /// Length:        9
    /// Notes:         the sum of each row in the 3x3 color correction matrix should be 1. Higher values for diagonal elements
    ///                result in higher color saturation with higher noise.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct c_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// c - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1, 0, 0, 0, 1, 0, 0, 0, 1
        /// Range:         [-15.99, 15.99]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT c[9];
    } c_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// k_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   offset in color correction
    /// Type:          int
    /// Default Value: 0, 0, 0
    /// Range:         [-4096, 4095]
    /// Bit Depth:     13bit, Q0, signed
    /// Length:        3
    /// Notes:         Negative values will make image darker while positive values make image brighter
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct k_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// k - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0, 0, 0
        /// Range:         [-4096, 4095]
        /// Length:        3
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT k[3];
    } k_tab;
};

struct cc13_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   3x3 color correction matrix elements
    /// Type:          float
    /// Default Value: 1, 0, 0, 0, 1, 0, 0, 0, 1
    /// Range:         [-15.99, 15.99]
    /// Bit Depth:     12bit, {Q7,Q8,Q9,Q10}, signed
    /// Length:        9
    /// Notes:         the sum of each row in the 3x3 color correction matrix should be 1. Higher values for diagonal elements
    ///                result in higher color saturation with higher noise.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct c_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// c - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1, 0, 0, 0, 1, 0, 0, 0, 1
        /// Range:         [-15.99, 15.99]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT c[9];
    } c_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// k_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   offset in color correction
    /// Type:          int
    /// Default Value: 0, 0, 0
    /// Range:         [-4096, 4095]
    /// Bit Depth:     13bit for BPS, 11 bit for PPS, Q0, signed
    /// Length:        3
    /// Notes:         Negative values will make image darker while positive values make image brighter
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct k_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// k - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0, 0, 0
        /// Range:         [-4096, 4095]
        /// Length:        3
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT k[3];
    } k_tab;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_cc13_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_cc13_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// q_factor
    ///
    /// Tuning Level:  NeverTune
    /// Description:   q factor to adjust precision of color correction matrix
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 3]
    /// Bit Depth:     2bit
    /// Notes:         range 0x0-0x3 maps to Q7-Q10 for CC matrix elements
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 q_factor;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// cc13_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct cc13_enable_sectionStruct
{
    UINT32 reserved;
    INT32  cc_enable;
};

struct cs20_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// knee_point_lut_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   luma knee point LUT. Define the luma points users want to control
    /// Type:          float
    /// Default Value: 0,17,34,51,68,85,102,119,136,153,170,187,204,221,238,256
    /// Range:         [0, 256]
    /// Bit Depth:     8bit, unsigned in chromatix (hw: 10u)
    /// Length:        16
    /// Conversion:    i.   cs_luma_threshold1 to knee_point_lut[1]. ii.    cs_luma_threshold2 to knee_point_lut[2]. iii.   cs_luma_threshold3 to knee_point_lut[13]. iv.   cs_luma_threshold4 to knee_point_lut[14].
    /// Notes:         i. LUTs are always programmed in ascending order. ii. First LUT is always 0. iii. Last LUT is always Max.
    ///                iv. No two entries for the LUT can be equal. v. must satisfy (knee_point_lut[i+1] –
    ///                knee_point_lut[i])>=2^(luma_q+1). By default, luma_q=1, so must have (knee_point_lut[i+1] –
    ///                knee_point_lut[i])>=1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct knee_point_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// knee_point_lut - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,17,34,51,68,85,102,119,136,153,170,187,204,221,238,256
        /// Range:         [0, 256]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT knee_point_lut[16];
    } knee_point_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// y_weight_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   define luma weight for chroma suppression at luma knee points. Higher value makes less suppression and
    ///                lower value makes more suppression
    /// Type:          float
    /// Default Value: 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     11uQ10
    /// Length:        16
    /// Conversion:    i. y_weight_lut[0] = y_weight_lut[0] = 0.0f. ii. y_weight_lut[2 … 13] = 1.0f.  iii. y_weight_lut[14] = y_weight_lut[15] = 0.0f.
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct y_weight_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_weight_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
        /// Range:         [0.0, 1.0]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT y_weight_lut[16];
    } y_weight_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c_thr1_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Define chroma threshold1. Chroma value smaller than this threshold will be suppressed with y_weight.
    ///                Higher value makes more suppression and lower value makes less suppression
    /// Type:          float
    /// Default Value: 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    /// Range:         [0, 128]
    /// Bit Depth:     7bit, unsigned in chromatix (hw: 9u)
    /// Length:        16
    /// Conversion:    i. cs_chroma_threshold1 to c_thr1_lut[0 .. 15] all the same
    /// Notes:         related with c_thr2_lut
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct c_thr1_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// c_thr1_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        /// Range:         [0, 128]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT c_thr1_lut[16];
    } c_thr1_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c_thr2_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Define chroma threshold2. Chroma value bigger than this threshold will not be suppressed. Higher value
    ///                makes more suppression and lower value makes less suppression
    /// Type:          float
    /// Default Value: 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
    /// Range:         [1, 128]
    /// Bit Depth:     7bit, unsigned in chromatix (hw: 9u)
    /// Length:        16
    /// Conversion:    i. cs_chroma_threshold2 to c_thr2_lut[0 .. 15] all the same
    /// Notes:         i. This value must be bigger then c_thr1_lut. ii. must satisfy (c_th2_lut[i] –
    ///                c_th1_lut[i])>=2^(chroma_q+1). By default, chroma_q=1, so must have (c_th2_lut[i] – c_th1_lut[i])>=1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct c_thr2_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// c_thr2_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
        /// Range:         [1, 128]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT c_thr2_lut[16];
    } c_thr2_lut_tab;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_cs20_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_cs20_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// luma_q
    ///
    /// Tuning Level:  NeverTune
    /// Description:   Q factor to define constraints on knee_point_lut
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 3]
    /// Bit Depth:     2u
    /// Notes:         knee_point_lut must satisfy (knee_point_lut[i+1] – knee_point_lut[i])>=2^(luma_q+1). By default, luma_q=1,
    ///                so must have (knee_point_lut[i+1] – knee_point_lut[i])>=1.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 luma_q;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chroma_q
    ///
    /// Tuning Level:  NeverTune
    /// Description:   Q factor to define constraints on c_thr1_lut and c_thr2_lut
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 3]
    /// Bit Depth:     2u
    /// Notes:         must satisfy (c_th2_lut[i] – c_th1_lut[i])>=2^(chroma_q+1). By default, chroma_q=1, so must have
    ///                (c_th2_lut[i] – c_th1_lut[i])>=1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 chroma_q;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// cs20_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct cs20_enable_sectionStruct
{
    UINT32 reserved;
    INT32  chroma_suppression_enable;
};

struct chromatix_cst12_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c_x0_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   lower clamps for the 1st, 2nd and 3rd components
    /// Type:          unsignedInt
    /// Default Value: 0, 0, 0
    /// Range:         [0, 1023]
    /// Bit Depth:     10bits
    /// Length:        3
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct c_x0_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// c_x0 - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0, 0, 0
        /// Range:         [0, 1023]
        /// Length:        3
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 c_x0[3];
    } c_x0_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c_x1_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   upper clamps for the 1st, 2nd and 3rd components
    /// Type:          unsignedInt
    /// Default Value: 1023, 1023, 1023
    /// Range:         [0, 1023]
    /// Bit Depth:     10 bits
    /// Length:        3
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct c_x1_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// c_x1 - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 1023, 1023, 1023
        /// Range:         [0, 1023]
        /// Length:        3
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 c_x1[3];
    } c_x1_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// m_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   elements of 3x3 color space transform matix
    /// Type:          float
    /// Default Value: 1, 0, 0, 0, 1, 0, 0, 0, 1
    /// Range:         [-3.999, 3.999]
    /// Bit Depth:     float
    /// Length:        9
    /// Conversion:    no conversion needed
    /// Notes:         larger values mean more cold pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct m_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// m - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 1, 0, 0, 0, 1, 0, 0, 0, 1
        /// Range:         [-3.999, 3.999]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT m[9];
    } m_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// o_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   additive offsets for the 1st, 2nd and 3rd components
    /// Type:          int
    /// Default Value: 0, 0, 0
    /// Range:         [0, 1023]
    /// Bit Depth:     10bits
    /// Length:        3
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct o_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// o - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0, 0, 0
        /// Range:         [0, 1023]
        /// Length:        3
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        INT32 o[3];
    } o_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// s_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   subtractive offsets for the 1st, 2nd and 3rd components
    /// Type:          xs:int
    /// Default Value: 0, 0, 0
    /// Range:         [-1024, 1023]
    /// Bit Depth:     11bits, signed
    /// Length:        3
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct s_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// s - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0, 0, 0
        /// Range:         [-1024, 1023]
        /// Length:        3
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        INT32 s[3];
    } s_tab;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// cst12_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct cst12_enable_sectionStruct
{
    UINT32 reserved;
    INT32  cst_enable;
};

struct cv12_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// r_to_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Coefficients for luma calculation
    /// Type:          float
    /// Default Value: 0.299
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT r_to_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// g_to_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Coefficients for luma calculation
    /// Type:          float
    /// Default Value: 0.587
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT g_to_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// b_to_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Coefficients for luma calculation
    /// Type:          float
    /// Default Value: 0.114
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT b_to_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// y_offset
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Constant offset for luma calculation
    /// Type:          float
    /// Default Value: 0
    /// Range:         [-16384, 16383]
    /// Bit Depth:      (9+e)sQ0
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT y_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ap
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Chroma enhancement matrix parameters
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT ap;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// am
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Chroma enhancement matrix parameters
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT am;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bp
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Chroma enhancement matrix parameters
    /// Type:          float
    /// Default Value: -0.338
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT bp;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bm
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Chroma enhancement matrix parameters
    /// Type:          float
    /// Default Value: -0.338
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT bm;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cp
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Chroma enhancement matrix parameters
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT cp;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cm
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Chroma enhancement matrix parameters
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT cm;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dp
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Chroma enhancement matrix parameters
    /// Type:          float
    /// Default Value: -0.162
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT dp;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dm
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Chroma enhancement matrix parameters
    /// Type:          float
    /// Default Value: -0.162
    /// Range:         [-7.99, 7.99]
    /// Bit Depth:     12sQ8
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT dm;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// kcr
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Constant offset for CSC matrix
    /// Type:          float
    /// Default Value: 128
    /// Range:         [-65536, 65535]
    /// Bit Depth:     (11+e)sQ0
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT kcr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// kcb
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Constant offset for CSC matrix
    /// Type:          float
    /// Default Value: 128
    /// Range:         [-65536, 65535]
    /// Bit Depth:     (11+e)sQ0
    /// Conversion:    No Conversion needed
    /// Notes:         See special effect defaults below
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT kcb;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// cv12_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct cv12_enable_sectionStruct
{
    UINT32 reserved;
    INT32  cv_enable;
};

struct demosaic36_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// edge_det_weight
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   weight of classifiers in edge detection
    /// Type:          float
    /// Default Value: 0.1
    /// Range:         [0, 0.99]
    /// Bit Depth:     10u
    /// Notes:         The vertical/horizontal detection becomes more aggressive as edge_det_weight increases. When
    ///                edge_det_weight = 0, no horizontal/vertical detection is detected. When edge_det_weight = 0.99, tend to
    ///                detect either horizontal or vertical direction.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT edge_det_weight;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// edge_det_noise_offset
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   noise level in edge detection
    /// Type:          uint
    /// Default Value: 128
    /// Range:         [1,400]
    /// Bit Depth:     12u
    /// Notes:         The vertical/horizontal edge detection becomes more aggressive as edge_det_noise_offset ak decreases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT edge_det_noise_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dis_directional_g
    ///
    /// Tuning Level:  NeverTune
    /// Description:   disable the directional G interpolation
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Notes:         0: enable directional interpolation (default). 1: disable directional interpolation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT dis_directional_g;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dis_directional_rb
    ///
    /// Tuning Level:  NeverTune
    /// Description:   disable the directional RB interpolation
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         0: enable directional interpolation (default). 1: disable directional interpolation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT dis_directional_rb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lambda_g
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   low-frequency component weighting in G interpolation
    /// Type:          int
    /// Default Value: 128
    /// Range:         [77, 178]
    /// Bit Depth:     8u, q8
    /// Notes:         The larger lambda_g, the more blurry the Demosaic output
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT lambda_g;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lambda_rb
    ///
    /// Tuning Level:  NeverTune
    /// Description:   low-frequency component weighting in RB interpolation
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 255]
    /// Bit Depth:     8u, q8
    /// Notes:         The larger lambda_rb, the more blurry the Demosaic output
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT lambda_rb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// en_dyna_clamp_g
    ///
    /// Tuning Level:  NeverTune
    /// Description:   enable dynamic clamping in G interpolation
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         0: enable dynamic clamping. 1: disable dynamic clamping.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT en_dyna_clamp_g;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// en_dyna_clamp_rb
    ///
    /// Tuning Level:  NeverTune
    /// Description:   enable dynamic clamping in RB interpolation
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Notes:         0: enable dynamic clamping. 1: disable dynamic clamping.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT en_dyna_clamp_rb;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// demosaic36_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct demosaic36_enable_sectionStruct
{
    UINT32 reserved;
    INT32  demosaic_enable;
};

struct chromatix_demux13_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// green_gain_even
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   channel 0 (green) gain for even rows
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 31.999]
    /// Bit Depth:     float in header, HW parameter is 15uQ10
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT green_gain_even;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// green_gain_odd
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   channel 0 (green) gain for odd rows
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 31.999]
    /// Bit Depth:     float in header, HW parameter is 15uQ10
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT green_gain_odd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blue_gain
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   channel 1 (blue) gain
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 31.999]
    /// Bit Depth:     float in header, HW parameter is 15uQ10
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT blue_gain;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// red_gain
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   channel 2 (red) gain
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 31.999]
    /// Bit Depth:     float in header, HW parameter is 15uQ10
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT red_gain;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// demux13_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct demux13_enable_sectionStruct
{
    UINT32 reserved;
    INT32  demux_enable;
};

struct gamma15_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// table - Array
    ///
    /// Tuning Level:  MustTune
    /// Default Value: 0, 16, 32, 48, 64, 75, 86, 97, 108, 117, 126, 135, 144, 152, 160, 168, 176, 183, 191, 198, 205, 212, 219, 226, 232, 239, 245, 251, 258, 264, 270, 276, 282, 287, 293, 299, 304, 310, 315, 321, 326, 331, 336, 342, 347, 352, 357, 362, 367, 372, 377, 381, 386, 391, 396, 400, 405, 410, 414, 419, 423, 428, 432, 437, 441, 445, 450, 454, 458, 462, 467, 471, 475, 479, 483, 487, 491, 495, 499, 503, 507, 511, 515, 519, 523, 527, 531, 534, 538, 542, 546, 550, 553, 557, 561, 564, 568, 572, 575, 579, 583, 586, 590, 593, 597, 600, 604, 607, 611, 614, 618, 621, 625, 628, 631, 635, 638, 641, 645, 648, 651, 655, 658, 661, 665, 668, 671, 674, 678, 681, 684, 687, 690, 693, 697, 700, 703, 706, 709, 712, 715, 718, 722, 725, 728, 731, 734, 737, 740, 743, 746, 749, 752, 755, 758, 761, 764, 767, 769, 772, 775, 778, 781, 784, 787, 790, 793, 795, 798, 801, 804, 807, 810, 812, 815, 818, 821, 824, 826, 829, 832, 835, 837, 840, 843, 846, 848, 851, 854, 856, 859, 862, 865, 867, 870, 873, 875, 878, 881, 883, 886, 888, 891, 894, 896, 899, 901, 904, 907, 909, 912, 914, 917, 919, 922, 925, 927, 930, 932, 935, 937, 940, 942, 945, 947, 950, 952, 955, 957, 960, 962, 965, 967, 969, 972, 974, 977, 979, 982, 984, 987, 989, 991, 994, 996, 999, 1001, 1003, 1006, 1008, 1010, 1013, 1015, 1018, 1020, 1022, 1023
    /// Range:         [0,1023]
    /// Length:        257
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT table[257];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// enable_section
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct gamma15_enable_sectionStruct
{
    UINT32 reserved;
    INT32  gamma_enable;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// gamma15_channel_dataType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct gamma15_channel_dataType
{
    /// channel_rgb_type
    /// Comments:      Channel type selection for R/G/B/All
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class  channel_rgb_type
    {
        channel_G   = 0,
        channel_B   = 1,
        channel_R   = 2,
        channel_RGB = 3
    } channel_type;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gamma15_rgn_data
    ///
    /// Tuning Level:  MustTune
    /// Description:   Gamma table per channel
    /// Type:          int
    /// Default Value: 0, 16, 32, 48, 64, 75, 86, 97, 108, 117, 126, 135, 144, 152, 160, 168, 176, 183, 191, 198, 205, 212, 219, 226, 232, 239, 245, 251, 258, 264, 270, 276, 282, 287, 293, 299, 304, 310, 315, 321, 326, 331, 336, 342, 347, 352, 357, 362, 367, 372, 377, 381, 386, 391, 396, 400, 405, 410, 414, 419, 423, 428, 432, 437, 441, 445, 450, 454, 458, 462, 467, 471, 475, 479, 483, 487, 491, 495, 499, 503, 507, 511, 515, 519, 523, 527, 531, 534, 538, 542, 546, 550, 553, 557, 561, 564, 568, 572, 575, 579, 583, 586, 590, 593, 597, 600, 604, 607, 611, 614, 618, 621, 625, 628, 631, 635, 638, 641, 645, 648, 651, 655, 658, 661, 665, 668, 671, 674, 678, 681, 684, 687, 690, 693, 697, 700, 703, 706, 709, 712, 715, 718, 722, 725, 728, 731, 734, 737, 740, 743, 746, 749, 752, 755, 758, 761, 764, 767, 769, 772, 775, 778, 781, 784, 787, 790, 793, 795, 798, 801, 804, 807, 810, 812, 815, 818, 821, 824, 826, 829, 832, 835, 837, 840, 843, 846, 848, 851, 854, 856, 859, 862, 865, 867, 870, 873, 875, 878, 881, 883, 886, 888, 891, 894, 896, 899, 901, 904, 907, 909, 912, 914, 917, 919, 922, 925, 927, 930, 932, 935, 937, 940, 942, 945, 947, 950, 952, 955, 957, 960, 962, 965, 967, 969, 972, 974, 977, 979, 982, 984, 987, 989, 991, 994, 996, 999, 1001, 1003, 1006, 1008, 1010, 1013, 1015, 1018, 1020, 1022, 1023
    /// Range:         [0,1023]
    /// Bit Depth:     (16+2*b) bit, signed. (8+b) LSBs base (unsigned), (8+b) MSBs delta (signed).
    /// Length:        257
    /// Conversion:    no conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    gamma15_rgn_dataType gamma15_rgn_data;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// gamma15_cct_dataType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct gamma15_cct_dataType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cct_trigger
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct trigger_pt_type
    {
        FLOAT start;
        FLOAT end;
    } cct_trigger;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cct_data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct cct_dataStruct
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// mod_gamma15_channel_data - Array
        /// Min Length:    1
        /// Max Length:    3
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32                    mod_gamma15_channel_dataCount;
        UINT32                    mod_gamma15_channel_dataID;
        gamma15_channel_dataType  mod_gamma15_channel_data[3];
    } cct_data;
};

struct gamma16_channel_dataType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// channel_type
    /// Comments:      Channel type selection for R/G/B/All
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 channel_type;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// table - Array
    ///
    /// Tuning Level:  MustTune
    /// Default Value: 0, 64, 108, 144, 176, 205, 232, 258, 282, 304, 326, 347, 367, 386, 405, 423, 441, 458, 475, 491, 507, 523, 538, 553, 568, 583, 597, 611, 625, 638, 651, 665, 678, 690, 703, 715, 728, 740, 752, 764, 775, 787, 798, 810, 821, 832, 843, 854, 865, 875, 886, 896, 907, 917, 927, 937, 947, 957, 967, 977, 987, 996, 1006, 1015, 1023
    /// Range:         [0,1023]
    /// Length:        65
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT table[65];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// gamma16_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct gamma16_enable_sectionStruct
{
    UINT32 reserved;
    INT32  gamma_enable;
};

struct gic30_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// enable_gic
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   GIC enable bit
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         0x1: enable GIC/PNR. 0x0: disable GIC/PNR.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  enable_gic;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// enable_pnr
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   PNR enable bit
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         0x1: enable GIC/PNR. 0x0: disable GIC/PNR.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  enable_pnr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_std_lut_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   65-entry shot noise profile (std) from measurement.
    /// Type:          float
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [0, 512.0]
    /// Bit Depth:     14u
    /// Length:        65
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_std_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_std_lut - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [0, 512.0]
        /// Length:        65
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_std_lut[65];
    } noise_std_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gic_noise_scale
    ///
    /// Tuning Level:  OftenTune
    /// Description:   noise scale factor for GIC correction
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0, 15.98]
    /// Bit Depth:     10u, q6
    /// Notes:         GIC correction will be stronger as gic_noise_scale increases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  gic_noise_scale;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gic_correction_strength
    ///
    /// Tuning Level:  OftenTune
    /// Description:   GIC correction strength
    /// Type:          float
    /// Default Value: 0.6
    /// Range:         [0, 1]
    /// Bit Depth:     9u, q8
    /// Notes:         GIC correction will be stronger as gic_correction_strength increases. 256 means full strength.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  gic_correction_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// thin_line_noise_offset
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   noise offset for edge-thin line detection in GIC
    /// Type:          uint
    /// Default Value: 400
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Notes:         GIC correction will be stronger and less edge/thin line will be detected as thin_line_noise_offset
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  thin_line_noise_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// pnr_noise_scale_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   noise scale factor for PNR correction
    /// Type:          float
    /// Default Value: 1.0 1.0 1.0 1.0
    /// Range:         [0, 15.98]
    /// Bit Depth:     10u, q6
    /// Length:        4
    /// Notes:         PNR correction will be stronger as pnr_noise_scale increases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct pnr_noise_scale_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// pnr_noise_scale - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 1.0 1.0 1.0 1.0
        /// Range:         [0, 15.98]
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT pnr_noise_scale[4];
    } pnr_noise_scale_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// pnr_correction_strength
    ///
    /// Tuning Level:  OftenTune
    /// Description:   PNR correction strength
    /// Type:          float
    /// Default Value: 0.6
    /// Range:         [0, 1]
    /// Bit Depth:     9u, q8
    /// Notes:         PNR correction will be stronger as pnr_correction_strength increases. 256 means full strength.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  pnr_correction_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_pnr_str_adj_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   radial adjustment for peak noise reduction (PNR) strength
    /// Type:          float
    /// Default Value: 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
    /// Range:         [0, 15.98]
    /// Bit Depth:     10u, q6
    /// Length:        7
    /// Notes:         PNR will be stronger as radial_pnr_str_adj increases. In general, radial_pnr_str_adj is a monotonically
    ///                increasing curve.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_pnr_str_adj_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_pnr_str_adj - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
        /// Range:         [0, 15.98]
        /// Length:        7
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_pnr_str_adj[7];
    } radial_pnr_str_adj_tab;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_gic30_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_gic30_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_anchor_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   radial anchor table (distance percentage)
    /// Type:          float
    /// Default Value: 0, 0.2, 0.3, 0.4, 0.6, 0.8, 1.0
    /// Range:         [0, 1]
    /// Bit Depth:     10u
    /// Length:        7
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_anchor_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_anchor - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0, 0.2, 0.3, 0.4, 0.6, 0.8, 1.0
        /// Range:         [0, 1]
        /// Length:        7
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_anchor[7];
    } radial_anchor_tab;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// gic30_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct gic30_enable_sectionStruct
{
    UINT32 reserved;
    INT32  gic_global_enable;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// gra10_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct gra10_enable_sectionStruct
{
    UINT32 reserved;
    INT32  gra_enable;
};

struct gra10_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// grain_strength
    ///
    /// Tuning Level:  MustTune
    /// Description:   number of bits of grain -E.g. This value contains the number of value bits to generate for grain (plus
    ///                additional 1 sign bits). For value of 3, the grain “noise” added is in the range from -7 to 7. . Larger
    ///                value means more noise.
    /// Type:          uint
    /// Default Value: 3
    /// Range:         [0, 4]
    /// Bit Depth:     3u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             grain_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// y_weight_lut_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   scale-factor to use as a function of luminance of the pixel. Larger value means more noise.
    /// Default Value: 255, 247, 239, 231, 223, 215, 207, 199, 191, 183, 175, 167, 159, 151, 143, 135, 127, 119, 111, 103, 95,  87,  79,  71,  63,  55,  47,  39,  31,  23,  15, 7
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Length:        32
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct y_weight_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// y_weight_lut - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 255, 247, 239, 231, 223, 215, 207, 199, 191, 183, 175, 167, 159, 151, 143, 135, 127, 119, 111, 103, 95,  87,  79,  71,  63,  55,  47,  39,  31,  23,  15, 7
        /// Range:         [0, 255]
        /// Length:        32
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT y_weight_lut[32];
    } y_weight_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cb_weight_lut_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   scale-factor to use as a function of cb of the pixel. Larger value means more noise.
    /// Default Value: 255
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Length:        32
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct cb_weight_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// cb_weight_lut - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Length:        32
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT cb_weight_lut[32];
    } cb_weight_lut_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cr_weight_lut_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   scale-factor to use as a function of cr of the pixel. . Larger value means more noise.
    /// Default Value: 255
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Length:        32
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct cr_weight_lut_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// cr_weight_lut - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 255
        /// Range:         [0, 255]
        /// Length:        32
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT cr_weight_lut[32];
    } cr_weight_lut_tab;
};

struct chromatix_gra10_reserveType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// enable_dithering_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Enables luma dithering bits. . (This is only applicable when input has 10bpp and output 8bpp).
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Constant: no dynamic configuration even when control variable change.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 enable_dithering_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// enable_dithering_c
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Enables chroma dithering bits. . (This is only applicable when input has 10bpp and output 8bpp).
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Constant: no dynamic configuration even when control variable change.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 enable_dithering_c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mcg_a
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Reserved parameter. To be used only in case of grain problems. Has to be an odd number.
    /// Type:          uint
    /// Default Value: 169764749
    /// Range:         [1, 268435455]
    /// Bit Depth:     28u
    /// Conversion:    No Conversion needed
    /// Notes:         Hide in GUI ! Constant: no dynamic configuration even when control variable change.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mcg_a;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skip_ahead_a_jump
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Reserved parameter. To be used only in case of grain problems.
    /// Type:          uint
    /// Default Value: 244113221
    /// Range:         [0, 268435455]
    /// Bit Depth:     28u
    /// Conversion:    No Conversion needed
    /// Notes:         Hide in GUI ! Constant: no dynamic configuration even when control variable change.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 skip_ahead_a_jump;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_gtm10_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_gtm10_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// v2_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable gtm v2
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 v2_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// manual_curve_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable gtm manual curve
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 manual_curve_enable;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// gtm10_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct gtm10_enable_sectionStruct
{
    UINT32 reserved;
    INT32  gtm_enable;
};

struct gtm10_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// maxval_th
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Lower bound of input Max value, to avoid non-linear mapping small/low dynamic range scenes to full output
    ///                range.
    /// Type:          uint
    /// Default Value: 1024
    /// Range:         [0,16383]
    /// Bit Depth:     N/A
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  maxval_th;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// key_min_th
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Key low threshold, constrain the Key to not fall below this to avoid making dark scenes too bright.
    /// Type:          uint
    /// Default Value: 64
    /// Range:         [0,16383]
    /// Bit Depth:     N/A
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  key_min_th;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// key_max_th
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Key high threshold, constrain the Key to not go above this to avoid making bright scenes too dark.
    /// Type:          uint
    /// Default Value: 4096
    /// Range:         [0,16383]
    /// Bit Depth:     N/A
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  key_max_th;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// key_hist_bin_weight
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Adjustment for average pixel value within a bin.
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0.0 ,1.0]
    /// Bit Depth:     N/A
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  key_hist_bin_weight;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// yout_maxval
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   output max value, should reflect output bit width.
    /// Type:          uint
    /// Default Value: 16383
    /// Range:         [0,16383]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  yout_maxval;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// minval_th
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   The minimum luma in image is not larger than this threshold. This threshold always keep minimum luma
    ///                smaller than a preset value
    /// Type:          uint
    /// Default Value: 128
    /// Range:         [0,1024]
    /// Bit Depth:     N/A
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  minval_th;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a_middletone
    ///
    /// Tuning Level:  MustTune
    /// Description:   Target middle tone target after GTM
    /// Type:          float
    /// Default Value: 0.12
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  a_middletone;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// middletone_w
    ///
    /// Tuning Level:  MustTune
    /// Description:   GTM strength, affect the target image brightness
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  middletone_w;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// temporal_w
    ///
    /// Tuning Level:  MustTune
    /// Description:   Temporal filter weight, between 0 and 1, larger value would cause slower adaptation, should be actively
    ///                tuned
    /// Type:          float
    /// Default Value: 0.92
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  temporal_w;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// max_percentile
    ///
    /// Tuning Level:  MustTune
    /// Description:   smaller than the max_percentile of pixels will be used in bHisto.
    /// Type:          float
    /// Default Value: 0.999999
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  max_percentile;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// min_percentile
    ///
    /// Tuning Level:  MustTune
    /// Description:   larger than the min_percentile of pixels will be used in bHisto..
    /// Type:          float
    /// Default Value: 1E-06
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  min_percentile;
    FLOAT                  reserved_1;
    FLOAT                  reserved_2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// extra_ratio_factor
    ///
    /// Tuning Level:  OftenTune
    /// Description:   extra ratio of GTM curve for auto generation
    /// Type:          float
    /// Default Value: 0.25
    /// Range:         [0.0, 8.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  extra_ratio_factor;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dark_index_range
    ///
    /// Tuning Level:  OftenTune
    /// Description:   index range with (extra_ratio+base_ratio) of GTM curve for auto generation
    /// Type:          uint
    /// Default Value: 8
    /// Range:         [0, 32]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  dark_index_range;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// yratio_base_manual_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   manual yratio base curve
    /// Type:          uint
    /// Default Value: 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4096
    /// Range:         [0, 262144]
    /// Bit Depth:     18uQ12
    /// Length:        65
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct yratio_base_manual_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// yratio_base_manual - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4906, 4096
        /// Range:         [0, 262144]
        /// Length:        65
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT yratio_base_manual[65];
    } yratio_base_manual_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// manual_curve_strength
    ///
    /// Tuning Level:  OftenTune
    /// Description:   manual curve strength
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 4.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  manual_curve_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// midlight_threshold_low
    ///
    /// Tuning Level:  OftenTune
    /// Description:   ending-point for linear blending to 45 degree line for low light
    /// Type:          uint
    /// Default Value: 776
    /// Range:         [0, 16383]
    /// Bit Depth:     14
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  midlight_threshold_low;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// midlight_threshold_high
    ///
    /// Tuning Level:  OftenTune
    /// Description:   starting-point for linear blending to 45 degree line for high light
    /// Type:          uint
    /// Default Value: 1024
    /// Range:         [0, 16383]
    /// Bit Depth:     14
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  midlight_threshold_high;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lowlight_w
    ///
    /// Tuning Level:  OftenTune
    /// Description:   strength for linear blending to 45 degree line for low light
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  lowlight_w;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// highlight_w
    ///
    /// Tuning Level:  OftenTune
    /// Description:   strength for linear blending to 45 degree line for high light
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  highlight_w;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// max_ratio
    ///
    /// Tuning Level:  OftenTune
    /// Description:   max ratio for Yratio
    /// Type:          float
    /// Default Value: 4
    /// Range:         [0.0, 16.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  max_ratio;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// luma_peak_th0
    ///
    /// Tuning Level:  OftenTune
    /// Description:   luma peak low threshold for histogram stretch
    /// Type:          uint
    /// Default Value: 6144
    /// Range:         [0, 16383]
    /// Bit Depth:     14
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  luma_peak_th0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// luma_peak_th1
    ///
    /// Tuning Level:  OftenTune
    /// Description:   luma peak high threshold for histogram stretch
    /// Type:          uint
    /// Default Value: 8192
    /// Range:         [0, 16383]
    /// Bit Depth:     14
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  luma_peak_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// stretch_gain_0
    ///
    /// Tuning Level:  OftenTune
    /// Description:   histogram stretch gain for dark image
    /// Type:          float
    /// Default Value: 1.2
    /// Range:         [1.0, 2.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  stretch_gain_0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// stretch_gain_1
    ///
    /// Tuning Level:  OftenTune
    /// Description:   histogram stretch gain for bright image
    /// Type:          float
    /// Default Value: 1
    /// Range:         [1.0, 2.0]
    /// Bit Depth:     N/A
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  stretch_gain_1;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_hdr20_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_hdr20_reserveType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_recon_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Enable RECON block of HDR
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 hdr_recon_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_mac_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Enable MAC block of HDR
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 hdr_mac_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_msb_align
    ///
    /// Tuning Level:  NeverTune
    /// Description:   MSB Align for HDR module
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 hdr_msb_align;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_zrec_g_grad_dth_log2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   This value represents threshold 2 for the green pixel gradient delta for directional interpolation.
    /// Type:          uint
    /// Default Value: 5
    /// Range:         [0,12]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 hdr_zrec_g_grad_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_zrec_rb_grad_dth_log2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   This value represents threshold 2 for the red/blue pixel gradient delta for directional interpolation.
    /// Type:          uint
    /// Default Value: 5
    /// Range:         [0,12]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 hdr_zrec_rb_grad_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_edge_lpf_tap0
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Tap0 value for low-pass filter applied before edge detection.
    /// Type:          uint
    /// Default Value: 3
    /// Range:         [0,5]
    /// Bit Depth:     3u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 recon_edge_lpf_tap0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_motion_dilation
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Size of motion dilation max filter, 5 means -5 to 5, i.e. 11-tap filter.
    /// Type:          uint
    /// Default Value: 5
    /// Range:         [0,5]
    /// Bit Depth:     3u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_motion_dilation;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_motion0_dt0
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   An additive term to noise estimate, to avoid dividing by zero.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [1,63]
    /// Bit Depth:     6u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_motion0_dt0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_low_light_dth_log2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   log2(th2-th1) for low light switching.
    /// Type:          uint
    /// Default Value: 6
    /// Range:         [2,14]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_low_light_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_smooth_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Enable motion adaptive smoothing, an optional step in MAC.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_smooth_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_smooth_th1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Th1 for motion adaptive smoothing.
    /// Type:          uint
    /// Default Value: 192
    /// Range:         [0,256]
    /// Bit Depth:     9u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_smooth_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_smooth_dth_log2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   log2(th2-th1) for motion adaptive smoothing.
    /// Type:          uint
    /// Default Value: 6
    /// Range:         [2,8]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_smooth_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_smooth_tap0
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Tap0 value for the low-pass filter in motion adaptive smoothing.
    /// Type:          uint
    /// Default Value: 5
    /// Range:         [0, 5]
    /// Bit Depth:     3u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_smooth_tap0;
};

struct hdr20_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_min_factor
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents min to mid factor to force vertical edge direction
    /// Type:          uint
    /// Default Value: 12
    /// Range:         [0, 31]
    /// Bit Depth:     5u
    /// Conversion:    No Conversion needed
    /// Notes:         range: 0-16 is meaningful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_min_factor;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_flat_region_th
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the threshold for the flat region in edge detection
    /// Type:          uint
    /// Default Value: 64
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_flat_region_th;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_h_edge_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents threshold 1 for the horizontal edge switching function
    /// Type:          uint
    /// Default Value: 16
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_h_edge_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_h_edge_dth_log2
    ///
    /// Tuning Level:  MustTune
    /// Description:   log2(th2-th1) for horizontal edge switching function.
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [4,7]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_h_edge_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_motion_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents threshold 1 for the motion switching function.
    /// Type:          uint
    /// Default Value: 184
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_motion_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_motion_dth_log2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Brief description: log2(th2-th1) for motion switching function.
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [4,7]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_motion_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_dark_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents threshold 1 for dark noise switching function.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_dark_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_dark_dth_log2
    ///
    /// Tuning Level:  MustTune
    /// Description:   Brief description: log2(th2-th1) for dark noise switching function.
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [0,4]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_dark_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_zrec_prefilt_tap0
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the strength of the Zigzag HDR prefilter.
    /// Type:          uint
    /// Default Value: 16
    /// Range:         [0,63]
    /// Bit Depth:     6u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT hdr_zrec_prefilt_tap0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_zrec_g_grad_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents threshold 1 for the green pixel gradient for directional interpolation.
    /// Type:          uint
    /// Default Value: 32
    /// Range:         [0,4095]
    /// Bit Depth:     12u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT hdr_zrec_g_grad_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_zrec_rb_grad_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents threshold 1 for the red/blue pixel gradient for directional interpolation.
    /// Type:          uint
    /// Default Value: 32
    /// Range:         [0,4095]
    /// Bit Depth:     12u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT hdr_zrec_rb_grad_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_motion0_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   Noise model is: noise = th2*sqrt(gain*intensity)+th1.
    /// Type:          uint
    /// Default Value: 60
    /// Range:         [0,1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_motion0_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_motion0_th2
    ///
    /// Tuning Level:  MustTune
    /// Description:   Noise model is: noise = th2*sqrt(gain*intensity)+th1. This value represents the noise-luma slope for
    ///                motion detection.
    /// Type:          uint
    /// Default Value: 24
    /// Range:         [0,255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_motion0_th2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_motion0_strength
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the motion adaptation strength value.
    /// Type:          uint
    /// Default Value: 8
    /// Range:         [0,16]
    /// Bit Depth:     5u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_motion0_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_low_light_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the threshold 1 value for low light switching.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0,16383]
    /// Bit Depth:     14u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_low_light_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_low_light_strength
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the log light switching strength value.
    /// Type:          uint
    /// Default Value: 8
    /// Range:         [0,16]
    /// Bit Depth:     5u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_low_light_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_high_light_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the threshold 1 value for high light switching.
    /// Type:          uint
    /// Default Value: 232
    /// Range:         [0,16383]
    /// Bit Depth:     14u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_high_light_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_high_light_dth_log2
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the log2(th2 ?th1) value for high light switching.
    /// Type:          uint
    /// Default Value: 10
    /// Range:         [2, 14]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_high_light_dth_log2;
};

struct hdr22_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_min_factor
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents min to mid factor to force vertical edge direction
    /// Type:          uint
    /// Default Value: 12
    /// Range:         [0, 31]
    /// Bit Depth:     5u
    /// Conversion:    No Conversion needed
    /// Notes:         range: 0-16 is meaningful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_min_factor;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_flat_region_th
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the threshold for the flat region in edge detection
    /// Type:          uint
    /// Default Value: 64
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_flat_region_th;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_h_edge_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents threshold 1 for the horizontal edge switching function
    /// Type:          uint
    /// Default Value: 16
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_h_edge_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_h_edge_dth_log2
    ///
    /// Tuning Level:  MustTune
    /// Description:   log2(th2-th1) for horizontal edge switching function.
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [4,7]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_h_edge_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_motion_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents threshold 1 for the motion switching function.
    /// Type:          uint
    /// Default Value: 184
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_motion_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_motion_dth_log2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Brief description: log2(th2-th1) for motion switching function.
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [4,7]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_motion_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_dark_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents threshold 1 for dark noise switching function.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_dark_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_dark_dth_log2
    ///
    /// Tuning Level:  MustTune
    /// Description:   Brief description: log2(th2-th1) for dark noise switching function.
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [0,4]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT recon_dark_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_zrec_prefilt_tap0
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the strength of the Zigzag HDR prefilter.
    /// Type:          uint
    /// Default Value: 16
    /// Range:         [0,63]
    /// Bit Depth:     6u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT hdr_zrec_prefilt_tap0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_zrec_g_grad_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents threshold 1 for the green pixel gradient for directional interpolation.
    /// Type:          uint
    /// Default Value: 32
    /// Range:         [0,4095]
    /// Bit Depth:     12u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT hdr_zrec_g_grad_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_zrec_rb_grad_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents threshold 1 for the red/blue pixel gradient for directional interpolation.
    /// Type:          uint
    /// Default Value: 32
    /// Range:         [0,4095]
    /// Bit Depth:     12u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT hdr_zrec_rb_grad_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_motion0_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   Noise model is: noise = th2*sqrt(gain*intensity)+th1.
    /// Type:          uint
    /// Default Value: 60
    /// Range:         [0,1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_motion0_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_motion0_th2
    ///
    /// Tuning Level:  MustTune
    /// Description:   Noise model is: noise = th2*sqrt(gain*intensity)+th1. This value represents the noise-luma slope for
    ///                motion detection.
    /// Type:          uint
    /// Default Value: 24
    /// Range:         [0,255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_motion0_th2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_motion0_strength
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the motion adaptation strength value.
    /// Type:          uint
    /// Default Value: 8
    /// Range:         [0,16]
    /// Bit Depth:     5u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_motion0_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_low_light_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the threshold 1 value for low light switching.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0,16383]
    /// Bit Depth:     14u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_low_light_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_low_light_strength
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the log light switching strength value.
    /// Type:          uint
    /// Default Value: 8
    /// Range:         [0,16]
    /// Bit Depth:     5u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_low_light_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_high_light_th1
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the threshold 1 value for high light switching.
    /// Type:          uint
    /// Default Value: 232
    /// Range:         [0,16383]
    /// Bit Depth:     14u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_high_light_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_high_light_dth_log2
    ///
    /// Tuning Level:  MustTune
    /// Description:   This value represents the log2(th2 ?th1) value for high light switching.
    /// Type:          uint
    /// Default Value: 10
    /// Range:         [2, 14]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT mac_high_light_dth_log2;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_hdr22_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_hdr22_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_recon_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Enable RECON block of HDR
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 hdr_recon_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_mac_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Enable MAC block of HDR
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 hdr_mac_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_msb_align
    ///
    /// Tuning Level:  NeverTune
    /// Description:   MSB Align for HDR module
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 hdr_msb_align;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_zrec_g_grad_dth_log2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   This value represents threshold 2 for the green pixel gradient delta for directional interpolation.
    /// Type:          uint
    /// Default Value: 5
    /// Range:         [0,12]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 hdr_zrec_g_grad_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hdr_zrec_rb_grad_dth_log2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   This value represents threshold 2 for the red/blue pixel gradient delta for directional interpolation.
    /// Type:          uint
    /// Default Value: 5
    /// Range:         [0,12]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 hdr_zrec_rb_grad_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// recon_edge_lpf_tap0
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Tap0 value for low-pass filter applied before edge detection.
    /// Type:          uint
    /// Default Value: 3
    /// Range:         [0,5]
    /// Bit Depth:     3u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 recon_edge_lpf_tap0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_motion_dilation
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Size of motion dilation max filter, 5 means -5 to 5, i.e. 11-tap filter.
    /// Type:          uint
    /// Default Value: 5
    /// Range:         [0,5]
    /// Bit Depth:     3u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_motion_dilation;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_motion0_dt0
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   An additive term to noise estimate, to avoid dividing by zero.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [1,63]
    /// Bit Depth:     6u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_motion0_dt0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_low_light_dth_log2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   log2(th2-th1) for low light switching.
    /// Type:          uint
    /// Default Value: 6
    /// Range:         [2,14]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_low_light_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_smooth_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Enable motion adaptive smoothing, an optional step in MAC.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_smooth_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_smooth_th1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Th1 for motion adaptive smoothing.
    /// Type:          uint
    /// Default Value: 192
    /// Range:         [0,256]
    /// Bit Depth:     9u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_smooth_th1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_smooth_dth_log2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   log2(th2-th1) for motion adaptive smoothing.
    /// Type:          uint
    /// Default Value: 6
    /// Range:         [2,8]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_smooth_dth_log2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mac_smooth_tap0
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Tap0 value for the low-pass filter in motion adaptive smoothing.
    /// Type:          uint
    /// Default Value: 5
    /// Range:         [0, 5]
    /// Bit Depth:     3u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 mac_smooth_tap0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// hdr22_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct hdr22_enable_sectionStruct
{
    UINT32 reserved;
    INT32  hdr_enable;
};

struct hnr10_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lnr_gain_arr_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   luma based noise reduction gain table
    /// Type:          uint
    /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
    /// Range:         [0, 63]
    /// Bit Depth:     6u, q5
    /// Length:        33
    /// Notes:         HNR will be stronger as lnr_gain_arr decreases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct lnr_gain_arr_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// lnr_gain_arr - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
        /// Range:         [0, 63]
        /// Length:        33
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT lnr_gain_arr[33];
    } lnr_gain_arr_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lnr_shift
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   LNR right shift used to adjust LNR LUT input value
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 3]
    /// Bit Depth:     2
    /// Notes:         HNR will be stronger as lnr_shift increases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     lnr_shift;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_noise_prsv_adj_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   radial adjustment for noise preserving percentage
    /// Type:          float
    /// Default Value: 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
    /// Range:         [0, 1.0]
    /// Bit Depth:     10uQ9
    /// Length:        7
    /// Notes:         HNR will be stronger denoising as radial_noise_prsv_adj decreases. In general, radial_noise_prsv_adj is a
    ///                monotonically decreasing curve.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_noise_prsv_adj_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_noise_prsv_adj - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
        /// Range:         [0, 1.0]
        /// Length:        7
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_noise_prsv_adj[7];
    } radial_noise_prsv_adj_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cnr_gain_arr_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   chroma based noise reduction(distance percentage) for peak noise reduction
    /// Type:          uint
    /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
    /// Range:         [0,63]
    /// Bit Depth:     6u, q5
    /// Length:        25
    /// Notes:         HNR will be stronger as cnr_gain_arr decreases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct cnr_gain_arr_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// cnr_gain_arr - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
        /// Range:         [0,63]
        /// Length:        25
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT cnr_gain_arr[25];
    } cnr_gain_arr_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cnr_low_thrd_u
    ///
    /// Tuning Level:  OftenTune
    /// Description:   CNR low-bound threshold for U channel
    /// Type:          int
    /// Default Value: 2
    /// Range:         [-128, 127]
    /// Bit Depth:     8s
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     cnr_low_thrd_u;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cnr_low_thrd_v
    ///
    /// Tuning Level:  OftenTune
    /// Description:   CNR low-bound threshold for V channel
    /// Type:          int
    /// Default Value: 2
    /// Range:         [-128, 127]
    /// Bit Depth:     8s
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     cnr_low_thrd_v;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cnr_low_gap_u
    ///
    /// Tuning Level:  OftenTune
    /// Description:   the difference between the upper and the lower threshold values in U channel are (2^cnr_thrd_gap_u)
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 7]
    /// Bit Depth:     3u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     cnr_low_gap_u;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cnr_low_gap_v
    ///
    /// Tuning Level:  OftenTune
    /// Description:   the difference between the upper and the lower threshold values in V channel are (2^cnr_thrd_gap_v)
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 7]
    /// Bit Depth:     3u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     cnr_low_gap_v;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cnr_adj_gain
    ///
    /// Tuning Level:  OftenTune
    /// Description:   CNR adjust gain
    /// Type:          uint
    /// Default Value: 32
    /// Range:         [0, 63]
    /// Bit Depth:     6u, q5
    /// Notes:         HNR will be stronger as cnr_adj_gain decreases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     cnr_adj_gain;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cnr_scale
    ///
    /// Tuning Level:  OftenTune
    /// Description:   scale of CNR input before the table look-up.
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [0, 15]
    /// Bit Depth:     4u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     cnr_scale;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fnr_gain_arr_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   gain table for flatness based noise reduction
    /// Type:          uint
    /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
    /// Range:         [0, 63]
    /// Bit Depth:     6u, q5
    /// Length:        17
    /// Notes:         HNR will be stronger as fnr_gain_arr decreases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fnr_gain_arr_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fnr_gain_arr - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
        /// Range:         [0, 63]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fnr_gain_arr[17];
    } fnr_gain_arr_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fnr_gain_clamp_arr_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   filtering gain clamp table for noise reduction
    /// Type:          uint
    /// Default Value: 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128
    /// Range:         [0, 255]
    /// Bit Depth:     8u, q7
    /// Length:        17
    /// Notes:         HNR will be stronger as fnr_gain_clamp_arr decreases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fnr_gain_clamp_arr_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fnr_gain_clamp_arr - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128
        /// Range:         [0, 255]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fnr_gain_clamp_arr[17];
    } fnr_gain_clamp_arr_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fnr_ac_th_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   FNR activity (variance )normalization by level
    /// Type:          uint
    /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
    /// Range:         [0, 63]
    /// Bit Depth:     6u, q5
    /// Length:        17
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fnr_ac_th_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fnr_ac_th - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
        /// Range:         [0, 63]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fnr_ac_th[17];
    } fnr_ac_th_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fnr_ac_shift
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   FNR activity (variance ) shift before looking up fnr_gain_arr
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 3]
    /// Bit Depth:     2u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     fnr_ac_shift;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// filtering_nr_gain_arr_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   final noise reduction gain table
    /// Type:          uint
    /// Default Value: 16, 16, 16, 16, 16, 32, 48, 64, 80, 96, 112, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128
    /// Range:         [0, 255]
    /// Bit Depth:     8u, q7
    /// Length:        33
    /// Notes:         HNR will be stronger as filtering_nr_gain_arr decreases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct filtering_nr_gain_arr_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// filtering_nr_gain_arr - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 16, 16, 16, 16, 16, 32, 48, 64, 80, 96, 112, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128
        /// Range:         [0, 255]
        /// Length:        33
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT filtering_nr_gain_arr[33];
    } filtering_nr_gain_arr_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// abs_amp_shift
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   right shift bit applied coefficient amplitude before looking up filtering_nr_gain_arr
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 3]
    /// Bit Depth:     2u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     abs_amp_shift;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lpf3_percent
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   low-pass filter threshold percentage
    /// Type:          uint
    /// Default Value: 16
    /// Range:         [0, 255]
    /// Bit Depth:     8u, q8
    /// Notes:         low-pass filer will be stronger as lpf3_percent increases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     lpf3_percent;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lpf3_offset
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   low-pass filter threshold offset
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Notes:         low-pass filer will be stronger as lpf3_offset increases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     lpf3_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lpf3_strength
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   low-pass filter gradient smoothing strength
    /// Type:          uint
    /// Default Value: 2
    /// Range:         [0, 5]
    /// Bit Depth:     3u
    /// Notes:         low-pass filer gradient smoothing will be stronger as lpf3_strength increases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     lpf3_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blend_lnr_gain_arr_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   luma based noise reduction gain table
    /// Type:          uint
    /// Default Value: 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48
    /// Range:         [0, 255]
    /// Bit Depth:     8u, q7
    /// Length:        17
    /// Notes:         more noise/details will be added back as blending_lnr_gain_arr increases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct blend_lnr_gain_arr_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// blend_lnr_gain_arr - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48
        /// Range:         [0, 255]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT blend_lnr_gain_arr[17];
    } blend_lnr_gain_arr_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blend_cnr_adj_gain
    ///
    /// Tuning Level:  OftenTune
    /// Description:   CNR adjust gain for blending
    /// Type:          uint
    /// Default Value: 32
    /// Range:         [0, 63]
    /// Bit Depth:     6u, q5
    /// Notes:         more noise/details will be added back as blend_cnr_adj_gain increases.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     blend_cnr_adj_gain;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blend_snr_gain_arr_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   skin based blending gain table
    /// Type:          uint
    /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
    /// Range:         [1, 32]
    /// Bit Depth:     5u, q5, SW tuning parameter need subtract by 1 and set it to HW register.
    /// Length:        17
    /// Notes:         more noise/details will be added back as blending_snr_gain_arr increases. SW tuning parameter need
    ///                subtract by 1 and set it to HW register.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct blend_snr_gain_arr_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// blend_snr_gain_arr - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
        /// Range:         [1, 32]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT blend_snr_gain_arr[17];
    } blend_snr_gain_arr_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_hue_min
    ///
    /// Tuning Level:  MustTune
    /// Description:   Min hue value for skin detection. Instead of negative (Cb/Cr), absolute value |Cb/Cr| is used in HW
    /// Type:          float
    /// Default Value: -1.39
    /// Range:         [-3.0, -1.0]
    /// Bit Depth:     10u, q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_hue_min;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_hue_max
    ///
    /// Tuning Level:  MustTune
    /// Description:   Max hue value for skin detection. Instead of negative (Cb/Cr), absolute value |Cb/Cr| is used in HW
    /// Type:          float
    /// Default Value: -0.4
    /// Range:         [-0.996, 0]
    /// Bit Depth:     8u, q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_hue_max;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_y_min
    ///
    /// Tuning Level:  MustTune
    /// Description:   Min Y value for skin detection.
    /// Type:          float
    /// Default Value: 0.125
    /// Range:         [0, 1.0]
    /// Bit Depth:     8u, q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_y_min;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_y_max
    ///
    /// Tuning Level:  MustTune
    /// Description:   Max Y value for skin detection
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0, 1.0]
    /// Bit Depth:     8u, q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_y_max;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_saturation_min_y_max
    ///
    /// Tuning Level:  MustTune
    /// Description:   Min saturation value for skin when Y=Y_max
    /// Type:          float
    /// Default Value: 0.08
    /// Range:         [0, 0.4]
    /// Bit Depth:     8u, q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_saturation_min_y_max;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_saturation_max_y_max
    ///
    /// Tuning Level:  MustTune
    /// Description:   Max saturation value for skin when Y=Y_max
    /// Type:          float
    /// Default Value: 0.8
    /// Range:         [0.1, 0.5]
    /// Bit Depth:     8u, q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_saturation_max_y_max;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_saturation_min_y_min
    ///
    /// Tuning Level:  MustTune
    /// Description:   Min saturation value for skin when Y=Y_min
    /// Type:          float
    /// Default Value: 0.32
    /// Range:         [0, 0.5]
    /// Bit Depth:     8u, q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_saturation_min_y_min;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_saturation_max_y_min
    ///
    /// Tuning Level:  MustTune
    /// Description:   Max saturation value for skin when Y=Y_min
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.2, 1.0]
    /// Bit Depth:     8u, q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_saturation_max_y_min;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_boundary_probability
    ///
    /// Tuning Level:  MustTune
    /// Description:   skin-tone probability at the boundary of skin-tone range
    /// Type:          uint
    /// Default Value: 8
    /// Range:         [1, 15]
    /// Bit Depth:     4u, q4
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_boundary_probability;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_percent
    ///
    /// Tuning Level:  MustTune
    /// Description:   Percentage of skin tone region with skin-tone probability 1
    /// Type:          float
    /// Default Value: 12
    /// Range:         [0,100]
    /// Bit Depth:     n/a
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_percent;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// skin_non_skin_to_skin_q_ratio
    ///
    /// Tuning Level:  MustTune
    /// Description:   Ratio of Q_nonSkin to Q_skin
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0.1, 10.0]
    /// Bit Depth:     n/a
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     skin_non_skin_to_skin_q_ratio;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// face_boundary
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   the lower bound of face region. Full skin detection is applied within radius smaller than face_boundary *
    ///                face_radius(from face detection).
    /// Type:          float
    /// Default Value: 1.2
    /// Range:         [0, 8]
    /// Bit Depth:     n/a
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     face_boundary;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// face_transition
    ///
    /// Tuning Level:  MustTune
    /// Description:   the upper bound of face region. no skin detection is applied within radius larger than face_transition *
    ///                face_radius(from face detection).
    /// Type:          float
    /// Default Value: 2
    /// Range:         [0, 8]
    /// Bit Depth:     n/a
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     face_transition;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// snr_gain_arr_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   skin based noise reduction gain table
    /// Type:          uint
    /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
    /// Range:         [1, 32]
    /// Bit Depth:     5u, q5, SW tuning parameter need subtract by 1 and set it to HW register.
    /// Length:        17
    /// Notes:         HNR will be stronger as snr_gain_arr decreases. SW tuning parameter need subtract by 1 and set it to HW
    ///                register.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct snr_gain_arr_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// snr_gain_arr - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
        /// Range:         [1, 32]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT snr_gain_arr[17];
    } snr_gain_arr_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// snr_skin_smoothing_str
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   skin map smoothing strength for SNR
    /// Type:          uint
    /// Default Value: 2
    /// Range:         [0, 2]
    /// Bit Depth:     2u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                     snr_skin_smoothing_str;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_hnr10_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_hnr10_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lnr_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of luma based noise reduction
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  lnr_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// rnr_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of radius based noise reduction
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  rnr_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cnr_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of chroma based noise reduction
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  cnr_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// snr_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of skin color based noise reduction
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  snr_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fd_snr_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of face guided skin color based noise reduction
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         if fd_snr_en = 1, snr is applied on face regions; if fd_snr_en = 0, snr is applied on both face regions
    ///                and non-face regions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  fd_snr_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fnr_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of flatness based noise reduction
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  fnr_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lpf3_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of low-pass filter
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  lpf3_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blend_cnr_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of color based noise blending
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  blend_cnr_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blend_snr_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable bit of skin color based blending
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  blend_snr_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// radial_anchor_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   radial anchor table (distance percentage)
    /// Type:          float
    /// Default Value: 0.0, 0.2, 0.3, 0.4, 0.6, 0.8, 1.0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     10u
    /// Length:        7
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct radial_anchor_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// radial_anchor - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0.0, 0.2, 0.3, 0.4, 0.6, 0.8, 1.0
        /// Range:         [0.0, 1.0]
        /// Length:        7
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT radial_anchor[7];
    } radial_anchor_tab;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// hnr10_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct hnr10_enable_sectionStruct
{
    UINT32 reserved;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hnr_nr_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   HNR noise reduction enable bit
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  hnr_nr_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hnr_blend_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   HNR blending enable bit
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32  hnr_blend_enable;
};

struct ica10_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// y_interpolation_type
    ///
    /// Tuning Level:  OftenTune
    /// Description:   0: 4x4 kernel. 1: Bi-linear (2x2).
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                                              y_interpolation_type;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ctc_grid_x_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Grid value for x.
    /// Default Value: 0
    /// Range:         [-65536, 65535]
    /// Bit Depth:     17s, Q3
    /// Length:        829
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct ctc_grid_x_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ctc_grid_x - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0
        /// Range:         [-65536, 65535]
        /// Length:        829
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT ctc_grid_x[829];
    } ctc_grid_x_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ctc_grid_y_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Grid value for y.
    /// Default Value: 0
    /// Range:         [-65536, 65535]
    /// Bit Depth:     17s, Q3
    /// Length:        829
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct ctc_grid_y_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ctc_grid_y - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0
        /// Range:         [-65536, 65535]
        /// Length:        829
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT ctc_grid_y[829];
    } ctc_grid_y_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// opg_interpolation_lut_0_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   LUT(0).
    /// Default Value: 0,-60,-113,-158,-196,-228,-254,-273,-288,-298,-303,-303,-300,-293,-284,-271
    /// Range:         [-8192, 8191]
    /// Bit Depth:     14s
    /// Length:        16
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct opg_interpolation_lut_0_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// opg_interpolation_lut_0 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,-60,-113,-158,-196,-228,-254,-273,-288,-298,-303,-303,-300,-293,-284,-271
        /// Range:         [-8192, 8191]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT opg_interpolation_lut_0[16];
    } opg_interpolation_lut_0_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// opg_interpolation_lut_1_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   LUT(1).
    /// Default Value: 2304,4086,4058,4011,3948,3869,3777,3670,3552,3423,3284,3136,2980,2818,2651,2479
    /// Range:         [-8192, 8191]
    /// Bit Depth:     14s
    /// Length:        16
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct opg_interpolation_lut_1_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// opg_interpolation_lut_1 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 2304,4086,4058,4011,3948,3869,3777,3670,3552,3423,3284,3136,2980,2818,2651,2479
        /// Range:         [-8192, 8191]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT opg_interpolation_lut_1[16];
    } opg_interpolation_lut_1_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// opg_interpolation_lut_2_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   LUT(2) .
    /// Default Value: -256,72,159,259,372,497,632,776,928,1087,1253,1422,1596,1772,1950,2127
    /// Range:         [-8192, 8191]
    /// Bit Depth:     14s
    /// Length:        16
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct opg_interpolation_lut_2_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// opg_interpolation_lut_2 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: -256,72,159,259,372,497,632,776,928,1087,1253,1422,1596,1772,1950,2127
        /// Range:         [-8192, 8191]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT opg_interpolation_lut_2[16];
    } opg_interpolation_lut_2_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// distorted_input_to_undistorted_ldc_grid_x_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Grid value for x.
    /// Default Value: 0
    /// Range:         [-65536, 65535]
    /// Bit Depth:     17s, Q3
    /// Length:        829
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct distorted_input_to_undistorted_ldc_grid_x_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// distorted_input_to_undistorted_ldc_grid_x - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0
        /// Range:         [-65536, 65535]
        /// Length:        829
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT distorted_input_to_undistorted_ldc_grid_x[829];
    } distorted_input_to_undistorted_ldc_grid_x_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// distorted_input_to_undistorted_ldc_grid_y_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Grid value for y.
    /// Default Value: 0
    /// Range:         [-65536, 65535]
    /// Bit Depth:     17s, Q3
    /// Length:        829
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct distorted_input_to_undistorted_ldc_grid_y_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// distorted_input_to_undistorted_ldc_grid_y - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0
        /// Range:         [-65536, 65535]
        /// Length:        829
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT distorted_input_to_undistorted_ldc_grid_y[829];
    } distorted_input_to_undistorted_ldc_grid_y_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// undistorted_to_lens_distorted_output_ld_grid_x_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Grid value for x.
    /// Default Value: 0
    /// Range:         [-65536, 65535]
    /// Bit Depth:     17s, Q3
    /// Length:        829
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct undistorted_to_lens_distorted_output_ld_grid_x_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// undistorted_to_lens_distorted_output_ld_grid_x - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0
        /// Range:         [-65536, 65535]
        /// Length:        829
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT undistorted_to_lens_distorted_output_ld_grid_x[829];
    } undistorted_to_lens_distorted_output_ld_grid_x_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// undistorted_to_lens_distorted_output_ld_grid_y_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Grid value for y.
    /// Default Value: 0
    /// Range:         [-65536, 65535]
    /// Bit Depth:     17s, Q3
    /// Length:        829
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct undistorted_to_lens_distorted_output_ld_grid_y_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// undistorted_to_lens_distorted_output_ld_grid_y - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0
        /// Range:         [-65536, 65535]
        /// Length:        829
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT undistorted_to_lens_distorted_output_ld_grid_y[829];
    } undistorted_to_lens_distorted_output_ld_grid_y_tab;
};

struct linearization33_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// r_lut_p_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel knee points
    /// Type:          uint
    /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
    /// Range:         [0, 16383]
    /// Bit Depth:     14uQ0
    /// Length:        8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct r_lut_p_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// r_lut_p - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
        /// Range:         [0, 16383]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT r_lut_p[8];
    } r_lut_p_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// r_lut_base_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel base output value
    /// Type:          uint
    /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
    /// Range:         [0, 16383]
    /// Bit Depth:     14uQ0
    /// Length:        9
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct r_lut_base_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// r_lut_base - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
        /// Range:         [0, 16383]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT r_lut_base[9];
    } r_lut_base_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gr_lut_p_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel knee points
    /// Type:          uint
    /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
    /// Range:         [0, 16383]
    /// Bit Depth:     14uQ0
    /// Length:        8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gr_lut_p_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gr_lut_p - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
        /// Range:         [0, 16383]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gr_lut_p[8];
    } gr_lut_p_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gr_lut_base_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel base output value
    /// Type:          uint
    /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
    /// Range:         [0, 16383]
    /// Bit Depth:     14uQ0
    /// Length:        9
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gr_lut_base_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gr_lut_base - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
        /// Range:         [0, 16383]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gr_lut_base[9];
    } gr_lut_base_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gb_lut_p_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel knee points
    /// Type:          uint
    /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
    /// Range:         [0, 16383]
    /// Bit Depth:     14uQ0
    /// Length:        8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gb_lut_p_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gb_lut_p - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
        /// Range:         [0, 16383]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gb_lut_p[8];
    } gb_lut_p_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gb_lut_base_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel base output value
    /// Type:          uint
    /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
    /// Range:         [0, 16383]
    /// Bit Depth:     14uQ0
    /// Length:        9
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gb_lut_base_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gb_lut_base - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
        /// Range:         [0, 16383]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gb_lut_base[9];
    } gb_lut_base_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// b_lut_p_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel knee points
    /// Type:          uint
    /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
    /// Range:         [0, 16383]
    /// Bit Depth:     14uQ0
    /// Length:        8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct b_lut_p_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// b_lut_p - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
        /// Range:         [0, 16383]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT b_lut_p[8];
    } b_lut_p_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// b_lut_base_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel base output value
    /// Type:          uint
    /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
    /// Range:         [0, 16383]
    /// Bit Depth:     14uQ0
    /// Length:        9
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct b_lut_base_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// b_lut_base - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
        /// Range:         [0, 16383]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT b_lut_base[9];
    } b_lut_base_tab;
};

struct linearization34_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// r_lut_p_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel knee points
    /// Type:          uint
    /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
    /// Range:         [0,16383]
    /// Bit Depth:     (8+e)uQ0
    /// Length:        8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct r_lut_p_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// r_lut_p - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
        /// Range:         [0,16383]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT r_lut_p[8];
    } r_lut_p_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// r_lut_base_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel base output value
    /// Type:          uint
    /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
    /// Range:         [0,16383]
    /// Bit Depth:     (8+e)uQ0
    /// Length:        9
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct r_lut_base_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// r_lut_base - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
        /// Range:         [0,16383]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT r_lut_base[9];
    } r_lut_base_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gr_lut_p_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel knee points
    /// Type:          uint
    /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
    /// Range:         [0,16383]
    /// Bit Depth:     (8+e)uQ0
    /// Length:        8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gr_lut_p_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gr_lut_p - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
        /// Range:         [0,16383]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gr_lut_p[8];
    } gr_lut_p_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gr_lut_base_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel base output value
    /// Type:          uint
    /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
    /// Range:         [0,16383]
    /// Bit Depth:     (8+e)uQ0
    /// Length:        9
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gr_lut_base_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gr_lut_base - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
        /// Range:         [0,16383]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gr_lut_base[9];
    } gr_lut_base_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gb_lut_p_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel knee points
    /// Type:          uint
    /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
    /// Range:         [0,16383]
    /// Bit Depth:     (8+e)uQ0
    /// Length:        8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gb_lut_p_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gb_lut_p - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
        /// Range:         [0,16383]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gb_lut_p[8];
    } gb_lut_p_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gb_lut_base_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel base output value
    /// Type:          uint
    /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
    /// Range:         [0,16383]
    /// Bit Depth:     (8+e)uQ0
    /// Length:        9
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gb_lut_base_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gb_lut_base - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
        /// Range:         [0,16383]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gb_lut_base[9];
    } gb_lut_base_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// b_lut_p_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel knee points
    /// Type:          uint
    /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
    /// Range:         [0,16383]
    /// Bit Depth:     (8+e)uQ0
    /// Length:        8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct b_lut_p_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// b_lut_p - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 256, 768, 1280, 1792, 2304, 2816, 3328, 3840
        /// Range:         [0,16383]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT b_lut_p[8];
    } b_lut_p_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// b_lut_base_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel base output value
    /// Type:          uint
    /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
    /// Range:         [0, 16383]
    /// Bit Depth:     (8+e)uQ0
    /// Length:        9
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct b_lut_base_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// b_lut_base - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0, 0, 512, 1024, 1536, 2048, 2560, 3072, 3584
        /// Range:         [0, 16383]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT b_lut_base[9];
    } b_lut_base_tab;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// linearization34_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct linearization34_enable_sectionStruct
{
    UINT32 reserved;
    INT32  linearization_enable;
};

struct lrme10_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// max_allowed_sad
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Maximum allowed SAD. If SAD exceeds this threshold the block will be marked invalid (probably ME problem
    ///                due to occlusion, too fast motion, etc).
    /// Type:          uint
    /// Default Value: 1200
    /// Range:         [0, 32767]
    /// Bit Depth:     15u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                           max_allowed_sad;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// min_allowed_tar_mad
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Minimum allowed tarMAD value. If below this value (non-informative block) – the block will be marked
    ///                invalid.
    /// Type:          uint
    /// Default Value: 96
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                           min_allowed_tar_mad;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// meaningful_sad_diff
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Locations with regular SAD difference below this threshold are considered to have exactly the same
    ///                similarity measure (even if normalized SAD difference is big).
    /// Type:          uint
    /// Default Value: 5
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                           meaningful_sad_diff;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// min_sad_diff_denom
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Determines the lower limit of denominator used in normalized difference of SAD calculation to avoid
    ///                overflow.
    /// Type:          uint
    /// Default Value: 384
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                           min_sad_diff_denom;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// robustness_measure_dist_map_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Provides a requirement on minimal normalized difference of SADs at distance 1-9 pixels (for big distances
    ///                the expected differences should be big). Default values: 0, 128, 128, 128, 128, 128, 128, 128 (no penalty
    ///                to single pixel distance; afterwards same penalty for all other distances) .
    /// Default Value: 0, 128, 128, 128, 128, 128, 128, 128
    /// Range:         [0, 511]
    /// Bit Depth:     9u
    /// Length:        8
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct robustness_measure_dist_map_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// robustness_measure_dist_map - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0, 128, 128, 128, 128, 128, 128, 128
        /// Range:         [0, 511]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT robustness_measure_dist_map[8];
    } robustness_measure_dist_map_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// enable_transform_confidence
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Whether to calculate and apply transform confidence (based on coverage of the frame by LRME-calculated
    ///                valid MVs). Frames with higher coverage get higher confidence.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                           enable_transform_confidence;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transform_confidence_mapping_base
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Defines mapping function from calculated transform confidence to actually used transform confidence. The
    ///                calculated confidence is in the range 0:256 (8 bit fraction). The mapping is: actual confidence =
    ///                transform_confidence_mapping_base when calculated confidence is less or equal than
    ///                transform_confidence_mapping_c1; 256 when calculated confidence is greater than
    ///                transform_confidence_mapping_c2 and linear interpolation in-between.
    /// Type:          uint
    /// Default Value: 128
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                           transform_confidence_mapping_base;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transform_confidence_mapping_c1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Defines mapping function from calculated transform confidence to actually used transform confidence. The
    ///                calculated confidence is in the range 0:256 (8 bit fraction). The mapping is: actual confidence =
    ///                transform_confidence_mapping_base when calculated confidence is less or equal than
    ///                transform_confidence_mapping_c1; 256 when calculated confidence is greater than
    ///                transform_confidence_mapping_c2 and linear interpolation in-between.
    /// Type:          uint
    /// Default Value: 100
    /// Range:         [0, 255]
    /// Bit Depth:     8u, Q7
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                           transform_confidence_mapping_c1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transform_confidence_mapping_c2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Defines mapping function from calculated transform confidence to actually used transform confidence. The
    ///                calculated confidence is in the range 0:256 (8 bit fraction). The mapping is: actual confidence =
    ///                transform_confidence_mapping_base when calculated confidence is less or equal than
    ///                transform_confidence_mapping_c1; 256 when calculated confidence is greater than
    ///                transform_confidence_mapping_c2 and linear interpolation in-between.
    /// Type:          uint
    /// Default Value: 240
    /// Range:         [0, 255]
    /// Bit Depth:     8u, Q7
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                           transform_confidence_mapping_c2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transform_confidence_thr_to_force_identity_transform
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   When calculated transform confidence (before mapping) is below this threshold, the transform is ignored
    ///                and replaced by identity transform. Hysteresis is used on this parameter to avoid excessive switching.
    /// Type:          uint
    /// Default Value: 100
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                           transform_confidence_thr_to_force_identity_transform;
};

struct lsc34_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// r_gain_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Calibrated roll off gain for r/gr/gb/b channels
    /// Type:          float
    /// Default Value: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    /// Range:         [1.0, 7.999]
    /// Bit Depth:     n/a
    /// Length:        221
    /// Notes:         larger value means stronger rolloff boost
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct r_gain_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// r_gain - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        /// Range:         [1.0, 7.999]
        /// Length:        221
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT r_gain[221];
    } r_gain_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gr_gain_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Calibrated roll off gain for r/gr/gb/b channels
    /// Type:          float
    /// Default Value: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    /// Range:         [1.0, 7.999]
    /// Bit Depth:     n/a
    /// Length:        221
    /// Notes:         larger value means stronger rolloff boost
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gr_gain_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gr_gain - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        /// Range:         [1.0, 7.999]
        /// Length:        221
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gr_gain[221];
    } gr_gain_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gb_gain_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Calibrated roll off gain for r/gr/gb/b channels
    /// Type:          float
    /// Default Value: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    /// Range:         [1.0, 7.999]
    /// Bit Depth:     n/a
    /// Length:        221
    /// Notes:         larger value means stronger rolloff boost
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct gb_gain_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// gb_gain - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        /// Range:         [1.0, 7.999]
        /// Length:        221
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT gb_gain[221];
    } gb_gain_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// b_gain_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Calibrated roll off gain for r/gr/gb/b channels
    /// Type:          float
    /// Default Value: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    /// Range:         [1.0, 7.999]
    /// Bit Depth:     n/a
    /// Length:        221
    /// Notes:         larger value means stronger rolloff boost
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct b_gain_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// b_gain - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        /// Range:         [1.0, 7.999]
        /// Length:        221
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT b_gain[221];
    } b_gain_tab;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// lsc34_enable_section
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct lsc34_enable_sectionStruct
{
    UINT32 reserved;
    INT32  rolloff_enable;
};

struct ltm13_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// la_curve_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   luma adaptation curve
    /// Type:          int
    /// Default Value: 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048, 2112, 2176, 2240, 2304, 2368, 2432, 2496, 2560, 2624, 2688, 2752, 2816, 2880, 2944, 3008, 3072, 3136, 3200, 3264, 3328, 3392, 3456, 3520, 3584, 3648, 3712, 3776, 3840, 3904, 3968, 4032, 4096
    /// Range:         [0, 4095]
    /// Bit Depth:      12u
    /// Length:        65
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct la_curve_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// la_curve - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048, 2112, 2176, 2240, 2304, 2368, 2432, 2496, 2560, 2624, 2688, 2752, 2816, 2880, 2944, 3008, 3072, 3136, 3200, 3264, 3328, 3392, 3456, 3520, 3584, 3648, 3712, 3776, 3840, 3904, 3968, 4032, 4096
        /// Range:         [0, 4095]
        /// Length:        65
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT la_curve[65];
    } la_curve_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ltm_scale_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   local tone map scale curve.
    /// Type:          int
    /// Default Value: 2343, 2022, 1605, 1198, 853, 617, 544, 473, 413, 361, 314, 270, 230, 192, 158, 125, 95, 71, 52, 37, 25, 17, 11, 6, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    /// Range:         [-2048, 2047]
    /// Bit Depth:      12s
    /// Length:        65
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct ltm_scale_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ltm_scale - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 2343, 2022, 1605, 1198, 853, 617, 544, 473, 413, 361, 314, 270, 230, 192, 158, 125, 95, 71, 52, 37, 25, 17, 11, 6, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        /// Range:         [-2048, 2047]
        /// Length:        65
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT ltm_scale[65];
    } ltm_scale_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lce_scale_pos_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   local contrast enhancement curve for positive/negative portion.
    /// Type:          int
    /// Default Value: 800, 800, 800, 700, 650, 625, 600, 575, 550, 550, 550, 550, 550, 550, 500, 300, 200
    /// Range:         [0, 2047]
    /// Bit Depth:      12s
    /// Length:        17
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct lce_scale_pos_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// lce_scale_pos - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 800, 800, 800, 700, 650, 625, 600, 575, 550, 550, 550, 550, 550, 550, 500, 300, 200
        /// Range:         [0, 2047]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT lce_scale_pos[17];
    } lce_scale_pos_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lce_scale_neg_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   local contrast enhancement curve for positive/negative portion.
    /// Type:          int
    /// Default Value: 300, 400, 600, 700, 700, 700, 700, 700, 700, 700, 700, 725, 750, 775, 800, 825, 850
    /// Range:         [0, 2047]
    /// Bit Depth:      12s
    /// Length:        17
    /// Conversion:    Bitwidth conversion if pipeline bitwidth is not 14bit
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct lce_scale_neg_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// lce_scale_neg - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 300, 400, 600, 700, 700, 700, 700, 700, 700, 700, 700, 725, 750, 775, 800, 825, 850
        /// Range:         [0, 2047]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT lce_scale_neg[17];
    } lce_scale_neg_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// y_ratio_max
    ///
    /// Tuning Level:  OftenTune
    /// Description:   maximum y_ratio
    /// Type:          float
    /// Default Value: 6
    /// Range:         [1.0, 16.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             y_ratio_max;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ltm_strength
    ///
    /// Tuning Level:  OftenTune
    /// Description:   overall scaling factor that will applied to ltm_scale curve
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 4.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             ltm_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// exp_atten_start
    ///
    /// Tuning Level:  OftenTune
    /// Description:   exposure related attenuation start and end
    /// Type:          float
    /// Default Value: 5
    /// Range:         [0, 100]
    /// Bit Depth:     8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             exp_atten_start;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// exp_atten_end
    ///
    /// Tuning Level:  OftenTune
    /// Description:   exposure related attenuation start and end
    /// Type:          float
    /// Default Value: 20
    /// Range:         [0, 100]
    /// Bit Depth:     float
    /// Conversion:    No conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             exp_atten_end;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dark_boost
    ///
    /// Tuning Level:  OftenTune
    /// Description:   scaling factor that will applied to negative portion of ltm_scale curve
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 4.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             dark_boost;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bright_suppress
    ///
    /// Tuning Level:  OftenTune
    /// Description:   scaling factor that will applied to positive portion of ltm_scale curve
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 4.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             bright_suppress;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lce_strength
    ///
    /// Tuning Level:  OftenTune
    /// Description:   scaling factor that will applied to lce_scale curve
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 4.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             lce_strength;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// p0
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   backup core parameters
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     Q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             p0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// p1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   backup core parameters
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     Q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             p1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dark_range
    ///
    /// Tuning Level:  OftenTune
    /// Description:   define the dark range of LTM scale curve
    /// Type:          float
    /// Default Value: 0.4
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             dark_range;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bright_range
    ///
    /// Tuning Level:  OftenTune
    /// Description:   define the bright range of LTM scale curve
    /// Type:          float
    /// Default Value: 0.25
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             bright_range;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dark_max
    ///
    /// Tuning Level:  OftenTune
    /// Description:   max value of dark part of LTM scale curve
    /// Type:          float
    /// Default Value: 4.25
    /// Range:         [0.0, 8.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             dark_max;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bright_max
    ///
    /// Tuning Level:  OftenTune
    /// Description:   max value of bright part of LTM scale curve
    /// Type:          float
    /// Default Value: 2
    /// Range:         [0.0, 4.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             bright_max;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dark_gamma
    ///
    /// Tuning Level:  OftenTune
    /// Description:   gamma value to generate dark boost curve for LTM
    /// Type:          float
    /// Default Value: 3.25
    /// Range:         [1.0, 5.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             dark_gamma;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bright_gamma
    ///
    /// Tuning Level:  OftenTune
    /// Description:   gamma value to generate bright suppression curve
    /// Type:          float
    /// Default Value: 3.25
    /// Range:         [1.0, 5.0]
    /// Bit Depth:     float
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT             bright_gamma;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ltm13_enable_section
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ltm13_enable_sectionStruct
{
    UINT32 reserved;
    INT32  ltm_enable;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_ltm13_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_ltm13_reserveType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ltm_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable ltm, independent of igamma
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                 ltm_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// data_collect_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable data collection
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                 data_collect_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// img_process_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable image processing
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                 img_process_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// igamma_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable reverse gamma, independent of ltm_en
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                 igamma_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// la_en
    ///
    /// Tuning Level:  OftenTune
    /// Description:   enable luma adaptation, valid under ltm_en
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                 la_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mask_filter_en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   apply filtering to mask, valid under ltm_en
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0,1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                 mask_filter_en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ltm_scale_manual_curve_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable manual LTM Scale curve; otherwise use automatically generated LTM Scale curve.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                 ltm_scale_manual_curve_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// la_manual_curve_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   enable manual La curve
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                 la_manual_curve_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   weights of R/G/B/MAX(R,G,B) contribute to Y
    /// Type:          float
    /// Default Value: 0.125
    /// Range:         [0.0, 1.0f]
    /// Bit Depth:     float
    /// Conversion:    Q6
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  c1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   weights of R/G/B/MAX(R,G,B) contribute to Y
    /// Type:          float
    /// Default Value: 0.25
    /// Range:         [0.0, 1.0f]
    /// Bit Depth:     float
    /// Conversion:    Q6
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  c2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c3
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   weights of R/G/B/MAX(R,G,B) contribute to Y
    /// Type:          float
    /// Default Value: 0.125
    /// Range:         [0.0, 1.0f]
    /// Bit Depth:     float
    /// Conversion:    Q6
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  c3;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c4
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   weights of R/G/B/MAX(R,G,B) contribute to Y
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0.0, 1.0f]
    /// Bit Depth:     float
    /// Conversion:    Q6
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  c4;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// wt_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   weight for stat filtering
    /// Type:          uint
    /// Default Value: 255, 179, 179, 125, 128, 90, 90, 63, 64, 45, 45, 31, 16, 11, 11, 8, 2, 2, 2, 1, 0, 0, 0, 0
    /// Range:         [0,255]
    /// Bit Depth:     8u
    /// Length:        24
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct wt_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// wt - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 255, 179, 179, 125, 128, 90, 90, 63, 64, 45, 45, 31, 16, 11, 11, 8, 2, 2, 2, 1, 0, 0, 0, 0
        /// Range:         [0,255]
        /// Length:        24
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 wt[24];
    } wt_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mask_filter_kernel_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   mask filtering kernel, when n>1, n means 1-shift-left-(n-1), when n=0, means 0
    /// Type:          uint
    /// Default Value: 5,3,3,2,2,1
    /// Range:         [[1,6],[0,5],[0,4],[0,3],[0,2],[0,1]]
    /// Bit Depth:     4u
    /// Length:        6
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct mask_filter_kernel_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// mask_filter_kernel - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 5,3,3,2,2,1
        /// Range:         [[1,6],[0,5],[0,4],[0,3],[0,2],[0,1]]
        /// Length:        6
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 mask_filter_kernel[6];
    } mask_filter_kernel_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mask_rect_curve_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   mask rectification curve
    /// Type:          int
    /// Default Value: 0, 0, 0, 105, 202, 289, 368, 441, 510, 576, 640, 704, 768, 832, 896, 960, 1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048, 2112, 2176, 2240, 2304, 2368, 2432, 2496, 2560, 2624, 2688, 2752, 2816, 2880, 2944, 3008, 3072, 3136, 3200, 3264, 3328, 3392, 3456, 3520, 3584, 3651, 3722, 3801, 3891, 3991, 4095, 4095, 4095
    /// Range:         [0,4095]
    /// Bit Depth:     12u
    /// Length:        65
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct mask_rect_curve_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// mask_rect_curve - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0, 0, 0, 105, 202, 289, 368, 441, 510, 576, 640, 704, 768, 832, 896, 960, 1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048, 2112, 2176, 2240, 2304, 2368, 2432, 2496, 2560, 2624, 2688, 2752, 2816, 2880, 2944, 3008, 3072, 3136, 3200, 3264, 3328, 3392, 3456, 3520, 3584, 3651, 3722, 3801, 3891, 3991, 4095, 4095, 4095
        /// Range:         [0,4095]
        /// Length:        65
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        INT32 mask_rect_curve[65];
    } mask_rect_curve_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ltm_curve_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   local tone map curve
    /// Type:          int
    /// Default Value: 0, 256, 512, 752, 992, 1208, 1416, 1592, 1760, 1904, 2016, 2112, 2192, 2256, 2288, 2312, 2328, 2328, 2312, 2288, 2264, 2232, 2192, 2152, 2104, 2056, 2008, 1960, 1904, 1856, 1800, 1752, 1696, 1648, 1592, 1544, 1488, 1440, 1384, 1336, 1280, 1232, 1176, 1128, 1072, 1016, 968, 912, 864, 808, 760, 704, 656, 600, 552, 496, 440, 392, 344, 288, 240, 184, 118, 50, 0
    /// Range:         [0, 4095]
    /// Bit Depth:      12u
    /// Length:        65
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct ltm_curve_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ltm_curve - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0, 256, 512, 752, 992, 1208, 1416, 1592, 1760, 1904, 2016, 2112, 2192, 2256, 2288, 2312, 2328, 2328, 2312, 2288, 2264, 2232, 2192, 2152, 2104, 2056, 2008, 1960, 1904, 1856, 1800, 1752, 1696, 1648, 1592, 1544, 1488, 1440, 1384, 1336, 1280, 1232, 1176, 1128, 1072, 1016, 968, 912, 864, 808, 760, 704, 656, 600, 552, 496, 440, 392, 344, 288, 240, 184, 118, 50, 0
        /// Range:         [0, 4095]
        /// Length:        65
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        INT32 ltm_curve[65];
    } ltm_curve_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lce_thd
    ///
    /// Tuning Level:  OftenTune
    /// Description:   threshold (shift-left-e) of local contrast enhancement
    /// Type:          uint
    /// Default Value: 512
    /// Range:         [0,1023]
    /// Bit Depth:     10uQ-e
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                 lce_thd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// r0
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   backup reserved parameters
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0, 1.0f]
    /// Bit Depth:     float
    /// Conversion:    Q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  r0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// r1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   backup reserved parameters
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0, 1.0f]
    /// Bit Depth:     float
    /// Conversion:    Q8
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  r1;
};

struct mf10_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// top
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct topStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// enable_mfsr
        ///
        /// Tuning Level:  MustTune
        /// Description:   Enable Multi-Frame Super-Resolution.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Nearest neighbor interpolation: discrete values allowed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT enable_mfsr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// num_of_frames
        ///
        /// Tuning Level:  MustTune
        /// Description:   Number of images to process.
        /// Type:          uint
        /// Default Value: 3
        /// Range:         [3, 31]
        /// Bit Depth:     5u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT num_of_frames;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// pre_filtering_strength_luma
        ///
        /// Tuning Level:  MustTune
        /// Description:   Strength (Q8) of luma pre-filtering. Strength less than 256 weakens spatial noise reduction for the anchor
        ///                frame. Strength 0 means no spatial noise reduction.
        /// Type:          uint
        /// Default Value: 256
        /// Range:         [0, 256]
        /// Bit Depth:     9u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT pre_filtering_strength_luma;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// pre_filtering_strength_chroma
        ///
        /// Tuning Level:  MustTune
        /// Description:   Strength (Q8) of chroma pre-filtering. Strength less than 256 weakens spatial noise reduction for the
        ///                anchor frame. Strength 0 means no spatial noise reduction.
        /// Type:          uint
        /// Default Value: 256
        /// Range:         [0, 256]
        /// Bit Depth:     9u
        /// Conversion:    No Conversion needed
        /// Notes:         Linear interpolation.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT pre_filtering_strength_chroma;
    } top;
};

struct pdpc11_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fmax_pixel_q6
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Bad pixel maximum threshold factor.
    /// Type:          float
    /// Default Value: 64
    /// Range:         [0, 255]
    /// Bit Depth:     8uQ6
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT fmax_pixel_q6;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fmin_pixel_q6
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Bad pixel minimum threshold factor.
    /// Type:          float
    /// Default Value: 64
    /// Range:         [0, 255]
    /// Bit Depth:     8uQ6
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT fmin_pixel_q6;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bp_offset_g_pixel
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset used in bad pixel detection for green pixels in regular bayer or T1 in HDR mode.
    /// Type:          uint
    /// Default Value: 32
    /// Range:         [0, 32767]
    /// Bit Depth:     15u
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT bp_offset_g_pixel;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bp_offset_rb_pixel
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset used in bad pixel detection for red or blue pixels in regular bayer or T1 in HDR mode.
    /// Type:          uint
    /// Default Value: 32
    /// Range:         [0, 32767]
    /// Bit Depth:     15u
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT bp_offset_rb_pixel;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// t2_bp_offset_g_pixel
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset used in bad green pixel detection for T2 in HDR mode.
    /// Type:          uint
    /// Default Value: 16384
    /// Range:         [0, 32767]
    /// Bit Depth:     15u
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT t2_bp_offset_g_pixel;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// t2_bp_offset_rb_pixel
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset used in bad red or blue pixel detection for T2 in HDR mode.
    /// Type:          uint
    /// Default Value: 16384
    /// Range:         [0, 32767]
    /// Bit Depth:     15u
    /// Conversion:    No Conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT t2_bp_offset_rb_pixel;
};

struct pdpc11_enable_section
{
    INT32 pdpc_enable;
    INT32 dsbpc_enable;
};

struct pdpc20_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fmax_pixel_q6
    ///
    /// Tuning Level:  OftenTune
    /// Description:   bad pixel maximum threshold factor
    /// Type:          float
    /// Default Value: 96
    /// Range:         [0,127]
    /// Bit Depth:     7uQ6
    /// Conversion:    no conversion needed
    /// Notes:         larger values mean less hot pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT fmax_pixel_q6;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fmin_pixel_q6
    ///
    /// Tuning Level:  OftenTune
    /// Description:   bad pixel minimum threshold factor
    /// Type:          float
    /// Default Value: 32
    /// Range:         [0,127]
    /// Bit Depth:     7uQ6
    /// Conversion:    no conversion needed
    /// Notes:         larger values mean more cold pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT fmin_pixel_q6;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// hot_pixel_correction_disable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   hot pixel correction enable/disable
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    no conversion needed
    /// Notes:         enable bit
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT hot_pixel_correction_disable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cold_pixel_correction_disable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   cold pixel correction enable/disable
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    no conversion needed
    /// Notes:         enable bit
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT cold_pixel_correction_disable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bpc_offset
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset used in single bad pixel detection for regular bayer or T1 in HDR mode
    /// Type:          uint
    /// Default Value: 512
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Conversion:    no conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT bpc_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bcc_offset
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset used in couplet bad pixel detection for regular bayer or T1 in HDR mode
    /// Type:          uint
    /// Default Value: 512
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Conversion:    no conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT bcc_offset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bpc_offset_t2
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset used in single bad pixel detection for T2 in HDR mode
    /// Type:          uint
    /// Default Value: 512
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Conversion:    no conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT bpc_offset_t2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// bcc_offset_t2
    ///
    /// Tuning Level:  OftenTune
    /// Description:   offset used in couplet bad pixel detection for T2 in HDR mode
    /// Type:          uint
    /// Default Value: 512
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Conversion:    no conversion needed
    /// Notes:         larger values mean less bad pixels detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT bcc_offset_t2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// correction_threshold
    ///
    /// Tuning Level:  OftenTune
    /// Description:   if departure of correction value from the original value is larger than this threshold, the pixel will be
    ///                corrected
    /// Type:          uint
    /// Default Value: 128
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Conversion:    no conversion needed
    /// Notes:         larger values mean less bad pixels will be corrected
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT correction_threshold;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// remove_along_edge
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   bad pixel along the edge feature enable or disable
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    no conversion needed
    /// Notes:         when it is enabled, more bad pixels on the edges will be detected and corrected
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT remove_along_edge;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// using_cross_channel
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   use cross channel information for detection and recovery
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    no conversion needed
    /// Notes:         cross channel has less color artifacts
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT using_cross_channel;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// saturation_threshold
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   pixels have value higher than the threshold is considered as saturated
    /// Type:          uint
    /// Default Value: 1023
    /// Range:         [0, 16383]
    /// Bit Depth:     14u
    /// Conversion:    no conversion needed
    /// Notes:         larger value mean more bad pixels will be detected, more likely to have false detection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT saturation_threshold;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// pdpc20_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct pdpc20_enable_sectionStruct
{
    UINT32 reserved;
    INT32  pdpc_enable;
    INT32  dsbpc_enable;
};

struct pedestal13_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// channel_black_level_r_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel black level. Increasing the value increases the black subtraction and increase the
    ///                contrast.
    /// Type:          uint
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [0, 4095]
    /// Bit Depth:      12u
    /// Length:        130
    /// Conversion:    Bitwidth conversion if pipeline bitwidth is not 14bit
    /// Notes:         2-D black correction to replace the single black level. Black level in chromatix_linearization_type data
    ///                should be set to zero. Use parameters from chromatix_black_level_type to adjust black for low light if
    ///                needed. Use trigger and max_blk_increase to interpolate blk_increase, and add blk_increase to each node in
    ///                LUTs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct channel_black_level_r_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// channel_black_level_r - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [0, 4095]
        /// Length:        130
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT channel_black_level_r[130];
    } channel_black_level_r_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// channel_black_level_gr_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel black level. Increasing the value increases the black subtraction and increase the
    ///                contrast.
    /// Type:          uint
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [0, 4095]
    /// Bit Depth:      12u
    /// Length:        130
    /// Conversion:    Bitwidth conversion if pipeline bitwidth is not 14bit
    /// Notes:         2-D black correction to replace the single black level. Black level in chromatix_linearization_type data
    ///                should be set to zero. Use parameters from chromatix_black_level_type to adjust black for low light if
    ///                needed. Use trigger and max_blk_increase to interpolate blk_increase, and add blk_increase to each node in
    ///                LUTs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct channel_black_level_gr_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// channel_black_level_gr - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [0, 4095]
        /// Length:        130
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT channel_black_level_gr[130];
    } channel_black_level_gr_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// channel_black_level_gb_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel black level. Increasing the value increases the black subtraction and increase the
    ///                contrast.
    /// Type:          uint
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [0, 4095]
    /// Bit Depth:      12u
    /// Length:        130
    /// Conversion:    Bitwidth conversion if pipeline bitwidth is not 14bit
    /// Notes:         2-D black correction to replace the single black level. Black level in chromatix_linearization_type data
    ///                should be set to zero. Use parameters from chromatix_black_level_type to adjust black for low light if
    ///                needed. Use trigger and max_blk_increase to interpolate blk_increase, and add blk_increase to each node in
    ///                LUTs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct channel_black_level_gb_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// channel_black_level_gb - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [0, 4095]
        /// Length:        130
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT channel_black_level_gb[130];
    } channel_black_level_gb_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// channel_black_level_b_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   R/Gr/Gb/B channel black level. Increasing the value increases the black subtraction and increase the
    ///                contrast.
    /// Type:          uint
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [0, 4095]
    /// Bit Depth:      12u
    /// Length:        130
    /// Conversion:    Bitwidth conversion if pipeline bitwidth is not 14bit
    /// Notes:         2-D black correction to replace the single black level. Black level in chromatix_linearization_type data
    ///                should be set to zero. Use parameters from chromatix_black_level_type to adjust black for low light if
    ///                needed. Use trigger and max_blk_increase to interpolate blk_increase, and add blk_increase to each node in
    ///                LUTs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct channel_black_level_b_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// channel_black_level_b - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [0, 4095]
        /// Length:        130
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT channel_black_level_b[130];
    } channel_black_level_b_tab;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// pedestal13_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct pedestal13_enable_sectionStruct
{
    UINT32 reserved;
    INT32  pedestal_enable;
};

struct cr_cb_triangle
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// point1 - Array
    ///
    /// Length:        2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 point1[2];

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// point2 - Array
    ///
    /// Length:        2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 point2[2];

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// point3 - Array
    ///
    /// Length:        2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 point3[2];
};

struct cr_cb_triangle_set
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// traingle1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cr_cb_triangle traingle1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// traingle2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cr_cb_triangle traingle2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// traingle3
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cr_cb_triangle traingle3;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// traingle4
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cr_cb_triangle traingle4;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// traingle5
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cr_cb_triangle traingle5;
};

struct sce11_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ori_triangle
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Three (Cr,Cb) points form a triangle. It changes color from ori triangular area to target triangular area.
    /// Default Value: {40, -30, 17, -3, 10, -20}, {40, -30, 70, -25, 17, -3}, {40, -30, 65, -55, 70, -25}, {40, -30, 45, -90, 65, -55}, {40, -30, 10, -20, 45, -90}
    /// Range:         [-128, 127]
    /// Bit Depth:     (8+e)bit, signed
    /// Conversion:    No Conversion needed
    /// Notes:         Special effect default value (ori == target): i. acc_color_green = {{-64, -64, -10, 0, -128, 0}, {-64,
    ///                -64, 0, -10, -10, 0}, {-64, -64, 0, -128, 0, -10}, {-64, -64, -128, -128, 0, -128}, {-64, -64, -128, 0,
    ///                -128, -128}}; ii. acc_color_blue = {{0, 127, -128, 127, -64, 64}, {-64, 64, -128, 127, -128, 0}, {-64, 64,
    ///                -128, 0, 0, 0}, {-64, 64, 0, 0, 0, 64}, {0, 127, -64, 64, 0, 64}}; iii. acc_color_orange = {{127, 0, 0, 0,
    ///                64, -64}, {0, 0, 0, -128, 64, -64}, {64, -64, 0, -128, 127, -128}, {127, -64, 64, -64, 127, -128}, {127,
    ///                -0, 64, -64, 127, -64}}
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cr_cb_triangle_set ori_triangle;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// target_triangle
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Three (Cr,Cb) points form a triangle. It changes color from ori triangular area to target triangular area.
    /// Default Value: {40, -30, 17, -3, 10, -20}, {40, -30, 70, -25, 17, -3}, {40, -30, 65, -55, 70, -25}, {40, -30, 45, -90, 65, -55}, {40, -30, 10, -20, 45, -90}
    /// Range:         [-128, 127]
    /// Bit Depth:     (8+e)bit, signed
    /// Conversion:    No Conversion needed
    /// Notes:         Special effect default value (ori == target): i. acc_color_green = {{-64, -64, -10, 0, -128, 0}, {-64,
    ///                -64, 0, -10, -10, 0}, {-64, -64, 0, -128, 0, -10}, {-64, -64, -128, -128, 0, -128}, {-64, -64, -128, 0,
    ///                -128, -128}}; ii. acc_color_blue = {{0, 127, -128, 127, -64, 64}, {-64, 64, -128, 127, -128, 0}, {-64, 64,
    ///                -128, 0, 0, 0}, {-64, 64, 0, 0, 0, 64}, {0, 127, -64, 64, 0, 64}}; iii. acc_color_orange = {{127, 0, 0, 0,
    ///                64, -64}, {0, 0, 0, -128, 64, -64}, {64, -64, 0, -128, 127, -128}, {127, -64, 64, -64, 127, -128}, {127,
    ///                -0, 64, -64, 127, -64}}
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cr_cb_triangle_set target_triangle;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// shift_vector_cb
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   shift vector for Cb.
    /// Type:          float
    /// Default Value: -0.64
    /// Range:         [0.0, 1.0]
    /// Bit Depth:      4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT              shift_vector_cb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// shift_vector_cr
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   shift vector for Cr.
    /// Type:          float
    /// Default Value: 0.768
    /// Range:         [0.0, 1.0]
    /// Bit Depth:      4u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT              shift_vector_cr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_sce11_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_sce11_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   2x3 affine transform matrix for colors outside region. Usually used for special effect.
    /// Type:          float
    /// Default Value: 1
    /// Range:         [-2048, 2047]
    /// Bit Depth:      12s
    /// Conversion:    No Conversion needed
    /// Notes:         cr' = [ a b c] cr; cb' = [d e f] cb; 1 = [0 0 1] 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT a;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// b
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   2x3 affine transform matrix for colors outside region. Usually used for special effect.
    /// Type:          float
    /// Default Value: 0
    /// Range:         [-2048, 2047]
    /// Bit Depth:      12s
    /// Conversion:    No Conversion needed
    /// Notes:         cr' = [ a b c] cr; cb' = [d e f] cb; 1 = [0 0 1] 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT b;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// c
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   2x3 affine transform matrix for colors outside region. Usually used for special effect.
    /// Type:          float
    /// Default Value: 0
    /// Range:         [-2048, 2047]
    /// Bit Depth:      12s
    /// Conversion:    No Conversion needed
    /// Notes:         cr' = [ a b c] cr; cb' = [d e f] cb; 1 = [0 0 1] 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// d
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   2x3 affine transform matrix for colors outside region. Usually used for special effect.
    /// Type:          float
    /// Default Value: 0
    /// Range:         [-2048, 2047]
    /// Bit Depth:      12s
    /// Conversion:    No Conversion needed
    /// Notes:         cr' = [ a b c] cr; cb' = [d e f] cb; 1 = [0 0 1] 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT d;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// e
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   2x3 affine transform matrix for colors outside region. Usually used for special effect.
    /// Type:          float
    /// Default Value: 1
    /// Range:         [-2048, 2047]
    /// Bit Depth:      12s
    /// Conversion:    No Conversion needed
    /// Notes:         cr' = [ a b c] cr; cb' = [d e f] cb; 1 = [0 0 1] 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT e;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// f
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   2x3 affine transform matrix for colors outside region. Usually used for special effect.
    /// Type:          float
    /// Default Value: 0
    /// Range:         [-2048, 2047]
    /// Bit Depth:      12s
    /// Conversion:    No Conversion needed
    /// Notes:         cr' = [ a b c] cr; cb' = [d e f] cb; 1 = [0 0 1] 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT f;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// sce11_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct sce11_enable_sectionStruct
{
    UINT32 reserved;
    INT32  sce_enable;
};

struct tdl10_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lut_2d_h_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   delta 2D LUT for hue.
    /// Type:          float
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [-360, 360]
    /// Bit Depth:      10s
    /// Length:        24x16
    /// Conversion:    No Conversion needed
    /// Notes:         row 24 should be the same as row 0 data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct lut_2d_h_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// lut_2d_h - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [-360, 360]
        /// Length:        384
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT lut_2d_h[384];
    } lut_2d_h_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lut_2d_s_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   delta 2D LUT for saturation.
    /// Type:          float
    /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    /// Range:         [-1.0, 1.0]
    /// Bit Depth:      10s
    /// Length:        24x16
    /// Conversion:    No Conversion needed
    /// Notes:         row 24 should be the same as row 0 data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct lut_2d_s_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// lut_2d_s - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        /// Range:         [-1.0, 1.0]
        /// Length:        384
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT lut_2d_s[384];
    } lut_2d_s_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// l_boundary_start_a
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   to eliminate hue and saturation change on low Luminance, set Luminance start boundary which only will not
    ///                change hue and saturation by 2D LUT. Lower value make more hue and saturation change.
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0.0f, 1.0f]
    /// Bit Depth:      11u
    /// Conversion:    No Conversion needed
    /// Notes:         Setting this value to 1.0f means no hue and saturation change at all.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT        l_boundary_start_a;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// l_boundary_start_b
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   to eliminate hue and saturation change on low Luminance, set Luminance start boundary which only will not
    ///                change hue and saturation by 2D LUT. Lower value make more hue and saturation change.
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0.0f, 1.0f]
    /// Bit Depth:      11u
    /// Conversion:    No Conversion needed
    /// Notes:         Setting this value to 1.0f means no hue and saturation change at all.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT        l_boundary_start_b;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// l_boundary_end_a
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   to eliminate hue and saturation change on high Luminance, set Luminance end boundary which only will not
    ///                change hue and saturation by 2D LUT. Higher value make more hue and saturation change.
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0f, 1.0f]
    /// Bit Depth:      11u
    /// Conversion:    No Conversion needed
    /// Notes:         Setting this value to 0.0f means no hue and saturation change at all.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT        l_boundary_end_a;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// l_boundary_end_b
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   to eliminate hue and saturation change on high Luminance, set Luminance end boundary which only will not
    ///                change hue and saturation by 2D LUT. Higher value make more hue and saturation change.
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0f, 1.0f]
    /// Bit Depth:      11u
    /// Conversion:    No Conversion needed
    /// Notes:         Setting this value to 0.0f means no hue and saturation change at all.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT        l_boundary_end_b;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// y_blend_factor_integer
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   hue and saturation change can cause Y value change. To prevent Y value change, calculate Y offset between
    ///                input Y and output Y and recover output Y value as same as input Y according to this factor. Lower value
    ///                means less Y recovery and higher value means more Y recovery.
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0.0f, 1.0f]
    /// Bit Depth:      5uQ4
    /// Conversion:    No Conversion needed
    /// Notes:         Setting this value to 0.0f means hue and saturation will be changed as intended even though Y value
    ///                changes. On the other hands, setting this value to 1.0f means hue and saturation will not be changed as
    ///                intended because of Y recovery.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT        y_blend_factor_integer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_tdl10_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_tdl10_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lut_1d_h_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   knee point 1D LUT for hue which will define the original point you want to move
    /// Type:          float
    /// Default Value: 0,15,30,45,60,75,90,105,120,135,150,165,180,195,210,225,240,255,270,285,300,315,330,345,360
    /// Range:         [0,360]
    /// Bit Depth:     14u
    /// Length:        25
    /// Conversion:    No Conversion needed
    /// Notes:         i. LUT are always programmed in ascending order. ii. No two entries for the LUT can be equal; iii. Hue's
    ///                first LUT is always 0; iv. Hue's last LUT(25 for h/w) is always 360.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct lut_1d_h_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// lut_1d_h - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0,15,30,45,60,75,90,105,120,135,150,165,180,195,210,225,240,255,270,285,300,315,330,345,360
        /// Range:         [0,360]
        /// Length:        25
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT lut_1d_h[25];
    } lut_1d_h_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lut_1d_s_tab
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   knee point 1D LUT for saturation which will define the original point you want to move
    /// Type:          float
    /// Default Value: 0.0625,0.125,0.1875,0.25,0.3125,0.375,0.4375,0.5,0.5625,0.625,0.6875,0.75,0.8125,0.875,0.9375,1.0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:      12u
    /// Length:        16
    /// Conversion:    No Conversion needed
    /// Notes:         i. LUT is always programmed in ascending order; ii. No two entries of LUT can be equal; iii. Saturation’s
    ///                first entry will define gray area which will not change at all; iv. Saturation's last LUT is always Max
    ///                (2048).
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct lut_1d_s_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// lut_1d_s - Array
        ///
        /// Tuning Level:  RarelyTune
        /// Default Value: 0.0625,0.125,0.1875,0.25,0.3125,0.375,0.4375,0.5,0.5625,0.625,0.6875,0.75,0.8125,0.875,0.9375,1.0
        /// Range:         [0.0, 1.0]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT lut_1d_s[16];
    } lut_1d_s_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// k_b_integer
    ///
    /// Tuning Level:  NeverTune
    /// Description:   to calculate Y offset to recover Y value, RGB to Y coefficient can be tuned.
    /// Type:          float
    /// Default Value: 0.114
    /// Range:         [0.0f, 0.5f]
    /// Bit Depth:     8uQ9
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT        k_b_integer;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// k_r_integer
    ///
    /// Tuning Level:  NeverTune
    /// Description:   to calculate Y offset to recover Y value, RGB to Y coefficient can be tuned.
    /// Type:          float
    /// Default Value: 0.299
    /// Range:         [0.0f, 0.5f]
    /// Bit Depth:     8uQ9
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT        k_r_integer;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// h_shift
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   adjust 2d delta hue range. When h_shift=5, delta hue range is +-360 degree. When h_shift=1, delta hue
    ///                range is +-30 degree.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 5]
    /// Bit Depth:     3u
    /// Conversion:    No Conversion needed
    /// Notes:         (deltaH SHIFT_LEFT h_shift) LEFT_ARROW 6*2^11=12288.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32       h_shift;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// s_shift
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   adjust 2d delta saturation range. When s_shift=2, delta saturation range is +-1.0. When s_shift=0, delta
    ///                saturation range is +-0.25.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 3]
    /// Bit Depth:     2u
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32       s_shift;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// tdl10_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct tdl10_enable_sectionStruct
{
    UINT32 reserved;
    INT32  twodlut_enable;
};

struct tf10_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// en
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Enable of TF module (including upscaler) per pass. When 0, ANR image is forwarded to TF output.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_y_ytb_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Contribution of luma to Y noise threshold, sampled at 17 points.
    /// Default Value: 100
    /// Range:         [0, 4095]
    /// Bit Depth:     12u, Q2
    /// Length:        17
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_params_y_ytb_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_params_y_ytb - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 100
        /// Range:         [0, 4095]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_params_y_ytb[17];
    } noise_params_y_ytb_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_y_ctb_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Contribution of chroma to Y noise threshold, specified for 8 extreme points.
    /// Default Value: 0
    /// Range:         [-1024, 1023]
    /// Bit Depth:     11s
    /// Length:        8
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_params_y_ctb_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_params_y_ctb - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_params_y_ctb[8];
    } noise_params_y_ctb_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_y_top_lim
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Upper limit of Luma noise threshold.
    /// Type:          uint
    /// Default Value: 100
    /// Range:         [0, 4095]
    /// Bit Depth:     12u, Q2
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       noise_params_y_top_lim;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_y_bot_lim
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Lower limit of luma noise threshold.
    /// Type:          uint
    /// Default Value: 100
    /// Range:         [0, 4095]
    /// Bit Depth:     12u, Q2
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       noise_params_y_bot_lim;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_cb_ytb_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Contribution of luma to Cb noise threshold, sampled at 17 points.
    /// Default Value: 30
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Length:        17
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_params_cb_ytb_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_params_cb_ytb - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 30
        /// Range:         [0, 1023]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_params_cb_ytb[17];
    } noise_params_cb_ytb_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_cb_ctb_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Contribution of chroma to Cb noise threshold, specified for 8 extreme points.
    /// Default Value: 0
    /// Range:         [-1024, 1023]
    /// Bit Depth:     11s
    /// Length:        8
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_params_cb_ctb_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_params_cb_ctb - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_params_cb_ctb[8];
    } noise_params_cb_ctb_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_cb_top_lim
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Upper limit of Cb noise threshold.
    /// Type:          uint
    /// Default Value: 30
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       noise_params_cb_top_lim;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_cb_bot_lim
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Lower limit of Cb noise threshold.
    /// Type:          uint
    /// Default Value: 30
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       noise_params_cb_bot_lim;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_cr_ytb_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Contribution of luma to Cr noise threshold, sampled at 17 points.
    /// Default Value: 30
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Length:        17
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_params_cr_ytb_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_params_cr_ytb - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 30
        /// Range:         [0, 1023]
        /// Length:        17
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_params_cr_ytb[17];
    } noise_params_cr_ytb_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_cr_ctb_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Contribution of chroma to Cr noise threshold, specified for 8 extreme points.
    /// Default Value: 0
    /// Range:         [-1024, 1023]
    /// Bit Depth:     11s
    /// Length:        8
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct noise_params_cr_ctb_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_params_cr_ctb - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 0
        /// Range:         [-1024, 1023]
        /// Length:        8
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT noise_params_cr_ctb[8];
    } noise_params_cr_ctb_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_cr_top_lim
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Upper limit of Cr noise threshold.
    /// Type:          uint
    /// Default Value: 30
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       noise_params_cr_top_lim;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// noise_params_cr_bot_lim
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Lower limit of Cr noise threshold.
    /// Type:          uint
    /// Default Value: 30
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       noise_params_cr_bot_lim;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a2_max_y
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Upper limit for a2 blending factor calculation (used in case FS=0) in Luma.
    /// Type:          uint
    /// Default Value: 250
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a2_max_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a2_min_y
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Lower limit for a2 blending factor calculation in Luma (this value may never be reached if a2_slope_y is
    ///                too small).
    /// Type:          uint
    /// Default Value: 25
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a2_min_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a2_slope_y
    ///
    /// Tuning Level:  OftenTune
    /// Description:   a2 slope as a function of FS (a2 starts from a2_max_y at FS=0 and goes down with slope a2_slope_y as FS
    ///                increases). Used for Luma.
    /// Type:          uint
    /// Default Value: 114
    /// Range:         [0, 255]
    /// Bit Depth:     8u, Q5
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a2_slope_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a2_max_c
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Upper limit for a2 blending factor calculation (used in case FS=0) in Chroma.
    /// Type:          uint
    /// Default Value: 250
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a2_max_c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a2_min_c
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Lower limit for a2 blending factor calculation in Chroma (this value may never be reached if a2_slope_c is
    ///                too small).
    /// Type:          uint
    /// Default Value: 180
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a2_min_c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a2_slope_c
    ///
    /// Tuning Level:  OftenTune
    /// Description:   a2 slope as a function of FS (a2 starts from a2_max_c at FS=0 and goes down with slope a2_slope_c as FS
    ///                increases). Used for Chroma.
    /// Type:          uint
    /// Default Value: 36
    /// Range:         [0, 255]
    /// Bit Depth:     8u, Q5
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a2_slope_c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// is_same_blending_for_all_frequencies
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Forces same blending factor both for low and for high frequencies during blending.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear Floor Interpolation: Do Linear Interpolation and then Floor.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       is_same_blending_for_all_frequencies;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_to_a1_map_y_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Sparse mapping between FS values [0:8:64] and a1 blending factor for Luma.
    /// Default Value: 0,24,49,73,98,123,147,172,197
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_to_a1_map_y_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_to_a1_map_y - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,24,49,73,98,123,147,172,197
        /// Range:         [0, 255]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_to_a1_map_y[9];
    } fs_to_a1_map_y_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_to_a1_map_c_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Sparse mapping between FS values [0:8:64] and a1 blending factor for Chroma.
    /// Default Value: 0,24,49,73,98,123,147,172,197
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_to_a1_map_c_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_to_a1_map_c - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,24,49,73,98,123,147,172,197
        /// Range:         [0, 255]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_to_a1_map_c[9];
    } fs_to_a1_map_c_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_to_a4_map_y_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Sparse mapping between FS values [0:8:64] and a4 blending factor for Luma.
    /// Default Value: 0,24,49,73,98,123,147,172,197
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_to_a4_map_y_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_to_a4_map_y - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,24,49,73,98,123,147,172,197
        /// Range:         [0, 255]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_to_a4_map_y[9];
    } fs_to_a4_map_y_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_to_a4_map_c_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Sparse mapping between FS values [0:8:64] and a4 blending factor for Chroma.
    /// Default Value: 0,24,49,73,98,123,147,172,197
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_to_a4_map_c_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_to_a4_map_c - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,24,49,73,98,123,147,172,197
        /// Range:         [0, 255]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_to_a4_map_c[9];
    } fs_to_a4_map_c_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dither_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Whether to apply dithering to Y channel of final TF output.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       dither_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dither_cb
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Whether to apply dithering to Cb channel of final TF output.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       dither_cb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dither_cr
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Whether to apply dithering to Cr channel of final TF output.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       dither_cr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lnr_lut_y_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   The LUT of scale factors to be applied to Luma noise thresholds (this LUT is accessed by normalized
    ///                distance to optical center).
    /// Default Value: 128
    /// Range:         [0, 511]
    /// Bit Depth:     9u, Q7
    /// Length:        16
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct lnr_lut_y_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// lnr_lut_y - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 128
        /// Range:         [0, 511]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT lnr_lut_y[16];
    } lnr_lut_y_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lnr_lut_c_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   The LUT of scale factors to be applied to Chroma noise thresholds (this LUT is accessed by normalized
    ///                distance to optical center).
    /// Default Value: 128
    /// Range:         [0, 511]
    /// Bit Depth:     9u, Q7
    /// Length:        16
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct lnr_lut_c_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// lnr_lut_c - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 128
        /// Range:         [0, 511]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT lnr_lut_c[16];
    } lnr_lut_c_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lnr_opt_center_x
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Optical center coordinate (Q15) in virtual coordinate system of [0,1]x[0,1].
    /// Type:          uint
    /// Default Value: 8192
    /// Range:         [0, 16384]
    /// Bit Depth:     15u, Q14
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       lnr_opt_center_x;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lnr_opt_center_y
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Optical center coordinate (Q15) in virtual coordinate system of [0,1]x[0,1].
    /// Type:          uint
    /// Default Value: 8192
    /// Range:         [0, 16384]
    /// Bit Depth:     15u, Q14
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       lnr_opt_center_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lnr_ellipses_bounding_rect_w
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Defines the minimal bounding rectangle of LNR geometry (in virtual coordinate system of [0,1]x[0,1]). LNR
    ///                gains are defined on uniformly sampled ellipses around the optical center, with lnr_ellipse_bounding_rect
    ///                being the minimal bounding rectangle of the biggest ellipse. CONSTRAINT: the whole [0,1]x[0,1] image
    ///                should be fully contained in the rectangle twice bigger than lnr_ellipse_bounding_rect.
    /// Type:          uint
    /// Default Value: 16384
    /// Range:         [8192, 65535]
    /// Bit Depth:     16u, Q14
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       lnr_ellipses_bounding_rect_w;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// lnr_ellipses_bounding_rect_h
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Defines the minimal bounding rectangle of LNR geometry (in virtual coordinate system of [0,1]x[0,1]). LNR
    ///                gains are defined on uniformly sampled ellipses around the optical center, with lnr_ellipse_bounding_rect
    ///                being the minimal bounding rectangle of the biggest ellipse. CONSTRAINT: the whole [0,1]x[0,1] image
    ///                should be fully contained in the rectangle twice bigger than lnr_ellipse_bounding_rect.
    /// Type:          uint
    /// Default Value: 16384
    /// Range:         [8192, 65535]
    /// Bit Depth:     16u, Q14
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       lnr_ellipses_bounding_rect_h;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_y_c1_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Multiplication factor used in Luma thresholds calculation to form weak threshold (multiplies basic noise
    ///                threshold).
    /// Default Value: 0,2,4,6,8,10,12,14,16
    /// Range:         [0, 31]
    /// Bit Depth:     5u, Q4
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_decision_params_y_c1_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_decision_params_y_c1 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,2,4,6,8,10,12,14,16
        /// Range:         [0, 31]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_decision_params_y_c1[9];
    } fs_decision_params_y_c1_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_y_c2_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Offset used in Luma thresholds calculation to form weak threshold.
    /// Default Value: 0,0,0,0,0,0,0,0,0
    /// Range:         [0, 8191]
    /// Bit Depth:     13u, Q4
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_decision_params_y_c2_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_decision_params_y_c2 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,0,0,0,0,0,0,0,0
        /// Range:         [0, 8191]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_decision_params_y_c2[9];
    } fs_decision_params_y_c2_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_y_c3_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Multiplication factor used in Luma thresholds calculation to form strong threshold (multiplies basic noise
    ///                threshold).
    /// Default Value: 0,3,7,11,15,19,23,27,31
    /// Range:         [0, 63]
    /// Bit Depth:     6u, Q4
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_decision_params_y_c3_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_decision_params_y_c3 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,3,7,11,15,19,23,27,31
        /// Range:         [0, 63]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_decision_params_y_c3[9];
    } fs_decision_params_y_c3_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_y_c4_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Offset used in Luma thresholds calculation to form strong threshold .
    /// Default Value: 64,64,64,64,64,64,64,64,64
    /// Range:         [0, 16383]
    /// Bit Depth:     14u, Q4
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_decision_params_y_c4_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_decision_params_y_c4 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 64,64,64,64,64,64,64,64,64
        /// Range:         [0, 16383]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_decision_params_y_c4[9];
    } fs_decision_params_y_c4_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_oof_y_c1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Multiplication factor used in Luma thresholds calculation to form weak threshold (multiplies basic noise
    ///                threshold). Used in case of out-of-frame Luma pixel.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 31]
    /// Bit Depth:     5u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       fs_decision_params_oof_y_c1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_oof_y_c2
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Offset used in Luma thresholds calculation to form weak threshold. Used in case of out-of-frame Luma
    ///                pixel.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 8191]
    /// Bit Depth:     13u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       fs_decision_params_oof_y_c2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_oof_y_c3
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Multiplication factor used in Luma thresholds calculation to form strong threshold (multiplies basic noise
    ///                threshold). Used in case of out-of-frame Luma pixel.
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [0, 63]
    /// Bit Depth:     6u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       fs_decision_params_oof_y_c3;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_oof_y_c4
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Offset used in Luma thresholds calculation to form strong threshold. Used in case of out-of-frame Luma
    ///                pixel.
    /// Type:          uint
    /// Default Value: 16
    /// Range:         [0, 16383]
    /// Bit Depth:     14u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       fs_decision_params_oof_y_c4;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_c_c1_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Multiplication factor used in Chroma thresholds calculation to form weak threshold (multiplies basic noise
    ///                threshold) .
    /// Default Value: 0,2,4,6,8,10,12,14,16
    /// Range:         [0, 31]
    /// Bit Depth:     5u, Q4
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_decision_params_c_c1_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_decision_params_c_c1 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,2,4,6,8,10,12,14,16
        /// Range:         [0, 31]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_decision_params_c_c1[9];
    } fs_decision_params_c_c1_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_c_c2_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Offset used in Chroma thresholds calculation to form weak threshold.
    /// Default Value: 0,0,0,0,0,0,0,0,0
    /// Range:         [0, 8191]
    /// Bit Depth:     13u, Q4
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_decision_params_c_c2_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_decision_params_c_c2 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,0,0,0,0,0,0,0,0
        /// Range:         [0, 8191]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_decision_params_c_c2[9];
    } fs_decision_params_c_c2_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_c_c3_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Multiplication factor used in Chroma thresholds calculation to form strong threshold (multiplies basic
    ///                noise threshold).
    /// Default Value: 0,3,7,11,15,19,23,27,31
    /// Range:         [0, 63]
    /// Bit Depth:     6u, Q4
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_decision_params_c_c3_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_decision_params_c_c3 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 0,3,7,11,15,19,23,27,31
        /// Range:         [0, 63]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_decision_params_c_c3[9];
    } fs_decision_params_c_c3_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_c_c4_tab
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Offset used in Chroma thresholds calculation to form strong threshold.
    /// Default Value: 64,64,64,64,64,64,64,64,64
    /// Range:         [0, 16383]
    /// Bit Depth:     14u, Q4
    /// Length:        9
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct fs_decision_params_c_c4_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fs_decision_params_c_c4 - Array
        ///
        /// Tuning Level:  OftenTune
        /// Default Value: 64,64,64,64,64,64,64,64,64
        /// Range:         [0, 16383]
        /// Length:        9
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT fs_decision_params_c_c4[9];
    } fs_decision_params_c_c4_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_oof_c_c1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Multiplication factor used in Chroma thresholds calculation to form weak threshold (multiplies basic noise
    ///                threshold). Used in case of out-of-frame Luma pixel.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 31]
    /// Bit Depth:     5u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       fs_decision_params_oof_c_c1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_oof_c_c2
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Offset used in Chroma thresholds calculation to form weak threshold. Used in case of out-of-frame Luma
    ///                pixel.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 8191]
    /// Bit Depth:     13u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       fs_decision_params_oof_c_c2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_oof_c_c3
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Multiplication factor used in Chroma thresholds calculation to form strong threshold (multiplies basic
    ///                noise threshold). Used in case of out-of-frame Luma pixel.
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [0, 63]
    /// Bit Depth:     6u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       fs_decision_params_oof_c_c3;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// fs_decision_params_oof_c_c4
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Offset used in Chroma thresholds calculation to form strong threshold. Used in case of out-of-frame Luma
    ///                pixel.
    /// Type:          uint
    /// Default Value: 16
    /// Range:         [0, 16383]
    /// Bit Depth:     14u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       fs_decision_params_oof_c_c4;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// apply_fs_rank_filter
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Whether to apply peak removal on the FS image (i.e. on the result of FS decision).
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       apply_fs_rank_filter;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// apply_fs_lpf
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Whether to apply low pass filter on the FS image (i.e. on the result of FS decision).
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       apply_fs_lpf;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// sad_y_calc_mode
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Window size for SAD Y calculation. 0 = 7x5 window, 1 = 5x5 window, 2 = 3x3 window, 3 = 1x1 window.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 3]
    /// Bit Depth:     2u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       sad_y_calc_mode;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// sad_c_calc_mode
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Window size for SAD Cb and Cr calculations. 0 = 3x3 window, 1 = 5x3 window.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       sad_c_calc_mode;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// use_indications
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Whether to use indications from lower resolutions (should be 0 for lowest resolution pass where there is
    ///                no indications input).
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       use_indications;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// indications_affect_fs_decision_also_directly
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   When this flag is 1, indications affect FS decision also directly, by taking minimum of normal FS decision
    ///                (i.e. at current resolution). and the indications value converted to 6 bits (i.e. from lower resolution;
    ///                for out of frame pixels the indication value is clipped to out_of_frame_pixels_confidence before
    ///                conversion). Evidently, in this mode, if ghost detection is disabled (by setting both
    ///                disable_luma_ghost_detection and disable_chroma_ghost_detection to 1), normal FS decisions are ignored and
    ///                only indications affect the FS decision. CONSTRAINT: Can be 1 only when use_indications is 1.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear Ceil Interpolation: Do Linear Interpolation and then Ceil.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       indications_affect_fs_decision_also_directly;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// morph_erode_size
    ///
    /// Tuning Level:  OftenTune
    /// Description:   0 = functional bypass (no filter), 1 = 5x5 window, 2 = 3x3 window.
    /// Type:          uint
    /// Default Value: 1
    /// Range:         [0, 2]
    /// Bit Depth:     2u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       morph_erode_size;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// morph_dilate_size
    ///
    /// Tuning Level:  OftenTune
    /// Description:   0 = functional bypass (no filter), 1 = 5x5 window, 2 = 3x3 window.
    /// Type:          uint
    /// Default Value: 2
    /// Range:         [0, 2]
    /// Bit Depth:     2u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       morph_dilate_size;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tr_enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Whether to enable warping transform refinement.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       tr_enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tr_block_num_x
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   refinement grid: number of columns (1...10).
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [1, 10]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       tr_block_num_x;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tr_block_num_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   refinement grid: number of rows (1...31).
    /// Type:          uint
    /// Default Value: 4
    /// Range:         [1, 31]
    /// Bit Depth:     5u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       tr_block_num_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tr_fs_threshold
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Minimal FS to contribute to transform refinement.
    /// Type:          uint
    /// Default Value: 60
    /// Range:         [0, 63]
    /// Bit Depth:     6u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       tr_fs_threshold;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tr_dead_zone
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Dead-zone for transform refinement results. 0 is equivalent to no dead-zone.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 3]
    /// Bit Depth:     2u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       tr_dead_zone;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tr_count_percentage_threshold
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Minimal percentage of good samples in a block to make its refinement valid.
    /// Type:          uint
    /// Default Value: 50
    /// Range:         [0, 100]
    /// Bit Depth:     7u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       tr_count_percentage_threshold;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// out_of_frame_pixels_confidence
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   When indications_affect_fs_decision_also_directly is 1, this value is used to update the indications in
    ///                out of frame regions (i.e. pixels going out of frame during global motion compensation) - as upper limit .
    /// Type:          uint
    /// Default Value: 8
    /// Range:         [0, 15]
    /// Bit Depth:     4u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       out_of_frame_pixels_confidence;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// is_dci_mode
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Whether TF is working in DCI mode – in this mode fs_to_a4_maps are not used.
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         Nearest neighbor interpolation: discrete values allowed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       is_dci_mode;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a3_t1_scale_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Multiplication factor used for determining weak Luma threshold for a3 (based on weak decision threshold
    ///                for FS).
    /// Type:          uint
    /// Default Value: 15
    /// Range:         [0, 31]
    /// Bit Depth:     5u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a3_t1_scale_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a3_t1_offs_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Offset used for determining weak Luma threshold for a3 (based on weak decision threshold for FS).
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1023]
    /// Bit Depth:     10u, Q5
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a3_t1_offs_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a3_t2_scale_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Multiplication factor used for determining strong Luma threshold for a3 (based on strong decision
    ///                threshold for FS).
    /// Type:          uint
    /// Default Value: 28
    /// Range:         [0, 63]
    /// Bit Depth:     6u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a3_t2_scale_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a3_t2_offs_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Offset used for determining strong Luma threshold for a3 (based on strong decision threshold for FS).
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 4095]
    /// Bit Depth:     12u, Q5
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a3_t2_offs_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a3_t1_scale_c
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Multiplication factor used for determining weak Chroma threshold for a3 (based on weak decision threshold
    ///                for FS).
    /// Type:          uint
    /// Default Value: 15
    /// Range:         [0, 31]
    /// Bit Depth:     5u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a3_t1_scale_c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a3_t1_offs_c
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Offset used for determining weak Chroma threshold for a3 (based on weak decision threshold for FS).
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 1023]
    /// Bit Depth:     10u, Q5
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a3_t1_offs_c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a3_t2_scale_c
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Multiplication factor used for determining strong Chroma threshold for a3 (based on strong decision
    ///                threshold for FS).
    /// Type:          uint
    /// Default Value: 28
    /// Range:         [0, 63]
    /// Bit Depth:     6u, Q4
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a3_t2_scale_c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// a3_t2_offs_c
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Offset used for determining strong Chroma threshold for a3 (based on strong decision threshold for FS).
    /// Type:          uint
    /// Default Value: 0
    /// Range:         [0, 4095]
    /// Bit Depth:     12u, Q5
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       a3_t2_offs_c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// constant_blending_factor_y
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Blending factor for Luma channel in case of constant blending mode.
    /// Type:          uint
    /// Default Value: 128
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       constant_blending_factor_y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// constant_blending_factor_c
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Blending factor for Chroma channels in case of constant blending mode.
    /// Type:          uint
    /// Default Value: 128
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       constant_blending_factor_c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// video_first_frame_spatial_nr_percentage
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   In video flow, in first frame: the spatial NR is configured somewhere between a2_max and a2_min. This
    ///                parameter defines the linearly increasing percentage: 0 =is greater than a2_min, 100 =is greater than
    ///                a2_max.
    /// Type:          uint
    /// Default Value: 50
    /// Range:         [0, 100]
    /// Bit Depth:     7u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       video_first_frame_spatial_nr_percentage;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// scene_cut_recovery_time
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Affects the algorithm of faster convergence in case of scene cut. When detectable scene-cut (e.g. exposure
    ///                change) happens, TF will usually significantly decrease temporal filtering. In such case temporal weights
    ///                should be kept small for some time to allow faster temporal noise convergence. This parameter defines the
    ///                allowed period in which the temporal weights should return to normal weights. When 0 - scene cut recovery
    ///                is disabled. When 128 - default algorithm behavior is used. When 255 - slowest recovery (according to the
    ///                algorithm) will be used.
    /// Type:          uint
    /// Default Value: 128
    /// Range:         [0, 255]
    /// Bit Depth:     8u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       scene_cut_recovery_time;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anr_final_blender_luma_min_strength_high_freq
    ///
    /// Tuning Level:  MustTune
    /// Description:   Controls adaptive blending of ANR output with ANR input for High Frequencies of Luma (see description of
    ///                anr_final_blender_strength_decision_ythr_low parameter for details). Effective strength = 0 means taking
    ///                only ANR input. Effective strength = 256 means taking only ANR output.
    /// Type:          uint
    /// Default Value: 256
    /// Range:         [0, 256]
    /// Bit Depth:     9u, Q8
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       anr_final_blender_luma_min_strength_high_freq;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anr_final_blender_luma_min_strength_low_freq
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Controls adaptive blending of ANR output with ANR input for Low Frequencies of Luma (see description of
    ///                anr_final_blender_strength_decision_ythr_low parameter for details). Effective strength = 0 means taking
    ///                only ANR input. Effective strength = 256 means taking only ANR output.
    /// Type:          uint
    /// Default Value: 256
    /// Range:         [0, 256]
    /// Bit Depth:     9u, Q8
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       anr_final_blender_luma_min_strength_low_freq;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anr_final_blender_chroma_min_strength_high_freq
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Controls adaptive blending of ANR output with ANR input for High Frequencies of Chroma (see description of
    ///                anr_final_blender_strength_decision_ythr_low parameter for details). Effective strength = 0 means taking
    ///                only ANR input. Effective strength = 256 means taking only ANR output.
    /// Type:          uint
    /// Default Value: 256
    /// Range:         [0, 256]
    /// Bit Depth:     9u, Q8
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       anr_final_blender_chroma_min_strength_high_freq;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anr_final_blender_chroma_min_strength_low_freq
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Controls adaptive blending of ANR output with ANR input for Low Frequencies of Chroma (see description of
    ///                anr_final_blender_strength_decision_ythr_low parameter for details). Effective strength = 0 means taking
    ///                only ANR input. Effective strength = 256 means taking only ANR output.
    /// Type:          uint
    /// Default Value: 256
    /// Range:         [0, 256]
    /// Bit Depth:     9u, Q8
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       anr_final_blender_chroma_min_strength_low_freq;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anr_final_blender_strength_decision_ythr_low
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Strength decision is based on dissimilarity decision between ANR input and ANR output. Similar pixels will
    ///                get strength=0, dissimilar pixels will get strength = 256; in-between, linear interpolation is used.
    ///                Similarity is calculated as min(Similarity_Y, Similarity_Cb, Similarity_Cr). If ABS(ANR_Y_IN-ANR_Y_OUT) is
    ///                less than anr_final_blender_strength_decision_ythr_low, then Similarity_Y = max;. if
    ///                ABS(ANR_Y_IN-ANR_Y_OUT) is greater or equal than anr_final_blender_strength_decision_ythr_high, then
    ///                Similarity_Y = min;. otherwise, linear interpolation is used. And similarly for Cb and Cr (using cthr
    ///                values): e.g. if ABS(ANR_Cb_IN-ANR_Cb_OUT) is less than anr_final_blender_strength_decision_cthr_low, then
    ///                Similarity_Cb = max;. if ABS(ANR_Cb_IN-ANR_Cb_OUT) is greater or equal than
    ///                anr_final_blender_strength_decision_cthr_high, then Similarity_Cb = min;. otherwise, linear interpolation
    ///                is used. This feature is designed to prevent peak noises back to the final image.
    /// Type:          uint
    /// Default Value: 20
    /// Range:         [0, 511]
    /// Bit Depth:     9u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       anr_final_blender_strength_decision_ythr_low;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anr_final_blender_strength_decision_ythr_high
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   see description of anr_final_blender_strength_decision_ythr_low parameter.
    /// Type:          uint
    /// Default Value: 307
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       anr_final_blender_strength_decision_ythr_high;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anr_final_blender_strength_decision_cthr_low
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   see description of anr_final_blender_strength_decision_ythr_low parameter.
    /// Type:          uint
    /// Default Value: 511
    /// Range:         [0, 511]
    /// Bit Depth:     9u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       anr_final_blender_strength_decision_cthr_low;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// anr_final_blender_strength_decision_cthr_high
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   see description of anr_final_blender_strength_decision_ythr_low parameter.
    /// Type:          uint
    /// Default Value: 1023
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         Linear interpolation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                       anr_final_blender_strength_decision_cthr_high;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// mod_tf10_pass_dataType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mod_tf10_pass_dataType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// pass_trigger
    ///
    /// Default Value: PASS_FULL(0),PASS_DC4(1),PASS_DC16(2),PASS_DC64(3)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 pass_trigger;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tf10_rgn_data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    tf10_rgn_dataType tf10_rgn_data;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// tf10_cct_dataType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct tf10_cct_dataType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// cct_data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct cct_dataStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// mod_tf10_pass_data - Array
        /// Length:        4
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        mod_tf10_pass_dataType mod_tf10_pass_data[4];
    } cct_data;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// tf10_enable_sectionStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct tf10_enable_sectionStruct
{
    UINT32 reserved;
    INT32  master_en;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// mod_tf10_pass_reserve_dataType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mod_tf10_pass_reserve_dataType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// pass_trigger
    ///
    /// Default Value: PASS_FULL(0),PASS_DC4(1),PASS_DC16(2),PASS_DC64(3)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 pass_trigger;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// pass_data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct pass_dataStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// blending_mode
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   0 = regular blending. 1 = copy IMG to TFO. 2 = copy ANRO to TFO. 3 = copy MCTFP to TFO. 4 = use blending
        ///                with constant factor between ANRO and MCTFP (this mode can not be used only when both inputs are available
        ///                to TF).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 4]
        /// Bit Depth:     3u
        /// Conversion:    No Conversion needed
        /// Notes:         Hide in GUI ! Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 blending_mode;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// is_anr_strength_blender_mode
        ///
        /// Tuning Level:  MustTune
        /// Description:   Whether to use TF unit as a strength blender for ANR. When 1 or 2 the TF is used as ANR post-processing
        ///                block and can not be used for temporal noise reduction. Usually used in still processing or MFNR Post. 0 -
        ///                use regular mode for Temporal Filtering. 1 - use as strength blender for ANR according to the
        ///                anr_final_blender parameters. (Basic mode). 2 - use as strength blender for ANR according to regular TF
        ///                parameters and not based on anr_final_blender parameters. (Advanced mode) (Example: Use this mode if one
        ///                wants to blend as a function of brighness or as a function of LNR).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 2]
        /// Bit Depth:     2u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 is_anr_strength_blender_mode;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// disable_luma_ghost_detection
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Whether ghost detection takes Luma into account. When both disable_luma_ghost_detection and
        ///                disable_chroma_ghost_detection are 1, ghost detection is bypassed. (setting filter strength either to
        ///                indication values or to zeros – depending on indications_affect_fs_decision_also_directly flag).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 disable_luma_ghost_detection;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// disable_chroma_ghost_detection
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Whether ghost detection takes Chroma into account. When both disable_luma_ghost_detection and
        ///                disable_chroma_ghost_detection are 1, ghost detection is bypassed. (setting filter strength either to
        ///                indication values or to zeros – depending on indications_affect_fs_decision_also_directly flag).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 disable_chroma_ghost_detection;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// enable_lnr
        ///
        /// Tuning Level:  OftenTune
        /// Description:   Whether to enable radial update of noise thresholds (due to lens roll-off).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 enable_lnr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_params_y_uv_limit
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Limit for chroma values that affect Luma noise threshold.
        /// Type:          uint
        /// Default Value: 511
        /// Range:         [0, 511]
        /// Bit Depth:     9u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 noise_params_y_uv_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_params_cb_uv_limit
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Limit for chroma values that affect Cb noise threshold.
        /// Type:          uint
        /// Default Value: 511
        /// Range:         [0, 511]
        /// Bit Depth:     9u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 noise_params_cb_uv_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// noise_params_cr_uv_limit
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Limit for chroma values that affect Cr noise threshold.
        /// Type:          uint
        /// Default Value: 511
        /// Range:         [0, 511]
        /// Bit Depth:     9u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 noise_params_cr_uv_limit;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// use_anr_for_decisions_y
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Whether to use spatially filtered Luma for ghost detection decisions instead of unfiltered Luma.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 use_anr_for_decisions_y;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// use_anr_for_decisions_c
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Whether to use spatially filtered Chroma for ghost detection decisions instead of unfiltered Chroma.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 use_anr_for_decisions_c;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// enable_noise_est_by_luma
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Enables dependence of noise thresholds on luma. (0 for power-saving mode: all noise thresholds will depend
        ///                only on chroma; if enable_noise_est_by_chroma is also 0 – noise thresholds will be constant). CONSTRAINT:
        ///                When disable_luma_ghost_detection is 1, enable_noise_est_by_luma should be 0.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Hide in GUI ! Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 enable_noise_est_by_luma;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// enable_noise_est_by_chroma
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Enables dependence of noise thresholds on chroma. (0 for power-saving mode: noise thresholds will depend
        ///                only on luma; if enable_noise_est_by_luma is also 0 – noise thresholds will be constant). CONSTRAINT: When
        ///                disable_chroma_ghost_detection is 1, enable_noise_est_by_chroma should be 0.
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Hide in GUI ! Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 enable_noise_est_by_chroma;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// padding_by_reflection_override
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   0 = Flow (i.e. SW) decisions may override chromatix value of padding_by_reflection, 1 = Chromatix value of
        ///                padding_by_reflection is used irrespective of flow decisions.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 padding_by_reflection_override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// padding_by_reflection
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Whether the padding in Luma anti-aliasing and SAD filters, is by reflection. When this flag is 0 (this is
        ///                the default value) the padding is done by duplication of the boundary pixel. When this flag is 1 the
        ///                padding is done by taking the value at a coordinate mirrored relative to the boundary.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 padding_by_reflection;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// morph_erode_en
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Enable morphological erode filter of indications (located before morphological dilate filter).
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 morph_erode_en;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// morph_dilate_en
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Enable morphological dilate filter of indications (located after morphological erode filter).
        /// Type:          uint
        /// Default Value: 1
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 morph_dilate_en;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// smear_inputs_for_decisions
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Determines whether to perform anti-aliasing (low pass) filter on Luma of the difference image prior to
        ///                using it in ghost detection. When this flag is 1, blending sub-module will be forced to bypass one of
        ///                inputs to TFO. (thus, anti-aliasing should be enabled only in low resolutions).
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Hide in GUI ! Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 smear_inputs_for_decisions;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// indications_premultiply_factor
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Multiplicative factor for decreasing the indication values (valid factor range is [0-16]). Value of 16
        ///                means no change to indications.
        /// Type:          uint
        /// Default Value: 16
        /// Range:         [0, 16]
        /// Bit Depth:     5u
        /// Conversion:    No Conversion needed
        /// Notes:         Hide in GUI ! Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 indications_premultiply_factor;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// invert_temporal_blending_weights
        ///
        /// Tuning Level:  RarelyTune
        /// Description:   Whether to take 1-a instead of a for a1 and a4 blending weights.
        /// Type:          uint
        /// Default Value: 0
        /// Range:         [0, 1]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         Hide in GUI ! Constant (per pass): no dynamic configuration even when control variable change.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UINT32 invert_temporal_blending_weights;
    } pass_data;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_tf10_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_tf10_reserveType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// mod_tf10_pass_reserve_data - Array
    /// Length:        4
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    mod_tf10_pass_reserve_dataType mod_tf10_pass_reserve_data[4];
};

struct tintless20_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tintless_threshold_tab
    ///
    /// Tuning Level:  MustTune
    /// Description:   Determine how much color shading will be treated as tint based on intensity.
    /// Type:          uint
    /// Default Value: 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
    /// Range:         [0, 10]
    /// Bit Depth:     4u
    /// Length:        16
    /// Notes:         Higher threshold will correct more color shading, but will also over correct actual color in scene.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct tintless_threshold_tabStruct
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// tintless_threshold - Array
        ///
        /// Tuning Level:  MustTune
        /// Default Value: 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
        /// Range:         [0, 10]
        /// Length:        16
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FLOAT tintless_threshold[16];
    } tintless_threshold_tab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tintless_high_accuracy_mode
    ///
    /// Tuning Level:  OftenTune
    /// Description:   Enable this to alleviate tint in shadow regions. A noisy sensor could use a higher value.
    /// Type:          uint
    /// Default Value: 8
    /// Range:         [0,255]
    /// Bit Depth:     8u
    /// Notes:         Can be used to run tintless on a noisy sensor.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  tintless_high_accuracy_mode;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tintless_update_delay
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Controls how frequently the color lens shade correction is updated.
    /// Type:          uint
    /// Default Value: 3
    /// Range:         [1,30]
    /// Bit Depth:     5u
    /// Notes:         A lower value means more frequent updates. The default value has the effect of updating the correction
    ///                table every 3rd preview frame
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  tintless_update_delay;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tintless_trace_percentage
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Tracing percentage of tintless to apply rolloff table.
    /// Type:          float
    /// Default Value: 0.4
    /// Range:         [0.0,1.0]
    /// Bit Depth:     8u
    /// Notes:         A lower value means less tracing. Rolloff may not be updated 100% as new tintless output and tracing
    ///                slowly. A higher value mean more aggressive tracing so it matches with new tintless output. It may cause
    ///                flickering.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  tintless_trace_percentage;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// center_weight
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   center compensation weight
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     16u
    /// Notes:         give center compensation weight and if it is 0, it means don't give weight on center
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  center_weight;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// corner_weight
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   corner compensation weight
    /// Type:          float
    /// Default Value: 1
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     16u
    /// Notes:         give corner compensation weight and if it is 0, it means don't give weight on corner
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                  corner_weight;
};

struct tmc10_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gtm_percentage
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Percentage of current DRC gain to be applied to GTM
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         sum of all percentages should be 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT gtm_percentage;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ltm_percentage
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Percentage of current DRC gain to be applied to LTM
    /// Type:          float
    /// Default Value: 0.5
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         sum of all percentages should be 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT ltm_percentage;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// la_percentage
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Percentage of current DRC gain to be applied to LA
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         sum of all percentages should be 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT la_percentage;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gamma_percentage
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Percentage of current DRC gain to be applied to GAMMA
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         sum of all percentages should be 1
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT gamma_percentage;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// knee_out_mid
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   mid-tone to be recovered from under-exposure image
    /// Type:          float
    /// Default Value: 0.2
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         more knee_out_mid for brighter image
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT knee_out_mid;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dark_boost_ratio
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Percentage of dark-boosting gain to be applied
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0.0, 1.0]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         more dark_boost_ratio for brighter image
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT dark_boost_ratio;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// core_rsv_para1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   none
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT core_rsv_para1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// core_rsv_para2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   none
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT core_rsv_para2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// core_rsv_para3
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   none
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT core_rsv_para3;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// core_rsv_para4
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   none
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT core_rsv_para4;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// core_rsv_para5
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   none
    /// Type:          float
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT core_rsv_para5;
};

struct upscale20_rgn_dataType
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// sharp_factor
    ///
    /// Tuning Level:  MustTune
    /// Description:   sharp_factor specifies sharpness/smooth level, positive for sharpness, 100 means 100% sharpness, negative
    ///                value for smooth, -100 means 100% smoth, 0 means no sharpening/smooth applied.
    /// Type:          float
    /// Default Value: 50
    /// Range:         [-100, 100]
    /// Bit Depth:     signed 8 bits
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT sharp_factor;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// upscale20_enable_section
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct upscale20_enable_sectionStruct
{
    UINT32 reserved;
    INT32  upscale_enable;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chromatix_upscale20_reserveType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct chromatix_upscale20_reserveType
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// enable
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Enable/Disable Detail Enhancerr
    /// Type:          int
    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1 bit
    /// Conversion:    No Conversion needed
    /// Notes:         0 DE disable, 1 DE enable
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 enable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// de_clip_shift
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   local dynamic clipping parameter within Detail Ehancement. When DE_CLIP_shift is more than 0, this feature
    ///                will be on. When it is on, the clipping would be based on the local supporting region’s minimal\maximal
    ///                levels, other than the global minimal\maximal levels (0 and 1023).
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 7]
    /// Bit Depth:     3 bits
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 de_clip_shift;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// sharpening_strength1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   sharpening strength level when ?out is no more than dECurveT1
    /// Type:          int
    /// Default Value: 0
    /// Range:         [-32, 160]
    /// Bit Depth:     9 bits. Signed Q0
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 sharpening_strength1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// sharpening_strength2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   sharpening strength level when ?out is more than dECurveT1
    /// Type:          int
    /// Default Value: 0
    /// Range:         [-32, 160]
    /// Bit Depth:     9 bits. Signed Q0
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 sharpening_strength2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// tquiet
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Quiet zone threshold
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 255]
    /// Bit Depth:     8 bits. Unsigned Q0
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 tquiet;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// decurvet1
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Threshold low (equal or larger than tquiet)
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 255]
    /// Bit Depth:     8 bits. Unsigned Q0
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 decurvet1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// decurvet2
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Threshold high (equal or larger than decurvet1)
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1023]
    /// Bit Depth:     10 bits. Unsigned Q0
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 decurvet2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// curverange
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   Dieout zone threshold (equal or larger than decurvet2)
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1023]
    /// Bit Depth:     4 bits. Unsigned Q0
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 curverange;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// delimiter
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   detail enhancement limit value
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 15]
    /// Bit Depth:     1 bit
    /// Conversion:    No Conversion needed
    /// Notes:         none
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 delimiter;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// blend_filter
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   This signal chooses the filter that is combined with directional filter to generate filter coefficients
    ///                for edge-directed interpolation
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1 bit
    /// Conversion:    No Conversion needed
    /// Notes:         0 (circular filter), 1 (separable filter)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 blend_filter;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// comp0_filter_method
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   This signal specifies the scaling method that is applied to Y component of YUV format
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 3]
    /// Bit Depth:     2 bits
    /// Conversion:    No Conversion needed
    /// Notes:         0x0: 2D 4x4; 0x1: 2D Circular; 0x2: 1D Separable; 0x3: bilinear
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 comp0_filter_method;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// comp1_2_filter_method
    ///
    /// Tuning Level:  RarelyTune
    /// Description:   This signal chooses the filter that is combined with directional filter to generate filter coefficients
    ///                for edge-directed interpolation for CbCr channel
    /// Type:          int
    /// Default Value: 0
    /// Range:         [0, 3]
    /// Bit Depth:     2 bits
    /// Conversion:    No Conversion needed
    /// Notes:         0x0: Same coefficients as Y; 0x1: 2D Circular; 0x2: 1D Separable; 0x3: bilinear
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 comp1_2_filter_method;
};

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIIQMODULEDEFINES_H

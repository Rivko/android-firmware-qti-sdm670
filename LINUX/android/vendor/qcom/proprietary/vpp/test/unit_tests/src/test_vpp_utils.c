/*!
 * @file test_vpp_utils.c
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Test utility routines in vpp
 */

#include <sys/types.h>
#include <stdlib.h>

#include <media/msm_media_info.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_VPPUTILS_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_VPPUTILS
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_utils.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

/************************************************************************
 * Local static variables
 ***********************************************************************/

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(VppUtilsSuiteInit)
{
}

TEST_SUITE_TERM(VppUtilsSuiteTerm)
{
}

TEST_SETUP(VppUtilsTestInit)
{
}

TEST_CLEANUP(VppUtilsTestTerm)
{
}

TEST(VppUtilsIsFmtUbwc)
{
    uint32_t u32, i;

    DVP_ASSERT_FALSE(u32VppUtils_IsFmtUbwc(VPP_COLOR_FORMAT_NV12_VENUS));
    DVP_ASSERT_FALSE(u32VppUtils_IsFmtUbwc(VPP_COLOR_FORMAT_NV21_VENUS));
    DVP_ASSERT_FALSE(u32VppUtils_IsFmtUbwc(VPP_COLOR_FORMAT_P010));

    DVP_ASSERT_TRUE(u32VppUtils_IsFmtUbwc(VPP_COLOR_FORMAT_UBWC_NV12));
    DVP_ASSERT_TRUE(u32VppUtils_IsFmtUbwc(VPP_COLOR_FORMAT_UBWC_NV21));
    DVP_ASSERT_TRUE(u32VppUtils_IsFmtUbwc(VPP_COLOR_FORMAT_UBWC_TP10));

    DVP_ASSERT_FALSE(u32VppUtils_IsFmtUbwc(VPP_COLOR_FORMAT_MAX));

    // Invalid
    for (i = 0; i < 100; i++)
    {
        u32 = rand() + VPP_COLOR_FORMAT_MAX;
        if (u32 > VPP_COLOR_FORMAT_MAX)
            DVP_ASSERT_FALSE(u32VppUtils_IsFmtUbwc(u32));
    }
}

TEST(VppUtilsGetNumPlanes)
{
    uint32_t u32, i;

    DVP_ASSERT_EQUAL(2, u32VppUtils_GetNumPlanes(VPP_COLOR_FORMAT_NV12_VENUS));
    DVP_ASSERT_EQUAL(2, u32VppUtils_GetNumPlanes(VPP_COLOR_FORMAT_NV21_VENUS));
    DVP_ASSERT_EQUAL(2, u32VppUtils_GetNumPlanes(VPP_COLOR_FORMAT_P010));
    DVP_ASSERT_EQUAL(4, u32VppUtils_GetNumPlanes(VPP_COLOR_FORMAT_UBWC_NV12));
    DVP_ASSERT_EQUAL(4, u32VppUtils_GetNumPlanes(VPP_COLOR_FORMAT_UBWC_NV21));
    DVP_ASSERT_EQUAL(4, u32VppUtils_GetNumPlanes(VPP_COLOR_FORMAT_UBWC_TP10));

    // Invalid
    for (i = 0; i < 100; i++)
    {
        u32 = rand() + VPP_COLOR_FORMAT_MAX;
        if (u32 > VPP_COLOR_FORMAT_MAX)
            DVP_ASSERT_EQUAL(0, u32VppUtils_GetNumPlanes(u32));
    }
}

enum {
    PLANE_Y,
    PLANE_UV,
    PLANE_Y_META,
    PLANE_UV_META,
};

static void vTestCalcStrideForPlane(uint32_t u32Plane, int msm_fmt,
                                    enum vpp_color_format eVppFmt)
{
    uint32_t i;
    uint32_t u32Str = 0;
    uint32_t u32VppStr = 0;
    uint32_t u32VppPlane = 0;

    uint32_t bIsUbwc = u32VppUtils_IsFmtUbwc(eVppFmt);

    for (i = 0; i < RES_FHD_MAX_W + 1; i++)
    {
        if (u32Plane == PLANE_Y)
        {
            u32VppPlane = !bIsUbwc ? 0 : 1;
            u32Str = VENUS_Y_STRIDE(msm_fmt, i);
        }
        else if (u32Plane == PLANE_UV)
        {
            u32VppPlane = !bIsUbwc ? 1 : 3;
            u32Str = VENUS_UV_STRIDE(msm_fmt, i);
        }
        else if (u32Plane == PLANE_Y_META)
        {
            u32VppPlane = 0;
            u32Str = VENUS_Y_META_STRIDE(msm_fmt, i);
        }
        else if (u32Plane == PLANE_UV_META)
        {
            u32VppPlane = 2;
            u32Str = VENUS_UV_META_STRIDE(msm_fmt, i);
        }

        u32VppStr = u32VppUtils_CalcStrideForPlane(i, eVppFmt, u32VppPlane);
        DVP_ASSERT_EQUAL(u32VppStr, u32Str);
        if (u32VppStr != u32Str)
        {
            printf("u32Plane=%u, msm_fmt=%u, vpp_fmt=%u, u32VppStr=%u, u32Str=%u, u32Wid=%u\n",
                   u32Plane, msm_fmt, eVppFmt, u32VppStr, u32Str, i);
            break;
        }
    }
}

TEST(VppUtilsCalcStrideForPlane)
{
    uint32_t wid, fmt, i;
    uint32_t bIsUbwc, u32Plane;

    vTestCalcStrideForPlane(PLANE_Y, COLOR_FMT_NV12, VPP_COLOR_FORMAT_NV12_VENUS);
    vTestCalcStrideForPlane(PLANE_UV, COLOR_FMT_NV12, VPP_COLOR_FORMAT_NV12_VENUS);

    vTestCalcStrideForPlane(PLANE_Y, COLOR_FMT_NV21, VPP_COLOR_FORMAT_NV21_VENUS);
    vTestCalcStrideForPlane(PLANE_UV, COLOR_FMT_NV21, VPP_COLOR_FORMAT_NV21_VENUS);

    // TODO: P010

    vTestCalcStrideForPlane(PLANE_Y, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV12);
    vTestCalcStrideForPlane(PLANE_UV, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV12);
    vTestCalcStrideForPlane(PLANE_Y_META, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV12);
    vTestCalcStrideForPlane(PLANE_UV_META, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV12);

    vTestCalcStrideForPlane(PLANE_Y, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV21);
    vTestCalcStrideForPlane(PLANE_UV, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV21);
    vTestCalcStrideForPlane(PLANE_Y_META, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV21);
    vTestCalcStrideForPlane(PLANE_UV_META, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV21);

    vTestCalcStrideForPlane(PLANE_Y, COLOR_FMT_NV12_BPP10_UBWC, VPP_COLOR_FORMAT_UBWC_TP10);
    vTestCalcStrideForPlane(PLANE_UV, COLOR_FMT_NV12_BPP10_UBWC, VPP_COLOR_FORMAT_UBWC_TP10);
    vTestCalcStrideForPlane(PLANE_Y_META, COLOR_FMT_NV12_BPP10_UBWC, VPP_COLOR_FORMAT_UBWC_TP10);
    vTestCalcStrideForPlane(PLANE_UV_META, COLOR_FMT_NV12_BPP10_UBWC, VPP_COLOR_FORMAT_UBWC_TP10);

    // invalid plane
    for (fmt = 0; fmt < VPP_COLOR_FORMAT_MAX; fmt++)
    {
        bIsUbwc = u32VppUtils_IsFmtUbwc(fmt);
        u32Plane = rand();

        if (!bIsUbwc)
            u32Plane = u32Plane > 2 ? u32Plane : u32Plane + 2;
        else
            u32Plane = u32Plane > 4 ? u32Plane : u32Plane + 4;

        for (wid = 0; wid < RES_FHD_MAX_W; wid++)
        {
            DVP_ASSERT_EQUAL(0, u32VppUtils_CalcStrideForPlane(wid, fmt, u32Plane));
        }
    }

    // invalid format
    for (i = 0; i < 10; i++)
    {
        fmt = rand() + VPP_COLOR_FORMAT_MAX;
        fmt = fmt > VPP_COLOR_FORMAT_MAX ? fmt : fmt + VPP_COLOR_FORMAT_MAX;
        for (wid = 0; wid < RES_FHD_MAX_W + 1; wid++)
        {
            DVP_ASSERT_EQUAL(0, u32VppUtils_CalcStrideForPlane(wid, fmt, 0));
            DVP_ASSERT_EQUAL(0, u32VppUtils_CalcStrideForPlane(wid, fmt, 1));
            DVP_ASSERT_EQUAL(0, u32VppUtils_CalcStrideForPlane(wid, fmt, 2));
            DVP_ASSERT_EQUAL(0, u32VppUtils_CalcStrideForPlane(wid, fmt, 3));
        }
    }
}

static void vTestCalcScanlinesForPlane(uint32_t u32Plane, int msm_fmt,
                                       enum vpp_color_format eVppFmt)
{
    uint32_t i;
    uint32_t u32Scl = 0;
    uint32_t u32VppScl = 0;
    uint32_t u32VppPlane = 0;

    uint32_t bIsUbwc = u32VppUtils_IsFmtUbwc(eVppFmt);

    for (i = 0; i < RES_FHD_MAX_H + 1; i++)
    {
        if (u32Plane == PLANE_Y)
        {
            u32VppPlane = !bIsUbwc ? 0 : 1;
            u32Scl = VENUS_Y_SCANLINES(msm_fmt, i);
        }
        else if (u32Plane == PLANE_UV)
        {
            u32VppPlane = !bIsUbwc ? 1 : 3;
            u32Scl = VENUS_UV_SCANLINES(msm_fmt, i);
        }
        else if (u32Plane == PLANE_Y_META)
        {
            u32VppPlane = 0;
            u32Scl = VENUS_Y_META_SCANLINES(msm_fmt, i);
        }
        else if (u32Plane == PLANE_UV_META)
        {
            u32VppPlane = 2;
            u32Scl = VENUS_UV_META_SCANLINES(msm_fmt, i);
        }

        u32VppScl = u32VppUtils_CalcScanlinesForPlane(i, eVppFmt, u32VppPlane);
        DVP_ASSERT_EQUAL(u32VppScl, u32Scl);
        if (u32VppScl != u32Scl)
        {
            printf("u32Plane=%u, msm_fmt=%u, vpp_fmt=%u, u32VppScl=%u, u32Scl=%u, u32Wid=%u\n",
                   u32Plane, msm_fmt, eVppFmt, u32VppScl, u32Scl, i);
            break;
        }
    }
}

TEST(VppUtilsCalcScanlinesForPlane)
{
    uint32_t height, fmt, i;
    uint32_t bIsUbwc, u32Plane;

    vTestCalcScanlinesForPlane(PLANE_Y, COLOR_FMT_NV12, VPP_COLOR_FORMAT_NV12_VENUS);
    vTestCalcScanlinesForPlane(PLANE_UV, COLOR_FMT_NV12, VPP_COLOR_FORMAT_NV12_VENUS);

    vTestCalcScanlinesForPlane(PLANE_Y, COLOR_FMT_NV21, VPP_COLOR_FORMAT_NV21_VENUS);
    vTestCalcScanlinesForPlane(PLANE_UV, COLOR_FMT_NV21, VPP_COLOR_FORMAT_NV21_VENUS);

    // TODO: P010

    vTestCalcScanlinesForPlane(PLANE_Y, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV12);
    vTestCalcScanlinesForPlane(PLANE_UV, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV12);
    vTestCalcScanlinesForPlane(PLANE_Y_META, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV12);
    vTestCalcScanlinesForPlane(PLANE_UV_META, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV12);

    vTestCalcScanlinesForPlane(PLANE_Y, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV21);
    vTestCalcScanlinesForPlane(PLANE_UV, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV21);
    vTestCalcScanlinesForPlane(PLANE_Y_META, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV21);
    vTestCalcScanlinesForPlane(PLANE_UV_META, COLOR_FMT_NV12_UBWC, VPP_COLOR_FORMAT_UBWC_NV21);

    vTestCalcScanlinesForPlane(PLANE_Y, COLOR_FMT_NV12_BPP10_UBWC, VPP_COLOR_FORMAT_UBWC_TP10);
    vTestCalcScanlinesForPlane(PLANE_UV, COLOR_FMT_NV12_BPP10_UBWC, VPP_COLOR_FORMAT_UBWC_TP10);
    vTestCalcScanlinesForPlane(PLANE_Y_META, COLOR_FMT_NV12_BPP10_UBWC, VPP_COLOR_FORMAT_UBWC_TP10);
    vTestCalcScanlinesForPlane(PLANE_UV_META, COLOR_FMT_NV12_BPP10_UBWC, VPP_COLOR_FORMAT_UBWC_TP10);

    // invalid plane
    for (fmt = 0; fmt < VPP_COLOR_FORMAT_MAX; fmt++)
    {
        bIsUbwc = u32VppUtils_IsFmtUbwc(fmt);
        u32Plane = rand();

        if (!bIsUbwc)
            u32Plane = u32Plane > 2 ? u32Plane : u32Plane + 2;
        else
            u32Plane = u32Plane > 4 ? u32Plane : u32Plane + 4;

        for (height = 0; height < RES_FHD_MAX_H; height++)
        {
            DVP_ASSERT_EQUAL(0, u32VppUtils_CalcScanlinesForPlane(height, fmt, u32Plane));
        }
    }

    // invalid format
    for (i = 0; i < 10; i++)
    {
        fmt = rand() + VPP_COLOR_FORMAT_MAX;
        fmt = fmt > VPP_COLOR_FORMAT_MAX ? fmt : fmt + VPP_COLOR_FORMAT_MAX;
        for (height = 0; height < RES_FHD_MAX_H + 1; height++)
        {
            DVP_ASSERT_EQUAL(0, u32VppUtils_CalcScanlinesForPlane(height, fmt, 0));
            DVP_ASSERT_EQUAL(0, u32VppUtils_CalcScanlinesForPlane(height, fmt, 1));
            DVP_ASSERT_EQUAL(0, u32VppUtils_CalcScanlinesForPlane(height, fmt, 2));
            DVP_ASSERT_EQUAL(0, u32VppUtils_CalcScanlinesForPlane(height, fmt, 3));
        }
    }
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES VppUtilsTests[] = {
    TEST_CASE(VppUtilsIsFmtUbwc),
    TEST_CASE(VppUtilsGetNumPlanes),
    TEST_CASE(VppUtilsCalcStrideForPlane),
    TEST_CASE(VppUtilsCalcScanlinesForPlane),
    TEST_CASE_NULL(),
};

TEST_SUITE(VppUtilsSuite,
           "VppUtils Tests",
           VppUtilsSuiteInit,
           VppUtilsSuiteTerm,
           VppUtilsTestInit,
           VppUtilsTestTerm,
           VppUtilsTests);

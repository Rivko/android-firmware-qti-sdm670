/*!
 * @file test_uc.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_UC_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_UC
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_core.h"
#include "vpp_ctx.h"
#include "vpp_reg.h"
#include "vpp_uc.h"
#include "test_mock_registry.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
/************************************************************************
 * Local static variables
 ***********************************************************************/


/************************************************************************
 * Forward Declarations
 ************************************************************************/
extern t_StVppUsecase *pstVppUsecase_Find(t_StVppHqvCtrl *pstCtrl, t_StVppParam *pstParam);
extern t_StVppUsecase uc_mock_uctest_hvx, uc_mock_uctest_fb_hvx, uc_mock_uctest_gpu,
    uc_mock_uctest_multi, uc_mock_uctest_singleip_multicredit, uc_mock_uctest_multiip,
    uc_mock_uctest_gpu_single_sess, uc_mock_hvx_auto, uc_mock_uctest_two_ip,
    uc_mock_uctest_three_ip, uc_mock_uctest_hvx_maxfps;
/************************************************************************
 * Local Functions
 ***********************************************************************/
static void print_uc(t_StVppUsecase *pstUc)
{
    if (pstUc == NULL)
    {
        LOGD("Unable to find usecase\n");
    }
    else
    {
        LOGD("Found usecase: %s\n", pstUc->name);
    }
}

void set_deinterlacer(t_StVppHqvCtrl *pstCtrl, uint32_t u32En)
{
    if (u32En)
    {
        pstCtrl->mode = HQV_MODE_MANUAL;
        pstCtrl->di.mode = HQV_DI_MODE_VIDEO_3F;
    }
    else
    {
        pstCtrl->di.mode = HQV_DI_MODE_OFF;
    }
}

void set_tnr(t_StVppHqvCtrl *pstCtrl, uint32_t u32En)
{
    if (u32En)
    {
        pstCtrl->mode = HQV_MODE_MANUAL;
        pstCtrl->tnr.mode = HQV_MODE_AUTO;
    }
    else
    {
        pstCtrl->tnr.mode = HQV_MODE_OFF;
    }
}

void set_aie(t_StVppHqvCtrl *pstCtrl, uint32_t u32En)
{
    if (u32En)
    {
        pstCtrl->mode = HQV_MODE_MANUAL;
        pstCtrl->aie.mode = HQV_MODE_AUTO;
    }
    else
    {
        pstCtrl->aie.mode = HQV_MODE_OFF;
    }
}

void set_cnr(t_StVppHqvCtrl *pstCtrl, uint32_t u32En)
{
    if (u32En)
    {
        pstCtrl->mode = HQV_MODE_MANUAL;
        pstCtrl->cnr.mode = HQV_MODE_AUTO;
    }
    else
    {
        pstCtrl->cnr.mode = HQV_MODE_OFF;
    }
}

void set_frc(t_StVppHqvCtrl *pstCtrl, uint32_t u32En)
{
    if (u32En)
    {
        pstCtrl->mode = HQV_MODE_MANUAL;
        pstCtrl->frc.mode = HQV_FRC_MODE_SMOOTH_MOTION;
        pstCtrl->frc.level = HQV_FRC_LEVEL_HIGH;
    }
    else
    {
        pstCtrl->frc.mode = HQV_FRC_MODE_OFF;
    }
}

uint32_t usecase_test_init(t_StVppHqvCtrl **stppCtrl1, t_StVppHqvCtrl **stppCtrl2,
                           t_StVppParam **ppstVppParam1, t_StVppParam **ppstVppParam2)
{
    *stppCtrl1 = calloc(sizeof(t_StVppHqvCtrl), 1);
    *stppCtrl2 = calloc(sizeof(t_StVppHqvCtrl), 1);
    *ppstVppParam1 = calloc(sizeof(t_StVppParam), 1);
    *ppstVppParam2 = calloc(sizeof(t_StVppParam), 1);

    if(!(*stppCtrl1) || !(*stppCtrl2) || !(*ppstVppParam1) || !(*ppstVppParam2))
    {
        if (*stppCtrl1) free(*stppCtrl1);
        if (*stppCtrl2) free(*stppCtrl2);
        if (*ppstVppParam1) free(*ppstVppParam1);
        if (*ppstVppParam2) free(*ppstVppParam2);

        *stppCtrl1 = NULL;
        *stppCtrl2 = NULL;
        *ppstVppParam1 = NULL;
        *ppstVppParam2 = NULL;

        LOGE("Use case memory allocation error!");
        return VPP_ERR;
    }

    (*ppstVppParam1)->input_port.width = 1920;
    (*ppstVppParam1)->input_port.height = 1080;
    (*ppstVppParam1)->input_port.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    (*ppstVppParam1)->output_port.width = 1920;
    (*ppstVppParam1)->output_port.height = 1080;
    (*ppstVppParam1)->output_port.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    (*ppstVppParam2)->input_port.width = 1920;
    (*ppstVppParam2)->input_port.height = 1080;
    (*ppstVppParam2)->input_port.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    (*ppstVppParam2)->output_port.width = 1920;
    (*ppstVppParam2)->output_port.height = 1080;
    (*ppstVppParam2)->output_port.fmt = VPP_COLOR_FORMAT_NV12_VENUS;

    set_cnr(*stppCtrl1, VPP_TRUE);
    set_aie(*stppCtrl1, VPP_TRUE);

    set_frc(*stppCtrl2, VPP_TRUE);

    return VPP_OK;
}

void usecase_test_deinit(t_StVppHqvCtrl **stppCtrl1, t_StVppHqvCtrl **stppCtrl2,
                       t_StVppParam **ppstVppParam1, t_StVppParam **ppstVppParam2)
{
    free(*stppCtrl1);
    free(*stppCtrl2);
    free(*ppstVppParam1);
    free(*ppstVppParam2);
}

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(UsecaseSuiteInit)
{
    init_test_registry(TEST_SUITE_USECASE);
}

TEST_SUITE_TERM(UsecaseSuiteTerm)
{
    term_test_registry();
}

TEST_SETUP(UsecaseTestInit)
{
}

TEST_CLEANUP(UsecaseTestTerm)
{
}

TEST(__unused Usecase_Find)
{
    t_StVppHqvCtrl stCtrl;
    t_StVppUsecase *pstUc = NULL;

    memset(&stCtrl, 0, sizeof(stCtrl));

    // DI
    set_deinterlacer(&stCtrl, VPP_TRUE);
    pstUc = pstVppUsecase_Find(&stCtrl, NULL);
    print_uc(pstUc);
    DVP_ASSERT_PTR_NNULL(pstUc);

    // DI, TNR
    set_tnr(&stCtrl, VPP_TRUE);
    pstUc = pstVppUsecase_Find(&stCtrl, NULL);
    print_uc(pstUc);
    DVP_ASSERT_PTR_NNULL(pstUc);

    // DI, TNR, AIE
    set_aie(&stCtrl, VPP_TRUE);
    pstUc = pstVppUsecase_Find(&stCtrl, NULL);
    print_uc(pstUc);
    DVP_ASSERT_PTR_NNULL(pstUc);

    // DI, TNR
    set_aie(&stCtrl, VPP_FALSE);
    pstUc = pstVppUsecase_Find(&stCtrl, NULL);
    print_uc(pstUc);
    DVP_ASSERT_PTR_NNULL(pstUc);

    // DI, TNR, CNR (NOT SUPPORTED!)
    set_cnr(&stCtrl, VPP_TRUE);
    pstUc = pstVppUsecase_Find(&stCtrl, NULL);
    print_uc(pstUc);
    DVP_ASSERT_PTR_NULL(pstUc);

    // AUTO
    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_AUTO;
    pstUc = pstVppUsecase_Find(&stCtrl, NULL);
    print_uc(pstUc);
    DVP_ASSERT_PTR_NNULL(pstUc);

}

TEST(Usecase_OpenClose)
{
    t_StVppCtx *pstCtx1, *pstCtx2, *pstCtx3, *pstCtx4, *pstCtx5, *pstCtx6, *pstCtx7;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t u32Ret;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    t_StVppParam *pstVppParam1, *pstVppParam2;
    t_StVppUsecase *pstUc1 = NULL;

    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);

    pstCtx1 = vpp_init(0, cb);
    pstCtx2 = vpp_init(0, cb);
    pstCtx3 = vpp_init(0, cb);
    pstCtx4 = vpp_init(0, cb);
    pstCtx5 = vpp_init(0, cb);
    pstCtx6 = vpp_init(0, cb);
    pstCtx7 = vpp_init(0, cb);
    DVP_ASSERT_PTR_NNULL(pstCtx1);
    DVP_ASSERT_PTR_NNULL(pstCtx2);
    DVP_ASSERT_PTR_NNULL(pstCtx3);
    DVP_ASSERT_PTR_NNULL(pstCtx4);
    DVP_ASSERT_PTR_NNULL(pstCtx5);
    DVP_ASSERT_PTR_NNULL(pstCtx6);
    DVP_ASSERT_PTR_NNULL(pstCtx7);

    //Try init every context again, should return error
    u32Ret = u32VppUsecase_Init(pstCtx1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Init(pstCtx2);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Init(pstCtx3);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Init(pstCtx4);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Init(pstCtx5);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Init(pstCtx6);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Init(pstCtx7);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    vpp_term(pstCtx1);
    vpp_term(pstCtx2);
    vpp_term(pstCtx7);
    vpp_term(pstCtx4);

    pstUc1 = &uc_mock_uctest_hvx;
    print_uc(pstUc1);

    // Usecase 1 1920x1080: Credits {15,15,15,30}, supports dual
    u32Ret = u32VppUsecase_Configure(pstCtx3, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Second session should be ok
    u32Ret = u32VppUsecase_Configure(pstCtx6, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Third session should be rejected
    u32Ret = u32VppUsecase_Configure(pstCtx5, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Close a running session
    vpp_term(pstCtx3);

    // New session should be ok after close
    u32Ret = u32VppUsecase_Configure(pstCtx5, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    vpp_term(pstCtx5);
    vpp_term(pstCtx6);

}

TEST(Usecase_Reconfigure)
{
    t_StVppCtx *pstCtx1, *pstCtx2;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t u32Ret;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    t_StVppParam *pstVppParam1, *pstVppParam2;
    t_StVppUsecase *pstUc1 = NULL;
    t_StVppUsecase *pstUc2 = NULL;
    t_StVppUsecase *pstUcFallBack = NULL;

    pstCtx1 = vpp_init(0,cb);
    pstCtx2 = vpp_init(0,cb);
    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    if (u32Ret != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstUc1 = &uc_mock_uctest_fb_hvx;
    pstUc2 = &uc_mock_uctest_gpu;
    // Usecase 1 1920x1080: CNR, AIE: Credits {5,10,15,NOT SUPPORTED}, AIE; supports dual
    // Usecase 2 1920x1080: GPU AIE: Credits {15,15,15,NOT SUPPORTED}, NULL; supports dual
    // Validate 1920x1080 CNR,AIE
    print_uc(pstUc1);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_PTR_NULL(pstUcFallBack);

    // Validate 4k CNR,AIE, should fail with a fallback usecase
    pstVppParam1->input_port.width = 3840;
    pstVppParam1->input_port.height = 2160;
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_PTR_NNULL(pstUcFallBack);

    pstVppParam1->input_port.width = 1920;
    pstVppParam1->input_port.height = 1080;

    // Configure 1920x1080 CNR, AIE as first context
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure 1920x1080 CNR, AIE as second context
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Reconfigure first context for 480p
    pstVppParam1->input_port.width = 720;
    pstVppParam1->input_port.height = 480;
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Reconfigure second context for 720p
    pstVppParam2->input_port.width = 1280;
    pstVppParam2->input_port.height = 720;
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Reconfigure second context for 1920x1080
    pstVppParam2->input_port.width = 1920;
    pstVppParam2->input_port.height = 1080;
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Reconfigure first context for 1920x1080
    pstVppParam1->input_port.width = 1920;
    pstVppParam1->input_port.height = 1080;
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Close the first context
    vVppUsecase_Term(pstCtx1);
    // Open the first context again
    u32VppUsecase_Init(pstCtx1);

    // Open 1920x1080 GPU as first context
    print_uc(pstUc2);
    pstVppParam2->input_port.width = 1920;
    pstVppParam2->input_port.height = 1080;
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl2, pstVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Reconfigure first context for 4k, should return error (unsupported)
    pstVppParam2->input_port.width = 3840;
    pstVppParam2->input_port.height = 2160;
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl2, pstVppParam2);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    vpp_term(pstCtx1);
    vpp_term(pstCtx2);
}

TEST(Usecase_Framerate)
{
    t_StVppCtx *pstCtx1, *pstCtx2;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t u32Ret;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    t_StVppParam *pstVppParam1, *pstVppParam2;
    t_StVppUsecase *pstUc1 = NULL;

    pstCtx1 = vpp_init(0,cb);
    pstCtx2 = vpp_init(0,cb);
    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    if (u32Ret != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }
    pstUc1 = &uc_mock_uctest_hvx;
    print_uc(pstUc1);

    // Usecase 1 1920x1080: Credits {15,15,15,30}, supports dual
    // Configure 1920x1080 as first context
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure 1920x1080 as second context
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Decrease second context FPS to 20
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx2, pstUc1, 20, eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Increase second context FPS to 60, should return error (not enough credits)
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx2, pstUc1, 60, eVppBufType_Progressive);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Decrease second context FPS to 30
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx2, pstUc1, 30, eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Close the first context
    vVppUsecase_Term(pstCtx1);

    // Increase second context FPS to 60
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx2, pstUc1, 60, eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure 1920x1080 as first context, should return error (not enough credits)
    u32Ret = u32VppUsecase_Init(pstCtx1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Decrease second context FPS to 30
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx2, pstUc1, 30, eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure 1920x1080 as first context
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Close both contexts
    vVppUsecase_Term(pstCtx1);
    vVppUsecase_Term(pstCtx2);

    u32Ret = u32VppUsecase_Init(pstCtx1);
    pstUc1 = &uc_mock_uctest_hvx_maxfps;
    print_uc(pstUc1);

    // Usecase 1 1920x1080: Credits {15,15,15,30}
    // VPP_UC_MAX_IN_FPS_P(60, 50, 40, 30)
    // VPP_UC_MAX_IN_FPS_I(0, 25, 20, 0)
    // Configure 1920x1080 as first context

    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Decrease FPS to 20, progressive
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx1, pstUc1, 20, eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Increase FPS to 45, progressive, should return error (max 40 for FHD progressive)
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx1, pstUc1, 45, eVppBufType_Progressive);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Decrease FPS to 30, progressive
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx1, pstUc1, 30, eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Change buf type to interlaced, should return error (max 20 for FHD interlaced)
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx1, pstUc1, 30, eVppBufType_Interleaved_BFF);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Decrease context FPS to 15, interlaced
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx1, pstUc1, 15, eVppBufType_Interleaved_BFF);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Increase FPS to 45, should return error (max 20 for FHD interlaced)
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx1, pstUc1, 45, eVppBufType_Interleaved_BFF);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure 720x480 as first context
    pstVppParam1->input_port.width = 720;
    pstVppParam1->input_port.height = 480;
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // FPS to 62, interlaced, should be ok since max FPS undefined and credits support up to 66
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx1, pstUc1, 62, eVppBufType_Interleaved_BFF);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Change buf type to progressive, should return error (max 60 for SD progressive)
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx1, pstUc1, 62, eVppBufType_Progressive);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    vVppUsecase_Term(pstCtx1);
    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    vpp_term(pstCtx1);
    vpp_term(pstCtx2);
}


TEST(Usecase_Credits)
{
    t_StVppCtx *pstCtx1, *pstCtx2, *pstCtx3;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t u32Ret;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    t_StVppParam *pstVppParam1, *pstVppParam2;
    t_StVppUsecase *pstUc1 = NULL;
    t_StVppUsecase *pstUc2 = NULL;
    t_StVppUsecase *pstUc3 = NULL;

    pstCtx1 = vpp_init(0, cb);
    pstCtx2 = vpp_init(0, cb);
    pstCtx3 = vpp_init(0, cb);
    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    if (u32Ret != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstUc1 = &uc_mock_uctest_hvx;
    pstUc2 = &uc_mock_uctest_gpu;
    pstUc3 = &uc_mock_uctest_singleip_multicredit;

    // Usecase 1 1920x1080: Credits HVX{15,15,15,30}, supports dual
    // Usecase 2 1920x1080: Credits GPU{15,20,25,NOT SUPPORTED}, supports dual
    // Usecase 3 1920x1080 Multi: Credits HVX{15,15,15,30}, GPU{6,12,18,24}, supports dual
    // Configure HVX as first context
    print_uc(pstUc1);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure 3840x2160 HVX as second context, should return error (not enough credits)
    pstVppParam2->input_port.width = 3840;
    pstVppParam2->input_port.height = 2160;
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure 1280x720 HVX as second context
    pstVppParam2->input_port.width = 1280;
    pstVppParam2->input_port.height = 720;
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Reconfigure second context for 3840x2160, should return error (not enough credits)
    pstVppParam2->input_port.width = 3840;
    pstVppParam2->input_port.height = 2160;
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure 1920x1080 multi as the second context, supports dual
    print_uc(pstUc3);
    pstVppParam2->input_port.width = 1920;
    pstVppParam2->input_port.height = 1080;
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc3, stpCtrl1, pstVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Reconfigure first context for 3840x2160, should return error (not enough HVX credits)
    print_uc(pstUc1);
    pstVppParam1->input_port.width = 3840;
    pstVppParam1->input_port.height = 2160;
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure first context for 720x480 GPU
    print_uc(pstUc2);
    pstVppParam1->input_port.width = 720;
    pstVppParam1->input_port.height = 480;
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Reconfigure second context for 3840x2160, should return error (not enough GPU credits)
    print_uc(pstUc3);
    pstVppParam2->input_port.width = 3840;
    pstVppParam2->input_port.height = 2160;
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc3, stpCtrl1, pstVppParam2);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Close the first context
    vVppUsecase_Term(pstCtx1);

    // Reconfigure second context for 1920x1080 HVX
    print_uc(pstUc1);
    pstVppParam2->input_port.width = 1920;
    pstVppParam2->input_port.height = 1080;
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, pstVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Decrease second context FPS to 20
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx2, pstUc1, 20, eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Increase second context FPS to 80, should return error (not enough credits)
    u32Ret = u32VppUsecase_UpdateFramerate(pstCtx2, pstUc1, 80, eVppBufType_Progressive);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    vpp_term(pstCtx1);
    vpp_term(pstCtx2);
    vpp_term(pstCtx3);
}


TEST(Usecase_Sessions)
{
    t_StVppCtx *pstCtx1, *pstCtx2, *pstCtx3;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t u32Ret;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    t_StVppParam *pstVppParam1, *pstVppParam2;
    t_StVppUsecase *pstUc1 = NULL;
    t_StVppUsecase *pstUc2 = NULL;

    pstCtx1 = vpp_init(0,cb);
    pstCtx2 = vpp_init(0,cb);
    pstCtx3 = vpp_init(0,cb);
    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    if (u32Ret != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstUc1 = &uc_mock_uctest_fb_hvx;
    pstUc2 = &uc_mock_uctest_gpu_single_sess;
    print_uc(pstUc1);

    // Usecase 1: Credits HVX{5,10,15,NOT SUPPORTED}, supports dual
    // Configure 720x480 as first context
    pstVppParam1->input_port.width = 720;
    pstVppParam1->input_port.height = 480;
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure 720x480 as second context
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure 720x480 as third context, should reject, only supports dual
    u32Ret = u32VppUsecase_Configure(pstCtx3, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Close the first context
    vVppUsecase_Term(pstCtx1);

    // Configure 720x480 as context 3, should be supported now
    u32Ret = u32VppUsecase_Configure(pstCtx3, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Usecase 2: Credits GPU{10,10,10,NOT SUPPORTED}, supports single
    // Configure 720x480 as first context
    print_uc(pstUc2);
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure 720x480 as second context, should reject, only supports single
    u32Ret = u32VppUsecase_Configure(pstCtx3, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    vpp_term(pstCtx1);
    vpp_term(pstCtx2);
    vpp_term(pstCtx3);
}

TEST(Usecase_CpQuirks)
{
    t_StVppCtx *pstCtx1, *pstCtx2;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t u32Ret;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    t_StVppParam *stpVppParam1, *stpVppParam2;
    t_StVppUsecase *pstUc1 = NULL;
    t_StVppUsecase *pstUc2 = NULL;

    pstCtx1 = vpp_init(0, cb);
    pstCtx2 = vpp_init(0, cb);
    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &stpVppParam1, &stpVppParam2);
    if (u32Ret != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstUc1 = &uc_mock_uctest_hvx;
    pstUc2 = &uc_mock_uctest_gpu;

    // Usecase 1 1920x1080: Credits {15,15,15,30}, supports CP
    // Usecase 2 1920x1080: Credits {15,20,25,NOT SUPPORTED}, no CP
    // Configure 1920x1080 as first context
    stpVppParam2->u32SessionFlags = VPP_SESSION_SECURE;
    // Configure 1920x1080 secure GPU as first context, should return error, does not support CP
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl2, stpVppParam2);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure non CP 1920x1080 HVX as first context
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, stpVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure CP 1920x1080 HVX as second context, should return error (non CP with CP)
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, stpVppParam2);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure non CP 1920x1080 HVX as second context
    stpVppParam2->u32SessionFlags = 0;
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, stpVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Reconfigure first context for secure, should return error (non CP with CP)
    stpVppParam1->u32SessionFlags = VPP_SESSION_SECURE;
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, stpVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Close the first context
    vVppUsecase_Term(pstCtx1);

    // Close the second context
    vVppUsecase_Term(pstCtx2);

    // Open CP 1920x1080 HVX as first context
    u32Ret = u32VppUsecase_Init(pstCtx1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, stpVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Open non CP 1920x1080 HVX as second context, should return error (non CP with CP)
    stpVppParam2->u32SessionFlags = 0;
    print_uc(pstUc1);
    u32Ret = u32VppUsecase_Init(pstCtx2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, stpVppParam2);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Open CP 1920x1080 HVX as second context
    stpVppParam2->u32SessionFlags = VPP_SESSION_SECURE;
    print_uc(pstUc1);
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, stpVppParam2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Reconfigure second context for non secure, should return error (non CP with CP)
    stpVppParam2->u32SessionFlags = 0;
    u32Ret = u32VppUsecase_Configure(pstCtx2, pstUc1, stpCtrl1, stpVppParam2);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &stpVppParam1, &stpVppParam2);
    vpp_term(pstCtx1);
    vpp_term(pstCtx2);
}

static uint64_t u64GetUcExpectedColorMask(t_StVppUsecase *pstUc)
{
    uint64_t u64Mask = 0;
    uint32_t i;
    enum vpp_color_format eColorFmt;

    if (!pstUc)
        return 0;

    for (i = 0; i < pstUc->u32PortCfgCnt; i++)
    {
        eColorFmt = pstUc->portCfg[i].internalReqs[0].eInColorFmt;
        u64Mask |= (0x1 << eColorFmt);
    }
    return u64Mask;
}

TEST(Usecase_GetRequirements)
{
    t_StVppCtx *pstCtx1, *pstCtx2;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t i, u32Ret;
    uint64_t u64ExpectedMask;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    t_StVppParam *pstVppParam1, *pstVppParam2;
    t_StVppUsecase *pstUc1 = NULL;
    t_StVppUsecase *pstUc2 = NULL;
    t_StVppUsecase *pstUc3 = NULL;
    t_StVppUsecase *pstUc4 = NULL;
    struct vpp_requirements *vpp_req;

    pstCtx1 = vpp_init(0, cb);
    pstCtx2 = vpp_init(0, cb);
    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    if (u32Ret != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    vpp_req = calloc(sizeof(struct vpp_requirements), 1);
    if(!vpp_req)
    {
        LOGE("Use case memory allocation error!");
        DVP_ASSERT_FAIL();
        return;
    }

    pstUc1 = &uc_mock_uctest_fb_hvx;
    pstUc2 = &uc_mock_uctest_hvx;
    pstUc3 = &uc_mock_hvx_auto;
    pstUc4 = &uc_mock_uctest_three_ip;

    // UC1 should return metadata count = 1, NV12
    print_uc(pstUc1);
    u32Ret = u32VppUsecase_GetRequirements(pstUc1, vpp_req);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(vpp_req->metadata.cnt, 1);
    u64ExpectedMask = u64GetUcExpectedColorMask(pstUc1);
    DVP_ASSERT_EQUAL(vpp_req->in_color_fmt_mask, u64ExpectedMask);

    // UC2 should return metadata count = 0, NV12
    print_uc(pstUc2);
    u32Ret = u32VppUsecase_GetRequirements(pstUc2, vpp_req);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(vpp_req->metadata.cnt, 0);
    u64ExpectedMask = u64GetUcExpectedColorMask(pstUc2);
    DVP_ASSERT_EQUAL(vpp_req->in_color_fmt_mask, u64ExpectedMask);

    // UC2 should return buf requirements (0,1,0,1)
    print_uc(pstUc2);
    u32Ret = u32VppUsecase_GetRequirements(pstUc2, vpp_req);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    for (i = 0; i < VPP_RESOLUTION_MAX; i++)
    {
        DVP_ASSERT_EQUAL(vpp_req->in_factor[i].add, pstUc2->in_factor[i].add);
        DVP_ASSERT_EQUAL(vpp_req->in_factor[i].mul, pstUc2->in_factor[i].mul);
        DVP_ASSERT_EQUAL(vpp_req->out_factor[i].add, pstUc2->out_factor[i].add);
        DVP_ASSERT_EQUAL(vpp_req->out_factor[i].mul, pstUc2->out_factor[i].mul);
    }

    // UC3 should return NV12, buf requirements (2,1,0,2) except UHD (0,1,0,1)
    u32Ret = u32VppUsecase_GetRequirements(pstUc3, vpp_req);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u64ExpectedMask = u64GetUcExpectedColorMask(pstUc3);
    DVP_ASSERT_EQUAL(vpp_req->in_color_fmt_mask, u64ExpectedMask);
    for (i = 0; i < VPP_RESOLUTION_MAX; i++)
    {
        DVP_ASSERT_EQUAL(vpp_req->in_factor[i].add, pstUc3->in_factor[i].add);
        DVP_ASSERT_EQUAL(vpp_req->in_factor[i].mul, pstUc3->in_factor[i].mul);
        DVP_ASSERT_EQUAL(vpp_req->out_factor[i].add, pstUc3->out_factor[i].add);
        DVP_ASSERT_EQUAL(vpp_req->out_factor[i].mul, pstUc3->out_factor[i].mul);
    }

    // UC4 should return NV12, NV21, UBWC_TP10
    u32Ret = u32VppUsecase_GetRequirements(pstUc4, vpp_req);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u64ExpectedMask = u64GetUcExpectedColorMask(pstUc4);
    DVP_ASSERT_EQUAL(vpp_req->in_color_fmt_mask, u64ExpectedMask);

    free(vpp_req);
    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    vpp_term(pstCtx1);
    vpp_term(pstCtx2);
}

TEST(Usecase_CtrlSupported)
{
    t_StVppCtx *pstCtx1, *pstCtx2;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t u32Ret;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    struct hqv_control stHqvCtrl;
    t_StVppParam *pstVppParam1, *pstVppParam2;
    t_StVppUsecase *pstUc1 = NULL;
    t_StVppUsecase *pstUc2 = NULL;

    pstCtx1 = vpp_init(0, cb);
    pstCtx2 = vpp_init(0, cb);
    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    if (u32Ret != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstUc1 = &uc_mock_uctest_fb_hvx;
    pstUc2 = &uc_mock_uctest_gpu;

    // Usecase 1 1920x1080: HVX Credits {5, 10, 15, NOT_SUPPORTED}, CNR + AIE
    // Usecase 2 1920x1080: GPU Credits {15,15,15,30}, AIE

    // UC1, any non CNR or AIE control should be rejected
    print_uc(pstUc1);
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_TNR;
    stHqvCtrl.tnr.mode = HQV_MODE_MANUAL;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc1, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // UC1, any non CNR or AIE control should be rejected
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_DI;
    stHqvCtrl.di.mode = HQV_DI_MODE_OFF;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc1, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // UC1, any non CNR or AIE control should be rejected
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_CADE;
    stHqvCtrl.cade.mode = HQV_MODE_AUTO;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc1, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // UC1, adjusting CNR should be allowed
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_CNR;
    stHqvCtrl.cnr.mode = HQV_MODE_MANUAL;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc1, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    // UC1, adjusting CNR should be allowed
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_CNR;
    stHqvCtrl.cnr.mode = HQV_MODE_AUTO;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc1, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    // UC1, cannot disable a running algorithm, should be rejected
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_CNR;
    stHqvCtrl.cnr.mode = HQV_MODE_OFF;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc1, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // UC1, adjusting AIE should be allowed
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_AIE;
    stHqvCtrl.aie.mode = HQV_MODE_AUTO;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc1, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    // UC1, cannot disable a running algorithm, should be rejected
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_AIE;
    stHqvCtrl.aie.mode = HQV_MODE_OFF;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc1, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // UC2, any non AIE control should be rejected
    print_uc(pstUc2);
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_FRC;
    stHqvCtrl.aie.mode = HQV_MODE_OFF;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc2, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // UC2, any non AIE control should be rejected
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_CNR;
    stHqvCtrl.cnr.mode = HQV_MODE_AUTO;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc2, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // UC2, any non AIE control should be rejected
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_TNR;
    stHqvCtrl.tnr.mode = HQV_MODE_MANUAL;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc2, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // UC2, adjusting AIE should be allowed
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_AIE;
    stHqvCtrl.aie.mode = HQV_MODE_MANUAL;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc2, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

     // UC2, cannot disable a running algorithm, should be rejected
    memset(&stHqvCtrl, 0, sizeof(stHqvCtrl));
    stHqvCtrl.mode = HQV_MODE_MANUAL;
    stHqvCtrl.ctrl_type = HQV_CONTROL_AIE;
    stHqvCtrl.aie.mode = HQV_MODE_OFF;
    u32Ret = u32VppUsecase_IsCtrlSupported(pstUc2, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    vpp_term(pstCtx1);
    vpp_term(pstCtx2);
}

TEST(Usecase_HelperFuncs)
{
    t_StVppCtx *pstCtx1, *pstCtx2;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t u32Ret;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    t_StVppParam *pstVppParam1, *pstVppParam2;
    t_StVppUsecase *pstUc1 = NULL;
    t_StVppUsecase *pstUc2 = NULL;
    t_StVppUsecase *pstUc3 = NULL;

    pstCtx1 = vpp_init(0, cb);
    pstCtx2 = vpp_init(0, cb);
    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    if (u32Ret != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstUc1 = &uc_mock_uctest_fb_hvx;
    pstUc2 = &uc_mock_uctest_multiip;
    pstUc3 = &uc_mock_uctest_singleip_multicredit;

    // Usecase 1 AIE+CNR; HVX Only
    // Usecase 2 FRC; HVX and GPU;
    // Usecase 3 TNR; HVX Only

    // Should return true since HVX supports CP;
//    u32Ret = u32VppIsCpSupported();
//    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    u32Ret = u32VppUsecase_SubmodCount(NULL);
    DVP_ASSERT_EQUAL(u32Ret, 0);

    // CNR, AIE uses HVX only
    u32Ret = u32VppUsecase_SubmodCount(pstUc1);
    DVP_ASSERT_EQUAL(u32Ret, 1);

    // FRC uses HVX and GPU
    u32Ret = u32VppUsecase_SubmodCount(pstUc2);
    DVP_ASSERT_EQUAL(u32Ret, 2);

    u32Ret = u32VppUsecase_CompositionEqual(NULL, pstUc1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    u32Ret = u32VppUsecase_CompositionEqual(pstUc2, NULL);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    u32Ret = u32VppUsecase_CompositionEqual(pstUc1, pstUc2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    u32Ret = u32VppUsecase_CompositionEqual(pstUc1, pstUc1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    u32Ret = u32VppUsecase_CompositionEqual(pstUc2, pstUc2);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    u32Ret = u32VppUsecase_CompositionEqual(pstUc1, pstUc3);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    vpp_term(pstCtx1);
    vpp_term(pstCtx2);
}

TEST(Usecase_Resolution)
{
    t_StVppCtx *pstCtx1;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t u32Ret;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    t_StVppParam *pstVppParam1, *pstVppParam2;
    t_StVppUsecase *pstUc1 = NULL;
    t_StVppUsecase *pstUc2 = NULL;
    t_StVppUsecase *pstUcFallBack = NULL;

    pstCtx1 = vpp_init(0, cb);
    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    if (u32Ret != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstUc1 = &uc_mock_uctest_hvx;
    pstUc2 = &uc_mock_uctest_gpu;

    // Usecase 1 Max resolution 4096x2160, Min resolution 128x96
    // Usecase 2 Max resolution 1920x1088, Min resolution 96x64

    print_uc(pstUc1);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc2, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure 4096x2160, should be accepted for UC1, rejected for UC2
    pstVppParam1->input_port.width = 4096;
    pstVppParam1->input_port.height = 2160;
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc2, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure 4097x2160, should be rejected by both
    pstVppParam1->input_port.width = 4097;
    pstVppParam1->input_port.height = 2160;
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc2, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure 4096x2161, should be rejected by both
    pstVppParam1->input_port.width = 4096;
    pstVppParam1->input_port.height = 2161;
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc2, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure 5000x3000, should be rejected by both
    pstVppParam1->input_port.width = 5000;
    pstVppParam1->input_port.height = 3000;
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc2, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure 96x64, should be rejected for UC1, accepted for UC2
    pstVppParam1->input_port.width = 96;
    pstVppParam1->input_port.height = 64;
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc2, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Configure 95x64, should be rejected by both
    pstVppParam1->input_port.width = 95;
    pstVppParam1->input_port.height = 64;
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc2, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure 96x63, should be rejected by both
    pstVppParam1->input_port.width = 96;
    pstVppParam1->input_port.height = 63;
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc2, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Configure 2x2, should be rejected by both
    pstVppParam1->input_port.width = 2;
    pstVppParam1->input_port.height = 2;
    u32Ret = u32VppUsecase_Validate(NULL, pstUc1, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc1, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Validate(NULL, pstUc2, pstVppParam1, &pstUcFallBack);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppUsecase_Configure(pstCtx1, pstUc2, stpCtrl1, pstVppParam1);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Close the first context
    vVppUsecase_Term(pstCtx1);

    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    vpp_term(pstCtx1);
}

static void set_usecase_ip_credits(t_StVppUsecase *pstUc, t_EVppIp eIp,
                                   uint32_t u32CredSd, uint32_t u32CredHd,
                                   uint32_t u32CredFhd, uint32_t u32CredUhd)
{
    if (!pstUc)
        return;

    if (eIp >= VPP_IP_MAX)
        return;

    if (!pstUc->credits[eIp].ip)
        return;

    pstUc->credits[eIp].au32Credit[VPP_RESOLUTION_SD] = u32CredSd;
    pstUc->credits[eIp].au32Credit[VPP_RESOLUTION_HD] = u32CredHd;
    pstUc->credits[eIp].au32Credit[VPP_RESOLUTION_FHD] = u32CredFhd;
    pstUc->credits[eIp].au32Credit[VPP_RESOLUTION_UHD] = u32CredUhd;
}

TEST(Usecase_SupportedRes)
{
    uint32_t res;
    uint32_t n = NOT_SUPPORTED;
    t_StVppUsecase *pstUcSingle, *pstUcDouble;

    pstUcSingle = getUsecaseAt(TEST_SUITE_USECASE_MAX_RES, 0); // HVX
    pstUcDouble = getUsecaseAt(TEST_SUITE_USECASE_MAX_RES, 1); // HVX + GPU

    if (!pstUcSingle || !pstUcDouble)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    set_usecase_ip_credits(pstUcSingle, VPP_IP_HVX, n, n, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcSingle);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_MAX, res);

    set_usecase_ip_credits(pstUcSingle, VPP_IP_HVX, 1, n, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcSingle);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_SD, res);

    set_usecase_ip_credits(pstUcSingle, VPP_IP_HVX, 1, 1, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcSingle);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_HD, res);

    set_usecase_ip_credits(pstUcSingle, VPP_IP_HVX, 1, 1, 1, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcSingle);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_FHD, res);

    set_usecase_ip_credits(pstUcSingle, VPP_IP_HVX, 1, 1, 1, 1);
    res = u32VppUsecase_GetMaxResForUc(pstUcSingle);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_UHD, res);

    // Double usecases
    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, n, n, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, n, n, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_MAX, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, n, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, n, n, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_MAX, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, n, n, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, n, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_MAX, res);

    // SD
    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, n, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, n, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_SD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, n, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_SD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, 1, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, n, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_SD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, 1, 1);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, n, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_SD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, n, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_SD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, n, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, 1, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_SD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, n, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, 1, 1);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_SD, res);

    // HD
    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_HD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, 1, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_HD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, 1, 1);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, n, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_HD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, 1, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_HD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, n, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, 1, 1);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_HD, res);

    // FHD
    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, 1, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, 1, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_FHD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, 1, 1);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, 1, n);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_FHD, res);

    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, 1, n);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, 1, 1);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_FHD, res);

    // UHD
    set_usecase_ip_credits(pstUcDouble, VPP_IP_HVX, 1, 1, 1, 1);
    set_usecase_ip_credits(pstUcDouble, VPP_IP_GPU, 1, 1, 1, 1);
    res = u32VppUsecase_GetMaxResForUc(pstUcDouble);
    DVP_ASSERT_EQUAL(VPP_RESOLUTION_UHD, res);
}

TEST(Usecase_MappedPxBufReq)
{
    uint32_t u32;

    t_StVppIpBlock stIpGpu, stIpHvx;
    t_StVppUsecase stUc;
    t_StVppUsecase *pstUc = &stUc;

    memset(&stIpGpu, 0, sizeof(stIpGpu));
    memset(&stIpHvx, 0, sizeof(stIpHvx));
    memset(&stUc, 0, sizeof(stUc));

    stIpGpu.name = "TEST_GPU";
    stIpGpu.eIp = VPP_IP_GPU;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_FALSE;

    stIpHvx.name = "TEST_HVX";
    stIpHvx.eIp = VPP_IP_HVX;
    stIpHvx.capabilities.bRequireMappedPxBuf = VPP_FALSE;

    stUc.name = "TEST_USECASE";
    stUc.bRequireMappedPxBuf = VPP_FALSE;
    stUc.composition = calloc(2 * sizeof(t_StVppUsecaseIp), 1);
    if (!stUc.composition)
    {
        LOGE("Couldn't allocate memory for stUc.composition");
        DVP_ASSERT_FAIL();
        return;
    }
    stUc.composition[VPP_IP_GPU].ip = &stIpGpu;
    stUc.composition[VPP_IP_HVX].ip = &stIpHvx;
    stUc.u32IpCnt = 2;

    // NULL refers to bypass, map not required
    u32 = u32VppUsecase_IsMappedPxBufRequired(NULL);
    DVP_ASSERT_FALSE(u32);

    // UC + IP don't require mapping
    u32 = u32VppUsecase_IsMappedPxBufRequired(pstUc);
    DVP_ASSERT_FALSE(u32);

    // UC takes prescedence over IP blocks
    stUc.bRequireMappedPxBuf = VPP_TRUE;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_FALSE;
    stIpHvx.capabilities.bRequireMappedPxBuf = VPP_FALSE;
    u32 = u32VppUsecase_IsMappedPxBufRequired(pstUc);
    DVP_ASSERT_TRUE(u32);

    stUc.bRequireMappedPxBuf = VPP_FALSE;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_TRUE;
    stIpHvx.capabilities.bRequireMappedPxBuf = VPP_FALSE;
    u32 = u32VppUsecase_IsMappedPxBufRequired(pstUc);
    DVP_ASSERT_TRUE(u32);

    stUc.bRequireMappedPxBuf = VPP_FALSE;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_FALSE;
    stIpHvx.capabilities.bRequireMappedPxBuf = VPP_TRUE;
    u32 = u32VppUsecase_IsMappedPxBufRequired(pstUc);
    DVP_ASSERT_TRUE(u32);

    stUc.bRequireMappedPxBuf = VPP_FALSE;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_TRUE;
    stIpHvx.capabilities.bRequireMappedPxBuf = VPP_TRUE;
    u32 = u32VppUsecase_IsMappedPxBufRequired(pstUc);
    DVP_ASSERT_TRUE(u32);

    stUc.bRequireMappedPxBuf = VPP_TRUE;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_TRUE;
    stIpHvx.capabilities.bRequireMappedPxBuf = VPP_TRUE;
    u32 = u32VppUsecase_IsMappedPxBufRequired(pstUc);
    DVP_ASSERT_TRUE(u32);
    free(stUc.composition);
}

TEST(Usecase_IpFuncs)
{
    t_StVppCtx *pstCtx1, *pstCtx2;
    struct vpp_callbacks cb = { NULL, NULL, NULL, NULL };
    uint32_t u32Ret;
    t_StVppHqvCtrl *stpCtrl1, *stpCtrl2;
    t_StVppParam *pstVppParam1, *pstVppParam2;
    t_StVppUsecase *pstUc1 = NULL;
    t_StVppUsecase *pstUc2 = NULL;
    t_StVppUsecase *pstUc3 = NULL;
    t_StVppIpInternalReqs stReq;
    t_StVppParam stIpParam;

    pstCtx1 = vpp_init(0, cb);
    pstCtx2 = vpp_init(0, cb);
    u32Ret = usecase_test_init(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    if (u32Ret != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstUc1 = &uc_mock_uctest_hvx;
    pstUc2 = &uc_mock_uctest_two_ip;
    pstUc3 = &uc_mock_uctest_three_ip;

    // Usecase 1 HVX AIE Only (1 IP)
    // Usecase 2 GPU CADE + HVX CNR (2 IP)
    // Usecase 3 HVX DI + GPU CADE(can scale) + HVX CNR+AIE (3 IP)

    u32Ret = u32VppUsecase_SubmodCount(pstUc1);
    DVP_ASSERT_EQUAL(u32Ret, 1);

    u32Ret = u32VppUsecase_SubmodCount(pstUc2);
    DVP_ASSERT_EQUAL(u32Ret, 2);

    u32Ret = u32VppUsecase_SubmodCount(pstUc3);
    DVP_ASSERT_EQUAL(u32Ret, 3);

    // Null use case
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(NULL, 0, HQV_CONTROL_AIE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Usecase 1
    // Supported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc1, 0, HQV_CONTROL_AIE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc1, 0, HQV_CONTROL_CADE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc1, 0, HQV_CONTROL_NONE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Supported control, out of range IP
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc1, 1, HQV_CONTROL_CADE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Usecase 2
    // Supported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc2, 0, HQV_CONTROL_CADE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    // Supported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc2, 1, HQV_CONTROL_CNR);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc2, 0, HQV_CONTROL_CNR);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc2, 1, HQV_CONTROL_CADE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Supported control, out of range IP
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc2, 2, HQV_CONTROL_CADE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Usecase 3
    // Supported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 0, HQV_CONTROL_DI);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    // Supported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 1, HQV_CONTROL_CADE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    // Supported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 2, HQV_CONTROL_CNR);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    // Supported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 2, HQV_CONTROL_AIE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_TRUE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 0, HQV_CONTROL_CNR);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 0, HQV_CONTROL_AIE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 0, HQV_CONTROL_CADE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 1, HQV_CONTROL_DI);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 1, HQV_CONTROL_AIE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 1, HQV_CONTROL_CNR);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 2, HQV_CONTROL_DI);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 2, HQV_CONTROL_CADE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Unsupported control
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc3, 2, HQV_CONTROL_FRC);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Supported control, out of range IP
    u32Ret = u32VppUsecase_IpIndexSupportsAlgo(pstUc2, 0xFFFF, HQV_CONTROL_CADE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_FALSE);

    // Invalid inputs
    u32Ret = u32VppUsecase_GetInternalBufReq(NULL, pstVppParam1, 0, &stReq);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc1, NULL, 0, &stReq);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc1, pstVppParam1, 0xFFFF, &stReq);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc1, pstVppParam1, 0, NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppUsecase_GetIpPortParam(NULL, pstVppParam1, 0 ,&stIpParam);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppUsecase_GetIpPortParam(pstUc1, NULL, 0, &stIpParam);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppUsecase_GetIpPortParam(pstUc1, pstVppParam1, 0xFFFF, &stIpParam);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppUsecase_GetIpPortParam(pstUc1, pstVppParam1, 0, NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Valid get
    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc1, pstVppParam1, 0, &stReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stReq.eInColorFmt, VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(stReq.eOutColorFmt, VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(stReq.u32InternalOutBufReq, 0);
    DVP_ASSERT_EQUAL(stReq.eOutBufType, eVppBufPxDataType_Raw);

    u32Ret = u32VppUsecase_GetIpPortParam(pstUc1, pstVppParam1, 0, &stIpParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stIpParam.input_port.fmt, pstVppParam1->input_port.fmt);
    DVP_ASSERT_EQUAL(stIpParam.input_port.width, pstVppParam1->input_port.width);
    DVP_ASSERT_EQUAL(stIpParam.input_port.height, pstVppParam1->input_port.height);
    DVP_ASSERT_EQUAL(stIpParam.output_port.fmt, pstVppParam1->output_port.fmt);
    DVP_ASSERT_EQUAL(stIpParam.output_port.width, pstVppParam1->output_port.width);
    DVP_ASSERT_EQUAL(stIpParam.output_port.height, pstVppParam1->output_port.height);

    // Valid get
    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc3, pstVppParam1, 0, &stReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stReq.eInColorFmt, VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(stReq.eOutColorFmt, VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(stReq.u32InternalOutBufReq, 8);
    DVP_ASSERT_EQUAL(stReq.eOutBufType, eVppBufPxDataType_Raw);

    // Valid get
    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc3, pstVppParam1, 1, &stReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stReq.eInColorFmt, VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(stReq.eOutColorFmt, VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(stReq.u32InternalOutBufReq, 6);
    DVP_ASSERT_EQUAL(stReq.eOutBufType, eVppBufPxDataType_Raw);

    // Valid get
    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc3, pstVppParam1, 2, &stReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stReq.eInColorFmt, VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(stReq.eOutColorFmt, VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(stReq.u32InternalOutBufReq, 0);
    DVP_ASSERT_EQUAL(stReq.eOutBufType, eVppBufPxDataType_Raw);

    // Invalid get
    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc3, pstVppParam1, 3, &stReq);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    pstVppParam2->input_port.fmt = VPP_COLOR_FORMAT_NV21_VENUS;
    pstVppParam2->output_port.fmt = VPP_COLOR_FORMAT_NV21_VENUS;
    pstVppParam2->output_port.width = 1280;
    pstVppParam2->output_port.height = 720;

    // UC1 does not support scaling
    u32Ret = u32VppUsecase_GetIpPortParam(pstUc1, pstVppParam2, 0, &stIpParam);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Valid get
    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc3, pstVppParam2, 0, &stReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stReq.eInColorFmt, VPP_COLOR_FORMAT_NV21_VENUS);
    DVP_ASSERT_EQUAL(stReq.eOutColorFmt, VPP_COLOR_FORMAT_NV21_VENUS);
    DVP_ASSERT_EQUAL(stReq.u32InternalOutBufReq, 12);
    DVP_ASSERT_EQUAL(stReq.eOutBufType, eVppBufPxDataType_Raw);

    // Valid get
    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc3, pstVppParam2, 1, &stReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stReq.eInColorFmt, VPP_COLOR_FORMAT_NV21_VENUS);
    DVP_ASSERT_EQUAL(stReq.eOutColorFmt, VPP_COLOR_FORMAT_NV21_VENUS);
    DVP_ASSERT_EQUAL(stReq.u32InternalOutBufReq, 10);
    DVP_ASSERT_EQUAL(stReq.eOutBufType, eVppBufPxDataType_Raw);

    // Valid get
    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc3, pstVppParam2, 2, &stReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stReq.eInColorFmt, VPP_COLOR_FORMAT_NV21_VENUS);
    DVP_ASSERT_EQUAL(stReq.eOutColorFmt, VPP_COLOR_FORMAT_NV21_VENUS);
    DVP_ASSERT_EQUAL(stReq.u32InternalOutBufReq, 0);
    DVP_ASSERT_EQUAL(stReq.eOutBufType, eVppBufPxDataType_Raw);

    u32Ret = u32VppUsecase_GetIpPortParam(pstUc3, pstVppParam2, 0, &stIpParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stIpParam.input_port.fmt, pstVppParam2->input_port.fmt);
    DVP_ASSERT_EQUAL(stIpParam.input_port.width, pstVppParam2->input_port.width);
    DVP_ASSERT_EQUAL(stIpParam.input_port.height, pstVppParam2->input_port.height);
    DVP_ASSERT_EQUAL(stIpParam.output_port.fmt, pstVppParam2->output_port.fmt);
    DVP_ASSERT_EQUAL(stIpParam.output_port.width, 1920);
    DVP_ASSERT_EQUAL(stIpParam.output_port.height, 1080);

    u32Ret = u32VppUsecase_GetIpPortParam(pstUc3, pstVppParam2, 1, &stIpParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stIpParam.input_port.fmt, pstVppParam2->input_port.fmt);
    DVP_ASSERT_EQUAL(stIpParam.input_port.width, pstVppParam2->input_port.width);
    DVP_ASSERT_EQUAL(stIpParam.input_port.height, pstVppParam2->input_port.height);
    DVP_ASSERT_EQUAL(stIpParam.output_port.fmt, pstVppParam2->output_port.fmt);
    DVP_ASSERT_EQUAL(stIpParam.output_port.width, pstVppParam2->output_port.width);
    DVP_ASSERT_EQUAL(stIpParam.output_port.height, pstVppParam2->output_port.height);

    u32Ret = u32VppUsecase_GetIpPortParam(pstUc3, pstVppParam2, 2, &stIpParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stIpParam.input_port.fmt, pstVppParam2->input_port.fmt);
    DVP_ASSERT_EQUAL(stIpParam.input_port.width, 1280);
    DVP_ASSERT_EQUAL(stIpParam.input_port.height, 720);
    DVP_ASSERT_EQUAL(stIpParam.output_port.fmt, pstVppParam2->output_port.fmt);
    DVP_ASSERT_EQUAL(stIpParam.output_port.width, pstVppParam2->output_port.width);
    DVP_ASSERT_EQUAL(stIpParam.output_port.height, pstVppParam2->output_port.height);

    // Invalid get
    u32Ret = u32VppUsecase_GetIpPortParam(pstUc3, pstVppParam2, 3, &stIpParam);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Input NV21, Output NV12 not supported
    pstVppParam2->output_port.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    u32Ret = u32VppUsecase_GetInternalBufReq(pstUc3, pstVppParam2, 0, &stReq);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppUsecase_GetIpPortParam(pstUc3, pstVppParam2, 2, &stIpParam);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Input UBWC_TP10, Output UBWC_TP10 Supported
    pstVppParam2->input_port.fmt = VPP_COLOR_FORMAT_UBWC_TP10;
    pstVppParam2->output_port.fmt = VPP_COLOR_FORMAT_UBWC_TP10;
    u32Ret = u32VppUsecase_GetIpPortParam(pstUc3, pstVppParam2, 0, &stIpParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stIpParam.input_port.fmt, pstVppParam2->input_port.fmt);
    DVP_ASSERT_EQUAL(stIpParam.input_port.width, pstVppParam2->input_port.width);
    DVP_ASSERT_EQUAL(stIpParam.input_port.height, pstVppParam2->input_port.height);
    DVP_ASSERT_EQUAL(stIpParam.output_port.fmt, VPP_COLOR_FORMAT_UBWC_TP10);
    DVP_ASSERT_EQUAL(stIpParam.output_port.width, 1920);
    DVP_ASSERT_EQUAL(stIpParam.output_port.height, 1080);

    u32Ret = u32VppUsecase_GetIpPortParam(pstUc3, pstVppParam2, 1, &stIpParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stIpParam.input_port.fmt, VPP_COLOR_FORMAT_UBWC_TP10);
    DVP_ASSERT_EQUAL(stIpParam.input_port.width, pstVppParam2->input_port.width);
    DVP_ASSERT_EQUAL(stIpParam.input_port.height, pstVppParam2->input_port.height);
    DVP_ASSERT_EQUAL(stIpParam.output_port.fmt, VPP_COLOR_FORMAT_UBWC_TP10);
    DVP_ASSERT_EQUAL(stIpParam.output_port.width, pstVppParam2->output_port.width);
    DVP_ASSERT_EQUAL(stIpParam.output_port.height, pstVppParam2->output_port.height);

    u32Ret = u32VppUsecase_GetIpPortParam(pstUc3, pstVppParam2, 2, &stIpParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stIpParam.input_port.fmt, VPP_COLOR_FORMAT_UBWC_TP10);
    DVP_ASSERT_EQUAL(stIpParam.input_port.width, 1280);
    DVP_ASSERT_EQUAL(stIpParam.input_port.height, 720);
    DVP_ASSERT_EQUAL(stIpParam.output_port.fmt, pstVppParam2->output_port.fmt);
    DVP_ASSERT_EQUAL(stIpParam.output_port.width, pstVppParam2->output_port.width);
    DVP_ASSERT_EQUAL(stIpParam.output_port.height, pstVppParam2->output_port.height);

    usecase_test_deinit(&stpCtrl1, &stpCtrl2, &pstVppParam1, &pstVppParam2);
    vpp_term(pstCtx1);
    vpp_term(pstCtx2);
}

/************************************************************************
 * Global Functions
 ***********************************************************************/

TEST_CASES UsecaseTests[] = {
//    TEST_CASE(Usecase_Find),
    TEST_CASE(Usecase_OpenClose),
    TEST_CASE(Usecase_Reconfigure),
    TEST_CASE(Usecase_Framerate),
    TEST_CASE(Usecase_Credits),
    TEST_CASE(Usecase_Sessions),
    TEST_CASE(Usecase_GetRequirements),
    TEST_CASE(Usecase_CpQuirks),
    TEST_CASE(Usecase_CtrlSupported),
    TEST_CASE(Usecase_HelperFuncs),
    TEST_CASE(Usecase_Resolution),
    TEST_CASE(Usecase_SupportedRes),
    TEST_CASE(Usecase_MappedPxBufReq),
    TEST_CASE(Usecase_IpFuncs),
    TEST_CASE_NULL(),
};

TEST_SUITE(UsecaseSuite,
        "Usecase Tests",
        UsecaseSuiteInit,
        UsecaseSuiteTerm,
        UsecaseTestInit,
        UsecaseTestTerm,
        UsecaseTests);

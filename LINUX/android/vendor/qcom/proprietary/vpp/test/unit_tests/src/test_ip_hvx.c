/*!
 * @file test_ip_hvx.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 *
 *
 */

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_MODULE_HVX_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_HVX

#include "vpp_dbg.h"
#include "vpp_def.h"
#include "vpp_ctx.h"
#include "vpp_core.h"
#include "vpp.h"
#include "vpp_uc.h"
#include "vpp_utils.h"

#include "test_mock_registry.h"
#include "vpp_ip_hvx.h"
#include "test_utils.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
struct StHvxTestCtx {
    void *pstHvxCb; // context returned from hvx init
    uint32_t u32ExpExtraLen;
    uint32_t u32ExpOBD;
    uint32_t u32DrainOBD;
    uint32_t u32BypassExpected;
    uint32_t* au32BypSeq;
    uint32_t* au32OutFlgSeq;
    uint32_t* au32MarkerSeq;
    // function to validate output buffers @ obd
    void (*pfnc_obd_cust_check)(t_StVppBuf*);
};

#define INPUT_FLUSH_DONE        (1<<0)
#define OUTPUT_FLUSH_DONE       (1<<1)
#define DRAIN_DONE              (1<<2)

#define ROTATETEST_INNAME                       "beach_1080p_track1.yuv"
#define ROTATETEST_INVAL_INNAME                 "beach_1080p_INVALID_track1.yuv"

#define ROTATETEST_CNR_OUTNAME                  "beach_1080p_CNR_track1.yuv"
#define ROTATETEST_AIE_OUTNAME                  "beach_1080p_AIE_track1.yuv"
#define ROTATETEST_DI_OUTNAME                   "beach_1080p_DI_track1.yuv"
#define ROTATETEST_GLOBAL_DEMO_OUTNAME          "beach_1080p_GLOBAL_DEMO_track1.yuv"
#define ROTATETEST_DEFAULT_OUTNAME              "beach_1080p_DEFAULT_track1.yuv"
#define ROTATETEST_INVAL_OUTNAME                "beach_1080p_INVALID_track1.yuv"
#define ROTATETEST_INVAL_OUTNAME                "beach_1080p_INVALID_track1.yuv"
#define ROTATETEST_CNR_AIE_OUTNAME              "beach_1080p_CNR_AIE_track1.yuv"
#define ROTATETEST_AIE_VALRECONFIG_OUTNAME      "beach_1080p_AIE_valid_track1.yuv"
#define ROTATETEST_AIE_INVALRECONFIG_OUTNAME    "beach_1080p_AIE_invalid_track1.yuv"

#define ROTATETEST2_INNAME                      "bridge_480p_track1.yuv"
#define ROTATETEST2_AIEVALID_OUTNAME            "bridge_480p_AIEVALID_track1.yuv"
#define ROTATETEST2_AIEINVALID_OUTNAME          "bridge_480p_AIEINVALID_track1.yuv"

#define SET_CTRL_CADE(ctrl,md,lvl,cont,sat)       ctrl.mode = md; ctrl.cade_level = lvl; ctrl.contrast = cont; ctrl.saturation = sat;
#define SET_CTRL_CNR(ctrl,md,lvl)                 ctrl.mode = md; ctrl.level = lvl;
#define SET_CTRL_AIE(ctrl,md,hmd,c_lvl,l_lvl)     ctrl.mode = md; ctrl.hue_mode = hmd; ctrl.cade_level = c_lvl; ctrl.ltm_level = l_lvl;
#define SET_CTRL_DI(ctrl,md)                      ctrl.mode = md;
#define SET_CTRL_DEMO(ctrl,perc,dir)              ctrl.process_percent = perc; ctrl.process_direction = dir;

#define SET_FILENAMES(inname,outname,maxlen) { \
    strlcpy(pstTestCtx->params.cInputName, inname, maxlen); \
    strlcpy(pstTestCtx->params.cOutputName, outname, maxlen); \
}
#define SET_BUF_PARAMS(w,h,fmt) { \
    pstTestCtx->params.u32Width = w; \
    pstTestCtx->params.u32Height = h; \
    pstTestCtx->params.eInputBufFmt = fmt; \
    pstTestCtx->params.eOutputBufFmt = fmt; \
}

/************************************************************************
 * Local static variables
 ***********************************************************************/
static struct test_ctx stTestCtx;
static struct test_ctx *pstTestCtx = &stTestCtx;
static struct StHvxTestCtx stCtxHvx;
static const uint32 cu32_valid_buf_flags = VPP_BUFFER_FLAG_EOS |
    VPP_BUFFER_FLAG_DATACORRUPT | VPP_BUFFER_FLAG_SYNCFRAME |
    VPP_BUFFER_FLAG_READONLY;

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/
static void obd_buf_check(t_StVppBuf *pBuf)
{
    t_EVppBufType eType;

    if (pBuf->eQueuedPort==VPP_PORT_INPUT)
        LOGI("BYPASS_DETECTED!\n");

    eType = eVppBuf_GetFrameType(pBuf);
    DVP_ASSERT_EQUAL(eType, eVppBufType_Progressive);

    if (!stCtxHvx.u32BypassExpected)
        DVP_ASSERT_EQUAL(pBuf->eQueuedPort,VPP_PORT_OUTPUT);

    return;
}

static void obd_di_buf_check(t_StVppBuf *pBuf)
{
    t_EVppBufType eType;
    uint32_t val, u32;

    u32 = u32VppBuf_GetFrameTypeExtradata(pBuf, VPP_EXTERNAL_EXTRADATA_TYPE, &eType);

    if (stTestCtx.u32OutRxCnt < stCtxHvx.u32ExpOBD)
    {
        if (stCtxHvx.au32BypSeq != NULL)
        {
            if (!stCtxHvx.au32BypSeq[stTestCtx.u32OutRxCnt])
            {
                DVP_ASSERT_EQUAL(eType, eVppBufType_Progressive);
                DVP_ASSERT_EQUAL(pBuf->eQueuedPort,VPP_PORT_OUTPUT);
            }
            else
            {
                DVP_ASSERT_EQUAL(pBuf->eQueuedPort,VPP_PORT_INPUT);
            }
        }
        else
        {
            DVP_ASSERT_EQUAL(eType, eVppBufType_Progressive);
            if (!stCtxHvx.u32BypassExpected)
                DVP_ASSERT_EQUAL(pBuf->eQueuedPort,VPP_PORT_OUTPUT);
        }

        if (stCtxHvx.au32OutFlgSeq != NULL)
        {
            DVP_ASSERT_TRUE(stTestCtx.u32OutRxCnt < stCtxHvx.u32ExpOBD);
            DVP_ASSERT_TRUE((pBuf->pBuf->flags & stCtxHvx.au32OutFlgSeq[stTestCtx.u32OutRxCnt]) ==
                    stCtxHvx.au32OutFlgSeq[stTestCtx.u32OutRxCnt]);
            LOGD("obd buf flags = %d", pBuf->pBuf->flags);
        }

        if (stCtxHvx.au32MarkerSeq != NULL)
        {
            get_extra_data_marker_int_buf(pBuf, &val);
            DVP_ASSERT_TRUE(stCtxHvx.au32MarkerSeq[stTestCtx.u32OutRxCnt] == val);
            LOGD("obd buf marker = %d", val);
        }
    }
    else
    {
        DVP_ASSERT_TRUE(pBuf->stPixel.u32FilledLen == 0);
    }

    return;
}

static uint32_t u32CheckMem(char *mem1,char *mem2,uint32_t len)
{
    uint32_t i = 0 ;
    for (i = 0; i < len; i++)
    {
        if(mem1[i] != mem2[i])
            return VPP_ERR;
    }
    return VPP_OK;
}

static int32_t s32MultiParamTest(char *pcParam[], uint32_t NmbOfParam,
                                 uint32_t *pfLocal, uint32_t *pu32ParamId,
                                 uint32_t *pu32ParamSubId, size_t *pu32ParamSize)
{
    uint32_t u32, i;
    struct hqv_control ctrl;
    uint32_t fLocal = 0;

    // Set to manual and custom
    ctrl.mode = HQV_MODE_MANUAL;
    ctrl.ctrl_type = HQV_CONTROL_CUST;
    ctrl.custom.id = VPP_IP_HVX;
    ctrl.custom.len = 0;
    char *cPtr = (char*) ctrl.custom.data;

    for (i = 0; i < NmbOfParam; i++)
    {
        fLocal = pfLocal[i];
        LOGD("%s(): Test %s parameter setting u32ParamId=%d u32ParamSubId=%d.pu32ParamSize[i]=%d",
             __func__, fLocal == 0 ? "global" : "local", pu32ParamId[i], pu32ParamSubId[i], pu32ParamSize[i]);
        if ((ctrl.custom.len + pu32ParamSize[i] + HVX_CUSTOM_HEADER_SIZE) > HQV_CUSTOM_PAYLOAD_SZ)
        {
            LOGD("Too many parameters to set. i=%d NmbOfParam=%d pu32ParamSize[i]=%d ctrl.custom.len=%d",
                 i, NmbOfParam, pu32ParamSize[i], ctrl.custom.len);
            break;
        }
        ctrl.custom.len += pu32ParamSize[i] + HVX_CUSTOM_HEADER_SIZE;//header length is 4
        *cPtr++ = pu32ParamSize[i] + HVX_CUSTOM_HEADER_SIZE; //lenth low 8-bit
        *cPtr++ = 0;// length hi 8-bit
        uint32_t tmp = fLocal;
        tmp |= pu32ParamId[i] << HVX_GLOBAL_LOCAL_BIT_LENGTH;
        tmp |= pu32ParamSubId[i] << (HVX_PARAMID_BIT_LENGTH + HVX_GLOBAL_LOCAL_BIT_LENGTH);
        *cPtr++ = tmp & 0xff;       //ID low 8-bit
        *cPtr++ = (tmp >> 8) & 0xff;  //ID hi 8-bit
        //LOGD("ThisID=0x%x",tmp);

        memset(cPtr, 0x55 + fLocal, pu32ParamSize[i]);
        cPtr += pu32ParamSize[i];
    }

    u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
    DVP_ASSERT_TRUE(u32 == VPP_OK);

    //check result
    uint32_t u32MaxParamNmb = i;
    cPtr  = (char*) ctrl.custom.data;
    for (i = 0; i < u32MaxParamNmb; i++)
    {
        char *ptr;
        fLocal = pfLocal[i];
        ptr = pcParam[i];

        cPtr += HVX_CUSTOM_HEADER_SIZE; //skip 4 bytes header
        u32 = u32CheckMem((char*) cPtr, ptr, pu32ParamSize[i]);
        DVP_ASSERT_TRUE(u32 == VPP_OK);
        LOGD("%s() After setting vpp_set_ctrl(),pstCustomHvxParams=%p fLocal=%d 0x%x %x %x %x",
                __func__, ptr, fLocal, ptr[0], ptr[1], ptr[2], ptr[3]);
        cPtr += pu32ParamSize[i];
    }
    return 0;
}

static int32_t s32ParamTest(char* cLocalPara, char *cGlobalPara,
                            uint32_t u32ParamId, uint32_t u32ParamSubId,
                            size_t ParamSize, char *cParamName)
{
    uint32_t u32;
    struct hqv_control ctrl;

    uint32_t fGlobalLocal = 0;

    //Test both global (fGlobalLocal==0) and local (fGlobalLocal==1)
    for (fGlobalLocal = 0; fGlobalLocal < 2; fGlobalLocal++)
    {
        char *ptr;
        LOGD("%s(): Test %s parameter setting u32ParamId=%d u32ParamSubId=%d.",
             __func__, fGlobalLocal == 0 ? "global" : "local", u32ParamId,
             u32ParamSubId);

        // Set to manual and custom
        ctrl.mode = HQV_MODE_MANUAL;
        ctrl.ctrl_type = HQV_CONTROL_CUST;
        ctrl.custom.id = VPP_IP_HVX;
        ctrl.custom.len = ParamSize + HVX_CUSTOM_HEADER_SIZE; //header length is 4
        char *cPtr = (char*)ctrl.custom.data;
        *cPtr++ = ParamSize + HVX_CUSTOM_HEADER_SIZE; //lenth low 8-bit
        *cPtr++ = 0; // length hi 8-bit
        uint32_t tmp = fGlobalLocal;
        tmp |= u32ParamId << HVX_GLOBAL_LOCAL_BIT_LENGTH;
        tmp |= u32ParamSubId << (HVX_PARAMID_BIT_LENGTH + HVX_GLOBAL_LOCAL_BIT_LENGTH);
        *cPtr++ = tmp & 0xff;       //ID low 8-bit
        *cPtr++ = (tmp >> 8) & 0xff;  //ID hi 8-bit
        //LOGD("ThisID=0x%x",tmp);

        memset(cPtr, 0x55 + fGlobalLocal, ParamSize);
        //u32 = vpp_set_ctrl(p, ctrl, &req);
        u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
        DVP_ASSERT_TRUE(u32 == VPP_OK);

        //check result
        if (fGlobalLocal == 0)
            ptr = cGlobalPara;
        else
            ptr = cLocalPara;

        u32 = u32CheckMem((char*) cPtr, ptr, ParamSize);
        DVP_ASSERT_TRUE(u32 == VPP_OK);
        LOGD("%s() After setting vpp_set_ctrl(),pstCustomHvxParams=%p 0x%x %x %x %x",
             __func__,ptr, ptr[0],ptr[1],ptr[2],ptr[3]);

        if (fGlobalLocal == 0)//Global
        {
            //check global is the same as local
            //char *ptr1 = (char*) &pstHvxCb->stLocalHvxParams.stCustomNrParams;
            char *ptr1 = cGlobalPara;
            LOGD("%s(),u32ParamSubId=%d After global parameters setting, local %s=%p 0x%x %x %x %x",
                 __func__, u32ParamSubId, cParamName, ptr1, ptr1[0], ptr1[1],
                 ptr1[2], ptr1[3]);
            ptr = cLocalPara;
            LOGD("%s(),u32ParamSubId=%d After global parameters setting, global %s=%p 0x%x %x %x %x",
                 __func__, u32ParamSubId, cParamName, ptr, ptr[0], ptr[1],
                 ptr[2], ptr[3]);
            u32 = u32CheckMem((char*) ptr1, ptr, ParamSize);
            DVP_ASSERT_TRUE(u32 == VPP_OK); //local parameters the same as global for session2
        }
        if (fGlobalLocal == 1)//local
        {
            //check global is not the same as local
            char *ptr1 = cLocalPara;
            LOGD("%s(),u32ParamSubId=%d After local parameters setting, local %s=%p 0x%x %x %x %x",
                 __func__, u32ParamSubId, cParamName, ptr1, ptr1[0], ptr1[1],
                 ptr1[2], ptr1[3]);
            char *ptr = cGlobalPara;
            LOGD("%s(),u32ParamSubId=%d After local parameters setting, global %s=%p 0x%x %x %x %x",
                 __func__, u32ParamSubId, cParamName, ptr, ptr[0], ptr[1],
                 ptr[2], ptr[3]);
            u32 = u32CheckMem((char*) ptr1, ptr, ParamSize);
            DVP_ASSERT_TRUE(u32 != VPP_OK);
        }
    }
    return 0;
}

static void test_hvx_input_buffer_done(void *pv, t_StVppBuf *pBuf)
{
    LOGI("Running test_hvx_input_buffer_done");

    struct test_ctx *pCtx = (struct test_ctx*) pv;
    struct bufnode *pNode = pBuf->pBuf->cookie;
    uint32_t u32Ret;

    LOGI("IBD_owner:%d\n", pNode->port_owner);

    DVP_ASSERT_TRUE(pNode->owner == BUF_OWNER_LIBRARY);
    pNode->owner = BUF_OWNER_CLIENT;
    u32Ret = restore_bufnode_internal_buf(pNode);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    put_buf(pCtx->buf_pool, pNode);
    pCtx->u32InRxCnt += 1;
    LOGI("-> Done: %d IBDs processed\n", pCtx->u32InRxCnt);
    pthread_cond_signal(&pCtx->cond);
}

static void test_hvx_output_buffer_done(void *pv, t_StVppBuf *pBuf)
{
    LOGI("Running test_hvx_output_buffer_done");
    struct test_ctx *pCtx = (struct test_ctx*) pv;
    struct bufnode *pNode = pBuf->pBuf->cookie;
    uint32_t u32Ret;

    LOGI("INJECT_OBD:internal_buf_owner=%d\n", pBuf->eQueuedPort);

    DVP_ASSERT_TRUE(pNode->owner == BUF_OWNER_LIBRARY);
    pNode->owner = BUF_OWNER_CLIENT;

    LOGI("OBD_owner:%d\n", pNode->port_owner);

    if (pNode->pIntBuf->stPixel.u32FilledLen)
    {
        LOGI("Running dump_buf");
        dump_buf(pNode);
        DVP_ASSERT_EQUAL(pNode->pIntBuf->stExtra.u32FilledLen, stCtxHvx.u32ExpExtraLen);
        LOGI("extradata: expFillLen=%u, act_fill_len=%u", stCtxHvx.u32ExpExtraLen,
             pNode->pIntBuf->stExtra.u32FilledLen);
        validate_extradata_integrity(pNode);
    }

    // Run odb custom check callback
    if (stCtxHvx.pfnc_obd_cust_check != NULL)
        (*stCtxHvx.pfnc_obd_cust_check) (pBuf);

    u32Ret = restore_bufnode_internal_buf(pNode);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    put_buf(pCtx->buf_pool, pNode);
    pCtx->u32OutRxCnt += 1;
    LOGI("-> Done: %d OBDs processed\n", pCtx->u32OutRxCnt);
    pthread_cond_signal(&pCtx->cond);
}

static void test_hvx_event(void *pv, t_StVppEvt stEvt)
{
    LOGI("%s() got event: %u", __func__, stEvt.eType);
    struct test_ctx *pCtx = (struct test_ctx *) pv;
    if (!pCtx)
    {
        LOGE("context was null");
        DVP_ASSERT_FAIL();
        return;
    }

    pthread_mutex_lock(&pCtx->mutex);

    if (stEvt.eType == VPP_EVT_FLUSH_DONE)
    {
        if (stEvt.flush.ePort == VPP_PORT_INPUT)
            pCtx->u32Flags |= INPUT_FLUSH_DONE;
        else if (stEvt.flush.ePort == VPP_PORT_OUTPUT)
            pCtx->u32Flags |= OUTPUT_FLUSH_DONE;
    }
    else if (stEvt.eType == VPP_EVT_DRAIN_DONE)
    {
        pCtx->u32Flags |= DRAIN_DONE;
    }

    pthread_mutex_unlock(&pCtx->mutex);

    pthread_cond_signal(&pCtx->cond);
}

/************************************************************************
 * Helper Functions
 ***********************************************************************/
static void queue_single_buf(int port_owner, t_EVppBufType eBufType,
                             uint32_t buf_flags, uint32_t buf_fill,
                             uint32_t buf_marker)
{
    struct bufnode *pNode;
    uint32_t u32;

    // Run get_buf() to retrieve a buffer from the buffer pool
    pNode = get_buf(stTestCtx.buf_pool);
    if (pNode)
    {
        if (port_owner == VPP_PORT_INPUT)
        {
            if (buf_fill == VPP_TRUE)
                fill_buf(pNode);
            else
            {
                pNode->pIntBuf->pBuf->pixel.filled_len = 0;
                pNode->pIntBuf->stPixel.u32FilledLen = 0;
            }

            pNode->owner = BUF_OWNER_LIBRARY;

            // Only support Progressive, Interleaved TFF, and Interleaved BFF
            DVP_ASSERT_TRUE(eBufType == eVppBufType_Progressive ||
                            eBufType == eVppBufType_Interleaved_TFF ||
                            eBufType == eVppBufType_Interleaved_BFF);
            fill_extra_data(pNode, eBufType, buf_marker);
            pNode->pIntBuf->eBufType = eBufType;
            DVP_ASSERT_FALSE(buf_flags & ~cu32_valid_buf_flags);
            pNode->pIntBuf->pBuf->flags = buf_flags;
            stCtxHvx.u32ExpExtraLen = pNode->pExtBuf->extradata.filled_len;

            LOGI("Starting u32VppIpHvx_QueueBuf for VPP_PORT_INPUT");
        }
        else
        {
            pNode->owner = BUF_OWNER_LIBRARY;

            DVP_ASSERT_FALSE(stTestCtx.u32Flags & DRAIN_DONE);

            LOGI("Starting u32VppIpHvx_QueueBuf for VPP_PORT_OUTPUT");
        }
        // 8996 requires a mapped pixel buffer since register fd doesn't work for non-secure
        if (u32VppUtils_IsSoc(MSM8996) &&
            (stTestCtx.buf_pool->params.eProtection == PROTECTION_ZONE_NONSECURE))
            pNode->pIntBuf->stPixel.eMapping = eVppBuf_MappedInternal;
        else
            pNode->pIntBuf->stPixel.eMapping= eVppBuf_Unmapped;

        pNode->pIntBuf->eQueuedPort = port_owner;
        pNode->pIntBuf->eCurrentIpPort = port_owner;
        u32 = u32VppIpHvx_QueueBuf(stCtxHvx.pstHvxCb, port_owner, pNode->pIntBuf);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }
    else
    {
        DVP_ASSERT_FAIL();
    }
}

static void set_default_hqv_control(struct hqv_control *ctrl, enum hqv_mode mode,
                                    enum hqv_control_type ctrl_type)
{
    if (ctrl == NULL)
    {
        LOGE("ctrl was null");
        DVP_ASSERT_FAIL();
        return;
    }

    ctrl->mode = mode;
    ctrl->ctrl_type = ctrl_type;

    // Exit early if not manual mode or auto mode
    if (ctrl->mode != HQV_MODE_MANUAL && ctrl->mode != HQV_MODE_AUTO)
    {
        ctrl->ctrl_type = HQV_CONTROL_NONE;
        return;
    }

    ctrl->ctrl_type = ctrl_type;
    switch(ctrl->ctrl_type)
    {
        case HQV_CONTROL_CADE:
            /*! Arg 1: Valid values: HQV_MODE_OFF, HQV_MODE_AUTO, HQV_MODE_MANUAL */
            /*! Arg 2: Range: 0 - 100 */
            /*! Arg 3: Range: (-50) - 50 */
            /*! Arg 4: Range: (-50) - 50 */
            SET_CTRL_CADE(ctrl->cade, HQV_MODE_MANUAL, 50, 0, 0);
            break;
        case HQV_CONTROL_CNR:
            /*! Arg 1: Valid values: HQV_MODE_OFF, HQV_MODE_AUTO, HQV_MODE_MANUAL */
            /*! Arg 2: Range: 0 - 100 */
            SET_CTRL_CNR(ctrl->cnr, HQV_MODE_MANUAL, 100);
            break;
        case HQV_CONTROL_AIE:
            /*! Arg 1: Valid values: HQV_MODE_OFF, HQV_MODE_AUTO, HQV_MODE_MANUAL */
            /*! Arg 2: Valid values: HQV_HUE_MODE_ON, HQV_HUE_MODE_OFF */
            /*! Arg 3: Range: 0 - 100 */
            /*! Arg 4: Range: 0 - 100 */
            SET_CTRL_AIE(ctrl->aie, HQV_MODE_MANUAL, HQV_HUE_MODE_OFF, 0, 30);
            break;
        case HQV_CONTROL_DI:
            /*! Arg 1: Valid values: HQV_DI_MODE_OFF, HQV_DI_MODE_VIDEO_1F, HQV_DI_MODE_VIDEO_3F, HQV_DI_MODE_AUTO */
            SET_CTRL_DI(ctrl->di, HQV_DI_MODE_AUTO);
            break;
        case HQV_CONTROL_GLOBAL_DEMO:
            /*! Arg 1: Range: 0 - 100 */
            /*! Arg 2: HQV_SPLIT_LEFT_TO_RIGHT, HQV_SPLIT_RIGHT_TO_LEFT,
            HQV_SPLIT_TOP_TO_BOTTOM, HQV_SPLIT_BOTTOM_TO_TOP, HQV_SPLIT_MAX */
            SET_CTRL_DEMO(ctrl->demo, 100, HQV_SPLIT_LEFT_TO_RIGHT);
            break;
        default:
            ctrl->ctrl_type = HQV_CONTROL_AIE;
            SET_CTRL_AIE(ctrl->aie, HQV_MODE_MANUAL, HQV_HUE_MODE_OFF, 0, 30);
            break;
    }
}

// Generalize the DI Tests using this helper
static void IpHvx_DI_Test(uint32 u32InBufCnt, t_EVppBufType* aeInBufSeq,
                          uint32* au32InBufFlags, uint32* au32FillSeq)
{
    uint32_t u32, i, buf_fill;
    struct hqv_control ctrl;

    SET_FILENAMES(ROTATETEST_INNAME, ROTATETEST_DI_OUTNAME, MAX_FILE_SEG_LEN);

    // Check config error
    DVP_ASSERT_TRUE(stCtxHvx.u32ExpOBD - stCtxHvx.u32DrainOBD >= 0);

    // 1. Run u32VppIpHvx_SetCtrl (see inside set_hqv_control for algo settings)
    set_default_hqv_control(&ctrl, HQV_MODE_MANUAL, HQV_CONTROL_DI);
    u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
    DVP_ASSERT_TRUE(u32 == VPP_OK);

    // 2. Running init_buf_pool
    uint32_t u32OutBufCnt, u32BufTotal;
    u32OutBufCnt = u32InBufCnt * 2; // 2 for every interleaved frame
    u32BufTotal = u32InBufCnt + u32OutBufCnt;
    u32 = init_buf_pool(&stTestCtx.stVppCtx, &stTestCtx.buf_pool,
                        &stTestCtx.params, u32BufTotal, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 3. Running u32VppIpHvx_Open
    u32 = u32VppIpHvx_Open(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 4. Running queue_single_buf
    for (i = 0; i < u32InBufCnt; i++)
    {
        buf_fill = (au32FillSeq == NULL) ? VPP_TRUE : au32FillSeq[i];
        queue_single_buf(VPP_PORT_INPUT, aeInBufSeq[i], au32InBufFlags[i],
                         buf_fill, i);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
    }

    // 5. Wait for the buffers to complete processing before continuing
    pthread_mutex_lock(&pstTestCtx->mutex);
    while (pstTestCtx->u32OutRxCnt < stCtxHvx.u32ExpOBD - stCtxHvx.u32DrainOBD)
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);

    // 6. Call Drain if requested
    if (stCtxHvx.u32DrainOBD)
    {
        u32 = u32VppIpHvx_Drain(stCtxHvx.pstHvxCb);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        // Continue once drain completed
        while (!(stTestCtx.u32Flags & DRAIN_DONE))
            pthread_cond_wait(&stTestCtx.cond, &stTestCtx.mutex);
        pthread_mutex_unlock(&stTestCtx.mutex);
        stTestCtx.u32Flags &= ~DRAIN_DONE;
    }

    // 7. Run u32VppIpHvx_Flush for both ports
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 8. Run u32VppIpHvx_Close
    u32 = u32VppIpHvx_Close(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 9. Running free_buf_pool
    free_buf_pool(stTestCtx.buf_pool, VPP_FALSE);
}

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(IpHvxSuiteInit)
{
    init_test_registry(TEST_SUITE_VPP_CLIENT);
}

TEST_SUITE_TERM(IpHvxSuiteTerm)
{
    term_test_registry();
}

TEST_SETUP(IpHvxTestInit)
{
    memset(&stCtxHvx, 0, sizeof(struct StHvxTestCtx));
    // Set default obd callback to use
    stCtxHvx.pfnc_obd_cust_check = &obd_buf_check;
    uint32_t u32Ret;
    u32Ret = tctx_common_init(pstTestCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Set the callbacks
    stTestCtx.cb.input_buffer_done = test_hvx_input_buffer_done;
    stTestCtx.cb.output_buffer_done = test_hvx_output_buffer_done;
    stTestCtx.cb.event = test_hvx_event;
    stTestCtx.cb.pv = &stTestCtx;

    tctx_set_port_params(&stTestCtx, 1920, 1080, VPP_COLOR_FORMAT_NV12_VENUS);

    u32Ret = u32VppIon_Init(&stTestCtx.stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    stCtxHvx.pstHvxCb = vpVppIpHvx_Init(&stTestCtx.stVppCtx, 0, stTestCtx.cb);
    DVP_ASSERT_PTR_NNULL(stCtxHvx.pstHvxCb);

    u32Ret = u32VppIpHvx_SetParam(stCtxHvx.pstHvxCb, VPP_PORT_INPUT,
                                  stTestCtx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppIpHvx_SetParam(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT,
                                  stTestCtx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    buf_params_init_default(&stTestCtx.params, &stTestCtx.port_param);
}

TEST_CLEANUP(IpHvxTestTerm)
{
    uint32_t u32Ret;

    vVppIpHvx_Term(stCtxHvx.pstHvxCb);

    u32Ret = u32VppIon_Term(&stTestCtx.stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    tctx_common_destroy(&stTestCtx);
}

TEST(IpHvx_SetCustomCtrlBasic)
{
    //Test t_StCustomNrParams
    t_StVppIpHvxCb *pstHvxCb =(t_StVppIpHvxCb *)stCtxHvx.pstHvxCb;
    char *cLocal = (char *)&pstHvxCb->stLocalHvxParams.stCustomNrParams;
    char *cGlobal = (char *)pstHvxCb->pstGlobalHvxParams;
    char *cParams = NULL;
    size_t ParamSize = sizeof(t_StCustomNrParams);
    uint32_t u32ParamSubId = 0;
    char cGlobalOrig[sizeof(t_StCustomHvxParams)];

    memcpy(cGlobalOrig, cGlobal, sizeof(t_StCustomHvxParams));
    cParams = (char *)&pstHvxCb->pstGlobalHvxParams->stCustomNrParams;
    s32ParamTest(cLocal, cParams, HVX_CUSTOM_SET_NR_PARAM, 0, ParamSize, (char *)"CustomNrParams");

    //test stCustomAieParams.stAieDe
    cLocal = (char *)&pstHvxCb->stLocalHvxParams.stCustomAieParams.stAieDe;
    cParams = (char *)&pstHvxCb->pstGlobalHvxParams->stCustomAieParams.stAieDe;
    ParamSize = sizeof(t_StCustomAieDe);
    u32ParamSubId= HVX_CUSTOM_SET_AIE_PARAM_ID_DE;
    s32ParamTest(cLocal, cParams, HVX_CUSTOM_SET_AIE_PARAM, u32ParamSubId, ParamSize,
                 (char *)"CustomAieDe");

    //test stCustomAieParams.stAieLtmCa
    cLocal = (char *)&pstHvxCb->stLocalHvxParams.stCustomAieParams.stAieLtmCa;
    cParams = (char *)&pstHvxCb->pstGlobalHvxParams->stCustomAieParams.stAieLtmCa;
    ParamSize = sizeof(t_StCustomAieLtmCa);
    u32ParamSubId = HVX_CUSTOM_SET_AIE_PARAM_ID_LTMCA;
    s32ParamTest(cLocal, cParams, HVX_CUSTOM_SET_AIE_PARAM, u32ParamSubId, ParamSize,
                 (char *)"CustomAieLtmCa");

    //test stCustomAieParams.stAieLtm
    cLocal = (char *)&pstHvxCb->stLocalHvxParams.stCustomAieParams.stAieLtm;
    cParams = (char *)&pstHvxCb->pstGlobalHvxParams->stCustomAieParams.stAieLtm;
    ParamSize = sizeof(t_StCustomAieLtm);
    u32ParamSubId = HVX_CUSTOM_SET_AIE_PARAM_ID_LTM;
    s32ParamTest(cLocal, cParams, HVX_CUSTOM_SET_AIE_PARAM, u32ParamSubId, ParamSize,
                 (char *)"CustomAieLtm");

    //test stCustomAieParams.stAieAce
    cLocal = (char *)&pstHvxCb->stLocalHvxParams.stCustomAieParams.stAieAce;
    cParams = (char *)&pstHvxCb->pstGlobalHvxParams->stCustomAieParams.stAieAce;
    ParamSize = sizeof(t_StCustomAieAce);
    u32ParamSubId = HVX_CUSTOM_SET_AIE_PARAM_ID_ACE;
    s32ParamTest(cLocal, cParams, HVX_CUSTOM_SET_AIE_PARAM, u32ParamSubId, ParamSize,
                 (char *)"CustomAieAce");

    //test HVX_CUSTOM_SET_LUT_CNR_AUTO
    usleep(10000);
    cLocal = (char *)&pstHvxCb->stLocalHvxParams.stLutCnrAuto;
    cParams = (char *)&pstHvxCb->pstGlobalHvxParams->stLutCnrAuto;
    ParamSize = sizeof(t_StLutNr);
    u32ParamSubId = 0;
    s32ParamTest(cLocal, cParams, HVX_CUSTOM_SET_LUT_CNR_AUTO, u32ParamSubId, ParamSize,
                 (char *)"LutCnrAuto");

    //test HVX_CUSTOM_SET_LUT_CNR_MANUAL
    usleep(10000);
    for (u32ParamSubId = 0; u32ParamSubId < 11; u32ParamSubId++)
    {
        cLocal = (char *)&pstHvxCb->stLocalHvxParams.stLutCnrManual[u32ParamSubId];
        cParams = (char *)&pstHvxCb->pstGlobalHvxParams->stLutCnrManual[u32ParamSubId];
        ParamSize = sizeof(t_StLutNr);
        s32ParamTest(cLocal, cParams, HVX_CUSTOM_SET_LUT_CNR_MANUAL, u32ParamSubId, ParamSize,
                     (char *)"LutCnrManual");
    }

    //test HVX_CUSTOM_SET_LUT_TNR_AUTO
    usleep(10000);
    cLocal = (char *)&pstHvxCb->stLocalHvxParams.stLutTnrAuto;
    cParams = (char *)&pstHvxCb->pstGlobalHvxParams->stLutTnrAuto;
    ParamSize = sizeof(t_StLutNr);
    u32ParamSubId = 0;
    s32ParamTest(cLocal, cParams, HVX_CUSTOM_SET_LUT_TNR_AUTO, u32ParamSubId, ParamSize,
                 (char *)"LutTnrAuto");

    //test HVX_CUSTOM_SET_LUT_TNR_MANUAL
    usleep(10000);
    for (u32ParamSubId = 0; u32ParamSubId < 11; u32ParamSubId++)
    {
        cLocal = (char *)&pstHvxCb->stLocalHvxParams.stLutTnrManual[u32ParamSubId];
        cParams = (char *)&pstHvxCb->pstGlobalHvxParams->stLutTnrManual[u32ParamSubId];
        ParamSize = sizeof(t_StLutNr);
        s32ParamTest(cLocal, cParams, HVX_CUSTOM_SET_LUT_TNR_MANUAL, u32ParamSubId, ParamSize,
                     (char *)"LutTnrManual");
    }

    memcpy(cGlobal, cGlobalOrig, sizeof(t_StCustomHvxParams));
}

// To test if global or local parameters changed for one session,
// it will affect or not affect that of another running sessions
// Only pstGlobalHvxParams->stCustomNrParams used for the test.
// It also tests pstHvxCb->stHvxParams changed or not changed

TEST(IpHvx_SetCustomCtrlTwoSessions)
{
    uint32_t u32;
    struct hqv_control ctrl;
    t_StVppCtx stVppCtx;

    //one session already started
    t_StVppIpHvxCb *pstHvxCb1 = (t_StVppIpHvxCb *)stCtxHvx.pstHvxCb;

    //Start another session session2
    memset(&stVppCtx, 0, sizeof(t_StVppCtx));
    t_StVppIpHvxCb *pstHvxCb2 = vpVppIpHvx_Init(&stVppCtx, 0, stTestCtx.cb);

    if (pstHvxCb1 == NULL)
    {
        DVP_ASSERT_PTR_NNULL(pstHvxCb1);
        return;
    }
    LOGD("pstHvxCb1=%p", pstHvxCb1);

    if (pstHvxCb2 == NULL)
    {
        DVP_ASSERT_PTR_NNULL(pstHvxCb2);
        return;
    }
    LOGD("pstHvxCb2=%p", pstHvxCb2);

    char *cGlobal = (char *)pstHvxCb1->pstGlobalHvxParams;
    char cGlobalOrig[sizeof(t_StCustomHvxParams)];

    u32 = u32VppIpHvx_SetParam(pstHvxCb2, VPP_PORT_INPUT,
                               stTestCtx.port_param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHvx_SetParam(pstHvxCb2, VPP_PORT_OUTPUT,
                               stTestCtx.port_param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memcpy(cGlobalOrig, cGlobal, sizeof(t_StCustomHvxParams));
    uint32_t fGlobalLocal = 0;//0,global;1:local
    t_StCustomNrParams stCustomNrParamsOld;

    //set new parameter with value
    t_StCustomNrParams stCustomNrParamsNew = {
        .fb_mode = 2,
        .ChromaSpFact = 26,
        .SpatialNRFact = 4,
        .NPcurveEn = 2,
        .DEen = 1,
        .DEgain = 1,
        .DenhMin = 3,
        .DenhMax = 22
    };

    //save session1 old local parameters
    memcpy((void*) &stCustomNrParamsOld, (void*) &pstHvxCb1->stLocalHvxParams.stCustomNrParams,
           sizeof(t_StCustomNrParams));

    //Test both global (fGlobalLocal==0) and local (fGlobalLocal==1)
    int cnt;
    for (cnt = 0; cnt < 2; cnt++)
    {
        //first do local (fGlobalLocal = 1) then do global (fGlobalLocal = 0)
        if (cnt == 0)
            fGlobalLocal = 1;
        else
            fGlobalLocal = 0 ;
        LOGD("%s(): Test %s parameter setting.",__func__,fGlobalLocal==0 ? "global" : "local");
        t_StCustomHvxParams *pstCustomHvxParams;
        if (fGlobalLocal)
            pstCustomHvxParams = &pstHvxCb2->stLocalHvxParams;
        else
            pstCustomHvxParams = pstHvxCb2->pstGlobalHvxParams;

        // Set to manual and custom
        ctrl.mode = HQV_MODE_MANUAL;
        ctrl.ctrl_type = HQV_CONTROL_CUST;
        ctrl.custom.id = VPP_IP_HVX;
        ctrl.custom.len = sizeof(t_StCustomNrParams) + HVX_CUSTOM_HEADER_SIZE;//header length is 4
        char *cPtr = (char*)ctrl.custom.data;
        *cPtr++ = sizeof(t_StCustomNrParams) + HVX_CUSTOM_HEADER_SIZE; //lenth low 8-bit
        *cPtr++ = 0;// length hi 8-bit
        uint32_t tmp = fGlobalLocal;
        tmp |= HVX_CUSTOM_SET_NR_PARAM << HVX_GLOBAL_LOCAL_BIT_LENGTH;
        uint32_t u32ParamSubId = 0 ;
        tmp |= u32ParamSubId << (HVX_PARAMID_BIT_LENGTH + HVX_GLOBAL_LOCAL_BIT_LENGTH);
        *cPtr++ = tmp & 0xff;       //ID low 8-bit
        *cPtr++ = (tmp >> 8) & 0xff;  //ID hi 8-bit

        memcpy((void*) cPtr, (void*) &stCustomNrParamsNew, sizeof(t_StCustomNrParams));
        u32 = u32VppIpHvx_SetCtrl(pstHvxCb2, ctrl);
        LOGD("u32VppIpHvx_SetCtrl custom,return = %d &pstCustomHvxParams->stCustomNrParams=%p",
             u32, (char*)&pstCustomHvxParams->stCustomNrParams);
        DVP_ASSERT_TRUE(u32 == VPP_OK);
        u32 = u32CheckMem((char*) cPtr, (char*) &pstCustomHvxParams->stCustomNrParams,
                          sizeof(t_StCustomNrParams));
        DVP_ASSERT_TRUE(u32 == VPP_OK);

        char *ptr = (char*) &pstCustomHvxParams->stCustomNrParams;
        LOGD("%s() After setting vpp_set_ctrl(),pstCustomHvxParams=%p  0x%x %x %x %x",
             __func__,ptr, ptr[0],ptr[1],ptr[2],ptr[3]);

        //No matter Global or local, session2 local should be changed to new param
        t_StHvxParam *pstHvxParams = &pstHvxCb2->stHvxParams;
        DVP_ASSERT_TRUE(pstHvxParams->stNrParams.fb_mode == stCustomNrParamsNew.fb_mode);
        DVP_ASSERT_TRUE(pstHvxParams->stNrParams.ChromaSpFact == stCustomNrParamsNew.ChromaSpFact);
        DVP_ASSERT_TRUE(pstHvxParams->stNrParams.SpatialNRFact == stCustomNrParamsNew.SpatialNRFact);
        DVP_ASSERT_TRUE(pstHvxParams->stNrParams.NPcurveEn == stCustomNrParamsNew.NPcurveEn);
        DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DEen == stCustomNrParamsNew.DEen);
        DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DEgain == stCustomNrParamsNew.DEgain);
        DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DenhMin == stCustomNrParamsNew.DenhMin);
        DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DenhMax == stCustomNrParamsNew.DenhMax);

        if (fGlobalLocal == 0)//Global
        {
            //check session2 global same as local
            char *ptr1 = (char*) &pstHvxCb2->stLocalHvxParams.stCustomNrParams;
            LOGD("%s(),After global parameters setting, local stCustomNrParams=%p  0x%x %x %x %x",
                 __func__,ptr1, ptr1[0],ptr1[1],ptr1[2],ptr1[3]);
            char *ptr = (char*) &pstHvxCb2->pstGlobalHvxParams->stCustomNrParams;
            LOGD("%s(),After global parameters setting, global stCustomNrParams=%p  0x%x %x %x %x",
                 __func__,ptr, ptr[0],ptr[1],ptr[2],ptr[3]);
            u32 = u32CheckMem((char*)ptr1, ptr, sizeof(t_StCustomNrParams));
            DVP_ASSERT_TRUE(u32 == VPP_OK); //session2 local parameters the same as global for session2

            //Check if the other thread has changed the parameters because one thread has done global setting
            usleep(100000);

            char *cPtr1 = (char*) &pstHvxCb1->stLocalHvxParams.stCustomNrParams;
            char *cPtr2 = (char*) &pstHvxCb1->pstGlobalHvxParams->stCustomNrParams;
            LOGD("%s(), After global parameters setting, the other thread local stCustomNrParams=%p 0x%x %x %x %x",
                 __func__,cPtr1, cPtr1[0],cPtr1[1],cPtr1[2],cPtr1[3]);
            LOGD("%s(), After global parameters setting, the other thread global stCustomNrParams=%p 0x%x %x %x %x",
                 __func__,cPtr2, cPtr2[0],cPtr2[1],cPtr2[2],cPtr2[3]);
            u32 = u32CheckMem((char*)cPtr1, ptr, sizeof(t_StCustomNrParams));
            DVP_ASSERT_TRUE(u32 == VPP_OK); //session1 local parameters the same as global ones of session2
            u32 = u32CheckMem((char*)cPtr2, ptr, sizeof(t_StCustomNrParams));
            DVP_ASSERT_TRUE(u32 == VPP_OK); //session1 global parameters the same as global ones of session2

            //If session2 global param changed, session1 local should be changed to new param
            pstHvxParams = &pstHvxCb1->stHvxParams;
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.fb_mode == stCustomNrParamsNew.fb_mode);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.ChromaSpFact == stCustomNrParamsNew.ChromaSpFact);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.SpatialNRFact == stCustomNrParamsNew.SpatialNRFact);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.NPcurveEn == stCustomNrParamsNew.NPcurveEn);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DEen == stCustomNrParamsNew.DEen);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DEgain == stCustomNrParamsNew.DEgain);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DenhMin == stCustomNrParamsNew.DenhMin);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DenhMax == stCustomNrParamsNew.DenhMax);
        }

        if (fGlobalLocal == 1)//local
        {
            char *ptr1 = (char*) &pstHvxCb2->stLocalHvxParams.stCustomNrParams;
            LOGD("%s(),After local parameters setting, local stCustomNrParams=%p  0x%x %x %x %x",
                 __func__,ptr1, ptr1[0],ptr1[1],ptr1[2],ptr1[3]);
            char *ptr = (char*) &pstHvxCb2->pstGlobalHvxParams->stCustomNrParams;
            LOGD("%s(),After local parameters setting, global stCustomNrParams=%p  0x%x %x %x %x",
                 __func__,ptr, ptr[0],ptr[1],ptr[2],ptr[3]);
            cPtr = (char*) &pstHvxCb2->stLocalHvxParams.stCustomNrParams;
            u32 = u32CheckMem((char*)cPtr, ptr, sizeof(t_StCustomNrParams));
            DVP_ASSERT_TRUE(u32 != VPP_OK); //session2 global parameters not the same as local, if we set local

            //Check if one session session2 changed local parameters the other session session1 will not change
            usleep(10000);

            char *cPtr1 = (char*) &pstHvxCb1->stLocalHvxParams.stCustomNrParams;
            char *cPtr2 = (char*) &pstHvxCb1->pstGlobalHvxParams->stCustomNrParams;
            LOGD("%s(),After local parameters setting, the other thread local stCustomNrParams=%p 0x%x %x %x %x",
                 __func__,cPtr1, cPtr1[0],cPtr1[1],cPtr1[2],cPtr1[3]);
            LOGD("%s(),After local parameters setting, the other thread global stCustomNrParams=%p 0x%x %x %x %x",
                 __func__,cPtr2, cPtr2[0],cPtr2[1],cPtr2[2],cPtr2[3]);
            u32 = u32CheckMem((char*)cPtr1, ptr, sizeof(t_StCustomNrParams));
            DVP_ASSERT_TRUE(u32 == VPP_OK); //session1 local parameters the same as global ones of session2, so not the same as locals of session2
            u32 = u32CheckMem((char*)cPtr2, ptr, sizeof(t_StCustomNrParams));
            DVP_ASSERT_TRUE(u32 == VPP_OK); //session1 global parameters the same as global ones of session2, so not the same as locals of session2

            //If session2 local param changed, session1 local param should not change
            pstHvxParams = &pstHvxCb1->stHvxParams;
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.fb_mode == stCustomNrParamsOld.fb_mode);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.ChromaSpFact == stCustomNrParamsOld.ChromaSpFact);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.SpatialNRFact == stCustomNrParamsOld.SpatialNRFact);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.NPcurveEn == stCustomNrParamsOld.NPcurveEn);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DEen == stCustomNrParamsOld.DEen);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DEgain == stCustomNrParamsOld.DEgain);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DenhMin == stCustomNrParamsOld.DenhMin);
            DVP_ASSERT_TRUE(pstHvxParams->stNrParams.DenhMax == stCustomNrParamsOld.DenhMax);
        }
    }

    usleep(10000);
    memcpy(cGlobal, cGlobalOrig, sizeof(t_StCustomHvxParams));
    vVppIpHvx_Term(pstHvxCb2);
}

TEST(IpHvx_SetCustomCtrlMulti)
{
    //16 in the following arrays means we can set up to 16 parameters in one payload
    //This needs to be changed if we need to test setting of more than 16 parameters one time
    char *pcParam[16];
    uint32_t pfLocal[16];
    uint32_t pu32ParamId[16];
    uint32_t pu32ParamSubId[16];
    size_t pu32ParamSize[16];
    uint32_t fLocal, i;
    t_StCustomHvxParams *pstHvxParams;
    t_StVppIpHvxCb *pstHvxCb = (t_StVppIpHvxCb *) stCtxHvx.pstHvxCb;
    char *cGlobal = (char *)pstHvxCb->pstGlobalHvxParams;
    char cGlobalOrig[sizeof(t_StCustomHvxParams)];

    memcpy(cGlobalOrig, cGlobal, sizeof(t_StCustomHvxParams));
    for (fLocal = 0; fLocal < 2; fLocal++)
    {
        if (fLocal == 0) //global
            pstHvxParams = pstHvxCb->pstGlobalHvxParams;
        else //local
            pstHvxParams = &pstHvxCb->stLocalHvxParams;

        for (i = 0; i < 5; i++)//to test up to 5 parameters in one payload
            pfLocal[i] = fLocal;

        i = 0;
        //"CustomNrParams"
        pcParam[i] = (char *) &pstHvxParams->stCustomNrParams;
        pu32ParamSize[i] = sizeof(t_StCustomNrParams);
        pu32ParamId[i] = HVX_CUSTOM_SET_NR_PARAM;
        pu32ParamSubId[i] = 0;
        i++;

        //"CustomAieDe"
        pcParam[i] = (char *) &pstHvxParams->stCustomAieParams.stAieDe;
        pu32ParamSize[i] = sizeof(t_StCustomAieDe);
        pu32ParamId[i] = HVX_CUSTOM_SET_AIE_PARAM;
        pu32ParamSubId[i] = HVX_CUSTOM_SET_AIE_PARAM_ID_DE;
        i++;

        //"CustomAieLtmCa"
        pcParam[i] = (char *) &pstHvxParams->stCustomAieParams.stAieLtmCa;
        pu32ParamSize[i] = sizeof(t_StCustomAieLtmCa);
        pu32ParamId[i] = HVX_CUSTOM_SET_AIE_PARAM;
        pu32ParamSubId[i] = HVX_CUSTOM_SET_AIE_PARAM_ID_LTMCA;
        i++;

        //"CustomAieLtm"
        pcParam[i] = (char *) &pstHvxParams->stCustomAieParams.stAieLtm;
        pu32ParamSize[i] = sizeof(t_StCustomAieLtm);
        pu32ParamId[i] = HVX_CUSTOM_SET_AIE_PARAM;
        pu32ParamSubId[i] = HVX_CUSTOM_SET_AIE_PARAM_ID_LTM;
        i++;

        //"CustomAieAce"
        pcParam[i] = (char *) &pstHvxParams->stCustomAieParams.stAieAce;
        pu32ParamSize[i] = sizeof(t_StCustomAieAce);
        pu32ParamId[i] = HVX_CUSTOM_SET_AIE_PARAM;
        pu32ParamSubId[i] = HVX_CUSTOM_SET_AIE_PARAM_ID_ACE;
        i++;

        //i=5. Total of 5 parameters in one payload:
        //CustomNrParams, CustomAieDe, CustomAieLtmCa, CustomAieLtm, CustomAieAce
        s32MultiParamTest(pcParam, i, pfLocal, pu32ParamId, pu32ParamSubId, pu32ParamSize);
    }

    //test HVX_CUSTOM_SET_LUT_CNR_AUTO
    usleep(10000);
    for (fLocal = 0; fLocal < 2; fLocal++)
    {
        if (fLocal == 0) //global
            pstHvxParams = pstHvxCb->pstGlobalHvxParams;
        else //local
            pstHvxParams = &pstHvxCb->stLocalHvxParams;

        for (i = 0; i < 5; i++)//to test up to 5 parameters in one payload
            pfLocal[i] = fLocal;

        i=0;
        //"LutCnrAuto"
        pcParam[i] = (char *) &pstHvxParams->stLutCnrAuto;
        pu32ParamSize[i] = sizeof(t_StLutNr); //88
        pu32ParamId[i] = HVX_CUSTOM_SET_LUT_CNR_AUTO;
        pu32ParamSubId[i] = 0;
        i++;

        //"LutCnrManual"
        uint32_t u32ParamSubId;
        for (u32ParamSubId = 0; u32ParamSubId < 1; u32ParamSubId++)
        {
            pcParam[i] = (char *) &pstHvxParams->stLutCnrManual[u32ParamSubId];
            pu32ParamSize[i] = sizeof(t_StLutNr);//88
            pu32ParamId[i] = HVX_CUSTOM_SET_LUT_CNR_MANUAL;
            pu32ParamSubId[i] = u32ParamSubId;
            i++;
        }
        //Set "LutCnrAuto" and stLutCnrManual[0]
        s32MultiParamTest(pcParam, i, pfLocal, pu32ParamId, pu32ParamSubId, pu32ParamSize);

        //"LutCnrManual"
        uint32_t j;
        //sizeof(t_StLutNr) is 88, HQV_CUSTOM_PAYLOAD_SZ is 256,
        //so we can set at most 2 t_StLutNr type paramers for one pay load
        //We need to set 5 times for the 10 t_StLutNr type paramers
        for (j = 1; j < 5; j++)
        {
            i = 0;
            for (u32ParamSubId = 2*j; u32ParamSubId < (2*j+2); u32ParamSubId++)
            {
                pcParam[i] = (char *) &pstHvxParams->stLutCnrManual[u32ParamSubId];
                pu32ParamSize[i] = sizeof(t_StLutNr);
                pu32ParamId[i] = HVX_CUSTOM_SET_LUT_CNR_MANUAL;
                pu32ParamSubId[i] = u32ParamSubId;
                i++;
            }
            s32MultiParamTest(pcParam, i, pfLocal, pu32ParamId, pu32ParamSubId, pu32ParamSize);
        }
    }

    //test HVX_CUSTOM_SET_LUT_TNR_AUTO
    usleep(10000);
    for (fLocal = 0; fLocal < 2; fLocal++)
    {
        if (fLocal == 0) //global
            pstHvxParams = pstHvxCb->pstGlobalHvxParams;
        else //local
            pstHvxParams = &pstHvxCb->stLocalHvxParams;

        for (i = 0; i < 5; i++)//to test up to 5 parameters in one payload
            pfLocal[i] = fLocal;

        i = 0;
        //"LutTnrAuto"
        pcParam[i] = (char *)&pstHvxParams->stLutTnrAuto;
        pu32ParamSize[i] = sizeof(t_StLutNr);
        pu32ParamId[i] = HVX_CUSTOM_SET_LUT_TNR_AUTO;
        pu32ParamSubId[i] = 0;
        i++;

        //"LutTnrManual"
        uint32_t u32ParamSubId;
        for (u32ParamSubId = 0; u32ParamSubId < 1; u32ParamSubId++)
        {
            pcParam[i] = (char *) &pstHvxParams->stLutTnrManual[u32ParamSubId];
            pu32ParamSize[i] = sizeof(t_StLutNr);
            pu32ParamId[i] = HVX_CUSTOM_SET_LUT_TNR_MANUAL;
            pu32ParamSubId[i] = u32ParamSubId;
            i++;
        }
        s32MultiParamTest(pcParam, i, pfLocal, pu32ParamId, pu32ParamSubId, pu32ParamSize);

        //"LutTnrManual"
        uint32_t j;
        //sizeof(t_StLutNr) is 88, HQV_CUSTOM_PAYLOAD_SZ is 256,
        //so we can set at most 2 t_StLutNr type paramers for one pay load
        //We need to set 5 times for the 10 t_StLutNr type paramers
        for (j = 1; j < 5; j++)
        {
            i = 0;
            for (u32ParamSubId = 2*j; u32ParamSubId < (2*j+2); u32ParamSubId++)
            {
                pcParam[i] = (char *) &pstHvxParams->stLutTnrManual[u32ParamSubId];
                pu32ParamSize[i] = sizeof(t_StLutNr);
                pu32ParamId[i] = HVX_CUSTOM_SET_LUT_TNR_MANUAL;
                pu32ParamSubId[i] = u32ParamSubId;
                i++;
            }
            s32MultiParamTest(pcParam, i, pfLocal, pu32ParamId, pu32ParamSubId, pu32ParamSize);
        }
    }
    memcpy(cGlobal, cGlobalOrig, sizeof(t_StCustomHvxParams));
}

TEST(IpHvx_AIE_BasicTestcase)
{
    uint32_t u32,i;
    struct hqv_control ctrl;
    stCtxHvx.u32BypassExpected = 0;

    SET_FILENAMES(ROTATETEST_INNAME, ROTATETEST_AIE_OUTNAME, MAX_FILE_SEG_LEN);

    // 1. Run u32VppIpHvx_SetCtrl (see inside set_hqv_control for algo settings)
    set_default_hqv_control(&ctrl, HQV_MODE_MANUAL,HQV_CONTROL_AIE);
    u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
    DVP_ASSERT_TRUE(u32 == VPP_OK);

    // 2. Running init_buf_pool
    uint32_t u32InBufCnt, u32OutBufCnt, u32BufTotal;
    u32InBufCnt = 5;
    u32OutBufCnt = u32InBufCnt; // In most cases, equal to u32InBufCnt
    u32BufTotal = u32InBufCnt + u32OutBufCnt;
    u32 = init_buf_pool(&stTestCtx.stVppCtx, &stTestCtx.buf_pool, &stTestCtx.params,
                        u32BufTotal, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 3. Running u32VppIpHvx_Open
    u32 = u32VppIpHvx_Open(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 4. Running queue_single_buf
    for (i = 0; i < u32InBufCnt; i++)
    {
        queue_single_buf(VPP_PORT_INPUT, eVppBufType_Progressive, 0, VPP_TRUE, 0);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
    }

    // 5. Wait for the buffers to complete processing before continuing
    pthread_mutex_lock(&pstTestCtx->mutex);
    while (pstTestCtx->u32OutRxCnt < u32OutBufCnt)
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);

    // 6. Run u32VppIpHvx_Flush for both ports
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 7. Run u32VppIpHvx_Close
    u32 = u32VppIpHvx_Close(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 8. Running free_buf_pool
    free_buf_pool(stTestCtx.buf_pool, VPP_FALSE);
}

TEST(IpHvx_DI_TFF_BasicTestcase)
{
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF};
    uint32 au32InBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_BFF_BasicTestcase)
{
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF};
    uint32 au32InBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_BypassTestcase)
{
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Progressive,
        eVppBufType_Progressive,
        eVppBufType_Progressive,
        eVppBufType_Progressive,
        eVppBufType_Progressive};
    uint32 au32InBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};

    stCtxHvx.u32ExpOBD = 5;
    stCtxHvx.u32BypassExpected = 1;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_FieldMixTestcase)
{
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_TFF};
    uint32 au32InBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_ProgressiveMixTestcase)
{
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Progressive,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF};
    uint32 au32InBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, 0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32BypSeq[] = {0, 0, 0, 0, 1, 0, 0, 0, 0};

    stCtxHvx.u32ExpOBD = 9;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.au32BypSeq = au32BypSeq;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_MultiMixTestcase)
{
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Progressive,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Progressive};
    uint32 au32InBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32BypSeq[] = {0, 0, 0, 0, 1, 0, 0, 1};

    stCtxHvx.u32ExpOBD = 8;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.au32BypSeq = au32BypSeq;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_TFF_FlagPropagationTestcase)
{
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF};
    uint32 au32InBufFlags[] = {0, VPP_BUFFER_FLAG_DATACORRUPT, 0,
        VPP_BUFFER_FLAG_SYNCFRAME, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {VPP_BUFFER_FLAG_DATACORRUPT, 0, 0, 0, 0,
        VPP_BUFFER_FLAG_SYNCFRAME, 0, 0, 0, VPP_BUFFER_FLAG_EOS};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_BFF_FlagPropagationTestcase)
{
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF};
    uint32 au32InBufFlags[] = {0, 0, VPP_BUFFER_FLAG_DATACORRUPT,
        VPP_BUFFER_FLAG_SYNCFRAME, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, VPP_BUFFER_FLAG_DATACORRUPT, 0, 0,
        0, VPP_BUFFER_FLAG_SYNCFRAME, 0, 0, VPP_BUFFER_FLAG_EOS};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_TFF_LoseIDRFlagPropagationTestcase)
{
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF};
    uint32 au32InBufFlags[] = {VPP_BUFFER_FLAG_DATACORRUPT, 0, VPP_BUFFER_FLAG_SYNCFRAME,
        VPP_BUFFER_FLAG_DATACORRUPT, VPP_BUFFER_FLAG_SYNCFRAME | VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {VPP_BUFFER_FLAG_DATACORRUPT, 0, 0, VPP_BUFFER_FLAG_DATACORRUPT,
        0, 0, 0, VPP_BUFFER_FLAG_SYNCFRAME, 0, VPP_BUFFER_FLAG_EOS};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_BFF_LoseIDRFlagPropagationTestcase)
{
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF};
    uint32 au32InBufFlags[] = {VPP_BUFFER_FLAG_DATACORRUPT, 0, VPP_BUFFER_FLAG_SYNCFRAME,
        VPP_BUFFER_FLAG_DATACORRUPT, VPP_BUFFER_FLAG_SYNCFRAME | VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {VPP_BUFFER_FLAG_DATACORRUPT, 0, 0, 0,
        VPP_BUFFER_FLAG_DATACORRUPT, 0, 0, 0, VPP_BUFFER_FLAG_SYNCFRAME, VPP_BUFFER_FLAG_EOS};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_TFF_DrainTestcase)
{
    // For the Drain Testcase we won't use an EOS
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF};
    uint32 au32InBufFlags[] = {VPP_BUFFER_FLAG_DATACORRUPT, 0, VPP_BUFFER_FLAG_SYNCFRAME,
        0, 0};
    uint32 au32OutBufFlags[] = {VPP_BUFFER_FLAG_DATACORRUPT, 0, 0, VPP_BUFFER_FLAG_SYNCFRAME,
        0, 0, 0, 0, 0, 0};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32DrainOBD = 4;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_BFF_DrainTestcase)
{
    // For the Drain Testcase we won't use an EOS
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF};
    uint32 au32InBufFlags[] = {VPP_BUFFER_FLAG_DATACORRUPT, 0, VPP_BUFFER_FLAG_SYNCFRAME,
        0, 0};
    uint32 au32OutBufFlags[] = {VPP_BUFFER_FLAG_DATACORRUPT, 0, 0, 0,
        VPP_BUFFER_FLAG_SYNCFRAME, 0, 0, 0, 0, 0};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32DrainOBD = 4;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_TFF_MidstreamEOSTestcase)
{
    // For the Drain Testcase we won't use an EOS
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF};
    uint32 au32InBufFlags[] = {0, 0, VPP_BUFFER_FLAG_EOS, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, 0, VPP_BUFFER_FLAG_EOS, 0, 0, 0,
        VPP_BUFFER_FLAG_EOS};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_BFF_MidstreamEOSTestcase)
{
    // For the Drain Testcase we won't use an EOS
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF};
    uint32 au32InBufFlags[] = {0, 0, VPP_BUFFER_FLAG_EOS, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, 0, VPP_BUFFER_FLAG_EOS, 0, 0, 0,
        VPP_BUFFER_FLAG_EOS};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_TFF_EmptyEOSTestcase)
{
    // For the Drain Testcase we won't use an EOS
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF};
    uint32 au32InBufFlags[] = {0, 0, VPP_BUFFER_FLAG_EOS, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS, 0, 0, 0,
        VPP_BUFFER_FLAG_EOS};
    uint32 au32FillBufSeq[] = {VPP_TRUE, VPP_TRUE, VPP_FALSE, VPP_TRUE, VPP_TRUE};
    uint32 au32BypSeq[] = {0, 0, 0, 0, 1, 0, 0, 0, 0};

    stCtxHvx.u32ExpOBD = 9;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.au32BypSeq = au32BypSeq;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, au32FillBufSeq);
}

TEST(IpHvx_DI_BFF_EmptyEOSTestcase)
{
    // For the Drain Testcase we won't use an EOS
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF};
    uint32 au32InBufFlags[] = {0, 0, VPP_BUFFER_FLAG_EOS, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS, 0, 0, 0,
        VPP_BUFFER_FLAG_EOS};
    uint32 au32FillBufSeq[] = {VPP_TRUE, VPP_TRUE, VPP_FALSE, VPP_TRUE, VPP_TRUE};
    uint32 au32BypSeq[] = {0, 0, 0, 0, 1, 0, 0, 0, 0};

    stCtxHvx.u32ExpOBD = 9;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.au32BypSeq = au32BypSeq;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, au32FillBufSeq);
}

TEST(IpHvx_DI_TFF_ExtraDataPropagationTestcase)
{
    // For the Drain Testcase we won't use an EOS
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF,
        eVppBufType_Interleaved_TFF};
    uint32 au32InBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32MarkerSeq[] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 4};
    uint32 au32BypSeq[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.au32MarkerSeq = au32MarkerSeq;
    stCtxHvx.au32BypSeq = au32BypSeq;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_DI_BFF_ExtraDataPropagationTestcase)
{
    // For the Drain Testcase we won't use an EOS
    uint32 u32InBufCnt = 5;
    t_EVppBufType aeInBufSeq[] = {
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF,
        eVppBufType_Interleaved_BFF};
    uint32 au32InBufFlags[] = {0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32OutBufFlags[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, VPP_BUFFER_FLAG_EOS};
    uint32 au32MarkerSeq[] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4};
    uint32 au32BypSeq[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    stCtxHvx.u32ExpOBD = 10;
    stCtxHvx.u32BypassExpected = 0;
    stCtxHvx.au32OutFlgSeq = au32OutBufFlags;
    stCtxHvx.au32MarkerSeq = au32MarkerSeq;
    stCtxHvx.au32BypSeq = au32BypSeq;
    stCtxHvx.pfnc_obd_cust_check = &obd_di_buf_check;

    IpHvx_DI_Test(u32InBufCnt, aeInBufSeq, au32InBufFlags, NULL);
}

TEST(IpHvx_Unsupported_BasicTestcase)
{
    uint32_t u32,i;
    struct hqv_control ctrl;
    stCtxHvx.u32BypassExpected = 1;

    SET_FILENAMES(ROTATETEST_INNAME, ROTATETEST_INVAL_OUTNAME, MAX_FILE_SEG_LEN);

    // 1. Run u32VppIpHvx_SetCtrl (see inside set_hqv_control for algo settings)
    set_default_hqv_control(&ctrl,HQV_MODE_MANUAL,HQV_CONTROL_CADE);
    u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
    DVP_ASSERT_TRUE(u32 != VPP_OK);

    // 2. Running init_buf_pool
    uint32_t u32InBufCnt, u32OutBufCnt, u32BufTotal;
    u32InBufCnt = 5;
    u32OutBufCnt = u32InBufCnt; // In most cases, equal to u32InBufCnt
    u32BufTotal = u32InBufCnt + u32OutBufCnt;
    u32 = init_buf_pool(&stTestCtx.stVppCtx, &stTestCtx.buf_pool, &stTestCtx.params,
                        u32BufTotal, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 3. Running u32VppIpHvx_Open
    u32 = u32VppIpHvx_Open(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 4. Running queue_single_buf
    for (i = 0; i < u32InBufCnt; i++)
    {
        queue_single_buf(VPP_PORT_INPUT, eVppBufType_Progressive, 0, VPP_TRUE, 0);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
    }

    // 5. Wait for the buffers to complete processing before continuing
    pthread_mutex_lock(&pstTestCtx->mutex);
    while (pstTestCtx->u32OutRxCnt < u32OutBufCnt)
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);

    // 6. Run u32VppIpHvx_Flush for both ports
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 7. Run u32VppIpHvx_Close
    u32 = u32VppIpHvx_Close(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 8. Running free_buf_pool
    free_buf_pool(stTestCtx.buf_pool, VPP_FALSE);
}

TEST(IpHvx_CNR_BasicTestcase)
{
    uint32_t u32,i;
    struct hqv_control ctrl;
    stCtxHvx.u32BypassExpected = 0;

    SET_FILENAMES(ROTATETEST_INNAME, ROTATETEST_CNR_OUTNAME, MAX_FILE_SEG_LEN);

    // 1. Run u32VppIpHvx_SetCtrl (see inside set_hqv_control for algo settings)
    set_default_hqv_control(&ctrl,HQV_MODE_MANUAL,HQV_CONTROL_CNR);
    u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
    DVP_ASSERT_TRUE(u32 == VPP_OK);

    // 2. Running init_buf_pool
    uint32_t u32InBufCnt, u32OutBufCnt, u32BufTotal;
    u32InBufCnt = 5;
    u32OutBufCnt = u32InBufCnt; // In most cases, equal to u32InBufCnt
    u32BufTotal = u32InBufCnt + u32OutBufCnt;
    u32 = init_buf_pool(&stTestCtx.stVppCtx, &stTestCtx.buf_pool, &stTestCtx.params,
                        u32BufTotal, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 3. Running u32VppIpHvx_Open
    u32 = u32VppIpHvx_Open(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 4. Running queue_single_buf
    for (i = 0; i < u32InBufCnt; i++)
    {
        queue_single_buf(VPP_PORT_INPUT, eVppBufType_Progressive, 0, VPP_TRUE, 0);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
    }

    // 5. Wait for the buffers to complete processing before continuing
    pthread_mutex_lock(&pstTestCtx->mutex);
    while (pstTestCtx->u32OutRxCnt < u32OutBufCnt)
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);

    // 6. Run u32VppIpHvx_Flush for both ports
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 7. Run u32VppIpHvx_Close
    u32 = u32VppIpHvx_Close(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 8. Running free_buf_pool
    free_buf_pool(stTestCtx.buf_pool, VPP_FALSE);
}

TEST(IpHvx_AUTO_BasicTestcase)
{
    uint32_t u32,i;
    struct hqv_control ctrl;
    stCtxHvx.u32BypassExpected = 0;

    SET_FILENAMES(ROTATETEST_INNAME, ROTATETEST_DEFAULT_OUTNAME, MAX_FILE_SEG_LEN);

    // 1. Run u32VppIpHvx_SetCtrl (see inside set_hqv_control for algo settings)
    set_default_hqv_control(&ctrl,HQV_MODE_AUTO,HQV_CONTROL_NONE);
    u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
    DVP_ASSERT_TRUE(u32 == VPP_OK);

    // 2. Running init_buf_pool
    uint32_t u32InBufCnt, u32OutBufCnt, u32BufTotal;
    u32InBufCnt = 5;
    u32OutBufCnt = u32InBufCnt; // In most cases, equal to u32InBufCnt
    u32BufTotal = u32InBufCnt + u32OutBufCnt;
    u32 = init_buf_pool(&stTestCtx.stVppCtx, &stTestCtx.buf_pool, &stTestCtx.params,
                        u32BufTotal, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 3. Running u32VppIpHvx_Open
    u32 = u32VppIpHvx_Open(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 4. Running queue_single_buf
    for (i = 0; i < u32InBufCnt; i++)
    {
        queue_single_buf(VPP_PORT_INPUT, eVppBufType_Progressive, 0, VPP_TRUE, 0);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
    }

    // 5. Wait for the buffers to complete processing before continuing
    pthread_mutex_lock(&pstTestCtx->mutex);
    while (pstTestCtx->u32OutRxCnt < u32OutBufCnt)
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);

    // 6. Run u32VppIpHvx_Flush for both ports
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 7. Run u32VppIpHvx_Close
    u32 = u32VppIpHvx_Close(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 8. Running free_buf_pool
    free_buf_pool(stTestCtx.buf_pool, VPP_FALSE);
}

TEST(IpHvx_MultiAlgo_BasicTestcase)
{
    uint32_t u32,i;
    struct hqv_control ctrl;
    stCtxHvx.u32BypassExpected = 0;

    SET_FILENAMES(ROTATETEST_INNAME, ROTATETEST_CNR_AIE_OUTNAME, MAX_FILE_SEG_LEN);

    // 1. Run u32VppIpHvx_SetCtrl (see inside set_hqv_control for algo settings)
    set_default_hqv_control(&ctrl,HQV_MODE_MANUAL,HQV_CONTROL_CNR);
    u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
    DVP_ASSERT_TRUE(u32 == VPP_OK);
    set_default_hqv_control(&ctrl,HQV_MODE_MANUAL,HQV_CONTROL_AIE);
    u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
    DVP_ASSERT_TRUE(u32 == VPP_OK);

    // 2. Running init_buf_pool
    uint32_t u32InBufCnt, u32OutBufCnt, u32BufTotal;
    u32InBufCnt = 5;
    u32OutBufCnt = u32InBufCnt; // In most cases, equal to u32InBufCnt
    u32BufTotal = u32InBufCnt + u32OutBufCnt;
    u32 = init_buf_pool(&stTestCtx.stVppCtx, &stTestCtx.buf_pool, &stTestCtx.params,
                        u32BufTotal, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 3. Running u32VppIpHvx_Open
    u32 = u32VppIpHvx_Open(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 4. Running queue_single_buf
    for (i = 0; i < u32InBufCnt; i++)
    {
        queue_single_buf(VPP_PORT_INPUT, eVppBufType_Progressive, 0, VPP_TRUE, 0);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
    }

    // 5. Wait for the buffers to complete processing before continuing
    pthread_mutex_lock(&pstTestCtx->mutex);
    while (pstTestCtx->u32OutRxCnt < u32OutBufCnt)
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);

    // 6. Run u32VppIpHvx_Flush for both ports
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 7. Run u32VppIpHvx_Close
    u32 = u32VppIpHvx_Close(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 8. Running free_buf_pool
    free_buf_pool(stTestCtx.buf_pool, VPP_FALSE);
}

TEST(IpHvx_ValidReconfig_BasicTestcase)
{
    uint32_t u32,i;
    struct hqv_control ctrl;
    stCtxHvx.u32BypassExpected = 0;

    // --- Original run ---
    SET_FILENAMES(ROTATETEST_INNAME, ROTATETEST_AIE_VALRECONFIG_OUTNAME, MAX_FILE_SEG_LEN);

    // 1. Run u32VppIpHvx_SetCtrl (see inside set_hqv_control for algo settings)
    set_default_hqv_control(&ctrl,HQV_MODE_MANUAL,HQV_CONTROL_AIE);
    u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
    DVP_ASSERT_TRUE(u32 == VPP_OK);

    // 2. Running init_buf_pool
    uint32_t u32InBufCnt, u32OutBufCnt, u32BufTotal;
    u32InBufCnt = 2;
    u32OutBufCnt = u32InBufCnt; // In most cases, equal to u32InBufCnt
    u32BufTotal = u32InBufCnt + u32OutBufCnt;
    u32 = init_buf_pool(&stTestCtx.stVppCtx, &stTestCtx.buf_pool, &stTestCtx.params,
                        u32BufTotal, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 3. Running u32VppIpHvx_Open
    u32 = u32VppIpHvx_Open(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 4. Running queue_single_buf
    for (i = 0; i < u32InBufCnt; i++)
    {
        queue_single_buf(VPP_PORT_INPUT, eVppBufType_Progressive, 0, VPP_TRUE, 0);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
    }

    // 5. Running u32VppIpHvx_Drain
    u32 = u32VppIpHvx_Drain(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    // Continue once drain completed
    while (!(stTestCtx.u32Flags & DRAIN_DONE))
        pthread_cond_wait(&stTestCtx.cond, &stTestCtx.mutex);
    pthread_mutex_unlock(&stTestCtx.mutex);
    stTestCtx.u32Flags &= ~DRAIN_DONE;

    // 6. Run u32VppIpHvx_Close
    u32 = u32VppIpHvx_Close(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 7. Running free_buf_pool
    free_buf_pool(stTestCtx.buf_pool, VPP_FALSE);

    // --- Reconfigure ---
    // 1. Change the parameters
    SET_FILENAMES(ROTATETEST2_INNAME, ROTATETEST2_AIEVALID_OUTNAME, MAX_FILE_SEG_LEN);
    tctx_set_port_params(pstTestCtx, 720, 480, VPP_COLOR_FORMAT_NV12_VENUS);
    u32InBufCnt = 2;
    u32OutBufCnt = u32InBufCnt; // In most cases, equal to u32InBufCnt
    u32BufTotal = u32InBufCnt + u32OutBufCnt;
    u32 = init_buf_pool(&stTestCtx.stVppCtx, &stTestCtx.buf_pool, &stTestCtx.params,
                        u32BufTotal, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHvx_SetParam(stCtxHvx.pstHvxCb, VPP_PORT_INPUT,
                               stTestCtx.port_param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHvx_SetParam(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT,
                               stTestCtx.port_param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 2. Running u32VppIpHvx_Open
    u32 = u32VppIpHvx_Open(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 3. Continue queue_single_buf
    for (i = 0; i < u32InBufCnt; i++)
    {
        queue_single_buf(VPP_PORT_INPUT, eVppBufType_Progressive, 0, VPP_TRUE, 0);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
    }

    // 4. Wait for the buffers to complete processing before continuing
    pthread_mutex_lock(&pstTestCtx->mutex);
    while (pstTestCtx->u32OutRxCnt < u32OutBufCnt)
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);

    // 5. Run u32VppIpHvx_Flush for both ports
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 6. Run u32VppIpHvx_Close
    u32 = u32VppIpHvx_Close(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 7. Running free_buf_pool
    free_buf_pool(stTestCtx.buf_pool, VPP_FALSE);
}

TEST(IpHvx_MemTooSmallReconfig_BasicTestcase)
{
    uint32_t u32,i;
    struct hqv_control ctrl;
    stCtxHvx.u32BypassExpected = 1;

    // --- Original run ---
    SET_FILENAMES(ROTATETEST_INNAME, ROTATETEST_AIE_INVALRECONFIG_OUTNAME, MAX_FILE_SEG_LEN);

    // 1. Run u32VppIpHvx_SetCtrl (see inside set_hqv_control for algo settings)
    set_default_hqv_control(&ctrl,HQV_MODE_MANUAL,HQV_CONTROL_AIE);
    u32 = u32VppIpHvx_SetCtrl(stCtxHvx.pstHvxCb, ctrl);
    DVP_ASSERT_TRUE(u32 == VPP_OK);

    // 2. Running init_buf_pool
    uint32_t u32InBufCnt, u32OutBufCnt, u32BufTotal;
    u32InBufCnt = 2;
    u32OutBufCnt = u32InBufCnt; // In most cases, equal to u32InBufCnt
    u32BufTotal = u32InBufCnt + u32OutBufCnt;
    u32 = init_buf_pool(&stTestCtx.stVppCtx, &stTestCtx.buf_pool, &stTestCtx.params,
                        u32BufTotal, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 3. Running u32VppIpHvx_Open
    u32 = u32VppIpHvx_Open(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 4. Running queue_single_buf
    for (i = 0; i < u32InBufCnt; i++)
    {
        queue_single_buf(VPP_PORT_INPUT, eVppBufType_Progressive, 0, VPP_TRUE, 0);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
    }

    // 5. Running u32VppIpHvx_Drain
    u32 = u32VppIpHvx_Drain(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    // Continue once drain completed
    while (!(stTestCtx.u32Flags & DRAIN_DONE))
        pthread_cond_wait(&stTestCtx.cond, &stTestCtx.mutex);
    pthread_mutex_unlock(&stTestCtx.mutex);
    stTestCtx.u32Flags &= ~DRAIN_DONE;

    // 6. Run u32VppIpHvx_Close
    u32 = u32VppIpHvx_Close(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 7. Running free_buf_pool
    free_buf_pool(stTestCtx.buf_pool, VPP_FALSE);

    // --- Reconfigure ---
    // 1. Change the parameters
    SET_FILENAMES(ROTATETEST2_INNAME, ROTATETEST2_AIEINVALID_OUTNAME, MAX_FILE_SEG_LEN);
    // Allocating memory for only 2x2 frames
    SET_BUF_PARAMS(2,2,VPP_COLOR_FORMAT_NV12_VENUS);
    // HVX to process for 720x480 frames
    tctx_set_port_params(pstTestCtx, 720, 480, VPP_COLOR_FORMAT_NV12_VENUS);
    u32InBufCnt = 2;
    u32OutBufCnt = u32InBufCnt; // In most cases, equal to u32InBufCnt
    u32BufTotal = u32InBufCnt + u32OutBufCnt;
    u32 = init_buf_pool(&stTestCtx.stVppCtx, &stTestCtx.buf_pool, &stTestCtx.params,
                        u32BufTotal, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHvx_SetParam(stCtxHvx.pstHvxCb, VPP_PORT_INPUT,
                               stTestCtx.port_param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHvx_SetParam(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT,
                               stTestCtx.port_param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 2. Running u32VppIpHvx_Open
    u32 = u32VppIpHvx_Open(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 3. Continue queue_single_buf
    for (i = 0; i < u32InBufCnt; i++)
    {
        queue_single_buf(VPP_PORT_INPUT, eVppBufType_Progressive, 0, VPP_TRUE, 0);
        queue_single_buf(VPP_PORT_OUTPUT, eVppBufType_Max, 0, VPP_TRUE, 0);
    }

    // 4. Wait for the buffers to complete processing before continuing
    pthread_mutex_lock(&pstTestCtx->mutex);
    while (pstTestCtx->u32OutRxCnt < u32OutBufCnt)
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);

    // 5. Run u32VppIpHvx_Flush for both ports
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHvx_Flush(stCtxHvx.pstHvxCb, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 6. Run u32VppIpHvx_Close
    u32 = u32VppIpHvx_Close(stCtxHvx.pstHvxCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // 7. Running free_buf_pool
    free_buf_pool(stTestCtx.buf_pool, VPP_FALSE);
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES IpHvxTests[] = {
    TEST_CASE(IpHvx_SetCustomCtrlBasic),
    TEST_CASE(IpHvx_SetCustomCtrlTwoSessions),
    TEST_CASE(IpHvx_SetCustomCtrlMulti),
    TEST_CASE(IpHvx_CNR_BasicTestcase),
    TEST_CASE(IpHvx_AUTO_BasicTestcase),
    TEST_CASE(IpHvx_AIE_BasicTestcase),
    TEST_CASE(IpHvx_DI_TFF_BasicTestcase),
    TEST_CASE(IpHvx_DI_BFF_BasicTestcase),
    TEST_CASE(IpHvx_DI_BypassTestcase),
    TEST_CASE(IpHvx_DI_FieldMixTestcase),
    TEST_CASE(IpHvx_DI_ProgressiveMixTestcase),
    TEST_CASE(IpHvx_DI_MultiMixTestcase),
    TEST_CASE(IpHvx_DI_TFF_FlagPropagationTestcase),
    TEST_CASE(IpHvx_DI_BFF_FlagPropagationTestcase),
    TEST_CASE(IpHvx_DI_TFF_LoseIDRFlagPropagationTestcase),
    TEST_CASE(IpHvx_DI_BFF_LoseIDRFlagPropagationTestcase),
    TEST_CASE(IpHvx_DI_TFF_DrainTestcase),
    TEST_CASE(IpHvx_DI_BFF_DrainTestcase),
    TEST_CASE(IpHvx_DI_TFF_MidstreamEOSTestcase),
    TEST_CASE(IpHvx_DI_BFF_MidstreamEOSTestcase),
    TEST_CASE(IpHvx_DI_TFF_EmptyEOSTestcase),
    TEST_CASE(IpHvx_DI_BFF_EmptyEOSTestcase),
    TEST_CASE(IpHvx_DI_TFF_ExtraDataPropagationTestcase),
    TEST_CASE(IpHvx_DI_BFF_ExtraDataPropagationTestcase),
    TEST_CASE(IpHvx_Unsupported_BasicTestcase),
    TEST_CASE(IpHvx_MultiAlgo_BasicTestcase),
    TEST_CASE(IpHvx_ValidReconfig_BasicTestcase),
    TEST_CASE(IpHvx_MemTooSmallReconfig_BasicTestcase),
    TEST_CASE_NULL(),
};

TEST_SUITE(IpHvxSuite,
           "IpHvx Tests",
           IpHvxSuiteInit,
           IpHvxSuiteTerm,
           IpHvxTestInit,
           IpHvxTestTerm,
           IpHvxTests);

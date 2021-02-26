/*!
 * @file vpp_ip_hcp_tunings.c
 *
 * @cr
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>

#define VPP_LOG_TAG     VPP_LOG_MODULE_HCP_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_HCP

#include "vpp_dbg.h"
#include "vpp_utils.h"

#include "hcp_hfi.h"
#include "hcp_rpc.h"

#include "vpp_ip_hcp_dbg.h"
#include "vpp_ip_hcp_priv.h"
#include "vpp_ip_hcp_tunings.h"
#include "vpp_ip_hcp_hfi.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

#define MAX_CHAR_PER_ROW 256
#define TUNINGS_FILE_DELIM ","

#define HCP_TUNING(_nm, _type, _count, _min, _max, _val) \
{ \
    .pcId = #_nm, \
    .u32Id = _nm, \
    .u32Count = _count, \
    .eType = TUNING_TYPE_##_type, \
    .min._type = _min, \
    .max._type = _max, \
    .pfVal = _val, \
}

#define GET_SYSTUNE_HEADER_SIZE()       (sizeof(uint32_t))
#define GET_SYSTUNEPARAM_HEADER_SIZE()  (sizeof(uint32_t) * 2)
#define GET_TUNING_ELEMENTS_SIZE(pst)   (sizeof(uint32_t) * pst->pstDef->u32Count)

#define DVP_CODEC_TYPE_MAX      12
#define IS_IN_RANGE(val, min, max)      ((val) >= (min) && (val) <= (max))
#define RET_ERR_IF_U32_OUT_OF_RANGE(pst, idx, min, max) \
    if (!IS_IN_RANGE((pst)->pstVal[(idx)].U32, (min), (max))) \
    { \
        LOGE("Tuning %s is out of range. idx=%u, val=%u, min=%u, max=%u", \
             (pst)->pstDef->pcId, (idx), (pst)->pstVal[(idx)].U32, \
             (min), (max)); \
        return VPP_ERR; \
    }

#define RET_ERR_IF_NULL_TUNING(pst) \
    if (!(pst) || !(pst)->pstDef || !(pst)->pstVal) \
        return VPP_ERR;

#define RET_ERR_IF_WRONG_TUNING(pst, id) \
    if ((pst)->pstDef->u32Id != (id)) \
        return VPP_ERR;

/************************************************************************
 * Forward Declarations
 ************************************************************************/
static uint32_t u32VppIpHcp_Validate_LTMTUNE_ANLTMZONEMAP(t_StHcpTuning *pstTuning);

/************************************************************************
 * Local static variables
 ***********************************************************************/
static const t_StHcpTuningDef astTuningDef[] = {
    // AIE, LTM, DE
    HCP_TUNING(DDTUNE_SAT_THR, U32, 1, 0, 63, NULL),
    HCP_TUNING(LTMTUNE_NLTMZONELEVEL, U32, 1, 0, 1, NULL),
    HCP_TUNING(LTMTUNE_ANLTMZONEMAP, U32, 20, 0, 0,
               u32VppIpHcp_Validate_LTMTUNE_ANLTMZONEMAP),
    HCP_TUNING(LTMTUNE_ANNYQDEGAINFACTOR, FLOAT, 9, 0, 2.00, NULL),
    HCP_TUNING(LTMTUNE_ANEARDEGAINFACTOR, FLOAT, 9, 0, 2.00, NULL),
    HCP_TUNING(LTMTUNE_FDEOFF, FLOAT, 1, -1.00, 7.00, NULL),
    HCP_TUNING(LTMTUNE_NDEMIN, U32, 1, 0, 63, NULL),
    HCP_TUNING(LTMTUNE_NDEMAX, U32, 1, 0, 63, NULL),
    HCP_TUNING(LTMTUNE_NDEPOW1, U32, 1, 0, 8, NULL),
    HCP_TUNING(LTMTUNE_NDEPOW2, U32, 1, 0, 8, NULL),
    HCP_TUNING(LTMTUNE_FHSTTHRDIFF, FLOAT, 1, 0.00, 1.00, NULL),
    HCP_TUNING(LTMTUNE_FLUTCOEFFIIR, FLOAT, 1, 0.00, 1.00, NULL),
    HCP_TUNING(LTMTUNE_NLUTCNTBLEND, U32, 1, 1, 64, NULL),
    HCP_TUNING(LTMTUNE_ANCLPGAP, U32, 8, 0, 127, NULL),
    HCP_TUNING(LTMTUNE_NRNGLIM, U32, 1, 0, 255, NULL),
    HCP_TUNING(LTMTUNE_NHSTLIM, U32, 1, 0, 255, NULL),
    HCP_TUNING(LTMTUNE_NENLUTROUND, U32, 1, 0, 1, NULL),
    HCP_TUNING(LTMTUNE_NUPDLUTWITHAVGHISTO, U32, 1, 0, 1, NULL),
    HCP_TUNING(LTMTUNE_NHSTUPDDEL, U32, 1, 0, 128, NULL),
    HCP_TUNING(LTMTUNE_NLUTUPDDEL, U32, 1, 0, 128, NULL),
    HCP_TUNING(LTMTUNE_FHSTCOEFFIIR, FLOAT, 1, 0.00, 1.00, NULL),
    HCP_TUNING(LTMTUNE_FDECOEFFIIR, FLOAT, 1, 0.00, 1.00, NULL),
    HCP_TUNING(LTMTUNE_FTMCOEFF, FLOAT, 1, 0.00, 10.00, NULL),
    HCP_TUNING(LTMTUNE_NTMORDER, U32, 1, 0, 1, NULL),
    HCP_TUNING(APPTUNE_NLTMMAPEN, U32, 1, 0, 1, NULL),
    HCP_TUNING(APPTUNE_NLTMSATEN, U32, 1, 0, 1, NULL),

    // EAR
    HCP_TUNING(EARTUNE_REG_MAP_H263, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_H264, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_HEVC, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_MPEG1, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_MPEG2, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_MPEG4, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_DIVX_311, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_DIVX, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_VC1, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_SPARK, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_VP8, U32, 3, 0, 7, NULL),
    HCP_TUNING(EARTUNE_REG_MAP_VP9, U32, 3, 0, 7, NULL),

    // Split Screen
    HCP_TUNING(DDTUNE_PPSPLIT_LINE_Y, U32, 1, 0, 15, NULL),
    HCP_TUNING(DDTUNE_PPSPLIT_LINE_CB, U32, 1, 0, 15, NULL),
    HCP_TUNING(DDTUNE_PPSPLIT_LINE_CR, U32, 1, 0, 15, NULL),
    HCP_TUNING(APPTUNE_ANPPSPLITWIDTHMAP, U32, 3, 0, 3, NULL),

    // Dithering
    HCP_TUNING(APPTUNE_NDITMODE, U32, 1, 1, 3, NULL),
};

static const uint32_t su32TuningCnt = sizeof(astTuningDef) / sizeof(t_StHcpTuningDef);

/************************************************************************
 * Local Functions
 ***********************************************************************/

#define IS_ODD(x)   ((x) & 0x1)
static uint32_t u32VppIpHcp_Validate_LTMTUNE_ANLTMZONEMAP(t_StHcpTuning *pstTuning)
{
    uint32_t u32;
    uint32_t u32Min, u32Max;

    RET_ERR_IF_NULL_TUNING(pstTuning);
    RET_ERR_IF_WRONG_TUNING(pstTuning, LTMTUNE_ANLTMZONEMAP);

    for (u32 = 0; u32 < pstTuning->pstDef->u32Count; u32++)
    {
        u32Min = 0;

        if (IS_ODD(u32))
            u32Max = 5;
        else
            u32Max = 7;

        RET_ERR_IF_U32_OUT_OF_RANGE(pstTuning, u32, u32Min, u32Max);
    }

    return VPP_OK;
}

static t_StHcpTuning *pstVppIpHcp_AllocTuning(t_StHcpTuningCb *pstCb,
                                              const t_StHcpTuningDef *pstDef)
{
    if (!pstDef || !pstCb)
        return NULL;

    t_StHcpTuning *pstTuning = calloc(1, sizeof(t_StHcpTuning));
    if (!pstTuning)
        goto err_alloc_top;

    pstTuning->pstDef = pstDef;
    pstTuning->pstVal = calloc(pstDef->u32Count, sizeof(t_UHcpTuningType));
    if (!pstTuning->pstVal)
        goto err_alloc_val;

    pstTuning->pstNext = pstCb->pstTuningList;
    pstCb->pstTuningList = pstTuning;

    return pstTuning;

err_alloc_val:
    free(pstTuning);

err_alloc_top:
    return NULL;
}

static void vVppIpHcp_FreeTuning(t_StHcpTuning *pstTuning)
{
    if (!pstTuning)
        return;

    if (pstTuning->pstVal)
        free(pstTuning->pstVal);

    free(pstTuning);
}

static void vVppIpHcp_FreeAllTunings(t_StHcpTuningCb *pstCb)
{
    uint32_t u32 = 0;
    t_StHcpTuning *pstTuning;

    if (!pstCb)
        return;

    while (pstCb->pstTuningList)
    {
        pstTuning = pstCb->pstTuningList;
        pstCb->pstTuningList = pstCb->pstTuningList->pstNext;
        vVppIpHcp_FreeTuning(pstTuning);
        u32++;
    }

    LOGD("free'd %u tunings", u32);
}

static t_StHcpTuning *pstVppIpHcp_FindTuning(t_StHcpTuningCb *pstCb,
                                             const t_StHcpTuningDef *pstDef)
{
    if (!pstCb || !pstDef)
        return NULL;

    t_StHcpTuning *pstTuning = pstCb->pstTuningList;

    while (pstTuning)
    {
        if (pstTuning->pstDef == pstDef)
        {
            // LOGI("found existing tuning at %u", u32);
            return pstTuning;
        }
        pstTuning = pstTuning->pstNext;
    }

    // LOGI("unable to find existing tuning");
    return NULL;
}

#define VALIDATE_TUNING(_type, _pt) \
    if ((_pt)->pstDef->eType == TUNING_TYPE_##_type) \
    { \
        uint32_t i; \
        for (i = 0; i < (_pt)->pstDef->u32Count; i++) \
        { \
            if ((_pt)->pstVal[i]._type > (_pt)->pstDef->max._type || \
                (_pt)->pstVal[i]._type < (_pt)->pstDef->min._type) \
                return VPP_ERR; \
        } \
        return VPP_OK; \
    }

static uint32_t u32VppIpHcp_ValidateTuning(t_StHcpTuning *pstTuning)
{
    uint32_t u32;

    if (!pstTuning || !pstTuning->pstDef || !pstTuning->pstVal)
        return VPP_ERR;

    if (pstTuning->pstDef->u32Count != pstTuning->u32ExternalCnt)
    {
        LOGE("incorrect number of tuning params, ext_cnt=%u, exp=%u",
             pstTuning->pstDef->u32Count, pstTuning->u32ExternalCnt);
        return VPP_ERR;
    }

    if (pstTuning->pstDef->eType == TUNING_TYPE_FLOAT)
    {
        for (u32 = 0; u32 < pstTuning->pstDef->u32Count; u32++)
        {
            // Check for NaN values.
            if (isnan(pstTuning->pstVal[u32].FLOAT))
            {
                return VPP_ERR;
            }
        }
    }

    if (pstTuning->pstDef->pfVal)
        return pstTuning->pstDef->pfVal(pstTuning);

    VALIDATE_TUNING(FLOAT, pstTuning);
    VALIDATE_TUNING(U32, pstTuning);
    VALIDATE_TUNING(S32, pstTuning);

    return VPP_ERR;
}

static uint32_t u32VppIpHcp_InvalidateTuning(t_StHcpTuning *pstTuning)
{
    VPP_RET_IF_NULL(pstTuning, VPP_ERR_PARAM);

    pstTuning->u32ExternalCnt = 0;
    pstTuning->bValid = VPP_FALSE;

    return VPP_OK;
}

static const t_StHcpTuningDef *pstVppIpHcp_GetTuningDefByStrId(char *pcId,
                                                               uint32_t u32Sz)
{
    const t_StHcpTuningDef *pstDef;
    uint32_t i, u32CmpLen;

    LOGI("searching for: '%s', sz=%u", pcId, u32Sz);
    for (i = 0; i < su32TuningCnt; i++)
    {
        pstDef = &astTuningDef[i];

        u32CmpLen = strlen(pstDef->pcId);
        if (u32Sz != u32CmpLen ||
            strncmp(pstDef->pcId, pcId, u32CmpLen))
        {
            // LOGI(".. [%u], def->id=%s, def->sz=%u", i, pstDef->pcId, u32CmpLen);
            continue;
        }

        LOGI(".. found at [%u], def->id=%s, def->sz=%u",
             i, pstDef->pcId, u32CmpLen);
        return pstDef;
    }

    return NULL;
}

static void DUMP_DEF(const t_StHcpTuningDef *pstDef)
{
    LOGD("DUMP_DEF: %s (%u) - type=%u, cnt=%u", pstDef->pcId, pstDef->u32Id,
         pstDef->eType, pstDef->u32Count);
}

static void DUMP_TUNING(const t_StHcpTuning *pstTuning)
{
#define SIZE 256
    char c[SIZE];
    int rem, len = 0;
    uint32_t i;
    t_EHcpTuningType eType;

    if (!pstTuning || !pstTuning->bValid)
        return;

    c[0] = '\0';
    eType = pstTuning->pstDef->eType;

    for (i = 0, rem = SIZE;
         i < pstTuning->u32ExternalCnt && rem > 0;
         i++, rem = SIZE - len)
    {
        if (eType == TUNING_TYPE_U32)
            len += snprintf(&c[len], rem, "%u, ", pstTuning->pstVal[i].U32);
        else if (eType == TUNING_TYPE_S32)
            len += snprintf(&c[len], rem, "%d, ", pstTuning->pstVal[i].S32);
        else if (eType == TUNING_TYPE_FLOAT)
            len += snprintf(&c[len], rem, "%f, ", pstTuning->pstVal[i].FLOAT);
    }

    if (len)
        c[len - 2] = '\0';

    LOGD("%s: %s", pstTuning->pstDef->pcId, c);
#undef SIZE
}

static char *strip_inplace(char *pc)
{
    uint32_t u32;
    char *pe;

    if (!pc)
        return pc;

    // strip leading whitespace
    u32 = 0;
    while (isspace((unsigned char)*pc))
    {
        u32++;
        pc++;
    }
    //LOGI("removed %u leading whitespaces. current string=%s", u32, pc);

    // trailing whitespace
    u32 = 0;
    pe = pc + strlen(pc) - 1;
    while (pe > pc && isspace((unsigned char)*pe))
    {
        u32++;
        pe--;
    }
    *(pe + 1) = '\0';
    //LOGI("removed %u trailing whitespaces. current string=%s", u32, pc);

    return pc;
}

// u32Sz = size of buffer in bytes
static char *strip_line_endings_inplace(char *pc, uint32_t u32Sz)
{
    if (!pc)
        return pc;

    uint32_t i = strcspn(pc, "\r\n");
    if (i < u32Sz)
    {
        pc[i] = '\0';
    }

    return pc;
}

static uint32_t u32VppIpHcp_ReadExternalTunings(t_StHcpTuningCb *pstCb)
{
    t_StHcpTuning *pstTuning = NULL;
    const t_StHcpTuningDef *pstDef = NULL;
    char cBuf[MAX_CHAR_PER_ROW];
    char *pcToken, *pcSave;
    uint32_t i;
    FILE *fp = NULL;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    fp = fopen(HCP_TUNINGS_FILE_NAME, "r");
    if (!fp)
    {
        LOGE("unable to open file: %s, err=%d (%s)", HCP_TUNINGS_FILE_NAME,
             errno, strerror(errno));
        return VPP_ERR_RESOURCES;
    }
    else
    {
        LOGI("opened file: %s", HCP_TUNINGS_FILE_NAME);
    }

    // if fgets is not null, it will always return the buffer with a null
    // terminated string, so it should be able to pass to functions which
    // require a null terminated buffer.
    while (fgets(cBuf, MAX_CHAR_PER_ROW, fp))
    {

        LOGI("-----");
        pstDef = NULL;

        strip_line_endings_inplace(cBuf, MAX_CHAR_PER_ROW);

        LOGD("read in: '%s'", cBuf);
        pcToken = strtok_r(cBuf, TUNINGS_FILE_DELIM, &pcSave);
        if (!pcToken)
        {
            LOGE("unable to find tuning id: '%s'", pcToken);
            continue;
        }

        pcToken = strip_inplace(pcToken);
        pstDef = pstVppIpHcp_GetTuningDefByStrId(pcToken, strlen(pcToken));
        if (!pstDef)
        {
            LOGE("unable to find tuning definition for id: '%s'", pcToken);
            continue;
        }
        DUMP_DEF(pstDef);

        pstTuning = pstVppIpHcp_FindTuning(pstCb, pstDef);
        if (!pstTuning)
        {
            LOGI("unable to find existing tuning, allocating...");
            pstTuning = pstVppIpHcp_AllocTuning(pstCb, pstDef);
        }

        if (!pstTuning)
        {
            LOGE("unable to find or allocate tuning structure");
            break;
        }

        if (pstTuning->bValid)
            pstCb->u32ValidTuningCnt--;
        u32VppIpHcp_InvalidateTuning(pstTuning);

        // load external values into internal structure
        for (i = 0; i < pstDef->u32Count; i++)
        {
            pcToken = strtok_r(NULL, TUNINGS_FILE_DELIM, &pcSave);
            if (!pcToken)
                break;
            pcToken = strip_inplace(pcToken);

            LOGD("token: '%s'", pcToken);
            if (pstDef->eType == TUNING_TYPE_FLOAT)
            {
                pstTuning->pstVal[i].FLOAT = strtod(pcToken, NULL);
            }
            else if (pstDef->eType == TUNING_TYPE_U32)
            {
                pstTuning->pstVal[i].U32 = strtol(pcToken, NULL, 0);
            }
            else if (pstDef->eType == TUNING_TYPE_S32)
            {
                pstTuning->pstVal[i].S32 = strtol(pcToken, NULL, 0);
            }
        }
        pstTuning->u32ExternalCnt = i;

        if (u32VppIpHcp_ValidateTuning(pstTuning) == VPP_OK)
        {
            LOGD("setting valid on tuning");
            pstTuning->bValid = VPP_TRUE;
            pstCb->u32ValidTuningCnt++;
        }
        DUMP_TUNING(pstTuning);
    }

    fclose(fp);

    return VPP_OK;
}

static uint32_t u32VppIpHcp_GetTuningsBufferSize(t_StVppIpHcpGlobalCb *pstGlobal)
{
    uint32_t u32Size = 0;
    t_StHcpTuningCb *pstTuningCb;
    t_StHcpTuning *pstTuning;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);

    pstTuningCb = (t_StHcpTuningCb *)pstGlobal->pvTunings;

    if (pstTuningCb->u32ValidTuningCnt == 0)
    {
        u32Size = 0;
    }
    else
    {
        u32Size = GET_SYSTUNE_HEADER_SIZE();

        for (pstTuning = pstTuningCb->pstTuningList;
             pstTuning != NULL;
             pstTuning = pstTuning->pstNext)
        {
            if (!pstTuning->bValid)
                continue;

            u32Size += GET_SYSTUNEPARAM_HEADER_SIZE();
            u32Size += GET_TUNING_ELEMENTS_SIZE(pstTuning);
        }
    }

    return u32Size;
}

static uint32_t u32VppIpHcp_AllocIonBuffer(t_StVppIpHcpGlobalCb *pstGlobal,
                                           uint32_t u32Size)
{
    uint32_t u32Ret;
    t_StHcpTuningCb *pstCb;

    if (!pstGlobal || !u32Size)
        return VPP_ERR_PARAM;

    pstCb = (t_StHcpTuningCb *)pstGlobal->pvTunings;

    u32Ret = u32VppIon_Alloc(&pstGlobal->stVppCtx, u32Size, VPP_FALSE,
                             &pstCb->stSysBuf.stIon);
    if (u32Ret)
    {
        LOGE("failed to init ion memory. ret=%u", u32Ret);
        return VPP_ERR;
    }
    pstCb->stSysBuf.u32Sz = u32Size;
    pstCb->stSysBuf.eSysBufType = HCP_BUF_TYPE_TUNING;

    return VPP_OK;
}

static uint32_t u32VppIpHcp_FreeIonBuffer(t_StVppIpHcpGlobalCb *pstGlobal)
{
    uint32_t u32Ret;
    t_StHcpTuningCb *pstCb;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);

    pstCb = (t_StHcpTuningCb *)pstGlobal->pvTunings;

    u32Ret = u32VppIon_Free(&pstGlobal->stVppCtx, &pstCb->stSysBuf.stIon);
    if (u32Ret)
    {
        LOGE("failed to free ion memory. ret=%u", u32Ret);
        return VPP_ERR;
    }
    pstCb->stSysBuf.u32Sz = 0;

    return VPP_OK;
}

#ifdef DUMP_BUFFER_ENABLED
static void DUMP_BUFFER(t_StVppIpHcpGlobalCb *pstGlobal)
{
    uint32_t *pu32;
    uint32_t u32;
    t_StHcpTuningCb *pstCb;

    if (!pstGlobal || !pstGlobal->pvTunings)
        return;

    pstCb = (t_StHcpTuningCb *)pstGlobal->pvTunings;
    pu32 = (uint32_t *)pstCb->stSysBuf.stIon.buf;

    for (u32 = 0;
         u32 < u32VppIpHcp_GetTuningsBufferSize(pstGlobal) / sizeof(uint32_t);
         u32++, pu32++)
    {
        LOGD("HCP_TUNING:DUMP_BUF: %u", *pu32);
    }
}
#endif

static int32_t s32VppIpHcp_ConvertFloatToFixed(t_UHcpTuningType unValue)
{
    int32_t s32Value = unValue.FLOAT * 65536.0f;
    LOGD("float=%f (0x%x) converted to fixed=0x%x", unValue.FLOAT, unValue.U32,
         s32Value);
    return s32Value;
}

static uint32_t u32VppIpHcp_PackTuningElement(t_EHcpTuningType eType,
                                              t_UHcpTuningType unValue,
                                              uint32_t *pu32Dest)
{
    int32_t s32Value;
    VPP_RET_IF_NULL(pu32Dest, VPP_ERR_PARAM);

    switch (eType)
    {
        // Intentional fall-through.
        case TUNING_TYPE_U32:
        case TUNING_TYPE_S32:
            memcpy(pu32Dest, &unValue.U32, sizeof(uint32_t));
            return VPP_OK;
        case TUNING_TYPE_FLOAT:
            s32Value = s32VppIpHcp_ConvertFloatToFixed(unValue);
            memcpy(pu32Dest, &s32Value, sizeof(int32_t));
            return VPP_OK;
        default:
            LOGE("given invalid tuning element type to pack. u32=%u", eType);
            return VPP_ERR_PARAM;
    }
}

static uint32_t u32VppIpHcp_PackTuningParam(t_StHcpTuning *pstTuning,
                                            t_StSysTuneParam *pstParam)
{
    uint32_t u32;
    uint32_t u32Ret;
    uint32_t *pu32TuneParamElement;

    VPP_RET_IF_NULL(pstTuning, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstParam, VPP_ERR_PARAM);

    pstParam->hdr.u32Id = pstTuning->pstDef->u32Id;
    pstParam->hdr.u32Len = pstTuning->pstDef->u32Count;

    pu32TuneParamElement = &pstParam->u32Bdy[0];

    for (u32 = 0; u32 < pstTuning->pstDef->u32Count; u32++)
    {
        u32Ret = u32VppIpHcp_PackTuningElement(pstTuning->pstDef->eType,
                                               pstTuning->pstVal[u32],
                                               &pu32TuneParamElement[u32]);
        if (u32Ret != VPP_OK)
        {
            LOGE("failed to pack tuning element. ret=%u, type=%u, value=0x%x",
                 u32Ret, pstTuning->pstDef->eType, pstTuning->pstVal[u32].U32);
        }
    }

    return VPP_OK;
}

static uint32_t u32VppIpHcp_PackBuffer(t_StVppIpHcpGlobalCb *pstGlobal)
{
    uint32_t u32Ret;
    char *pcParam;
    t_StHcpTuningCb *pstCb;
    t_StHcpTuning *pstTuning;
    t_StSysTune *pstSysTune;
    t_StSysTuneParam *pstParam;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);

    pstCb = (t_StHcpTuningCb *)pstGlobal->pvTunings;

    memset(pstCb->stSysBuf.stIon.buf, 0, pstCb->stSysBuf.stIon.len);

    pstSysTune = (t_StSysTune *)pstCb->stSysBuf.stIon.buf;
    pstSysTune->u32NumTuneParams = 0;
    pcParam = (char *)&pstSysTune->u32Bdy[0];

    for (pstTuning = pstCb->pstTuningList;
         pstTuning != NULL;
         pstTuning = pstTuning->pstNext)
    {
        if (pstTuning->bValid)
        {
            pstParam = (t_StSysTuneParam *)pcParam;

            u32Ret = u32VppIpHcp_PackTuningParam(pstTuning, pstParam);
            if (u32Ret != VPP_OK)
            {
                LOGE("failed to pack a tuning param. ret=%u", u32Ret);
                return VPP_ERR;
            }

            pstSysTune->u32NumTuneParams++;
            pcParam += sizeof(pstParam->hdr) +
                       pstParam->hdr.u32Len * sizeof(uint32_t);
        }
    }
#ifdef DUMP_BUFFER_ENABLED
    DUMP_BUFFER(pstGlobal);
#endif

    return VPP_OK;
}

static uint32_t u32VppIpHcp_ValidateMessagePacket(t_StVppIpHcpGlobalCb *pstGlobal,
                                                  t_StHcpHfiMsgPkt *pstMsg)
{
    uint32_t u32Val;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstGlobal->pvTunings, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    u32Val = pstMsg->stHdr.eMsgId;
    if (u32Val != MSG_ID_BUFF_RELEASED)
    {
        LOGE("given invalid message id. id=%u", u32Val);
        return VPP_ERR_PARAM;
    }
    u32Val = pstMsg->stHdr.u32Size;
    if (u32Val == 0)
    {
        LOGE("given invalid message packet size. size=%u", u32Val);
        return VPP_ERR_PARAM;
    }
    u32Val = pstMsg->stMsgBuf.bufAttr.eBufType;
    if (u32Val != BUFF_TYPE_GENERIC)
    {
        LOGE("given invalid buffer type. type=%u", u32Val);
        return VPP_ERR_PARAM;
    }
    u32Val = pstMsg->stMsgBuf.bufAttr.u32Cookie_h;
    if (u32Val != HCP_SYS_TUNE_COOKIE_H)
    {
        LOGE("given invalid cookie_h. h=%u", u32Val);
        return VPP_ERR_PARAM;
    }
    u32Val = pstMsg->stMsgBuf.bufAttr.u32Cookie_l;
    if (u32Val != HCP_SYS_TUNE_COOKIE_L)
    {
        LOGE("given invalid cookie_l. l=%u", u32Val);
        return VPP_ERR_PARAM;
    }
    u32Val = pstMsg->stMsgBuf.bufAttr.stGenBufAttr.u32Size;
    if (u32Val != u32VppIpHcp_GetTuningsBufferSize(pstGlobal))
    {
        LOGE("given invalid buffer size. size=%u, expected=%u", u32Val,
             u32VppIpHcp_GetTuningsBufferSize(pstGlobal));
        return VPP_ERR_PARAM;
    }
    u32Val = pstMsg->stMsgBuf.bufAttr.stGenBufAttr.eUsage;
    if (u32Val != BUFF_USAGE_TUNE)
    {
        LOGE("given invalid buffer usage. usage=%u", u32Val);
        return VPP_ERR_PARAM;
    }

    return VPP_OK;
}

/************************************************************************
 * Global Functions
 ***********************************************************************/

uint32_t u32VppIpHcp_TuningInit(t_StVppIpHcpGlobalCb *pstGlobal)
{
    int rc;
    uint32_t u32;
    t_StHcpTuningCb *pstCb;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    if (pstGlobal->pvTunings)
    {
        LOGE("tunings already initialized in global cb");
        return VPP_ERR_STATE;
    }

    pstCb = calloc(1, sizeof(t_StHcpTuningCb));
    if (!pstCb)
    {
        LOGE("unable to allocate tuning cb");
        u32 = VPP_ERR_NO_MEM;
        goto err_calloc_cb;
    }

    rc = pthread_mutex_init(&pstCb->mutex, NULL);
    if (rc)
    {
        LOGE("unable to init mutex. rc=%d (%s)", rc, strerror(rc));
        u32 = VPP_ERR_RESOURCES;
        goto err_mutex_init;
    }

    u32 = u32VppIpHcp_ReadExternalTunings(pstCb);
    if (u32 == VPP_ERR_RESOURCES)
    {
        LOGE("unable to read external tunings file. will use default tunings.");
        u32 = VPP_OK;
        goto err_read_tunings;
    }
    else if (u32)
    {
        LOGE("unable to read external tunings, u32=%u", u32);
        goto err_read_tunings;
    }

    pstGlobal->pvTunings = pstCb;

    pstCb->eState = TUNING_STATE_INITED;

    return VPP_OK;

err_read_tunings:
    rc = pthread_mutex_destroy(&pstCb->mutex);
    LOGE_IF(rc, "failed to destroy mutex. rc=%d (%s)", rc, strerror(rc));
err_mutex_init:
    free(pstCb);
err_calloc_cb:
    return u32;
}

uint32_t u32VppIpHcp_TuningTerm(t_StVppIpHcpGlobalCb *pstGlobal)
{
    uint32_t u32;
    t_StHcpTuningCb *pstCb;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstGlobal->pvTunings, VPP_OK);

    pstCb = pstGlobal->pvTunings;

    if (pstCb->eState != TUNING_STATE_INITED &&
        pstCb->eState != TUNING_STATE_BUFFER_SENT)
    {
        LOGE("unable to term tuning due to invalid state. state=%u",
             pstCb->eState);
        return VPP_ERR_STATE;
    }

    if (pstCb->eState == TUNING_STATE_BUFFER_SENT)
    {
        LOGE("called tuning term() without receiving tuning buffer back.");
        u32 = u32VppIpHcp_FreeIonBuffer(pstGlobal);
        LOGE_IF(u32 != VPP_OK, "failed to free tuning buffer. ret=%u", u32);
    }

    vVppIpHcp_FreeAllTunings(pstCb);

    u32 = pthread_mutex_destroy(&pstCb->mutex);
    LOGE_IF(u32, "failed to destroy mutex. u32=%u", u32);

    free(pstCb);
    pstGlobal->pvTunings = NULL;

    return VPP_OK;
}

uint32_t u32VppIpHcp_TuningLoad(t_StVppIpHcpGlobalCb *pstGlobal)
{
    uint32_t u32Ret;
    uint32_t u32FuncRet;
    uint32_t u32Size;
    t_StHcpTuningCb *pstCb;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstGlobal->pvTunings, VPP_OK);

    pstCb = pstGlobal->pvTunings;

    pthread_mutex_lock(&pstCb->mutex);

    if (pstCb->eState != TUNING_STATE_INITED)
    {
        LOGE("unable to load tunings due to invalid state. state=%u",
             pstCb->eState);
        u32FuncRet = VPP_ERR_STATE;
        pthread_mutex_unlock(&pstCb->mutex);
        goto err_wrong_state;
    }

    pstCb->eState = TUNING_STATE_BUFFER_SEND_PENDING;

    pthread_mutex_unlock(&pstCb->mutex);

    u32Size = u32VppIpHcp_GetTuningsBufferSize(pstGlobal);
    LOGD("received size for tunings buffer. size=%u", u32Size);

    if (u32Size > 0)
    {
        u32Ret = u32VppIpHcp_AllocIonBuffer(pstGlobal, u32Size);
        if (u32Ret)
        {
            LOGE("failed to allocate tunings buffer. ret=%u", u32Ret);
            u32FuncRet = VPP_ERR;
            goto err_alloc_ion_buffer;
        }

        u32Ret = u32VppIpHcp_PackBuffer(pstGlobal);
        if (u32Ret != VPP_OK)
        {
            LOGE("failed to pack tuning buffer. ret=%u", u32Ret);
            u32FuncRet = VPP_ERR;
            goto err_pack_buffer;
        }

        pthread_mutex_lock(&pstCb->mutex);

        u32Ret = u32VppIpHcp_HwGlobalSendSysBuf(pstGlobal, &pstCb->stSysBuf);
        if (u32Ret != VPP_OK)
        {
            LOGE("failed to send tuning buffer to firmware. ret=%u", u32Ret);
            u32FuncRet = VPP_ERR;
            pthread_mutex_unlock(&pstCb->mutex);
            goto err_send_buffer;
        }

        pstCb->eState = TUNING_STATE_BUFFER_SENT;
    }
    else
    {
        pthread_mutex_lock(&pstCb->mutex);
        pstCb->eState = TUNING_STATE_INITED;
    }

    pthread_mutex_unlock(&pstCb->mutex);

    return VPP_OK;

err_send_buffer:
err_pack_buffer:
    u32Ret = u32VppIpHcp_FreeIonBuffer(pstGlobal);
    LOGE_IF(u32Ret != VPP_OK, "failed to free tuning buffer. ret=%u", u32Ret);
err_alloc_ion_buffer:
    pthread_mutex_lock(&pstCb->mutex);
    pstCb->eState = TUNING_STATE_INITED;
    pthread_mutex_unlock(&pstCb->mutex);
err_wrong_state:
    return u32FuncRet;
}

uint32_t u32VppIpHcp_TuningProcBuffReleasedMsg(t_StVppIpHcpGlobalCb *pstGlobal,
                                               t_StHcpHfiMsgPkt *pstMsg)
{
    uint32_t u32Ret;
    uint32_t u32FuncRet;
    t_StHcpTuningCb *pstCb;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    if (pstGlobal->pvTunings == NULL)
    {
        LOGE("ProcBuff() called, but not expecting a buffer.");
        return VPP_ERR;
    }

    pstCb = pstGlobal->pvTunings;

    pthread_mutex_lock(&pstCb->mutex);

    if (pstCb->eState != TUNING_STATE_BUFFER_SENT)
    {
        LOGE("unable to process buffer due to invalid state. state=%u",
             pstCb->eState);
        u32FuncRet = VPP_ERR_STATE;
        pthread_mutex_unlock(&pstCb->mutex);
        goto err_wrong_state;
    }
    pstCb->eState = TUNING_STATE_BUFFER_RECV_PENDING;

    pthread_mutex_unlock(&pstCb->mutex);

    u32Ret = u32VppIpHcp_ValidateMessagePacket(pstGlobal, pstMsg);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to process buffer due to invalid message packet. ret=%u",
             u32Ret);
        u32FuncRet = VPP_ERR_PARAM;
        goto err_validate_message_packet;
    }

    u32Ret = u32VppIpHcp_FreeIonBuffer(pstGlobal);
    if (u32Ret)
        LOGE("failed to free tuning buffer. ret=%u", u32Ret);

    LOGI("received tuning buffer back. err=%d, op=%u", pstMsg->stHdr.s32Err,
         pstMsg->stMsgBuf.bufAttr.u32Operation);

    pthread_mutex_lock(&pstCb->mutex);
    pstCb->eState = TUNING_STATE_INITED;
    pthread_mutex_unlock(&pstCb->mutex);

    return VPP_OK;

err_free_ion_buffer:
err_validate_message_packet:
    pthread_mutex_lock(&pstCb->mutex);
    pstCb->eState = TUNING_STATE_BUFFER_SENT;
    pthread_mutex_unlock(&pstCb->mutex);
err_wrong_state:
    return u32FuncRet;
}

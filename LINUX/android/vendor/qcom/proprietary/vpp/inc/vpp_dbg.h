/*!
 * @file vpp_dbg.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */
#ifndef _VPP_DBG_H_
#define _VPP_DBG_H_


#define VPP_PROPERTY_LOG_CORE           "vendor.media.vpp.loglevel"
#define VPP_PROPERTY_LOG_PERFMASK       "vendor.media.vpp.logperfmask"
#define VPP_PROPERTY_DUMP_BUFFER_ENABLE "vendor.media.vpp.bufdump_en"
#define VPP_PROPERTY_DUMP_BUFFER_PATH   "vendor.media.vpp.bufdump_path"
#define VPP_PROPERTY_STATS_LEVEL        "vendor.media.vpp.statsmask"
#define VPP_DEFAULT_DUMP_BUF_PATH       "/data/vendor/vpp/"

// #define VPP_FORCE_DISABLE_LOGS          // uncomment to turn off all logs
// #define VPP_FORCE_LOG_PRINTF            // uncomment to direct logs to printf
// #define FORCE_COMMON_LOGTAG             // uncomment to use common vpp log tags

#define VPP_TAG_DEFAULT "vpp-lib"

/*!
 * VPP Library
 */
#define VPP_LOG_MODULE_CORE             0
#define VPP_LOG_MODULE_CORE_TAG         "vpp-core"
#define VPP_LOG_MODULE_PIPELINE         1
#define VPP_LOG_MODULE_PIPELINE_TAG     "vpp-pipeline"
#define VPP_LOG_MODULE_ION              2
#define VPP_LOG_MODULE_ION_TAG          "vpp-ion"
#define VPP_LOG_MODULE_BUF              3
#define VPP_LOG_MODULE_BUF_TAG          "vpp-buf"
#define VPP_LOG_MODULE_CB               4
#define VPP_LOG_MODULE_CB_TAG           "vpp-callback"
#define VPP_LOG_MODULE_UC               5
#define VPP_LOG_MODULE_UC_TAG           "vpp-uc"
#define VPP_LOG_MODULE_GPU              6
#define VPP_LOG_MODULE_GPU_TAG          "vpp-gpu"
#define VPP_LOG_MODULE_HVX_CORE         7
#define VPP_LOG_MODULE_HVX_CORE_TAG     "vpp-hvx-core"
#define VPP_LOG_MODULE_HVX              8
#define VPP_LOG_MODULE_HVX_TAG          "vpp-hvx"
#define VPP_LOG_MODULE_FRC_CORE         9
#define VPP_LOG_MODULE_FRC_CORE_TAG     "vpp-frc-core"
#define VPP_LOG_MODULE_FRC_ME           10
#define VPP_LOG_MODULE_FRC_ME_TAG       "vpp-frc-me"
#define VPP_LOG_MODULE_FRC_MC           11
#define VPP_LOG_MODULE_FRC_MC_TAG       "vpp-frc-mc"
#define VPP_LOG_MODULE_HCP              12
#define VPP_LOG_MODULE_HCP_TAG          "vpp-hcp"
#define VPP_LOG_MODULE_PROFILE          13
#define VPP_LOG_MODULE_PROFILE_TAG      "vpp-prof"
#define VPP_LOG_MODULE_OTHER            14
#define VPP_LOG_MODULE_OTHER_TAG        VPP_TAG_DEFAULT

/*!
 * VPP Unit Test
 */
#define VPP_LOG_UT_PL                   0
#define VPP_LOG_UT_PL_TAG               "vpp-test_pipeline"
#define VPP_LOG_UT_QUEUE                1
#define VPP_LOG_UT_QUEUE_TAG            "vpp-test_queue"
#define VPP_LOG_UT_VPP_CORE             2
#define VPP_LOG_UT_VPP_CORE_TAG         "vpp-test_core"
#define VPP_LOG_UT_BUF                  3
#define VPP_LOG_UT_BUF_TAG              "vpp-test_buf"
#define VPP_LOG_UT_UC                   4
#define VPP_LOG_UT_UC_TAG               "vpp-test_uc"
#define VPP_LOG_UT_CB                   5
#define VPP_LOG_UT_CB_TAG               "vpp-test_callback"
#define VPP_LOG_UT_GPU                  6
#define VPP_LOG_UT_GPU_TAG              "vpp-test_gpu"
#define VPP_LOG_UT_ME                   7
#define VPP_LOG_UT_ME_TAG               "vpp-test_me"
#define VPP_LOG_UT_MC                   8
#define VPP_LOG_UT_MC_TAG               "vpp-test_mc"
#define VPP_LOG_UT_OTHER                9
#define VPP_LOG_UT_OTHER_TAG            "vpp-test_other"
#define VPP_LOG_UT_IP                   10
#define VPP_LOG_UT_IP_TAG               "vpp-test_ip"
#define VPP_LOG_UT_STATS                11
#define VPP_LOG_UT_STATS_TAG            "vpp-test_stats"
#define VPP_LOG_UT_VPPUTILS             12
#define VPP_LOG_UT_VPPUTILS_TAG         "vpp-test_vpputils"

/*!
 * VPP Functional Test
 */
#define VPP_LOG_FT_BUFFER_EXCHANGE      0
#define VPP_LOG_FT_BUFFER_EXCHANGE_TAG  "vpp-test_buffer_exchange"
#define VPP_LOG_FT_CONCURRENCY          1
#define VPP_LOG_FT_CONCURRENCY_TAG      "vpp-test_buffer_exchange"
#define VPP_LOG_FT_MAIN                 2
#define VPP_LOG_FT_MAIN_TAG             "vpp-test_main"
#define VPP_LOG_FT_OTHER                3
#define VPP_LOG_FT_OTHER_TAG            "vpp-test_other"

/*!
 * Test Utils
 */
#define VPP_LOG_TEST_UTILS_POOL         0
#define VPP_LOG_TEST_UTILS_POOL_TAG     "vpp-test_utils_pool"
#define VPP_LOG_TEST_UTILS_MOCK         1
#define VPP_LOG_TEST_UTILS_MOCK_TAG     "vpp-test_utils_mock"
#define VPP_LOG_TEST_UTILS_OTHER        2
#define VPP_LOG_TEST_UTILS_OTHER_TAG    "vpp-test_utils"

/*!
 * IPC Client/Service
 */
#define VPP_LOG_IPC_CLIENT              0
#define VPP_LOG_IPC_CLIENT_TAG          "vpp-ipc_client"
#define VPP_LOG_IPC_SERVICE             1
#define VPP_LOG_IPC_SERVICE_TAG         "vpp-ipc_service"
#define VPP_LOG_IPC_OTHER               2
#define VPP_LOG_IPC_OTHER_TAG           "vpp-ipc"

/*!
 * IPC Unit Test
 */
#define VPP_LOG_UT_IPC_CLIENT           0
#define VPP_LOG_UT_IPC_CLIENT_TAG       "vpp-test_ipc_client"
#define VPP_LOG_UT_IPC_SVC              1
#define VPP_LOG_UT_IPC_SVC_TAG          "vpp-test_ipc_svc"
#define VPP_LOG_UT_IPC_UTILS            2
#define VPP_LOG_UT_IPC_UTILS_TAG        "vpp-test_ipc_utils"

/*!
 * Log Levels
 */
#define VPP_LOG_LVL_VERBOSE         3
#define VPP_LOG_LVL_INFO            2
#define VPP_LOG_LVL_DEBUG           1
#define VPP_LOG_LVL_ERROR           0
#define VPP_LOG_MASK                3
#define VPP_LOG_FLD_WID             2

/*!
 * Buffer Dump
 */
#define VPP_BUFDUMP_OUTPUT      (1 << 0)
#define VPP_BUFDUMP_INPUT       (1 << 1)
#define VPP_BUFDUMP_COMPRESSED  (1 << 2)


#if defined(VPP_FUNCTIONAL_TEST)
#define U64_LOG_LEVEL           u64LogLevelFunc
#define VPP_LOG_OTHER_DEFAULT   VPP_LOG_FT_OTHER
#define VPP_LOG_OTHER_TAG       VPP_LOG_FT_OTHER_TAG
#elif defined(VPP_UNIT_TEST)
#define U64_LOG_LEVEL           u64LogLevelUnit
#define VPP_LOG_OTHER_DEFAULT   VPP_LOG_UT_OTHER
#define VPP_LOG_OTHER_TAG       VPP_LOG_UT_OTHER_TAG
#elif defined(VPP_TEST_UTILS)
#define U64_LOG_LEVEL           u64LogLevelTestUtils
#define VPP_LOG_OTHER_DEFAULT   VPP_LOG_TEST_UTILS_OTHER
#define VPP_LOG_OTHER_TAG       VPP_LOG_TEST_UTILS_OTHER_TAG
#elif defined(VPP_IPC)
#define U64_LOG_LEVEL           u64LogLevelIpc
#define VPP_LOG_OTHER_DEFAULT   VPP_LOG_IPC_OTHER
#define VPP_LOG_OTHER_TAG       VPP_LOG_IPC_OTHER_TAG
#else
#define U64_LOG_LEVEL           u64LogLevel
#define VPP_LOG_OTHER_DEFAULT   VPP_LOG_MODULE_OTHER
#define VPP_LOG_OTHER_TAG       VPP_LOG_MODULE_OTHER_TAG
#endif

extern uint64_t U64_LOG_LEVEL;
#ifndef VPP_LOG_MODULE
#define VPP_LOG_MODULE VPP_LOG_OTHER_DEFAULT
#ifndef VPP_LOG_TAG
#define VPP_LOG_TAG VPP_LOG_OTHER_TAG
#endif
#endif

#ifndef VPP_LOG_TAG
#define VPP_LOG_TAG VPP_TAG_DEFAULT
#endif

#ifdef FORCE_COMMON_LOGTAG
#undef VPP_LOG_TAG
#define VPP_LOG_TAG VPP_TAG_DEFAULT
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG VPP_LOG_TAG

#if !defined(VPP_FORCE_DISABLE_LOGS) && defined(_ANDROID_) && !defined(VPP_FORCE_LOG_PRINTF)
#include <utils/Log.h>
#define VPP_LOGI                ALOGI
#define VPP_LOGD                ALOGD
#define VPP_LOGV                ALOGV
#define VPP_LOGE                ALOGE
#elif !defined(VPP_FORCE_DISABLE_LOGS)
#include <stdio.h>
#define VPP_LOGI(fmt, a...)     printf("%20s - I/ " fmt "\n", LOG_TAG, ##a)
#define VPP_LOGD(fmt, a...)     printf("%20s - D/ " fmt "\n", LOG_TAG, ##a)
#define VPP_LOGV(fmt, a...)     printf("%20s - V/ " fmt "\n", LOG_TAG, ##a)
#define VPP_LOGE(fmt, a...)     printf("%20s - E/ " fmt "\n", LOG_TAG, ##a)
#else
static inline void vpp_warning_suppressor(int a, ...) { (void)a; }
#define VPP_LOGV(fmt, a...)     ({ vpp_warning_suppressor(0, fmt, ##a); })
#define VPP_LOGI(fmt, a...)     ({ vpp_warning_suppressor(0, fmt, ##a); })
#define VPP_LOGD(fmt, a...)     ({ vpp_warning_suppressor(0, fmt, ##a); })
#define VPP_LOGE(fmt, a...)     ({ vpp_warning_suppressor(0, fmt, ##a); })
#endif

#define LOGV(fmt, args...) ({ \
    if (((U64_LOG_LEVEL >> (2*VPP_LOG_MODULE)) & VPP_LOG_MASK) >= VPP_LOG_LVL_VERBOSE) \
        VPP_LOGV(fmt, ##args); \
    })

#define LOGV_IF(cond, fmt, args...) ({ \
    if (cond) \
        LOGV(fmt, ##args); \
    })

#define LOGI(fmt, args...) ({ \
    if (((U64_LOG_LEVEL >> (2*VPP_LOG_MODULE)) & VPP_LOG_MASK) >= VPP_LOG_LVL_INFO) \
        VPP_LOGI(fmt, ##args); \
    })

#define LOGI_IF(cond, fmt, args...) ({ \
    if (cond) \
        LOGI(fmt, ##args); \
    })

#define LOGD(fmt, args...) ({ \
    if (((U64_LOG_LEVEL >> (2*VPP_LOG_MODULE)) & VPP_LOG_MASK) >= VPP_LOG_LVL_DEBUG) \
        VPP_LOGD(fmt, ##args); \
    })

#define LOGD_IF(cond, fmt, args...) ({ \
    if (cond) \
        LOGD(fmt, ##args); \
    })

#define LOGE(fmt, args...) \
    VPP_LOGE("%s (%u): " fmt, __func__, __LINE__, ##args)

#define LOGE_IF(cond, fmt, args...) ({ \
    if (cond) \
        VPP_LOGE("%s (%u): " fmt, __func__, __LINE__, ##args); \
    })

#define ASSERT_LOGE(fmt, args...) LOGE(fmt, ##args)

typedef struct {
    uint64_t u64LogLevelCore;
    uint32_t u32LogPerfMask;
    uint32_t u32BufDumpEnable;
    char cBufDumpPath[128];
    uint32_t u32StatsEn;
} t_StVppDbgProp;

/***************************************************************************
 * Helper Macros
 ***************************************************************************/

#define VPP_ASSERT(_cond) { if (!_cond) { \
    ASSERT_LOGE("ASSERT_FAILED: Line: %u File: %s Function: %s Cond: %s", \
                __LINE__, __FILE__, __func__, #_cond); } }

#define LOG_ENTER()                 LOGI(">> %s()", __func__)
#define LOG_ENTER_ARGS(str, ...)    LOGI(">> %s() " str, __func__, __VA_ARGS__)
#define LOG_EXIT()                  LOGI("<< %s()", __func__)
#define LOG_EXIT_ARGS(str, ...)     LOGI("<< %s() " str, __func__, __VA_ARGS__)

#define LOG_EXIT_RET(ret)           do { LOG_EXIT(); return ret; } while(0)
#define LOG_EXITI(ret)              LOGI("<< %s() returns %d", __func__, ret)
#define LOG_EXITI_RET(ret)          do { LOG_EXITI(ret); return ret; } while(0)
#define LOG_EXITP(ret)              LOGI("<< %s() returns %p", __func__, ret)
#define LOG_EXITP_RET(ret)          do { LOG_EXITP(ret); return ret; } while(0)

#define VPP_RET_VOID_IF_NULL(ptr) \
    if (!ptr) { \
        LOGE(#ptr " is null. Returning."); \
        return; \
    }
#define VPP_RET_IF_NULL(ptr, ret) \
    if (!ptr) { \
        LOGE(#ptr " is null. Returning: " #ret); \
        return ret; \
    }

#define PU(ptr, field) ((ptr) ? (ptr)->field : 0)
#define STRU(str, field) (str.field)
#define LOG_PARAM_PTR(lvl, p, q) \
    LOGI(#p":{w=%u, h=%u, fmt=%u}, "#q":{w=%u, h=%u, fmt=%u}", \
         PU(p, width), PU(p, height), PU(p, fmt), \
         PU(q, width), PU(q, height), PU(q, fmt))

#define LOG_PARAM_STR(lvl, p, q) \
    LOGI(#p":{w=%u, h=%u, str=%u, scl=%u, fmt=%u}, "#q":{w=%u, h=%u, str=%u, scl=%u, fmt=%u}", \
         STRU(p, width), STRU(p, height), STRU(p, stride), STRU(p, scanlines), STRU(p, fmt), \
         STRU(q, width), STRU(q, height), STRU(q, stride), STRU(q, scanlines), STRU(q, fmt))

#define LOG_CASE(pc, x)         case x: LOGI("%s: %s", pc, #x); break;

#define LOG_CADE(L, pcade)      LOG##L("->cade:{mode=%u, cade_level=%u, contrast=%d, saturation=%d}", (pcade)->mode, (pcade)->cade_level, (pcade)->contrast, (pcade)->saturation);
#define LOG_DI(L, pdi)          LOG##L("->di:{mode=%u}", (pdi)->mode);
#define LOG_TNR(L, ptnr)        LOG##L("->tnr:{mode=%u, level=%u}", (ptnr)->mode, (ptnr)->level);
#define LOG_CNR(L, pcnr)        LOG##L("->cnr:{mode=%u, level=%u}", (pcnr)->mode, (pcnr)->level);
#define LOG_AIE(L, paie)        LOG##L("->aie:{mode=%u, hue_mode=%u, cade_level=%u, ltm_level=%u, sat_gain=%u, sat_off=%u, ace_str=%u, ace_bl=%u, ace_bh=%u}", \
                                       (paie)->mode, (paie)->hue_mode, (paie)->cade_level, (paie)->ltm_level, \
                                       (paie)->ltm_sat_gain, (paie)->ltm_sat_offset, (paie)->ltm_ace_str, \
                                       (paie)->ltm_ace_bright_l, (paie)->ltm_ace_bright_h);
#define LOG_FRC(L, pfrc)        LOG##L("->frc:{mode=%u, level=%u, interp=%u}", (pfrc)->mode, (pfrc)->level, (pfrc)->interp);
#define LOG_EAR(L, pear)        LOG##L("->ear:{mode=%u}", (pear)->mode);
#define LOG_QBR(L, pqbr)        LOG##L("->qbr:{mode=%u}", (pqbr)->mode);
#define VPP_NOT_IMPLEMENTED()   LOGE("%s:%u %s() not implemented", __FILE__, __LINE__, __func__)

#define LOG_U32(L, u32)         LOG##L(#u32"=%u", u32)
#define LOG_U32_H(L, u32)       LOG##L(#u32"=0x%x", u32)
/***************************************************************************
 * Function Prototypes
 ***************************************************************************/


#endif /* _VPP_DBG_H_ */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  defog_internal.h
/// @brief DefogLib class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DEFOG_INTERNAL_H__
#define __DEFOG_INTERNAL_H__

#include "defog.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define GAMMA_INDEX_NUM 257
#define GAMMA_MAX_VAL 1023
#define MAX_AEC_AWB_CHECKER_INDEX_NUM 10
#define MAX_SHDR_CHECKER_INDEX_NUM 2
extern UINT32  g_log_level;

    typedef struct st_ipe_cv
    {
        FLOAT r_to_y;
        FLOAT g_to_y;
        FLOAT b_to_y;
        INT32 y_offset;
    } IPE_CV_PARAMS;

    /* simple histograms for contrast calculation */
    typedef struct st_simple_histogram
    {
        UINT64 shist_3bins[3];
        UINT64 shist_4bins[4];
        UINT64 shist_8bins[8];
        UINT64 shist_8bins_pct[8]; /* ex: 755 = 75.5% */
        UINT64 shist_16bins[16];
        UINT64 shist_16bins_pct[16]; /* ex: 755 = 75.5% */
    } S_HIST;

    /* @brief Structure to measure KPI */
    typedef struct
    {
        UINT32 start_time;
        UINT32 start_time_of_each_frame;
        UINT32 end_time;
        UINT32 total_time;
        UINT32 frame_count;
    } StatsKPIType;

	/* simple AEC checker for settle detection */
	typedef struct st_simple_aec_checker
	{
        BOOL settledFromAECLib[MAX_AEC_AWB_CHECKER_INDEX_NUM];
        FLOAT luxIdxTb[MAX_AEC_AWB_CHECKER_INDEX_NUM];
        UINT32 idxNum;
	} S_AEC_CHECKER;

    /* simple AWB checker for settle detection */
    typedef struct st_simple_awb_checker
    {
        BOOL settledFromAWBLib[MAX_AEC_AWB_CHECKER_INDEX_NUM];
        UINT32 cctTb[MAX_AEC_AWB_CHECKER_INDEX_NUM];
        UINT32 idxNum;
    } S_AWB_CHECKER;

    /* simple SHDR checker for settle detection */
    typedef struct st_simple_shdr_checker
    {
        BOOL settledFromSHDRLib[MAX_SHDR_CHECKER_INDEX_NUM];
        UINT32 ratioTb[MAX_SHDR_CHECKER_INDEX_NUM];
        UINT32 idxNum;
    } S_SHDR_CHECKER;

    struct DefogInternalData
    {
        /* Store input data */
        DefogConfigParams                                           defogConfigData;
        DefogInputData                                              defogInputData;
        DefogOutputData                                             defogOutputData;

        /* Defog KPI convergence time structure */
        StatsKPIType                                                kpi;

        /* internal parameters */
        UINT32 convergence_mode;
        FLOAT lp_defog_param;
        ParsedIHistStatsOutput*                                     pInterHistStats;
        S_HIST                                                      s_hist;
        IPE_CV_PARAMS                                               ipe_cv_param;
        IPE_CV_PARAMS                                               defogCV;
        FLOAT                                                       gain_up_tb[GAMMA_INDEX_NUM];
        FLOAT                                                       contrast_tb[GAMMA_INDEX_NUM];
        S_AEC_CHECKER                                               aecChecker;
        S_AWB_CHECKER                                               awbChecker;
        S_SHDR_CHECKER                                              shdrChecker;
        UINT32 fog_scene_probability;
        UINT32 ce_scene_probability;
        INT32 hq_defog_state;
        INT32 lp_defog_state;
        INT32 abc_state;
        INT32 acc_state;
        INT32 guc_state;
        INT32 dcc_state;
        BOOL is_aec_settled;
        BOOL is_awb_settled;
        BOOL is_shdr_settled;
        BOOL is_iHist_settled;
        BOOL is_iHist_ready;                    ///< iHist module can get sensor data and ouput iHist
        BOOL is_iHist_valid;                    ///< Check if iHist data populated by SHDR3 lib is valid per frame
        BOOL is_defog_settled;
        BOOL is_ce_settled;
        UINT32 iHist_sum;
        UINT32 iHist_deci_cnt;
        INT32 iHist_dark_bin;
        INT32 iHist_mid_bin;
        INT32 iHist_bright_bin;
        INT32 iHist_min_bins;
        INT32 iHist_max_bins;
        INT32 iHist_avg_bins;
        INT32 iHist_old_dark_bin;
        INT32 iHist_old_bright_bin;

        /* parameters for video simulation */
        FLOAT defog_cur_strength;
        FLOAT defog_cur_abc_knee_pt;
        FLOAT defog_cur_acc_dark_knee_pt;
        FLOAT defog_cur_acc_bright_knee_pt;
        FLOAT ce_cur_strength;
        FLOAT ce_cur_guc_knee_pt;
        FLOAT ce_cur_dcc_dark_knee_pt;
        FLOAT ce_cur_dcc_bright_knee_pt;
        FLOAT acc_strength;
        FLOAT lp_defog_cur_param;
        FLOAT lp_defog_step;
        INT32 hq_defog_org_dark_bin;
        INT32 hq_defog_org_bright_bin;
        INT32 hq_defog_cur_dark_bin;
        INT32 hq_defog_cur_bright_bin;
        INT32 defog_ce_step_num;
        INT32 hq_defog_dark_step_sz;
        INT32 hq_defog_bright_step_sz;
        INT32 hq_defog_step_num;                ///< final HQ defog steps
        FLOAT defog_abc_step;
        FLOAT defog_acc_dark_step;
        FLOAT defog_acc_bright_step;
        FLOAT ce_guc_step;
        FLOAT ce_dcc_dark_step;
        FLOAT ce_dcc_bright_step;
    };

#undef CAMX_LOG_VERBOSE
#undef CAMX_LOG_INFO
#undef CAMX_LOG_WARN
#undef CAMX_LOG_ERROR
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Macro for printing error logs to the system
/// @param group LogGroup of the log
/// @param fmt Format string, printf style
/// @param ... Parameters required by format
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAMX_LOG_ERROR(group, fmt, ...)                                    \
    CAMX_LOG(CamxLogError, "[ERROR]", group, fmt, ##__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Macro for printing warning logs to the system
/// @param group LogGroup of the log
/// @param fmt Format string, printf style
/// @param ... Parameters required by format
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAMX_LOG_WARN(group, fmt, ...)                                     \
    if (g_log_level >= CamxLogWarning)                               \
    {                                                                      \
            CAMX_LOG(CamxLogWarning, "[ WARN]", group, fmt, ##__VA_ARGS__) \
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Macro for printing info logs to the system
/// @param group LogGroup of the log
/// @param fmt Format string, printf style
/// @param ... Parameters required by format
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAMX_LOG_INFO(group, fmt, ...)                                     \
    if (g_log_level >= CamxLogInfo)                                  \
    {                                                                      \
        CAMX_LOG(CamxLogInfo, "[ INFO]", group, fmt, ##__VA_ARGS__)        \
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Macro for printing verbose logs to the system
/// @param group LogGroup of the log
/// @param fmt Format string, printf style
/// @param ... Parameters required by format
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAMX_LOG_VERBOSE(group, fmt, ...)                                  \
    if (g_log_level >= CamxLogVerbose)                               \
    {                                                                      \
        CAMX_LOG(CamxLogInfo, "[ VERB]", group, fmt, ##__VA_ARGS__)        \
    }

#ifdef __cplusplus
}
#endif

#endif /* __DEFOG_INTERNAL_H__ */

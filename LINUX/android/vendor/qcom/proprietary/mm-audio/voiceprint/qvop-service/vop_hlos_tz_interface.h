/********************************************************************
---------------------------------------------------------------------
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
VoicePrint low level header file for TZ interface
*********************************************************************/

/* commands supported from sample client */
#define QVoP_CMD0_GET_VERSION             0
#define QVoP_CMD1_SEND_PCM                1
#define QVoP_CMD3_TEST_CMD                  3
#define QVoP_CMD5_TEST_DISPLAY              5
#define QVoP_CMD6_DISABLE_TEST_DISPLAY      6

#define QVoP_PCM_BUF_SIZE (1*1024)

#define QVoP_MAX_TZ_ION_BUF (16*1024)
#define QVOP_PCM_EOS                       1
#define QVOP_PCM_NO_EOS                   0
#include <stdint.h>
#include "qvop_qsee_interface.h"
struct QVoP_pcm_buffer_cmd {
    uint32_t cmd_id;
    uint32_t pcm_format;
    uint32_t num_pcm_samples;
    uint16_t eos_flag;
    int16_t pcm_sample_buff[QVoP_PCM_BUF_SIZE];

 };

//#include <comdef.h>

/* commands supported from sample client */
#define QVOP_CMD0_GET_VERSION             0
#define QVoP_CMD1_SEND_PCM                1
#define QVoP_CMD3_TEST_CMD                3
#define QVoP_CMD5_TEST_DISPLAY              5
#define QVoP_CMD6_DISABLE_TEST_DISPLAY      6

//#define QVoP_MAX_TZ_ION_BUF 4*1024
#define QVOP_CMD_ENROLL_START             7
#define QVOP_CMD_ENROLL_CONTINUE          8
#define QVOP_CMD_PCM_DATA                  9
#define QVOP_CMD_MATCH_START              10
#define QVOP_CMD_MATCH_CANCEL              11
#define QVOP_CMD_GET_MATCH_RESULT          12

#define QVOP_CMD_SESSION_CANCEL              13
#define QVOP_CMD_ENROLL_COMMIT    16
#define QVOP_CMD_ENROLL_STAGE  17

#define QVOP_CMD_DELETE_ALL                  18
#define QVOP_CMD_DELETE_TEMPLATE          19
#define QVOP_CMD_RENAME_TEMPLATE          20
#define QVOP_CMD_CONFIG_AUTH_PARAMS          21
#define QVOP_CMD_CONFIG_THRES              22
#define QVOP_CMD_SET_VOP_DIR              23
#define QVOP_CMD_GET_ENROLL_STATS          24
#define QVOP_CMD_MATCH_INIT_CONTINUE      25

#define MAX_KEYPHRASE_LEN                  1000
#define MAX_PHRASEID_LEN                  300
struct qvop_cmd_version_t {
    uint32_t cmd_id;
};
struct qvop_cmd_delete_all_t {
    uint32_t cmd_id;
};

struct qvop_cmd_delete_template_t {
    uint32_t cmd_id;
    char user_id[QVOP_MAX_ENROLLEE_ID_LEN];
    char keyphrase_id[MAX_PHRASEID_LEN];
};
struct qvop_cmd_rename_template_t {
    uint32_t cmd_id;
    char user_id[QVOP_MAX_ENROLLEE_ID_LEN];
    char old_keyphrase_id[MAX_PHRASEID_LEN];
    char new_keyphrase_id[MAX_PHRASEID_LEN];
};
struct qvop_cmd_pcm_data_t {
    uint32_t cmd_id;
    uint32_t pcm_format;
    uint32_t num_pcm_samples;
    uint16_t eos_flag;
    int16_t pcm_sample_buff[QVoP_PCM_BUF_SIZE];
 };

struct qvop_cmd_enroll_start_t {
    uint32_t cmd_id;
    int mode;
    char   user_id[QVOP_MAX_ENROLLEE_ID_LEN];
    char   keyphrase[MAX_KEYPHRASE_LEN];
    char   keyphrase_id[MAX_PHRASEID_LEN];
    uint32_t threshold;
};

struct qvop_cmd_enroll_continue_t {
    uint32_t cmd_id;
};
struct qvop_cmd_enroll_commit_t {
    uint32_t cmd_id;
};
struct qvop_cmd_config_auth_params_t {
    uint32_t cmd_id;
    char   user_id[QVOP_MAX_ENROLLEE_ID_LEN];
    char   keyphrase_id[MAX_PHRASEID_LEN];
    uint32_t auth_level; //1 high 2 med 3 low// anything else is error
};
struct qvop_cmd_set_vop_dir_t {
    uint32_t cmd_id;
    char vop_dir[100];
};
struct qvop_cmd_get_enroll_stats {
    uint32_t cmd_id;
};

struct qvop_cmd_thres_config_t{
    uint32_t cmd_id;
    int32_t set_uv_thres;
    int32_t set_sv_thres;
    int32_t set_as_thres;
    float uv_hi;
    float uv_mid;
    float uv_lo;
    float sv_th;
    float as_th;
};

#define QVOP_TZ_PCM_REQ        1
#define QVOP_TZ_ENROLL_DONE 2
#define QVOP_TZ_ENROLL_EPD  3
#define QVOP_TZ_ENROLL_ERROR 4
#define QVOP_TZ_STATE_ERROR 5

#define QVOP_TZ_MATCH_PASS 10
#define QVOP_TZ_ENROLL_REWIND 7
#define QVOP_TZ_MATCH_FAIL 11
#define QVOP_TZ_MATCH_ERROR 12
#define QVOP_TZ_CANCEL_SUCCESS 13
#define QVOP_TZ_CANCEL_FAIL 14
#define QVOP_TZ_MATCH_COMPLETE 15
#define QVOP_TZ_ENROLL_COMMIT_DONE 16
#define QVOP_TZ_ENROLL_COMMIT_CONTINUE 17


#define QVOP_TZ_VERSION_INFO 20
#define QVOP_DELETE_ALL_SUCCESS 25
#define QVOP_DELETE_ALL_FAIL (-25)
#define QVOP_DELETE_TEMPLATE_SUCCESS 26
#define QVOP_DELETE_TEMPLATE_FAIL (-26)
#define QVOP_RENAME_TEMPLATE_SUCCESS (27)
#define QVOP_RENAME_TEMPLATE_FAIL (-27)
#define QVOP_CONFIG_AUTH_PARAMS_SUCCESS (28)
#define QVOP_CONFIG_AUTH_PARAMS_FAIL (-28)
#define QVOP_CONFIG_THRES_SUCCESS 29
#define QVOP_CONFIG_THRES_FAIL (-29)
#define QVOP_SET_VOP_DIR_SUCCESS (30)
#define QVOP_SET_VOP_DIR_FAILURE (-30)
#define QVOP_GET_ENROLL_STATS_SUCCESS (31)
#define QVOP_GET_ENROLL_STATS_FAILURE (-31)
#define QVOP_TZ_MATCH_INIT_CONTINUE 32
//enroll error codes
#define QVOP_TZ_ENROLL_ERROR_SNR_LOW 0x101
#define QVOP_TZ_ENROLL_ERROR_SENTENCE_MATCH_FAIL 0x102
#define QVOP_TZ_ENROLL_ERROR_PARAMS_ERROR 0x103
#define QVOP_TZ_ENROLL_ERROR_REINIT_ERROR 0x104
#define QVOP_TZ_ENROLL_ERROR_FILES_MISSING 0x105

struct qvop_set_vop_dir_response_t {
    uint32_t resp_id;
};
struct qvop_enroll_start_response_t {
    uint32_t resp_id;
};
struct qvop_config_auth_params_response_t {
    uint32_t resp_id;
};

struct qvop_enroll_commit_response_t {
    uint32_t resp_id;
};

struct qvop_enroll_continue_response_t {
    uint32_t resp_id;
};

struct qvop_pcm_data_response_t {
    uint32_t resp_id;
};
struct qvop_delete_all_response_t {
    uint32_t resp_id;
};
struct qvop_delete_template_response_t {
    uint32_t resp_id;
};
struct qvop_rename_template_response_t {
    uint32_t resp_id;
};
struct qvop_config_thres_respose_t {
    uint32_t resp_id;
};
#define MATCH_MODE_TP     0x1
#define MATCH_MODE_TI      0x2
#define MATCH_MODE_TD     0x3

struct qvop_cmd_match_start_t {
    uint32_t cmd_id;
    uint32_t match_mode;
    char   user_id[QVOP_MAX_ENROLLEE_ID_LEN];
    char  keyphraseid[MAX_PHRASEID_LEN];
    char   keyphrase[MAX_KEYPHRASE_LEN];
    char session_id[20];
};

struct qvop_match_start_response_t {
    uint32_t resp_id;
};

struct qvop_cmd_get_match_result_t {
    uint32_t cmd_id;

};
struct qvop_cmd_enroll_stage_t {
    uint32_t cmd_id;
};
#define QVOP_TZ_MATCH_PASS 10
#define QVOP_TZ_MATCH_FAIL 11
#define QVOP_TZ_MATCH_ERROR 12
#define QVOP_TZ_MATCH_COMPLETE 15
#define QVOP_TZ_ENROLL_COMMIT_DONE 16

struct qvop_session_cancel_t {
    uint32_t status;
};
struct qvop_cancel_response_t {
    uint32_t resp_id;
};
struct qvop_enroll_stage_response_t {
    uint32_t resp_id;
};
struct qvop_match_result_response_t {
    uint32_t resp_id;
    int32_t pcm_index;
    char user_id[QVOP_MAX_ENROLLEE_ID_LEN];
    char keyphrase_id[QVOP_MAX_PHRASEID_LEN];
    //sentence related
    int32_t numFrames;
    double sentence_score;
    //biometrics related
    double verification_score;
    //antiSpoofefd
    float as_fSvmScore;
};
struct qvop_version_response_t {
    uint32_t resp_id;
    uint16_t major;
    uint16_t minor;
};
struct qvop_get_enroll_stats_response_t{
    uint32_t resp_id;
    float snr;
    float sentence_score;
};
//declare to avoid incomplete declaration warnings
struct QSEECom_handle;

int32_t qsc_start_app(struct QSEECom_handle **l_QSEEComHandle,
                      const char *appname, int32_t buf_size);

int32_t qsc_shutdown_app(struct QSEECom_handle **l_QSEEComHandle);

int32_t qvop_issue_send_cmd_req(struct QSEECom_handle *l_QSEEComHandle,
                                         void *send_cmd , uint32_t cmd_size, void *resP_ptr);


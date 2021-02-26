/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QVOP_QSEE_INTERFACE_H_
#define _QVOP_QSEE_INTERFACE_H_

typedef enum QvopStatus_t {
    QVOP_SUCCESS = 0,  // The call executed successfully
    QVOP_FAILURE = -1,  // There was an error executing the call
    QVOP_TIMEOUT = 1,  // A timeout was triggered during the call
    QVOP_CANCEL = 2,  // The call was canceled by a user application
    QVOP_STOPPED = 3,  // The call was forced to stop by the OS or framework
    QVOP_PCM_DATA_REQUEST = 4,  //requesting PCM
    QVOP_PCM_REWIND = 5,
    QVOP_ENROLLMENT_COMPLETE = 10,  // All individual enrollment scans have been successfully collected for the current finger
    QVOP_TZ_CANCEL_SUCCESS = 13,
    QVOP_ENROLLMENT_FAILED = -10,  // The system was unable to collect an adequate number of enrollment scans from the user
    QVOP_ENROLLMENT_NO_SPACE = -11,  // There is no more space to enroll
    QVOP_ENROLLEE_NOT_FOUND = -12, // Enrollee not found (for delete enrollee case)
    QVOP_MATCH_SUCCESS = 20,  // Inquiry was successfully matched to a fingerprint in the database
    QVOP_MATCH_COMPLETE = 21,
    QVOP_MATCH_FAILURE = -21,  // Inquiry was not matched to a voiceprint in the database

    QVOP_ERR_UNKNOWN_COMMAND = -110,
    QVOP_ERR_NOT_IMPLEMENTED = -120,
    QVOP_ERR_NOT_INITIALIZED = -130,
    QVOP_ERR_BAD_PARAM = -140,
    QVOP_ERR_MALLOC = -210,
    QVOP_ERR_FREE = -211,
    QVOP_ENROLLMENT_SECURE_UI_ERR = -312
} QvopStatus;

typedef int int32_t;
typedef short int16_t;
typedef unsigned int uint32_t;

// limits
#define QVOP_MAX_VERSION_LEN                 4
#define QVOP_MAX_VENDOR_LEN                 32
#define QVOP_MAX_KEYPHRASE_LEN                  1000
#define QVOP_MAX_PHRASEID_LEN                  300
#define QVOP_MAX_ENROLLEE_ID_LEN            100 // Maximum length of an enrollee identifier string, generated from within the fingerprint system
#define MAX_PCM_BUFFER 300000

#define ION_BUFFER_LENGTH (64*1024)

#define MAX_RESP_BUFF_SIZE (4*1024)
#define PCM_MONO_BIT_16                1
#define PCM_MULTI_MIC_FORMAT_1          2 // to be decided what this is.
#define QVOP_PCM_EOS                       1
#define QVOP_PCM_NO_EOS                   0

struct qvop_pcm_packet {
    uint32_t pcm_format;
    uint32_t num_pcm_samples;
    int16_t eos_flag;
    int16_t pcm_data[MAX_PCM_BUFFER];
};
struct qvop_enroll_algo_response {
    float snr;
    float sentence_score;
};

typedef enum QvopMatchMode_t {
    text_prompted = 1,
    text_independent = 2,
    text_dependent = 3
}QvopMatchMode;
typedef enum QvopMode_t {
    secureui_mode = 1,
    keyphrase_mode = 2,
    ti_mode = 3
}QvopMode;

struct qvop_version_info {
    int16_t major;
    int16_t minor;
};

struct qvop_enroll_params {
    char enrollee_id[QVOP_MAX_ENROLLEE_ID_LEN];
    QvopMode mode;
    char keyphrase[QVOP_MAX_KEYPHRASE_LEN];
    char keyphrase_id[QVOP_MAX_PHRASEID_LEN];
    uint32_t threshold;
};

struct qvop_match_params{
    char user_id[QVOP_MAX_ENROLLEE_ID_LEN];
    QvopMode mode;
    char keyphrase[QVOP_MAX_KEYPHRASE_LEN];
    char keyphrase_id[QVOP_MAX_PHRASEID_LEN];
    char session_id[20];
};

struct qvop_delete_params{
    char user_id[QVOP_MAX_ENROLLEE_ID_LEN];
    QvopMode mode;
    uint32_t keyphrase_id;
    char keyphrase[QVOP_MAX_KEYPHRASE_LEN];
};
struct qvop_match_results {
    uint32_t resp_id;
    int32_t index;
    uint32_t numFrames;
    //sentence related
    double sentence_score; // How much sentence really matched
    //biometrics related
    double verification_score; // User score (confidence user is user)
    //antiSpoof
    float as_fSvmScore; // Anti-spoof score
    char user_id[QVOP_MAX_ENROLLEE_ID_LEN];
    char keyphrase_id[QVOP_MAX_PHRASEID_LEN];
};
struct qvop_config_auth_params {
    char user_id[QVOP_MAX_ENROLLEE_ID_LEN];
    char keyphrase_id[QVOP_MAX_PHRASEID_LEN];
    //params start here
    uint32_t auth_level; //1 high 2 medium 3 low

};
struct qvop_config_vop_params {
    int32_t set_uv_thres;
    int32_t set_sv_thres;
    int32_t set_as_thres;
    float uv_hi; // user verification threshold - hi
    float uv_mid; // user verification threshold - mid
    float uv_lo; // user verification threshold - lo
    float sv_th; // sentence verification threshold
    float as_th; // antispoof thresholds
};
struct qvop_set_vop_dir_params {
    char vop_dir[100];
};
struct qvop_get_enroll_stats{
    float snr;
    float sentence_score;
};
QvopStatus QvopGetVersion(struct qvop_version_info *pInfo);
QvopStatus QvopSessionInit();
QvopStatus QvopEnrollBegin(struct qvop_enroll_params*); //returns QVOP_FAILURE or QVOP_PCM_DATA_REQUEST
QvopStatus QvopEnrollCancel(); //returns QVOP_SUCCESS for successful cancel
QvopStatus QvopRetrieveEnrollee(const char* enrollee_id); // returns QVOP SUCESS, QVOP_ENROLLEE_NOT_FOUND
QvopStatus QvopDeleteEnrollee(const char* enrollee_id); // return QVOP_SUCCESS, QVOP_ERROR is called in invalid state
QvopStatus QvopDeleteAllTemplates(); //returns QVOP_SUCCESS or QVOP_FAILURE if there are FIle system issues
QvopStatus QvopMatchBegin(struct qvop_match_params *);
QvopStatus QvopMatchUser(struct qvop_match_params *); //return QVOP_FAILURE or QVOP_PCM_DATA_REQUEST or QVOP_ENROLLEE_NOT_FOUND
QvopStatus QvopMatchCancel(); // return QVOP_SUCCESS or QVOP_FAILURE
QvopStatus QvopPCMData(struct qvop_pcm_packet *pcm_data); //returns QVOP_PCM_DATA_REQUEST or QVOP_ENROLLMENT_COMPLETE or QVOP_MATCH_SUCCESS
QvopStatus QvopSendPCM(struct qvop_pcm_packet *pcm_data); //returns QVOP_PCM_DATA_REQUEST or QVOP_ENROLLMENT_COMPLETE or QVOP_MATCH_SUCCESS
QvopStatus QvopSessionCancel();
QvopStatus QvopProfileDelete(struct qvop_delete_params *);
QvopStatus QvopMatchGetResult(struct qvop_match_results *);
QvopStatus QvopEnrollCommit();
QvopStatus QvopSessionEnd();
QvopStatus QvopRenameTemplate(char *userid, char *old_keyphrase_id, char *new_keyphrase_id);
QvopStatus QvopDeleteTemplate(char *userid, char *keyphrase_id);
QvopStatus QvopConfigAuthParams(struct qvop_config_auth_params *ptr_config);
QvopStatus QvopConfigThresholds(struct qvop_config_vop_params *params);
QvopStatus QvopGetEnrollStats(struct qvop_get_enroll_stats* ptr_stats);
QvopStatus qvop_init_session();

QvopStatus qvop_shutdown_session();

#endif // _QVOP_QSEE_INTERFACE_H_

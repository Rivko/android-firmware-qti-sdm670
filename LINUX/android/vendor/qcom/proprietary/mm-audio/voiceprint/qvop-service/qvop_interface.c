/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "qvop_qsee_interface.h"
#include "vop_hlos_tz_interface.h"

#if !defined(_MSC_VER) && !defined(_ANDROID_) && !defined(QVOP_LNX_ONLY)
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/msm_ion.h>
#include <utils/Log.h>
#include "QSEEComAPI.h"
#include "common_log.h"
#include <sys/mman.h>
#include <getopt.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "QVOP_QSEE"
#ifdef LOG_NDDEBUG
#undef LOG_NDDEBUG
#endif
#define LOG_NDDEBUG 1 //Define to enable LOGD
#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#endif
#define LOG_NDEBUG  0 //Define to enable LOGV
#else
#define LOG_TAG "QVOP_QSEE"

#define LOGE printf
#define LOGD printf
#define LOGV printf
#define LOGI printf
#endif

#define QVOP_APPNAME "voicepri"
#define NON_LEGACY_CMD_LINE_ARGS 1
//#define QVoP_MAX_TZ_ION_BUF (1024*1024)

typedef enum qvop_status_t {
    qvop_reset = 0,
    enroll_inprogress,
    match_inprogress
} qvop_status;


typedef enum touch_info_t {
    init,
    msg_received
} touch_info;

struct qvop_session_t{
    qvop_status current_status;
    void *response_buff;
    struct QSEECom_handle *l_QSEEComHandle;
    int fd;
} qvop_session;

struct qvop_cmd_enroll_start_t enroll_start_cmd;
struct qvop_cmd_enroll_continue_t enroll_continue_cmd;
struct qvop_cmd_pcm_data_t pcm_data_cmd;
struct qvop_cmd_match_start_t match_start_cmd;
struct qvop_cmd_version_t version_cmd;
struct qvop_cmd_get_match_result_t get_match_result_cmd;
struct qvop_cmd_enroll_commit_t enroll_commit_cmd;
struct qvop_cmd_enroll_stage_t enroll_stage_cmd;
struct qvop_cmd_delete_all_t delete_all_cmd;
struct qvop_cmd_delete_template_t delete_template_cmd;
struct qvop_cmd_rename_template_t rename_template_cmd;
struct qvop_cmd_config_auth_params_t config_auth_cmd;
struct qvop_cmd_thres_config_t config_thres_cmd;
struct qvop_cmd_set_vop_dir_t set_vop_dir_cmd;
struct qvop_cmd_get_enroll_stats get_enroll_stats;


QvopStatus QvopGetEnrollStats(struct qvop_get_enroll_stats *ptr_stats)
{
    struct qvop_get_enroll_stats_response_t *resp_ptr;

    if (ptr_stats == NULL) {
        return QVOP_FAILURE;
    }

    set_vop_dir_cmd.cmd_id = QVOP_CMD_GET_ENROLL_STATS;
    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &get_enroll_stats, sizeof(get_enroll_stats), \
        qvop_session.response_buff)) {
        //LOGE("%s: enroll cmd failed to send to TZ\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    resp_ptr = (struct qvop_get_enroll_stats_response_t *) qvop_session.response_buff;
    ptr_stats->sentence_score = resp_ptr->sentence_score;
    ptr_stats->snr = resp_ptr->snr;
    if (QVOP_GET_ENROLL_STATS_SUCCESS == resp_ptr->resp_id) {
        return QVOP_SUCCESS;
    }
    else {
        return QVOP_FAILURE;
    }
}



QvopStatus QvopSetVopDir(struct qvop_set_vop_dir_params *ptr_params)
{
    struct qvop_set_vop_dir_response_t *resp_ptr;
    if (ptr_params == NULL) {
        return QVOP_FAILURE;
    }
    set_vop_dir_cmd.cmd_id = QVOP_CMD_SET_VOP_DIR;

#if defined(ANDROID)
    strlcpy(set_vop_dir_cmd.vop_dir, ptr_params->vop_dir, 100);
#elif defined(_MSC_VER)
    strcpy_s(set_vop_dir_cmd.vop_dir, 100, ptr_params->vop_dir);
#endif

    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &set_vop_dir_cmd, sizeof(set_vop_dir_cmd), \
        qvop_session.response_buff)) {
        //LOGE("%s: QvopSetVopDir cmd failed to send to TZ\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    resp_ptr = (struct qvop_set_vop_dir_response_t *) qvop_session.response_buff;
    if (QVOP_SET_VOP_DIR_SUCCESS == resp_ptr->resp_id) {
        return QVOP_SUCCESS;
    }
    else {
        return QVOP_FAILURE;
    }
}

QvopStatus QvopEnrollCommit()
{
    struct qvop_enroll_commit_response_t *resp_ptr;

    enroll_commit_cmd.cmd_id = QVOP_CMD_ENROLL_COMMIT;
    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &enroll_commit_cmd, sizeof(enroll_commit_cmd), \
        qvop_session.response_buff)) {
        //LOGE("%s: enroll cmd failed to send to TZ\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    resp_ptr = (struct qvop_enroll_commit_response_t *) qvop_session.response_buff;
    while (QVOP_TZ_ENROLL_COMMIT_CONTINUE == resp_ptr->resp_id)
    {
        if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &enroll_commit_cmd, sizeof(enroll_commit_cmd), \
            qvop_session.response_buff)) {
            //LOGE("%s: enroll cmd failed to send to TZ\n", LOG_TAG);
            return QVOP_FAILURE;
        }
    }
    if (QVOP_TZ_ENROLL_COMMIT_DONE == resp_ptr->resp_id) {
        return QVOP_SUCCESS;
    }
    else {
        return QVOP_FAILURE;
    }
}


QvopStatus QvopConfigAuthParams(struct qvop_config_auth_params *ptr_config_params)
{

    struct qvop_config_auth_params_response_t * resp_ptr;
    if (ptr_config_params == NULL) {
        return QVOP_FAILURE;
    }

#if !defined(_MSC_VER)
    strlcpy(config_auth_cmd.user_id, ptr_config_params->user_id, QVOP_MAX_ENROLLEE_ID_LEN);

    strlcpy(config_auth_cmd.keyphrase_id, ptr_config_params->keyphrase_id, QVOP_MAX_PHRASEID_LEN);
#else
    strlcpy_s(config_auth_cmd.user_id, QVOP_MAX_ENROLLEE_ID_LEN, ptr_config_params->user_id);

    strlcpy_s(config_auth_cmd.keyphrase_id, QVOP_MAX_PHRASEID_LEN, ptr_config_params->keyphrase_id);
#endif
    config_auth_cmd.auth_level = ptr_config_params->auth_level;
    config_auth_cmd.cmd_id = QVOP_CMD_CONFIG_AUTH_PARAMS;

    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &config_auth_cmd, sizeof(config_auth_cmd), \
        qvop_session.response_buff)) {
        //LOGE("%s: enroll cmd failed to send to TZ\n", LOG_TAG);
        return QVOP_FAILURE;
    }

    resp_ptr = (struct qvop_config_auth_params_response_t *) qvop_session.response_buff;

    if (QVOP_CONFIG_AUTH_PARAMS_SUCCESS == resp_ptr->resp_id) {
        return QVOP_SUCCESS;
    }
    else {
        return QVOP_FAILURE;

    }
}

QvopStatus QvopDeleteTemplate(char *userid, char *keyphrase_id)
{
    struct qvop_delete_template_response_t * resp_ptr;

    if (userid == NULL) {
        return QVOP_FAILURE;
    }

#if !defined(_MSC_VER)
    strlcpy(delete_template_cmd.user_id, userid, QVOP_MAX_ENROLLEE_ID_LEN);

    strlcpy(delete_template_cmd.keyphrase_id, keyphrase_id, QVOP_MAX_PHRASEID_LEN);
#else
    strlpy_s(delete_template_cmd.user_id, QVOP_MAX_ENROLLEE_ID_LEN, userid);

    strlpy_s(delete_template_cmd.keyphrase_id, QVOP_MAX_PHRASEID_LEN, keyphrase_id);
#endif

    delete_template_cmd.cmd_id = QVOP_CMD_DELETE_TEMPLATE;

    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &delete_template_cmd, sizeof(delete_template_cmd), \
        qvop_session.response_buff)) {
        //LOGE("%s: enroll cmd failed to send to TZ\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    resp_ptr = (struct qvop_delete_template_response_t *) qvop_session.response_buff;

    if (QVOP_DELETE_TEMPLATE_SUCCESS == resp_ptr->resp_id) {
        return QVOP_SUCCESS;
    }
    else {
        return QVOP_FAILURE;
    }
}

QvopStatus QvopConfigThresholds(struct qvop_config_vop_params *params)
{
    struct qvop_config_thres_respose_t *resp_ptr;

    if (params == NULL) {
        return QVOP_FAILURE;
    }

    config_thres_cmd.uv_hi = params->uv_hi;
    config_thres_cmd.uv_mid = params->uv_mid;
    config_thres_cmd.uv_lo = params->uv_lo;
    config_thres_cmd.sv_th = params->sv_th;
    config_thres_cmd.as_th = params->as_th;
    config_thres_cmd.cmd_id = QVOP_CMD_CONFIG_THRES;

    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &config_thres_cmd, sizeof(config_thres_cmd), \
        qvop_session.response_buff)) {
        //LOGE("%s: enroll cmd failed to send to TZ\n", LOG_TAG);
        return QVOP_FAILURE;
    }

    resp_ptr = (struct qvop_config_thres_respose_t *) qvop_session.response_buff;

    if (QVOP_CONFIG_THRES_SUCCESS == resp_ptr->resp_id) {
        return QVOP_SUCCESS;
    }
    else {
        return QVOP_FAILURE;
    }

    return QVOP_SUCCESS;
}

QvopStatus QvopRenameTemplate(char *userid, char *old_keyphrase_id,char *new_keyphrase_id)
{
    struct qvop_rename_template_response_t * resp_ptr;

    if (userid == NULL) {
        return QVOP_FAILURE;
    }

#if !defined(_MSC_VER)
    strlcpy(rename_template_cmd.user_id, userid, QVOP_MAX_ENROLLEE_ID_LEN);
    strlcpy(rename_template_cmd.old_keyphrase_id, old_keyphrase_id, QVOP_MAX_PHRASEID_LEN);
    strlcpy(rename_template_cmd.new_keyphrase_id, new_keyphrase_id, QVOP_MAX_PHRASEID_LEN);
#else
    strlpy_s(rename_template_cmd.user_id, QVOP_MAX_ENROLLEE_ID_LEN, userid);
    strlpy_s(rename_template_cmd.old_keyphrase_id, QVOP_MAX_PHRASEID_LEN, old_keyphrase_id);
    strlpy_s(rename_template_cmd.new_keyphrase_id, QVOP_MAX_PHRASEID_LEN, new_keyphrase_id);
#endif

    rename_template_cmd.cmd_id = QVOP_CMD_RENAME_TEMPLATE;

    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &rename_template_cmd, sizeof(rename_template_cmd), \
        qvop_session.response_buff)) {
        //LOGE("%s: enroll cmd failed to send to TZ\n", LOG_TAG);
        return QVOP_FAILURE;
    }

    resp_ptr = (struct qvop_rename_template_response_t *) qvop_session.response_buff;
    if (QVOP_RENAME_TEMPLATE_SUCCESS == resp_ptr->resp_id) {
        return QVOP_SUCCESS;
    }
    else {
        return QVOP_FAILURE;
    }

}

QvopStatus QvopDeleteAllTemplates()
{
    struct qvop_delete_all_response_t *resp_ptr;
    delete_all_cmd.cmd_id = QVOP_CMD_DELETE_ALL;

    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &delete_all_cmd, sizeof(delete_all_cmd), \
        qvop_session.response_buff)) {
        //LOGE("%s: Delete all to TZ failed\n", LOG_TAG);

        return QVOP_FAILURE;
    }

    resp_ptr = (struct qvop_delete_all_response_t *) qvop_session.response_buff;

    if (QVOP_DELETE_ALL_SUCCESS == resp_ptr->resp_id) {
        return QVOP_SUCCESS;
    }
    else {
        return QVOP_FAILURE;
    }
}

/**@brief:  Implement simple application start
*
* @param[in/out]    handle.
* @param[in]        appname.
* @param[in]        buffer size.
* @return    zero on success or error count on failure.
*/

QvopStatus QvopGetVersion(struct qvop_version_info *ptr_version_info)
{
    struct qvop_version_response_t * ptr_version_rsp;
    QvopStatus ret = QVOP_SUCCESS;
    //LOGE("%s: ENTER\n", LOG_TAG);
    if (NULL == ptr_version_info)
    {
        //LOGE("%s: Voiceprint : Memory Error\n", LOG_TAG);
        return QVOP_FAILURE;
    }

    version_cmd.cmd_id = QVoP_CMD0_GET_VERSION;
    ret = qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &version_cmd, sizeof(version_cmd), qvop_session.response_buff);
    if (QVOP_SUCCESS != ret){
        //LOGE("%s: get version cmd failed to send to TZ %d\n", LOG_TAG, ret);
        return QVOP_FAILURE;
    }

    ptr_version_rsp = (struct qvop_version_response_t *) qvop_session.response_buff;
    if (QVOP_TZ_VERSION_INFO != ptr_version_rsp->resp_id)
    {
        ///LOGE("%s: Unexpected return for voiceprint\n", LOG_TAG);
        return QVOP_FAILURE;
    }

    ptr_version_info->major = ptr_version_rsp->major;
    ptr_version_info->minor = ptr_version_rsp->minor;

    return ret;
}

/**@brief:  Implement simple application start
*
* @param[in/out]    handle.
* @param[in]        appname.
* @param[in]        buffer size.
* @return    zero on success or error count on failure.
*/
QvopStatus qvop_init_session()
{
     qvop_session.response_buff = malloc(MAX_RESP_BUFF_SIZE);
     if (qsc_start_app(&qvop_session.l_QSEEComHandle,QVOP_APPNAME, QVoP_MAX_TZ_ION_BUF))
     {
         return QVOP_FAILURE;
     }

     return QVOP_SUCCESS;

}
QvopStatus qvop_get_match_result(struct qvop_match_results *ptr_match_results)
{
    struct  qvop_match_result_response_t * resp_ptr;
    get_match_result_cmd.cmd_id = QVOP_CMD_GET_MATCH_RESULT;

    if (match_inprogress != qvop_session.current_status) {
        return QVOP_FAILURE;
    }

    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &get_match_result_cmd, sizeof(get_match_result_cmd), \
        qvop_session.response_buff)) {
        return QVOP_FAILURE;
    }
    resp_ptr = (struct qvop_match_result_response_t *)qvop_session.response_buff;

    ptr_match_results->index = resp_ptr->pcm_index;
    ptr_match_results->sentence_score = resp_ptr->sentence_score;
    ptr_match_results->verification_score = resp_ptr->verification_score;
    ptr_match_results->as_fSvmScore = resp_ptr->as_fSvmScore;
#if !defined(_MSC_VER)
    strlcpy(ptr_match_results->user_id, resp_ptr->user_id, QVOP_MAX_ENROLLEE_ID_LEN);
    //LOGD("%s: User ID %s Keyphrase id %s\n", LOG_TAG, resp_ptr->user_id,  resp_ptr->keyphrase_id);
    strlcpy(ptr_match_results->keyphrase_id, resp_ptr->keyphrase_id, QVOP_MAX_PHRASEID_LEN);
#else
    strlcpy_s(ptr_match_results->user_id, QVOP_MAX_ENROLLEE_ID_LEN, resp_ptr->user_id );
    //LOGD("%s: User ID %s Keyphrase id %s\n", LOG_TAG, resp_ptr->user_id, resp_ptr->keyphrase_id);
    strlcpy_s(ptr_match_results->keyphrase_id, QVOP_MAX_PHRASEID_LEN, resp_ptr->keyphrase_id );
#endif

    if (QVOP_TZ_MATCH_PASS != resp_ptr->resp_id) {
        return QVOP_MATCH_FAILURE;
    }

    return QVOP_MATCH_SUCCESS;

}

/**@brief:  Implement simple application start
*
* @param[in/out]    handle.
* @param[in]        appname.
* @param[in]        buffer size.
* @return    zero on success or error count on failure.
*/
QvopStatus qvop_shutdown_session()
{
    struct qvop_session_cancel_t cancel_cmd;
    struct qvop_cancel_response_t *resp_ptr;

    cancel_cmd.status = QVOP_CMD_SESSION_CANCEL;
    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &cancel_cmd, sizeof(cancel_cmd), \
        qvop_session.response_buff)) {
        return QVOP_FAILURE;
    }
    resp_ptr = (struct qvop_cancel_response_t *)qvop_session.response_buff;
    if (QVOP_TZ_CANCEL_SUCCESS != resp_ptr->resp_id) {
        return QVOP_FAILURE;
    }
    free(qvop_session.response_buff);
    return qsc_shutdown_app(&qvop_session.l_QSEEComHandle);
}

/**@brief:  Implement simple application start
*
* @param[in/out]    handle.
* @param[in]        appname.
* @param[in]        buffer size.
* @return    zero on success or error count on failure.
*//**@brief:  Implement simple application start
 *
 * @param[in/out]    handle.
 * @param[in]        appname.
 * @param[in]        buffer size.
 * @return    zero on success or error count on failure.
 */
QvopStatus QvopSessionInit()
{
    if (qvop_session.current_status != qvop_reset) {
        return QVOP_FAILURE;
    }

    qvop_session.current_status = qvop_reset;

    return qvop_init_session();
}

QvopStatus QvopEnrollBegin(struct qvop_enroll_params *enroll_params)
{
    struct qvop_enroll_start_response_t *resp_ptr;

    qvop_session.current_status = enroll_inprogress;
    enroll_start_cmd.cmd_id = QVOP_CMD_ENROLL_START;
    enroll_start_cmd.mode = (int)enroll_params->mode;
#if defined(_MSC_VER)
    strlcpy_s(enroll_start_cmd.user_id, QVOP_MAX_ENROLLEE_ID_LEN,enroll_params->enrollee_id, strlen(enroll_params->enrollee_id));
    strlcpy_s(enroll_start_cmd.keyphrase, QVOP_MAX_KEYPHRASE_LEN, enroll_params->keyphrase, strlen(enroll_params->keyphrase));
    strlcpy_s(enroll_start_cmd.keyphrase_id, QVOP_MAX_PHRASEID_LEN,enroll_params->keyphrase_id, QVOP_MAX_PHRASEID_LEN);


#else
    strlcpy(enroll_start_cmd.user_id, enroll_params->enrollee_id, QVOP_MAX_ENROLLEE_ID_LEN);
    //enroll_start_cmd.user_id[2] = 0;
    strlcpy(enroll_start_cmd.keyphrase,enroll_params->keyphrase, QVOP_MAX_PHRASEID_LEN);
    strlcpy(enroll_start_cmd.keyphrase_id,enroll_params->keyphrase_id, QVOP_MAX_PHRASEID_LEN);
#endif

    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &enroll_start_cmd, sizeof(enroll_start_cmd), \
                                qvop_session.response_buff)) {
        //LOGE("%s: enroll cmd failed to send to TZ\n", LOG_TAG);
        qvop_session.current_status = qvop_reset;
        return QVOP_FAILURE;
    }
    resp_ptr = (struct qvop_enroll_start_response_t *) qvop_session.response_buff;
    if (QVOP_TZ_PCM_REQ == resp_ptr->resp_id) {
        //only valid response is PCM request
        return QVOP_PCM_DATA_REQUEST;
    }
    qvop_session.current_status = qvop_reset;
    //LOGE("%s: Enroll Cmd received invalid return value\n", LOG_TAG);
    return QVOP_FAILURE;
}

QvopStatus QvopMatchBegin(struct qvop_match_params *ptr_params)
{
    struct qvop_match_start_response_t * resp_ptr;
    if (qvop_reset != qvop_session.current_status) {
        //LOGE("%s: QVOP is not in reset state\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    qvop_session.current_status = match_inprogress;
    if (strlen(ptr_params->user_id) == 0) {
        //LOGD("%s: invalid user id for match\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    if (strlen(ptr_params->keyphrase_id) == 0) {
        //LOGD("%s: invalid keyphrase id for match\n", LOG_TAG);
        return QVOP_FAILURE;
    }

#if !defined(_MSC_VER)
    strlcpy( match_start_cmd.user_id ,ptr_params->user_id, strlen(ptr_params->user_id) + 1);
    strlcpy(match_start_cmd.keyphraseid, ptr_params->keyphrase_id,  strlen(ptr_params->keyphrase_id) + 1);
    strlcpy(match_start_cmd.session_id, ptr_params->session_id,  strlen(ptr_params->session_id) + 1);
    //LOGD("%s: Match Params %s %s\n", LOG_TAG, match_start_cmd.user_id, match_start_cmd.keyphraseid);
#else
    strlcpy_s(match_start_cmd.user_id, sizeof(ptr_params->user_id), ptr_params->user_id, strlen(ptr_params->user_id) + 1);
    strlcpy_s(match_start_cmd.keyphraseid, MAX_PHRASEID_LEN, ptr_params->keyphrase_id, MAX_PHRASEID_LEN);
    strlcpy_s(match_start_cmd.session_id, 20, ptr_params->session_id, 20);
#endif
    //match_start_cmd.keyphrase_id = ptr_params->keyphrase_id;
    match_start_cmd.match_mode = ptr_params->mode;
    match_start_cmd.cmd_id = QVOP_CMD_MATCH_START;
    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &match_start_cmd, sizeof(match_start_cmd), \
        qvop_session.response_buff)) {
        //LOGE("%s: match cmd failed to send to TZ\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    resp_ptr = (struct qvop_match_start_response_t *) qvop_session.response_buff;
    match_start_cmd.cmd_id = QVOP_CMD_MATCH_INIT_CONTINUE;

    //continue match init until PCM_REQUEST or error comes through
    while (QVOP_TZ_MATCH_INIT_CONTINUE == resp_ptr->resp_id)
    {
        if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &match_start_cmd, sizeof(match_start_cmd), \
            qvop_session.response_buff)) {
            //LOGE("%s: match cmd failed to send to TZ\n", LOG_TAG);
            return QVOP_FAILURE;
        }
    }
    if (QVOP_TZ_PCM_REQ == resp_ptr->resp_id) {
        return QVOP_PCM_DATA_REQUEST; //only valid response is usually PCM request, any thing else is an error
    }


    //LOGE("%s: Match Cmd received invalid return value\n", LOG_TAG);
    return QVOP_FAILURE;

}

QvopStatus QvopMatchUser(struct qvop_match_params *ptr_params)
{
    struct qvop_match_start_response_t *resp_ptr;
    if (NULL == ptr_params)
    {
        return QVOP_FAILURE;
    }

    if (strlen(ptr_params->user_id) == 0) {
        //LOGE("%s: invalid user id for match\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    qvop_session.current_status = match_inprogress;
#if !defined(_MSC_VER)
    strlcpy(ptr_params->user_id, match_start_cmd.user_id, strlen(ptr_params->user_id) + 1);
    strlcpy(ptr_params->keyphrase, match_start_cmd.keyphrase, strlen(ptr_params->keyphrase) + 1);
#else
    strlcpy_s(match_start_cmd.user_id, sizeof(ptr_params->user_id),ptr_params->user_id, strlen(ptr_params->user_id) + 1);
    strlcpy_s(match_start_cmd.keyphraseid, MAX_PHRASEID_LEN, ptr_params->keyphrase_id, MAX_PHRASEID_LEN);
#endif
    //match_start_cmd.keyphraseid = ptr_params->keyphrase_id;
    match_start_cmd.match_mode = ptr_params->mode;
    match_start_cmd.cmd_id = QVOP_CMD_MATCH_START;
    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &match_start_cmd, sizeof(match_start_cmd), \
                                qvop_session.response_buff)) {
        //LOGE("%s: match cmd failed to send to TZ\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    resp_ptr = (struct qvop_match_start_response_t *) qvop_session.response_buff;
    if (QVOP_TZ_PCM_REQ == resp_ptr->resp_id) {
        //at enroll begin.. we need to set up secure UI as well. Use a seperate function to kill current
        //session and spawn a new thread to launce secureTouch session

        return QVOP_PCM_DATA_REQUEST; //only valid response is usually PCM request, any thing else is an error

    }


    //LOGE("%s: Enroll Cmd received invalid return value\n", LOG_TAG);
    return QVOP_FAILURE;
}

static QvopStatus QvopProcessPCMForEnroll(struct qvop_pcm_packet *pcm_data)
{

    uint32_t i;
    uint64_t c = 1;
    struct qvop_pcm_data_response_t * resp_ptr = (struct qvop_pcm_data_response_t *)qvop_session.response_buff;
    struct qvop_enroll_start_response_t *enroll_resp_ptr = (struct qvop_enroll_start_response_t *)qvop_session.response_buff;
    struct qvop_enroll_stage_response_t *stage_resp_ptr = (struct qvop_enroll_stage_response_t *)qvop_session.response_buff;
    if (!((enroll_inprogress == qvop_session.current_status) || (match_inprogress == qvop_session.current_status)))
    {
        return QVOP_FAILURE;
    }
    if (NULL == pcm_data) {
        //LOGE("%s: invalid pointer for pcm data\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    pcm_data_cmd.cmd_id = QVOP_CMD_PCM_DATA;
    pcm_data_cmd.num_pcm_samples = pcm_data->num_pcm_samples;
    pcm_data_cmd.pcm_format = pcm_data->pcm_format;
    pcm_data_cmd.eos_flag = pcm_data->eos_flag;

    if (0 == pcm_data->num_pcm_samples) {
        //LOGE("%s: There are 0 samples\n", LOG_TAG);
        return QVOP_FAILURE;
    }
    for (i = 0; i < pcm_data_cmd.num_pcm_samples; ++i) {
        pcm_data_cmd.pcm_sample_buff[i] = pcm_data->pcm_data[i];
    }

    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle,&pcm_data_cmd, sizeof(pcm_data_cmd), \
                                qvop_session.response_buff)) {
        //LOGE("%s: cmd send failure\n", LOG_TAG);
        return QVOP_FAILURE;
    }

    if (enroll_inprogress == qvop_session.current_status )
    {
        if (QVOP_TZ_PCM_REQ == resp_ptr->resp_id) {
            return QVOP_PCM_DATA_REQUEST;
        }
    }

    if (QVOP_TZ_ENROLL_REWIND == resp_ptr->resp_id)
    {
        do
        {
            enroll_stage_cmd.cmd_id = QVOP_CMD_ENROLL_STAGE;
            if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle, &enroll_stage_cmd, sizeof(enroll_stage_cmd), \
                qvop_session.response_buff)) {
                return QVOP_FAILURE;
            }

        } while (QVOP_TZ_ENROLL_REWIND == resp_ptr->resp_id);

        if (QVOP_TZ_ENROLL_DONE == stage_resp_ptr->resp_id) {
            return QVOP_ENROLLMENT_COMPLETE;
        }
        return QVOP_ENROLLMENT_FAILED;
    }

    if (match_inprogress == qvop_session.current_status)
    {
        if (QVOP_TZ_PCM_REQ == resp_ptr->resp_id) {
            return QVOP_PCM_DATA_REQUEST;
        }
        if (QVOP_TZ_MATCH_PASS == resp_ptr->resp_id) {
            //return QVOP_MATCH_SUCCESS;
            return QVOP_SUCCESS;
        }
        else {
            return QVOP_MATCH_FAILURE;
        }
    }

    //LOGE("%s: UNKNOWN RESPONSE %d for Pcm command\n", LOG_TAG, resp_ptr->resp_id);

    return QVOP_FAILURE; //if the control reaches here, its an unknown command
}

static QvopStatus QvopProcessPCMForMatch(struct qvop_pcm_packet *pcm_data)
{
    uint32_t i;
    uint64_t c = 1;
    struct qvop_pcm_data_response_t * resp_ptr = (struct qvop_pcm_data_response_t *)qvop_session.response_buff;
    struct qvop_match_start_response_t *match_resp_ptr = (struct qvop_match_start_response_t *)qvop_session.response_buff;
    if (NULL == pcm_data) {
        //LOGE("%s: invalid pointer for pcm data\n", LOG_TAG);
        return QVOP_FAILURE;
    }

    //clean up command object which will be sent to TZ
    memset(&pcm_data_cmd, 0, sizeof(pcm_data_cmd));
    pcm_data_cmd.cmd_id = QVOP_CMD_PCM_DATA;
    pcm_data_cmd.eos_flag = pcm_data->eos_flag;

    //in the case where eos_flag != QVOP_PCM_EOS,
    //need to copy buffer & check inputs
    if (pcm_data->eos_flag != QVOP_PCM_EOS) {
        pcm_data_cmd.num_pcm_samples = pcm_data->num_pcm_samples > QVoP_PCM_BUF_SIZE ? QVoP_PCM_BUF_SIZE : pcm_data->num_pcm_samples;
        pcm_data_cmd.pcm_format = pcm_data->pcm_format;
        if ((0 == pcm_data->num_pcm_samples) ||
            pcm_data->num_pcm_samples > MAX_PCM_BUFFER) {
            return QVOP_FAILURE;
        }
        for (i = 0; i < pcm_data_cmd.num_pcm_samples; ++i) {
            pcm_data_cmd.pcm_sample_buff[i] = pcm_data->pcm_data[i];
        }
    }

    if (qvop_issue_send_cmd_req(qvop_session.l_QSEEComHandle,&pcm_data_cmd, sizeof(pcm_data_cmd), \
                                qvop_session.response_buff)) {
        return QVOP_FAILURE;
    }
    if (QVOP_TZ_PCM_REQ == resp_ptr->resp_id ) {
        return QVOP_PCM_DATA_REQUEST;
    }
    //not a pcm request anymore. So at this point we can disable the secure ui
    //need to check the mode here though!
    if (QVOP_TZ_MATCH_COMPLETE == resp_ptr->resp_id)
    {
        return QVOP_MATCH_COMPLETE;
    }
    if (QVOP_TZ_MATCH_PASS == resp_ptr->resp_id) {
        return QVOP_MATCH_SUCCESS;
    } else {
        return QVOP_MATCH_FAILURE;
    }

}

QvopStatus QvopSendPCM(struct qvop_pcm_packet *pcm_data){
    int i;
    if (enroll_inprogress == qvop_session.current_status) {
        return QvopProcessPCMForEnroll(pcm_data);

    }
    if (match_inprogress == qvop_session.current_status) {
        return QvopProcessPCMForMatch(pcm_data);

    }

    //LOGE("%s: PCM Cmd in invalid state\n", LOG_TAG);
    return QVOP_FAILURE;
}

QvopStatus QvopMatchGetResult(struct qvop_match_results *ptr_match_results)
{
    if (match_inprogress == qvop_session.current_status) {
        return qvop_get_match_result(ptr_match_results);

    }
    else {
        return QVOP_FAILURE;
    }
}

QvopStatus QvopPCMData(struct qvop_pcm_packet *pcm_data)
{
    int i;
    if (enroll_inprogress == qvop_session.current_status) {
        return QvopProcessPCMForEnroll(pcm_data);

    }
    if (match_inprogress == qvop_session.current_status) {
        return QvopProcessPCMForMatch(pcm_data);

    }

    //LOGE("%s: PCM Cmd in invalid state\n", LOG_TAG);
    return QVOP_FAILURE;
}

QvopStatus QvopSessionCancel()
{
    return qvop_shutdown_session();

}

QvopStatus QvopSessionEnd()
{
    QvopStatus result;
    result = qvop_shutdown_session();
    qvop_session.current_status = qvop_reset;
    return result;
}


QvopStatus QvopProfileDelete(struct qvop_delete_params *ptr_params)
{
    return QVOP_SUCCESS;
}

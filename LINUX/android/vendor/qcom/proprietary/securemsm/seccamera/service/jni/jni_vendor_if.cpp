/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
=============================================================

                          EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ------------------------------------------
04/12/17   gs      Initial version
=============================================================*/
#include <jni.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <QSEEComAPI.h>
#include "seccam_if.h"

#ifdef HAS_SECURE_PROCESSOR
	extern "C" {
		#include <sp_iris.h>
	}
#endif


#ifdef LOG_TAG
    #undef LOG_TAG
#endif
#define LOG_TAG "SECCAM-VENDOR-LIB-JNI"

typedef enum tz_app_vendor_if_cmd_id_t {
    TZ_APP_IF_CMD_VENDOR_EXCHANGE_TIMESTAMP     = 2000,
    TZ_APP_IF_CMD_VENDOR_INIT_BIOMETRICS_BUFFER = 2001,
    TZ_APP_IF_CMD_VENDOR_RUN_BIOMETRICS_TEST    = 2002,
    TZ_APP_IF_CMD_VENDOR_SIZE = 0x7FFFFFFF
} tz_app_vendor_if_cmd_id_t;

typedef enum tz_app_vendor_if_status_t {
    TZ_APP_IF_STATUS_VENDOR_SUCCESS = 0,
    TZ_APP_IF_STATUS_VENDOR_GENERAL_FAILURE = -1,
    TZ_APP_IF_STATUS_VENDOR_INVALID_INPUT_PARAMS = -2,
    TZ_APP_IF_STATUS_VENDOR_ERR_SIZE = 0x7FFFFFFF
} tz_app_vendor_if_status_t;
#pragma pack(push, tz_app_if, 1)

typedef struct ml_app_vendor_if_send_cmd_t {
    ml_app_if_cmd_id_t         cmd_id;
    uint64_t                   cmd_data;
} ml_app_vendor_if_send_cmd_t;

typedef struct ml_app_vendor_if_send_cmd_rsp_t {
    tz_app_if_status_t         ret;
    uint64_t                   ret_data;
} ml_app_vendor_if_send_cmd_rsp_t;

typedef struct tz_app_vendor_if_send_cmd_t {
    tz_app_vendor_if_cmd_id_t  cmd_id;
    uint64_t                   cmd_data;
} tz_app_vendor_if_send_cmd_t;

typedef struct tz_app_vendor_if_send_cmd_rsp_t {
    tz_app_vendor_if_status_t  ret;
    uint64_t                   ret_data;
} tz_app_vendor_if_send_cmd_rsp_t;

typedef struct tz_app_vendor_if_init_biometrics_cmd_t {
    tz_app_vendor_if_cmd_id_t  cmd_id;
    uint64_t                   buffer_base_address;
    uint64_t                   buffer_length;
} tz_app_vendor_if_init_biometrics_cmd_t;

typedef struct tz_app_vendor_if_run_biometrics_test_cmd_t {
    tz_app_vendor_if_cmd_id_t  cmd_id;
    uint64_t                   test_number;
    uint64_t                   buffer_length;
} tz_app_vendor_if_run_biometrics_test_cmd_t;


#pragma pack(pop, tz_app_if)

static pthread_mutex_t access_lock_ = PTHREAD_MUTEX_INITIALIZER;

//=========================================================================
inline void lock() {
    pthread_mutex_lock(&access_lock_);
}

//=========================================================================
inline void unlock() {
    pthread_mutex_unlock(&access_lock_);
}

//=========================================================================
extern "C" jlong Java_com_qualcomm_qti_seccamservice_SecCamServiceVendorHandler_exchangeTimestampWithTA(
    JNIEnv* env, jobject thiz, jlong hlosTimestamp) {

    jlong ret = 0;
    ALOGD("exchangeTimestampWithTA - Enter");

    lock();

    tz_app_vendor_if_send_cmd_t cmd_;
    tz_app_vendor_if_send_cmd_rsp_t cmd_rsp_;
    memset(&cmd_, 0, sizeof(tz_app_vendor_if_send_cmd_t));
    memset(&cmd_rsp_, 0, sizeof(tz_app_vendor_if_send_cmd_rsp_t));
    cmd_.cmd_id = TZ_APP_IF_CMD_VENDOR_EXCHANGE_TIMESTAMP;
    cmd_.cmd_data = hlosTimestamp;

    int retval = secCamSendCmd(&cmd_, sizeof(tz_app_vendor_if_send_cmd_t),
            &cmd_rsp_, sizeof(tz_app_vendor_if_send_cmd_rsp_t));
    if (retval) {
        ALOGE("exchangeTimestampWithTA - send command to TZ failed (%d)",
            retval);
    }

    ret = cmd_rsp_.ret_data;
    ALOGI("exchangeTimestampWithTA - ret_data is (%d)", ret);
    unlock();

    return ret;
}

//=========================================================================
extern "C" jlong Java_com_qualcomm_qti_seccamservice_SecCamServiceVendorHandler_startMLVMApp(
    JNIEnv* env, jobject thiz) {

    jlong ret = 0;
    ALOGD("startMLApp - Enter");

    lock();

    int retval = secCamStart();

    if (retval) {
        ALOGE("startMLVMApp - send command to MLVM failed (%d)",
            retval);
    }

    ALOGI("startMLApp - ret_data is (%d)", retval);
    unlock();

    return ret;
}

//=========================================================================
extern "C" jlong Java_com_qualcomm_qti_seccamservice_SecCamServiceVendorHandler_stopMLVMApp(
    JNIEnv* env, jobject thiz) {

    jlong ret = 0;
    ALOGD("stopMLApp - Enter");

    lock();

    int retval = secCamStop();

    if (retval) {
        ALOGE("stopMLVMApp - send command to MLVM failed (%d)",
            retval);
    }

    ALOGI("stopMLApp - ret_data is (%d)", retval);
    unlock();

    return ret;
}
//=========================================================================
extern "C" jlong Java_com_qualcomm_qti_seccamservice_SecCamServiceVendorHandler_exchangeTimestampWithML(
    JNIEnv* env, jobject thiz, jlong hlosTimestamp) {

    jlong ret = 0;
    ALOGD("exchangeTimestampWithML - Enter");

    lock();

    ml_app_vendor_if_send_cmd_t cmd_;
    ml_app_vendor_if_send_cmd_rsp_t cmd_rsp_;
    memset(&cmd_, 0, sizeof(ml_app_vendor_if_send_cmd_t));
    memset(&cmd_rsp_, 0, sizeof(ml_app_vendor_if_send_cmd_rsp_t));
    cmd_.cmd_id = ML_APP_IF_CMD_VENDOR_EXCHANGE_TIMESTAMP;
    cmd_.cmd_data = hlosTimestamp;

    int retval = secCamSendCmd(&cmd_, sizeof(ml_app_vendor_if_send_cmd_t),
            &cmd_rsp_, sizeof(ml_app_vendor_if_send_cmd_rsp_t));
    if (retval) {
        ALOGE("exchangeTimestampWithTA - send command to TZ failed (%d)",
            retval);
    }

    ret = cmd_rsp_.ret_data;
    ALOGI("exchangeTimestampWithTA - ret_data is (%d)", ret);
    unlock();

    return ret;
}
#ifdef HAS_SECURE_PROCESSOR

//=========================================================================
extern "C" jlong Java_com_qualcomm_qti_seccamservice_SecCamServiceVendorHandler_initBiometricsBufferAndShareWithTA(
    JNIEnv* env, jobject thiz, jlong bufferSize) {
    tz_app_vendor_if_init_biometrics_cmd_t cmd_;
    tz_app_vendor_if_send_cmd_rsp_t cmd_rsp_;
    QSEECom_ion_fd_info ion_fd_info;
    uint32_t enroll_template_size;
    int32_t ion_data_fd;
    int retval;
    jlong ret = 0;

    ALOGD("initBiometricsBufferAndShareWithTA - Enter");

    if (bufferSize < 0 || bufferSize > 0xffffffffL) {
        return TZ_APP_IF_STATUS_VENDOR_INVALID_INPUT_PARAMS;
    }
    enroll_template_size = (uint32_t)bufferSize;

    lock();
    do { // lock scope

        memset(&cmd_, 0, sizeof(tz_app_vendor_if_init_biometrics_cmd_t));
        memset(&cmd_rsp_, 0, sizeof(tz_app_vendor_if_send_cmd_rsp_t));
        cmd_.cmd_id = TZ_APP_IF_CMD_VENDOR_INIT_BIOMETRICS_BUFFER;
        cmd_.buffer_base_address = 0; // send modified command will fill the physical address here
        cmd_.buffer_length = bufferSize;

        // initialize ION buffer only
        retval = sp_iris_init(&enroll_template_size, &ion_data_fd);
        if (retval) {
            ALOGE("initBiometricsBufferAndShareWithTA - sp_iris_init failed %d",
                retval);
            ret = retval;
            break;
        }
        ALOGD("sp_iris_init created fd 0x%x", ion_data_fd);

        memset(&ion_fd_info, 0, sizeof(struct QSEECom_ion_fd_info));
        ion_fd_info.data[0].fd = ion_data_fd;
        ion_fd_info.data[0].cmd_buf_offset = offsetof(tz_app_vendor_if_init_biometrics_cmd_t, buffer_base_address);
        ion_fd_info.data[1].fd = -1;
        ion_fd_info.data[2].fd = -1;
        ion_fd_info.data[3].fd = -1;

        for (unsigned int i = 0; i < 4; ++i) {
            ALOGD("ion_fd_info.data[%d].fd = %d",             i, ion_fd_info.data[i].fd);
            ALOGD("ion_fd_info.data[%d].cmd_buf_offset = %d", i, ion_fd_info.data[i].cmd_buf_offset);
        }

        retval = secCamSendModifiedCmd(
                    &cmd_, sizeof(tz_app_vendor_if_init_biometrics_cmd_t),
                    &cmd_rsp_, sizeof(tz_app_vendor_if_send_cmd_rsp_t),
                    &ion_fd_info);

        if (retval) {
            ALOGE("initBiometricsBufferAndShareWithTA - send command to TZ failed (%d)",
                    retval);
            ret = retval;
            break;
        }

        ret = cmd_rsp_.ret_data;
        ALOGI("initBiometricsBufferAndShareWithTA - ret_data is (%d)", ret);

    } while (false);

    unlock();
    return ret;
}

#else

extern "C" jlong Java_com_qualcomm_qti_seccamservice_SecCamServiceVendorHandler_initBiometricsBufferAndShareWithTA(
    JNIEnv* env, jobject thiz, jlong bufferSize) {
	return -1; // not implemented
}
#endif


//=========================================================================
extern "C" jlong Java_com_qualcomm_qti_seccamservice_SecCamServiceVendorHandler_runBiometricsTestOnTA(
    JNIEnv* env, jobject thiz, jlong test_num, jlong bufferSize) {
    tz_app_vendor_if_run_biometrics_test_cmd_t cmd_;
    tz_app_vendor_if_send_cmd_rsp_t cmd_rsp_;
    int retval;
    jlong ret = 0;

    ALOGD("runBiometricsTestOnTA - Enter");

    lock();
    do { // lock scope

        memset(&cmd_, 0, sizeof(tz_app_vendor_if_run_biometrics_test_cmd_t));
        memset(&cmd_rsp_, 0, sizeof(tz_app_vendor_if_send_cmd_rsp_t));
        cmd_.cmd_id = TZ_APP_IF_CMD_VENDOR_RUN_BIOMETRICS_TEST;
        cmd_.test_number = test_num;
        cmd_.buffer_length = bufferSize;

        retval = secCamSendCmd(
                    &cmd_, sizeof(tz_app_vendor_if_run_biometrics_test_cmd_t),
                    &cmd_rsp_, sizeof(tz_app_vendor_if_send_cmd_rsp_t));

        if (retval) {
            ALOGE("runBiometricsTestOnTA - send command to TZ failed (%d)",
                    retval);
            ret = retval;
            break;
        }

        ret = cmd_rsp_.ret_data;
        ALOGI("runBiometricsTestOnTA - ret_data is (%d)", ret);

    } while (false);

    unlock();
    return ret;
}


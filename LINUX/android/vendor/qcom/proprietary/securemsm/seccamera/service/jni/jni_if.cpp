/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
=============================================================

                          EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ------------------------------------------
09/19/17   ak      Implement generic return_params approach
08/15/17   ak      Switch to generic C IF
06/25/17   ak      Split to NDK and LibGUI oriented implementations
06/07/17   ak      Add Camera2 API support
03/22/17   ak      Add Frame Available callback support
12/25/16   ak      Initial version
=============================================================*/
#include <map>
#include <string>
#include <pthread.h>
#include <errno.h>
#include <android/native_window_jni.h>
#include <utils/Log.h>

#include "seccam_if.h"

#ifdef LOG_TAG
    #undef LOG_TAG
#endif
#define LOG_TAG "SECCAM-JNI"

#define JNI_IF_VERSION_MAJ  2
#define JNI_IF_VERSION_MIN  3

#define SECCAM_FRAME_INFO_LOCK_TIMEOUT  2000

class SecCamFrameInfo {
public:
    SecCamFrameInfo(uint32_t return_params_size) {
        return_params_size_ = return_params_size;
        return_params_ = new uint8_t[return_params_size_];
        frame_ready_ = false;
        pthread_mutex_init(&frame_ready_mutex_, nullptr);
        pthread_cond_init(&frame_ready_cond_, nullptr);
    }

    ~SecCamFrameInfo() {
        delete return_params_;
        pthread_mutex_destroy(&frame_ready_mutex_);
        pthread_cond_destroy(&frame_ready_cond_);
    }

    status_t setFrameInfo(
            uint64_t frame_number, int64_t time_stamp,
            uint32_t width, uint32_t height, uint32_t stride,
            uint32_t format,
            const void* custom_rsp, uint32_t custom_rsp_size);

    int32_t getFrameInfo(uint32_t timeout_ms,
             uint64_t* number,
             int64_t* time_stamp,
             uint32_t* width,
             uint32_t* height,
             uint32_t* stride,
             uint32_t* format,
             uint8_t* return_params, uint32_t return_params_size);

    uint32_t  getReturnParamsSize() {
        return return_params_size_;
    }

    static SecCamFrameInfo* acquire(uint64_t surface_id);
    static void release(SecCamFrameInfo* frame_info);
    static SecCamFrameInfo* insert(uint64_t surface_id, uint32_t return_params_size);
    static void erase(uint64_t surface_id);

private:
    static void mapAccessUnlock();
    static status_t mapAccessWaitAndLock();
    static uint32_t mapAccessAcquire();
    static uint32_t mapAccessRelease();

private:
    pthread_cond_t     frame_ready_cond_;
    pthread_mutex_t    frame_ready_mutex_;

    bool        frame_ready_;
    uint64_t    frame_number_;
    int64_t     frame_time_stamp_;
    uint32_t    frame_width_;
    uint32_t    frame_height_;
    uint32_t    frame_stride_;
    uint32_t    frame_format_;
    uint8_t*    return_params_;
    uint32_t    return_params_size_;

    typedef std::map<uint64_t, SecCamFrameInfo*> seccam_frame_info_map_t;
    static seccam_frame_info_map_t seccam_frame_info_map_;

    static pthread_cond_t map_access_cond_;
    static pthread_mutex_t map_access_mutex_;
    static pthread_mutex_t map_insert_mutex_;
    static uint32_t map_access_counter_;
};

SecCamFrameInfo::seccam_frame_info_map_t SecCamFrameInfo::seccam_frame_info_map_;
pthread_cond_t SecCamFrameInfo::map_access_cond_ = PTHREAD_COND_INITIALIZER;
pthread_mutex_t SecCamFrameInfo::map_access_mutex_ = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t SecCamFrameInfo::map_insert_mutex_ = PTHREAD_MUTEX_INITIALIZER;
uint32_t SecCamFrameInfo::map_access_counter_ = 0;

//=========================================================================
SecCamFrameInfo* SecCamFrameInfo::acquire(uint64_t surface_id) {
    mapAccessAcquire();
    pthread_mutex_lock(&map_insert_mutex_);
    seccam_frame_info_map_t::iterator frame_info_iter =
            seccam_frame_info_map_.find(surface_id);
    pthread_mutex_unlock(&map_insert_mutex_);
    if (frame_info_iter != seccam_frame_info_map_.end() ) {
        return frame_info_iter->second;
    }
    return nullptr;
}

//=========================================================================
void SecCamFrameInfo::release(SecCamFrameInfo* frame_info) {
    mapAccessRelease();
}

//=========================================================================
SecCamFrameInfo* SecCamFrameInfo::insert(uint64_t surface_id, uint32_t return_params_size) {
    pthread_mutex_lock(&map_insert_mutex_);
    SecCamFrameInfo* new_frame_info = new SecCamFrameInfo(return_params_size);
    seccam_frame_info_map_.insert(std::pair<uint64_t, SecCamFrameInfo*> (surface_id,
            new_frame_info));
    pthread_mutex_unlock(&map_insert_mutex_);
    return new_frame_info;
}

//=========================================================================
void SecCamFrameInfo::erase(uint64_t surface_id) {
    mapAccessWaitAndLock();
    pthread_mutex_lock(&map_insert_mutex_);
    seccam_frame_info_map_t::iterator frame_info_iter =
            seccam_frame_info_map_.find(surface_id);
    if (frame_info_iter != seccam_frame_info_map_.end() ) {
        delete frame_info_iter->second;
        seccam_frame_info_map_.erase(frame_info_iter);
    }
    pthread_mutex_unlock(&map_insert_mutex_);
    mapAccessUnlock();
}

//=========================================================================
void SecCamFrameInfo::mapAccessUnlock() {
    pthread_mutex_unlock(&map_access_mutex_);
}

//=========================================================================
status_t SecCamFrameInfo::mapAccessWaitAndLock() {
    status_t ret = OK;
    pthread_mutex_lock(&map_access_mutex_);
    if (0 == map_access_counter_) {
        return ret;
    }
    uint32_t timeout_ms = SECCAM_FRAME_INFO_LOCK_TIMEOUT;
    struct timespec mutex_time;
    memset(&mutex_time, 0, sizeof(mutex_time));
    clock_gettime(CLOCK_REALTIME, &mutex_time);
    uint64_t timeout_ns = mutex_time.tv_nsec + (uint64_t)timeout_ms * (1000*1000);
    mutex_time.tv_nsec = timeout_ns % (1000*1000*1000);
    mutex_time.tv_sec += timeout_ns / (1000*1000*1000);

    //"-" required to convert errno to android::status_t
    ret = -pthread_cond_timedwait(
            &map_access_cond_,
            &map_access_mutex_,
            &mutex_time);
    if (ret == TIMED_OUT) {
        ALOGD( "mapAccessWaitAndLock - timeout %dms, %s (%d)",
                timeout_ms, strerror(-ret), -ret);
    }
    return ret;
}

//=========================================================================
uint32_t SecCamFrameInfo::mapAccessAcquire() {
    pthread_mutex_lock(&map_access_mutex_);
    map_access_counter_++;
    uint32_t ret = map_access_counter_;
    pthread_mutex_unlock(&map_access_mutex_);
    return ret;
}

//=========================================================================
uint32_t SecCamFrameInfo::mapAccessRelease() {
    pthread_mutex_lock(&map_access_mutex_);
    if(map_access_counter_ > 0) {
        map_access_counter_--;
    }
    if (0 == map_access_counter_) {
        pthread_cond_signal(&map_access_cond_);
    }
    uint32_t ret = map_access_counter_;
    pthread_mutex_unlock(&map_access_mutex_);
    return ret;
}

//=========================================================================
status_t SecCamFrameInfo::setFrameInfo(
        uint64_t frame_number, int64_t time_stamp,
        uint32_t width, uint32_t height, uint32_t stride,
        uint32_t format,
        const void* custom_rsp, uint32_t custom_rsp_size) {
    pthread_mutex_lock(&frame_ready_mutex_);
    status_t ret = OK;
    if (custom_rsp_size != return_params_size_ ||
            custom_rsp == nullptr || return_params_ == nullptr) {
        ALOGE("setFrameInfo - Incorrect return parameters buffer");
        frame_ready_ = false;
        ret = BAD_VALUE;
    }
    else {
        frame_number_ = frame_number;
        frame_time_stamp_ = time_stamp;
        frame_width_ = width;
        frame_height_ = height;
        frame_stride_ = stride;
        frame_format_ = format;
        memcpy(return_params_,  custom_rsp, return_params_size_);
        frame_ready_ = true;
        pthread_cond_signal(&frame_ready_cond_);
    }
    pthread_mutex_unlock(&frame_ready_mutex_);
    return ret;
}

//=========================================================================
status_t SecCamFrameInfo::getFrameInfo(uint32_t timeout_ms,
        uint64_t* number, int64_t* time_stamp,
        uint32_t* width, uint32_t* height, uint32_t* stride,
        uint32_t* format,
        uint8_t* return_params, uint32_t return_params_size) {
    status_t ret = UNKNOWN_ERROR;

    pthread_mutex_lock(&frame_ready_mutex_);
    ALOGD("getFrameInfo - Enter");
    if(!frame_ready_) {
        struct timespec mutex_time;
        memset(&mutex_time, 0, sizeof(mutex_time));
        clock_gettime(CLOCK_REALTIME, &mutex_time);
        uint64_t timeout_ns = mutex_time.tv_nsec + (uint64_t)timeout_ms * (1000*1000);
        mutex_time.tv_nsec = timeout_ns % (1000*1000*1000);
        mutex_time.tv_sec += timeout_ns / (1000*1000*1000);

        //"-" required to convert errno to android::status_t
        ret = -pthread_cond_timedwait(&frame_ready_cond_, &frame_ready_mutex_, &mutex_time);
        if (ret == TIMED_OUT) {
            ALOGD( "getFrameInfo - timeout %dms, %s (%d)",
                    timeout_ms, strerror(-ret), -ret);
        }
    }
    else {
        ret = OK;
    }
    if (OK == ret) {
        if (return_params_size != return_params_size_ ||
                return_params == nullptr || return_params_ == nullptr) {
            ALOGE("getFrameInfo - Incorrect return parameters buffer");
            ret = BAD_VALUE;
        }
        else {
            *number = frame_number_;
            *time_stamp = frame_time_stamp_;
            *width = frame_width_;
            *height = frame_height_;
            *stride = frame_stride_;
            *format = frame_format_;
            memcpy(return_params, return_params_, return_params_size_);
            ALOGD("getFrameInfo - Done, FrameId = %d", frame_number_);
        }
    }
    frame_ready_ = false;
    pthread_mutex_unlock(&frame_ready_mutex_);
    return ret;
}

//=========================================================================
//                       Frame Info Callbacks
//=========================================================================

//=========================================================================
seccam_if_notif_action_t secCamWillSendFrameNotification(
        uint64_t surface_id,
        seccam_if_frame_req_data_t* frame_data, uint32_t len,
        void** context,
        void** custom_cmd, uint32_t* custom_cmd_size,
        uint32_t* custom_rsp_size) {
    ALOGD("secCamWillSendFrameNotification - Enter");

    SecCamFrameInfo* frame_info = SecCamFrameInfo::acquire(surface_id);
    if (nullptr == frame_info) {
        ALOGE("secCamWillSendFrameNotification - no frame info (ID = %lld)!",
                (long long)surface_id);
        *custom_rsp_size = 0;
    }
    else {
        *custom_rsp_size = frame_info->getReturnParamsSize();
    }
    *context = (void*)frame_data;
    SecCamFrameInfo::release(frame_info);
    return SECCAM_IF_NOTIF_ACTION_CONTINUE;
}

//=========================================================================
seccam_if_notif_action_t secCamDoneSendFrameNotification(
        uint64_t surface_id,
        int32_t ret, seccam_if_frame_rsp_data_t* rsp,
        void* context,
        const void* custom_rsp, uint32_t custom_rsp_size) {
    ALOGD("secCamDoneSendFrameNotification - Enter");

    if (nullptr == rsp || nullptr == context || nullptr == custom_rsp) {
        ALOGE("secCamDoneSendFrameNotification - bad frame info (ID = %lld)!",
                (long long)surface_id);
    }
    else {
        SecCamFrameInfo* frame_info = SecCamFrameInfo::acquire(surface_id);
        seccam_if_frame_req_data_t* frame_data = (seccam_if_frame_req_data_t*)context;
        if (nullptr == frame_info) {
            ALOGE("secCamDoneSendFrameNotification - no frame info (ID = %lld)!",
                    (long long)surface_id);
        }
        else {
            frame_info->setFrameInfo(
                    frame_data[0].frame_number,
                    frame_data[0].time_stamp,
                    frame_data[0].in_buffer.width,
                    frame_data[0].in_buffer.height,
                    frame_data[0].in_buffer.stride,
                    frame_data[0].in_buffer.format,
                    custom_rsp, custom_rsp_size);
        }
        SecCamFrameInfo::release(frame_info);
    }
    return SECCAM_IF_NOTIF_ACTION_CONTINUE;
}
//=========================================================================
//                           JNI API
//=========================================================================

//=========================================================================
extern "C" jboolean Java_com_qualcomm_qti_seccamservice_SecCamService_getJniVersion(
        JNIEnv* env,
        jobject thiz,
        jobject j_ver_info_object) {
    jclass cls = env->GetObjectClass(j_ver_info_object);
    if (nullptr == cls) {
        ALOGE("getJniVersion - invalid appInfo object");
        return false;
    }
    jfieldID field_id;
    field_id = env->GetFieldID(cls, "verMaj_", "I");
    env->SetIntField(j_ver_info_object, field_id, JNI_IF_VERSION_MAJ);
    field_id = env->GetFieldID(cls, "verMin_", "I");
    env->SetIntField(j_ver_info_object, field_id, JNI_IF_VERSION_MIN);
    ALOGI("getJniVersion - JNI ver: %d.%d",
        JNI_IF_VERSION_MAJ, JNI_IF_VERSION_MIN);
    return true;
}
//=========================================================================
extern "C" jint Java_com_qualcomm_qti_seccamservice_SecCamService_doStartTzApp(
        JNIEnv* env,
        jobject thiz,
        jstring j_app_name,
        jint j_buff_size,
        jobject j_app_info_object,
        jboolean j_is_2dfa) {
    ALOGD("doStartTzApp - Enter");
    const char *s = env->GetStringUTFChars(j_app_name, 0);
    std::string app_name(s);
    env->ReleaseStringUTFChars(j_app_name, s);

    jclass cls = env->GetObjectClass(j_app_info_object);
    if (nullptr == cls) {
        ALOGE("doStartTzApp - invalid appInfo object");
        return (jint)BAD_VALUE;
    }

    uint32_t if_version_maj = 0;
    uint32_t if_version_min = 0;

    int32_t ret = secCamStartTzApp(app_name.c_str(),
            j_buff_size, &if_version_maj, &if_version_min, j_is_2dfa);
    if (OK == ret) {
        jfieldID field_id;
        field_id = env->GetFieldID(cls, "verMaj_", "I");
        env->SetIntField(j_app_info_object, field_id, if_version_maj);
        field_id = env->GetFieldID(cls, "verMin_", "I");
        env->SetIntField(j_app_info_object, field_id, if_version_min);
    }
    return (jint)ret;
}

//=========================================================================
extern "C" jint Java_com_qualcomm_qti_seccamservice_SecCamService_doShutdownTzApp(
        JNIEnv* env,
        jobject thiz) {
    ALOGD("doShutdownTzApp - Enter");
    return (jint)secCamShutdownTzApp();
}
//=========================================================================
extern "C" jobject Java_com_qualcomm_qti_seccamservice_SecCamService_getSecureCameraSurface(
    JNIEnv* env, jobject thiz,
    jint j_camera_id, jint j_width, jint j_height, jint j_format, jint j_max_images,
    jobject j_surface_info_object) {
    jclass cls = env->GetObjectClass(j_surface_info_object);
    if (nullptr == cls) {
        ALOGE("getSecureCameraSurface - invalid surfaceInfo object");
        return nullptr;
    }
    jobject surface = secCamGetSecureCameraSurface(
            env, thiz,
            j_camera_id, j_width, j_height, j_format, j_max_images,
            j_surface_info_object);

    jfieldID field_id;
    field_id = env->GetFieldID(cls, "surfaceId_", "J");
    jlong surface_id = env->GetLongField(j_surface_info_object, field_id);
    return surface;
}

//=========================================================================
extern "C" jboolean Java_com_qualcomm_qti_seccamservice_SecCamService_releaseSecureCameraSurface(
    JNIEnv* env, jobject thiz,
    jlong j_surface_id) {
    ALOGD("releaseCaptureSurface - Enter");
    bool ret = secCamReleaseSecureCameraSurface(j_surface_id);

    SecCamFrameInfo::erase(j_surface_id);
    return (jboolean)ret;
}

//=========================================================================
extern "C" jboolean Java_com_qualcomm_qti_seccamservice_SecCamService_setSecurePreviewSurface(
    JNIEnv* env, jobject thiz,
    jobject j_obj_preview_surface, jlong j_capture_surface_id,
    jint j_width, jint j_height, jint j_format, jint j_rotation, jint j_max_images) {

    jboolean ret = true;
    status_t err;

    ALOGD("setSecurePreviewSurface - Enter");
    ANativeWindow* preview_native_vindow = ANativeWindow_fromSurface(env, j_obj_preview_surface);
    if (preview_native_vindow == nullptr) {
        ALOGE( "setSecurePreviewSurface - ANativeWindow_fromSurface failed");
        return false;
    }
    return (jboolean)secCamSetSecurePreviewSurface(preview_native_vindow, j_capture_surface_id,
            j_width, j_height, j_format, j_rotation, j_max_images);
}

//=========================================================================
extern "C" jboolean Java_com_qualcomm_qti_seccamservice_SecCamService_releaseSecurePreviewSurface(
    JNIEnv* env, jobject thiz,
    jobject j_obj_preview_surface, jlong j_capture_surface_id) {

    status_t err;
    jboolean ret = true;
    ALOGD("releaseSecurePreviewSurface - Enter");

    ANativeWindow* native_vindow = ANativeWindow_fromSurface(env, j_obj_preview_surface);
    if (native_vindow == nullptr) {
        ALOGE( "releaseSecurePreviewSurface - ANativeWindow_fromSurface failed");
        return false;
    }
    return (jboolean)secCamReleaseSecurePreviewSurface(native_vindow, j_capture_surface_id);
}

//=========================================================================
extern "C" jint Java_com_qualcomm_qti_seccamservice_SecCamService_isFrameAvailable(
    JNIEnv* env, jobject thiz,
    jlong j_surface_id, jint j_timeout,
    jobject j_frame_info_object,
    jbyteArray j_return_params, jint j_return_params_size) {

    status_t ret = UNKNOWN_ERROR;
    ALOGD("isFrameAvailable - Enter");

    uint64_t surface_id = (uint64_t)j_surface_id;
    uint64_t number = 0;
    int64_t time_stamp = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t stride = 0;
    uint32_t format = 0;
    uint8_t* return_params = nullptr;
    uint32_t return_params_size;

    SecCamFrameInfo* frame_info = SecCamFrameInfo::acquire(surface_id);
    if (nullptr == frame_info) {
        ALOGE( "isFrameAvailable - No Frame Info found (ID = %lld)!",
                (long long)surface_id);
    }
    else {
        return_params_size = frame_info->getReturnParamsSize();
        if ((uint32_t)j_return_params_size != return_params_size) {
            ALOGE("isFrameAvailable - Invalid size of return params array");
            SecCamFrameInfo::release(frame_info);
            return (jint)BAD_VALUE;
        }
        return_params = new (std::nothrow) uint8_t[return_params_size];
        if(!return_params) {
            ALOGE("Alloc failed for return_params (%d bytes)", return_params_size);
            SecCamFrameInfo::release(frame_info);
            return (jint)NO_MEMORY;
        }

        ret = frame_info->getFrameInfo(j_timeout,
                &number, &time_stamp,
                &width, &height, &stride,
                &format,
                return_params, return_params_size);
        if (OK == ret) {
            jclass cls = env->GetObjectClass(j_frame_info_object);
            if (nullptr == cls) {
                ALOGE("isFrameAvailable - Invalid frameInfo object");
                ret = BAD_VALUE;
            }
            else {
                jfieldID field_id;
                field_id = env->GetFieldID(cls, "frameNumber_", "J");
                env->SetLongField(j_frame_info_object, field_id, number);
                field_id = env->GetFieldID(cls, "timeStamp_", "J");
                env->SetLongField(j_frame_info_object, field_id, time_stamp);

                field_id = env->GetFieldID(cls, "width_", "I");
                env->SetIntField(j_frame_info_object, field_id, width);
                field_id = env->GetFieldID(cls, "height_", "I");
                env->SetIntField(j_frame_info_object, field_id, height);
                field_id = env->GetFieldID(cls, "stride_", "I");
                env->SetIntField(j_frame_info_object, field_id, stride);
                field_id = env->GetFieldID(cls, "format_", "I");
                env->SetIntField(j_frame_info_object, field_id, format);
                //return parameters
                env->SetByteArrayRegion (j_return_params, 0, return_params_size,
                        (int8_t*)return_params);
            }
        }
        delete return_params;
    }
    SecCamFrameInfo::release(frame_info);
    return (jint)ret;
}

//=========================================================================
extern "C" jboolean Java_com_qualcomm_qti_seccamservice_SecCamService_enableFrameCallback(
    JNIEnv* env, jobject thiz,
    jlong j_surface_id, jint return_params_size) {
    ALOGD("enableFrameCallback - Enter");

    bool ret = secCamEnableFrameCallback(j_surface_id,
            secCamWillSendFrameNotification, secCamDoneSendFrameNotification);
    if (ret) {
        SecCamFrameInfo::insert(j_surface_id, return_params_size);
    }
    return (jboolean)ret;
}

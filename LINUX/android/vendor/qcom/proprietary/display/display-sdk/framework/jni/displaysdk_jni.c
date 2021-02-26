/* ====================================================================
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================
 * @file displaysdk_jni.c
 *
 */
#include <stdbool.h>
#include <stdlib.h>
#include <jni.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <android/log.h>
#include "disp_color_apis.h"

#define SUCCESS 0
#define LOG(msg)   __android_log_print(ANDROID_LOG_DEBUG, "IColorServiceImpl", msg);
#define PARAM_LOG(fmt, args...)     __android_log_print(ANDROID_LOG_DEBUG, "IColorServiceImpl", fmt, ##args)

DISPAPI_HANDLE ctx;

typedef enum {
    DISP_COLOR_BALANCE_API,
    DISP_COLOR_MODE_SEL_API,
    DISP_COLOR_MODE_MANAGEMENT_API,
    DISP_ADAPTIVE_BACKLIGHT_API,
    DISP_GLOBAL_PICT_ADJ_API,
    DISP_MEM_COLOR_ADJ_API,
    DISP_SUNLIGHT_VISIBILITY_API,
} display_old_api_type;

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1isFeatureSupported(JNIEnv * env, jobject this,
        jint displayId, jint feature) {
    PARAM_LOG("JNI: IsfeatureSupported() called in native with values Display: %d and feature: %d", displayId, feature);

    int feature_supported = 0;
    int ret = -1;
    switch (feature) {
        case DISP_COLOR_BALANCE_API:
            if (displayId == 0)
                feature_supported = 1;
            break;
        case DISP_COLOR_MODE_SEL_API:
            feature_supported = 1;
            break;
        case DISP_COLOR_MODE_MANAGEMENT_API:
            feature_supported = 1;
            break;
        case DISP_ADAPTIVE_BACKLIGHT_API:
            if (displayId == 0)
                feature_supported = 0;
            break;
        case DISP_GLOBAL_PICT_ADJ_API:
            feature_supported = 1;
            break;
        case DISP_MEM_COLOR_ADJ_API:
            feature_supported = 0;
            break;
        case DISP_SUNLIGHT_VISIBILITY_API:
            if (displayId == 0)
                feature_supported = 0;
            break;
        default:
            break;
    }
    return feature_supported;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1init(JNIEnv* env, jobject this) {
    int32_t err = 0;
    err = disp_api_init((DISPAPI_HANDLE*) &ctx, 0);
    return err;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1deinit(JNIEnv* env, jobject this) {
    jint ret = 0;
    int32_t err = 0;
    err = disp_api_deinit((DISPAPI_HANDLE) ctx, 0);
    if(!err) {
      ret = 1;
    }
    return ret;
}

//ColorBalance
jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1setColorBalance(JNIEnv* env, jobject this,
        jint displayId, jint warmth) {
    //0 means pass, -22 refer ERRNO.h
    PARAM_LOG("JNI: Passing warmth %d for display %d", warmth, displayId);
    return disp_api_set_global_color_balance((DISPAPI_HANDLE) ctx, (uint32_t)displayId, (int32_t) warmth, 0);
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1getColorBalance(JNIEnv* env, jobject this,
        jint displayId) {
    PARAM_LOG("JNI: Asking Color Balance for display %d", displayId);
    jint warmth = -101; //lowest warmth is -100
    uint32_t flags = 0;
    disp_api_get_global_color_balance((DISPAPI_HANDLE) ctx, (uint32_t) displayId, (int32_t*)&warmth, &flags);
    PARAM_LOG("JNI: get_color_balance returns with warmth %d", warmth);
    return warmth;
}
//End ColorBalance

//Mode
jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1getNumberOfModes(JNIEnv* env, jobject this, jint displayId, jint modeType)
{
    PARAM_LOG("JNI: Asking num of modes of type %d", modeType);
    jint nummodes = -1;
    uint32_t flags = 0;
    int ret = disp_api_get_num_display_modes ((DISPAPI_HANDLE) ctx, (uint32_t) displayId, (disp_mode_type)modeType,
                                          (int32_t *)&nummodes, &flags);
    PARAM_LOG("JNI: num_modes returns %d with count %d", ret, nummodes);
    return nummodes;
}

JNIEXPORT jobjectArray JNICALL Java_com_qti_service_colorservice_IColorServiceImpl_native_1getModes(JNIEnv *env, jobject this, jint displayId, jint modeType)
{
    jstring str = NULL;
    int i;
    jclass complexClass = (*env)->FindClass(env,"com/qti/snapdragon/sdk/display/ModeInfo");
    LOG("JNI: native getModes called");
    if(complexClass == NULL){
        LOG("JNI: Class not found for modes");
        return NULL;
    }
    jmethodID constructor = (*env)->GetMethodID(env,complexClass, "<init>", "(ILjava/lang/String;I)V"); //The name of constructor method is "<init>"
    if(constructor == NULL){
        LOG("JNI: Method not found for modes");
        return NULL;
    }

    int modeCount = -1;
    uint32_t flags = 0;
    int retVal = disp_api_get_num_display_modes((DISPAPI_HANDLE) ctx, (uint32_t) displayId,
                                           (disp_mode_type) modeType,
                                           (int32_t *) &modeCount, &flags);
    if(SUCCESS == retVal){
        if(modeCount <= 0){
            return NULL;
        }
        jobjectArray objArray = (*env)->NewObjectArray(env,modeCount,complexClass,NULL);
        if(objArray == NULL){
            LOG("JNI: Couldn't instantiate object array for modes");
            return NULL;
        }

        struct disp_mode modes[modeCount];
        int j =0; //TODO: get rid of this loop and free stmt once fixed in native interface
        for(j = 0; j < modeCount; j++)
        {
            modes[j].name = malloc(255);
            modes[j].name_len = 255;
        }
        flags = 0;
        retVal = disp_api_get_display_modes((DISPAPI_HANDLE) ctx, (uint32_t) displayId, (disp_mode_type)modeType,
                                           &modes[0], modeCount, &flags);
        PARAM_LOG("JNI: getModes finished with count %d", modeCount);
        if(retVal == SUCCESS){
            for(i = 0; i < modeCount; i++){
                str = (*env)->NewStringUTF(env, modes[i].name);
                jobject instance = (*env)->NewObject(env,complexClass, constructor, modes[i].id, str, (int)modes[i].type);
                (*env)->SetObjectArrayElement(env,objArray,i,instance);
                (*env)->DeleteLocalRef(env,str);
                (*env)->DeleteLocalRef(env,instance);
                free(modes[i].name);
            }
        }
        else{
            PARAM_LOG("JNI: get_modes returned %d", retVal);
            return NULL;
        }
        LOG("JNI: got the array");
        return objArray;
    }
    else{
        PARAM_LOG("JNI: num of modes returned err: %d", retVal);
        return NULL;
    }
    //end
}

jlongArray Java_com_qti_service_colorservice_IColorServiceImpl_native_1getActiveMode(JNIEnv* env, jobject this, jint displayId)
{
    PARAM_LOG("JNI: Asking Active Mode for display %d", displayId);
    int active_mode = -1;
    uint32_t flag = 0;
    uint32_t mask = 0;
    jlong narr[2];

    int ret = disp_api_get_active_display_mode((DISPAPI_HANDLE) ctx, (uint32_t) displayId,
                                             (int32_t*) &active_mode, (uint32_t *) &mask, (uint32_t *) &flag);

    if (SUCCESS == ret) {
        narr[0] = active_mode;
        narr[1] = flag;
    } else {
        narr[0] = -1;
        narr[1] = 0;
    }
    PARAM_LOG("JNI: get_active_mode returns %d with id %d and flag %d", ret, active_mode, flag);
    jlongArray newArray = (*env)->NewLongArray(env, 2);
    (*env)->SetLongArrayRegion(env, newArray, 0, 2, narr);
    return newArray;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1setActiveMode(JNIEnv* env, jobject this, jint displayId, jint modeId)
{
    PARAM_LOG("JNI: Passing modeId %d for display %d", modeId, displayId);
    uint32_t flags = 0;
    return disp_api_set_active_display_mode((DISPAPI_HANDLE) ctx, (uint32_t) displayId, (int32_t) modeId, flags);
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1deleteMode(JNIEnv* env, jobject this, jint displayId, jint modeId)
{
    PARAM_LOG("JNI: Passing displayId %d and modeId %d for deletion", displayId, modeId);
    uint32_t flags = 0;
    return disp_api_delete_display_mode((DISPAPI_HANDLE) ctx, (int32_t) modeId, flags);
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1createNewMode(JNIEnv* env, jobject this, jint displayId, jstring name, jint nameLen, jlong flag, jint cbWarmth)
{
    PARAM_LOG("CreateNewMode not yet supported. Note: This API call is currently not used.");
    return -1;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1modifyMode(JNIEnv* env, jobject this, jint displayId, jint modeId, jstring name, jint nameLen, jlong flag, jint cbWarmth)
{
    PARAM_LOG("ModifyMode not yet supported. Note: This API call is currently not used.");
    return -1;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1createNewModeAllFeatures(JNIEnv* env, jobject this, jint displayId, jstring name, jint nameLen)
{
    char *modeName = (char*)((*env)->GetStringUTFChars(env, name, 0));
    PARAM_LOG("JNI: Passing displayId %d with name %s for saving", displayId, modeName);
    int32_t mode_id = -1;
    uint32_t flags = 0;

    int ret = disp_api_save_display_mode((DISPAPI_HANDLE) ctx, (uint32_t) displayId, modeName,
                                   (uint32_t) nameLen, (int32_t*) &mode_id, flags);

    if (SUCCESS == ret) {
        PARAM_LOG("JNI: create_new_mode_all_features returns %d with mode id %d", ret, mode_id);
        jint newModeId = mode_id;
        return newModeId;
    } else {
        PARAM_LOG("JNI: create_new_mode_all_features failed");
        jint newModeId = -1;
        return newModeId;
    }
    return -1;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1modifyModeAllFeatures(JNIEnv* env, jobject this, jint displayId, jint modeId, jstring name, jint nameLen)
{
    char *modeName = (char*)((*env)->GetStringUTFChars(env, name, 0));
    PARAM_LOG("JNI: Passing displayId %d with name %s and id %d for saving", displayId, modeName, modeId);
    int32_t mode_id = (int32_t)modeId;
    uint32_t flags = 0;

    int ret = disp_api_save_display_mode((DISPAPI_HANDLE) ctx, (uint32_t) displayId, modeName,
                                   (uint32_t) nameLen, (int32_t*) &mode_id, flags);

    if (SUCCESS == ret) {
        PARAM_LOG("JNI: modifyModeAllFeatures returns %d with mode id %d", ret, mode_id);
    } else {
        PARAM_LOG("JNI: modifyModeAllFeatures failed");
    }
    return ret;
}
//end mode

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1restoreConfig(JNIEnv* env, jobject this,
        jint displayId, jint feature) {
    PARAM_LOG("RestoreConfig not yet supported");
    return -1;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1getDefaultMode(JNIEnv* env, jobject this, jint displayId)
{
    PARAM_LOG("JNI: Asking Default Mode for display %d", displayId);
    int32_t defaultMode = -1;
    uint32_t flags = 0;
    int retVal = disp_api_get_default_display_mode((DISPAPI_HANDLE) ctx, (uint32_t) displayId, &defaultMode, &flags);
    PARAM_LOG("JNI: get_default_mode returns %d with id %d", retVal, defaultMode);
    return defaultMode;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1setDefaultMode(JNIEnv* env, jobject this, jint displayId, jint modeId)
{
    PARAM_LOG("JNI: Setting Default Mode for display %d and mode %d", displayId, modeId);
    uint32_t flags = 0;
    return disp_api_set_default_display_mode((DISPAPI_HANDLE) ctx, (uint32_t) displayId, (int32_t) modeId, flags);
}

//svi
jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1getRangeSVI(JNIEnv* env, jobject this, jint displayId, jint minMax)
{
    PARAM_LOG("GetRangeSVI not yet supported");
    return -1;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1setSVI(JNIEnv* env, jobject this, jint displayId, jint sviStrength)
{
    PARAM_LOG("SetSVI not yet supported");
    return -1;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1getSVI(JNIEnv* env, jobject this, jint displayId)
{
    PARAM_LOG("GetSVI not yet supported");
    return -1;
}
//end svi

//backlight
jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1getBacklightQualityLevel(JNIEnv* env, jobject this, int displayId)
{
    PARAM_LOG("GetBackLightQualityLevel not yet supported");
    return -1;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1setBacklightQualityLevel(JNIEnv* env, jobject this, int displayId, int level)
{
    PARAM_LOG("GetBackLightQualityLevel not yet supported");
    return -1;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1getAdaptiveBacklightScale(JNIEnv* env, jobject this, int displayId)
{
    PARAM_LOG("GetAdaptiveBacklightScale not yet supported");
    return -1;
}
//end backlight

//active feature
jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1isActiveFeatureOn(JNIEnv* env, jobject this, int displayId, int feature)
{
    PARAM_LOG("IsActiveFeatureOn not yet supported");
    return -1;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1setActiveFeatureControl(JNIEnv* env, jobject this, int displayId, int feature, int request)
{
    PARAM_LOG("SetActiveFeatureControl not yet supported");
    return -1;
}
//end active feature

//mem color
jintArray Java_com_qti_service_colorservice_IColorServiceImpl_native_1getRangeMemoryColorParameter(JNIEnv* env, jobject this, jint displayId, jint memColorType)
{
    PARAM_LOG("GetMemoryColorParameter not yet supported");
    return NULL;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1setMemoryColorParameters(JNIEnv* env, jobject this, int displayId, int type, int hue, int saturation, int intensity)
{
    PARAM_LOG("SetMemoryColorParameters not yet supported");
    return -1;
}

jintArray Java_com_qti_service_colorservice_IColorServiceImpl_native_1getMemoryColorParameters(JNIEnv* env, jobject this, jint displayId, jint col_type)
{
    PARAM_LOG("GetMemoryColorParameters not yet supported");
    return NULL;
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1disableMemoryColorConfiguration(JNIEnv* env, jobject this, int displayId, int type)
{
    PARAM_LOG("DisableMemoryColorConfiguration not yet supported");
    return -1;
}
//end mem color


int desiredRange[10] = { -180, 180, -50, 100, -100, 100, -100, 100, 0, 1};
float hueScaler, satScaler, valScaler, contScaler;

//pa
jintArray Java_com_qti_service_colorservice_IColorServiceImpl_native_1getRangePAParameter(JNIEnv* env, jobject this, jint displayId)
{
    PARAM_LOG("JNI: Get PA range for display %d", displayId);
    jint narr[10];

    struct disp_pa_range range;
    int retVal = disp_api_get_global_pa_range((DISPAPI_HANDLE) ctx, (uint32_t) displayId, &range);
    PARAM_LOG("JNI: get_PARange returns %d", retVal);

    if (SUCCESS == retVal) {
        int i = 0;
        narr[i++] = 0;
        narr[i++] = 360;
        narr[i++] = 0;
        narr[i++] = 150;
        narr[i++] = 0;
        narr[i++] = 200;
        narr[i++] = 0;
        narr[i++] = 200;
        narr[i++] = 0;
        narr[i++] = 1;

        hueScaler = 180/range.hue.max_val;
        satScaler = 100/range.saturation.max_val;
        valScaler = range.value.max_val/100;
        contScaler = 100/range.contrast.max_val;


        jintArray newArray = (*env)->NewIntArray(env, i);
        (*env)->SetIntArrayRegion(env, newArray, 0, i, narr);
        return newArray;
    } else {
        return NULL;
    }
}

jint Java_com_qti_service_colorservice_IColorServiceImpl_native_1setPAParameters(JNIEnv* env, jobject this, jint displayId, jint ops, jint hue, jint saturation, jint intensity, jint contrast, jint satThresh) {
    PARAM_LOG("JNI: Passing values- ops: %d, hue:%d, saturation:%d, intensity:%d, contrast:%d, thresh:%d", ops, hue,saturation,intensity,contrast,satThresh);

    struct disp_pa_config pa_cfg;
    pa_cfg.flags = ops;
    pa_cfg.data.hue = (hue + desiredRange[0])/hueScaler;
    pa_cfg.data.saturation = (saturation + desiredRange[2])/satScaler; //100.0
    pa_cfg.data.value = (intensity + desiredRange[4]) *valScaler; //2.55
    pa_cfg.data.contrast = (contrast + desiredRange[6])/contScaler; //100.0
    pa_cfg.data.sat_thresh = satThresh;
    uint32_t enable = 1;
    int retVal = disp_api_set_global_pa_config((DISPAPI_HANDLE) ctx, (uint32_t) displayId, enable, &pa_cfg);
    return retVal;
}

jintArray Java_com_qti_service_colorservice_IColorServiceImpl_native_1getPAParameters(JNIEnv* env, jobject this, jint displayId)
{
    PARAM_LOG("JNI: Getting PA Params for display id %d", displayId);

    struct disp_pa_config pa_config;
    uint32_t enable = 0;

    int retVal = disp_api_get_global_pa_config((DISPAPI_HANDLE) ctx, (uint32_t) displayId, &enable,
                                          &pa_config);
    if(retVal != 0){
        PARAM_LOG("JNI: getPA failed with return value %d", retVal);
        return NULL;
    }
    else{
        int narr[6];
        jint i=0;
        narr[i++] = pa_config.flags;
        narr[i++] = pa_config.data.hue*hueScaler - desiredRange[0];
        narr[i++] = pa_config.data.saturation*satScaler - desiredRange[2];
        narr[i++] = pa_config.data.value/valScaler - desiredRange[4];
        narr[i++] = pa_config.data.contrast*contScaler - desiredRange[6];
        narr[i++] = pa_config.data.sat_thresh;
        PARAM_LOG("JNI: getPA returned %d,%d,%f,%f,%f,%f", pa_config.flags,pa_config.data.hue,pa_config.data.saturation,pa_config.data.value,pa_config.data.contrast,pa_config.data.sat_thresh);
        jintArray newArray = (*env)->NewIntArray(env, i);
        (*env)->SetIntArrayRegion(env, newArray, 0, i, narr);
        return newArray;
    }
}
//end pa
